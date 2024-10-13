# This starter workflow is for a CMake project running on a single platform. There is a different starter workflow if you need cross-platform coverage.
# See: https://github.com/actions/starter-workflows/blob/main/ci/cmake-multi-platform.yml
name: CMake on a single platform

on: // ---------------
// Basic benchmark
// ---------------

// Benchmark file's have to have <file_name>_test.go and use the Benchmark functions like below.
// The goal is to know what perform better and what allocate more or less between Sprint and Sprintf.

// Our guess is that Sprint is gonna be better because it doesn't have any overhead doing the
// formatting. However, this is not true. Remember we have to optimize for correctness so we don't
// want to guess.

// Run benchmark:
// go test -run none -bench . -benchmem -benchtime 3s

// Sample output:
// BenchmarkSprintBasic-8       50000000                78.7 ns/op             5 B/op          1 allocs/op
// BenchmarkSprintfBasic-8      100000000               60.5 ns/op             5 B/op          1 allocs/op

package main

import (
        "fmt"
        "testing"
)

var gs string

// BenchmarkSprint tests the performance of using Sprint.
// All the code we want to benchmark need to be inside the b.N for loop.
// The first time the tool call it, b.N is equal to 1. It will keep increasing the value of N and
// run long enough based on our bench time.
// fmt.Sprint returns a value and we want to capture this value so it doesn't look like dead code.
// We assign it to the global variable gs.
func BenchmarkSprintBasic(b *testing.B) {
        var s string

        for i := 0; i < b.N; i++ {
                s = fmt.Sprint("hello")
        }

        gs = s
}

// BenchmarkSprint tests the performance of using Sprintf.
func BenchmarkSprintfBasic(b *testing.B) {
        var s string

        for i := 0; i < b.N; i++ {
                s = fmt.Sprintf("hello")
        }

        gs = s
}
  push: // -------------
// Sub benchmark
// -------------

// Like sub test, we can also do sub benchmark.

// Sample available commands:
// go test -run none -bench . -benchtime 3s -benchmem
// go test -run none -bench BenchmarkSprintSub/none -benchtime 3s -benchmem
// go test -run none -bench BenchmarkSprintSub/format -benchtime 3s -benchmem

package main

import (
        "fmt"
        "testing"
)

var gs string

// BenchmarkSprint tests all the Sprint related benchmarks as sub benchmarks.
func BenchmarkSprintSub(b *testing.B) {
        b.Run("none", benchSprint)
        b.Run("format", benchSprintf)
}

// benchSprint tests the performance of using Sprint.
func benchSprint(b *testing.B) {
        var s string

        for i := 0; i < b.N; i++ {
                s = fmt.Sprint("hello")
        }

        gs = s
}

// benchSprintf tests the performance of using Sprintf.
func benchSprintf(b *testing.B) {
        var s string

        for i := 0; i < b.N; i++ {
                s = fmt.Sprintf("hello")
        }

        gs = s
}
    branches: [ "main" ]
  pull_request: #!/bin/bash -ex

export NDK_CCACHE=$(which ccache)

if [ ! -z "${ANDROID_KEYSTORE_B64}" ]; then
    export ANDROID_KEYSTORE_FILE="${GITHUB_WORKSPACE}/ks.jks"
    base64 --decode <<< "${ANDROID_KEYSTORE_B64}" > "${ANDROID_KEYSTORE_FILE}"
fi

cd src/android
chmod +x ./gradlew
./gradlew assembleRelease
./gradlew bundleRelease

ccache -s -v

if [ ! -z "${ANDROID_KEYSTORE_B64}" ]; then
    rm "${ANDROID_KEYSTORE_FILE}"
fi
    branches: [ "main" ]

env: #!/bin/bash -ex

if grep -nrI '\s$' src *.yml *.txt *.md Doxyfile .gitignore .gitmodules .ci* dist/*.desktop \
                 dist/*.svg dist/*.xml; then
    echo Trailing whitespace found, aborting
    exit 1
fi

# Default clang-format points to default 3.5 version one
CLANG_FORMAT=clang-format-18
$CLANG_FORMAT --version

if [ "$GITHUB_EVENT_NAME" = "pull_request" ]; then
    # Get list of every file modified in this pull request
    files_to_lint="$(git diff --name-only --diff-filter=ACMRTUXB $COMMIT_RANGE | grep '^src/[^.]*[.]\(cpp\|h\)$' || true)"
else
    # Check everything for branch pushes
    files_to_lint="$(find src/ -name '*.cpp' -or -name '*.h')"
fi

# Turn off tracing for this because it's too verbose
set +x

for f in $files_to_lint; do
    d=$(diff -u "$f" <($CLANG_FORMAT "$f") || true)
    if ! [ -z "$d" ]; then
        echo "!!! $f not compliant to coding style, here is the fix:"
        echo "$d"
        fail=1
    fi
done

set -x

if [ "$fail" = 1 ]; then
    exit 1
fi
  # Customize the CMake build type here (Release, Debug, RelWithDebInfo, etc.)
  BUILD_TYPE: #!/bin/bash -ex

if [ "$TARGET" = "appimage" ]; then
    export EXTRA_CMAKE_FLAGS=(-DCMAKE_LINKER=/etc/bin/ld.lld)
    # Bundle required QT wayland libraries
    export EXTRA_QT_PLUGINS="waylandcompositor"
    export EXTRA_PLATFORM_PLUGINS="libqwayland-egl.so;libqwayland-generic.so"
else
    # For the linux-fresh verification target, verify compilation without PCH as well.
    export EXTRA_CMAKE_FLAGS=(-DMANDARINE_USE_PRECOMPILED_HEADERS=OFF)
fi

mkdir build && cd build
cmake .. -G Ninja \
    -DCMAKE_BUILD_TYPE=Release \
    -DCMAKE_C_COMPILER_LAUNCHER=ccache \
    -DCMAKE_CXX_COMPILER_LAUNCHER=ccache \
    -DCMAKE_CXX_COMPILER=clang++-18 \
    -DCMAKE_C_COMPILER=clang-18 \
    "${EXTRA_CMAKE_FLAGS[@]}" \
    -DENABLE_QT_TRANSLATION=ON \
    -DUSE_DISCORD_PRESENCE=ON
ninja

if [ "$TARGET" = "appimage" ]; then
    ninja bundle
    # TODO: Our AppImage environment currently uses an older ccache version without the verbose flag.
    ccache -s
else
    ccache -s -v
fi

ctest -VV -C Release

jobs: #!/bin/bash -ex

ARTIFACTS_LIST=($ARTIFACTS)

BUNDLE_DIR=build/bundle
mkdir build

# Set up the base artifact to combine into.
BASE_ARTIFACT=${ARTIFACTS_LIST[0]}
BASE_ARTIFACT_ARCH="${BASE_ARTIFACT##*-}"
mv $BASE_ARTIFACT $BUNDLE_DIR

# Executable binary paths that need to be combined.
BIN_PATHS=(mandarine-room mandarine.app/Contents/MacOS/mandarine)

# Dylib paths that need to be combined.
IFS=$'\n'
DYLIB_PATHS=($(cd $BUNDLE_DIR && find . -name '*.dylib'))
unset IFS

# Combine all of the executable binaries and dylibs.
for OTHER_ARTIFACT in "${ARTIFACTS_LIST[@]:1}"; do
    OTHER_ARTIFACT_ARCH="${OTHER_ARTIFACT##*-}"

    for BIN_PATH in "${BIN_PATHS[@]}"; do
        lipo -create -output $BUNDLE_DIR/$BIN_PATH $BUNDLE_DIR/$BIN_PATH $OTHER_ARTIFACT/$BIN_PATH
    done

    for DYLIB_PATH in "${DYLIB_PATHS[@]}"; do
        # Only merge if the libraries do not have conflicting arches, otherwise it will fail.
        DYLIB_INFO=`file $BUNDLE_DIR/$DYLIB_PATH`
        OTHER_DYLIB_INFO=`file $OTHER_ARTIFACT/$DYLIB_PATH`
        if ! [[ "$DYLIB_INFO" =~ "$OTHER_ARTIFACT_ARCH" ]] && ! [[ "$OTHER_DYLIB_INFO" =~ "$BASE_ARTIFACT_ARCH" ]]; then
            lipo -create -output $BUNDLE_DIR/$DYLIB_PATH $BUNDLE_DIR/$DYLIB_PATH $OTHER_ARTIFACT/$DYLIB_PATH
        fi
    done
done

# Re-sign executables and bundles after combining.
APP_PATHS=(mandarine-room mandarine.app)
for APP_PATH in "${APP_PATHS[@]}"; do
    codesign --deep -fs - $BUNDLE_DIR/$APP_PATH
done
  build: #!/bin/bash -ex

mkdir build && cd build
cmake .. -GNinja \
    -DCMAKE_BUILD_TYPE=Release \
    -DCMAKE_OSX_ARCHITECTURES="$TARGET" \
    -DCMAKE_C_COMPILER_LAUNCHER=ccache \
    -DCMAKE_CXX_COMPILER_LAUNCHER=ccache \
    -DENABLE_QT_TRANSLATION=ON \
    -DUSE_DISCORD_PRESENCE=ON
ninja
ninja bundle

ccache -s -v

CURRENT_ARCH=`arch`
if [ "$TARGET" = "$CURRENT_ARCH" ]; then
  ctest -VV -C Release
fi
    # The CMake configure and build commands are platform agnostic and should work equally well on Windows or Mac.
    # You can convert this to a matrix build if you need cross-platform coverage.
    # See: https://docs.github.com/en/free-pro-team@latest/actions/learn-github-actions/managing-complex-workflows#using-a-build-matrix
    runs-on: #!/bin/bash -ex

# Determine the full revision name.
GITDATE="`git show -s --date=short --format='%ad' | sed 's/-//g'`"
GITREV="`git show -s --format='%h'`"
REV_NAME="mandarine-$OS-$TARGET-$GITDATE-$GITREV"

# Determine the name of the release being built.
if [[ "$GITHUB_REF_NAME" =~ ^canary- ]] || [[ "$GITHUB_REF_NAME" =~ ^nightly- ]]; then
    RELEASE_NAME=$(echo $GITHUB_REF_NAME | cut -d- -f1)
else
    RELEASE_NAME=head
fi

# Archive and upload the artifacts.
mkdir artifacts

function pack_artifacts() {
    ARTIFACTS_PATH="$1"

    # Set up root directory for archive.
    mkdir "$REV_NAME"
    if [ -f "$ARTIFACTS_PATH" ]; then
        mv "$ARTIFACTS_PATH" "$REV_NAME"

        # Use file extension to differentiate archives.
        FILENAME=$(basename "$ARTIFACT")
        EXTENSION="${FILENAME##*.}"
        ARCHIVE_NAME="$REV_NAME.$EXTENSION"
    else
        mv "$ARTIFACTS_PATH"/* "$REV_NAME"

        ARCHIVE_NAME="$REV_NAME"
    fi

    # Create .zip/.tar.gz
    if [ "$OS" = "windows" ]; then
        ARCHIVE_FULL_NAME="$ARCHIVE_NAME.zip"
        powershell Compress-Archive "$REV_NAME" "$ARCHIVE_FULL_NAME"
    elif [ "$OS" = "android" ] || [ "$OS" = "macos" ]; then
        ARCHIVE_FULL_NAME="$ARCHIVE_NAME.zip"
        zip -r "$ARCHIVE_FULL_NAME" "$REV_NAME"
    else
        ARCHIVE_FULL_NAME="$ARCHIVE_NAME.tar.gz"
        tar czvf "$ARCHIVE_FULL_NAME" "$REV_NAME"
    fi
    mv "$ARCHIVE_FULL_NAME" artifacts/
    # Clean up created rev artifacts directory.
    rm -rf "$REV_NAME"
}

if [ -n "$UNPACKED" ]; then
    # Copy the artifacts to be uploaded unpacked.
    for ARTIFACT in build/bundle/*; do
        FILENAME=$(basename "$ARTIFACT")
        EXTENSION="${FILENAME##*.}"

        mv "$ARTIFACT" "artifacts/$REV_NAME.$EXTENSION"
    done
elif [ -n "$PACK_INDIVIDUALLY" ]; then
    # Pack and upload the artifacts one-by-one.
    for ARTIFACT in build/bundle/*; do
        pack_artifacts "$ARTIFACT"
    done
else
    # Pack all of the artifacts into a single archive.
    pack_artifacts build/bundle
fi

    steps: #!/bin/bash -ex

GITDATE="`git show -s --date=short --format='%ad' | sed 's/-//g'`"
GITREV="`git show -s --format='%h'`"
REV_NAME="mandarine-unified-source-${GITDATE}-${GITREV}"

COMPAT_LIST='dist/compatibility_list/compatibility_list.json'

mkdir artifacts

pip3 install git-archive-all
touch "${COMPAT_LIST}"
git describe --abbrev=0 --always HEAD > GIT-COMMIT
git describe --tags HEAD > GIT-TAG || echo 'unknown' > GIT-TAG
git archive-all --include "${COMPAT_LIST}" --include GIT-COMMIT --include GIT-TAG --force-submodules artifacts/"${REV_NAME}.tar"

cd artifacts/
xz -T0 -9 "${REV_NAME}.tar"
sha256sum "${REV_NAME}.tar.xz" > "${REV_NAME}.tar.xz.sha256sum"
cd ..
    - uses: #!/bin/sh -ex

mkdir build && cd build
cmake .. -G Ninja \
    -DCMAKE_BUILD_TYPE=Release \
    -DCMAKE_C_COMPILER_LAUNCHER=ccache \
    -DCMAKE_CXX_COMPILER_LAUNCHER=ccache \
    -DENABLE_QT_TRANSLATION=ON \
    -DUSE_DISCORD_PRESENCE=ON
ninja
ninja bundle

ccache -s -v

ctest -VV -C Release || echo "::error ::Test error occurred on Windows build"

    - name: Configure CMake
      # Configure CMake in a 'build' subdirectory. `CMAKE_BUILD_TYPE` is only required if you are using a single-configuration generator such as make.
      # See https://cmake.org/cmake/help/latest/variable/CMAKE_BUILD_TYPE.html?highlight=cmake_build_type
      run: cmake -B ${{github.workspace}}/build -DCMAKE_BUILD_TYPE=${{env.BUILD_TYPE}}

    - name: Build
      # Build your program with the given configuration
      run: cmake --build ${{github.workspace}}/build --config ${{env.BUILD_TYPE}}

    - name: Test
      working-directory: ${{github.workspace}}/build
      # Execute tests defined by the CMake configuration.
      # See https://cmake.org/cmake/help/latest/manual/ctest.1.html for more detail
      run: ctest -C ${{env.BUILD_TYPE}}

