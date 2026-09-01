# This workflow will build and push a new container image to Amazon ECR,
# and then will deploy a new task definition to Amazon ECS, when there is a push to the "main" branch.
#
# To use this workflow, you will need to complete the following set-up steps:
#
# 1. Create an ECR repository to store your images.
#    For example: `aws ecr create-repository --repository-name my-ecr-repo --region us-east-2`.
#    Replace the value of the `ECR_REPOSITORY` environment variable in the workflow below with your repository's name.
#    Replace the value of the `AWS_REGION` environment variable in the workflow below with your repository's region.
#
# 2. Create an ECS task definition, an ECS cluster, and an ECS service.
#    For example, follow the Getting Started guide on the ECS console:
#      https://us-east-2.console.aws.amazon.com/ecs/home?region=us-east-2#/firstRun
#    Replace the value of the `ECS_SERVICE` environment variable in the workflow below with the name you set for the Amazon ECS service.
#    Replace the value of the `ECS_CLUSTER` environment variable in the workflow below with the name you set for the cluster.
#
# 3. Store your ECS task definition as a JSON file in your repository.
#    The format should follow the output of `aws ecs register-task-definition --generate-cli-skeleton`.
#    Replace the value of the `ECS_TASK_DEFINITION` environment variable in the workflow below with the path to the JSON file.
#    Replace the value of the `CONTAINER_NAME` environment variable in the workflow below with the name of the container
#    in the `containerDefinitions` section of the task definition.
#
# 4. Store an IAM user access key in GitHub Actions secrets named `AWS_ACCESS_KEY_ID` and `AWS_SECRET_ACCESS_KEY`.
#    See the documentation for each action used below for the recommended IAM policies for this IAM user,
#    and best practices on handling the access key credentials.

name: Deploy to Amazon ECS

on: version: 2

aliases:
  docker-cp: &docker-cp
    run:
      name: Create Docker Volume
      command: |
        echo "DOCKER_VERSION 
        DEFCONFIG_FILE="build/configs/ARG base_image
ARG python_version=base

FROM ${base_image} AS base

RUN yum install -y \
    boost-devel \
    jemalloc-devel \
    libxml2-devel \
    libxslt-devel \
    libatomic \
    bison \
    make \
    gcc10 \
    gcc10-c++ \
    flex \
    autoconf \
    zip \
    git \
    ninja-build

WORKDIR /root

ENV CC=/usr/bin/gcc10-cc
ENV CXX=/usr/bin/gcc10-c++
ENV LD=/usr/bin/gcc10-gcc

RUN ln -s /usr/bin/gcc10-gcc /usr/bin/gcc
RUN ln -s /usr/bin/gcc10-gcc /usr/bin/cc
RUN ln -s /usr/bin/gcc10-g++ /usr/bin/g++
RUN ln -s /usr/bin/gcc10-nm /usr/bin/nm
RUN ln -s /usr/bin/gcc10-ar /usr/bin/ar
RUN ln -s /usr/bin/gcc10-mpn /usr/bin/mpn
RUN ln -s /usr/bin/gcc10-ld /usr/bin/ld

FROM ${python_version}
COPY pyproject.toml uv.lock ./

# Setuptools is a runtime dependency of some of our dependencies (mainly redshift-connector).
RUN pip3 install --upgrade pip wheel setuptools>=78.1.1 setuptools_scm>=8
RUN pip3 install --upgrade urllib3==1.26.16  # temporary to avoid https://github.com/urllib3/urllib3/issues/2168 (TODO remove when the AL2 image updates to support OpenSSL 1.1.1+)
# In new CMake 4, compatibility with CMake < 3.5 has been removed.
# Unpin CMake when arrow version shipped with lambda layers is updated.
RUN pip3 install --upgrade cmake==3.31.6
# libcst (a pyarrow 24+ build dep, used for build-time stub docstring extraction)
# only ships manylinux_2_28 aarch64 wheels and falls back to a Rust build on AL2's
# glibc 2.26. Install rustup so cargo is on PATH for that source build.
RUN curl --proto '=https' --tlsv1.2 -sSf https://sh.rustup.rs | \
    sh -s -- -y --profile minimal --default-toolchain stable
ENV PATH="/root/.cargo/bin:${PATH}"

# pyarrow 24+ build deps: scikit-build-core replaces setuptools; cython>=3.1 and
# libcst (used for build-time stub docstring extraction) are required.
RUN pip3 install --upgrade six "cython>=3.1" hypothesis uv build scikit-build-core "libcst>=1.8.6"

ENV PIP_NO_BINARY="numpy,pandas"
ENV UV_PROJECT_ENVIRONMENT="/var/lang"
RUN uv sync --frozen --inexact --no-dev --no-install-project \
    --no-binary-package numpy \
    --no-binary-package pandas

RUN rm -f pyproject.toml uv.lock

ENTRYPOINT ["/bin/sh"]/defconfig"
        if [ -f "$DEFCONFIG_FILE" ] && grep -q '^CONFIG_DOCKER_VERSION' "$DEFCONFIG_FILE"; then
          DOCKER_IMG_VERSION=$(grep '^CONFIG_DOCKER_VERSION' ARG base_image
ARG python_version=base

FROM ${base_image} AS base

RUN dnf install -y \
    boost-devel \
    jemalloc-devel \
    libxml2-devel \
    libxslt-devel \
    libatomic \
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
COPY pyproject.toml uv.lock ./

# Setuptools is a runtime dependency of some of our dependencies (mainly redshift-connector).
RUN pip3 install --upgrade pip wheel setuptools>=78.1.1 setuptools_scm>=8
# In new CMake 4, compatibility with CMake < 3.5 has been removed.
# Unpin CMake when arrow version shipped with lambda layers is updated.
RUN pip3 install --upgrade cmake==3.31.6
# pyarrow 24+ build deps: scikit-build-core replaces setuptools; cython>=3.1 and
# libcst (used for build-time stub docstring extraction) are required.
RUN pip3 install --upgrade six "cython>=3.1" hypothesis uv build scikit-build-core "libcst>=1.8.6"

# AL2023 has glibc 2.34, which satisfies the manylinux_2_28 tag that PyPI
# ships numpy/pandas wheels under. Building numpy/pandas from source here
# without locking the build-time numpy to the runtime numpy produces an
# ABI-mismatched pandas binary that SIGSEGVs on datetime64 construction
# (issue #3393). The AL2-based Dockerfile still forces source builds because
# AL2's glibc 2.26 can't load manylinux_2_28 wheels.
ENV UV_PROJECT_ENVIRONMENT="/var/lang"
RUN uv sync --frozen --inexact --no-dev --no-install-project

RUN rm -f pyproject.toml uv.lock

ENTRYPOINT ["/bin/sh"] | sed 's/.*=//; s/"//g')
          echo "GREP AND MODIFIED VERSION #!/usr/bin/env bash
set -ex

cp ../../pyproject.toml .
cp ../../uv.lock .

export DOCKER_BUILDKIT=1

PYTHON_VERSION=${1:-ALL}

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

# Python 3.13
if [[ $PYTHON_VERSION == "ALL" || $PYTHON_VERSION == "3.13" ]]
then
  docker build \
    --pull \
    --tag awswrangler-build-py313 \
    --build-arg base_image=public.ecr.aws/lambda/python:3.13 \
    --file Dockerfile.al2023 \
    .
fi

# Python 3.14
if [[ $PYTHON_VERSION == "ALL" || $PYTHON_VERSION == "3.14" ]]
then
  docker build \
    --pull \
    --tag awswrangler-build-py314 \
    --build-arg base_image=public.ecr.aws/lambda/python:3.14 \
    --file Dockerfile.al2023 \
    .
fi

rm -rf pyproject.toml uv.lock
        fi

        docker create -v /#!/usr/bin/env bash
set -ex

FILENAME="awswrangler-layer-${1}.zip"
NINJA=${2}

pushd /aws-sdk-pandas
rm -rf python dist/pyarrow_files "dist/${FILENAME}" "${FILENAME}"
popd

rm -rf dist arrow

export ARROW_HOME=$(pwd)/dist
export ARROW_VERSION=24.0.0
export LD_LIBRARY_PATH=$(pwd)/dist/lib:$LD_LIBRARY_PATH
export CMAKE_PREFIX_PATH=$ARROW_HOME:$CMAKE_PREFIX_PATH
export SETUPTOOLS_SCM_PRETEND_VERSION=$ARROW_VERSION

git clone \
  --depth 1 \
  --branch "apache-arrow-${ARROW_VERSION}" \
  --single-branch \
  https://github.com/apache/arrow.git

mkdir $ARROW_HOME
mkdir arrow/cpp/build
pushd arrow/cpp/build

# -Wl,--as-needed drops DT_NEEDED entries for libraries whose symbols are never
# referenced. On AL2023, boost-devel pulls libicu into the linker's library
# path; without --as-needed, libparquet.so picks up libicudata/libicui18n/
# libicuuc as DT_NEEDED even though Arrow uses zero ICU symbols. Lambda's
# AL2023 runtime doesn't ship libicu, so the resulting layer fails to import.
cmake \
    -DCMAKE_INSTALL_PREFIX=$ARROW_HOME \
    -DCMAKE_INSTALL_LIBDIR=lib \
    -DCMAKE_SHARED_LINKER_FLAGS="-Wl,--as-needed" \
    -DCMAKE_EXE_LINKER_FLAGS="-Wl,--as-needed" \
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
    -DARROW_WITH_ICU=OFF \
    -GNinja \
    ..

eval $NINJA
eval "${NINJA} install"

popd

pushd arrow/python

export CMAKE_PREFIX_PATH=${ARROW_HOME}${CMAKE_PREFIX_PATH:+:${CMAKE_PREFIX_PATH}}
export PYARROW_BUNDLE_ARROW_CPP=ON
export PYARROW_WITH_HDFS=0
export PYARROW_WITH_FLIGHT=0
export PYARROW_WITH_GANDIVA=0
export PYARROW_WITH_ORC=0
export PYARROW_WITH_CUDA=0
export PYARROW_WITH_PARQUET=1
export PYARROW_WITH_DATASET=1

# pyarrow 24+ dropped setup.py in favour of scikit-build-core via pyproject.toml.
# --no-isolation reuses build deps already installed in the image (scikit-build-core,
# cython>=3.1, libcst, setuptools_scm, numpy) instead of pip resolving them again.
python3 -m build --wheel . --no-isolation \
  -C cmake.build-type=Release

pip3 install dist/pyarrow-*.whl -t /aws-sdk-pandas/dist/pyarrow_files

PYARROW_WHEEL_DIR=$(pwd)/dist

popd

pushd /aws-sdk-pandas

# Building numpy/pandas from source on AL2023 without pinning the build-time
# numpy to the runtime numpy produces an ABI-mismatched pandas binary that
# SIGSEGVs on datetime64 construction (issue #3393). AL2023's glibc 2.34
# satisfies the manylinux_2_28 tag that PyPI ships numpy/pandas wheels under,
# so use those wheels. AL2 (glibc 2.26) is too old to load manylinux_2_28,
# so keep forcing source builds there.
NO_BINARY_FLAG=""
if [ -r /etc/os-release ] && grep -q '^VERSION_ID="2"' /etc/os-release; then
  NO_BINARY_FLAG="--no-binary numpy,pandas"
fi

pip3 install . ${NO_BINARY_FLAG} --find-links="${PYARROW_WHEEL_DIR}" -t ./python ".[redshift,mysql,postgres,gremlin,opensearch,openpyxl]" "pyarrow==${ARROW_VERSION}"

# CVE-2026-41066: upgrade lxml past redshift-connector's <=6.0.2 cap.
# pyproject.toml's [tool.uv] override-dependencies only applies to uv, not pip,
# so the Lambda layer needs this force-upgrade. Remove the capped install
# first — pip -t doesn't clean old dist-info, so scanners would see both versions.
rm -rf python/lxml python/lxml-*.dist-info
pip3 install --no-deps -t ./python "lxml>=6.1.0"

rm -rf python/pyarrow*
rm -rf python/boto*
rm -rf python/urllib3*
rm -rf python/s3transfer*

cp -r /aws-sdk-pandas/dist/pyarrow_files/pyarrow* python/

# Removing nonessential files
# Skip auditwheel-vendored libraries (numpy.libs/, scipy.libs/, ...): these are
# pre-built shared objects (e.g. libscipy_openblas64_.so) whose LOAD segments use
# large-page alignment. GNU strip rewrites their file offsets without preserving
# the ELF invariant p_offset ≡ p_vaddr (mod p_align), which mmap-based loaders
# tolerate but Lambda's loader rejects with "ELF load command address/offset not
# page-aligned" (issue: numpy C-extension import fails on Lambda). Our own
# extension modules (awswrangler/pandas/pyarrow) are safe to strip.
find python -name '*.so' -type f -not -path '*.libs/*' -exec strip "{}" \;
find python -wholename "*/tests/*" -type f -delete
find python -regex '^.*\(__pycache__\|\.py[co]\)$' -delete

# Bundle system shared libraries needed at runtime (e.g. libxslt for lxml,
# libatomic for pyarrow 22+). Lambda extracts layers to /opt/ and /opt/lib
# is on LD_LIBRARY_PATH. Search ldconfig cache first, then fall back to a
# filesystem search (libatomic under gcc10 lives in /usr/lib/gcc/*).
# ICU libraries are intentionally excluded: Arrow is built with -DARROW_WITH_ICU=OFF
# so pyarrow has no ICU dependency, keeping the layer size minimal.
mkdir -p lib
for libfile in libxslt.so.1 libexslt.so.0 libatomic.so.1; do
  src=$(ldconfig -p 2>/dev/null | awk -v lib="${libfile}" '$1 == lib { print $NF; exit }')
  if [ -z "${src}" ] || [ ! -e "${src}" ]; then
    src=$(find /usr/lib /usr/lib64 -name "${libfile}" -print -quit 2>/dev/null)
  fi
  if [ -n "${src}" ] && [ -e "${src}" ]; then
    cp -L "${src}" "lib/${libfile}"
    echo "bundled ${libfile} from ${src}"
  else
    echo "WARNING: ${libfile} not found on this image"
  fi
done

# Strip symbol tables and debug info to reduce binary size
find lib -name '*.so*' -type f -exec strip "{}" \;

zip -r9 "${FILENAME}" ./python ./lib
mv "${FILENAME}" dist/

rm -rf python lib dist/pyarrow_files "${FILENAME}"

popd

rm -rf dist arrow --name #!/usr/bin/env bash
set -ex

pushd ..
rm -rf docs/build docs/source/stubs
make -C docs/ html
doc8 --ignore-path docs/source/stubs --max-line-length 120 docs/source --privileged tizenrt/tizenrt:#!/usr/bin/env bash
set -ex

VERSION=$(uv version --short)
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

# Python 3.13
if [[ $PYTHON_VERSION == "ALL" || $PYTHON_VERSION == "3.13" ]]
then
  docker run \
    --volume "$DIR_NAME":/aws-sdk-pandas/ \
    --workdir /aws-sdk-pandas/building/lambda \
    --rm \
    awswrangler-build-py313 \
    build-lambda-layer.sh "${VERSION}-py3.13${ARCH_SUFFIX}" "ninja-build"
fi

# Python 3.14
if [[ $PYTHON_VERSION == "ALL" || $PYTHON_VERSION == "3.14" ]]
then
  docker run \
    --volume "$DIR_NAME":/aws-sdk-pandas/ \
    --workdir /aws-sdk-pandas/building/lambda \
    --rm \
    awswrangler-build-py314 \
    build-lambda-layer.sh "${VERSION}-py3.14${ARCH_SUFFIX}" "ninja-build"
fi /bin/bash
        docker cp ./. #!/usr/bin/env bash
set -ex

pushd ..
rm -rf dist/*.whl
uv build --wheel:/#!/usr/bin/env bash
set -ex

pushd ..
rm -rf dist/*.whl
uv build --wheel --sdist
uv publish dist/*.whl
        docker run -d --rm -it --name #!/usr/bin/env bash
set -ex

pushd ..
rm -fr awswrangler.zip

# Zip the library
git archive HEAD:awswrangler --format zip --prefix awswrangler/awswrangler/ --output awswrangler.zip

# Upload the Zip file
s3_location=$(aws cloudformation describe-stacks --stack-name aws-sdk-pandas-glueray --query "Stacks[0].Outputs[?OutputKey=='AWSSDKforpandasZIPLocation'].OutputValue" --output text)
aws s3 cp awswrangler.zip $s3_location

rm -fr awswrangler.zip --volumes-from #include "winver.h"

IDI_ICON1               ICON    DISCARDABLE     "@ICON_FILE@"

VS_VERSION_INFO VERSIONINFO
 FILEVERSION @PROJECT_VERSION_MAJOR@,@PROJECT_VERSION_MINOR@,@PROJECT_VERSION_PATCH@,@PROJECT_VERSION_TWEAK@
 PRODUCTVERSION @PROJECT_VERSION_MAJOR@,@PROJECT_VERSION_MINOR@,@PROJECT_VERSION_PATCH@,@PROJECT_VERSION_TWEAK@
 FILEFLAGS 0x0L
 FILEFLAGSMASK 0x3fL
 FILEOS 0x00040004L
 FILETYPE 0x1L
 FILESUBTYPE 0x0L
BEGIN
    BLOCK "StringFileInfo"
    BEGIN
        BLOCK "000004b0"
        BEGIN
            VALUE "CompanyName", "@COMPANY@"
            VALUE "FileDescription", "@PROJECT_NAME@"
            VALUE "FileVersion", "@PROJECT_VERSION@"
            VALUE "LegalCopyright", "@COPYRIGHT@"
            VALUE "InternalName", "@PROJECT_NAME@"
            VALUE "OriginalFilename", "@PROJECT_NAME@.exe"
            VALUE "ProductName", "@PROJECT_NAME@"
            VALUE "ProductVersion", "@PROJECT_VERSION@"
        END
    END
END
 -w /${WORKING_DIR}/os --privileged tizenrt/tizenrt:${DOCKER_IMG_VERSION} /bin/bash

  arm-version: &arm-version
    run:
      name: ARM GCC Version
      command: |
        docker exec ${BUILDER} arm-none-eabi-gcc --version

  build-job: &build-job
    run:
      name: TizenRT Build Test
      command: |
        docker exec -it ${BUILDER} bash -c "cd tools; ./configure.sh ${CONFIG_PATH}"
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
            
  artik055s_audio:
    machine:
      image: default
    working_directory: ~/TizenRT
    environment:
      CONFIG_PATH: artik055s/audio
    steps:
      - attach_workspace:
          at: ~/TizenRT
      - *docker-cp
      - *build-job

  artik053_tc:
    machine:
      image: default
    working_directory: ~/TizenRT
    environment:
      CONFIG_PATH: artik053/tc
    steps:
      - attach_workspace:
          at: ~/TizenRT
      - *docker-cp
      - *build-job

  qemu_build_test:
    machine:
      image: default
    working_directory: ~/TizenRT
    environment:
      CONFIG_PATH: qemu/build_test
    steps:
      - attach_workspace:
          at: ~/TizenRT
      - *docker-cp
      - *build-job

  esp_wrover_kit_hello_with_tash:
    machine:
      image: default
    working_directory: ~/TizenRT
    environment:
      CONFIG_PATH: esp_wrover_kit/hello_with_tash
    steps:
      - attach_workspace:
          at: ~/TizenRT
      - *docker-cp
      - *build-job

  imxrt1020-evk_loadable_elf_apps:
    machine:
      image: default
    working_directory: ~/TizenRT
    environment:
      CONFIG_PATH: imxrt1020-evk/loadable_elf_apps
    steps:
      - attach_workspace:
          at: ~/TizenRT
      - *docker-cp
      - *build-job

  rtl8721csm_hello:
    machine:
      image: default
    working_directory: ~/TizenRT
    environment:
      CONFIG_PATH: rtl8721csm/hello
    steps:
      - attach_workspace:
          at: ~/TizenRT
      - *docker-cp
      - *build-job

  rtl8721csm_loadable_apps:
    machine:
      image: default
    working_directory: ~/TizenRT
    environment:
      CONFIG_PATH: rtl8721csm/loadable_apps
    steps:
      - attach_workspace:
          at: ~/TizenRT
      - *docker-cp
      - *build-job

  rtl8730e_flat_apps:
    machine:
      image: default
    working_directory: ~/TizenRT
    environment:
      CONFIG_PATH: rtl8730e/flat_apps
    steps:
      - attach_workspace:
          at: ~/TizenRT
      - *docker-cp
      - *build-job

  rtl8730e_flat_dev_ddr:
    machine:
      image: default
    working_directory: ~/TizenRT
    environment:
      CONFIG_PATH: rtl8730e/flat_dev_ddr
    steps:
      - attach_workspace:
          at: ~/TizenRT
      - *docker-cp
      - *build-job

  rtl8730e_flat_dev_ddr_nand:
    machine:
      image: default
    working_directory: ~/TizenRT
    environment:
      CONFIG_PATH: rtl8730e/flat_dev_ddr_nand
    steps:
      - attach_workspace:
          at: ~/TizenRT
      - *docker-cp
      - *build-job

  rtl8730e_flat_dev_psram:
    machine:
      image: default
    working_directory: ~/TizenRT
    environment:
      CONFIG_PATH: rtl8730e/flat_dev_psram
    steps:
      - attach_workspace:
          at: ~/TizenRT
      - *docker-cp
      - *build-job

  rtl8730e_loadable_apps:
    machine:
      image: default
    working_directory: ~/TizenRT
    environment:
      CONFIG_PATH: rtl8730e/loadable_apps
    steps:
      - attach_workspace:
          at: ~/TizenRT
      - *docker-cp
      - *build-job

  rtl8730e_loadable_ext_ddr:
    machine:
      image: default
    working_directory: ~/TizenRT
    environment:
      CONFIG_PATH: rtl8730e/loadable_ext_ddr
    steps:
      - attach_workspace:
          at: ~/TizenRT
      - *docker-cp
      - *build-job

  rtl8730e_loadable_ext_ddr_st7785:
    machine:
      image: default
    working_directory: ~/TizenRT
    environment:
      CONFIG_PATH: rtl8730e/loadable_ext_ddr_st7785
    steps:
      - attach_workspace:
          at: ~/TizenRT
      - *docker-cp
      - *build-job

  rtl8730e_loadable_ext_psram:
    machine:
      image: default
    working_directory: ~/TizenRT
    environment:
      CONFIG_PATH: rtl8730e/loadable_ext_psram
    steps:
      - attach_workspace:
          at: ~/TizenRT
      - *docker-cp
      - *build-job

  bk7239n_hello:
    machine:
      image: default
    working_directory: ~/TizenRT
    environment:
      CONFIG_PATH: bk7239n/hello
    steps:
      - attach_workspace:
          at: ~/TizenRT
      - *docker-cp
      - *build-job

  bk7239n_loadable_apps:
    machine:
      image: default
    working_directory: ~/TizenRT
    environment:
      CONFIG_PATH: bk7239n/loadable_apps
    steps:
      - attach_workspace:
          at: ~/TizenRT
      - *docker-cp
      - *build-job

  bk7239n_xip_all:
    machine:
      image: default
    working_directory: ~/TizenRT
    environment:
      CONFIG_PATH: bk7239n/xip_all
    steps:
      - attach_workspace:
          at: ~/TizenRT
      - *docker-cp
      - *build-job

  bk7239n_loadable_all:
    machine:
      image: default
    working_directory: ~/TizenRT
    environment:
      CONFIG_PATH: bk7239n/loadable_all
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
      - artik055s_audio:
          requires:
            - checkout_code
      - artik053_tc:
          requires:
            - checkout_code
      - qemu_build_test:
          requires:
            - checkout_code
      - esp_wrover_kit_hello_with_tash:
          requires:
            - checkout_code
      - imxrt1020-evk_loadable_elf_apps:
          requires:
            - checkout_code
      - rtl8721csm_hello:
          requires:
            - checkout_code
      - rtl8721csm_loadable_apps:
          requires:
            - checkout_code
      - rtl8730e_flat_apps:
          requires:
            - checkout_code
      - rtl8730e_flat_dev_ddr:
          requires:
            - checkout_code
      - rtl8730e_flat_dev_ddr_nand:
          requires:
            - checkout_code
      - rtl8730e_flat_dev_psram:
          requires:
            - checkout_code
      - rtl8730e_loadable_apps:
          requires:
            - checkout_code
      - rtl8730e_loadable_ext_ddr:
          requires:
            - checkout_code
      - rtl8730e_loadable_ext_ddr_st7785:
          requires:
            - checkout_code
      - rtl8730e_loadable_ext_psram:
          requires:
            - checkout_code
      - bk7239n_hello:
          requires:
            - checkout_code
      - bk7239n_loadable_apps:
          requires:
            - checkout_code
      - bk7239n_xip_all:
          requires:
            - checkout_code
      - bk7239n_loadable_all:
          requires:
            - checkout_code
  push:
    branches: [ "main" ]

env:
  AWS_REGION: MY_AWS_REGION                   # set this to your preferred AWS region, e.g. us-west-1
  ECR_REPOSITORY: MY_ECR_REPOSITORY           # set this to your Amazon ECR repository name
  ECS_SERVICE: MY_ECS_SERVICE                 # set this to your Amazon ECS service name
  ECS_CLUSTER: MY_ECS_CLUSTER                 # set this to your Amazon ECS cluster name
  ECS_TASK_DEFINITION: MY_ECS_TASK_DEFINITION # set this to the path to your Amazon ECS task definition
                                               # file, e.g. .aws/task-definition.json
  CONTAINER_NAME: MY_CONTAINER_NAME           # set this to the name of the container in the
                                               # containerDefinitions section of your task definition

permissions:
  contents: read

jobs:
  deploy:
    name: Deploy
    runs-on: ubuntu-latest
    environment: production

    steps:
    - name: Checkout
      uses: actions/checkout@v4

    - name: Configure AWS credentials
      uses: aws-actions/configure-aws-credentials@v1
      with:
        aws-access-key-id: ${{ secrets.AWS_ACCESS_KEY_ID }}
        aws-secret-access-key: ${{ secrets.AWS_SECRET_ACCESS_KEY }}
        aws-region: ${{ env.AWS_REGION }}

    - name: Login to Amazon ECR
      id: login-ecr
      uses: aws-actions/amazon-ecr-login@v1

    - name: Build, tag, and push image to Amazon ECR
      id: build-image
      env:
        ECR_REGISTRY: ${{ steps.login-ecr.outputs.registry }}
        IMAGE_TAG: ${{ github.sha }}
      run: |
        # Build a docker container and
        # push it to ECR so that it can
        # be deployed to ECS.
        docker build -t $ECR_REGISTRY/$ECR_REPOSITORY:$IMAGE_TAG .
        docker push $ECR_REGISTRY/$ECR_REPOSITORY:$IMAGE_TAG
        echo "image=$ECR_REGISTRY/$ECR_REPOSITORY:$IMAGE_TAG" >> $GITHUB_OUTPUT

    - name: Fill in the new image ID in the Amazon ECS task definition
      id: task-def
      uses: aws-actions/amazon-ecs-render-task-definition@v1
      with:
        task-definition: ${{ env.ECS_TASK_DEFINITION }}
        container-name: ${{ env.CONTAINER_NAME }}
        image: ${{ steps.build-image.outputs.image }}

    - name: Deploy Amazon ECS task definition
      uses: aws-actions/amazon-ecs-deploy-task-definition@v1
      with:
        task-definition: ${{ steps.task-def.outputs.task-definition }}
        service: ${{ env.ECS_SERVICE }}
        cluster: ${{ env.ECS_CLUSTER }}
        wait-for-service-stability: true
