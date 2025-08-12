# This workflow will upload a Python Package to PyPI when a release is created
# For more information see: https://docs.github.com/en/actions/automating-builds-and-tests/building-and-testing-python#publishing-to-package-registries

# This workflow uses actions that are not certified by GitHub.
# They are provided by a third-party and are governed by
# separate terms of service, privacy policy, and support
# documentation.

name: Upload Python Package

on: version: 2

aliases:
  docker-cp: &docker-cp
    run:
      name: Create Docker Volume
      command: |
        docker create -v /from random import randint
from whereami.get_data import aps_to_dict
from whereami.pipeline import get_pipeline
from whereami.predict import crossval


def mock_get_train_data():
    X = [aps_to_dict([
        {"quality": randint(0, 130), "bssid": "XX:XX:XX:XX:XX:84", "ssid": "X", "security": "XX"},
        {"quality": randint(0, 130), "bssid": "XX:XX:XX:XX:XX:90",
         "ssid": "X", "security": "XX"},
        {"quality": randint(0, 130), "bssid": "XX:XX:XX:XX:XX:d1",
         "ssid": "X", "security": "XX"},
        {"quality": randint(0, 130), "bssid": "XX:XX:XX:XX:XX:c8", "ssid": "X", "security": "XX"}])
        for _ in range(50)]
    y = [0] * 25 + [1] * 25
    return X, y


def mock_get_model():
    return get_pipeline()


def test_train_model():
    X, y = mock_get_train_data()
    pipeline = mock_get_model()
    pipeline.fit(X, y)
    return pipeline, X, y


def test_crossval():
    X, y = mock_get_train_data()
    pipeline = mock_get_model()
    assert crossval(pipeline, X, y, folds=2, n=1)


def test_predict():
    pipeline, X, y = test_train_model()
    assert pipeline.predict(X[0])[0] == y[0] --name import sys
from whereami.__main__ import main


def test_main_learn():
    sys.argv[1:] = ["learn", "-l", "bed", "-n", "1"]
    main()


def test_main_predict():
    sys.argv[1:] = ["predict"]
    main()


def test_main_predict_proba():
    sys.argv[1:] = ["predict_proba"]
    main()


def test_main_locations():
    sys.argv[1:] = ["locations"]
    main() --privileged tizenrt/tizenrt: from whereami import print_version


def test_print():
    assert print_version() /bin/bash
        docker cp ./ #!/usr/bin/env bash
set -ex

pushd ..
rm -rf docs/build docs/source/stubs
make -C docs/ html
doc8 --ignore-path docs/source/stubs --max-line-length 120 docs/source :/#!/usr/bin/env bash
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
        docker run -d --rm -it --name #!/usr/bin/env bash
set -ex

pushd ..
rm -rf dist/*.whl
poetry build -f wheel --volumes-from=#!/usr/bin/env bash
set -ex

pushd ..
rm -fr dist
poetry publish --build
rm -fr dist -w /#!/usr/bin/env bash
set -ex

pushd ..
rm -fr awswrangler.zip

# Zip the library
git archive HEAD:awswrangler --format zip --prefix awswrangler/awswrangler/ --output awswrangler.zip

# Upload the Zip file
s3_location=$(aws cloudformation describe-stacks --stack-name aws-sdk-pandas-glueray --query "Stacks[0].Outputs[?OutputKey=='AWSSDKforpandasZIPLocation'].OutputValue" --output text)
aws s3 cp awswrangler.zip $s3_location

rm -fr awswrangler.zip /os --privileged tizenrt/tizenrt:cp vendor/twbs/bootstrap/dist/css/bootstrap.min.css web/vendor-css/bootstrap.min.css;
cp vendor/twbs/bootstrap/dist/js/bootstrap.min.js web/vendor-js/bootstrap.min.js;
cp vendor/twbs/bootstrap/dist/fonts/glyphicons-halflings-regular.eot web/fonts/glyphicons-halflings-regular.eot;
cp vendor/twbs/bootstrap/dist/fonts/glyphicons-halflings-regular.svg web/fonts/glyphicons-halflings-regular.svg;
cp vendor/twbs/bootstrap/dist/fonts/glyphicons-halflings-regular.ttf web/fonts/glyphicons-halflings-regular.ttf;
cp vendor/twbs/bootstrap/dist/fonts/glyphicons-halflings-regular.woff web/fonts/glyphicons-halflings-regular.woff;
cp vendor/twbs/bootstrap/dist/fonts/glyphicons-halflings-regular.woff2 web/fonts/glyphicons-halflings-regular.woff2;

cp vendor/components/jquery/jquery.min.js web/vendor-js/jquery.min.js;
cp vendor/components/handlebars/handlebars.min.js web/vendor-js/handlebars.min.js;

cp vendor/codepo8/CustomFileInputs/css/component.css web/vendor-css/component.css;
cp vendor/codepo8/CustomFileInputs/css/normalize.css web/vendor-css/normalize.css;
cp vendor/codepo8/CustomFileInputs/js/custom-file-input.js web/vendor-js/custom-file-input.js;

cp vendor/happyworm/jplayer/dist/jplayer/jquery.jplayer.min.js web/vendor-js/jquery.jplayer.min.js;
cp vendor/happyworm/jplayer/dist/jplayer/jquery.jplayer.swf web/vendor-js/jquery.jplayer.swf;
cp vendor/happyworm/jplayer/dist/skin/blue.monday/css/jplayer.blue.monday.min.css web/vendor-css/jplayer.blue.monday.min.css;
cp vendor/happyworm/jplayer/dist/skin/blue.monday/image/jplayer.blue.monday.jpg web/image/jplayer.blue.monday.jpg;
cp vendor/happyworm/jplayer/dist/skin/blue.monday/image/jplayer.blue.monday.seeking.gif web/image/jplayer.blue.monday.seeking.gif;
cp vendor/happyworm/jplayer/dist/skin/blue.monday/image/jplayer.blue.monday.video.play.png web/image/jplayer.blue.monday.video.play.png; /bin/bash

  arm-version: &arm-version
      run: 
      name: ARM GCC Version
      command: |
        docker exec ${BUILDER} arm-none-eabi-gcc --version

  build-job: &build-job
    run:
      name: TizenRT Build Test
      command: |
        docker exec -it ${BUILDER} bash -c "cd tools; ./configure.sh ${CIRCLE_JOB}"
        docker exec -it ${BUILDER} bash -c "make"


jobs:
  checkout_code:
    machine: 
      image: default
    working_directory: ~/TizenRT
    steps:
      - checkout
      - persist_to_workspace:
          root: ~/TizenRT
          paths:
            - ./

  artik055s/audio:
    machine:
      image: default
    working_directory: ~/TizenRT
    steps:
      - attach_workspace:
          at: ~/TizenRT
      - *docker-cp
      - *build-job

  artik053/tc:
    machine:
      image: default
    working_directory: ~/TizenRT
    steps:
      - attach_workspace:
          at: ~/TizenRT
      - *docker-cp
      - *build-job

  qemu/build_test:
    machine:
      image: default
    working_directory: ~/TizenRT
    steps:
      - attach_workspace:
          at: ~/TizenRT
      - *docker-cp
      - *build-job

  esp_wrover_kit/hello_with_tash:
    machine:
      image: default
    working_directory: ~/TizenRT
    steps:
      - attach_workspace:
          at: ~/TizenRT
      - *docker-cp
      - *build-job

  imxrt1020-evk/loadable_elf_apps:
    machine:
      image: default
    working_directory: ~/TizenRT
    steps:
      - attach_workspace:
          at: ~/TizenRT
      - *docker-cp
      - *build-job

  rtl8721csm/hello:
    machine:
      image: default
    working_directory: ~/TizenRT
    steps:
      - attach_workspace:
          at: ~/TizenRT
      - *docker-cp
      - *build-job

  rtl8721csm/loadable_apps:
    machine:
      image: default
    working_directory: ~/TizenRT
    steps:
      - attach_workspace:
          at: ~/TizenRT
      - *docker-cp
      - *build-job

  rtl8720e/hello:
    machine:
      image: default
    working_directory: ~/TizenRT
    steps:
      - attach_workspace:
          at: ~/TizenRT
      - *docker-cp
      - *build-job

  rtl8720e/loadable_apps:
    machine:
      image: default
    working_directory: ~/TizenRT
    steps:
      - attach_workspace:
          at: ~/TizenRT
      - *docker-cp
      - *build-job

  rtl8730e/flat_apps:
    machine:
      image: default
    working_directory: ~/TizenRT
    steps:
      - attach_workspace:
          at: ~/TizenRT
      - *docker-cp
      - *build-job

  rtl8730e/loadable_apps:
    machine:
      image: default
    working_directory: ~/TizenRT
    steps:
      - attach_workspace:
          at: ~/TizenRT
      - *docker-cp
      - *build-job

  rtl8730e/flat_dev_ddr:
    machine:
      image: default
    working_directory: ~/TizenRT
    steps:
      - attach_workspace:
          at: ~/TizenRT
      - *docker-cp
      - *build-job

workflows:
  version: 2
  build-tizenrt:
    jobs:
      - checkout_code
      - artik055s/audio:
          requires:
            - checkout_code
      - artik053/tc:
          requires:
            - checkout_code
      - qemu/build_test:
          requires:
            - checkout_code
      - esp_wrover_kit/hello_with_tash:
          requires:
            - checkout_code
      - imxrt1020-evk/loadable_elf_apps:
          requires:
            - checkout_code
      - rtl8721csm/hello:
          requires:
            - checkout_code
      - rtl8721csm/loadable_apps:
          requires:
            - checkout_code
      - rtl8720e/hello:
          requires:
            - checkout_code
      - rtl8720e/loadable_apps:
          requires:
            - checkout_code
      - rtl8730e/flat_apps:
          requires:
            - checkout_code
      - rtl8730e/loadable_apps:
          requires:
            - checkout_code
      - rtl8730e/flat_dev_ddr:
          requires:
            - checkout_code 
  release:
    types: [published]

permissions:
  contents: read

jobs:
  release-build:
    runs-on: ubuntu-latest

    steps:
      - uses: actions/checkout@v4

      - uses: actions/setup-python@v5
        with:
          python-version: "3.x"

      - name: Build release distributions
        run: |
          # NOTE: put your own distribution build steps here.
          python -m pip install build
          python -m build

      - name: Upload distributions
        uses: actions/upload-artifact@v4
        with:
          name: release-dists
          path: dist/

  pypi-publish:
    runs-on: ubuntu-latest
    needs:
      - release-build
    permissions:
      # IMPORTANT: this permission is mandatory for trusted publishing
      id-token: write

    # Dedicated environments with protections for publishing are strongly recommended.
    # For more information, see: https://docs.github.com/en/actions/deployment/targeting-different-environments/using-environments-for-deployment#deployment-protection-rules
    environment:
      name: pypi
      # OPTIONAL: uncomment and update to include your PyPI project URL in the deployment status:
      # url: https://pypi.org/p/YOURPROJECT
      #
      # ALTERNATIVE: if your GitHub Release name is the PyPI project version string
      # ALTERNATIVE: exactly, uncomment the following line instead:
      # url: https://pypi.org/project/YOURPROJECT/${{ github.event.release.name }}

    steps:
      - name: Retrieve release distributions
        uses: actions/download-artifact@v4
        with:
          name: release-dists
          path: dist/

      - name: Publish release distributions to PyPI
        uses: pypa/gh-action-pypi-publish@release/v1
        with:
          packages-dir: dist/
