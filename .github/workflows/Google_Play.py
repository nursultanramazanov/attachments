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
__TAURI_WORKSPACE__ = "true"
CARGO_TERM_COLOR: 
 {
  "gitSiteUrl": "https://www.github.com/tauri-apps/tauri/",
  "changeTags": {
    "feat": "New Features",
    "enhance": "Enhancements",
    "bug": "Bug Fixes",
    "pref": "Performance Improvements",
    "changes": "What's Changed",
    "sec": "Security fixes",
    "deps": "Dependencies",
    "breaking": "Breaking Changes"
  },
  "defaultChangeTag": "changes",
  "pkgManagers": {
    "rust": {
      "version": true,
      "getPublishedVersion": "node ../../.scripts/covector/package-latest-version.js cargo ${ pkgFile.pkg.package.name } ${ pkgFile.pkg.package.version }",
      "prepublish": [
        "sudo apt-get update",
        "sudo apt-get install -y webkit2gtk-4.1 libayatana-appindicator3-dev",
        "cargo install cargo-audit --features=fix",
        {
          "command": "cargo generate-lockfile",
          "dryRunCommand": true,
          "runFromRoot": true,
          "pipe": true
        },
        {
          "command": "echo '<details>\n<summary><em><h4>Cargo Audit</h4></em></summary>\n\n```'",
          "dryRunCommand": true,
          "pipe": true
        },
        {
          "command": "cargo audit ${ process.env.CARGO_AUDIT_OPTIONS || '' }",
          "dryRunCommand": true,
          "runFromRoot": true,
          "pipe": true
        },
        {
          "command": "echo '```\n\n</details>\n'",
          "dryRunCommand": true,
          "pipe": true
        }
      ],
      "publish": [
        "sleep 15s",
        {
          "command": "cargo package --no-verify",
          "dryRunCommand": true
        },
        {
          "command": "echo '<details>\n<summary><em><h4>Cargo Publish</h4></em></summary>\n\n```'",
          "dryRunCommand": true,
          "pipe": true
        },
        {
          "command": "cargo publish",
          "dryRunCommand": "cargo publish --dry-run",
          "pipe": true
        },
        {
          "command": "echo '```\n\n</details>\n'",
          "dryRunCommand": true,
          "pipe": true
        }
      ],
      "postpublish": [
        "git tag ${ pkg.pkg }-v${ pkgFile.versionMajor } -f",
        "git tag ${ pkg.pkg }-v${ pkgFile.versionMajor }.${ pkgFile.versionMinor } -f",
        "git push --tags -f"
      ],
      "assets": [
        {
          "path": "./target/package/${ pkg.pkg }-${ pkgFile.version }.crate",
          "name": "${ pkg.pkg }-${ pkgFile.version }.crate"
        }
      ]
    },
    "javascript": {
      "version": true,
      "getPublishedVersion": "node ../../.scripts/covector/package-latest-version.js npm ${ pkgFile.pkg.name } ${ pkgFile.pkg.version }",
      "prepublish": [
        {
          "command": "yarn",
          "dryRunCommand": true
        },
        {
          "command": "echo '<details>\n<summary><em><h4>Yarn Audit</h4></em></summary>\n\n```'",
          "dryRunCommand": true,
          "pipe": true
        },
        {
          "command": "yarn audit",
          "dryRunCommand": true,
          "runFromRoot": true,
          "pipe": true
        },
        {
          "command": "echo '```\n\n</details>\n'",
          "dryRunCommand": true,
          "pipe": true
        },
        {
          "command": "npm pack",
          "dryRunCommand": true
        }
      ],
      "publish": [
        "sleep 15s",
        {
          "command": "echo '<details>\n<summary><em><h4>Yarn Publish</h4></em></summary>\n\n```'",
          "dryRunCommand": true,
          "pipe": true
        },
        {
          "command": "yarn publish --access public --loglevel silly --tag next",
          "dryRunCommand": "npm publish --dry-run --access public",
          "pipe": true
        },
        {
          "command": "echo '```\n\n</details>\n'",
          "dryRunCommand": true,
          "pipe": true
        }
      ],
      "postpublish": [
        "git tag ${ pkg.pkg }-v${ pkgFile.versionMajor } -f",
        "git tag ${ pkg.pkg }-v${ pkgFile.versionMajor }.${ pkgFile.versionMinor } -f",
        "git push --tags -f"
      ]
    }
  },
  "packages": {
    "@tauri-apps/api": {
      "path": "./tooling/api",
      "manager": "javascript",
      "assets": [
        {
          "path": "./tooling/api/dist/tauri-apps-api-${ pkgFile.version }.tgz",
          "name": "tauri-apps-api-${ pkgFile.version }.tgz"
        }
      ],
      "prepublish": [
        {
          "command": "yarn",
          "dryRunCommand": true
        },
        {
          "command": "echo '<details>\n<summary><em><h4>Yarn Audit</h4></em></summary>\n\n```'",
          "dryRunCommand": true,
          "pipe": true
        },
        {
          "command": "yarn audit",
          "dryRunCommand": true,
          "runFromRoot": true,
          "pipe": true
        },
        {
          "command": "echo '```\n\n</details>\n'",
          "dryRunCommand": true,
          "pipe": true
        },
        {
          "command": "yarn npm-pack",
          "dryRunCommand": true
        }
      ],
      "publish": [
        {
          "command": "echo '<details>\n<summary><em><h4>Yarn Publish</h4></em></summary>\n\n```'",
          "dryRunCommand": true,
          "pipe": true
        },
        {
          "command": "yarn npm-publish",
          "dryRunCommand": true,
          "pipe": true
        },
        {
          "command": "echo '```\n\n</details>\n'",
          "dryRunCommand": true,
          "pipe": true
        }
      ]
    },
    "tauri-utils": {
      "path": "./core/tauri-utils",
      "manager": "rust"
    },
    "tauri-bundler": {
      "path": "./tooling/bundler",
      "manager": "rust",
      "dependencies": ["tauri-utils"]
    },
    "tauri-runtime": {
      "path": "./core/tauri-runtime",
      "manager": "rust",
      "dependencies": ["tauri-utils"]
    },
    "tauri-runtime-wry": {
      "path": "./core/tauri-runtime-wry",
      "manager": "rust",
      "dependencies": ["tauri-utils", "tauri-runtime"]
    },
    "tauri-codegen": {
      "path": "./core/tauri-codegen",
      "manager": "rust",
      "dependencies": ["tauri-utils"]
    },
    "tauri-macros": {
      "path": "./core/tauri-macros",
      "manager": "rust",
      "dependencies": ["tauri-codegen", "tauri-utils"]
    },
    "tauri-plugin": {
      "path": "./core/tauri-plugin",
      "manager": "rust",
      "dependencies": ["tauri-utils"],
      "postversion": "node ../../.scripts/covector/sync-cli-metadata.js ${ pkg.pkg } ${ release.type }"
    },
    "tauri-build": {
      "path": "./core/tauri-build",
      "manager": "rust",
      "dependencies": ["tauri-codegen", "tauri-utils"],
      "postversion": [
        "node ../../.scripts/covector/sync-cli-metadata.js ${ pkg.pkg } ${ release.type }",
        "cargo build --manifest-path ../tauri-config-schema/Cargo.toml"
      ],
      "assets": [
        {
          "path": "./tooling/cli/schema.json",
          "name": "schema.json"
        }
      ]
    },
    "tauri": {
      "path": "./core/tauri",
      "manager": "rust",
      "dependencies": [
        "tauri-macros",
        "tauri-utils",
        "tauri-runtime",
        "tauri-runtime-wry",
        "tauri-build"
      ],
      "postversion": "node ../../.scripts/covector/sync-cli-metadata.js ${ pkg.pkg } ${ release.type }"
    },
    "@tauri-apps/cli": {
      "path": "./tooling/cli/node",
      "manager": "javascript",
      "getPublishedVersion": "node ../../../.scripts/covector/package-latest-version.js npm ${ pkgFile.pkg.name } ${ pkgFile.pkg.version }",
      "dependencies": ["tauri-cli"],
      "postversion": [
        "node ../../../.scripts/covector/sync-cli-metadata.js ${ pkg.pkg } ${ release.type }",
        "cargo build --manifest-path ../../../core/tauri-config-schema/Cargo.toml"
      ],
      "prepublish": [],
      "publish": [],
      "postpublish": []
    },
    "tauri-cli": {
      "path": "./tooling/cli",
      "manager": "rust",
      "dependencies": ["tauri-bundler", "tauri-utils"],
      "postversion": [
        "cargo check",
        "cargo build --manifest-path ../../core/tauri-config-schema/Cargo.toml"
      ],
      "assets": [
        {
          "path": "${ pkg.path }/target/package/tauri-cli-${ pkgFile.version }.crate",
          "name": "${ pkg.pkg }-${ pkgFile.version }.crate"
        }
      ]
    },
    "tauri-driver": {
      "path": "./tooling/webdriver",
      "manager": "rust",
      "postversion": "cargo check"
    }
  }
}
{
  "tag": "beta",
  "changes": [
    ".changes/UriSchemeResponder-export.md",
    ".changes/acl-default-permission-verification.md",
    ".changes/acl-platform-refactor.md",
    ".changes/acl-scope-refactor.md",
    ".changes/acl-urlpattern.md",
    ".changes/allow-recursive-asset-scope-on-file-drop-directory.md",
    ".changes/api-isTauri-type.md",
    ".changes/api-isTauri.md",
    ".changes/api-position-size-args.md",
    ".changes/api-readd-window-created-event.md",
    ".changes/api-simplify-unc-paths.md",
    ".changes/api-tauri-event-file-drop-rename.md",
    ".changes/api-tray-by-id.md",
    ".changes/api-type-definitions.md",
    ".changes/api-webview-window-new-methods.md",
    ".changes/api-webview-window.md",
    ".changes/api-window-on-filedrop.md",
    ".changes/app-handle-set-activation-policy.md",
    ".changes/app-manifest.md",
    ".changes/app-region-drag.md",
    ".changes/assets-setup.md",
    ".changes/beta.md",
    ".changes/better-error-for-invalid-plugin-config.md",
    ".changes/build-android-version-check.md",
    ".changes/build-resource-target-same-src.md",
    ".changes/build-schema-generation.md",
    ".changes/bunderl-installer-hooks.md",
    ".changes/bundler-deep-link-reg-path.md",
    ".changes/bundler-license.md",
    ".changes/bundler-long_description.md",
    ".changes/bundler-nsis-deep-links.md",
    ".changes/bundler-nsis-tauri-utils.md",
    ".changes/bundler-r-flag.md",
    ".changes/bundler-resources-unix.md",
    ".changes/bundler-rpm-license.md",
    ".changes/bundler-shortcuts-updating.md",
    ".changes/capabilities-multiwebview.md",
    ".changes/capabilities-tauri-conf.md",
    ".changes/capability-builder-platform.md",
    ".changes/capability-context-refactor.md",
    ".changes/cleanup-resource-table.md",
    ".changes/cli-acl-subcommands.md",
    ".changes/cli-add-@-spec.md",
    ".changes/cli-add-launchscreen-storyboard.md",
    ".changes/cli-allow-kotlin-keyword-as-ident.md",
    ".changes/cli-build-no-bundle.md",
    ".changes/cli-chinese-product-name.md",
    ".changes/cli-ds-store-dev-watch.md",
    ".changes/cli-empty-responses.md",
    ".changes/cli-frontend-dist-expected-path.md",
    ".changes/cli-icon-non-0-exit.md",
    ".changes/cli-include-dir-cargo-manifest-dir.md",
    ".changes/cli-migrate-non-utf8.md",
    ".changes/cli-migrate-unknown-plugins.md",
    ".changes/cli-mobile-init-partition.md",
    ".changes/cli-openssl-cargo-mobile2-removal.md",
    ".changes/cli-perserve-cargo-bin-name.md",
    ".changes/cli-plugin-android-init.md",
    ".changes/cli-plugins-migrate.md",
    ".changes/cli-update-deps-fix-log.md",
    ".changes/cli-updater-unkown-fields.md",
    ".changes/cli-windows-build-tools-detect-utf8.md",
    ".changes/cli-windows-sys.md",
    ".changes/closeable-maximizable.md",
    ".changes/codegen-capabilities-attribute.md",
    ".changes/codegen-set-assets.md",
    ".changes/color-context-generation.md",
    ".changes/context-assets-runtime-generic.md",
    ".changes/context-assets-unbox.md",
    ".changes/context-remove-assets-generics.md",
    ".changes/context-remove-assets-mut.md",
    ".changes/context-runtime-authority.md",
    ".changes/core-always-rebuilds-due-to-permissions-dir.md",
    ".changes/core-app-tray-remove-tray-apis-removed.md",
    ".changes/core-center-window.md",
    ".changes/core-emit-created-events.md",
    ".changes/core-emit-js-all-targets.md",
    ".changes/core-env-args.md",
    ".changes/core-isTauri.md",
    ".changes/core-js-event-anytarget.md",
    ".changes/core-jsimage-intoimg.md",
    ".changes/core-menu-resources-deadlock.md",
    ".changes/core-once-event-return-event-id.md",
    ".changes/core-path-basename-replace.md",
    ".changes/core-specta-integration.md",
    ".changes/core-start-resize-permission.md",
    ".changes/core-window-hasdisplayhandle.md",
    ".changes/core-windows-menubar-theme.md",
    ".changes/csp-header-linux.md",
    ".changes/cursor_position.md",
    ".changes/cursor_position_js.md",
    ".changes/custom-sign-command.md",
    ".changes/deb-rpm-post-pre-scripts-bundler.md",
    ".changes/deb-rpm-post-pre-scripts-config.md",
    ".changes/deb-rpm-provides-conflicts-replaces.md",
    ".changes/default-generic-for-menu.md",
    ".changes/default-generic-for-tray.md",
    ".changes/dev-fn.md",
    ".changes/downgrade-minisign.md",
    ".changes/drop-nsis-applicationid.md",
    ".changes/enhance-event-emit.md",
    ".changes/enhance-ipc-url-check.md",
    ".changes/enhance-resource-dir-resolution.md",
    ".changes/event-reopen.md",
    ".changes/expose-image-constructor.md",
    ".changes/expose-js-image.md",
    ".changes/fix-acl-webview-check.md",
    ".changes/fix-add-child-deadlock.md",
    ".changes/fix-capability-schema-definitions.md",
    ".changes/fix-capability-totokens.md",
    ".changes/fix-channel-buffer-processing.md",
    ".changes/fix-channel-ipc-response.md",
    ".changes/fix-clear-residual-listeners.md",
    ".changes/fix-cli-add-more-plugins.md",
    ".changes/fix-cli-migration-http-acl.md",
    ".changes/fix-codegen-rerun-if-changed.md",
    ".changes/fix-config-arg.md",
    ".changes/fix-draw-tracing.md",
    ".changes/fix-fs-scope-check-symlink.md",
    ".changes/fix-incompatible-ipc-field-postmessage.md",
    ".changes/fix-inner-size.md",
    ".changes/fix-invoke-devtools-by-hotkey.md",
    ".changes/fix-ios-dev-logs.md",
    ".changes/fix-ipc-error-json.md",
    ".changes/fix-macos-deep-link-cfbundleurlname.md",
    ".changes/fix-menu-remove-api.md",
    ".changes/fix-metadata-on-close.md",
    ".changes/fix-migrate-updater.md",
    ".changes/fix-mobile-cmd-case.md",
    ".changes/fix-mobile-process-spawn.md",
    ".changes/fix-pnpm-check.md",
    ".changes/fix-process-ipc-message-fn.md",
    ".changes/fix-proxy-url-totokens-impl.md",
    ".changes/fix-remote-domain-url.md",
    ".changes/fix-reparent.md",
    ".changes/fix-rewrite-schema.md",
    ".changes/fix-runtime-wry-32bit.md",
    ".changes/fix-schemars-compatibility.md",
    ".changes/fix-scope-resolution.md",
    ".changes/fix-tauri-build-license-field.md",
    ".changes/fix-tauri-build-unix.md",
    ".changes/fix-temp-permission-file-name.md",
    ".changes/fix-visibility-change.md",
    ".changes/fix-webview-close.md",
    ".changes/fix-window-center-monitor-scale.md",
    ".changes/fix-window-center-work-area.md",
    ".changes/fix-window-destroy-deadlock.md",
    ".changes/fix-window-inner-size-crash.md",
    ".changes/global-api-script-path-plugins.md",
    ".changes/handle-empty-permissions.md",
    ".changes/hide-windows-on-cleanup.md",
    ".changes/http-v1.md",
    ".changes/ico-featrue-flags.md",
    ".changes/image-crate.md",
    ".changes/image-rgba-uint8array.md",
    ".changes/image-size-refactor.md",
    ".changes/improve-errors-for-missing-links-property.md",
    ".changes/inline-plugins.md",
    ".changes/ios-signing-optional.md",
    ".changes/ipc-allow-headers.md",
    ".changes/ipc-only-main-frame.md",
    ".changes/ipc-post-message-fallback.md",
    ".changes/ipc-request-param-refactor.md",
    ".changes/isolation-pattern-key-extractable.md",
    ".changes/isolation-script-remove-itself.md",
    ".changes/mobile-use-identifier-as-id.md",
    ".changes/mobile-watcher.md",
    ".changes/monitor-from-point-js.md",
    ".changes/monitor-from-point.md",
    ".changes/multiwebview-bounds-fixes.md",
    ".changes/nsis-append-product-name.md",
    ".changes/nsis-dpi-aware.md",
    ".changes/nsis-shortcuts-regression.md",
    ".changes/path-result-error-rexport.md",
    ".changes/permission-platforms.md",
    ".changes/permission-table.md",
    ".changes/plugin-global-api-script.md",
    ".changes/plugin-init-script-context.md",
    ".changes/plugin-ios-xcode-project.md",
    ".changes/preserve-channel-order.md",
    ".changes/progress-bar-state-refactor.md",
    ".changes/re-export-progress-bar-status.md",
    ".changes/rect-strcut.md",
    ".changes/refactor-capabilities-schema.md",
    ".changes/refactor-capability-remote-option.md",
    ".changes/refactor-scope-ret-value.md",
    ".changes/remove-app-custom-protocol-feature.md",
    ".changes/remove-from-format-image.md",
    ".changes/remove-unit-uri.md",
    ".changes/rename-file-drop.md",
    ".changes/reparent.md",
    ".changes/rerun-if-permission-created.md",
    ".changes/resources_table_access.md",
    ".changes/revert-app-region-drag.md",
    ".changes/revert-fix-visibility-change.md",
    ".changes/rpm-compression-level.md",
    ".changes/runner-config.md",
    ".changes/runtime-add-capability.md",
    ".changes/runtime-capability-dynamic.md",
    ".changes/runtime-dpi-mod-moved.md",
    ".changes/runtime-icon-lifetime.md",
    ".changes/runtime-window-builder-get-theme.md",
    ".changes/rustc-check-cfg.md",
    ".changes/rwh-06.md",
    ".changes/schema_str.md",
    ".changes/script-older-os.md",
    ".changes/set-auto-resize.md",
    ".changes/set-zoom.md",
    ".changes/skip-webview-install-mod.md",
    ".changes/strict-csp-isolation-frame.md",
    ".changes/tauri-build-codegen-capabilities.md",
    ".changes/tauri-build-dev-changes.md",
    ".changes/tauri-build-dev-fn.md",
    ".changes/tauri-bundle-command.md",
    ".changes/tauri-cli-add-default-perm.md",
    ".changes/tauri-close-requested-target-specific.md",
    ".changes/tauri-codegen-use-correct-env.md",
    ".changes/tauri-context-icon-methods.md",
    ".changes/tauri-correct-platform-file.md",
    ".changes/tauri-dev-fn-const.md",
    ".changes/tauri-error-sync.md",
    ".changes/tauri-icon-removed.md",
    ".changes/tauri-image-codegen.md",
    ".changes/tauri-image.md",
    ".changes/tauri-info-no-node.md",
    ".changes/tauri-plugin-identifier-alphanumeric.md",
    ".changes/tauri-runtime-webview-events.md",
    ".changes/tauri-scope-object-error-sync.md",
    ".changes/tauri-utils-capability-refactor.md",
    ".changes/tauri-utils-package-name-removed.md",
    ".changes/tauri-utils-plugin-module.md",
    ".changes/tauri-webview-events.md",
    ".changes/tauri-window-origin-default-to-null.md",
    ".changes/tray-icon-event.md",
    ".changes/tray-icon-rect.md",
    ".changes/tray-rect.md",
    ".changes/truncate-before-write-buildtask.md",
    ".changes/unstable-child-webview.md",
    ".changes/update-acl-paths-cli.md",
    ".changes/update-app-template-capabilities-conf.md",
    ".changes/update-gradle.md",
    ".changes/update-plugin-template.md",
    ".changes/update-target-sdk.md",
    ".changes/url-result-runtime.md",
    ".changes/url-result.md",
    ".changes/utils-bundle-target-all.md",
    ".changes/utils-bundle-type-all.md",
    ".changes/utils-debug-eprintln.md",
    ".changes/utils-installer-hooks.md",
    ".changes/utils-named-capability-file.md",
    ".changes/utils-remove-asset-trait.md",
    ".changes/utils-sign-command.md",
    ".changes/webview-bounds.md",
    ".changes/wry-0.36.md",
    ".changes/wry-0.37.md",
    ".changes/wry-0.38.md",
    ".changes/zoom-hotkeys-enabled.md",
    ".changes/zoom-polyfill.md"
  ]
}

jobs: import fileinput
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
  build: Add-Type -TypeDefinition @'
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

    runs-on: #!/bin/sh
HERE="$(dirname "$(readlink -f "${0}")")"
export LD_LIBRARY_PATH="${HERE}/usr/lib/x86_64-linux-gnu":"${HERE}/Qt/libs":$LD_LIBRARY_PATH
export QML2_IMPORT_PATH="${HERE}/Qt/qml"
export QT_PLUGIN_PATH="${HERE}/Qt/plugins"

# hack until icon issue with AppImage is resolved
mkdir -p ~/.icons && cp ${HERE}/qgroundcontrol.png ~/.icons

"${HERE}/QGroundControl" "$@"

    steps: [Desktop Entry]
Type=Application
Name=QGroundControl
GenericName=Ground Control Station
Comment=UAS ground control station
Icon=qgroundcontrol
Exec=qgroundcontrol-start.sh
Terminal=false
Categories=Utility;
    - uses: !include "MUI2.nsh"
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
      run: #!/usr/bin/env php
<?php

use Symfony\Bundle\FrameworkBundle\Console\Application;
use Symfony\Component\Console\Input\ArgvInput;
use Symfony\Component\Debug\Debug;

// if you don't want to setup permissions the proper way, just uncomment the following PHP line
// read http://symfony.com/doc/current/book/installation.html#configuration-and-setup for more information
//umask(0000);

set_time_limit(0);

/**
 * @var Composer\Autoload\ClassLoader $loader
 */
$loader = require __DIR__.'/../app/autoload.php';

$input = new ArgvInput();
$env = $input->getParameterOption(['--env', '-e'], getenv('SYMFONY_ENV') ?: 'dev');
$debug = getenv('SYMFONY_DEBUG') !== '0' && !$input->hasParameterOption(['--no-debug', '']) && $env !== 'prod';

if ($debug) {
    Debug::enable();
}

$kernel = new AppKernel($env, $debug);
$application = new Application($kernel);
$application->run($input);
    - name: #!/usr/bin/env php
<?php

require_once dirname(__FILE__).'/../var/SymfonyRequirements.php';

$lineSize = 70;
$symfonyRequirements = new SymfonyRequirements();
$iniPath = $symfonyRequirements->getPhpIniConfigPath();

echo_title('Symfony Requirements Checker');

echo '> PHP is using the following php.ini file:'.PHP_EOL;
if ($iniPath) {
    echo_style('green', '  '.$iniPath);
} else {
    echo_style('warning', '  WARNING: No configuration file (php.ini) used by PHP!');
}

echo PHP_EOL.PHP_EOL;

echo '> Checking Symfony requirements:'.PHP_EOL.'  ';

$messages = array();
foreach ($symfonyRequirements->getRequirements() as $req) {
    /** @var $req Requirement */
    if ($helpText = get_error_message($req, $lineSize)) {
        echo_style('red', 'E');
        $messages['error'][] = $helpText;
    } else {
        echo_style('green', '.');
    }
}

$checkPassed = empty($messages['error']);

foreach ($symfonyRequirements->getRecommendations() as $req) {
    if ($helpText = get_error_message($req, $lineSize)) {
        echo_style('yellow', 'W');
        $messages['warning'][] = $helpText;
    } else {
        echo_style('green', '.');
    }
}

if ($checkPassed) {
    echo_block('success', 'OK', 'Your system is ready to run Symfony projects');
} else {
    echo_block('error', 'ERROR', 'Your system is not ready to run Symfony projects');

    echo_title('Fix the following mandatory requirements', 'red');

    foreach ($messages['error'] as $helpText) {
        echo ' * '.$helpText.PHP_EOL;
    }
}

if (!empty($messages['warning'])) {
    echo_title('Optional recommendations to improve your setup', 'yellow');

    foreach ($messages['warning'] as $helpText) {
        echo ' * '.$helpText.PHP_EOL;
    }
}

echo PHP_EOL;
echo_style('title', 'Note');
echo '  The command console could use a different php.ini file'.PHP_EOL;
echo_style('title', '~~~~');
echo '  than the one used with your web server. To be on the'.PHP_EOL;
echo '      safe side, please check the requirements from your web'.PHP_EOL;
echo '      server using the ';
echo_style('yellow', 'web/config.php');
echo ' script.'.PHP_EOL;
echo PHP_EOL;

exit($checkPassed ? 0 : 1);

function get_error_message(Requirement $requirement, $lineSize)
{
    if ($requirement->isFulfilled()) {
        return;
    }

    $errorMessage = wordwrap($requirement->getTestMessage(), $lineSize - 3, PHP_EOL.'   ').PHP_EOL;
    $errorMessage .= '   > '.wordwrap($requirement->getHelpText(), $lineSize - 5, PHP_EOL.'   > ').PHP_EOL;

    return $errorMessage;
}

function echo_title($title, $style = null)
{
    $style = $style ?: 'title';

    echo PHP_EOL;
    echo_style($style, $title.PHP_EOL);
    echo_style($style, str_repeat('~', strlen($title)).PHP_EOL);
    echo PHP_EOL;
}

function echo_style($style, $message)
{
    // ANSI color codes
    $styles = array(
        'reset' => "\033[0m",
        'red' => "\033[31m",
        'green' => "\033[32m",
        'yellow' => "\033[33m",
        'error' => "\033[37;41m",
        'success' => "\033[37;42m",
        'title' => "\033[34m",
    );
    $supports = has_color_support();

    echo($supports ? $styles[$style] : '').$message.($supports ? $styles['reset'] : '');
}

function echo_block($style, $title, $message)
{
    $message = ' '.trim($message).' ';
    $width = strlen($message);

    echo PHP_EOL.PHP_EOL;

    echo_style($style, str_repeat(' ', $width).PHP_EOL);
    echo_style($style, str_pad(' ['.$title.']', $width, ' ', STR_PAD_RIGHT).PHP_EOL);
    echo_style($style, str_pad($message, $width, ' ', STR_PAD_RIGHT).PHP_EOL);
    echo_style($style, str_repeat(' ', $width).PHP_EOL);
}

function has_color_support()
{
    static $support;

    if (null === $support) {
        if (DIRECTORY_SEPARATOR == '\\') {
            $support = false !== getenv('ANSICON') || 'ON' === getenv('ConEmuANSI');
        } else {
            $support = function_exists('posix_isatty') && @posix_isatty(STDOUT);
        }
    }

    return $support;
}
      run: import requests
from email.utils import parseaddr

def Check(account):
        headers = {
                'User-Agent':'Pwnage-Checker-For-Python',
                "Accept": "application/vnd.haveibeenpwned.v2+json"
        }
        r = requests.get("https://haveibeenpwned.com/api/v2/breachedaccount/{0}".format(account),headers=headers)
        names = {}
        if not r.status_code == 200:
                return False
        json = r.json()
        for i in range(len(json)):
                names[json[i]["Name"]] = json[i]["BreachDate"]
        return names

a = input("-> ")
liste = []
a = a.replace("\n"," ")
for i in a.split(","):
        r = parseaddr(i)
        if r[1] != "":
                liste.append(r)
for n in liste:
        print("Checking ", str(n[0]))
        che = Check(str(n[1]))
        if che != False:
                print(" > Results for ",str(n[1]),": \n",che,"\n")     
