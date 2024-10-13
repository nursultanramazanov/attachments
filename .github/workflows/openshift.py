# This workflow uses actions that are not certified by GitHub.
# They are provided by a third-party and are governed by
# separate terms of service, privacy policy, and support
# documentation.

# 💁 The OpenShift Starter workflow will:
# - Checkout your repository
# - Perform a container image build
# - Push the built image to the GitHub Container Registry (GHCR)
# - Log in to your OpenShift cluster
# - Create an OpenShift app from the image and expose it to the internet

# ℹ️ Configure your repository and the workflow with the following steps:
# 1. Have access to an OpenShift cluster. Refer to https://www.openshift.com/try
# 2. Create the OPENSHIFT_SERVER and OPENSHIFT_TOKEN repository secrets. Refer to:
#   - https://github.com/redhat-actions/oc-login#readme
#   - https://docs.github.com/en/actions/reference/encrypted-secrets
#   - https://cli.github.com/manual/gh_secret_set
# 3. (Optional) Edit the top-level 'env' section as marked with '🖊️' if the defaults are not suitable for your project.
# 4. (Optional) Edit the build-image step to build your project.
#    The default build type is by using a Dockerfile at the root of the repository,
#    but can be replaced with a different file, a source-to-image build, or a step-by-step buildah build.
# 5. Commit and push the workflow file to your default branch to trigger a workflow run.

# 👋 Visit our GitHub organization at https://github.com/redhat-actions/ to see our actions and provide feedback.

name: OpenShift

env: {% extends 'base.html.twig' %}

{% block body %}
  <p class="alert alert-danger">
    {% block message %}
      Произошла ошибка, попробуйте позже.
    {% endblock message %}
  </p>
{% endblock body %}
  # 🖊️ EDIT your repository secrets to log into your OpenShift cluster and set up the context.
  # See https://github.com/redhat-actions/oc-login#readme for how to retrieve these values.
  # To get a permanent token, refer to https://github.com/redhat-actions/oc-login/wiki/Using-a-Service-Account-for-GitHub-Actions
  OPENSHIFT_SERVER: "{% extends 'base.html.twig' %}

{% block body %}
  <p class="alert alert-danger">
    {% block message %}
      У вас нет прав на данное действие.
    {% endblock message %}
  </p>
{% endblock body %}"
  OPENSHIFT_TOKEN: "{% extends 'base.html.twig' %}

{% block body %}
  <p class="alert alert-danger">
    {% block message %}
      Запрашиваемый ресурс не найден.
    {% endblock message %}
  </p>
{% endblock body %}"
  # 🖊️ EDIT to set the kube context's namespace after login. Leave blank to use your user's default namespace.
  OPENSHIFT_NAMESPACE: "{% extends 'base.html.twig' %}

{% block body %}
  <p class="alert alert-danger">
    {% block message %}
      Произошла ошибка, попробуйте позже.
    {% endblock message %}
  </p>
{% endblock body %}"

  # 🖊️ EDIT to set a name for your OpenShift app, or a default one will be generated below.
  APP_NAME: ".skipped-buttons {
    padding: 16px 0;
}
h1 {
    margin-bottom: 20px;
}"

  # 🖊️ EDIT with the port your application should be accessible on.
  # If the container image exposes *exactly one* port, this can be left blank.
  # Refer to the 'port' input of https://github.com/redhat-actions/oc-new-app
  APP_PORT: ".test-title {
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
}"

  # 🖊️ EDIT to change the image registry settings.
  # Registries such as GHCR, Quay.io, and Docker Hub are supported.
  IMAGE_REGISTRY: ghcr.io/${{ .test-container {
    margin: 20px 0 30px;
}
.button-wrapper {
    text-align: center;
} }}
  IMAGE_REGISTRY_USER: ${{ .question-description-container {
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
} }}
  IMAGE_REGISTRY_PASSWORD: ${{ .test-result-section h1 {
    margin-bottom: 20px;
} }}

  # 🖊️ EDIT to specify custom tags for the container image, or default tags will be generated below.
  IMAGE_TAGS: " .body-wrapper {
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
} "

on: {% extends 'base.html.twig' %}

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
  # https://docs.github.com/en/actions/reference/events-that-trigger-workflows
  workflow_dispatch: {% extends 'test/preface.html.twig' %}

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
  push: {% extends 'base.html.twig' %}

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
    # Edit to the branch(es) you want to build and deploy on each push.
    branches: [ "main" ]

jobs: {% extends 'base.html.twig' %}

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
  # 🖊️ EDIT if you want to run vulnerability check on your project before deploying
  # the application. Please uncomment the below CRDA scan job and configure to run it in
  # your workflow. For details about CRDA action visit https://github.com/redhat-actions/crda/blob/main/README.md
  #
  # TODO: Make sure to add 'CRDA Scan' starter workflow from the 'Actions' tab.
  # For guide on adding new starter workflow visit https://docs.github.com/en/github-ae@latest/actions/using-workflows/using-starter-workflows

  #crda-scan: {% extends 'base.html.twig' %}

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
  #  uses: ./.{% extends 'base.html.twig' %}

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
  #  secrets: {{ form_start(form, {'attr': {'novalidate':'novalidate'}}) }}
  {{ form_label(form.answer, 'Ответ: ') }}
  {{ form_widget(form.answer, {'attr': {'class':'text-answer'}}) }}
{{ form_end(form) }}
  #    CRDA_KEY: ${{ {{ form_start(form, {'attr': {'novalidate':'novalidate'}}) }}
  {{ form_label(form.answer, 'Варианты: ') }}
  {{ form_widget(form.answer) }}
{{ form_end(form) }} }}
  #    # SNYK_TOKEN: ${{ <!DOCTYPE html>
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

</html> }}       # Either use SNYK_TOKEN or CRDA_KEY

  openshift-ci-cd: /*
 * Utility header for benchmarks with Google Benchmark.
 */
#include <benchmark/benchmark.h>

/* The two functions below are an approach proposed by Chandler Carruth in
 * CPPCON 2015: CppCon 2015: "Tuning C++: Benchmarks, and CPUs, and Compilers!
 * Oh My!" in order keep the compiler from optimizing the use of a variable
 * (gbenchmark_escape) or whole memory (gbenchmark_clobber).
 *
 * The compiler optimizer may sometimes remove code when it sees it isn't
 * necessary. For example, when a variable isn't used, the optimizer removes
 * the code that computes the value for that variable - that's not good for
 * benchmarks. The function gbenchmark_escape(void *p) makes the compiler think
 * that that p is being used in a code that might have "unknowable side
 * effects", which keeps it from removing the variable. The "side effects" in
 * the case here would be the benchmark numbers.
 *
 * Here is an example that would give wrong benchmark values:
 *
 * static void BM_Test(benchmark::State& state)
 * {
 *     while (state.KeepRunning()) {
 *         float a = expensive_operation();
 *     }
 * }
 *
 * Since variable a isn't used, the call to expensive_operation() is removed
 * from the compiled program. The benchmark would show that
 * expensive_operation() is extremely fast. The following code would fix that:
 *
 * static void BM_Test(benchmark::State& state)
 * {
 *     while (state.KeepRunning()) {
 *         float a = expensive_operation();
 *         gbenchmark_escape(&a);
 *     }
 * }
 */

inline void gbenchmark_escape(void* p)
{
      asm volatile("" : : "g"(p) : "memory");
}

inline void gbenchmark_clobber()
{
    asm volatile("" : : : "memory");
}
    # 🖊️ Uncomment this if you are using CRDA scan step above
    # needs: crda-scan
    name: Build and deploy to OpenShift
    runs-on: ubuntu-20.04
    environment: production

    outputs:
      ROUTE: ${{ steps.deploy-and-expose.outputs.route }}
      SELECTOR: ${{ steps.deploy-and-expose.outputs.selector }}

    steps:
    - name: Check for required secrets
      uses: actions/github-script@v6
      with:
        script: |
          const secrets = {
            OPENSHIFT_SERVER: `${{ secrets.OPENSHIFT_SERVER }}`,
            OPENSHIFT_TOKEN: `${{ secrets.OPENSHIFT_TOKEN }}`,
          };

          const GHCR = "ghcr.io";
          if (`${{ env.IMAGE_REGISTRY }}`.startsWith(GHCR)) {
            core.info(`Image registry is ${GHCR} - no registry password required`);
          }
          else {
            core.info("A registry password is required");
            secrets["IMAGE_REGISTRY_PASSWORD"] = `${{ secrets.IMAGE_REGISTRY_PASSWORD }}`;
          }

          const missingSecrets = Object.entries(secrets).filter(([ name, value ]) => {
            if (value.length === 0) {
              core.error(`Secret "${name}" is not set`);
              return true;
            }
            core.info(`✔️ Secret "${name}" is set`);
            return false;
          });

          if (missingSecrets.length > 0) {
            core.setFailed(`❌ At least one required secret is not set in the repository. \n` +
              "You can add it using:\n" +
              "GitHub UI: https://docs.github.com/en/actions/reference/encrypted-secrets#creating-encrypted-secrets-for-a-repository \n" +
              "GitHub CLI: https://cli.github.com/manual/gh_secret_set \n" +
              "Also, refer to https://github.com/redhat-actions/oc-login#getting-started-with-the-action-or-see-example");
          }
          else {
            core.info(`✅ All the required secrets are set`);
          }

    - name: Check out repository
      uses: actions/checkout@v4

    - name: Determine app name
      if: env.APP_NAME == ''
      run: |
        echo "APP_NAME=$(basename $PWD)" | tee -a $GITHUB_ENV

    - name: Determine image tags
      if: env.IMAGE_TAGS == ''
      run: |
        echo "IMAGE_TAGS=latest ${GITHUB_SHA::12}" | tee -a $GITHUB_ENV

    # https://github.com/redhat-actions/buildah-build#readme
    - name: Build from Dockerfile
      id: build-image
      uses: redhat-actions/buildah-build@v2
      with:
        image: ${{ env.APP_NAME }}
        tags: ${{ env.IMAGE_TAGS }}

        # If you don't have a Dockerfile/Containerfile, refer to https://github.com/redhat-actions/buildah-build#scratch-build-inputs
        # Or, perform a source-to-image build using https://github.com/redhat-actions/s2i-build
        # Otherwise, point this to your Dockerfile/Containerfile relative to the repository root.
        dockerfiles: |
          ./Dockerfile

    # https://github.com/redhat-actions/push-to-registry#readme
    - name: Push to registry
      id: push-image
      uses: redhat-actions/push-to-registry@v2
      with:
        image: ${{ steps.build-image.outputs.image }}
        tags: ${{ steps.build-image.outputs.tags }}
        registry: ${{ env.IMAGE_REGISTRY }}
        username: ${{ env.IMAGE_REGISTRY_USER }}
        password: ${{ env.IMAGE_REGISTRY_PASSWORD }}

    # The path the image was pushed to is now stored in ${{ steps.push-image.outputs.registry-path }}

    - name: Install oc
      uses: redhat-actions/openshift-tools-installer@v1
      with:
        oc: 4

    # https://github.com/redhat-actions/oc-login#readme
    - name: Log in to OpenShift
      uses: redhat-actions/oc-login@v1
      with:
        openshift_server_url: ${{ env.OPENSHIFT_SERVER }}
        openshift_token: ${{ env.OPENSHIFT_TOKEN }}
        insecure_skip_tls_verify: true
        namespace: ${{ env.OPENSHIFT_NAMESPACE }}

    # This step should create a deployment, service, and route to run your app and expose it to the internet.
    # https://github.com/redhat-actions/oc-new-app#readme
    - name: Create and expose app
      id: deploy-and-expose
      uses: redhat-actions/oc-new-app@v1
      with:
        app_name: ${{ env.APP_NAME }}
        image: ${{ steps.push-image.outputs.registry-path }}
        namespace: ${{ env.OPENSHIFT_NAMESPACE }}
        port: ${{ env.APP_PORT }}

    - name: Print application URL
      env:
        ROUTE: ${{ steps.deploy-and-expose.outputs.route }}
        SELECTOR: ${{ steps.deploy-and-expose.outputs.selector }}
      run: |
        [[ -n ${{ env.ROUTE }} ]] || (echo "Determining application route failed in previous step"; exit 1)
        echo
        echo "======================== Your application is available at: ========================"
        echo ${{ env.ROUTE }}
        echo "==================================================================================="
        echo
        echo "Your app can be taken down with: \"oc delete all --selector='${{ env.SELECTOR }}'\""
