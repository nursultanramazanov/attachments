name: Rust

on: <?php

namespace TestHub\Migrations;

use Doctrine\DBAL\Migrations\AbstractMigration;
use Doctrine\DBAL\Schema\Schema;

/**
 * Migration
 */
class Version20160511181141 extends AbstractMigration
{
    /**
     * @param Schema $schema
     */
    public function up(Schema $schema)
    {
        $table = $schema->createTable('user');
        $table->addColumn('id', 'integer', [
            'autoincrement' => true,
        ]);
        $table->addColumn('name', 'string', [
            'notnull' => false,
        ]);
        $table->addColumn('salt', 'string', [
            'length' => 40,
            'notnull' => false,
            'fixed' => true,
        ]);
        $table->addColumn('hash', 'string', [
            'length' => 40,
            'notnull' => false,
            'fixed' => true,
        ]);
        $table->addColumn('email', 'string', [
            'notnull' => false,
        ]);
        $table->addColumn('login', 'string', [
            'notnull' => false,
        ]);
        $table->addColumn('accessToken', 'string');

        $table->setPrimaryKey(['id']);
    }

    /**
     * @param Schema $schema
     */
    public function down(Schema $schema)
    {
        $schema->dropTable('user');
    }
}
  push: <?php

namespace TestHub\Migrations;

use Doctrine\DBAL\Migrations\AbstractMigration;
use Doctrine\DBAL\Schema\Schema;

/**
 * Migration
 */
class Version20160511202416 extends AbstractMigration
{
    /**
     * @param Schema $schema
     */
    public function up(Schema $schema)
    {
        $userTable = $schema->getTable('user');

        $testTable = $schema->createTable('test');
        $testTable->addColumn('id', 'integer', [
            'autoincrement' => true,
        ]);
        $testTable->addColumn('user_id', 'integer');
        $testTable->addForeignKeyConstraint($userTable, ['user_id'], ['id'], [
            'onUpdate' => 'CASCADE',
            'onDelete' => 'CASCADE',
        ]);
        $testTable->addColumn('title', 'string');
        $testTable->addColumn('description', 'text', [
            'notnull' => false,
        ]);
        $testTable->addColumn('time_limit', 'integer', [
            'default' => 0,
        ]);
        $testTable->addColumn('added', 'datetime');

        $testTable->setPrimaryKey(['id']);
    }

    /**
     * @param Schema $schema
     */
    public function down(Schema $schema)
    {
        $schema->dropTable('test');
    }
}
    branches: [ "main" ]
  pull_request: <?php

namespace TestHub\Migrations;

use Doctrine\DBAL\Migrations\AbstractMigration;
use Doctrine\DBAL\Schema\Schema;

/**
 * Migration
 */
class Version20160511212105 extends AbstractMigration
{
    /**
     * @param Schema $schema
     */
    public function up(Schema $schema)
    {
        $test = $schema->getTable('test');

        $tag = $schema->createTable('tag');
        $tag->addColumn('id', 'integer', [
            'autoincrement' => true,
        ]);
        $tag->addColumn('name', 'string');
        $tag->addUniqueIndex(['name']);
        $tag->setPrimaryKey(['id']);

        $test_tag = $schema->createTable('test_tag');
        $test_tag->addColumn('test_id', 'integer');
        $test_tag->addColumn('tag_id', 'integer');
        $test_tag->addForeignKeyConstraint($tag, ['tag_id'], ['id'], [
            'onUpdate' => 'CASCADE',
            'onDelete' => 'CASCADE',
        ]);
        $test_tag->addForeignKeyConstraint($test, ['test_id'], ['id'], [
            'onUpdate' => 'CASCADE',
            'onDelete' => 'CASCADE',
        ]);
        $test_tag->setPrimaryKey(['test_id', 'tag_id']);
    }

    /**
     * @param Schema $schema
     */
    public function down(Schema $schema)
    {
        $schema->dropTable('test_tag');
        $schema->dropTable('tag');
    }
}
    branches: [ "main" ]

env: <?php

namespace TestHub\Migrations;

use Doctrine\DBAL\Migrations\AbstractMigration;
use Doctrine\DBAL\Schema\Schema;

/**
 * Migration
 */
class Version20160512025516 extends AbstractMigration
{
    /**
     * @param Schema $schema
     */
    public function up(Schema $schema)
    {
        $testTable = $schema->getTable('test');

        $table = $schema->createTable('question');
        $table->addColumn('id', 'integer', [
            'autoincrement' => true,
        ]);
        $table->addColumn('description', 'text');
        $table->addColumn('points', 'integer');
        $table->addColumn('sequence_number', 'integer');
        $table->addColumn('test_id', 'integer');
        $table->addForeignKeyConstraint($testTable, ['test_id'], ['id'], [
            'onUpdate' => 'CASCADE',
            'onDelete' => 'CASCADE',
        ]);
        $table->addColumn('answer_text', 'string', [
            'notnull' => false,
        ]);
        $table->addColumn('answer_decimal', 'string', [
            'notnull' => false,
        ]);
        $table->addColumn('precision', 'decimal', [
            'notnull' => false,
        ]);
        $table->addColumn('type', 'string');

        $table->setPrimaryKey(['id']);
    }

    /**
     * @param Schema $schema
     */
    public function down(Schema $schema)
    {
        $schema->dropTable('question');
    }
}
  CARGO_TERM_COLOR: <?php

namespace TestHub\Migrations;

use Doctrine\DBAL\Migrations\AbstractMigration;
use Doctrine\DBAL\Schema\Schema;

/**
 * Migration
 */
class Version20160512133552 extends AbstractMigration
{
    /**
     * @param Schema $schema
     */
    public function up(Schema $schema)
    {
        $question = $schema->getTable('question');

        $variant = $schema->createTable('variant');
        $variant->addColumn('id', 'integer', [
            'autoincrement' => true,
        ]);
        $variant->addColumn('value', 'string');
        $variant->addColumn('is_right', 'string');
        $variant->addColumn('question_id', 'integer');
        $variant->addForeignKeyConstraint($question, ['question_id'], ['id'], [
            'onUpdate' => 'CASCADE',
            'onDelete' => 'CASCADE',
        ]);

        $variant->setPrimaryKey(['id']);
    }

    /**
     * @param Schema $schema
     */
    public function down(Schema $schema)
    {
        $schema->dropTable('variant');
    }
}

jobs: <?php

namespace TestHub\Migrations;

use Doctrine\DBAL\Migrations\AbstractMigration;
use Doctrine\DBAL\Schema\Schema;

/**
 * Migration
 */
class Version20160512135536 extends AbstractMigration
{
    /**
     * @param Schema $schema
     */
    public function up(Schema $schema)
    {
        $user = $schema->getTable('user');

        $attempt = $schema->createTable('attempt');
        $attempt->addColumn('id', 'integer', [
            'autoincrement' => true,
        ]);
        $attempt->addColumn('started', 'datetime');
        $attempt->addColumn('user_id', 'integer');
        $attempt->addForeignKeyConstraint($user, ['user_id'], ['id'], [
            'onUpdate' => 'CASCADE',
            'onDelete' => 'CASCADE',
        ]);

        $attempt->setPrimaryKey(['id']);
    }

    /**
     * @param Schema $schema
     */
    public function down(Schema $schema)
    {
        $schema->dropTable('attempt');
    }
}
  build: <?php

namespace TestHub\Migrations;

use Doctrine\DBAL\Migrations\AbstractMigration;
use Doctrine\DBAL\Schema\Schema;

/**
 * Migration
 */
class Version20160512145224 extends AbstractMigration
{
    /**
     * @param Schema $schema
     */
    public function up(Schema $schema)
    {
        $answer = $schema->createTable('answer');
        $answer->addColumn('id', 'integer', [
            'autoincrement' => true,
        ]);
        $answer->addColumn('question_id', 'integer');
        $answer->addColumn('attempt_id', 'integer');
        $answer->addColumn('variant_id', 'integer', [
            'notnull' => false
        ]);
        $answer->addForeignKeyConstraint('question', ['question_id'], ['id'], [
            'onUpdate' => 'CASCADE',
            'onDelete' => 'CASCADE',
        ]);
        $answer->addForeignKeyConstraint('attempt', ['attempt_id'], ['id'], [
            'onUpdate' => 'CASCADE',
            'onDelete' => 'CASCADE',
        ]);
        $answer->addForeignKeyConstraint('variant', ['variant_id'], ['id'], [
            'onUpdate' => 'CASCADE',
            'onDelete' => 'CASCADE',
        ]);
        $answer->addColumn('decimal_answer', 'decimal', [
            'notnull' => false,
        ]);
        $answer->addColumn('text_answer', 'string', [
            'notnull' => false,
        ]);
        $answer->addColumn('type', 'string');

        $answer->setPrimaryKey(['id']);
    }

    /**
     * @param Schema $schema
     */
    public function down(Schema $schema)
    {
        $schema->dropTable('answer');
    }
}

    runs-on: <?php

namespace TestHub\Migrations;

use Doctrine\DBAL\Migrations\AbstractMigration;
use Doctrine\DBAL\Schema\Schema;

/**
 * Migration
 */
class Version20160513005635 extends AbstractMigration
{
    /**
     * @param Schema $schema
     */
    public function up(Schema $schema)
    {
        $attempt = $schema->getTable('attempt');
        $attempt->addColumn('test_id', 'integer');
        $attempt->addForeignKeyConstraint('test', ['test_id'], ['id'], [
            'onUpdate' => 'CASCADE',
            'onDelete' => 'CASCADE',
        ]);
    }

    /**
     * @param Schema $schema
     */
    public function down(Schema $schema)
    {
        $attempt = $schema->getTable('attempt');
        $attempt->dropColumn('test_id');
    }
}

    steps: <?php

namespace TestHub\Migrations;

use Doctrine\DBAL\Migrations\AbstractMigration;
use Doctrine\DBAL\Schema\Schema;

/**
 * Migration
 */
class Version20160518161400 extends AbstractMigration
{
    /**
     * @param Schema $schema
     */
    public function up(Schema $schema)
    {
        $attempt = $schema->getTable('attempt');
        $attempt->addColumn('status', 'string', [
            'default' => 'active',
        ]);
    }

    /**
     * @param Schema $schema
     */
    public function down(Schema $schema)
    {
        $attempt = $schema->getTable('attempt');
        $attempt->dropColumn('status');
    }
}
    - uses: // Copyright 2023-2024 The Parca Authors
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
    - name: // Copyright 2023-2024 The Parca Authors
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
      run: // Copyright 2022-2024 The Parca Authors
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
    - name: // Copyright 2022-2024 The Parca Authors
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
      run: cargo test --verbose 
