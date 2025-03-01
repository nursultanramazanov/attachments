# This workflow will upload a Python Package to PyPI when a release is created
# For more information see: https://docs.github.com/en/actions/automating-builds-and-tests/building-and-testing-python#publishing-to-package-registries

# This workflow uses actions that are not certified by GitHub.
# They are provided by a third-party and are governed by
# separate terms of service, privacy policy, and support
# documentation.

name: Upload Python Package

on: ARG base_image
ARG python_version=base

FROM {
  "variant": {
    "label": "Debug",
    "keywordSettings": {
      "buildType": "debug"
    },
    "description": "Emit debug information without performing optimizations"
  },
  "activeEnvironments": []
} AS base

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

FROM {
    "version": "0.2.0",
    "configurations": [
        
        {
            "name": "C++ Launch",
            "type": "cppdbg",
            "request": "launch",
            "program": "${workspaceRoot}/build/dramcoremain",
            "args": ["-c", "./../configs/ddr4_8G_x8_config.ini", "-n", "100000", "--trace-cpu", "--trace-file", "./../sample_trace.txt"],
            "stopAtEntry": true,
            "cwd": "${workspaceRoot}/build",
            "environment": [],
            "externalConsole": true,
            "linux": {
                "MIMode": "gdb",
                "setupCommands": [
                    {
                        "description": "Enable pretty-printing for gdb",
                        "text": "-enable-pretty-printing",
                        "ignoreFailures": true
                    }
                ]
            },
            "osx": {
                "MIMode": "lldb"
            },
            "windows": {
                "MIMode": "gdb",
                "setupCommands": [
                    {
                        "description": "Enable pretty-printing for gdb",
                        "text": "-enable-pretty-printing",
                        "ignoreFailures": true
                    }
                ]
            }
        },
        {
            "name": "C++ Attach",
            "type": "cppdbg",
            "request": "attach",
            "program": "enter program name, for example ${workspaceRoot}/a.out",
            "processId": "${command.pickProcess}",
            "linux": {
                "MIMode": "gdb",
                "setupCommands": [
                    {
                        "description": "Enable pretty-printing for gdb",
                        "text": "-enable-pretty-printing",
                        "ignoreFailures": true
                    }
                ]
            },
            "osx": {
                "MIMode": "lldb"
            },
            "windows": {
                "MIMode": "gdb",
                "setupCommands": [
                    {
                        "description": "Enable pretty-printing for gdb",
                        "text": "-enable-pretty-printing",
                        "ignoreFailures": true
                    }
                ]
            }
        }
    ]
}
COPY pyproject.toml poetry.lock ./

RUN pip3 install --upgrade pip wheel
RUN pip3 install --upgrade urllib3==1.26.16  # temporary to avoid https://github.com/urllib3/urllib3/issues/2168 (TODO remove when the AL2 image updates to support OpenSSL 1.1.1+)
RUN pip3 install --upgrade six cython==3.0.8 cmake hypothesis poetry
RUN poetry config virtualenvs.create false --local && poetry install --no-root --only main

RUN rm -f pyproject.toml poetry.lock

ENTRYPOINT ["/bin/sh"] 
  release: ARG base_image
ARG python_version=base

FROM {
    "files.associations": {
        "array": "cpp",
        "initializer_list": "cpp"
    }
} AS base

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

FROM {
    // See https://go.microsoft.com/fwlink/?LinkId=733558
    // for the documentation about the tasks.json format
    "version": "0.1.0",
    "command": "make",
    "isShellCommand": true,
    "args": ["-j5"],
    "showOutput": "always",
    "options": {
        "cwd": "${workspaceRoot}/build"
    }
} 
COPY pyproject.toml poetry.lock ./

RUN pip3 install --upgrade pip wheel
RUN pip3 install --upgrade six cython==3.0.8 cmake hypothesis poetry
RUN poetry config virtualenvs.create false --local && poetry install --no-root --only main

RUN rm -f pyproject.toml poetry.lock

ENTRYPOINT ["/bin/sh"] 
    types: #!/usr/bin/env bash
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

permissions: #!/usr/bin/env bash
set -ex

FILENAME="awswrangler-layer-[dram_structure]
protocol = DDR3
bankgroups = 1
banks_per_group = 8
rows = 16384
columns = 1024
device_width = 8
BL = 8

[timing]
tCK = 1.5
AL = 0
CL = 10
CWL = 7
tRCD = 10
tRP = 10
tRAS = 24
tRFC = 74
tREFI = 5200
tRPRE = 1
tWPRE = 1
tRRD_S = 4
tWTR_S = 5
tFAW = 20
tWR = 10
tCCD_S = 4
tRTP = 5
tCKE = 4
tCKESR = 1
tXS = 81
tXP = 5
tRTRS = 1

[power]
VDD = 1.35
IDD0 = 33
IDD2P = 12
IDD2N = 17
IDD3P = 14
IDD3N = 23
IDD4W = 77
IDD4R = 72
IDD5AB = 155
IDD6x = 12

[system]
channel_size = 2048
channels = 1
bus_width = 64
address_mapping = rochrababgco
queue_structure = PER_BANK
trans_queue_size = 32
row_buf_policy = OPEN_PAGE
cmd_queue_size = 8

[other]
epoch_period = 666666
output_level = 1

[thermal]
loc_mapping = 30,30,30,29:27,26:13,12:3
power_epoch_period = 10000; power epoch period (# cycle)
chip_dim_x = 0.008; chip size in x dimension [m]
chip_dim_y = 0.008; chip size in y dimension [m]
amb_temp = 40; The ambient temperature in [C]
mat_dim_x = 512;
mat_dim_y = 512;
bank_order = 1; 0: x direction first, 1: y direction first
.zip"
NINJA=[dram_structure]
protocol = DDR3
bankgroups = 1
banks_per_group = 8
rows = 32768
columns = 1024
device_width = 16
BL = 8

[timing]
tCK = 1.25
AL = 0
CL = 11
CWL = 8
tRCD = 11
tRP = 11
tRAS = 28
tRFC = 208
tRFC2 = 208
tRFC4 = 208
REFI = 6240
tRPRE = 0
tWPRE = 0
tRRD_S = 5
tRRD_L = 5
tWTR_S = 6
tWTR_L = 6
tFAW = 32
tWR = 12
tWR2 = 12
tRTP = 6
tCCD_S = 4
tCCD_L = 4
tCKE = 4
tCKESR = 5
tXS = 216
tXP = 5
tRTRS = 1

[power]
VDD = 1.35
IDD0 = 66
IPP0 = 0.0
IDD2P = 18
IDD2N = 32
IDD3P = 38
IDD3N = 47
IDD4W = 171
IDD4R = 235
IDD5AB = 235
IDD6x = 20

[system]
channel_size = 4096
channels = 1
bus_width = 64
address_mapping = rochrababgco
queue_structure = PER_BANK
refresh_policy = RANK_LEVEL_STAGGERED
row_buf_policy = OPEN_PAGE
cmd_queue_size = 8
trans_queue_size = 32

[other]
epoch_period = 800000
output_level = 1
 

pushd /aws-sdk-pandas
rm -rf python dist/pyarrow_files "dist/[dram_structure]
protocol = DDR3
bankgroups = 1
banks_per_group = 8
rows = 32768
columns = 1024
device_width = 16
BL = 8

[timing]
tCK = 1.07
AL = 0
CL = 13
CWL = 9
tRCD = 13
tRP = 13
tRAS = 32
tRFC = 243
tRFC2 = 243
tRFC4 = 243
REFI = 7290
tRPRE = 0
tWPRE = 0
tRRD_S = 6
tRRD_L = 6
tWTR_S = 7
tWTR_L = 7
tFAW = 33
tWR = 15
tWR2 = 15
tRTP = 7
tCCD_S = 4
tCCD_L = 4
tCKE = 5
tCKESR = 6
tXS = 253
tXP = 6
tRTRS = 1

[power]
VDD = 1.35
IDD0 = 73
IPP0 = 0.0
IDD2P = 18
IDD2N = 35
IDD3P = 41
IDD3N = 49
IDD4W = 190
IDD4R = 252
IDD5AB = 242
IDD6x = 20

[system]
channel_size = 4096
channels = 1
bus_width = 64
address_mapping = rochrababgco
queue_structure = PER_BANK
refresh_policy = RANK_LEVEL_STAGGERED
row_buf_policy = OPEN_PAGE
cmd_queue_size = 8
trans_queue_size = 32

[other]
epoch_period = 934579
output_level = 1
" "[dram_structure]
protocol = DDR3
bankgroups = 1
banks_per_group = 8
rows = 65536
columns = 2048
device_width = 4
BL = 8

[timing]
tCK = 1.25
AL = 0
CL = 11
CWL = 8
tRCD = 11
tRP = 11
tRAS = 28
tRFC = 208
tRFC2 = 208
tRFC4 = 208
REFI = 6240
tRPRE = 0
tWPRE = 0
tRRD_S = 5
tRRD_L = 5
tWTR_S = 6
tWTR_L = 6
tFAW = 24
tWR = 12
tWR2 = 12
tRTP = 6
tCCD_S = 4
tCCD_L = 4
tCKE = 4
tCKESR = 5
tXS = 216
tXP = 5
tRTRS = 1

[power]
VDD = 1.35
IDD0 = 55
IPP0 = 0.0
IDD2P = 18
IDD2N = 32
IDD3P = 38
IDD3N = 38
IDD4W = 118
IDD4R = 147
IDD5AB = 235
IDD6x = 20

[system]
channel_size = 16384
channels = 1
bus_width = 64
address_mapping = rochrababgco
queue_structure = PER_BANK
refresh_policy = RANK_LEVEL_STAGGERED
row_buf_policy = OPEN_PAGE
cmd_queue_size = 8
trans_queue_size = 32

[other]
epoch_period = 800000
output_level = 1
"
popd

rm -rf dist arrow

export ARROW_HOME=$(pwd)/dist
export LD_LIBRARY_PATH=$(pwd)/dist/lib:$LD_LIBRARY_PATH
export CMAKE_PREFIX_PATH=$ARROW_HOME:$CMAKE_PREFIX_PATH

git clone \
  --depth 1 \
  --branch apache-arrow-14.0.1 \
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
eval "[dram_structure]
protocol = DDR3
bankgroups = 1
banks_per_group = 8
rows = 65536
columns = 2048
device_width = 4
BL = 8

[timing]
tCK = 1.07
AL = 0
CL = 13
CWL = 9
tRCD = 13
tRP = 13
tRAS = 32
tRFC = 243
tRFC2 = 243
tRFC4 = 243
REFI = 7290
tRPRE = 0
tWPRE = 0
tRRD_S = 5
tRRD_L = 5
tWTR_S = 7
tWTR_L = 7
tFAW = 26
tWR = 15
tWR2 = 15
tRTP = 6
tCCD_S = 4
tCCD_L = 4
tCKE = 5
tCKESR = 6
tXS = 253
tXP = 6
tRTRS = 1

[power]
VDD = 1.35
IDD0 = 62
IPP0 = 0.0
IDD2P = 18
IDD2N = 35
IDD3P = 41
IDD3N = 41
IDD4W = 133
IDD4R = 164
IDD5AB = 242
IDD6x = 20

[system]
channel_size = 16384
channels = 1
bus_width = 64
address_mapping = rochrababgco
queue_structure = PER_BANK
refresh_policy = RANK_LEVEL_STAGGERED
row_buf_policy = OPEN_PAGE
cmd_queue_size = 8
trans_queue_size = 32

[other]
epoch_period = 934579
output_level = 1
 install"

popd

pushd arrow/python

export CMAKE_PREFIX_PATH=[dram_structure]
protocol = DDR3
bankgroups = 1
banks_per_group = 8
rows = 65536
columns = 1024
device_width = 8
BL = 8

[timing]
tCK = 1.25
AL = 0
CL = 11
CWL = 8
tRCD = 11
tRP = 11
tRAS = 28
tRFC = 208
tRFC2 = 208
tRFC4 = 208
REFI = 6240
tRPRE = 0
tWPRE = 0
tRRD_S = 5
tRRD_L = 5
tWTR_S = 6
tWTR_L = 6
tFAW = 24
tWR = 12
tWR2 = 12
tRTP = 6
tCCD_S = 4
tCCD_L = 4
tCKE = 4
tCKESR = 5
tXS = 216
tXP = 5
tRTRS = 1

[power]
VDD = 1.35
IDD0 = 55
IPP0 = 0.0
IDD2P = 18
IDD2N = 32
IDD3P = 38
IDD3N = 38
IDD4W = 125
IDD4R = 157
IDD5AB = 235
IDD6x = 20

[system]
channel_size = 8192
channels = 1
bus_width = 64
address_mapping = rochrababgco
queue_structure = PER_BANK
refresh_policy = RANK_LEVEL_STAGGERED
row_buf_policy = OPEN_PAGE
cmd_queue_size = 8
trans_queue_size = 32

[other]
epoch_period = 800000
output_level = 1
 [dram_structure]
protocol = DDR3
bankgroups = 1
banks_per_group = 8
rows = 65536
columns = 1024
device_width = 8
BL = 8

[timing]
tCK = 1.07
AL = 0
CL = 13
CWL = 9
tRCD = 13
tRP = 13
tRAS = 32
tRFC = 243
tRFC2 = 243
tRFC4 = 243
REFI = 7290
tRPRE = 0
tWPRE = 0
tRRD_S = 5
tRRD_L = 5
tWTR_S = 7
tWTR_L = 7
tFAW = 26
tWR = 15
tWR2 = 15
tRTP = 6
tCCD_S = 4
tCCD_L = 4
tCKE = 5
tCKESR = 6
tXS = 253
tXP = 6
tRTRS = 1

[power]
VDD = 1.35
IDD0 = 62
IPP0 = 0.0
IDD2P = 18
IDD2N = 35
IDD3P = 41
IDD3N = 41
IDD4W = 141
IDD4R = 174
IDD5AB = 242
IDD6x = 20

[system]
channel_size = 8192
channels = 1
bus_width = 64
address_mapping = rochrababgco
queue_structure = PER_BANK
refresh_policy = RANK_LEVEL_STAGGERED
row_buf_policy = OPEN_PAGE
cmd_queue_size = 8
trans_queue_size = 32

[other]
epoch_period = 934579
output_level = 1
:+:[dram_structure]
protocol = DDR3
bankgroups = 1
banks_per_group = 8
rows = 65536
columns = 1024
device_width = 16
BL = 8

[timing]
tCK = 1.25
AL = 0
CL = 11
CWL = 8
tRCD = 11
tRP = 11
tRAS = 28
tRFC = 280
tRFC2 = 280
tRFC4 = 280
REFI = 6240
tRPRE = 0
tWPRE = 0
tRRD_S = 6
tRRD_L = 6
tWTR_S = 6
tWTR_L = 6
tFAW = 32
tWR = 12
tWR2 = 12
tRTP = 6
tCCD_S = 4
tCCD_L = 4
tCKE = 4
tCKESR = 5
tXS = 288
tXP = 5
tRTRS = 1

[power]
VDD = 1.35
IDD0 = 67
IPP0 = 0.0
IDD2P = 11
IDD2N = 36
IDD3P = 36
IDD3N = 51
IDD4W = 185
IDD4R = 185
IDD5AB = 270
IDD6x = 24

[system]
channel_size = 8192
channels = 1
bus_width = 64
address_mapping = rochrababgco
queue_structure = PER_BANK
refresh_policy = RANK_LEVEL_STAGGERED
row_buf_policy = OPEN_PAGE
cmd_queue_size = 8
trans_queue_size = 32

[other]
epoch_period = 800000
output_level = 1

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

zip -r9 "[dram_structure]
protocol = DDR3
bankgroups = 1
banks_per_group = 8
rows = 65536
columns = 1024
device_width = 16
BL = 8

[timing]
tCK = 1.07
AL = 0
CL = 13
CWL = 9
tRCD = 13
tRP = 13
tRAS = 32
tRFC = 328
tRFC2 = 328
tRFC4 = 328
REFI = 7290
tRPRE = 0
tWPRE = 0
tRRD_S = 6
tRRD_L = 6
tWTR_S = 7
tWTR_L = 7
tFAW = 33
tWR = 15
tWR2 = 15
tRTP = 7
tCCD_S = 4
tCCD_L = 4
tCKE = 5
tCKESR = 6
tXS = 338
tXP = 6
tRTRS = 1

[power]
VDD = 1.35
IDD0 = 69
IPP0 = 0.0
IDD2P = 11
IDD2N = 38
IDD3P = 38
IDD3N = 53
IDD4W = 195
IDD4R = 195
IDD5AB = 275
IDD6x = 24

[system]
channel_size = 8192
channels = 1
bus_width = 64
address_mapping = rochrababgco
queue_structure = PER_BANK
refresh_policy = RANK_LEVEL_STAGGERED
row_buf_policy = OPEN_PAGE
cmd_queue_size = 8
trans_queue_size = 32

[other]
epoch_period = 934579
output_level = 1
" ./python
mv "[dram_structure]
protocol = DDR3
bankgroups = 1
banks_per_group = 8
rows = 65536
columns = 4096
device_width = 4
BL = 8

[timing]
tCK = 1.25
AL = 0
CL = 11
CWL = 8
tRCD = 11
tRP = 11
tRAS = 28
tRFC = 280
tRFC2 = 280
tRFC4 = 280
REFI = 6240
tRPRE = 0
tWPRE = 0
tRRD_S = 6
tRRD_L = 6
tWTR_S = 6
tWTR_L = 6
tFAW = 32
tWR = 12
tWR2 = 12
tRTP = 6
tCCD_S = 4
tCCD_L = 4
tCKE = 4
tCKESR = 5
tXS = 288
tXP = 5
tRTRS = 1

[power]
VDD = 1.35
IDD0 = 67
IPP0 = 0.0
IDD2P = 11
IDD2N = 36
IDD3P = 36
IDD3N = 51
IDD4W = 125
IDD4R = 125
IDD5AB = 245
IDD6x = 24

[system]
channel_size = 32768
channels = 1
bus_width = 64
address_mapping = rochrababgco
queue_structure = PER_BANK
refresh_policy = RANK_LEVEL_STAGGERED
row_buf_policy = OPEN_PAGE
cmd_queue_size = 8
trans_queue_size = 32

[other]
epoch_period = 800000
output_level = 1
" dist/

rm -rf python dist/pyarrow_files "[dram_structure]
protocol = DDR3
bankgroups = 1
banks_per_group = 8
rows = 65536
columns = 4096
device_width = 4
BL = 8

[timing]
tCK = 1.07
AL = 0
CL = 13
CWL = 9
tRCD = 13
tRP = 13
tRAS = 32
tRFC = 328
tRFC2 = 328
tRFC4 = 328
REFI = 7290
tRPRE = 0
tWPRE = 0
tRRD_S = 6
tRRD_L = 6
tWTR_S = 7
tWTR_L = 7
tFAW = 33
tWR = 15
tWR2 = 15
tRTP = 7
tCCD_S = 4
tCCD_L = 4
tCKE = 5
tCKESR = 6
tXS = 338
tXP = 6
tRTRS = 1

[power]
VDD = 1.35
IDD0 = 69
IPP0 = 0.0
IDD2P = 11
IDD2N = 38
IDD3P = 38
IDD3N = 53
IDD4W = 135
IDD4R = 135
IDD5AB = 250
IDD6x = 24

[system]
channel_size = 32768
channels = 1
bus_width = 64
address_mapping = rochrababgco
queue_structure = PER_BANK
refresh_policy = RANK_LEVEL_STAGGERED
row_buf_policy = OPEN_PAGE
cmd_queue_size = 8
trans_queue_size = 32

[other]
epoch_period = 934579
output_level = 1
"

popd

rm -rf dist arrow  
  contents: #!/usr/bin/env bash
set -ex

pushd ..
rm -rf docs/build docs/source/stubs
make -C docs/ html
doc8 --ignore-path docs/source/stubs --max-line-length 120 docs/source 

jobs: #!/usr/bin/env bash
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
  release-build: #!/usr/bin/env bash
set -ex

pushd ..
rm -rf dist/*.whl
poetry build -f wheel 
    runs-on: #!/usr/bin/env bash
set -ex

pushd ..
rm -fr dist
poetry publish --build
rm -fr dist 

    steps: #!/usr/bin/env bash
set -ex

pushd ..
rm -fr awswrangler.zip

# Zip the library
git archive HEAD:awswrangler --format zip --prefix awswrangler/awswrangler/ --output awswrangler.zip

# Upload the Zip file
s3_location=$(aws cloudformation describe-stacks --stack-name aws-sdk-pandas-glueray --query "Stacks[0].Outputs[?OutputKey=='AWSSDKforpandasZIPLocation'].OutputValue" --output text)
aws s3 cp awswrangler.zip $s3_location

rm -fr awswrangler.zip 
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
