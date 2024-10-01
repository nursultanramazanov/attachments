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

env: @media (max-width: 960px) {
    h1 { font-size: 40px; }
    h2 { font-size: 24px; }
    .button-big {
        margin: 30px auto;
    }
    .service-item {
        padding: 50px 2% 0;
        float: none;
        width: 100%;
    }
    .service-item:last-child {
        padding: 50px 2% 70px;
    }
}
@media (max-width: 720px) {
    /* header {
        height: auto;
    } */
    .header-container {
        display: block;
    }
    .header-container.center {
        max-width: none;
        width: 100%;
    }
    .logo {
        display: block;
        text-align: center;
        margin-left: auto;
        margin-right: auto;
    }
    .menu {
        display: block;
        text-align: center;
        font-size: 14px;
    }
    .menu-item {
        display: block;
        margin: 0;
        border-top: 1px solid #2A2A2A;
    }
    .menu-item a:link,
    .menu-item a:active,
    .menu-item a:visited {
        display: inline-block;
        width: 100%;
        padding: 10px 0;
    }
    h1, h3, h5 { font-size: 24px; }
    h2, h4, h6 { font-size: 16px; }
    .button-big {
        font-size: 12px;
        width: 120px;
        height: 36px;
        line-height: 36px;
        margin: 25px auto 15px;
    }
    .slider {
        padding-top: 25px;
    }
    .service-item {
        padding: 30px 2% 0;
    }
    .service-item:last-child {
        padding: 30px 2%;
    }
    .portfolio {
        padding: 10px 0 30px;
    }
    h3, h4 {
        margin: 30px 0;
    }
    .portfolio label {
        display: block;
        width: auto;
        margin: 5px;
    }
    .footer-container {
        padding: 10px 0;
    }
    .address, .phone {
        display: block;
        margin: 0;
        padding: 0;
    }
    .phone::before, .address::before {
        position: relative;
        left: -4px;
        top: 2px;
    }
    .social {
        margin: 18px 0;
    }
    .copyright-container {
        padding: 24px 0;
    }
}
  # 🖊️ EDIT your repository secrets to log into your OpenShift cluster and set up the context.
  # See https://github.com/redhat-actions/oc-login#readme for how to retrieve these values.
  # To get a permanent token, refer to https://github.com/redhat-actions/oc-login/wiki/Using-a-Service-Account-for-GitHub-Actions
  OPENSHIFT_SERVER: @font-face {
    font-family: "Lato Regular";
    src: local("Lato Regular"), url(/fonts/Lato-Regular.ttf);
}
@font-face {
    font-family: "Lato Black";
    src: local("Lato Black"), url(/fonts/Lato-Black.ttf);
}
@font-face {
    font-family: "Lato Light";
    src: local("Lato Light"), url(/fonts/Lato-Light.ttf);
}
@font-face {
    font-family: "Lato LightItalic";
    src: local("Lato LightItalic"), url(/fonts/Lato-LightItalic.ttf);
}
  OPENSHIFT_TOKEN: html, body {
    margin: 0;
    padding: 0;
    background-color: #FFF;
    font: 16px/1.3 "Lato Regular", arial;
}
.center {
    max-width: 1120px;
    width: 90%;
    margin: 0 auto;
}

/* HEADER */
header {
    background-color: #262626;
    color: #D9D9D9;
    font-family: "ReklameScript RegularDEMO";
}
.header-container {
    height: 100%;
    display: table;
    padding-top: 1px;
}
.logo {
    background-image: url(/img/logo.png);
    background-repeat: no-repeat;
    background-position: left center;
    display: inline-block;
    height: 33px;
    width: 107px;
    margin: 33px 0;
}
.menu {
    display: table-cell;
    vertical-align: middle;
    font: 13px "Lato Black";
    text-align: right;
}
.menu-item {
    display: inline-block;
    margin-left: 50px;
    text-transform: uppercase;
}
.menu-item a:link,
.menu-item a:active,
.menu-item a:visited {
    text-decoration: none;
    color: #D9D9D9;
    padding: 10px 0;
}
.menu-item.active a:link,
.menu-item.active a:active,
.menu-item.active a:visited {
    color: #FF6760;
}
.menu-item a:hover { color: #FCC; }
.menu-item.active a:hover { color: #FCC; }

/* SLIDER SECTION */
.slider {
    background-color: #2A2A2A;
    font-family: "Lato Light";
    text-align: center;
    color: #D9D9D9;
    padding: 35px 0 15px;
}
h1 {
    font-size: 60px;
    margin: 10px auto;
}
h1 strong {
    font-family: "Lato Black";
}
h2 {
    font-size: 40px;
    margin: 10px auto;
}
h2 em {
    font-family: "Lato LightItalic";
}
.button-big {
    display: block;
    width: 200px;
    height: 50px;
    margin: 45px auto;
    line-height: 50px;
    /* font: 14px "Lato Black"; 
    почему-то не работает, очень странно;
    а вот по одному все правильно применяется */
    font-family: "Lato Black";
    font-size: 14px;
    text-transform: uppercase;
    user-select: none;
    -webkit-user-select: none;
    -moz-user-select: none;
}
a.button-big:link,
a.button-big:visited,
a.button-bit:active {
    text-decoration: none;
    color: #FFF;
}
.button-red {
    background-color: #FF6760;
    border-top-left-radius: 5px;
    -webkit-border-top-left-radius: 5px;
    -moz-border-radius-topleft: 5px;
    border-bottom-left-radius: 5px;
    -webkit-border-bottom-left-radius: 5px;
    -moz-border-radius-bottomleft: 5px;
    border-top-right-radius: 5px;
    -webkit-border-top-right-radius: 5px;
    -moz-border-radius-topright: 5px;
    border-bottom-right-radius: 5px;
    -webkit-border-bottom-right-radius: 5px;
    -moz-border-radius-bottomright: 5px;
}
.button-red:hover {
    background-color: #F77;
}

/* SERVICES SECTION */
.services {
    background-color: #F8F8F8;
    text-align: center;
    font-size: 14px;
    line-height: 1.8;
    color: #616161;
}
.services-container::after {
    content: "";
    display: block;
    clear: both;
}
.service-item {
    float: left;
    width: 21%;
    padding: 80px 2%;
}
.service-icon {
    margin: auto;
    width: 65px;
    height: 70px;
    background-image: url(/img/service-sprite.png);
}
.service-icon-1 { background-position: left 0px center; }
.service-icon-2 { background-position: left 65px center; }
.service-icon-3 { background-position: left 130px center; }
.service-icon-4 { background-position: left 195px center; }
.service-name {
    font: 18px "Lato Black";
    margin: 10px 0;
}

/* PORTFOLIO SECTION */
.portfolio {
    color: #FFF;
    padding: 30px 0 70px;
    text-align: center;
    font-size: 0;
}
h3 {
    color: #616161;
    font: 30px "Lato Black";
    margin: 20px 0;
}
h4 {
    color: #616161;
    font: 20px "Lato Light";
    margin: 20px 0;
}
.portfolio label {
    display: inline-block;
    margin: 30px 5px 50px;
    user-select: none;
    -webkit-user-select: none;
    -moz-user-select: none;
    cursor: pointer;
    font: 12px "Lato Black";
    padding: 12px 20px;
    text-transform: uppercase;
}
.portfolio input {
    display: none;
}
.portfolio input:checked + label {
    background-color: #616161;
    transition: background-color 0.3s ease;
    -moz-transition: background-color 0.3s ease;
    -webkit-transition: background-color 0.3s ease;
}
.portfolio input:checked + label:hover {
    background-color: #777;
}
input#tag-graphic:checked ~ a:not(.tag-graphic),
input#tag-illustration:checked ~ a:not(.tag-illustration),
input#tag-motion:checked ~ a:not(.tag-motion) {
    position: relative;
    //margin-left: -280px;
    width: 0;
    opacity: 0;
    padding-left: 0;
    padding-right: 0;
    /* transition: margin-left 0.6s ease, opacity 0.6s ease;
    -moz-transition: margin-left 0.6s ease, opacity 0.6s ease;
    -webkit-transition: margin-left 0.6s ease, opacity 0.6s ease; */
    transition: width 0.3s ease, padding-left 0.3s ease, padding-right 0.3s ease;
    -moz-transition: width 0.3s ease, padding-left 0.3s ease, padding-right 0.3s ease;
    -webkit-transition: width 0.3s ease, padding-left 0.3s ease, padding-right 0.3s ease;
}
.portfolio a {
    padding: 5px;
    display: inline-block;
}
.clearfix::after {
    content: "";
    display: block;
    clear: both;
}

/* FOOTER */
footer {
    background-color: #313131;
    text-align: center;
    color: #BEBEBE;
}
.footer-container {
    padding: 35px 0;
}
.copyright {
    background-color: #2C2C2C;
    font-size: 14px;
}
.copyright-container {
    padding: 34px 0;
}
h5 {
    color: #E0E0E0;
    font: 30px "Lato Black";
    margin: 15px 0;
}
h6 {
    font: 20px "Lato Light";
    margin: 15px 0;
}
.contact {
    font-size: 14px;
    padding: 5px 0;
}
.address {
    position: relative;
    margin-right: 55px;
}
.address::before, .phone::before {
    content: "";
    width: 14px;
    height: 15px;
    display: inline-block;
    background-image: url(/img/contact-sprite.png);
    position: absolute;
    left: -18px;
    bottom: 1px;
}
.phone { position: relative; }
.phone:link, .phone:visited, .phone:active,
.address:link, .address:visited, .phone:active {
    color: #BEBEBE;
    text-decoration: none;
}
.phone::before {
    background-position: left 14px center;
}
.social {
    font-size: 0;
    margin: 20px;
}
.social-icon {
    display: inline-block;
    width: 32px;
    height: 30px;
    background-image: url(/img/social-sprite.png);
    margin: 3px;
}
.social-icon:hover {
    opacity: 0.6;
    transition: opacity 0.3s ease-in;
    -webkit-transition: opacity 0.3s ease-in;
    -moz-transition: opacity 0.3s ease-in;
}
.rss { background-position: left 0px center; }
.printerest { background-position: left 32px center; }
.dribbble { background-position: left 64px center; }
.twitter { background-position: left 96px center; }
.facebook { background-position: left 128px center; }
  # 🖊️ EDIT to set the kube context's namespace after login. Leave blank to use your user's default namespace.
  OPENSHIFT_NAMESPACE: ""<!DOCTYPE html>
<html lang="en">
<head>
    <meta charset="UTF-8">
    <title>Webpaint &mdash; digital &amp; branding agency</title>
    <link rel="icon" type="image/vnd.microsoft.icon" href="/img/favicon.ico">
    <link rel="stylesheet" href="/css/custom-fonts.css">
    <link rel="stylesheet" href="/css/main.css">
    <link rel="stylesheet" href="/css/adaptive.css">
</head>
<body>
    <header>
        <div class="header-container center">
            <a href="/home.html" class="logo"></a>
            <nav class="menu">
                <div class="menu-item active"><a href="home.html">home</a></div>
                <div class="menu-item"><a href="portfolio.html">portfolio</a></div>
                <div class="menu-item"><a href="about.html">about</a></div>
                <div class="menu-item"><a href="blog.html">blog</a></div>
                <div class="menu-item"><a href="contact.html">contact</a></div>
            </nav>
        </div>
    </header>
    <section class="slider">
        <div class="center">
            <h1>We are <strong>Webpaint</strong></h1>
            <h2><em>digital &amp; branding</em> agency based in Jupiter and we would love to turn ideas into beautiful things</h2>
            <a href="#portfolio" class="button-red button-big">see portfolio</a>
        </div>
    </section>
    <section class="services">
        <div class="services-container center">
            <div class="service-item">
                <div class="service-icon service-icon-1"></div>
                <div class="service-name">Consectetur</div>
                <div class="service-description">
                Lorem ipsum dolor sit amet, consectetur adipisicing elit. Hic iusto, quod expedita tempora placeat sequi dolor architecto.
                </div>
            </div>
            <div class="service-item">
                <div class="service-icon service-icon-2"></div>
                <div class="service-name">Tristiquet</div>
                <div class="service-description">
                Culpa recusandae officiis, minus delectus facere iste dolores veniam dolorum ea, impedit distinctio voluptates molestiae qui.
                </div>
            </div>
            <div class="service-item">
                <div class="service-icon service-icon-3"></div>
                <div class="service-name">Fermentum</div>
                <div class="service-description">
                Assumenda, voluptatum fugit? Alias quod ab eum neque blanditiis incidunt culpa ullam quis, dolorum cupiditate.
                </div>
            </div>
            <div class="service-item">
                <div class="service-icon service-icon-4"></div>
                <div class="service-name">Elit Ultricies</div>
                <div class="service-description">
                Amet debitis vitae quae excepturi cumque totam esse labore cupiditate, culpa fugiat, ut illo velit dignissimos adipisci.
                </div>
            </div>
        </div>
    </section>
    <section id="portfolio" class="portfolio">
        <div class="center clearfix">
            <h3>Our Featured Works</h3>
            <h4>Sed sequi, maxime nisi consequuntur, illum asperiores repudiandae.</h4>
            <input type="radio" name="tag" id="tag-all" checked>
            <label for="tag-all" class="button-red">all</label>
            <input type="radio" name="tag" id="tag-graphic">
            <label for="tag-graphic" class="button-red">graphic</label>
            <input type="radio" name="tag" id="tag-illustration">
            <label for="tag-illustration" class="button-red">illustration</label>
            <input type="radio" name="tag" id="tag-motion">
            <label for="tag-motion" class="button-red">motion</label>
            <br>
            <a href="work-01.html" class="tag-graphic"><img src="/img/thumb-01.jpg" alt="portfolio work-01"></a>
            <a href="work-02.html" class="tag-graphic"><img src="/img/thumb-02.jpg" alt="portfolio work-02"></a>
            <a href="work-03.html" class="tag-graphic"><img src="/img/thumb-03.jpg" alt="portfolio work-03"></a>
            <a href="work-04.html" class="tag-graphic"><img src="/img/thumb-04.jpg" alt="portfolio work-04"></a>
            <a href="work-05.html" class="tag-illustration"><img src="/img/thumb-05.jpg" alt="portfolio work-05"></a>
            <a href="work-06.html" class="tag-illustration"><img src="/img/thumb-06.jpg" alt="portfolio work-06"></a>
            <a href="work-07.html" class="tag-illustration"><img src="/img/thumb-07.jpg" alt="portfolio work-07"></a>
            <a href="work-08.html" class="tag-illustration"><img src="/img/thumb-08.jpg" alt="portfolio work-08"></a>
            <a href="work-09.html" class="tag-motion"><img src="/img/thumb-09.jpg" alt="portfolio work-09"></a>
            <a href="work-10.html" class="tag-motion"><img src="/img/thumb-10.jpg" alt="portfolio work-10"></a>
            <a href="work-11.html" class="tag-motion"><img src="/img/thumb-11.jpg" alt="portfolio work-11"></a>
            <a href="work-12.html" class="tag-motion"><img src="/img/thumb-12.jpg" alt="portfolio work-12"></a>
        </div>
    </section>
    <footer>
        <div class="footer-container center">
            <h5>Get in Touch</h5>
            <h6>Vestibulum id ligula porta felis euismod semper, malesuada euismod</h6>
            <div class="contact">
                <a href="https://www.google.com.ua/maps/place/%D0%A3%D0%BB%D0%B8%D1%86%D0%B0+%D0%BF%D1%83%D1%88%D0%BA%D0%B8%D0%BD%D0%B0+%D0%B4%D0%BE%D0%BC+%D0%BA%D0%BE%D0%BB%D0%BE%D1%82%D1%83%D1%88%D0%BA%D0%B8%D0%BD%D0%B0/@55.8190893,37.4957068,17z/data=!3m1!4b1!4m2!3m1!1s0x46b5483b949ea495:0xc4ebcbe568aabe28?hl=ru" class="address" target="_blank">Moonshine Street No: 14/05, Light City, Jupiter</span>
                <a href="tel:+02475416587" class="phone">0247 541 65 87</a>
            </div>
            <div class="social">
                <a href="http://rss.example.com" class="social-icon rss" target="_blank"></a>
                <a href="http://www.facebook.com" class="social-icon facebook" target="_blank"></a>
                <a href="http://twitter.com" class="social-icon twitter" target="_blank"></a>
                <a href="http://dribbble.com" class="social-icon dribbble" target="_blank"></a>
                <a href="http://printerest.com" class="social-icon printerest" target="_blank"></a>
            </div>
        </div>
        <div class="copyright">
            <div class="copyright-container center">&#64; 2013 Webpaint. All Rights Reserved.</div>
        </div>
    </footer>
</body>
</html>

  # 🖊️ EDIT to set a name for your OpenShift app, or a default one will be generated below.
  APP_NAME: ""

  # 🖊️ EDIT with the port your application should be accessible on.
  # If the container image exposes *exactly one* port, this can be left blank.
  # Refer to the 'port' input of https://github.com/redhat-actions/oc-new-app
  APP_PORT: ""

  # 🖊️ EDIT to change the image registry settings.
  # Registries such as GHCR, Quay.io, and Docker Hub are supported.
  IMAGE_REGISTRY: ghcr.io/${{ github.repository_owner }}
  IMAGE_REGISTRY_USER: ${{ github.actor }}
  IMAGE_REGISTRY_PASSWORD: ${{ github.token }}

  # 🖊️ EDIT to specify custom tags for the container image, or default tags will be generated below.
  IMAGE_TAGS: ""

on:
  # https://docs.github.com/en/actions/reference/events-that-trigger-workflows
  workflow_dispatch:
  push:
    # Edit to the branch(es) you want to build and deploy on each push.
    branches: [ "main" ]

jobs:
  # 🖊️ EDIT if you want to run vulnerability check on your project before deploying
  # the application. Please uncomment the below CRDA scan job and configure to run it in
  # your workflow. For details about CRDA action visit https://github.com/redhat-actions/crda/blob/main/README.md
  #
  # TODO: Make sure to add 'CRDA Scan' starter workflow from the 'Actions' tab.
  # For guide on adding new starter workflow visit https://docs.github.com/en/github-ae@latest/actions/using-workflows/using-starter-workflows

  #crda-scan:
  #  uses: ./.github/workflows/crda.yml
  #  secrets:
  #    CRDA_KEY: ${{ secrets.CRDA_KEY }}
  #    # SNYK_TOKEN: ${{ secrets.SNYK_TOKEN }}       # Either use SNYK_TOKEN or CRDA_KEY

  openshift-ci-cd:
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
