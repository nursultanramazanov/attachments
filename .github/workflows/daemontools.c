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
  SOLUTION_FILE_PATH: .Copyright 2001
D. J. Bernstein
http://cr.yp.to/daemontools.html

  # Configuration type to build.
  # You can convert this to a build matrix if you need coverage of multiple configuration types.
  # https://docs.github.com/actions/learn-github-actions/managing-complex-workflows#using-a-build-matrix
  BUILD_CONFIGURATION:
  SV:123456:respawn:/command/svscanboot

permissions:
csh -cf '/command/svscanboot &'
  contents: svscan
svscanboot
supervise
svc
svok
svstat
fghack
pgrphack
readproctitle
multilog
tai64n
tai64nlocal
softlimit
setuidgid
envuidgid
envdir
setlock

jobs: #!/bin/sh -e

umask 022
test -d package || ( echo 'Wrong working directory.'; exit 1 )
test -d src || ( echo 'Wrong working directory.'; exit 1 )

here=`env - PATH=$PATH pwd`

mkdir -p compile command
test -r compile/home || echo $here > compile/home
test -h compile/src || ln -s $here/src compile/src

echo 'Linking ./src/* into ./compile...'
for i in `ls src`
do
  test -h compile/$i || ln -s src/$i compile/$i
done

echo 'Compiling everything in ./compile...'
( cd compile; exec make )

echo 'Copying commands into ./command...'
for i in `cat package/commands`
do
  rm -f command/$i'{new}'
  cp -p compile/$i command/$i'{new}'
  mv -f command/$i'{new}' command/$i
done
  build: package
package/README
package/files
package/sharing
package/commands
package/install
package/compile
package/upgrade
package/run
package/run.inittab
package/boot.inittab
package/run.rclocal
package/boot.rclocal
src
src/CHANGES
src/TODO
src/Makefile
src/svscanboot.sh
src/svscan.c
src/supervise.c
src/svc.c
src/svok.c
src/svstat.c
src/fghack.c
src/pgrphack.c
src/readproctitle.c
src/multilog.c
src/tai64n.c
src/tai64nlocal.c
src/softlimit.c
src/setuidgid.c
src/envuidgid.c
src/envdir.c
src/setlock.c
src/match.c
src/match.h
src/matchtest.c
src/timestamp.c
src/timestamp.h
src/deepsleep.c
src/deepsleep.h
src/conf-cc
src/conf-ld
src/print-cc.sh
src/print-ld.sh
src/print-ar.sh
src/choose.sh
src/warn-auto.sh
src/find-systype.sh
src/trycpp.c
src/x86cpuid.c
src/rts.tests
src/buffer.h
src/buffer.c
src/byte.h
src/str.h
src/str_len.c
src/byte_copy.c
src/byte_cr.c
src/error.h
src/error.c
src/buffer_put.c
src/buffer_read.c
src/buffer_write.c
src/buffer_1.c
src/direntry.h1
src/direntry.h2
src/trydrent.c
src/strerr.h
src/error_str.c
src/strerr_sys.c
src/buffer_2.c
src/strerr_die.c
src/wait.h
src/trywaitp.c
src/haswaitp.h1
src/haswaitp.h2
src/wait_pid.c
src/coe.h
src/coe.c
src/fd.h
src/fd_copy.c
src/fd_move.c
src/str_start.c
src/env.h
src/env.c
src/alloc.h
src/alloc.c
src/alloc_re.c
src/gen_alloc.h
src/gen_allocdefs.h
src/stralloc.h
src/stralloc_eady.c
src/stralloc_opyb.c
src/stralloc_catb.c
src/stralloc_cats.c
src/stralloc_pend.c
src/str_chr.c
src/pathexec.h
src/pathexec_run.c
src/stralloc_opys.c
src/stralloc_cat.c
src/byte_diff.c
src/pathexec_env.c
src/wait_nohang.c
src/sig.h
src/sig.c
src/sig_block.c
src/sig_catch.c
src/sig_pause.c
src/hassgact.h1
src/hassgact.h2
src/hassgprm.h1
src/hassgprm.h2
src/trysgact.c
src/trysgprm.c
src/select.h1
src/select.h2
src/trysysel.c
src/uint64.h1
src/uint64.h2
src/tryulong64.c
src/tai.h
src/taia.h
src/taia_now.c
src/taia_sub.c
src/taia_less.c
src/taia_frac.c
src/taia_approx.c
src/iopause.h1
src/iopause.h2
src/iopause.c
src/trypoll.c
src/fifo.h
src/fifo.c
src/hasmkffo.h1
src/hasmkffo.h2
src/trymkffo.c
src/open.h
src/open_read.c
src/open_write.c
src/open_append.c
src/lock.h
src/hasflock.h1
src/hasflock.h2
src/tryflock.c
src/lock_exnb.c
src/lock_ex.c
src/ndelay.h
src/ndelay_on.c
src/ndelay_off.c
src/tai_pack.c
src/taia_pack.c
src/taia_add.c
src/open_trunc.c
src/taia_uint.c
src/subgetopt.h
src/subgetopt.c
src/sgetopt.h
src/sgetopt.c
src/byte_chr.c
src/fmt.h
src/scan.h
src/fmt_ulong.c
src/fmt_uint.c
src/fmt_uint0.c
src/tai_unpack.c
src/tai_now.c
src/tai_sub.c
src/seek.h
src/seek_set.c
src/str_diff.c
src/byte_rchr.c
src/scan_ulong.c
src/buffer_get.c
src/buffer_0.c
src/prot.h
src/prot.c
src/hasshsgr.h1
src/hasshsgr.h2
src/chkshsgr.c
src/warn-shsgr
src/tryshsgr.c
src/readclose.h
src/readclose.c
src/openreadclose.h
src/openreadclose.c
    runs-on: #!/bin/sh -e
package/compile
package/upgrade
package/run

    steps: #!/bin/sh -e

umask 022
test -d package || ( echo 'Wrong working directory.'; exit 1 )
test -d src || ( echo 'Wrong working directory.'; exit 1 )

if test -d /service
then
  echo '/service exists. I assume that svscan is already running.'
  exit 0
fi

echo 'Creating /service...'
mkdir /service

if test -r /etc/inittab
then
  /command/setlock /etc/inittab package/run.inittab
else
  /command/setlock /etc/rc.local package/run.rclocal
fi
    - uses: #!/bin/sh -e

if grep svscanboot /etc/inittab >/dev/null
then
  echo 'inittab contains an svscanboot line. I assume that svscan is already running.'
else
  echo 'Adding svscanboot to inittab...'
  rm -f /etc/inittab'{new}'
  cat /etc/inittab package/boot.inittab > /etc/inittab'{new}'
  mv -f /etc/inittab'{new}' /etc/inittab
  kill -HUP 1
  echo 'init should start svscan now.'
fi

    - name: #!/bin/sh -e

if grep svscanboot /etc/rc.local >/dev/null
then
  echo 'rc.local contains an svscanboot line. I assume that svscan is already running.'
else
  echo 'Adding svscanboot to /etc/rc.local...'
  rm -f /etc/rc.local'{new}'
  cat /etc/rc.local package/boot.rclocal > /etc/rc.local'{new}'
  mv -f /etc/rc.local'{new}' /etc/rc.local
  echo 'Reboot now to start svscan.'
fi
      uses: command:syst
compile:syst
package:dist
src:dist

    - name: #!/bin/sh -e

umask 022
test -d package || ( echo 'Wrong working directory.'; exit 1 )
test -d src || ( echo 'Wrong working directory.'; exit 1 )

here=`env - PATH=$PATH pwd`
parent=`dirname $here`

echo 'Creating symlink daemontools -> daemontools-0.76...'
rm -f daemontools
ln -s daemontools-0.76 daemontools
mv -f daemontools ..

echo 'Making command links in /command...'
mkdir -p /command
for i in `cat package/commands`
do
  rm -f /command/$i'{new}'
  ln -s $parent/daemontools/command/$i /command/$i'{new}'
  mv -f /command/$i'{new}' /command/$i
done
echo 'Making compatibility links in /usr/local/bin...'
mkdir -p /usr/local/bin
for i in `cat package/commands`
do
  rm -f /usr/local/bin/$i'{new}'
  ln -s /command/$i /usr/local/bin/$i'{new}'
  mv -f /usr/local/bin/$i'{new}' /usr/local/bin/$i
done
      working-directory: 
#include "MoveCommand.h"

static const std::string _name("move");
static const std::string _description =
    "Arguments: {column_index}{row_index}\n"
    "Example: " +
    _name + " e4\n"
            "Description: Moves the selected piece to the specified square if legal.";

MoveCommand::MoveCommand(Chess &chess)
    : Command(chess, _name, _description)
{
}

Result MoveCommand::apply(const std::vector<std::string> &params)
{
    if (params.size() != 1 || params[0].length() != 2)
        return {true, false};

    int row = params[0][1] - '1';
    int column = params[0][0] - 'a';

    Chess &chess = this->get_chess();
    bool error = chess.move({row, column});
    return {error, false};
}
      run: #pragma once

#include "../Command.h"

// Moves the selected piece to a new square.
// Params:
//      1. string of format {char}{int} representing a
//          field of the chessboard.
class MoveCommand : public Command
{
public:
    MoveCommand(Chess &chess);

    Result apply(const std::vector<std::string> &params) override;
};

    - name: 
#include "QuitCommand.h"

static const std::string _name = "quit";
static const std::string _description =
    "Arguments: [None]\n"
    "Description: Quits the game.";

QuitCommand::QuitCommand(Chess &chess)
    : Command(chess, _name, _description)
{
}

Result QuitCommand::apply(const std::vector<std::string> &params)
{
    bool error = !params.empty();
    bool quit = true;
    return {error, quit};
}
      working-directory: #pragma once

#include "../Command.h"

// Sends a quit signal to the session.
// No params.
class QuitCommand : public Command
{
public:
    QuitCommand(Chess &chess);

    Result apply(const std::vector<std::string> &params) override;
};
      # Add additional options to the MSBuild command line here (like platform or verbosity level).
      # See https://docs.microsoft.com/visualstudio/msbuild/msbuild-command-line-reference
      run: 
#include "SelectCommand.h"

static const std::string _name("select");
static const std::string _description =
    "Arguments: {column_index}{row_index}\n"
    "Example: " +
    _name + " e2\n"
            "Description: Selects the specified square.";

SelectCommand::SelectCommand(Chess &chess)
    : Command(chess, _name, _description)
{
}

Result SelectCommand::apply(const std::vector<std::string> &params)
{
    if (params.size() != 1 || params[0].length() != 2)
        return {true, false};

    int row = params[0][1] - '1';
    int column = params[0][0] - 'a';

    Chess &chess = this->get_chess();
    bool error = chess.select({row, column});
    return {error, false};
} ${{ #pragma once

#include "../Command.h"

// Selects a square.
// Params:
//     1. row
//     2. column
class SelectCommand : public Command
{
public:
    SelectCommand(Chess &chess);

    Result apply(const std::vector<std::string> &params) override;
}; 
