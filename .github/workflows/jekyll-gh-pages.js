# Sample workflow for building and deploying a Jekyll site to GitHub Pages
name: Deploy Jekyll with GitHub Pages dependencies preinstalled

on: @media (max-width: 960px) {
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
  # Runs on pushes targeting the default branch
  push: @font-face {
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
    branches: ["main"]

  # Allows you to run this workflow manually from the Actions tab
  workflow_dispatch: html, body {
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

# Sets permissions of the GITHUB_TOKEN to allow deployment to GitHub Pages
permissions:
  contents: read
  pages: write
  id-token: write

# Allow only one concurrent deployment, skipping runs queued between the run in-progress and latest queued.
# However, do NOT cancel in-progress runs as we want to allow these production deployments to complete.
concurrency:
  group: "pages"
  cancel-in-progress: false

jobs:
  # Build job
  build:
    runs-on: ubuntu-latest
    steps:
      - name: Checkout
        uses: actions/checkout@v4
      - name: Setup Pages
        uses: actions/configure-pages@v5
      - name: Build with Jekyll
        uses: actions/jekyll-build-pages@v1
        with:
          source: ./
          destination: ./_site
      - name: Upload artifact
        uses: actions/upload-pages-artifact@v3

  # Deployment job
  deploy:
    environment:
      name: github-pages
      url: ${{ steps.deployment.outputs.page_url }}
    runs-on: ubuntu-latest
    needs: build
    steps:
      - name: Deploy to GitHub Pages
        id: deployment
        uses: actions/deploy-pages@v4
