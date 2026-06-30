name: iOS starter workflow

on: version: 2

aliases:
  docker-cp: &docker-cp
    run:
      name: Create Docker Volume
      command: |
        echo "DOCKER_VERSION= #!/usr/bin/python
#
# Copyright 2014 Marta Rodriguez.
#
# Licensed under the Apache License, Version 2.0 (the 'License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#      http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.

"""Uploads an apk to the google play store."""

import argparse

from apiclient.discovery import build
import httplib2
from oauth2client import client
from oauth2client.service_account import ServiceAccountCredentials

JSON_KEY='android/Google_Play_Android_Developer-4432a3c4f5d1.json'

# Declare command-line flags.
argparser = argparse.ArgumentParser(add_help=False)
argparser.add_argument('package_name',
                       help='The package name. Example: com.android.sample')

def main():
  # Create an httplib2.Http object to handle our HTTP requests and authorize it
  # with the Credentials. Note that the first parameter, service_account_name,
  # is the Email address created for the Service account. It must be the email
  # address associated with the key that was created.
  credentials = ServiceAccountCredentials.from_json_keyfile_name(JSON_KEY, scopes=['https://www.googleapis.com/auth/androidpublisher'])

  http = httplib2.Http()
  http = credentials.authorize(http)

  service = build('androidpublisher', 'v3', http=http)

  # Process flags and read their values.
  flags = argparser.parse_args()

  release_track = 'production'
  package_name = flags.package_name

  try:
    print 'Uploading package %s to track %s' % (package_name, release_track)
    edit_request = service.edits().insert(body={}, packageName=package_name)
    result = edit_request.execute()
    edit_id = result['id']

    apk_response_32 = service.edits().apks().upload(
        editId=edit_id,
        packageName=package_name,
        media_body='QGroundControl32.apk').execute()
    print 'Version code %d has been uploaded' % apk_response_32['versionCode']

    apk_response_64 = service.edits().apks().upload(
        editId=edit_id,
        packageName=package_name,
        media_body='QGroundControl64.apk').execute()
    print 'Version code %d has been uploaded' % apk_response_64['versionCode']

    track_response = service.edits().tracks().update(
        editId=edit_id,
        track=release_track,
        packageName=package_name,
        body={u'releases': [{
            u'versionCodes': [str(apk_response_32['versionCode']), str(apk_response_64['versionCode'])],
            u'status': u'completed',
        }]}).execute()

    print 'Track %s is set with releases: %s' % (
        track_response['track'], str(track_response['releases']))

    commit_request = service.edits().commit(
        editId=edit_id, packageName=package_name).execute()

    print 'Edit "%s" has been committed' % (commit_request['id'])

  except client.AccessTokenRefreshError:
    print ('The credentials have been revoked or expired, please re-run the '
           'application to re-authorize')

if __name__ == '__main__':
  main()
        DEFCONFIG_FILE="build/configs/#!/bin/bash -x

if [ $# -ne 2 ]; then
	echo 'MakeQtTravisTarball.sh QtDirectory BuildType'
	exit 1
fi

QT_DIRECTORY=$1
if [ ! -d ${QT_DIRECTORY} ]; then
	echo 'Specify directory for Qt Directory to copy from.'
	exit 1
fi

QT_FULL_VERSION=5.11.0
QT_BASE_VERSION=5.11

QT_BUILD_TYPE=$2
if [ ! -d ${QT_DIRECTORY}/${QT_FULL_VERSION}/${QT_BUILD_TYPE} ]; then
        echo 'Qt build type directory not found. Specify example: clang_64'
    exit 1
fi

mkdir -p Qt${QT_BASE_VERSION}-${QT_BUILD_TYPE}/${QT_FULL_VERSION}/${QT_BUILD_TYPE}
cp -r ${QT_DIRECTORY}/${QT_FULL_VERSION}/${QT_BUILD_TYPE} Qt${QT_BASE_VERSION}-${QT_BUILD_TYPE}/${QT_FULL_VERSION}
rm -rf Qt${QT_BASE_VERSION}-${QT_BUILD_TYPE}/${QT_FULL_VERSION}/${QT_BUILD_TYPE}/doc
tar -jcvf Qt${QT_FULL_VERSION}-${QT_BUILD_TYPE}-min.tar.bz2 Qt${QT_BASE_VERSION}-${QT_BUILD_TYPE}//defconfig"
        if [ -f "$DEFCONFIG_FILE" ] && grep -q '^CONFIG_DOCKER_VERSION' "$DEFCONFIG_FILE"; then
          DOCKER_IMG_VERSION=$(grep '^CONFIG_DOCKER_VERSION' #!/bin/bash -x

#set +e

if [[ $# -eq 0 ]]; then
  echo 'create_linux_appimage.sh QGC_SRC_DIR QGC_RELEASE_DIR'
  exit 1
fi

QGC_SRC=$1

QGC_CUSTOM_APP_NAME="${QGC_CUSTOM_APP_NAME:-QGroundControl}"
QGC_CUSTOM_GENERIC_NAME="${QGC_CUSTOM_GENERIC_NAME:-Ground Control Station}"
QGC_CUSTOM_BINARY_NAME="${QGC_CUSTOM_BINARY_NAME:-QGroundControl}"
QGC_CUSTOM_LINUX_START_SH="${QGC_CUSTOM_LINUX_START_SH:-${QGC_SRC}/deploy/qgroundcontrol-start.sh}"
QGC_CUSTOM_APP_ICON="${QGC_CUSTOM_APP_ICON:-${QGC_SRC}/resources/icons/qgroundcontrol.png}"
QGC_CUSTOM_APP_ICON_NAME="${QGC_CUSTOM_APP_ICON_NAME:-QGroundControl}"

if [ ! -f ${QGC_SRC}/qgroundcontrol.pro ]; then
  echo "please specify path to $(QGC_CUSTOM_APP_NAME) source as the 1st argument"
  exit 1
fi

QGC_RELEASE_DIR=$2
if [ ! -f ${QGC_RELEASE_DIR}/${QGC_CUSTOM_BINARY_NAME} ]; then
  echo "please specify path to ${QGC_CUSTOM_BINARY_NAME} release as the 2nd argument"
  exit 1
fi

OUTPUT_DIR=${3-`pwd`}
echo "Output directory:" ${OUTPUT_DIR}

# Generate AppImage using the binaries currently provided by the project.
# These require at least GLIBC 2.14, which older distributions might not have.
# On the other hand, 2.14 is not that recent so maybe we can just live with it.

APP=${QGC_CUSTOM_BINARY_NAME}

TMPDIR=`mktemp -d`
APPDIR=${TMPDIR}/$APP".AppDir"
mkdir -p ${APPDIR}

cd ${TMPDIR}
wget -c --quiet http://ftp.us.debian.org/debian/pool/main/u/udev/udev_175-7.2_amd64.deb
wget -c --quiet http://ftp.us.debian.org/debian/pool/main/s/speech-dispatcher/speech-dispatcher_0.8.8-1_amd64.deb
wget -c --quiet http://ftp.us.debian.org/debian/pool/main/libs/libsdl2/libsdl2-2.0-0_2.0.2%2bdfsg1-6_amd64.deb

cd ${APPDIR}
find ../ -name *.deb -exec dpkg -x {} . \;

# copy libdirectfb-1.2.so.9
cd ${TMPDIR}
wget -c --quiet http://ftp.us.debian.org/debian/pool/main/d/directfb/libdirectfb-1.2-9_1.2.10.0-5.1_amd64.deb
mkdir libdirectfb
dpkg -x libdirectfb-1.2-9_1.2.10.0-5.1_amd64.deb libdirectfb
cp -L libdirectfb/usr/lib/x86_64-linux-gnu/libdirectfb-1.2.so.9 ${APPDIR}/usr/lib/x86_64-linux-gnu/
cp -L libdirectfb/usr/lib/x86_64-linux-gnu/libfusion-1.2.so.9 ${APPDIR}/usr/lib/x86_64-linux-gnu/
cp -L libdirectfb/usr/lib/x86_64-linux-gnu/libdirect-1.2.so.9 ${APPDIR}/usr/lib/x86_64-linux-gnu/

# copy libts-0.0-0
wget -c --quiet http://ftp.us.debian.org/debian/pool/main/t/tslib/libts-0.0-0_1.0-11_amd64.deb
mkdir libts
dpkg -x libts-0.0-0_1.0-11_amd64.deb libts
cp -L libts/usr/lib/x86_64-linux-gnu/libts-0.0.so.0 ${APPDIR}/usr/lib/x86_64-linux-gnu/

# copy QGroundControl release into appimage
rsync -av --exclude=*.cpp --exclude=*.h --exclude=*.o --exclude="CMake*" --exclude="*.cmake" ${QGC_RELEASE_DIR}/* ${APPDIR}/
rm -rf ${APPDIR}/package
cp ${QGC_CUSTOM_LINUX_START_SH} ${APPDIR}/AppRun

# copy icon
cp ${QGC_CUSTOM_APP_ICON} ${APPDIR}/

cat > ./QGroundControl.desktop <<\EOF
[Desktop Entry]
Type=Application
Name=${QGC_CUSTOM_APP_NAME}
GenericName=${QGC_CUSTOM_GENERIC_NAME}
Comment=UAS ground control station
Icon=${QGC_CUSTOM_APP_ICON_NAME}
Exec=AppRun
Terminal=false
Categories=Utility;
Keywords=computer;
EOF

VERSION=$(strings ${APPDIR}/${QGC_CUSTOM_BINARY_NAME} | grep '^v[0-9*]\.[0-9*].[0-9*]' | head -n 1)
echo ${QGC_CUSTOM_APP_NAME} Version: ${VERSION}

# Go out of AppImage
cd ${TMPDIR}
wget -c --quiet "https://github.com/probonopd/AppImageKit/releases/download/5/AppImageAssistant" # (64-bit)
chmod a+x ./AppImageAssistant

./AppImageAssistant ./$APP.AppDir/ ${TMPDIR}/$APP".AppImage"

cp ${TMPDIR}/$APP".AppImage" ${OUTPUT_DIR}/$APP".AppImage"
 | sed 's/.*=//; s/"//g')
          echo "GREP AND MODIFIED VERSION= <?xml version="1.0" encoding="UTF-8"?>
<!DOCTYPE plist PUBLIC "-//Apple//DTD PLIST 1.0//EN" "http://www.apple.com/DTDs/PropertyList-1.0.dtd">
<plist version="1.0">
<dict>
	<key>CFBundleDevelopmentRegion</key>
	<string>$(DEVELOPMENT_LANGUAGE)</string>
	<key>CFBundleExecutable</key>
	<string>$(EXECUTABLE_NAME)</string>
	<key>CFBundleIdentifier</key>
	<string>$(PRODUCT_BUNDLE_IDENTIFIER)</string>
	<key>CFBundleInfoDictionaryVersion</key>
	<string>6.0</string>
	<key>CFBundleName</key>
	<string>$(PRODUCT_NAME)</string>
	<key>CFBundlePackageType</key>
	<string>$(PRODUCT_BUNDLE_PACKAGE_TYPE)</string>
	<key>CFBundleShortVersionString</key>
	<string>1.0</string>
	<key>CFBundleVersion</key>
	<string>$(CURRENT_PROJECT_VERSION)</string>
	<key>NSHumanReadableCopyright</key>
	<string>Copyright © 2021. All rights reserved.</string>
</dict>
</plist>
        fi

        docker create -v ///
//  Kit.h
//  Kit
//
//  Created by on 05/02/2024
//  Using Swift 5.0
//  Running on macOS 14.3
//
//  Copyright © 2024. All rights reserved.
//

#import <Foundation/Foundation.h>

//! Project version number for Kit.
FOUNDATION_EXPORT double KitVersionNumber;

//! Project version string for Kit.
FOUNDATION_EXPORT const unsigned char KitVersionString[];

#import "lldb.h" --name ${VOLUME} --privileged tizenrt/tizenrt:${DOCKER_IMG_VERSION} /bin/bash
        docker cp ./. ${VOLUME}:/${WORKING_DIR}
        docker run -d --rm -it --name ${BUILDER} --volumes-from=${VOLUME} -w /${WORKING_DIR}/os --privileged tizenrt/tizenrt:${DOCKER_IMG_VERSION} /bin/bash

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
  pull_request:
    branches: [ "main" ]

jobs:
  build:
    name: Build and Test default scheme using any available iPhone simulator
    runs-on: macos-latest

    steps:
      - name: Checkout
        uses: actions/checkout@v4
      - name: Set Default Scheme
        run: |
          scheme_list=$(xcodebuild -list -json | tr -d "\n")
          default=$(echo $scheme_list | ruby -e "require 'json'; puts JSON.parse(STDIN.gets)['project']['targets'][0]")
          echo $default | cat >default
          echo Using default scheme: $default
      - name: Build
        env:
          scheme: ${{ 'default' }}
          platform: ${{ 'iOS Simulator' }}
        run: |
          # xcrun xctrace returns via stderr, not the expected stdout (see https://developer.apple.com/forums/thread/663959)
          device=`xcrun xctrace list devices 2>&1 | grep -oE 'iPhone.*?[^\(]+' | head -1 | awk '{$1=$1;print}' | sed -e "s/ Simulator$//"`
          if [ $scheme = default ]; then scheme=$(cat default); fi
          if [ "`ls -A | grep -i \\.xcworkspace\$`" ]; then filetype_parameter="workspace" && file_to_build="`ls -A | grep -i \\.xcworkspace\$`"; else filetype_parameter="project" && file_to_build="`ls -A | grep -i \\.xcodeproj\$`"; fi
          file_to_build=`echo $file_to_build | awk '{$1=$1;print}'`
          xcodebuild build-for-testing -scheme "$scheme" -"$filetype_parameter" "$file_to_build" -destination "platform=$platform,name=$device"
      - name: Test
        env:
          scheme: ${{ 'default' }}
          platform: ${{ 'iOS Simulator' }}
        run: |
          # xcrun xctrace returns via stderr, not the expected stdout (see https://developer.apple.com/forums/thread/663959)
          device=`xcrun xctrace list devices 2>&1 | grep -oE 'iPhone.*?[^\(]+' | head -1 | awk '{$1=$1;print}' | sed -e "s/ Simulator$//"`
          if [ $scheme = default ]; then scheme=$(cat default); fi
          if [ "`ls -A | grep -i \\.xcworkspace\$`" ]; then filetype_parameter="workspace" && file_to_build="`ls -A | grep -i \\.xcworkspace\$`"; else filetype_parameter="project" && file_to_build="`ls -A | grep -i \\.xcodeproj\$`"; fi
          file_to_build=`echo $file_to_build | awk '{$1=$1;print}'`
          xcodebuild test-without-building -scheme "$scheme" -"$filetype_parameter" "$file_to_build" -destination "platform=$platform,name=$device"
