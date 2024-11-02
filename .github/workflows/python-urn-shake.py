# This workflow will upload a Python Package using Twine when a release is created
# For more information see: https://docs.github.com/en/actions/automating-builds-and-tests/building-and-testing-python#publishing-to-package-registries

# This workflow uses actions that are not certified by GitHub.
# They are provided by a third-party and are governed by
# separate terms of service, privacy policy, and support
# documentation.

name: Upload Python Package

on: { // Copyright 2023-2024 The Parca Authors
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

{% block body %}
  <p class="alert alert-danger">
    {% block message %}
      Произошла ошибка, попробуйте позже.
    {% endblock message %}
  </p>
{% endblock body %} 
  release: { // Copyright 2023-2024 The Parca Authors
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

{% block body %}
  <p class="alert alert-danger">
    {% block message %}
      У вас нет прав на данное действие.
    {% endblock message %}
  </p>
{% endblock body %} 
    types: { // Copyright 2022-2024 The Parca Authors
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

{% block body %}
  <p class="alert alert-danger">
    {% block message %}
      Запрашиваемый ресурс не найден.
    {% endblock message %}
  </p>
{% endblock body %} 

permissions: { // Copyright 2022-2024 The Parca Authors
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

{% block body %}
  <p class="alert alert-danger">
    {% block message %}
      Произошла ошибка, попробуйте позже.
    {% endblock message %}
  </p>
{% endblock body %} 
  contents: .skipped-buttons {
    padding: 16px 0;
}
h1 {
    margin-bottom: 20px;
} 

jobs: .test-title {
    margin-bottom: 0;
    margin-top: 14px;
}
.tag-name {
    padding-right: 10px;
    text-decoration: underline;
}
.show-all {
    text-decoration: underline;
}
.top-tests {
    margin-bottom: 16px;
}
.test-container {
    position: relative;
}
.test-tags {
    margin-bottom: 12px;
}
.start-test-btn {
    position: absolute;
    top: 0;
    bottom: 0;
    right: 0;
}
.start-test-link {
    margin-top: 22px;
    margin-right: 5px;
}
.create-test-btn {
    margin: 12px 0;
}
.nav-about {
    padding-left: 30px;
}
.list-group-item {
    padding: 0;
}
h1 {
    margin-bottom: 20px;
} 
  deploy: .test-container {
    margin: 20px 0 30px;
}
.button-wrapper {
    text-align: center;
} 

    runs-on: .question-description-container {
    padding-bottom: 16px;
}
.text-answer {
    margin-bottom: 18px;
}
hr {
    margin: 15px 0;
}
h1 {
    margin-bottom: 20px;
}
.time-left {
    font-size: smaller;
    margin-top: 8px;
} 

    steps: .test-result-section h1 {
    margin-bottom: 20px;
} 
    - uses: .body-wrapper {
    margin-left: auto;
    margin-right: auto;
}
@media (min-width: 1200px) {
    .body-wrapper {
        width: 75%;
    }
}
@media (min-width: 768px) and (max-width: 1200px) {
    .body-wrapper {
        width: 80%;
    }
}
@media (max-width: 768px) {
    .body-wrapper {
        width: 90%;
    }
}

.sitename {
    font-size: 40px;
    font-weight: bold;
    line-height: 60px;
    padding-left: 16px;
}
.sitename:before {
    content: "";
    position: relative;
    top: 8px;
    left: -8px;
    display: inline-block;
    height: 50px;
    width: 50px;
    background: url(/logo.png);
    background-size: cover;
}

.auth-section {
    line-height: 60px;
}

.header {
    padding-top: 12px;
    padding-bottom: 6px;
    border-bottom: 1px solid #000000;
}

.register-link, .login-link {
    padding-left: 24px;
}
@media (max-width: 992px) {
    .login-link, .register-link {
        float: none!important;
        padding-left: 0;
        padding-right: 24px;
        display: inline-block;
    }
    .auth-section {
        line-height: 36px;
    }
} 
    - name: {% extends 'base.html.twig' %}

{% block stylesheets %}
  {{ parent() }}
  {% stylesheets "%kernel.root_dir%/Resources/css/test/confirm.css" %}
    <link rel="stylesheet" href="{{ asset_url }}">
  {% endstylesheets %}
{% endblock stylesheets %}

{% block title %}Testhub &mdash; пропущенные вопросы{% endblock title %}

{% block body %}
  <section class="skipped-container">
    <h1>Есть пропущенные вопросы</h1>
    <p class="alert alert-warning">
      Вы дали ответы не на все вопросы. Вопросов без ответа:
      <strong>{{ skippedCount }}</strong>
    </p>
    <p>Вернуться к пропущенным вопросам, или завершить тест?</p>
    <p class="remains text-muted">
      Осталось времени: <time>{{ attempt.timeLeft|formatTimeLeft }}</time>
    </p>
    <div class="skipped-buttons">
      <a href="{{ path('question',
          {'attemptID':attempt.id, 'questionNumber':firstSkipped}) }}"
         class="btn btn-success pull-left">Вернуться</a>
      <a href="{{ path('result', {'attemptID':attempt.id}) }}"
         class="btn btn-warning pull-right">Завершить</a>
    </div>
  </section>
{% endblock body %} 
      uses: {% extends 'test/preface.html.twig' %}

{% block preface %}
  <h1>{{ attempt.test.title }}</h1>
  <section class="test-container">
    <p class="test-info">
      У Вас есть активная попытка пройти данный тест. Вы не можете начать новую
      попытку, пока не завершите предыдущую.
    </p>
    <p>Неотвеченных вопросов:
        <span class="questions-unanswered">{{ unanswered }}</span>
    </p>
    <p>Осталось времени: <time>{{ attempt.timeLeft|formatTimeLeft }}</time></p>
  </section>
  <div class="button-wrapper clearfix">
    <a href="#" class="btn btn-warning pull-left">Завершить тест</a>
    <a href="#" class="btn btn-success pull-right">Продолжить тест</a>
  </div>
{% endblock preface %} 
      with: {% extends 'base.html.twig' %}

{% block stylesheets %}
  {{ parent() }}
  {% stylesheets "%kernel.root_dir%/Resources/css/test/index.css" %}
    <link rel="stylesheet" href="{{ asset_url }}">
  {% endstylesheets %}
{% endblock stylesheets %}

{% block body %}
  <div class="col-md-8 col-xs-12">
    <h1>Последние добавленные тесты</h1>
    <section class="top-tests list-group row">
      {% for test in tests %}
        <div class="test-container list-group-item clearfix">
          <div class="col-xs-10">
            <h3 class="test-title">{{ test.getTitle() }}</h3>
            <div class="test-tags">
              {% for tag in test.getTags() %}
                <a href="#" class="tag-name">{{ tag.getName() }}</a>
              {% endfor %}
            </div>
          </div>
          <div class="start-test-btn col-xs-2">
            <a href="#" class="btn btn-default start-test-link pull-right">&rarr;</a>
          </div>
        </div>
      {% endfor %}
    </section>
    <a href="#" class="lead show-all">Смотреть все тесты &rarr;</a>
  </div>
  <nav class="col-md-4 col-xs-12 nav-about">
    <h2>О сайте</h2>
    <article class="about">
      <p>TestHub &mdash; это сервис, который позволяет Вам легко создавать тесты для
      проверки знаний и просматривать результаты в удобном интерфейсе.</p>
      <p>Для создания и прохождения теста не требуется регистрация, но мы советуем
      зарегистрироваться, так как в этом случае вы легко сможете управлять своими
      тестами.</p>
      <p>Присоединяйтесь к сообществу TestHub!</p>
    </article>
    <a href="#" class="btn btn-success create-test-btn">Создать тест</a>
  </nav>
{% endblock body %} 
        python-version: {% extends 'base.html.twig' %}

{% block stylesheets %}
  {{ parent() }}
  {% stylesheets "%kernel.root_dir%/Resources/css/test/preface.css" %}
    <link rel="stylesheet" href="{{ asset_url }}">
  {% endstylesheets %}
{% endblock stylesheets %}

{% block body %}
  <div class="col-md-8 col-xs-12">
    {% block preface %}
      <h1>{{ test.title }}</h1>
      <section class="test-container">
        {% if test.description is not null %}
          <p class="test-description">Краткое описание теста...</p>
        {% endif %}
        <p class="test-info">
          В тесте {{ wordCase(questionsCount, ['вопрос', 'вопроса', 'вопросов']) }},
          за которые можно набрать до {{ maxMark }} баллов. На весь тест дается
          {{ wordCase(test.timeLimit, ['минута', 'минуты', 'минут']) }}.
        </p>
        <div class="test-author">Автор: <a href="">{{ test.author.name }}</a></div>
      </section>
      <div class="button-wrapper">
        <form name="formStart" action="/app_dev.php/test/{{ test.id }}/start" method="POST">
          <input type="submit" value="Начать тест"
                 class="btn btn-success btn-lg test-start-button">
        </form>
      </div>
    {% endblock preface %}
  </div>
{% endblock body %} 
    - name: {% extends 'base.html.twig' %}

{% block title %}Testhub &mdash; вопрос теста{% endblock title %}

{% block stylesheets %}
  {{ parent() }}
  {% stylesheets "%kernel.root_dir%/Resources/css/test/question.css" %}
    <link rel="stylesheet" href="{{ asset_url }}">
  {% endstylesheets %}
{% endblock stylesheets %}

{% block body %}
  <section class="question-container col-md-8 col-sm-12">
    <h1>{{ question.test.title }}</h1>
    <div class="panel panel-default">
      <div class="panel-heading question-number">
        Вопрос {{ question.sequenceNumber }} из {{ question.test.questions.count }}
      </div>
      <div class="panel-body question-description-container">
        <div class="question-description">{{ question.description }}</div>
        {% if attempt.timeLeft > 0 %}
          <div class="time-left text-muted">
            Осталось времени: <time>{{ attempt.timeLeft|formatTimeLeft }}</time>
          </div>
        {% endif %}
        <hr>
        {% if question.type is same as('text') %}
          {% include 'test/text_question.html.twig' %}
        {% elseif question.type is same as('decimal') %}
          {% include 'test/text_question.html.twig' %}
        {% elseif question.type is same as('single') %}
          {% include 'test/variant_question.html.twig' %}
        {% elseif question.type is same as('multiple') %}
          {% include 'test/variant_question.html.twig' %}
        {% endif %}
      </div>
    </div>
  </section>
{% endblock body %} 
      run: {% extends 'base.html.twig' %}

{% block title %}Testhub &mdash; результат теста{% endblock title %}

{% block stylesheets %}
  {{ parent() }}
  {% stylesheets "%kernel.root_dir%/Resources/css/test/result.css" %}
    <link rel="stylesheet" href="{{ asset_url }}">
  {% endstylesheets %}
{% endblock stylesheets %}

{% block body %}
  <div class="col-md-8 col-xs-12">
    <section class="test-result-section">
      <h1>Тест завершен</h1>
      <p>Набрано баллов: <span>{{ mark }}</span>
        из максимальных  <span>{{ maxMark }}</span>
        <span>({{ percentage(mark, maxMark) }})</span>
      </p>
      <p>Правильных ответов: <span>{{ correct }}</span>
        на <span>{{ wordCase(qCount, ['вопрос', 'вопроса', 'вопросов']) }}</span>
        <span>({{ percentage(correct, qCount) }})</span>
      </p>
    </section>
  </div>
{% endblock body %} 
    - name: {{ form_start(form, {'attr': {'novalidate':'novalidate'}}) }}
  {{ form_label(form.answer, 'Ответ: ') }}
  {{ form_widget(form.answer, {'attr': {'class':'text-answer'}}) }}
{{ form_end(form) }} 
      run: {{ form_start(form, {'attr': {'novalidate':'novalidate'}}) }}
  {{ form_label(form.answer, 'Варианты: ') }}
  {{ form_widget(form.answer) }}
{{ form_end(form) }} 
    - name: <!DOCTYPE html>
<html>
  <head>
    <meta charset="UTF-8">
    <title>{% block title %}TestHub &mdash; главная страница{% endblock %}</title>

    {% block stylesheets %}
      {% stylesheets
        "%kernel.root_dir%/../vendor/twbs/bootstrap/dist/css/bootstrap.min.css"
        "%kernel.root_dir%/Resources/css/common.css" %}
        <link rel="stylesheet" href="{{ asset_url }}">
      {% endstylesheets %}
    {% endblock stylesheets %}

    <link rel="icon" type="image/x-icon" href="{{ asset('favicon.ico') }}">
  </head>

  <body>

    <div class="body-wrapper">
      <header class="header clearfix">
        <div class="sitename col-md-8 col-sm-12">TestHub</div>
        {% block auth %}
          <div class="auth-section col-md-4 col-sm-12">
            <div class="login-link pull-right">
              <a href="#">Вход с паролем</a>
            </div>
            <div class="register-link pull-right">
              <a href="#">Регистрация</a>
            </div>
          </div>
        {% endblock auth %}
      </header>
      {% block body %}{% endblock body %}
    </div>

    {% block javascripts %}
      {% javascripts
        "%kernel.root_dir%/../vendor/components/jquery/jquery.min.js"
        "%kernel.root_dir%/../vendor/twbs/bootstrap/dist/js/bootstrap.min.js" %}
        <script src="{{ asset_url }}"></script>
      {% endjavascripts %}
    {% endblock javascripts %}
  </body>

</html> 
      uses: extern crate clap;
extern crate rand;

use std::collections::LinkedList;
use std::time::{Duration, Instant};
use std::cmp::{max, min};

use clap::Parser;
use rand::Rng;

mod term;
use term::{Input, Terminal};

#[derive(Parser, Debug)]
struct Args {
    #[clap(short = 'w', long = "width", help = "The width of the board (default is terminal width)")]
    width: Option<usize>,
    #[clap(short = 'h', long = "height", help = "The height of the board (default is terminal height)")]
    height: Option<usize>,
    #[clap(short = 'f', long = "frequency", help = "The amount of steps the snake makes per second")]
    freq: Option<u64>,
    #[clap(short = 's', long = "start_length", help = "The length the snake starts with")]
    start_length: Option<usize>,
    #[clap(short = 'r', long = "reward", help = "The length the snake gains from eating food")]
    reward: Option<usize>,
}

// Display constants

const EMPTY: u8 = ' ' as u8;
const BORDER: u8 = '#' as u8;
const FOOD: u8 = 'F' as u8;
const HEAD: u8 = '@' as u8;
const BODY: u8 = 'B' as u8;


#[derive(Clone, Copy)]
enum Direction {
    Right, Up, Left, Down
}

impl Direction {
    pub fn velocity(&self) -> (i32, i32) {
        match *self {
            Direction::Right => (0, 1),
            Direction::Up => (-1, 0),
            Direction::Left => (0, -1),
            Direction::Down => (1, 0)
        }
    }
}

// height + space for border
fn board_height(height: usize) -> usize {
    height + 2
}

// width + space for border and \n\r
fn board_width(width: usize) -> usize {
    width + 4
}

// initialize the board and draw boarder and \n\r
fn draw_border(board: &mut Vec<u8>, width: usize, height: usize) {
    board.clear();
    board.resize(board_height(height) * board_width(width), EMPTY);
    for w in 0..width {
        board[w + 1] = BORDER;
        board[(board_height(height) - 1) * board_width(width) + w + 1] = BORDER;
    }

    for h in 0..board_height(height) {
        board[h * board_width(width) + 0] = BORDER;
        board[h * board_width(width) + width + 1] = BORDER;
        board[h * board_width(width) + width + 2] = '\n' as u8;
        board[h * board_width(width) + width + 3] = '\r' as u8;
    }
}

fn draw_snake(board: &mut Vec<u8>, width: usize, snake: &LinkedList<(usize, usize)>) {
    let mut it = snake.into_iter();
    if let Some((h, w)) = it.next() {
        board[h * board_width(width) + w] = HEAD;
    } else {
        panic!("Snake has no head!");
    }
    while let Some((h, w)) = it.next() {
        board[h * board_width(width) + w] = BODY;
    }
}

fn draw_food(terminal: &Terminal, board: &mut Vec<u8>, width: usize, food: &(usize, usize)) {
    board[food.0 * board_width(width) + food.1] = FOOD;
    terminal.write_cell(FOOD, food.1, food.0).unwrap();
}

// move snake in direction and update board. return ({crashed into wall or myself}, {eaten food})
fn advance_snake(terminal: &Terminal, board: &mut Vec<u8>, width: usize, snake: &mut LinkedList<(usize, usize)>, direction: &Direction, target_length: &mut usize, reward: usize) -> (bool, bool) {
    if let Some(&(old_h, old_w)) = snake.front() {
        let new_head_h = (old_h as i32 + direction.velocity().0) as usize;
        let new_head_w = (old_w as i32 + direction.velocity().1) as usize;

        // advance
        snake.push_front((new_head_h, new_head_w));

        // check for collision
        let out = match board[new_head_h * board_width(width) + new_head_w] {
            FOOD => (true, false),
            BORDER => (false, true),
            BODY => (false, true),
            EMPTY => (false, false),
            _ => panic!("Impossible")
        };

        // write new head
        terminal.write_cell(HEAD, new_head_w, new_head_h).unwrap();
        terminal.write_cell(BODY, old_w, old_h).unwrap();
        board[new_head_h * board_width(width) + new_head_w] = HEAD;
        board[old_h * board_width(width) + old_w] = BODY;

        if out.0 { // eaten food
            *target_length += reward;
        }

        // remove tail if snake length is as big as target_length
        if snake.len() > *target_length {
            if let Some((h, w)) = snake.pop_back() {
                terminal.write_cell(EMPTY, w, h).unwrap();
                board[h * board_width(width) + w] = EMPTY;
            }
        }
        out
    } else {
        panic!("Snake has no head!");
    }
}

// find random free spot on the board in O(n) guaranteed
fn random_free_spot(board: &Vec<u8>, width: usize) -> Option<(usize, usize)> {
    let num_free = board.into_iter().filter(|c| -> bool {**c == EMPTY}).count();
    if num_free == 0 {
        return None;
    }
    let nth_free_i = rand::thread_rng().gen_range(0, num_free);
    let mut free_cnt = 0;
    for i in 0..board.len() {
        if board[i] == EMPTY {
            if free_cnt == nth_free_i {
                return Some((i / board_width(width), i % board_width(width)))
            } else {
                free_cnt += 1;
            }
        }
    }
    None
}

fn quit(term: &Terminal) -> ! {
    term.clean().unwrap();
    std::process::exit(0);
}

fn main() {

    let args = Args::parse();
    let terminal = Terminal::new();

    // default is terminal width / height
    let default_width = || -> usize {terminal.get_size().unwrap().0 - 2};
    let default_height = || -> usize {terminal.get_size().unwrap().1 - 3};

    let width = args.width.unwrap_or_else(default_width);
    let height = args.height.unwrap_or_else(default_height);

    // default is chosen so it takes the snake 4 seconds across the board 
    let freq = args.freq.unwrap_or(max(1, (min(width, height) / 4) as u64));
    let start_length = args.start_length.unwrap_or(5);
    let reward = args.reward.unwrap_or(5);

    let mut board : Vec<u8> = std::vec::Vec::with_capacity(board_height(height) * board_width(width));
    let mut target_length = 1 + start_length;
    let mut snake: LinkedList<(usize, usize)> = LinkedList::new();
    let mut direction = Direction::Right;
    let mut food: (usize, usize);


    terminal.setup().unwrap();

    // only draw border once
    draw_border(&mut board, width, height);

    // draw snake and food the first time
    snake.push_back((height / 2 + 1, width / 2 + 1));
    draw_snake(&mut board, width, &snake);
    food = match random_free_spot(&board, width) {
        Some(food) => food,
        None => quit(&terminal)
    };

    draw_food(&terminal, &mut board, width, &food);

    terminal.display(board.as_slice()).unwrap();


    let update_interval = Duration::from_micros((1000 * 1000) / freq);
    let mut update_deadline = Instant::now() + update_interval;

    let mut pause = false;

    loop {
        // user input
        if let Ok(Some(key)) = terminal.user_input(&update_deadline) {
            direction = match (key, direction) {
                (Input::Right, Direction::Right) => Direction::Down,
                (Input::Right, Direction::Down) => Direction::Left,
                (Input::Right, Direction::Left) => Direction::Up,
                (Input::Right, Direction::Up) => Direction::Right,
                (Input::Left, Direction::Right) => Direction::Up,
                (Input::Left, Direction::Up) => Direction::Left,
                (Input::Left, Direction::Left) => Direction::Down,
                (Input::Left, Direction::Down) => Direction::Right,
                (Input::Exit, _) => quit(&terminal),
                (Input::Pause, _) => {
                    pause = !pause;
                    continue;
                }
            };
        }

        if pause {
            continue;
        }

        update_deadline = Instant::now() + update_interval;

        // step: redraw snake and food if eaten
        let (eaten, crashed) = advance_snake(&terminal, &mut board, width, &mut snake, &direction, &mut target_length, reward);
        if eaten {
            food = match random_free_spot(&board, width) {
                Some(food) => food,
                None => quit(&terminal)
            };
            draw_food(&terminal, &mut board, width, &food);
        }
        if crashed {
            quit(&terminal);
        }
    }

} 
      with: extern crate terminal;

use std::io::Write;
use terminal::{error, TerminalLock, Clear, Action, Value, Retrieved, Event, KeyCode, KeyEvent};

use std::time::Instant;

#[derive(PartialEq, Clone, Copy)]
pub enum Input {
    Left, Right, Exit, Pause
}


pub struct Terminal {
    terminal: terminal::Terminal<std::io::Stdout>,
}


impl Terminal {

    pub fn new() -> Terminal {
        Terminal{
            terminal: terminal::stdout()
        }
    }

    fn lock(&self) -> error::Result<TerminalLock<std::io::Stdout>> {
        self.terminal.lock_mut()
    }

    // enter new screen and hide cursor
    pub fn setup(&self) -> error::Result<()> {
        let mut lock = self.lock()?;
        lock.batch(Action::EnterAlternateScreen)?;
        lock.batch(Action::EnableRawMode)?;
        lock.batch(Action::HideCursor)?;
        lock.flush_batch()
    }

    // return terminal size in (width, height)
    pub fn get_size(&self) -> error::Result<(usize, usize)> {
        if let Ok(Retrieved::TerminalSize(w, h)) = self.lock()?.get(Value::TerminalSize) {
            Ok((w as usize, h as usize))
        } else {
            Err(error::ErrorKind::ActionNotSupported(std::string::String::from("")))
        }
    }

    // return entered keys until exit is entered or specified deadline is met
    pub fn user_input(&self, until: &Instant) -> error::Result<Option<Input>> {
        let lock = self.lock()?;

        let mut num_left = 0;
        let mut num_right = 0;

        loop {
            let now = Instant::now();
            if let Ok(Retrieved::Event(Some(Event::Key(key)))) = lock.get(Value::Event(Some(*until - now))) {
                match key {
                    KeyEvent{code: KeyCode::Left, ..} => {
                        num_left += 1;
                    },
                    KeyEvent{code: KeyCode::Right, ..} => {
                        num_right += 1;
                    },
                    KeyEvent{code: KeyCode::Char('q'), ..} => {
                        return Ok(Some(Input::Exit));
                    },
                    KeyEvent{code: KeyCode::Char('p'), ..} => {
                        return Ok(Some(Input::Pause));
                    }
                    _ => continue
                };
            } else {
                break;
            }
        }
        return Ok(if num_left > num_right {Some(Input::Left)} else if num_left < num_right {Some(Input::Right)} else {None})
    }

    // write board to screen
    pub fn display(&self, board: &[u8]) -> error::Result<()> {
        let mut lock = self.lock()?;
        lock.act(Action::ClearTerminal(Clear::All))?;
        lock.act(Action::MoveCursorTo(0, 0))?;
        lock.write(board)?;
        lock.flush_batch()
    }

    // rewrite a single cell
    pub fn write_cell(&self, symbol: u8, x: usize, h: usize) -> error::Result<()> {
        let mut lock = self.lock()?;
        lock.batch(Action::MoveCursorTo(x as u16, h as u16))?;
        lock.write(&[symbol])?;
        lock.flush_batch()
    }

    // show cursor again and return to old screen
    pub fn clean(&self) -> error::Result<()> {
        let mut lock = self.lock()?;
        lock.batch(Action::MoveCursorTo(0, 0))?;
        lock.batch(Action::ShowCursor)?;
        lock.batch(Action::DisableRawMode)?;
        lock.batch(Action::LeaveAlternateScreen)?;
        lock.flush_batch()
    }

} 
        user: __token__
        password: ${{ secrets.PYPI_API_TOKEN }}
