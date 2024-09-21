name: Rust

on: #!/usr/bin/python
#
# Copyright 2014 
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
  push: #!/bin/bash -x

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
tar -jcvf Qt${QT_FULL_VERSION}-${QT_BUILD_TYPE}-min.tar.bz2 Qt${QT_BASE_VERSION}-${QT_BUILD_TYPE}/
    branches: [ "main" ]
  pull_request: #!/bin/bash -x

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

    branches: [ "main" ]

env: [env]
__DEVTOOLS_LOCAL_DEVELOPMENT = "true" # disable gRPC CORS checking & point to the right URL for local development
  CARGO_TERM_COLOR: import fileinput
import sys
import os
import glob

def get_actual_filename(name):
    dirs = name.split('\\')
    # disk letter
    test_name = [dirs[0].upper()]
    for d in dirs[1:]:
        test_name += ["%s[%s]" % (d[:-1], d[-1])]
    res = glob.glob('\\'.join(test_name))
    if not res:
        #File not found
        return None
    return res[0]

filelist = [get_actual_filename(x.rstrip()) for x in fileinput.input(['-'])]
prefix_len = len(os.path.commonprefix(filelist))

print("""SRCSRV: ini ------------------------------------------------
VERSION=2
SRCSRV: variables ------------------------------------------
SRCSRVVERCTRL=https
SRCSRVTRG=https://raw.github.com/mavlink/qgroundcontrol/%s/%%var2%%
SRCSRV: source files ---------------------------------------""" % os.environ['APPVEYOR_REPO_COMMIT'])
for line in filelist:
    if line is not None:
        print('%s*%s' % (line, line[prefix_len:].replace('\\','/')))

print("SRCSRV: end ------------------------------------------------")

jobs: try { var browser = chrome } catch { }

browser.runtime.onMessage.addListener(
    function (request, sender, sendResponse) {

        if (request.message == "passe") {

            let [found, source] = getSource()
            console.log("Source:", source)
            let url = document.URL
            sendResponse({ found, source, url })

            return true
        }
    }
)
const getSource = () => {

    let source = ""
    const clear = (s) => {
        return s.split('?')[0].split('#')[0]
    }
    try {
        let innerHTML = document.body.innerHTML
        source = clear(innerHTML.split("player.vimeo.com/video/")[1].split('"')[0])
    } catch {
        try {
            for (let i of document.getElementsByTagName("iframe")) {
                let s = clear(i.src)
                if (s.split('/').slice(0, -1).join('/').length == 9) {
                    source = s.slice(-1)[0]
                }
            }
        } catch { }
        if (!source) {
            console.log("Not found error")
            return [false, ""]
        }
    }

    if (source.length != 9) { //May need enhancement
        console.log("Wrong size, no error")
        return [false, ""]
    }
    console.log('fine')
    return [true, source]
}

build: <html>
    <head>
        <title>Vimeo Details</title>
    </head>
    <body>
        <script src="./popup.js"></script>
        <h3 id="title">Nothin</h3>
        <div id="results">
            <p id="id_"></p>
            <p id="owner_name"></p>
            <p id="owner_account_type"></p>
            <button id="owner_url"></button>

            <p id="privacy"></p>

            <ul id=videos>

            </ul>

        </div>
    </body>
</html>

    runs-on: Add-Type -TypeDefinition @'
    using System;
    using System.Runtime.InteropServices;

    public class NativeMethods
    {
        [DllImport("kernel32.dll", EntryPoint="MoveFileW", SetLastError=true,
                   CharSet=CharSet.Unicode, ExactSpelling=true,
                   CallingConvention=CallingConvention.StdCall)]
        public static extern bool MoveFile(string lpExistingFileName, string lpNewFileName);
    }
'@

Get-ChildItem ".\symbols" -recurse | ForEach-Object {[NativeMethods]::MoveFile($_.FullName,[io.path]::combine((Split-Path $_.FullName -Parent),$_.Name.ToLower()))}

    steps: /*
author:t0pl

When popup gets clicked
    popup wakes content script up
        content script retreives ID
        content script sends it back to popup
    popup fetches data
popup gets dressed up

TODO
    Handle errors
*/
try { var browser = chrome } catch { }

var headers = {
    'Accept': '*/*',
    'Accept-Encoding': 'gzip, deflate, br',
    'Accept-Language': 'en-US,en;q=0.9',
    'DNT': '1',
    'Referer': '',
    'User-Agent': 'Mozilla/5.0 (Macintosh; Intel Mac OS X 10_14_6) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/87.0.4280.88 Safari/537.36'
}
browser.tabs.query({ active: true, currentWindow: true }, (tab) => {

    browser.tabs.sendMessage(tab[0].id, { message: "passe" }, res => {
        if (!res.found) {
            console.log("Nothin here")
            return
        }
        console.log("Received from content.js", res, res.source)

        /*Fetch data from vimeo*/
        const url = `https://player.vimeo.com/video/${res.source}`
        window.url = res.url
        window.source = res.source
        set_referer_in_headers()

        set_listener()
        fetch(url)
            .then(check_status_code)
            .then(res => {

                //Locate JSON
                let output = {}
                const cleared_JSON = vimeo_JSON(res)
                console.log(cleared_JSON)

                //Store interesting data
                const all_videos = get_videos(cleared_JSON)
                const title = cleared_JSON.video.title
                const owner = cleared_JSON.video.owner
                const privacy = cleared_JSON.video.privacy
                const source = window.source

                output = { all_videos, title, owner, privacy, source }

                /*Display data*/
                get_dressed(output)
            })
            .catch(err => {
                console.error(`${err} :: ${url}`)
            })
            .finally(() => {
                remove_listener()
            })
        return true
    })
})

const get_dressed = (output) => {

    //Title
    document.getElementById("title").textContent = output.title
    //Source
    document.getElementById("id_").textContent = output.source
    //Videos
    let ol_tag = document.getElementById('videos')
    for (let i = 0; i < output.all_videos.length; i++) {

        let li = document.createElement('li')

        //Each video has a button with its quality on it
        let btn_open_video = document.createElement('button')
        btn_open_video.textContent = output.all_videos[i].quality

        //and leads to matching url
        btn_open_video.onclick = () => {
            browser.tabs.create({ active: true, url: output.all_videos[i].url }, tab => {

            })
        }

        //Add url as plain text next to button
        li.textContent += output.all_videos[i].url
        li.appendChild(btn_open_video)
        ol_tag.appendChild(li)
    }

    //Owner
    document.getElementById("owner_name").textContent = output.owner.name
    document.getElementById("owner_account_type").textContent = output.owner.account_type
    let owner_url = document.getElementById("owner_url")
    owner_url.textContent = output.owner.url
    owner_url.onclick = () => {
        browser.tabs.create({ active: true, url: output.owner.url }, tab => {

        })
    }

    //Privacy
    document.getElementById("privacy").textContent = output.privacy
}

const check_status_code = response => {

    if (!response.ok) (console.warn(`${url} returned wrong status code: ${response.status}`));
    return response.text();

}

/* Parsing */
const vimeo_JSON = part => {

    //Locate JSON in response and Convert from Vimeo WebPage
    let located_json = part.split('"};')[0].split('= {"')[1];
    let cleared_json = JSON.parse(`{"${located_json}"}`);

    return cleared_json;
}

const get_videos = cleared_JSON => {
    let videos = [];

    for (var _ = 0; _ < cleared_JSON.request.files.progressive.length; _++) {
        let top = cleared_JSON.request.files.progressive[_]

        let new_vid = { quality: top.quality, url: top.url }

        videos.push(new_vid);
    }
    return videos;
}

/* Header stuff */
const set_referer_in_headers = () => {
    window.headers['Referer'] = window.url;
}

const set_listener = () => {
    browser.webRequest.onBeforeSendHeaders.addListener(
        onBeforeSendHeaders_callback, { urls: ["https://player.vimeo.com/*"] }, OnBeforeRequestOptions()
    );
}

const remove_listener = () => {
    browser.webRequest.onBeforeSendHeaders.removeListener(onBeforeSendHeaders_callback);
}

const modify_headers = (header_array, _name, _value) => { // Credits: https://stackoverflow.com/a/11602753
    var did_set = false;
    for (var i in header_array) {
        var header = header_array[i];
        var name = header.name;
        if (name == _name) {
            header.value = _value;
            did_set = true;
        }
    }
    if (!did_set) header_array.push({ name: _name, value: _value })
}

const onBeforeSendHeaders_callback = (details) => {
    //Fired to modify request headers
    Object.keys(window.headers).forEach(function (key) {
        modify_headers(details.requestHeaders, key, window.headers[key]);
    });

    return { requestHeaders: details.requestHeaders };
}

const isFirefox = () => {
    return browser.webRequest.getSecurityInfo !== undefined
}

const OnBeforeRequestOptions = () => {
    //Options differ in Chrome/Firefox
    return isFirefox() ? ['blocking', 'requestHeaders'] : ['blocking', 'requestHeaders', 'extraHeaders']
}
    - uses: #!/bin/sh
HERE="$(dirname "$(readlink -f "${0}")")"
export LD_LIBRARY_PATH="${HERE}/usr/lib/x86_64-linux-gnu":"${HERE}/Qt/libs":$LD_LIBRARY_PATH
export QML2_IMPORT_PATH="${HERE}/Qt/qml"
export QT_PLUGIN_PATH="${HERE}/Qt/plugins"

# hack until icon issue with AppImage is resolved
mkdir -p ~/.icons && cp ${HERE}/qgroundcontrol.png ~/.icons

"${HERE}/QGroundControl" "$@"
    - name: [Desktop Entry]
Type=Application
Name=QGroundControl
GenericName=Ground Control Station
Comment=UAS ground control station
Icon=qgroundcontrol
Exec=qgroundcontrol-start.sh
Terminal=false
Categories=Utility;
      run: !include "MUI2.nsh"
!include LogicLib.nsh
!include Win\COM.nsh
!include Win\Propkey.nsh
!include "FileFunc.nsh"

!macro DemoteShortCut target
    !insertmacro ComHlpr_CreateInProcInstance ${CLSID_ShellLink} ${IID_IShellLink} r0 ""
    ${If} $0 <> 0
            ${IUnknown::QueryInterface} $0 '("${IID_IPersistFile}",.r1)'
            ${If} $1 P<> 0
                    ${IPersistFile::Load} $1 '("${target}",1)'
                    ${IUnknown::Release} $1 ""
            ${EndIf}
            ${IUnknown::QueryInterface} $0 '("${IID_IPropertyStore}",.r1)'
            ${If} $1 P<> 0
                    System::Call '*${SYSSTRUCT_PROPERTYKEY}(${PKEY_AppUserModel_StartPinOption})p.r2'
                    System::Call '*${SYSSTRUCT_PROPVARIANT}(${VT_UI4},,&i4 ${APPUSERMODEL_STARTPINOPTION_NOPINONINSTALL})p.r3'
                    ${IPropertyStore::SetValue} $1 '($2,$3)'

                    ; Reuse the PROPERTYKEY & PROPVARIANT buffers to set another property
                    System::Call '*$2${SYSSTRUCT_PROPERTYKEY}(${PKEY_AppUserModel_ExcludeFromShowInNewInstall})'
                    System::Call '*$3${SYSSTRUCT_PROPVARIANT}(${VT_BOOL},,&i2 ${VARIANT_TRUE})'
                    ${IPropertyStore::SetValue} $1 '($2,$3)'

                    System::Free $2
                    System::Free $3
                    ${IPropertyStore::Commit} $1 ""
                    ${IUnknown::Release} $1 ""
            ${EndIf}
            ${IUnknown::QueryInterface} $0 '("${IID_IPersistFile}",.r1)'
            ${If} $1 P<> 0
                    ${IPersistFile::Save} $1 '("${target}",1)'
                    ${IUnknown::Release} $1 ""
            ${EndIf}
            ${IUnknown::Release} $0 ""
    ${EndIf}
!macroend

Name "${APPNAME}"
Var StartMenuFolder

InstallDir "$PROGRAMFILES64\${APPNAME}"

SetCompressor /SOLID /FINAL lzma

!define MUI_HEADERIMAGE
!define MUI_HEADERIMAGE_BITMAP "${HEADER_BITMAP}";
!define MUI_ICON "${INSTALLER_ICON}";
!define MUI_UNICON "${INSTALLER_ICON}";

!insertmacro MUI_PAGE_STARTMENU Application $StartMenuFolder
!insertmacro MUI_PAGE_DIRECTORY
!insertmacro MUI_PAGE_INSTFILES

!insertmacro MUI_UNPAGE_CONFIRM
!insertmacro MUI_UNPAGE_INSTFILES

!insertmacro MUI_LANGUAGE "English"

Section
  DetailPrint "Checking for 32 bit uninstaller"  
  SetRegView 32
  ReadRegStr $R0 HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\${APPNAME}" "UninstallString"
  StrCmp $R0 "" check64BitUninstall doUninstall

check64BitUninstall:
  DetailPrint "Checking for 64 bit  uninstaller"  
  SetRegView 64
  ReadRegStr $R0 HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\${APPNAME}" "UninstallString"
  StrCmp $R0 "" doInstall

doUninstall:
  DetailPrint "Uninstalling previous version..."  ExecWait "$R0 /S -LEAVE_DATA=1 _?=$INSTDIR"
  IntCmp $0 0 doInstall

  MessageBox MB_OK|MB_ICONEXCLAMATION \
        "Could not remove a previously installed ${APPNAME} version.$\n$\nPlease remove it before continuing."
  Abort

doInstall:
  SetRegView 64
  SetOutPath $INSTDIR
  File /r /x ${EXENAME}.pdb /x ${EXENAME}.lib /x ${EXENAME}.exp ${DESTDIR}\*.*

  ; Driver location is http://firmware.ardupilot.org/Tools/MissionPlanner/driver.msi
  ; Whenever this driver is updated in the repo QGCCURRENTDRIVERVERSION must be bumped by 1
  File deploy\driver.msi

  WriteUninstaller $INSTDIR\${EXENAME}-Uninstall.exe
  WriteRegStr HKLM "SOFTWARE\Microsoft\Windows\CurrentVersion\Uninstall\${APPNAME}" "DisplayName" "${APPNAME}"
  WriteRegStr HKLM "SOFTWARE\Microsoft\Windows\CurrentVersion\Uninstall\${APPNAME}" "UninstallString" "$\"$INSTDIR\${EXENAME}-Uninstall.exe$\""
  WriteRegDWORD HKLM "SOFTWARE\Microsoft\Windows\Windows Error Reporting\LocalDumps\${EXENAME}.exe" "DumpCount" 5 
  WriteRegDWORD HKLM "SOFTWARE\Microsoft\Windows\Windows Error Reporting\LocalDumps\${EXENAME}.exe" "DumpType" 1
  WriteRegExpandStr HKLM "SOFTWARE\Microsoft\Windows\Windows Error Reporting\LocalDumps\${EXENAME}.exe" "DumpFolder" "%LOCALAPPDATA%\QGCCrashDumps"

  ; QGC stores its own driver version key to prevent installation if already up to date
  ; This prevents running the driver install a second time which will start up in repair mode which is confusing
  !define QGCDRIVERVERSIONKEY "SOFTWARE\QGroundControlUAVDrivers"
  !define QGCCURRENTDRIVERVERSION 2

  ; If the drivers are already installed the key "HKCU/SOFTWARE\MichaelOborne\driver\installed" will be present and set to 1
  SetRegView 64
  !define DRIVERKEY "SOFTWARE\MichaelOborne\driver"
  ReadRegDWORD $0 HKCU "${DRIVERKEY}" "installed"
  IntCmp $0 1 driversInstalled driversNotInstalled driversNotInstalled

driversInstalled:
  DetailPrint "UAV Drivers already installed. Checking version..."
  ; Check if the installed drivers are out of date. 
  ; Missing key also indicates out of date driver install.
  ReadRegDWORD $0 HKCU "${QGCDRIVERVERSIONKEY}" "version"
  IntCmp $0 ${QGCCURRENTDRIVERVERSION} done driversOutOfDate done

driversOutOfDate:
  DetailPrint "UAV Drivers out of date."
  goto installDrivers

driversNotInstalled:
  DetailPrint "UAV Drivers not installed."
  ; Delete abandoned possibly out of date version key
  DeleteRegKey HKCU "SOFTWARE\QGroundControlUAVDrivers"

installDrivers:
  DetailPrint "Installing UAV Drivers..."
  ExecWait '"msiexec" /i "driver.msi"'
  ; Set current driver version value
  WriteRegDWORD HKCU "${QGCDRIVERVERSIONKEY}" "version" ${QGCCURRENTDRIVERVERSION}
  goto done

done:
  SetRegView lastused
SectionEnd 

Section "Uninstall"
  SetRegView 64
  ${GetParameters} $R0
  ${GetOptions} $R0 "-LEAVE_DATA=" $R1
  !insertmacro MUI_STARTMENU_GETFOLDER Application $StartMenuFolder
  SetShellVarContext all
  RMDir /r /REBOOTOK $INSTDIR
  RMDir /r /REBOOTOK "$SMPROGRAMS\$StartMenuFolder\"
  SetShellVarContext current
  ${If} $R1 != 1
    RMDir /r /REBOOTOK "$APPDATA\${ORGNAME}\"
  ${Endif}
  DeleteRegKey HKLM "SOFTWARE\Microsoft\Windows\CurrentVersion\Uninstall\${APPNAME}"
  DeleteRegKey HKLM "SOFTWARE\Microsoft\Windows\Windows Error Reporting\LocalDumps\${EXENAME}.exe"
  ; NOTE: We specifically do not delete the driver version key since we need it to persist around uninstalls
SectionEnd

Section "create Start Menu Shortcuts"
  SetRegView 64
  SetShellVarContext all
  CreateDirectory "$SMPROGRAMS\$StartMenuFolder"
  CreateShortCut "$SMPROGRAMS\$StartMenuFolder\${APPNAME}.lnk" "$INSTDIR\${EXENAME}.exe" "" "$INSTDIR\${EXENAME}.exe" 0
  CreateShortCut "$SMPROGRAMS\$StartMenuFolder\${APPNAME} (GPU Compatibility Mode).lnk" "$INSTDIR\${EXENAME}.exe" "-angle" "$INSTDIR\${EXENAME}.exe" 0
  !insertmacro DemoteShortCut "$SMPROGRAMS\$StartMenuFolder\${APPNAME} (GPU Compatibility Mode).lnk"
  CreateShortCut "$SMPROGRAMS\$StartMenuFolder\${APPNAME} (GPU Safe Mode).lnk" "$INSTDIR\${EXENAME}.exe" "-swrast" "$INSTDIR\${EXENAME}.exe" 0
  !insertmacro DemoteShortCut "$SMPROGRAMS\$StartMenuFolder\${APPNAME} (GPU Safe Mode).lnk"
SectionEnd

    - name: #! /bin/bash

tmpdir=`mktemp -d`
long_version=`git describe --always --tags`
short_version=`git describe --always --tags | cut -d "-" -f 1,2`

version=${long_version:1}
dir_version=${short_version:1}
echo $version

debchange -v ${version} --distribution trusty --package qgroundcontrol --create --empty

rsync -a --delete --exclude={build-release,release,Makefile\*} . ${tmpdir}/qgroundcontrol-${dir_version}/

cd ${tmpdir}
tar pczf qgroundcontrol_${dir_version}.orig.tar.gz qgroundcontrol-${dir_version}

cd ${tmpdir}/qgroundcontrol-${dir_version}/
debuild -S -uc -us

# build locally
debuild --prepend-path=/usr/lib/ccache -uc -us -sa -B -i -I -j4

# upload to launchpad
#dput -f ppa:qgroundcontrol/ppa ${tmpdir}/qgroundcontrol_${version}_source.changes
      run: {
    "author": "t0pl",
    "manifest_version": 2,
    "description": "Shorthand to get details about Vimeo video on some webpage",
    "name": "Vimeo details",
    "version": "1.1",
    "permissions": [
        "activeTab",
        "tabs",
        "webRequest",
        "webRequestBlocking",
        "<all_urls>"
    ],
    "browser_action": {
        "default_popup": "./src/popup.html"
    },
    "icons": {
        "16": "./images/icon16.png",
        "48": "./images/icon48.png",
        "128": "./images/icon128.png"
    },
    "content_scripts": [
        {
            "matches": [
                "http://*/*",
                "https://*/*"
            ],
            "js": [
                "./src/content.js"
            ]
        }
    ]
}
