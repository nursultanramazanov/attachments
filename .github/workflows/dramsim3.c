# This starter workflow is for a CMake project running on multiple platforms. There is a different starter workflow if you just want a single platform.
# See: https://github.com/actions/starter-workflows/blob/main/ci/cmake-single-platform.yml
name: CMake on multiple platforms

on: version: 2

aliases:
  docker-cp: &docker-cp
    run:
      name: Create Docker Volume
      command: |
        docker create -v /{
  "variant": {
    "label": "Debug",
    "keywordSettings": {
      "buildType": "debug"
    },
    "description": "Emit debug information without performing optimizations"
  },
  "activeEnvironments": []
} --name {
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
} --privileged tizenrt/tizenrt:{
    "files.associations": {
        "array": "cpp",
        "initializer_list": "cpp"
    }
} /bin/bash
        docker cp ./. {
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
}:/[dram_structure]
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

        docker run -d --rm -it --name [dram_structure]
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
 --volumes-from=[dram_structure]
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
 -w /[dram_structure]
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
/os --privileged tizenrt/tizenrt:${DOCKER_IMG_VERSION} /bin/bash

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

  rtl8730e/loadable_ext_ddr:
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
      - rtl8730e/loadable_ext_ddr:
          requires:
            - checkout_code
  push:
    branches: [ "main" ]
  pull_request:
    branches: [ "main" ]

jobs:
  build:
    runs-on: ${{ matrix.os }}

    strategy:
      # Set fail-fast to false to ensure that feedback is delivered for all matrix combinations. Consider changing this to true when your workflow is stable.
      fail-fast: false

      # Set up a matrix to run the following 3 configurations:
      # 1. <Windows, Release, latest MSVC compiler toolchain on the default runner image, default generator>
      # 2. <Linux, Release, latest GCC compiler toolchain on the default runner image, default generator>
      # 3. <Linux, Release, latest Clang compiler toolchain on the default runner image, default generator>
      #
      # To add more build types (Release, Debug, RelWithDebInfo, etc.) customize the build_type list.
      matrix:
        os: [ubuntu-latest, windows-latest]
        build_type: [Release]
        c_compiler: [gcc, clang, cl]
        include:
          - os: windows-latest
            c_compiler: cl
            cpp_compiler: cl
          - os: ubuntu-latest
            c_compiler: gcc
            cpp_compiler: g++
          - os: ubuntu-latest
            c_compiler: clang
            cpp_compiler: clang++
        exclude:
          - os: windows-latest
            c_compiler: gcc
          - os: windows-latest
            c_compiler: clang
          - os: ubuntu-latest
            c_compiler: cl

    steps:
    - uses: actions/checkout@v4

    - name: Set reusable strings
      # Turn repeated input strings (such as the build output directory) into step outputs. These step outputs can be used throughout the workflow file.
      id: strings
      shell: bash
      run: |
        echo "build-output-dir=${{ github.workspace }}/build" >> "$GITHUB_OUTPUT"

    - name: Configure CMake
      # Configure CMake in a 'build' subdirectory. `CMAKE_BUILD_TYPE` is only required if you are using a single-configuration generator such as make.
      # See https://cmake.org/cmake/help/latest/variable/CMAKE_BUILD_TYPE.html?highlight=cmake_build_type
      run: >
        cmake -B ${{ steps.strings.outputs.build-output-dir }}
        -DCMAKE_CXX_COMPILER=${{ matrix.cpp_compiler }}
        -DCMAKE_C_COMPILER=${{ matrix.c_compiler }}
        -DCMAKE_BUILD_TYPE=${{ matrix.build_type }}
        -S ${{ github.workspace }}

    - name: Build
      # Build your program with the given configuration. Note that --config is needed because the default Windows generator is a multi-config generator (Visual Studio generator).
      run: cmake --build ${{ steps.strings.outputs.build-output-dir }} --config ${{ matrix.build_type }}

    - name: Test
      working-directory: ${{ steps.strings.outputs.build-output-dir }}
      # Execute tests defined by the CMake configuration. Note that --build-config is needed because the default Windows generator is a multi-config generator (Visual Studio generator).
      # See https://cmake.org/cmake/help/latest/manual/ctest.1.html for more detail
      run: ctest --build-config ${{ matrix.build_type }}
