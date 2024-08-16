# This workflow will install Python dependencies, run tests and lint with a variety of Python versions
# For more information see: https://docs.github.com/en/actions/automating-builds-and-tests/building-and-testing-python

name: Python package

on: ARG base_image
ARG python_version=base

FROM ${base_image} AS base

RUN yum install -y \
    boost-devel \
    jemalloc-devel \
    libxml2-devel \
    libxslt-devel \
    bison \
    make \
    gcc \
    gcc-c++ \
    flex \
    autoconf \
    zip \
    git \
    ninja-build

WORKDIR /root

FROM ${python_version}
COPY pyproject.toml poetry.lock ./

RUN pip3 install --upgrade pip wheel
RUN pip3 install --upgrade urllib3==1.26.16  # temporary to avoid https://github.com/urllib3/urllib3/issues/2168 (TODO remove when the AL2 image updates to support OpenSSL 1.1.1+)
RUN pip3 install --upgrade six cython==3.0.8 cmake hypothesis poetry
RUN poetry config virtualenvs.create false --local && poetry install --no-root --only main

RUN rm -f pyproject.toml poetry.lock

ENTRYPOINT ["/bin/sh"]
  push: ARG base_image
ARG python_version=base

FROM ${base_image} AS base

RUN dnf install -y \
    boost-devel \
    jemalloc-devel \
    libxml2-devel \
    libxslt-devel \
    bison \
    make \
    gcc \
    gcc-c++ \
    flex \
    autoconf \
    zip \
    git \
    ninja-build

WORKDIR /root

FROM ${python_version}
COPY pyproject.toml poetry.lock ./

RUN pip3 install --upgrade pip wheel
RUN pip3 install --upgrade six cython==3.0.8 cmake hypothesis poetry
RUN poetry config virtualenvs.create false --local && poetry install --no-root --only main

RUN rm -f pyproject.toml poetry.lock

ENTRYPOINT ["/bin/sh"]
    branches: [ "main" ]
  pull_request:  #!/usr/bin/env bash
set -ex

cp ../../pyproject.toml .
cp ../../poetry.lock .

export DOCKER_BUILDKIT=1

PYTHON_VERSION=${1:-ALL}

# Python 3.8
if [[ $PYTHON_VERSION == "ALL" || $PYTHON_VERSION == "3.8" ]]
then
  docker build \
    --pull \
    --tag awswrangler-build-py38 \
    --build-arg base_image=public.ecr.aws/lambda/python:3.8 \
    .
fi

# Python 3.9
if [[ $PYTHON_VERSION == "ALL" || $PYTHON_VERSION == "3.9" ]]
then
  docker build \
    --pull \
    --tag awswrangler-build-py39 \
    --build-arg base_image=public.ecr.aws/lambda/python:3.9 \
    .
fi

# Python 3.10
if [[ $PYTHON_VERSION == "ALL" || $PYTHON_VERSION == "3.10" ]]
then
  docker build \
    --pull \
    --tag awswrangler-build-py310 \
    --build-arg base_image=public.ecr.aws/lambda/python:3.10 \
    .
fi

# Python 3.11
if [[ $PYTHON_VERSION == "ALL" || $PYTHON_VERSION == "3.11" ]]
then
  docker build \
    --pull \
    --tag awswrangler-build-py311 \
    --build-arg base_image=public.ecr.aws/lambda/python:3.11 \
    .
fi

# Python 3.12
if [[ $PYTHON_VERSION == "ALL" || $PYTHON_VERSION == "3.12" ]]
then
  docker build \
    --pull \
    --tag awswrangler-build-py312 \
    --build-arg base_image=public.ecr.aws/lambda/python:3.12 \
    --file Dockerfile.al2023 \
    .
fi

rm -rf pyproject.toml poetry.lock
    branches: [ "main" ]

jobs: #!/usr/bin/env bash
set -ex

FILENAME="awswrangler-layer-${1}.zip"
NINJA=${2}

pushd /aws-sdk-pandas
rm -rf python dist/pyarrow_files "dist/${FILENAME}" "${FILENAME}"
popd

rm -rf dist arrow

export ARROW_HOME=$(pwd)/dist
export LD_LIBRARY_PATH=$(pwd)/dist/lib:$LD_LIBRARY_PATH
export CMAKE_PREFIX_PATH=$ARROW_HOME:$CMAKE_PREFIX_PATH

git clone \
  --depth 1 \
  --branch apache-arrow-16.1.0 \
  --single-branch \
  https://github.com/apache/arrow.git

mkdir $ARROW_HOME
mkdir arrow/cpp/build
pushd arrow/cpp/build

cmake \
    -DCMAKE_INSTALL_PREFIX=$ARROW_HOME \
    -DCMAKE_INSTALL_LIBDIR=lib \
    -DARROW_PYTHON=ON \
    -DARROW_PARQUET=ON \
    -DARROW_DATASET=ON \
    -DARROW_WITH_SNAPPY=ON \
    -DARROW_WITH_ZLIB=ON \
    -DARROW_FLIGHT=OFF \
    -DARROW_GANDIVA=OFF \
    -DARROW_ORC=OFF \
    -DARROW_CSV=ON \
    -DARROW_JSON=ON \
    -DARROW_COMPUTE=ON \
    -DARROW_FILESYSTEM=ON \
    -DARROW_PLASMA=OFF \
    -DARROW_WITH_BZ2=OFF \
    -DARROW_WITH_ZSTD=OFF \
    -DARROW_WITH_LZ4=OFF \
    -DARROW_WITH_BROTLI=OFF \
    -DARROW_BUILD_TESTS=OFF \
    -GNinja \
    ..

eval $NINJA
eval "${NINJA} install"

popd

pushd arrow/python

export CMAKE_PREFIX_PATH=${ARROW_HOME}${CMAKE_PREFIX_PATH:+:${CMAKE_PREFIX_PATH}}
export ARROW_PRE_0_15_IPC_FORMAT=0
export PYARROW_WITH_HDFS=0
export PYARROW_WITH_FLIGHT=0
export PYARROW_WITH_GANDIVA=0
export PYARROW_WITH_ORC=0
export PYARROW_WITH_CUDA=0
export PYARROW_WITH_PLASMA=0
export PYARROW_WITH_PARQUET=1
export PYARROW_WITH_DATASET=1
export PYARROW_WITH_FILESYSTEM=1
export PYARROW_WITH_CSV=1
export PYARROW_WITH_JSON=1
export PYARROW_WITH_COMPUTE=1

python3 setup.py build_ext \
  --build-type=release \
  --bundle-arrow-cpp \
  bdist_wheel

pip3 install dist/pyarrow-*.whl -t /aws-sdk-pandas/dist/pyarrow_files

popd

pushd /aws-sdk-pandas

pip3 install . -t ./python ".[redshift,mysql,postgres,gremlin,opensearch,openpyxl]"

rm -rf python/pyarrow*
rm -rf python/boto*
rm -rf python/urllib3*
rm -rf python/s3transfer*

cp -r /aws-sdk-pandas/dist/pyarrow_files/pyarrow* python/

# Removing nonessential files
find python -name '*.so' -type f -exec strip "{}" \;
find python -wholename "*/tests/*" -type f -delete
find python -regex '^.*\(__pycache__\|\.py[co]\)$' -delete

zip -r9 "${FILENAME}" ./python
mv "${FILENAME}" dist/

rm -rf python dist/pyarrow_files "${FILENAME}"

popd

rm -rf dist arrow
  build: #!/usr/bin/env bash
set -ex

pushd ..
rm -rf docs/build docs/source/stubs
make -C docs/ html
doc8 --ignore-path docs/source/stubs --max-line-length 120 docs/source

    runs-on: #!/usr/bin/env bash
set -ex

VERSION=$(poetry version --short)
DIR_NAME=$(dirname "$PWD")

PYTHON_VERSION=${1:-ALL}

ARCH=$(arch)
[ "${ARCH}" = "aarch64" ] && ARCH_SUFFIX="-arm64" # AWS Lambda, the name arm64 is used instead of aarch64

if [[ $PYTHON_VERSION == "ALL" ]]
then
  echo "Building Lambda Layers for AWS SDK for pandas ${VERSION} (ALL supported Python versions)"
else
  echo "Building Lambda Layers for AWS SDK for pandas ${VERSION} (ONLY Python $PYTHON_VERSION)"
fi

pushd lambda

# Building all related docker images
./build-docker-images.sh $PYTHON_VERSION

# Python 3.8
if [[ $PYTHON_VERSION == "ALL" || $PYTHON_VERSION == "3.8" ]]
then
  docker run \
    --volume "$DIR_NAME":/aws-sdk-pandas/ \
    --workdir /aws-sdk-pandas/building/lambda \
    --rm \
    awswrangler-build-py38 \
    build-lambda-layer.sh "${VERSION}-py3.8${ARCH_SUFFIX}" "ninja-build"
fi

# Python 3.9
if [[ $PYTHON_VERSION == "ALL" || $PYTHON_VERSION == "3.9" ]]
then
  docker run \
    --volume "$DIR_NAME":/aws-sdk-pandas/ \
    --workdir /aws-sdk-pandas/building/lambda \
    --rm \
    awswrangler-build-py39 \
    build-lambda-layer.sh "${VERSION}-py3.9${ARCH_SUFFIX}" "ninja-build"
fi

# Python 3.10
if [[ $PYTHON_VERSION == "ALL" || $PYTHON_VERSION == "3.10" ]]
then
  docker run \
    --volume "$DIR_NAME":/aws-sdk-pandas/ \
    --workdir /aws-sdk-pandas/building/lambda \
    --rm \
    awswrangler-build-py310 \
    build-lambda-layer.sh "${VERSION}-py3.10${ARCH_SUFFIX}" "ninja-build"
fi

# Python 3.11
if [[ $PYTHON_VERSION == "ALL" || $PYTHON_VERSION == "3.11" ]]
then
  docker run \
    --volume "$DIR_NAME":/aws-sdk-pandas/ \
    --workdir /aws-sdk-pandas/building/lambda \
    --rm \
    awswrangler-build-py311 \
    build-lambda-layer.sh "${VERSION}-py3.11${ARCH_SUFFIX}" "ninja-build"
fi

# Python 3.12
if [[ $PYTHON_VERSION == "ALL" || $PYTHON_VERSION == "3.12" ]]
then
  docker run \
    --volume "$DIR_NAME":/aws-sdk-pandas/ \
    --workdir /aws-sdk-pandas/building/lambda \
    --rm \
    awswrangler-build-py312 \
    build-lambda-layer.sh "${VERSION}-py3.12${ARCH_SUFFIX}" "ninja-build"
fi
    strategy: #!/usr/bin/env bash
set -ex

pushd ..
rm -rf dist/*.whl
poetry build -f wheel 
      fail-fast: #!/usr/bin/env bash
set -ex

pushd ..
rm -fr dist
poetry publish --build
rm -fr dist
      matrix: #!/usr/bin/env bash
set -ex

pushd ..
rm -fr awswrangler.zip

# Zip the library
git archive HEAD:awswrangler --format zip --prefix awswrangler/awswrangler/ --output awswrangler.zip

# Upload the Zip file
s3_location=$(aws cloudformation describe-stacks --stack-name aws-sdk-pandas-glueray --query "Stacks[0].Outputs[?OutputKey=='AWSSDKforpandasZIPLocation'].OutputValue" --output text)
aws s3 cp awswrangler.zip $s3_location

rm -fr awswrangler.zip
        python-version: [ apiVersion: v1alpha
kind: playwright
sauce:
  region: us-west-1
  concurrency: 10 # Controls how many suites are executed at the same time.
  metadata:
    tags:
      - e2e
      - release team
      - other tag
playwright:
  version: package.json # See https://docs.saucelabs.com/web-apps/automated-testing/playwright/#supported-testing-platforms for a list of supported versions.
  configFile: playwright.config.js # See https://docs.saucelabs.com/web-apps/automated-testing/playwright/yaml/#configfile for a list of supported configuration files.
# Controls what files are available in the context of a test run (unless explicitly excluded by .sauceignore).
rootDir: ./tests/e2e
suites:
  - name: "Firefox Win"
    platformName: "Windows 11"
    screenResolution: "1440x900"
    testMatch: ['.*.js']
    params:
      browserName: "firefox"
      project: "firefox"  # Runs the project that's defined in `playwright.config.js`
  # - name: "Chromium Mac"
  #   platformName: "macOS 12"
  #   screenResolution: "1440x900"
  #   testMatch: ['.*.js']
  #   params:
  #     browserName: "chromium"
  #     project: "chromium"
  # - name: "Webkit Win"
  #   platformName: "Windows 11"
  #   screenResolution: "1440x900"
  #   testMatch: ['.*.js']
  #   params:
  #     browserName: "webkit"
  #     project: "webkit"
# Controls what artifacts to fetch when the suites have finished.
# artifacts:
#   download:
#     when: always
#     match:
#       - console.log
#     directory: ./artifacts/ ]

    steps: {
    // Use IntelliSense to learn about possible attributes.
    // Hover to view descriptions of existing attributes.
    // For more information, visit: https://go.microsoft.com/fwlink/?linkid=830387
    "version": "0.2.0",
    "configurations": [
        {
            "name": "Listen for Xdebug",
            "type": "php",
            "request": "launch",
            "port": 9003,
            "log": false,
            "pathMappings": {
                "/var/www/html/": "${workspaceFolder}"
            }
        }
    ]
}
    - uses: {
    "sqltools.connections": [
        {
            "mysqlOptions": {
                "authProtocol": "default",
                "enableSsl": "Disabled"
            },
            "previewLimit": 50,
            "server": "localhost",
            "port": 3306,
            "driver": "MariaDB",
            "name": "mariadb-docker",
            "database": "formulize",
            "username": "user"
        }
    ]
}
    - name: <script>history.go(-1);</script>
      uses: @media (max-width: 960px) {
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
      with: @font-face {
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
        python-version: html, body {
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
    - name: Install dependencies
      run: |
        python -m pip install --upgrade pip
        python -m pip install flake8 pytest
        if [ -f requirements.txt ]; then pip install -r requirements.txt; fi
    - name: Lint with flake8
      run: |
        # stop the build if there are Python syntax errors or undefined names
        flake8 . --count --select=E9,F63,F7,F82 --show-source --statistics
        # exit-zero treats all errors as warnings. The GitHub editor is 127 chars wide
        flake8 . --count --exit-zero --max-complexity=10 --max-line-length=127 --statistics
    - name: Test with pytest
      run: |
        pytest
