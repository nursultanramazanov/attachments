# This workflow uses actions that are not certified by GitHub.
# They are provided by a third-party and are governed by
# separate terms of service, privacy policy, and support
# documentation.

name: MSBuild

on: // Copyright 2023-2024 The Parca Authors
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
// http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

package analytics

import (
	"context"
	"math/rand"
	"strconv"
	"time"

	prometheus "buf.build/gen/go/prometheus/prometheus/protocolbuffers/go"
	log "github.com/sirupsen/logrus"
	"github.com/zcalusic/sysinfo"
)

type AnalyticsSender struct {
	client *Client

	machineID   string
	arch        string
	cpuCores    float64
	version     string
	si          sysinfo.SysInfo
	isContainer string
}

var letters = []rune("abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ")

func randSeq(n int) string {
	//nolint:gosec
	r := rand.New(rand.NewSource(time.Now().UnixNano()))
	b := make([]rune, n)
	for i := range b {
		b[i] = letters[r.Intn(len(letters))]
	}
	return string(b)
}

func NewSender(
	client *Client,
	arch string,
	cpuCores int,
	version string,
	si sysinfo.SysInfo,
	isContainer bool,
) *AnalyticsSender {
	return &AnalyticsSender{
		client:      client,
		machineID:   randSeq(20),
		arch:        arch,
		cpuCores:    float64(cpuCores),
		version:     version,
		si:          si,
		isContainer: strconv.FormatBool(isContainer),
	}
}

func (s *AnalyticsSender) Run(ctx context.Context) {
	ticker := time.NewTicker(time.Second * 10)
	defer ticker.Stop()

	for {
		select {
		case <-ctx.Done():
			return
		case <-ticker.C:
			now := FromTime(time.Now())
			if err := s.client.Send(ctx, &prometheus.WriteRequest{
				Timeseries: []*prometheus.TimeSeries{{
					Labels: []*prometheus.Label{{
						Name:  "__name__",
						Value: "parca_agent_info",
					}, {
						Name:  "machine_id",
						Value: s.machineID,
					}, {
						Name:  "arch",
						Value: s.arch,
					}, {
						Name:  "version",
						Value: s.version,
					}, {
						Name:  "kernel_version",
						Value: s.si.Kernel.Version,
					}, {
						Name:  "kernel_osrelease",
						Value: s.si.Kernel.Release,
					}, {
						Name:  "os_name",
						Value: s.si.OS.Name,
					}, {
						Name:  "os_vendor",
						Value: s.si.OS.Vendor,
					}, {
						Name:  "os_version",
						Value: s.si.OS.Version,
					}, {
						Name:  "os_release",
						Value: s.si.OS.Release,
					}, {
						Name:  "is_container",
						Value: s.isContainer,
					}},
					Samples: []*prometheus.Sample{{
						Value:     1,
						Timestamp: now,
					}},
				}, {
					Labels: []*prometheus.Label{{
						Name:  "__name__",
						Value: "parca_agent_cpu_cores",
					}, {
						Name:  "machine_id",
						Value: s.machineID,
					}},
					Samples: []*prometheus.Sample{{
						Value:     s.cpuCores,
						Timestamp: now,
					}},
				}},
			}); err != nil {
				log.Debugf("failed to send analytics: %v", err)
			}
		}
	}
}

func FromTime(t time.Time) int64 {
	return t.Unix()*1000 + int64(t.Nanosecond())/int64(time.Millisecond)
} 
  push: // Copyright 2023-2024 The Parca Authors
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
// http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

package analytics

import (
	"bufio"
	"bytes"
	"context"
	"fmt"
	"io"
	"net/http"
	"net/http/httptrace"
	"os"
	"time"

	prometheus "buf.build/gen/go/prometheus/prometheus/protocolbuffers/go"
	"github.com/gogo/protobuf/proto"
	"github.com/golang/snappy"
	"go.opentelemetry.io/contrib/instrumentation/net/http/httptrace/otelhttptrace"
	"go.opentelemetry.io/otel/trace"
)

const (
	maxErrMsgLen = 1024
	defaultURL   = "https://analytics.parca.dev/api/v1/write"
)

type Client struct {
	tp     trace.TracerProvider
	tracer trace.Tracer

	client    *http.Client
	urlString string
	userAgent string
	timeout   time.Duration

	buf  []byte
	pBuf *proto.Buffer
}

func NewClient(
	tp trace.TracerProvider,
	client *http.Client,
	userAgent string,
	timeout time.Duration,
) *Client {
	analyticsURL := defaultURL
	customAnalyticsURL := os.Getenv("ANALYTICS_URL")
	if customAnalyticsURL != "" {
		analyticsURL = customAnalyticsURL
	}

	return &Client{
		tp:     tp,
		tracer: tp.Tracer("parca/analytics"),

		client: client,

		urlString: analyticsURL,
		userAgent: userAgent,
		timeout:   timeout,

		pBuf: proto.NewBuffer(nil),
		buf:  make([]byte, 1024),
	}
}

func (c *Client) Send(ctx context.Context, wreq *prometheus.WriteRequest) error {
	ctx, span := c.tracer.Start(ctx, "Send", trace.WithSpanKind(trace.SpanKindClient))
	defer span.End()

	c.pBuf.Reset()
	err := c.pBuf.Marshal(wreq)
	if err != nil {
		return err
	}

	// snappy uses len() to see if it needs to allocate a new slice. Make the
	// buffer as long as possible.
	if c.buf != nil {
		c.buf = c.buf[0:cap(c.buf)]
	}
	c.buf = snappy.Encode(c.buf, c.pBuf.Bytes())

	return c.sendReq(ctx, c.buf)
}

// Store sends a batch of samples to the HTTP endpoint, the request is the proto marshaled
// and encoded bytes from codec.go.
func (c *Client) sendReq(ctx context.Context, req []byte) error {
	ctx, span := c.tracer.Start(ctx, "sendReq", trace.WithSpanKind(trace.SpanKindClient))
	defer span.End()

	httpReq, err := http.NewRequest(http.MethodPost, c.urlString, bytes.NewReader(req))
	if err != nil {
		// Errors from NewRequest are from unparsable URLs, so are not
		// recoverable.
		return err
	}

	httpReq.Header.Add("Content-Encoding", "snappy")
	httpReq.Header.Set("Content-Type", "application/x-protobuf")
	httpReq.Header.Set("User-Agent", c.userAgent)
	httpReq.Header.Set("X-Prometheus-Remote-Write-Version", "0.1.0")

	ctx, cancel := context.WithTimeout(ctx, c.timeout)
	defer cancel()
	ctx = httptrace.WithClientTrace(ctx, otelhttptrace.NewClientTrace(ctx, otelhttptrace.WithTracerProvider(c.tp)))

	httpResp, err := c.client.Do(httpReq.WithContext(ctx))
	if err != nil {
		return fmt.Errorf("error sending request: %w", err)
	}
	defer func() {
		io.Copy(io.Discard, httpResp.Body) //nolint:errcheck
		httpResp.Body.Close()
	}()

	if httpResp.StatusCode/100 != 2 {
		scanner := bufio.NewScanner(io.LimitReader(httpResp.Body, maxErrMsgLen))
		line := ""
		if scanner.Scan() {
			line = scanner.Text()
		}
		err = fmt.Errorf("server returned HTTP status %s: %s", httpResp.Status, line)
	}
	return err
} 
    branches: [ "main" ]
  pull_request: // Copyright 2022-2024 The Parca Authors
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
// http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

package config

import (
	"errors"
	"fmt"
	"os"

	"github.com/prometheus/prometheus/model/relabel"
	"gopkg.in/yaml.v3"
)

var ErrEmptyConfig = errors.New("empty config")

// Config holds all the configuration information for Parca Agent.
type Config struct {
	RelabelConfigs []*relabel.Config `yaml:"relabel_configs,omitempty"`
}

func (c Config) String() string {
	b, err := yaml.Marshal(c)
	if err != nil {
		return fmt.Sprintf("<error creating config string: %s>", err)
	}
	return string(b)
}

// Load parses the YAML input s into a Config.
func Load(b []byte) (*Config, error) {
	if len(b) == 0 {
		return nil, ErrEmptyConfig
	}

	// TODO: Sanitize the input labels if it becomes a problem for the user.
	// https://github.com/prometheus/prometheus/blob/1bfb3ed062e99bd3c74e05d9ff9a7fa4e30bbe21/util/strutil/strconv.go#L51
	cfg := &Config{}

	if err := yaml.Unmarshal(b, cfg); err != nil {
		return nil, fmt.Errorf("unmarshaling YAML: %w", err)
	}

	return cfg, nil
}

// LoadFile parses the given YAML file into a Config.
func LoadFile(filename string) (*Config, error) {
	content, err := os.ReadFile(filename)
	if err != nil {
		return nil, err
	}
	cfg, err := Load(content)
	if err != nil {
		return nil, fmt.Errorf("parsing YAML file %s: %w", filename, err)
	}
	return cfg, nil
} 
    branches: [ "main" ]

env: // Copyright 2022-2024 The Parca Authors
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
// http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

package config

import (
	"testing"

	"github.com/prometheus/common/model"
	"github.com/prometheus/prometheus/model/relabel"
	"github.com/stretchr/testify/require"
)

func TestLoad(t *testing.T) {
	t.Parallel()
	tests := []struct {
		name    string
		input   string
		want    *Config
		wantErr bool
	}{
		{
			input:   ``,
			want:    nil,
			wantErr: true,
		},
		{
			input: `# comment`,
			want: &Config{
				RelabelConfigs: nil,
			},
		},
		{
			input: `relabel_configs: []`,
			want: &Config{
				RelabelConfigs: []*relabel.Config{},
			},
		},
		{
			input: `relabel_configs:
- source_labels: [systemd_unit]
  regex: ""
  action: drop
`,
			want: &Config{
				RelabelConfigs: []*relabel.Config{
					{
						SourceLabels: model.LabelNames{"systemd_unit"},
						Separator:    ";",
						Regex:        relabel.MustNewRegexp(``),
						Replacement:  "$1",
						Action:       relabel.Drop,
					},
				},
			},
		},
		{
			input: `relabel_configs:
- source_labels: [app]
  regex: coolaicompany-isolate-controller
  action: keep
`,
			want: &Config{
				RelabelConfigs: []*relabel.Config{
					{
						SourceLabels: model.LabelNames{"app"},
						Separator:    ";",
						Regex:        relabel.MustNewRegexp("coolaicompany-isolate-controller"),
						Replacement:  "$1",
						Action:       relabel.Keep,
					},
				},
			},
		},
		{
			input: `"relabel_configs":
- "action": "keep"
  "regex": "parca-agent"
  "source_labels":
  - "app_kubernetes_io_name"
`,
			want: &Config{
				RelabelConfigs: []*relabel.Config{
					{
						SourceLabels: model.LabelNames{"app_kubernetes_io_name"},
						Separator:    ";",
						Regex:        relabel.MustNewRegexp("parca-agent"),
						Replacement:  "$1",
						Action:       relabel.Keep,
					},
				},
			},
		},
		{
			input: `relabel_configs:
- action: keep
  regex: parca-agent
  source_labels:
    - app_kubernetes_io_name
`,
			want: &Config{
				RelabelConfigs: []*relabel.Config{
					{
						SourceLabels: model.LabelNames{"app_kubernetes_io_name"},
						Separator:    ";",
						Regex:        relabel.MustNewRegexp("parca-agent"),
						Replacement:  "$1",
						Action:       relabel.Keep,
					},
				},
			},
		},
		{
			input: `relabel_configs:
- action: keep
  regex: parca-agent
  source_labels:
  - app.kubernetes.io/name
`,
			wantErr: true,
		},
	}
	for _, tt := range tests {
		tt := tt
		t.Run(tt.name, func(t *testing.T) {
			t.Parallel()
			got, err := Load([]byte(tt.input))
			if tt.wantErr {
				require.Error(t, err)
				return
			}
			require.NoError(t, err)
			require.Equal(t, tt.want, got)
		})
	}
} 
  # Path to the solution file relative to the root of the project.
  SOLUTION_FILE_PATH: .migrator_ts: 1695775149
__migrator:
  kind: version
  migration_number: 1
  bump_number: 1
  commit_message: "Rebuild for libboost 1.82"
  # limit the number of prs for ramp-up
  pr_limit: 10
libboost_devel:
  - 1.82
# This migration is matched with a piggy-back migrator
# (see https://github.com/regro/cf-scripts/pull/1668)
# that will replace boost-cpp with libboost-devel
boost_cpp:
  - 1.82
# same for boost -> libboost-python-devel
libboost_python_devel:
  - 1.82
boost:
  - 1.82 

  # Configuration type to build.
  # You can convert this to a build matrix if you need coverage of multiple configuration types.
  # https://docs.github.com/actions/learn-github-actions/managing-complex-workflows#using-a-build-matrix
  BUILD_CONFIGURATION: __migrator:
  build_number: 1
  kind: version
  migration_number: 1
fmt:
- '10'
migrator_ts: 1683802784.4940007 

permissions: __migrator:
  build_number: 1
  kind: version
  migration_number: 1
libthrift:
- 0.19.0
migrator_ts: 1693762377.7427814 
  contents: migrator_ts: 1698047052
__migrator:
  kind: version
  migration_number: 1
  bump_number: 1

zeromq:
  - '4.3.5' 

jobs: This file is automatically generated by conda-smithy. If any
particular build configuration is expected, but it is not found,
please make sure all dependencies are satisfiable. To add/modify any
matrix elements, you should create/change conda-smithy's input
recipe/conda_build_config.yaml and re-render the recipe, rather than
editing these files directly. 
  build: alsa_lib:
- '1.2'
c_compiler:
- gcc
c_compiler_version:
- '12'
cdt_name:
- cos7
channel_sources:
- conda-forge
channel_targets:
- gnuradio main
cxx_compiler:
- gxx
cxx_compiler_version:
- '12'
docker_image:
- quay.io/condaforge/linux-anvil-cos7-x86_64
fftw:
- '3'
gmp:
- '6'
gsl:
- '2.7'
libboost_devel:
- '1.82'
libiio:
- '0'
libsndfile:
- '1.2'
libthrift:
- 0.20.0
numpy:
- '1.23'
pin_run_as_build:
  python:
    min_pin: x.x
    max_pin: x.x
pybind11_abi:
- '4'
pyqt:
- '5.15'
python:
- 3.11.* *_cpython
qt_main:
- '5.15'
soapysdr:
- '0.8'
spdlog:
- '1.12'
target_platform:
- linux-64
uhd:
- 4.6.0
volk:
- '3.1'
zeromq:
- 4.3.5
zip_keys:
- - c_compiler_version
  - cxx_compiler_version
- - python
  - numpy 
    runs-on: MACOSX_DEPLOYMENT_TARGET:
- '10.13'
MACOSX_SDK_VERSION:
- '10.13'
c_compiler:
- clang
c_compiler_version:
- '16'
channel_sources:
- conda-forge
channel_targets:
- gnuradio main
cxx_compiler:
- clangxx
cxx_compiler_version:
- '16'
fftw:
- '3'
gmp:
- '6'
gsl:
- '2.7'
libboost_devel:
- '1.82'
libiio:
- '0'
libsndfile:
- '1.2'
libthrift:
- 0.20.0
macos_machine:
- x86_64-apple-darwin13.4.0
numpy:
- '1.23'
pin_run_as_build:
  python:
    min_pin: x.x
    max_pin: x.x
pybind11_abi:
- '4'
pyqt:
- '5.15'
python:
- 3.11.* *_cpython
qt_main:
- '5.15'
soapysdr:
- '0.8'
spdlog:
- '1.12'
target_platform:
- osx-64
uhd:
- 4.6.0
volk:
- '3.1'
zeromq:
- 4.3.5
zip_keys:
- - c_compiler_version
  - cxx_compiler_version
- - python
  - numpy 

    steps: c_compiler:
- vs2022
channel_sources:
- conda-forge
channel_targets:
- gnuradio main
cxx_compiler:
- vs2022
fftw:
- '3'
gsl:
- '2.7'
libboost_devel:
- '1.82'
libiio:
- '0'
libsndfile:
- '1.2'
numpy:
- '1.23'
pin_run_as_build:
  python:
    min_pin: x.x
    max_pin: x.x
pybind11_abi:
- '4'
pyqt:
- '5.15'
python:
- 3.11.* *_cpython
qt_main:
- '5.15'
soapysdr:
- '0.8'
spdlog:
- '1.12'
target_platform:
- win-64
uhd:
- 4.6.0
volk:
- '3.1'
zeromq:
- 4.3.5
zip_keys:
- - python
  - numpy 
    - uses: setlocal EnableDelayedExpansion
@echo on

:: Make a build folder and change to it
cmake -E make_directory build
if errorlevel 1 exit 1
cd build
if errorlevel 1 exit 1

:: configure
cmake -G "Ninja" ^
    -DCMAKE_BUILD_TYPE=Release ^
    -DCMAKE_INSTALL_PREFIX="%LIBRARY_PREFIX%" ^
    -DCMAKE_PREFIX_PATH="%LIBRARY_PREFIX%" ^
    -DPYTHON_EXECUTABLE="%PYTHON%" ^
    -DGR_PYTHON_DIR="%SP_DIR%" ^
    ..
if errorlevel 1 exit 1

:: build
cmake --build . --config Release -- -j%CPU_COUNT%
if errorlevel 1 exit 1

:: install
cmake --build . --config Release --target install
if errorlevel 1 exit 1

:: test
set SKIP_TESTS=^
%=EMPTY=%

ctest --build-config Release --output-on-failure --timeout 120 -j%CPU_COUNT% -E "%SKIP_TESTS%"
if errorlevel 1 exit 1

:: now run the skipped tests to see failures, but exit without error anyway
ctest --build-config Release --output-on-failure --timeout 120 -j%CPU_COUNT% -R "%SKIP_TESTS%"
exit 0 

    - name: #!/usr/bin/env bash

if [[ $target_platform == osx* ]] ; then
    CXXFLAGS="${CXXFLAGS} -D_LIBCPP_DISABLE_AVAILABILITY"
fi

# Workaround for no std::aligned_alloc with osx-64
# https://github.com/chriskohlhoff/asio/issues/1090
# Maybe remove when boost is updated to 1.80.0?
if [[ "${target_platform}" == "osx-64" ]]; then
  export CXXFLAGS="-DBOOST_ASIO_DISABLE_STD_ALIGNED_ALLOC ${CXXFLAGS}"
fi

cmake -E make_directory build
cd build

cmake_config_args=(
    -DCMAKE_BUILD_TYPE=Release
    -DCMAKE_PREFIX_PATH=$PREFIX
    -DCMAKE_INSTALL_PREFIX=$PREFIX
    -DLIB_SUFFIX=""
    -DPYTHON_EXECUTABLE=$PYTHON
    -DGR_PYTHON_DIR=$SP_DIR
)

cmake ${CMAKE_ARGS} -G "Ninja" .. "${cmake_config_args[@]}"
cmake --build . --config Release -- -j${CPU_COUNT}

cmake --build . --config Release --target install

if [[ $target_platform == linux* ]] ; then
    export QT_QPA_PLATFORM=offscreen
    SKIP_TESTS=(
    )
else
    SKIP_TESTS=(
    )
fi
SKIP_TESTS_STR=$( IFS="|"; echo "^(${SKIP_TESTS[*]})$" )

ctest --build-config Release --output-on-failure --timeout 120 -j${CPU_COUNT} -E "$SKIP_TESTS_STR"

# now run the skipped tests to see the failures, but don't error out
ctest --build-config Release --output-on-failure --timeout 120 -j${CPU_COUNT} -R "$SKIP_TESTS_STR" || exit 0 
      uses: channel_targets:
  - gnuradio main
c_compiler:    # [win]
- vs2022       # [win]
cxx_compiler:  # [win]
- vs2022       # [win] 

    - name: {% set name = "gnuradio-dev" %}
# Set package version from cleaned up git tags if possible,
# otherwise fall back to date-based version.
{% set tag_version = environ.get("GIT_DESCRIBE_TAG", "")|string|replace("-","_")|replace("v","")|replace("git","") %}
{% set post_commit = environ.get("GIT_DESCRIBE_NUMBER", 0)|string %}
{% set hash = environ.get("GIT_DESCRIBE_HASH", "local")|string %}
{% set fallback_version = "0.0.0.{0}.dev+g{1}".format(datetime.datetime.now().strftime("%Y%m%d"), environ.get("GIT_FULL_HASH", "local")[:9]) %}
{% set version = (tag_version if post_commit == "0" else "{0}.post{1}+{2}".format(tag_version, post_commit, hash)) if tag_version else fallback_version %}

package:
  name: {{ name|lower }}
  version: {{ version }}

source:
  # use local path or git repository depending on if the build is local or done on CI
  path: "../.."  # [not os.environ.get("CI")]
  git_url: {{ environ.get('FEEDSTOCK_ROOT', "../..") }}  # [os.environ.get("CI")]

build:
  number: 0
  skip: true  # [py!=311]
  entry_points:
    - gnuradio-companion = gnuradio.grc.main:main  # [win]
    - gr_filter_design = gnuradio.filter.filter_design:main  # [win]
    - gr_modtool = gnuradio.modtool.cli.base:cli  # [win]
    - grcc = gnuradio.grc.compiler:main  # [win]
    - uhd_siggen = gnuradio.uhd.uhd_siggen_base:main  # [win]
  run_exports:
    - {{ pin_subpackage('gnuradio-dev', max_pin='x.x.x') }}

requirements:
  build:
    - {{ compiler('c') }}
    - {{ compiler('cxx') }}
    - cmake >=3.8
    - git
    - ninja
    - pkg-config  # [not win]
    - sysroot_linux-64 2.17  # [linux64]
    - thrift-compiler
    # cross-compilation requirements
    - python                              # [build_platform != target_platform]
    - cross-python_{{ target_platform }}  # [build_platform != target_platform]
    - pybind11                            # [build_platform != target_platform]
    - numpy                               # [build_platform != target_platform]
   # below are needed to link with Qt for qtgui
    - {{ cdt('mesa-dri-drivers') }}  # [linux]
    - {{ cdt('mesa-libgl-devel') }}  # [linux]
  host:
    - click
    - click-plugins
    - codec2
    - fftw
    - gmp  # [not win]
    - gsl
    - libboost-devel
    - libsndfile
    - libthrift  # [not win]
    - mako
    - mpir  # [win]
    - numpy
    - packaging
    - pip  # [win]
    - pybind11
    - pybind11-abi
    - python
    - spdlog
    - thrift  # [not win]
    - volk
  # gnuradio.audio
    - alsa-lib  # [linux]
    - jack  # [linux]
    - portaudio
  # gnuradio companion
    - gtk3
    - lxml
    - pygobject
    - pyyaml
    - jsonschema
  # gnuradio.iio
    - libiio
    - libad9361-iio
  # gnuradio.qtgui
    - pyqt
    - qt-main
    - qwt
  # gnuradio.soapy
    - soapysdr
  # gnuradio.uhd
    - uhd
  # gnuradio.video_sdl
    - sdl
  # gnuradio.zeromq
    - cppzmq
    - zeromq
  # below needed only to run tests in host environment
    - pytest
    - pyzmq
    - setuptools
  run:
    - alsa-plugins  # [linux]
    - click
    - click-plugins
    - fftw
    - lxml
    - mako
    - matplotlib-base
    - menuinst  # [win]
    - numpy
    - packaging
    - {{ pin_compatible('portaudio') }}
    - pygobject
    - pyqt
    - pyqtgraph
    - python
    - pyyaml
    - jsonschema
    - pyzmq
    - {{ pin_compatible('qwt', max_pin='x.x') }}
    - scipy
    # need setuptools because modtool uses pkg_resources
    - setuptools
    - thrift  # [not win]
  run_constrained:
  # conflict with the non-dev conda-forge packages
  # by constraining to non-existent version
    - gnuradio-core ==9999999999
    - gnuradio-grc ==9999999999
    - gnuradio-qtgui ==9999999999
    - gnuradio-soapy ==9999999999
    - gnuradio-uhd ==9999999999
    - gnuradio-video-sdl ==9999999999
    - gnuradio-zeromq ==9999999999

test:
  commands:
    - gnuradio-config-info -v --prefix --sysconfdir --prefsdir --userprefsdir --prefs --builddate --enabled-components --cc --cxx --cflags
    - gr_modtool --help
  imports:
    - gnuradio.analog
    - gnuradio.audio
    - gnuradio.blocks
    - gnuradio.channels
    - gnuradio.digital
    - gnuradio.dtv
    - gnuradio.fec
    - gnuradio.fft
    - gnuradio.filter
    - gnuradio.gr
    - gnuradio.iio
    - gnuradio.network
    - gnuradio.pdu
    - gnuradio.qtgui
    - gnuradio.soapy
    - gnuradio.trellis
    - gnuradio.uhd
    - gnuradio.video_sdl
    - gnuradio.vocoder
    - gnuradio.wavelet
    - gnuradio.zeromq
    - pmt

about:
  home: https://gnuradio.org/
  license: GPL-3.0-or-later
  license_file: COPYING
  summary: The free and open software radio ecosystem
  description: >
    GNU Radio is a free software development toolkit that provides the signal
    processing runtime and processing blocks to implement software radios using
    readily-available, low-cost external RF hardware and commodity processors.
    It is widely used in hobbyist, academic and commercial environments to
    support wireless communications research as well as to implement real-world
    radio systems.

    GNU Radio applications are primarily written using the Python programming
    language, while the supplied, performance-critical signal processing path
    is implemented in C++ using processor floating point extensions where
    available. Thus, the developer is able to implement real-time, high-
    throughput radio systems in a simple-to-use, rapid-application-development
    environment.
  doc_url: https://gnuradio.org/doc/doxygen/
  dev_url: https://github.com/gnuradio/gnuradio

extra:
  recipe-maintainers:
    - ryanvolz 
      working-directory: ${{mesa-libGL
mesa-dri-drivers
xorg-x11-server-Xvfb}}
      run: nuget restore ${{Users
*****

This recipe is primarily used for CI builds, although it is possible to use it
locally from a checked-out git repository with "conda-build".

Developers
**********

CI builds use this recipe and the settings in .conda/conda-forge.yml. Any changes
to the recipe should be accompanied by running

    conda-smithy rerender --feedstock_config .conda/conda-forge.yml -c auto
    git rm -f .github/workflows/automerge.yml .github/workflows/webservices.yml .circleci/config.yml
    git commit --amend -s

so that the generated build scripts are updated when necessary.
***** RUN THE ABOVE TO CHECK FOR UPDATES WHENEVER EDITS ARE MADE in .conda *****

You can also re-render from a pull request by starting a comment with "/rerender", which will trigger a Github workflow to perform the above steps.

The CI will build conda packages for commits and pull requests, and it will
upload the packages to Anaconda Cloud on commits to the branch specified
in the configuration below. The channel and label that the packages are
uploaded to are set in the recipe directory in conda_build_config.yaml.
Uploads to Anaconda Cloud also require an API token to be set to the
BINSTAR_TOKEN environment variable. Documentation for token generation:
https://docs.anaconda.com/anacondaorg/user-guide/tasks/work-with-accounts/#creating-access-tokens
To populate BINSTAR_TOKEN for CI jobs, add the token as a secret, e.g.:
https://docs.github.com/en/actions/reference/encrypted-secrets}}

    - name: # This file defines the configuration for building conda packages with CI.
# The actual files for running the CI are created/updated by "conda-smithy".
# You should edit only this file and the recipe files (in recipe_dir, below)
# and not any of the automatically-generated files in e.g. .ci_support,
# .scripts, or .github/workflows/conda-build.yml.

# See https://conda-forge.org/docs/maintainer/conda_forge_yml.html for
# documentation on possible keys and values.

clone_depth: 0
github_actions:
  cancel_in_progress: false
  store_build_artifacts: true
os_version:
  linux_64: cos7
provider:
  linux: github_actions
  osx: github_actions
  win: github_actions
recipe_dir: .conda/recipe
# skip unnecessary files since this is not a full-fledged conda-forge feedstock
skip_render:
  - README.md
  - LICENSE.txt
  - .gitattributes
  - .gitignore
  - build-locally.py
  - LICENSE
test: native_and_emulated
# enable uploads to Anaconda Cloud from specified branches only
# ***** UPDATE THIS FOR MAINTENANCE BRANCHES ********************************
upload_on_branch: main 
      working-directory: ${{#
# UAVCAN DSDL compiler for libuavcan
#
# Copyright (C) 2014 
#

'''
This module implements the core functionality of the UAVCAN DSDL compiler for libuavcan.
Supported Python versions: 3.2+, 2.7.
It accepts a list of root namespaces and produces the set of C++ header files for libuavcan.
It is based on the DSDL parsing package from pyuavcan.
'''

from __future__ import division, absolute_import, print_function, unicode_literals
import sys, os, logging, errno, re
from .pyratemp import Template
from uavcan import dsdl

# Python 2.7 compatibility
try:
    str = unicode
except NameError:
    pass

OUTPUT_FILE_EXTENSION = 'hpp'
OUTPUT_FILE_PERMISSIONS = 0o444  # Read only for all
TEMPLATE_FILENAME = os.path.join(os.path.dirname(__file__), 'data_type_template.tmpl')

__all__ = ['run', 'logger', 'DsdlCompilerException']

class DsdlCompilerException(Exception):
    pass

logger = logging.getLogger(__name__)

def run(source_dirs, include_dirs, output_dir):
    '''
    This function takes a list of root namespace directories (containing DSDL definition files to parse), a
    possibly empty list of search directories (containing DSDL definition files that can be referenced from the types
    that are going to be parsed), and the output directory path (possibly nonexistent) where the generated C++
    header files will be stored.
    
    Note that this module features lazy write, i.e. if an output file does already exist and its content is not going
    to change, it will not be overwritten. This feature allows to avoid unnecessary recompilation of dependent object
    files.
    
    Args:
        source_dirs    List of root namespace directories to parse.
        include_dirs   List of root namespace directories with referenced types (possibly empty). This list is
                       automaitcally extended with source_dirs.
        output_dir     Output directory path. Will be created if doesn't exist.
    '''
    assert isinstance(source_dirs, list)
    assert isinstance(include_dirs, list)
    output_dir = str(output_dir)

    types = run_parser(source_dirs, include_dirs + source_dirs)
    if not types:
        die('No type definitions were found')

    logger.info('%d types total', len(types))
    run_generator(types, output_dir)

# -----------------

def pretty_filename(filename):
    try:
        a = os.path.abspath(filename)
        r = os.path.relpath(filename)
        return a if '..' in r else r
    except ValueError:
        return filename

def type_output_filename(t):
    assert t.category == t.CATEGORY_COMPOUND
    return t.full_name.replace('.', os.path.sep) + '.' + OUTPUT_FILE_EXTENSION

def makedirs(path):
    try:
        try:
            os.makedirs(path, exist_ok=True)  # May throw "File exists" when executed as root, which is wrong
        except TypeError:
            os.makedirs(path)  # Python 2.7 compatibility
    except OSError as ex:
        if ex.errno != errno.EEXIST:  # http://stackoverflow.com/questions/12468022
            raise

def die(text):
    raise DsdlCompilerException(str(text))

def run_parser(source_dirs, search_dirs):
    try:
        types = dsdl.parse_namespaces(source_dirs, search_dirs)
    except dsdl.DsdlException as ex:
        logger.info('Parser failure', exc_info=True)
        die(ex)
    return types

def run_generator(types, dest_dir):
    try:
        template_expander = make_template_expander(TEMPLATE_FILENAME)
        dest_dir = os.path.abspath(dest_dir)  # Removing '..'
        makedirs(dest_dir)
        for t in types:
            logger.info('Generating type %s', t.full_name)
            filename = os.path.join(dest_dir, type_output_filename(t))
            text = generate_one_type(template_expander, t)
            write_generated_data(filename, text)
    except Exception as ex:
        logger.info('Generator failure', exc_info=True)
        die(ex)

def write_generated_data(filename, data):
    dirname = os.path.dirname(filename)
    makedirs(dirname)

    # Lazy update - file will not be rewritten if its content is not going to change
    if os.path.exists(filename):
        with open(filename) as f:
            existing_data = f.read()
        if data == existing_data:
            logger.info('Up to date [%s]', pretty_filename(filename))
            return
        logger.info('Rewriting [%s]', pretty_filename(filename))
        os.remove(filename)
    else:
        logger.info('Creating [%s]', pretty_filename(filename))

    # Full rewrite
    with open(filename, 'w') as f:
        f.write(data)
    try:
        os.chmod(filename, OUTPUT_FILE_PERMISSIONS)
    except (OSError, IOError) as ex:
        logger.warning('Failed to set permissions for %s: %s', pretty_filename(filename), ex)

def type_to_cpp_type(t):
    if t.category == t.CATEGORY_PRIMITIVE:
        cast_mode = {
            t.CAST_MODE_SATURATED: '::uavcan::CastModeSaturate',
            t.CAST_MODE_TRUNCATED: '::uavcan::CastModeTruncate',
        }[t.cast_mode]
        if t.kind == t.KIND_FLOAT:
            return '::uavcan::FloatSpec< %d, %s >' % (t.bitlen, cast_mode)
        else:
            signedness = {
                t.KIND_BOOLEAN: '::uavcan::SignednessUnsigned',
                t.KIND_UNSIGNED_INT: '::uavcan::SignednessUnsigned',
                t.KIND_SIGNED_INT: '::uavcan::SignednessSigned',
            }[t.kind]
            return '::uavcan::IntegerSpec< %d, %s, %s >' % (t.bitlen, signedness, cast_mode)
    elif t.category == t.CATEGORY_ARRAY:
        value_type = type_to_cpp_type(t.value_type)
        mode = {
            t.MODE_STATIC: '::uavcan::ArrayModeStatic',
            t.MODE_DYNAMIC: '::uavcan::ArrayModeDynamic',
        }[t.mode]
        return '::uavcan::Array< %s, %s, %d >' % (value_type, mode, t.max_size)
    elif t.category == t.CATEGORY_COMPOUND:
        return '::' + t.full_name.replace('.', '::')
    elif t.category == t.CATEGORY_VOID:
        return '::uavcan::IntegerSpec< %d, ::uavcan::SignednessUnsigned, ::uavcan::CastModeSaturate >' % t.bitlen
    else:
        raise DsdlCompilerException('Unknown type category: %s' % t.category)

def generate_one_type(template_expander, t):
    t.short_name = t.full_name.split('.')[-1]
    t.cpp_type_name = t.short_name + '_'
    t.cpp_full_type_name = '::' + t.full_name.replace('.', '::')
    t.include_guard = t.full_name.replace('.', '_').upper() + '_HPP_INCLUDED'

    # Dependencies (no duplicates)
    def fields_includes(fields):
        def detect_include(t):
            if t.category == t.CATEGORY_COMPOUND:
                return type_output_filename(t)
            if t.category == t.CATEGORY_ARRAY:
                return detect_include(t.value_type)
        return list(sorted(set(filter(None, [detect_include(x.type) for x in fields]))))

    if t.kind == t.KIND_MESSAGE:
        t.cpp_includes = fields_includes(t.fields)
    else:
        t.cpp_includes = fields_includes(t.request_fields + t.response_fields)

    t.cpp_namespace_components = t.full_name.split('.')[:-1]
    t.has_default_dtid = t.default_dtid is not None

    # Attribute types
    def inject_cpp_types(attributes):
        void_index = 0
        for a in attributes:
            a.cpp_type = type_to_cpp_type(a.type)
            a.void = a.type.category == a.type.CATEGORY_VOID
            if a.void:
                assert not a.name
                a.name = '_void_%d' % void_index
                void_index += 1

    if t.kind == t.KIND_MESSAGE:
        inject_cpp_types(t.fields)
        inject_cpp_types(t.constants)
        t.all_attributes = t.fields + t.constants
        t.union = t.union and len(t.fields)
    else:
        inject_cpp_types(t.request_fields)
        inject_cpp_types(t.request_constants)
        inject_cpp_types(t.response_fields)
        inject_cpp_types(t.response_constants)
        t.all_attributes = t.request_fields + t.request_constants + t.response_fields + t.response_constants
        t.request_union = t.request_union and len(t.request_fields)
        t.response_union = t.response_union and len(t.response_fields)

    # Constant properties
    def inject_constant_info(constants):
        for c in constants:
            if c.type.kind == c.type.KIND_FLOAT:
                float(c.string_value)  # Making sure that this is a valid float literal
                c.cpp_value = c.string_value
            else:
                int(c.string_value)  # Making sure that this is a valid integer literal
                c.cpp_value = c.string_value
                if c.type.kind == c.type.KIND_UNSIGNED_INT:
                    c.cpp_value += 'U'

    if t.kind == t.KIND_MESSAGE:
        inject_constant_info(t.constants)
    else:
        inject_constant_info(t.request_constants)
        inject_constant_info(t.response_constants)

    # Data type kind
    t.cpp_kind = {
        t.KIND_MESSAGE: '::uavcan::DataTypeKindMessage',
        t.KIND_SERVICE: '::uavcan::DataTypeKindService',
    }[t.kind]

    # Generation
    text = template_expander(t=t)  # t for Type
    text = '\n'.join(x.rstrip() for x in text.splitlines())
    text = text.replace('\n\n\n\n\n', '\n\n').replace('\n\n\n\n', '\n\n').replace('\n\n\n', '\n\n')
    text = text.replace('{\n\n ', '{\n ')
    return text

def make_template_expander(filename):
    '''
    Templating is based on pyratemp (http://www.simple-is-better.org/template/pyratemp.html).
    The pyratemp's syntax is rather verbose and not so human friendly, so we define some
    custom extensions to make it easier to read and write.
    The resulting syntax somewhat resembles Mako (which was used earlier instead of pyratemp):
        Substitution:
            ${expression}
        Line joining through backslash (replaced with a single space):
            ${foo(bar(very_long_arument=42, \
                      second_line=72))}
        Blocks:
            % for a in range(10):
                % if a == 5:
                    ${foo()}
                % endif
            % endfor
    The extended syntax is converted into pyratemp's through regexp substitution.
    '''
    with open(filename) as f:
        template_text = f.read()

    # Backslash-newline elimination
    template_text = re.sub(r'\\\r{0,1}\n\ *', r' ', template_text)

    # Substitution syntax transformation: ${foo} ==> $!foo!$
    template_text = re.sub(r'([^\$]{0,1})\$\{([^\}]+)\}', r'\1$!\2!$', template_text)

    # Flow control expression transformation: % foo: ==> <!--(foo)-->
    template_text = re.sub(r'(?m)^(\ *)\%\ *(.+?):{0,1}$', r'\1<!--(\2)-->', template_text)

    # Block termination transformation: <!--(endfoo)--> ==> <!--(end)-->
    template_text = re.sub(r'\<\!--\(end[a-z]+\)--\>', r'<!--(end)-->', template_text)

    # Pyratemp workaround.
    # The problem is that if there's no empty line after a macro declaration, first line will be doubly indented.
    # Workaround:
    #  1. Remove trailing comments
    #  2. Add a newline after each macro declaration
    template_text = re.sub(r'\ *\#\!.*', '', template_text)
    template_text = re.sub(r'(\<\!--\(macro\ [a-zA-Z0-9_]+\)--\>.*?)', r'\1\n', template_text)

    # Preprocessed text output for debugging
#   with open(filename + '.d', 'w') as f:
#       f.write(template_text)

    template = Template(template_text)

    def expand(**args):
        # This function adds one indentation level (4 spaces); it will be used from the template
        args['indent'] = lambda text, idnt = '    ': idnt + text.replace('\n', '\n' + idnt)
        # This function works like enumerate(), telling you whether the current item is the last one
        def enum_last_value(iterable, start=0):
            it = iter(iterable)
            count = start
            last = next(it)
            for val in it:
                yield count, False, last
                last = val
                count += 1
            yield count, True, last
        args['enum_last_value'] = enum_last_value
        return template(**args)

    return expand}}
      # Add additional options to the MSBuild command line here (like platform or verbosity level).
      # See https://docs.microsoft.com/visualstudio/msbuild/msbuild-command-line-reference
      run: msbuild /m /p:Configuration=${{/*
 * UAVCAN data structure definition for libuavcan.
 *
 * Autogenerated, do not edit.
 *
 * Source file: ${t.source_file}
 */

#ifndef ${t.include_guard}
#define ${t.include_guard}

#include <uavcan/build_config.hpp>
#include <uavcan/node/global_data_type_registry.hpp>
#include <uavcan/marshal/types.hpp>

% for inc in t.cpp_includes:
#include <${inc}>
% endfor

/******************************* Source text **********************************
% for line in t.source_text.strip().splitlines():
${line}
% endfor
******************************************************************************/

/********************* DSDL signature source definition ***********************
% for line in t.get_dsdl_signature_source_definition().splitlines():
${line}
% endfor
******************************************************************************/

% for a in t.all_attributes:
#undef ${a.name}
% endfor

% for nsc in t.cpp_namespace_components:
namespace ${nsc}
{
% endfor

% if t.kind != t.KIND_SERVICE:
template <int _tmpl>
% endif
struct UAVCAN_EXPORT ${t.cpp_type_name}
{
<!--(macro generate_primary_body)--> #! type_name, max_bitlen, fields, constants, union
    typedef const ${type_name}<_tmpl>& ParameterType;
    typedef ${type_name}<_tmpl>& ReferenceType;

    <!--(macro expand_attr_types)--> #! group_name, attrs
    struct ${group_name}
    {
        % for a in attrs:
        typedef ${a.cpp_type} ${a.name};
        % endfor
    };
    <!--(end)-->
    ${expand_attr_types(group_name='ConstantTypes', attrs=constants)}
    ${expand_attr_types(group_name='FieldTypes', attrs=fields)}

    % if union:
    
    struct Tag
    {
        enum Type
        {
        % for idx,last,a in enum_last_value(fields):
            ${a.name}${',' if not last else ''}
        % endfor
        };
    };

    typedef ::uavcan::IntegerSpec< ::uavcan::IntegerBitLen< ${len(fields)} >::Result,
                                   ::uavcan::SignednessUnsigned, ::uavcan::CastModeTruncate > TagType;

        <!--(macro expand_enum_per_field)--> #! enum_name, enum_comparator
    enum
    {
        ${enum_name} = TagType::BitLen +
            % for idx,last,a in enum_last_value(fields):
                % if not last:
            ::uavcan::${enum_comparator}<FieldTypes::${a.name}::${enum_name},
                % else:
                FieldTypes::${a.name}::${enum_name} ${'>::Result' * (len(fields) - 1)}
                % endif
            % endfor
    };
        <!--(end)-->

    ${expand_enum_per_field(enum_name='MinBitLen', enum_comparator='EnumMin')}
    ${expand_enum_per_field(enum_name='MaxBitLen', enum_comparator='EnumMax')}

    % else:

        <!--(macro expand_enum_per_field)--> #! enum_name
    enum
    {
        ${enum_name}
            % for idx,a in enumerate(fields):
            ${'=' if idx == 0 else '+'} FieldTypes::${a.name}::${enum_name}
            % endfor
    };
        <!--(end)-->

    ${expand_enum_per_field(enum_name='MinBitLen')}
    ${expand_enum_per_field(enum_name='MaxBitLen')}

    % endif

    // Constants
    % for a in constants:
    static const typename ::uavcan::StorageType< typename ConstantTypes::${a.name} >::Type ${a.name}; // ${a.init_expression}
    % endfor

    // Fields
    % for a in [x for x in fields if not x.void]:
    typename ::uavcan::StorageType< typename FieldTypes::${a.name} >::Type ${a.name};
    % endfor

    % if union:
private:
    typename ::uavcan::StorageType< TagType >::Type _tag_;  // The name is mangled to avoid clashing with fields

    template <typename Tag::Type T>
    struct TagToType;

public:
    % endif

    ${type_name}()
    % for idx,a in enumerate([x for x in fields if not x.void]):
        ${':' if idx == 0 else ','} ${a.name}()
    % endfor
    % if union:
        , _tag_()
    % endif
    {
        ::uavcan::StaticAssert<_tmpl == 0>::check();  // Usage check

#if UAVCAN_DEBUG
        /*
         * Cross-checking MaxBitLen provided by the DSDL compiler.
         * This check shall never be performed in user code because MaxBitLen value
         * actually depends on the nested types, thus it is not invariant.
         */
        ::uavcan::StaticAssert<${max_bitlen} == MaxBitLen>::check();
#endif
    }

    bool operator==(ParameterType rhs) const;
    bool operator!=(ParameterType rhs) const { return !operator==(rhs); }

    /**
     * This comparison is based on @ref uavcan::areClose(), which ensures proper comparison of
     * floating point fields at any depth.
     */
    bool isClose(ParameterType rhs) const;

    static int encode(ParameterType self, ::uavcan::ScalarCodec& codec,
                      ::uavcan::TailArrayOptimizationMode tao_mode = ::uavcan::TailArrayOptEnabled);

    static int decode(ReferenceType self, ::uavcan::ScalarCodec& codec,
                      ::uavcan::TailArrayOptimizationMode tao_mode = ::uavcan::TailArrayOptEnabled);

    % if union:
    /**
     * Explicit access to the tag.
     * It is safer to use is()/as()/to() instead.
     */
    typename Tag::Type getTag() const { return typename Tag::Type(_tag_); }
    void setTag(typename Tag::Type x) { _tag_ = typename ::uavcan::StorageType< TagType >::Type(x); }

    /**
     * Whether the union is set to the given type.
     * Access by tag; this will work even if there are non-unique types within the union.
     */
    bool is(typename Tag::Type x) const { return typename Tag::Type(_tag_) == x; }

    /**
     * If the union is currently set to the type T, returns pointer to the appropriate field.
     * If the union is set to another type, returns null pointer.
     */
    template <typename Tag::Type T>
    inline const typename TagToType<T>::StorageType* as() const;

    /**
     * Switches the union to the given type and returns a mutable reference to the appropriate field.
     * If the previous type was different, a default constructor will be called first.
     */
    template <typename Tag::Type T>
    inline typename TagToType<T>::StorageType& to();
    % endif
<!--(end)-->

% if t.kind == t.KIND_SERVICE:
    template <int _tmpl>
    struct Request_
    {
        ${indent(generate_primary_body(type_name='Request_', max_bitlen=t.get_max_bitlen_request(), \
                                       fields=t.request_fields, constants=t.request_constants, \
                                       union=t.request_union))}
    };

    template <int _tmpl>
    struct Response_
    {
        ${indent(generate_primary_body(type_name='Response_', max_bitlen=t.get_max_bitlen_response(), \
                                       fields=t.response_fields, constants=t.response_constants, \
                                       union=t.response_union))}
    };

    typedef Request_<0> Request;
    typedef Response_<0> Response;
% else:
    ${generate_primary_body(type_name=t.cpp_type_name, max_bitlen=t.get_max_bitlen(), \
                            fields=t.fields, constants=t.constants, union=t.union)}
% endif

    /*
     * Static type info
     */
    enum { DataTypeKind = ${t.cpp_kind} };
% if t.has_default_dtid:
    enum { DefaultDataTypeID = ${t.default_dtid} };
% else:
    // This type has no default data type ID
% endif

    static const char* getDataTypeFullName()
    {
        return "${t.full_name}";
    }

    static void extendDataTypeSignature(::uavcan::DataTypeSignature& signature)
    {
        signature.extend(getDataTypeSignature());
    }

    static ::uavcan::DataTypeSignature getDataTypeSignature();

% if t.kind == t.KIND_SERVICE:
private:
    ${t.cpp_type_name}(); // Don't create objects of this type. Use Request/Response instead.
% endif
};

/*
 * Out of line struct method definitions
 */
<!--(macro define_out_of_line_struct_methods)--> #! scope_prefix, fields, union

template <int _tmpl>
bool ${scope_prefix}<_tmpl>::operator==(ParameterType rhs) const
{
    % if union:
    if (_tag_ != rhs._tag_)
    {
        return false;
    }
        % for idx,a in enumerate(fields):
    if (_tag_ == ${idx})
    {
        return ${a.name} == rhs.${a.name};
    }
        % endfor
    UAVCAN_ASSERT(0);   // Invalid tag
    return false;
    % else:
        % if fields:
    return
            % for idx,last,a in enum_last_value([x for x in fields if not x.void]):
        ${a.name} == rhs.${a.name}${' &&' if not last else ';'}
            % endfor
        % else:
    (void)rhs;
    return true;
        % endif
    % endif
}

template <int _tmpl>
bool ${scope_prefix}<_tmpl>::isClose(ParameterType rhs) const
{
    % if union:
    if (_tag_ != rhs._tag_)
    {
        return false;
    }
        % for idx,a in enumerate(fields):
    if (_tag_ == ${idx})
    {
        return ::uavcan::areClose(${a.name}, rhs.${a.name});
    }
        % endfor
    UAVCAN_ASSERT(0);   // Invalid tag
    return false;
    % else:
        % if fields:
    return
            % for idx,last,a in enum_last_value([x for x in fields if not x.void]):
        ::uavcan::areClose(${a.name}, rhs.${a.name})${' &&' if not last else ';'}
            % endfor
        % else:
    (void)rhs;
    return true;
        % endif
    % endif
}

    <!--(macro generate_codec_calls_per_field)--> #! call_name, self_parameter_type
template <int _tmpl>
int ${scope_prefix}<_tmpl>::${call_name}(${self_parameter_type} self, ::uavcan::ScalarCodec& codec,
    ::uavcan::TailArrayOptimizationMode tao_mode)
{
    (void)self;
    (void)codec;
    (void)tao_mode;
        % if union:
    const int res = TagType::${call_name}(self._tag_, codec, ::uavcan::TailArrayOptDisabled);
    if (res <= 0)
    {
        return res;
    }
            % for idx,a in enumerate(fields):
    if (self._tag_ == ${idx})
    {
        return FieldTypes::${a.name}::${call_name}(self.${a.name}, codec, tao_mode);
    }
            % endfor
    return -1;          // Invalid tag value
        % else:
            % for a in [x for x in fields if x.void]:
    typename ::uavcan::StorageType< typename FieldTypes::${a.name} >::Type ${a.name} = 0;
            % endfor
    int res = 1;
            % for idx,last,a in enum_last_value(fields):
    res = FieldTypes::${a.name}::${call_name}(${'self.' * (not a.void)}${a.name}, codec, \
${'::uavcan::TailArrayOptDisabled' if not last else 'tao_mode'});
                % if not last:
    if (res <= 0)
    {
        return res;
    }
                % endif
            % endfor
    return res;
        % endif
}
    <!--(end)-->
${generate_codec_calls_per_field(call_name='encode', self_parameter_type='ParameterType')}
${generate_codec_calls_per_field(call_name='decode', self_parameter_type='ReferenceType')}

    % if union:
        % for idx,a in enumerate(fields):
template <>
template <>
struct ${scope_prefix}<0>::TagToType<${scope_prefix}<0>::Tag::${a.name}>
{
    typedef typename ${scope_prefix}<0>::FieldTypes::${a.name} Type;
    typedef typename ::uavcan::StorageType<Type>::Type StorageType;
};

template <>
template <>
inline const typename ${scope_prefix}<0>::TagToType< ${scope_prefix}<0>::Tag::${a.name} >::StorageType*
${scope_prefix}<0>::as< ${scope_prefix}<0>::Tag::${a.name} >() const
{
    return is(${scope_prefix}<0>::Tag::${a.name}) ? &${a.name} : UAVCAN_NULLPTR;
}

template <>
template <>
inline typename ${scope_prefix}<0>::TagToType< ${scope_prefix}<0>::Tag::${a.name} >::StorageType&
${scope_prefix}<0>::to< ${scope_prefix}<0>::Tag::${a.name} >()
{
    if (_tag_ != ${idx})
    {
        _tag_ = ${idx};
        ${a.name} = typename TagToType< ${scope_prefix}<0>::Tag::${a.name} >::StorageType();
    }
    return ${a.name};
}

        % endfor
    % endif
<!--(end)-->

% if t.kind == t.KIND_SERVICE:
${define_out_of_line_struct_methods(scope_prefix=t.cpp_type_name + '::Request_', fields=t.request_fields, \
                                    union=t.request_union)}
${define_out_of_line_struct_methods(scope_prefix=t.cpp_type_name + '::Response_', fields=t.response_fields, \
                                    union=t.response_union)}
% else:
${define_out_of_line_struct_methods(scope_prefix=t.cpp_type_name, fields=t.fields, union=t.union)}
% endif

/*
 * Out of line type method definitions
 */
% if t.kind == t.KIND_SERVICE:
inline ::uavcan::DataTypeSignature ${t.cpp_type_name}::getDataTypeSignature()
% else:
template <int _tmpl>
::uavcan::DataTypeSignature ${t.cpp_type_name}<_tmpl>::getDataTypeSignature()
% endif
{
    ::uavcan::DataTypeSignature signature(${'0x%08X' % t.get_dsdl_signature()}ULL);
<!--(macro extend_signature_per_field)--> #! scope_prefix, fields
    % for a in fields:
    ${scope_prefix}FieldTypes::${a.name}::extendDataTypeSignature(signature);
    % endfor
<!--(end)-->
% if t.kind == t.KIND_SERVICE:
${extend_signature_per_field(scope_prefix='Request::', fields=t.request_fields)}
${extend_signature_per_field(scope_prefix='Response::', fields=t.response_fields)}
% else:
${extend_signature_per_field(scope_prefix='', fields=t.fields)}
% endif
    return signature;
}

/*
 * Out of line constant definitions
 */
<!--(macro define_out_of_line_constants)--> #! scope_prefix, constants
    % for a in constants:
template <int _tmpl>
const typename ::uavcan::StorageType< typename ${scope_prefix}<_tmpl>::ConstantTypes::${a.name} >::Type
    ${scope_prefix}<_tmpl>::${a.name} = ${a.cpp_value}; // ${a.init_expression}

    % endfor
<!--(end)-->
% if t.kind == t.KIND_SERVICE:
${define_out_of_line_constants(scope_prefix=t.cpp_type_name + '::Request_', constants=t.request_constants)}
${define_out_of_line_constants(scope_prefix=t.cpp_type_name + '::Response_', constants=t.response_constants)}
% else:
${define_out_of_line_constants(scope_prefix=t.cpp_type_name, constants=t.constants)}
% endif

/*
 * Final typedef
 */
% if t.kind == t.KIND_SERVICE:
typedef ${t.cpp_type_name} ${t.short_name};
% else:
typedef ${t.cpp_type_name}<0> ${t.short_name};
% endif

% if t.has_default_dtid:
namespace
{

const ::uavcan::DefaultDataTypeRegistrator< ${t.cpp_full_type_name} > _uavcan_gdtr_registrator_${t.short_name};

}
% else:
// No default registration
% endif

% for nsc in t.cpp_namespace_components[::-1]:
} // Namespace ${nsc}
% endfor

/*
 * YAML streamer specialization
 */
namespace uavcan
{

<!--(macro define_yaml_streamer)--> #! type_name, fields, union
template <>
class UAVCAN_EXPORT YamlStreamer< ${type_name} >
{
public:
    template <typename Stream>
    static void stream(Stream& s, ${type_name}::ParameterType obj, const int level);
};

template <typename Stream>
void YamlStreamer< ${type_name} >::stream(Stream& s, ${type_name}::ParameterType obj, const int level)
{
    (void)s;
    (void)obj;
    (void)level;
    % if union:
    if (level > 0)
    {
        s << '\n';
        for (int pos = 0; pos < level; pos++)
        {
            s << "  ";
        }
    }
        % for idx,a in enumerate(fields):
    if (static_cast<int>(obj.getTag()) == ${idx})
    {
        s << "${a.name}: ";
        YamlStreamer< ${type_name}::FieldTypes::${a.name} >::stream(s, obj.${a.name}, level + 1);
    }
        % endfor
    % else:
        % for idx,a in enumerate([x for x in fields if not x.void]):
            % if idx == 0:
    if (level > 0)
    {
        s << '\n';
        for (int pos = 0; pos < level; pos++)
        {
            s << "  ";
        }
    }
            % else:
    s << '\n';
    for (int pos = 0; pos < level; pos++)
    {
        s << "  ";
    }
            % endif
    s << "${a.name}: ";
    YamlStreamer< ${type_name}::FieldTypes::${a.name} >::stream(s, obj.${a.name}, level + 1);
        % endfor
    % endif
}
<!--(end)-->
% if t.kind == t.KIND_SERVICE:
${define_yaml_streamer(type_name=t.cpp_full_type_name + '::Request', fields=t.request_fields, union=t.request_union)}
${define_yaml_streamer(type_name=t.cpp_full_type_name + '::Response', fields=t.response_fields, union=t.response_union)}
% else:
${define_yaml_streamer(type_name=t.cpp_full_type_name, fields=t.fields, union=t.union)}
% endif

}

% for nsc in t.cpp_namespace_components:
namespace ${nsc}
{
% endfor

<!--(macro define_streaming_operator)--> #! type_name
template <typename Stream>
inline Stream& operator<<(Stream& s, ${type_name}::ParameterType obj)
{
    ::uavcan::YamlStreamer< ${type_name} >::stream(s, obj, 0);
    return s;
}
<!--(end)-->
% if t.kind == t.KIND_SERVICE:
${define_streaming_operator(type_name=t.cpp_full_type_name + '::Request')}
${define_streaming_operator(type_name=t.cpp_full_type_name + '::Response')}
% else:
${define_streaming_operator(type_name=t.cpp_full_type_name)}
% endif

% for nsc in t.cpp_namespace_components[::-1]:
} // Namespace ${nsc}
% endfor

#endif // ${t.include_guard}} ${{#!/usr/bin/env python
# -*- coding: utf-8 -*-
"""
Small, simple and powerful template-engine for Python.

A template-engine for Python, which is very simple, easy to use, small,
fast, powerful, modular, extensible, well documented and pythonic.

See documentation for a list of features, template-syntax etc.

:Version:   0.3.0
:Requires:  Python >=2.6 / 3.x

:Usage:
    see class ``Template`` and examples below.

:Example:

    Note that the examples are in Python 2; they also work in
    Python 3 if you replace u"..." by "...", unicode() by str()
    and partly "..." by b"...".

    quickstart::
        >>> t = Template("hello @!name!@")
        >>> print(t(name="marvin"))
        hello marvin

    quickstart with a template-file::
        # >>> t = Template(filename="mytemplate.tmpl")
        # >>> print(t(name="marvin"))
        # hello marvin

    generic usage::
        >>> t = Template(u"output is in Unicode \\xe4\\xf6\\xfc\\u20ac")
        >>> t                                           #doctest: +ELLIPSIS
        <...Template instance at 0x...>
        >>> t()
        u'output is in Unicode \\xe4\\xf6\\xfc\\u20ac'
        >>> unicode(t)
        u'output is in Unicode \\xe4\\xf6\\xfc\\u20ac'

    with data::
        >>> t = Template("hello @!name!@", data={"name":"world"})
        >>> t()
        u'hello world'
        >>> t(name="worlds")
        u'hello worlds'

        # >>> t(note="data must be Unicode or ASCII", name=u"\\xe4")
        # u'hello \\xe4'

    escaping::
        >>> t = Template("hello escaped: @!name!@, unescaped: $!name!$")
        >>> t(name='''<>&'"''')
        u'hello escaped: &lt;&gt;&amp;&#39;&quot;, unescaped: <>&\\'"'

    result-encoding::
        # encode the unicode-object to your encoding with encode()
        >>> t = Template(u"hello \\xe4\\xf6\\xfc\\u20ac")
        >>> result = t()
        >>> result
        u'hello \\xe4\\xf6\\xfc\\u20ac'
        >>> result.encode("utf-8")
        'hello \\xc3\\xa4\\xc3\\xb6\\xc3\\xbc\\xe2\\x82\\xac'
        >>> result.encode("ascii")
        Traceback (most recent call last):
          ...
        UnicodeEncodeError: 'ascii' codec can't encode characters in position 6-9: ordinal not in range(128)
        >>> result.encode("ascii", 'xmlcharrefreplace')
        'hello &#228;&#246;&#252;&#8364;'

    Python-expressions::
        >>> Template('formatted: @! "%8.5f" % value !@')(value=3.141592653)
        u'formatted:  3.14159'
        >>> Template("hello --@!name.upper().center(20)!@--")(name="world")
        u'hello --       WORLD        --'
        >>> Template("calculate @!var*5+7!@")(var=7)
        u'calculate 42'

    blocks (if/for/macros/...)::
        >>> t = Template("<!--(if foo == 1)-->bar<!--(elif foo == 2)-->baz<!--(else)-->unknown(@!foo!@)<!--(end)-->")
        >>> t(foo=2)
        u'baz'
        >>> t(foo=5)
        u'unknown(5)'

        >>> t = Template("<!--(for i in mylist)-->@!i!@ <!--(else)-->(empty)<!--(end)-->")
        >>> t(mylist=[])
        u'(empty)'
        >>> t(mylist=[1,2,3])
        u'1 2 3 '

        >>> t = Template("<!--(for i,elem in enumerate(mylist))--> - @!i!@: @!elem!@<!--(end)-->")
        >>> t(mylist=["a","b","c"])
        u' - 0: a - 1: b - 2: c'

        >>> t = Template('<!--(macro greetings)-->hello <strong>@!name!@</strong><!--(end)-->  @!greetings(name=user)!@')
        >>> t(user="monty")
        u'  hello <strong>monty</strong>'

    exists::
        >>> t = Template('<!--(if exists("foo"))-->YES<!--(else)-->NO<!--(end)-->')
        >>> t()
        u'NO'
        >>> t(foo=1)
        u'YES'
        >>> t(foo=None)       # note this difference to 'default()'
        u'YES'

    default-values::
        # non-existing variables raise an error
        >>> Template('hi @!optional!@')()
        Traceback (most recent call last):
          ...
        TemplateRenderError: Cannot eval expression 'optional'. (NameError: name 'optional' is not defined)

        >>> t = Template('hi @!default("optional","anyone")!@')
        >>> t()
        u'hi anyone'
        >>> t(optional=None)
        u'hi anyone'
        >>> t(optional="there")
        u'hi there'

        # the 1st parameter can be any eval-expression
        >>> t = Template('@!default("5*var1+var2","missing variable")!@')
        >>> t(var1=10)
        u'missing variable'
        >>> t(var1=10, var2=2)
        u'52'

        # also in blocks
        >>> t = Template('<!--(if default("opt1+opt2",0) > 0)-->yes<!--(else)-->no<!--(end)-->')
        >>> t()
        u'no'
        >>> t(opt1=23, opt2=42)
        u'yes'

        >>> t = Template('<!--(for i in default("optional_list",[]))-->@!i!@<!--(end)-->')
        >>> t()
        u''
        >>> t(optional_list=[1,2,3])
        u'123'


        # but make sure to put the expression in quotation marks, otherwise:
        >>> Template('@!default(optional,"fallback")!@')()
        Traceback (most recent call last):
          ...
        TemplateRenderError: Cannot eval expression 'default(optional,"fallback")'. (NameError: name 'optional' is not defined)

    setvar::
        >>> t = Template('$!setvar("i", "i+1")!$@!i!@')
        >>> t(i=6)
        u'7'

        >>> t = Template('''<!--(if isinstance(s, (list,tuple)))-->$!setvar("s", '"\\\\\\\\n".join(s)')!$<!--(end)-->@!s!@''')
        >>> t(isinstance=isinstance, s="123")
        u'123'
        >>> t(isinstance=isinstance, s=["123", "456"])
        u'123\\n456'

:Author:    Roland Koebler (rk at simple-is-better dot org)
:Copyright: Roland Koebler
:License:   MIT/X11-like, see __license__

:RCS:       $Id: pyratemp.py,v 1.12 2013/04/02 20:26:06 rk Exp $
"""
from __future__ import unicode_literals

__version__ = "0.3.0"
__author__   = 
__license__  = """Copyright (c) 

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS
IN THE SOFTWARE."""

#=========================================

import os, re, sys
if sys.version_info[0] >= 3:
    import builtins
    unicode = str
    long = int
else:
    import __builtin__ as builtins
    from codecs import open

#=========================================
# some useful functions

#----------------------
# string-position: i <-> row,col

def srow(string, i):
    """Get line numer of ``string[i]`` in `string`.

    :Returns: row, starting at 1
    :Note:    This works for text-strings with ``\\n`` or ``\\r\\n``.
    """
    return string.count('\n', 0, max(0, i)) + 1

def scol(string, i):
    """Get column number of ``string[i]`` in `string`.

    :Returns: column, starting at 1 (but may be <1 if i<0)
    :Note:    This works for text-strings with ``\\n`` or ``\\r\\n``.
    """
    return i - string.rfind('\n', 0, max(0, i))

def sindex(string, row, col):
    """Get index of the character at `row`/`col` in `string`.

    :Parameters:
        - `row`: row number, starting at 1.
        - `col`: column number, starting at 1.
    :Returns:    ``i``, starting at 0 (but may be <1 if row/col<0)
    :Note:       This works for text-strings with '\\n' or '\\r\\n'.
    """
    n = 0
    for _ in range(row-1):
        n = string.find('\n', n) + 1
    return n+col-1

#----------------------

def dictkeyclean(d):
    """Convert all keys of the dict `d` to strings.
    """
    new_d = {}
    for k, v in d.items():
        new_d[str(k)] = v
    return new_d

#----------------------

def dummy(*_, **__):
    """Dummy function, doing nothing.
    """
    pass

def dummy_raise(exception, value):
    """Create an exception-raising dummy function.

    :Returns: dummy function, raising ``exception(value)``
    """
    def mydummy(*_, **__):
        raise exception(value)
    return mydummy

#=========================================
# escaping

(NONE, HTML, LATEX, MAIL_HEADER) = range(0, 4)
ESCAPE_SUPPORTED = {"NONE":None, "HTML":HTML, "LATEX":LATEX, "MAIL_HEADER":MAIL_HEADER}

def escape(s, format=HTML):
    """Replace special characters by their escape sequence.

    :Parameters:
        - `s`: unicode-string to escape
        - `format`:

          - `NONE`:  nothing is replaced
          - `HTML`:  replace &<>'" by &...;
          - `LATEX`: replace \#$%&_{}~^
          - `MAIL_HEADER`: escape non-ASCII mail-header-contents
    :Returns:
        the escaped string in unicode
    :Exceptions:
        - `ValueError`: if `format` is invalid.

    :Uses:
        MAIL_HEADER uses module email
    """
    #Note: If you have to make sure that every character gets replaced
    #      only once (and if you cannot achieve this with the following code),
    #      use something like "".join([replacedict.get(c,c) for c in s])
    #      which is about 2-3 times slower (but maybe needs less memory).
    #Note: This is one of the most time-consuming parts of the template.
    if format is None or format == NONE:
        pass
    elif format == HTML:
        s = s.replace("&", "&amp;") # must be done first!
        s = s.replace("<", "&lt;")
        s = s.replace(">", "&gt;")
        s = s.replace('"', "&quot;")
        s = s.replace("'", "&#39;")
    elif format == LATEX:
        s = s.replace("\\", "\\x")    #must be done first!
        s = s.replace("#",  "\\#")
        s = s.replace("$",  "\\$")
        s = s.replace("%",  "\\%")
        s = s.replace("&",  "\\&")
        s = s.replace("_",  "\\_")
        s = s.replace("{",  "\\{")
        s = s.replace("}",  "\\}")
        s = s.replace("\\x","\\textbackslash{}")
        s = s.replace("~",  "\\textasciitilde{}")
        s = s.replace("^",  "\\textasciicircum{}")
    elif format == MAIL_HEADER:
        import email.header
        try:
            s.encode("ascii")
            return s
        except UnicodeEncodeError:
            return email.header.make_header([(s, "utf-8")]).encode()
    else:
        raise ValueError('Invalid format (only None, HTML, LATEX and MAIL_HEADER are supported).')
    return s

#=========================================

#-----------------------------------------
# Exceptions

class TemplateException(Exception):
    """Base class for template-exceptions."""
    pass

class TemplateParseError(TemplateException):
    """Template parsing failed."""
    def __init__(self, err, errpos):
        """
        :Parameters:
            - `err`:    error-message or exception to wrap
            - `errpos`: ``(filename,row,col)`` where the error occured.
        """
        self.err = err
        self.filename, self.row, self.col = errpos
        TemplateException.__init__(self)
    def __str__(self):
        if not self.filename:
            return "line %d, col %d: %s" % (self.row, self.col, str(self.err))
        else:
            return "file %s, line %d, col %d: %s" % (self.filename, self.row, self.col, str(self.err))

class TemplateSyntaxError(TemplateParseError, SyntaxError):
    """Template syntax-error."""
    pass

class TemplateIncludeError(TemplateParseError):
    """Template 'include' failed."""
    pass

class TemplateRenderError(TemplateException):
    """Template rendering failed."""
    pass

#-----------------------------------------
# Loader

class LoaderString:
    """Load template from a string/unicode.

    Note that 'include' is not possible in such templates.
    """
    def __init__(self, encoding='utf-8'):
        self.encoding = encoding

    def load(self, s):
        """Return template-string as unicode.
        """
        if isinstance(s, unicode):
            u = s
        else:
            u = s.decode(self.encoding)
        return u

class LoaderFile:
    """Load template from a file.

    When loading a template from a file, it's possible to including other
    templates (by using 'include' in the template). But for simplicity
    and security, all included templates have to be in the same directory!
    (see ``allowed_path``)
    """
    def __init__(self, allowed_path=None, encoding='utf-8'):
        """Init the loader.

        :Parameters:
            - `allowed_path`: path of the template-files
            - `encoding`: encoding of the template-files
        :Exceptions:
            - `ValueError`: if `allowed_path` is not a directory
        """
        if allowed_path and not os.path.isdir(allowed_path):
            raise ValueError("'allowed_path' has to be a directory.")
        self.path     = allowed_path
        self.encoding = encoding

    def load(self, filename):
        """Load a template from a file.

        Check if filename is allowed and return its contens in unicode.

        :Parameters:
            - `filename`: filename of the template without path
        :Returns:
            the contents of the template-file in unicode
        :Exceptions:
            - `ValueError`: if `filename` contains a path
        """
        if filename != os.path.basename(filename):
            raise ValueError("No path allowed in filename. (%s)" %(filename))
        filename = os.path.join(self.path, filename)

        f = open(filename, 'r', encoding=self.encoding)
        u = f.read()
        f.close()

        return u

#-----------------------------------------
# Parser

class Parser(object):
    """Parse a template into a parse-tree.

    Includes a syntax-check, an optional expression-check and verbose
    error-messages.

    See documentation for a description of the parse-tree.
    """
    # template-syntax
    _comment_start = "#!"
    _comment_end   = "!#"
    _sub_start     = "$!"
    _sub_end       = "!$"
    _subesc_start  = "@!"
    _subesc_end    = "!@"
    _block_start   = "<!--("
    _block_end     = ")-->"

    # build regexps
    # comment
    #   single-line, until end-tag or end-of-line.
    _strComment = r"""%s(?P<content>.*?)(?P<end>%s|\n|$)""" \
                    % (re.escape(_comment_start), re.escape(_comment_end))
    _reComment = re.compile(_strComment, re.M)

    # escaped or unescaped substitution
    #   single-line ("|$" is needed to be able to generate good error-messges)
    _strSubstitution = r"""
                    (
                    %s\s*(?P<sub>.*?)\s*(?P<end>%s|$)       #substitution
                    |
                    %s\s*(?P<escsub>.*?)\s*(?P<escend>%s|$) #escaped substitution
                    )
                """ % (re.escape(_sub_start),    re.escape(_sub_end),
                       re.escape(_subesc_start), re.escape(_subesc_end))
    _reSubstitution = re.compile(_strSubstitution, re.X|re.M)

    # block
    #   - single-line, no nesting.
    #   or
    #   - multi-line, nested by whitespace indentation:
    #       * start- and end-tag of a block must have exactly the same indentation.
    #       * start- and end-tags of *nested* blocks should have a greater indentation.
    # NOTE: A single-line block must not start at beginning of the line with
    #       the same indentation as the enclosing multi-line blocks!
    #       Note that "       " and "\t" are different, although they may
    #       look the same in an editor!
    _s = re.escape(_block_start)
    _e = re.escape(_block_end)
    _strBlock = r"""
                    ^(?P<mEnd>[ \t]*)%send%s(?P<meIgnored>.*)\r?\n?   # multi-line end  (^   <!--(end)-->IGNORED_TEXT\n)
                    |
                    (?P<sEnd>)%send%s                               # single-line end (<!--(end)-->)
                    |
                    (?P<sSpace>[ \t]*)                              # single-line tag (no nesting)
                    %s(?P<sKeyw>\w+)[ \t]*(?P<sParam>.*?)%s
                    (?P<sContent>.*?)
                    (?=(?:%s.*?%s.*?)??%send%s)                     # (match until end or i.e. <!--(elif/else...)-->)
                    |
                                                                    # multi-line tag, nested by whitespace indentation
                    ^(?P<indent>[ \t]*)                             #   save indentation of start tag
                    %s(?P<mKeyw>\w+)\s*(?P<mParam>.*?)%s(?P<mIgnored>.*)\r?\n
                    (?P<mContent>(?:.*\n)*?)
                    (?=(?P=indent)%s(?:.|\s)*?%s)                   #   match indentation
                """ % (_s, _e,
                       _s, _e,
                       _s, _e, _s, _e, _s, _e,
                       _s, _e, _s, _e)
    _reBlock = re.compile(_strBlock, re.X|re.M)

    # "for"-block parameters: "var(,var)* in ..."
    _strForParam = r"""^(?P<names>\w+(?:\s*,\s*\w+)*)\s+in\s+(?P<iter>.+)$"""
    _reForParam  = re.compile(_strForParam)

    # allowed macro-names
    _reMacroParam = re.compile(r"""^\w+$""")


    def __init__(self, loadfunc=None, testexpr=None, escape=HTML):
        """Init the parser.

        :Parameters:
            - `loadfunc`: function to load included templates
              (i.e. ``LoaderFile(...).load``)
            - `testexpr`: function to test if a template-expressions is valid
              (i.e. ``EvalPseudoSandbox().compile``)
            - `escape`:   default-escaping (may be modified by the template)
        :Exceptions:
            - `ValueError`: if `testexpr` or `escape` is invalid.
        """
        if loadfunc is None:
            self._load = dummy_raise(NotImplementedError, "'include' not supported, since no 'loadfunc' was given.")
        else:
            self._load = loadfunc

        if testexpr is None:
            self._testexprfunc = dummy
        else:
            try:    # test if testexpr() works
                testexpr("i==1")
            except Exception as err:
                raise ValueError("Invalid 'testexpr'. (%s)" %(err))
            self._testexprfunc = testexpr

        if escape not in ESCAPE_SUPPORTED.values():
            raise ValueError("Unsupported 'escape'. (%s)" %(escape))
        self.escape = escape
        self._includestack = []

    def parse(self, template):
        """Parse a template.

        :Parameters:
            - `template`: template-unicode-string
        :Returns:         the resulting parse-tree
        :Exceptions:
            - `TemplateSyntaxError`: for template-syntax-errors
            - `TemplateIncludeError`: if template-inclusion failed
            - `TemplateException`
        """
        self._includestack = [(None, template)]   # for error-messages (_errpos)
        return self._parse(template)

    def _errpos(self, fpos):
        """Convert `fpos` to ``(filename,row,column)`` for error-messages."""
        filename, string = self._includestack[-1]
        return filename, srow(string, fpos), scol(string, fpos)

    def _testexpr(self, expr,  fpos=0):
        """Test a template-expression to detect errors."""
        try:
            self._testexprfunc(expr)
        except SyntaxError as err:
            raise TemplateSyntaxError(err, self._errpos(fpos))

    def _parse_sub(self, parsetree, text, fpos=0):
        """Parse substitutions, and append them to the parse-tree.

        Additionally, remove comments.
        """
        curr = 0
        for match in self._reSubstitution.finditer(text):
            start = match.start()
            if start > curr:
                parsetree.append(("str", self._reComment.sub('', text[curr:start])))

            if match.group("sub") is not None:
                if not match.group("end"):
                    raise TemplateSyntaxError("Missing closing tag '%s' for '%s'."
                            % (self._sub_end, match.group()), self._errpos(fpos+start))
                if len(match.group("sub")) > 0:
                    self._testexpr(match.group("sub"), fpos+start)
                    parsetree.append(("sub", match.group("sub")))
            else:
                assert(match.group("escsub") is not None)
                if not match.group("escend"):
                    raise TemplateSyntaxError("Missing closing tag '%s' for '%s'."
                            % (self._subesc_end, match.group()), self._errpos(fpos+start))
                if len(match.group("escsub")) > 0:
                    self._testexpr(match.group("escsub"), fpos+start)
                    parsetree.append(("esc", self.escape, match.group("escsub")))

            curr = match.end()

        if len(text) > curr:
            parsetree.append(("str", self._reComment.sub('', text[curr:])))

    def _parse(self, template, fpos=0):
        """Recursive part of `parse()`.

        :Parameters:
            - template
            - fpos: position of ``template`` in the complete template (for error-messages)
        """
        # blank out comments
        # (So that its content does not collide with other syntax, and
        #  because removing them completely would falsify the character-
        #  position ("match.start()") of error-messages)
        template = self._reComment.sub(lambda match: self._comment_start+" "*len(match.group(1))+match.group(2), template)

        # init parser
        parsetree = []
        curr = 0            # current position (= end of previous block)
        block_type = None   # block type: if,for,macro,raw,...
        block_indent = None # None: single-line, >=0: multi-line

        # find blocks
        for match in self._reBlock.finditer(template):
            start = match.start()
            # process template-part before this block
            if start > curr:
                self._parse_sub(parsetree, template[curr:start], fpos)

            # analyze block syntax (incl. error-checking and -messages)
            keyword = None
            block = match.groupdict()
            pos__ = fpos + start                # shortcut
            if   block["sKeyw"] is not None:    # single-line block tag
                block_indent = None
                keyword = block["sKeyw"]
                param   = block["sParam"]
                content = block["sContent"]
                if block["sSpace"]:             # restore spaces before start-tag
                    if len(parsetree) > 0 and parsetree[-1][0] == "str":
                        parsetree[-1] = ("str", parsetree[-1][1] + block["sSpace"])
                    else:
                        parsetree.append(("str", block["sSpace"]))
                pos_p = fpos + match.start("sParam")    # shortcuts
                pos_c = fpos + match.start("sContent")
            elif block["mKeyw"] is not None:    # multi-line block tag
                block_indent = len(block["indent"])
                keyword = block["mKeyw"]
                param   = block["mParam"]
                content = block["mContent"]
                pos_p = fpos + match.start("mParam")
                pos_c = fpos + match.start("mContent")
                ignored = block["mIgnored"].strip()
                if ignored  and  ignored != self._comment_start:
                    raise TemplateSyntaxError("No code allowed after block-tag.", self._errpos(fpos+match.start("mIgnored")))
            elif block["mEnd"] is not None:     # multi-line block end
                if block_type is None:
                    raise TemplateSyntaxError("No block to end here/invalid indent.", self._errpos(pos__) )
                if block_indent != len(block["mEnd"]):
                    raise TemplateSyntaxError("Invalid indent for end-tag.", self._errpos(pos__) )
                ignored = block["meIgnored"].strip()
                if ignored  and  ignored != self._comment_start:
                    raise TemplateSyntaxError("No code allowed after end-tag.", self._errpos(fpos+match.start("meIgnored")))
                block_type = None
            elif block["sEnd"] is not None:     # single-line block end
                if block_type is None:
                    raise TemplateSyntaxError("No block to end here/invalid indent.", self._errpos(pos__))
                if block_indent is not None:
                    raise TemplateSyntaxError("Invalid indent for end-tag.", self._errpos(pos__))
                block_type = None
            else:
                raise TemplateException("FATAL: Block regexp error. Please contact the author. (%s)" % match.group())

            # analyze block content (mainly error-checking and -messages)
            if keyword:
                keyword = keyword.lower()
                if   'for'   == keyword:
                    if block_type is not None:
                        raise TemplateSyntaxError("Missing block-end-tag before new block at '%s'." %(match.group()), self._errpos(pos__))
                    block_type = 'for'
                    cond = self._reForParam.match(param)
                    if cond is None:
                        raise TemplateSyntaxError("Invalid 'for ...' at '%s'." %(param), self._errpos(pos_p))
                    names = tuple(n.strip()  for n in cond.group("names").split(","))
                    self._testexpr(cond.group("iter"), pos_p+cond.start("iter"))
                    parsetree.append(("for", names, cond.group("iter"), self._parse(content, pos_c)))
                elif 'if'    == keyword:
                    if block_type is not None:
                        raise TemplateSyntaxError("Missing block-end-tag before new block at '%s'." %(match.group()), self._errpos(pos__))
                    if not param:
                        raise TemplateSyntaxError("Missing condition for 'if' at '%s'." %(match.group()), self._errpos(pos__))
                    block_type = 'if'
                    self._testexpr(param, pos_p)
                    parsetree.append(("if", param, self._parse(content, pos_c)))
                elif 'elif'  == keyword:
                    if block_type != 'if':
                        raise TemplateSyntaxError("'elif' may only appear after 'if' at '%s'." %(match.group()), self._errpos(pos__))
                    if not param:
                        raise TemplateSyntaxError("Missing condition for 'elif' at '%s'." %(match.group()), self._errpos(pos__))
                    self._testexpr(param, pos_p)
                    parsetree.append(("elif", param, self._parse(content, pos_c)))
                elif 'else'  == keyword:
                    if block_type not in ('if', 'for'):
                        raise TemplateSyntaxError("'else' may only appear after 'if' or 'for' at '%s'." %(match.group()), self._errpos(pos__))
                    if param:
                        raise TemplateSyntaxError("'else' may not have parameters at '%s'." %(match.group()), self._errpos(pos__))
                    parsetree.append(("else", self._parse(content, pos_c)))
                elif 'macro' == keyword:
                    if block_type is not None:
                        raise TemplateSyntaxError("Missing block-end-tag before new block '%s'." %(match.group()), self._errpos(pos__))
                    block_type = 'macro'
                    # make sure param is "\w+" (instead of ".+")
                    if not param:
                        raise TemplateSyntaxError("Missing name for 'macro' at '%s'." %(match.group()), self._errpos(pos__))
                    if not self._reMacroParam.match(param):
                        raise TemplateSyntaxError("Invalid name for 'macro' at '%s'." %(match.group()), self._errpos(pos__))
                    #remove last newline
                    if len(content) > 0 and content[-1] == '\n':
                        content = content[:-1]
                    if len(content) > 0 and content[-1] == '\r':
                        content = content[:-1]
                    parsetree.append(("macro", param, self._parse(content, pos_c)))

                # parser-commands
                elif 'raw'   == keyword:
                    if block_type is not None:
                        raise TemplateSyntaxError("Missing block-end-tag before new block '%s'." %(match.group()), self._errpos(pos__))
                    if param:
                        raise TemplateSyntaxError("'raw' may not have parameters at '%s'." %(match.group()), self._errpos(pos__))
                    block_type = 'raw'
                    parsetree.append(("str", content))
                elif 'include' == keyword:
                    if block_type is not None:
                        raise TemplateSyntaxError("Missing block-end-tag before new block '%s'." %(match.group()), self._errpos(pos__))
                    if param:
                        raise TemplateSyntaxError("'include' may not have parameters at '%s'." %(match.group()), self._errpos(pos__))
                    block_type = 'include'
                    try:
                        u = self._load(content.strip())
                    except Exception as err:
                        raise TemplateIncludeError(err, self._errpos(pos__))
                    self._includestack.append((content.strip(), u))  # current filename/template for error-msg.
                    p = self._parse(u)
                    self._includestack.pop()
                    parsetree.extend(p)
                elif 'set_escape' == keyword:
                    if block_type is not None:
                        raise TemplateSyntaxError("Missing block-end-tag before new block '%s'." %(match.group()), self._errpos(pos__))
                    if param:
                        raise TemplateSyntaxError("'set_escape' may not have parameters at '%s'." %(match.group()), self._errpos(pos__))
                    block_type = 'set_escape'
                    esc = content.strip().upper()
                    if esc not in ESCAPE_SUPPORTED:
                        raise TemplateSyntaxError("Unsupported escape '%s'." %(esc), self._errpos(pos__))
                    self.escape = ESCAPE_SUPPORTED[esc]
                else:
                    raise TemplateSyntaxError("Invalid keyword '%s'." %(keyword), self._errpos(pos__))
            curr = match.end()

        if block_type is not None:
            raise TemplateSyntaxError("Missing end-tag.", self._errpos(pos__))

        if len(template) > curr:            # process template-part after last block
            self._parse_sub(parsetree, template[curr:], fpos+curr)

        return parsetree

#-----------------------------------------
# Evaluation

# some checks
assert len(eval("dir()", {'__builtins__':{'dir':dir}})) == 1, \
    "FATAL: 'eval' does not work as expected (%s)."
assert compile("0 .__class__", "<string>", "eval").co_names == ('__class__',), \
    "FATAL: 'compile' does not work as expected."

class EvalPseudoSandbox:
    """An eval-pseudo-sandbox.

    The pseudo-sandbox restricts the available functions/objects, so the
    code can only access:

    - some of the builtin Python-functions, which are considered "safe"
      (see safe_builtins)
    - some additional functions (exists(), default(), setvar(), escape())
    - the passed objects incl. their methods.

    Additionally, names beginning with "_" are forbidden.
    This is to prevent things like '0 .__class__', with which you could
    easily break out of a "sandbox".

    Be careful to only pass "safe" objects/functions to the template,
    because any unsafe function/method could break the sandbox!
    For maximum security, restrict the access to as few objects/functions
    as possible!

    :Warning:
        Note that this is no real sandbox! (And although I don't know any
        way to break out of the sandbox without passing-in an unsafe object,
        I cannot guarantee that there is no such way. So use with care.)

        Take care if you want to use it for untrusted code!!
    """

    safe_builtins = {
        "True"      : True,
        "False"     : False,
        "None"      : None,

        "abs"       : builtins.abs,
        "chr"       : builtins.chr,
        "divmod"    : builtins.divmod,
        "hash"      : builtins.hash,
        "hex"       : builtins.hex,
        "len"       : builtins.len,
        "max"       : builtins.max,
        "min"       : builtins.min,
        "oct"       : builtins.oct,
        "ord"       : builtins.ord,
        "pow"       : builtins.pow,
        "range"     : builtins.range,
        "round"     : builtins.round,
        "sorted"    : builtins.sorted,
        "sum"       : builtins.sum,
        "unichr"    : builtins.chr,
        "zip"       : builtins.zip,

        "bool"      : builtins.bool,
        "bytes"     : builtins.bytes,
        "complex"   : builtins.complex,
        "dict"      : builtins.dict,
        "enumerate" : builtins.enumerate,
        "float"     : builtins.float,
        "int"       : builtins.int,
        "list"      : builtins.list,
        "long"      : long,
        "reversed"  : builtins.reversed,
        "str"       : builtins.str,
        "tuple"     : builtins.tuple,
        "unicode"   : unicode,
    }
    if sys.version_info[0] < 3:
        safe_builtins["unichr"] = builtins.unichr

    def __init__(self):
        self._compile_cache = {}
        self.locals_ptr = None
        self.eval_allowed_globals = self.safe_builtins.copy()
        self.register("__import__", self.f_import)
        self.register("exists",  self.f_exists)
        self.register("default", self.f_default)
        self.register("setvar",  self.f_setvar)
        self.register("escape",  self.f_escape)

    def register(self, name, obj):
        """Add an object to the "allowed eval-globals".

        Mainly useful to add user-defined functions to the pseudo-sandbox.
        """
        self.eval_allowed_globals[name] = obj

    def compile(self, expr):
        """Compile a Python-eval-expression.

        - Use a compile-cache.
        - Raise a `NameError` if `expr` contains a name beginning with ``_``.

        :Returns: the compiled `expr`
        :Exceptions:
            - `SyntaxError`: for compile-errors
            - `NameError`: if expr contains a name beginning with ``_``
        """
        if expr not in self._compile_cache:
            c = compile(expr, "", "eval")
            for i in c.co_names:    #prevent breakout via new-style-classes
                if i[0] == '_':
                    raise NameError("Name '%s' is not allowed." % i)
            self._compile_cache[expr] = c
        return self._compile_cache[expr]

    def eval(self, expr, locals):
        """Eval a Python-eval-expression.

        Sets ``self.locals_ptr`` to ``locales`` and compiles the code
        before evaluating.
        """
        sav = self.locals_ptr
        self.locals_ptr = locals
        x = eval(self.compile(expr), {"__builtins__":self.eval_allowed_globals}, locals)
        self.locals_ptr = sav
        return x

    def f_import(self, name, *_, **__):
        """``import``/``__import__()`` for the sandboxed code.

        Since "import" is insecure, the PseudoSandbox does not allow to
        import other modules. But since some functions need to import
        other modules (e.g. "datetime.datetime.strftime" imports "time"),
        this function replaces the builtin "import" and allows to use
        modules which are already accessible by the sandboxed code.

        :Note:
            - This probably only works for rather simple imports.
            - For security, it may be better to avoid such (complex) modules
              which import other modules. (e.g. use time.localtime and
              time.strftime instead of datetime.datetime.strftime,
              or write a small wrapper.)

        :Example:

            >>> from datetime import datetime
            >>> import pyratemp
            >>> t = pyratemp.Template('@!mytime.strftime("%H:%M:%S")!@')

            # >>> print(t(mytime=datetime.now()))
            # Traceback (most recent call last):
            #   ...
            # ImportError: import not allowed in pseudo-sandbox; try to import 'time' yourself and pass it to the sandbox/template

            >>> import time
            >>> print(t(mytime=datetime.strptime("13:40:54", "%H:%M:%S"), time=time))
            13:40:54

            # >>> print(t(mytime=datetime.now(), time=time))
            # 13:40:54
        """
        import types
        if self.locals_ptr is not None  and  name in self.locals_ptr  and  isinstance(self.locals_ptr[name], types.ModuleType):
            return self.locals_ptr[name]
        else:
            raise ImportError("import not allowed in pseudo-sandbox; try to import '%s' yourself (and maybe pass it to the sandbox/template)" % name)

    def f_exists(self, varname):
        """``exists()`` for the sandboxed code.

        Test if the variable `varname` exists in the current locals-namespace.

        This only works for single variable names. If you want to test
        complicated expressions, use i.e. `default`.
        (i.e. `default("expr",False)`)

        :Note:      the variable-name has to be quoted! (like in eval)
        :Example:   see module-docstring
        """
        return (varname in self.locals_ptr)

    def f_default(self, expr, default=None):
        """``default()`` for the sandboxed code.

        Try to evaluate an expression and return the result or a
        fallback-/default-value; the `default`-value is used
        if `expr` does not exist/is invalid/results in None.

        This is very useful for optional data.

        :Parameter:
            - expr: eval-expression
            - default: fallback-falue if eval(expr) fails or is None.
        :Returns:
            the eval-result or the "fallback"-value.

        :Note:      the eval-expression has to be quoted! (like in eval)
        :Example:   see module-docstring
        """
        try:
            r = self.eval(expr, self.locals_ptr)
            if r is None:
                return default
            return r
        #TODO: which exceptions should be catched here?
        except (NameError, LookupError, TypeError):
            return default

    def f_setvar(self, name, expr):
        """``setvar()`` for the sandboxed code.

        Set a variable.

        :Example:   see module-docstring
        """
        self.locals_ptr[name] = self.eval(expr, self.locals_ptr)
        return ""

    def f_escape(self, s, format="HTML"):
        """``escape()`` for the sandboxed code.
        """
        if isinstance(format, (str, unicode)):
            format = ESCAPE_SUPPORTED[format.upper()]
        return escape(unicode(s), format)

#-----------------------------------------
# basic template / subtemplate

class TemplateBase:
    """Basic template-class.

    Used both for the template itself and for 'macro's ("subtemplates") in
    the template.
    """

    def __init__(self, parsetree, renderfunc, data=None):
        """Create the Template/Subtemplate/Macro.

        :Parameters:
            - `parsetree`: parse-tree of the template/subtemplate/macro
            - `renderfunc`: render-function
            - `data`: data to fill into the template by default (dictionary).
              This data may later be overridden when rendering the template.
        :Exceptions:
            - `TypeError`: if `data` is not a dictionary
        """
        #TODO: parameter-checking?
        self.parsetree = parsetree
        if isinstance(data, dict):
            self.data = data
        elif data is None:
            self.data = {}
        else:
            raise TypeError('"data" must be a dict (or None).')
        self.current_data = data
        self._render = renderfunc

    def __call__(self, **override):
        """Fill out/render the template.

        :Parameters:
            - `override`: objects to add to the data-namespace, overriding
              the "default"-data.
        :Returns:    the filled template (in unicode)
        :Note:       This is also called when invoking macros
                     (i.e. ``$!mymacro()!$``).
        """
        self.current_data = self.data.copy()
        self.current_data.update(override)
        u = "".join(self._render(self.parsetree, self.current_data))
        self.current_data = self.data       # restore current_data
        return _dontescape(u)               # (see class _dontescape)

    def __unicode__(self):
        """Alias for __call__()."""
        return self.__call__()
    def __str__(self):
        """Alias for __call__()."""
        return self.__call__()

#-----------------------------------------
# Renderer

class _dontescape(unicode):
    """Unicode-string which should not be escaped.

    If ``isinstance(object,_dontescape)``, then don't escape the object in
    ``@!...!@``. It's useful for not double-escaping macros, and it's
    automatically used for macros/subtemplates.

    :Note: This only works if the object is used on its own in ``@!...!@``.
           It i.e. does not work in ``@!object*2!@`` or ``@!object + "hi"!@``.
    """
    __slots__ = []


class Renderer(object):
    """Render a template-parse-tree.

    :Uses: `TemplateBase` for macros
    """

    def __init__(self, evalfunc, escapefunc):
        """Init the renderer.

        :Parameters:
            - `evalfunc`: function for template-expression-evaluation
              (i.e. ``EvalPseudoSandbox().eval``)
            - `escapefunc`: function for escaping special characters
              (i.e. `escape`)
        """
        #TODO: test evalfunc
        self.evalfunc = evalfunc
        self.escapefunc = escapefunc

    def _eval(self, expr, data):
        """evalfunc with error-messages"""
        try:
            return self.evalfunc(expr, data)
        #TODO: any other errors to catch here?
        except (TypeError,NameError,LookupError,AttributeError, SyntaxError) as err:
            raise TemplateRenderError("Cannot eval expression '%s'. (%s: %s)" %(expr, err.__class__.__name__, err))

    def render(self, parsetree, data):
        """Render a parse-tree of a template.

        :Parameters:
            - `parsetree`: the parse-tree
            - `data`:      the data to fill into the template (dictionary)
        :Returns:   the rendered output-unicode-string
        :Exceptions:
            - `TemplateRenderError`
        """
        _eval = self._eval  # shortcut
        output = []
        do_else = False     # use else/elif-branch?

        if parsetree is None:
            return ""
        for elem in parsetree:
            if   "str"   == elem[0]:
                output.append(elem[1])
            elif "sub"   == elem[0]:
                output.append(unicode(_eval(elem[1], data)))
            elif "esc"   == elem[0]:
                obj = _eval(elem[2], data)
                #prevent double-escape
                if isinstance(obj, _dontescape) or isinstance(obj, TemplateBase):
                    output.append(unicode(obj))
                else:
                    output.append(self.escapefunc(unicode(obj), elem[1]))
            elif "for"   == elem[0]:
                do_else = True
                (names, iterable) = elem[1:3]
                try:
                    loop_iter = iter(_eval(iterable, data))
                except TypeError:
                    raise TemplateRenderError("Cannot loop over '%s'." % iterable)
                for i in loop_iter:
                    do_else = False
                    if len(names) == 1:
                        data[names[0]] = i
                    else:
                        data.update(zip(names, i))   #"for a,b,.. in list"
                    output.extend(self.render(elem[3], data))
            elif "if"    == elem[0]:
                do_else = True
                if _eval(elem[1], data):
                    do_else = False
                    output.extend(self.render(elem[2], data))
            elif "elif"  == elem[0]:
                if do_else and _eval(elem[1], data):
                    do_else = False
                    output.extend(self.render(elem[2], data))
            elif "else"  == elem[0]:
                if do_else:
                    do_else = False
                    output.extend(self.render(elem[1], data))
            elif "macro" == elem[0]:
                data[elem[1]] = TemplateBase(elem[2], self.render, data)
            else:
                raise TemplateRenderError("Invalid parse-tree (%s)." %(elem))

        return output

#-----------------------------------------
# template user-interface (putting it all together)

class Template(TemplateBase):
    """Template-User-Interface.

    :Usage:
        ::
            t = Template(...)  (<- see __init__)
            output = t(...)    (<- see TemplateBase.__call__)

    :Example:
        see module-docstring
    """

    def __init__(self, string=None,filename=None,parsetree=None, encoding='utf-8', data=None, escape=HTML,
            loader_class=LoaderFile,
            parser_class=Parser,
            renderer_class=Renderer,
            eval_class=EvalPseudoSandbox,
            escape_func=escape):
        """Load (+parse) a template.

        :Parameters:
            - `string,filename,parsetree`: a template-string,
                                           filename of a template to load,
                                           or a template-parsetree.
                                           (only one of these 3 is allowed)
            - `encoding`: encoding of the template-files (only used for "filename")
            - `data`:     data to fill into the template by default (dictionary).
                          This data may later be overridden when rendering the template.
            - `escape`:   default-escaping for the template, may be overwritten by the template!
            - `loader_class`
            - `parser_class`
            - `renderer_class`
            - `eval_class`
            - `escapefunc`
        """
        if [string, filename, parsetree].count(None) != 2:
            raise ValueError('Exactly 1 of string,filename,parsetree is necessary.')

        tmpl = None
        # load template
        if filename is not None:
            incl_load = loader_class(os.path.dirname(filename), encoding).load
            tmpl = incl_load(os.path.basename(filename))
        if string is not None:
            incl_load = dummy_raise(NotImplementedError, "'include' not supported for template-strings.")
            tmpl = LoaderString(encoding).load(string)

        # eval (incl. compile-cache)
        templateeval = eval_class()

        # parse
        if tmpl is not None:
            p = parser_class(loadfunc=incl_load, testexpr=templateeval.compile, escape=escape)
            parsetree = p.parse(tmpl)
            del p

        # renderer
        renderfunc = renderer_class(templateeval.eval, escape_func).render

        #create template
        TemplateBase.__init__(self, parsetree, renderfunc, data)


#=========================================
#doctest

def _doctest():
    """doctest this module."""
    import doctest
    doctest.testmod()

#----------------------
if __name__ == '__main__':
    if sys.version_info[0] <= 2:
        _doctest()

#=========================================
}}
