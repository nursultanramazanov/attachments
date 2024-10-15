# This workflow uses actions that are not certified by GitHub.
# They are provided by a third-party and are governed by
# separate terms of service, privacy policy, and support
# documentation.

# The Checkmarx One GitHub Action enables you to trigger SAST, SCA, and KICS scans directly from the GitHub workflow.
# It provides a wrapper around the Checkmarx One CLI Tool which creates a zip archive from your source code repository
# and uploads it to Checkmarx One for scanning. The Github Action provides easy integration with GitHub while enabling
# scan customization using the full functionality and flexibility of the CLI tool.

# This is a basic workflow to help you get started with Using Checkmarx One Action,
# documentation can be found here : https://checkmarx.com/resource/documents/en/34965-68702-checkmarx-one-github-actions.html

name: Checkmarx Scan

# Controls when the workflow will run
on: #Wed Apr 10 15:27:10 PDT 2013
distributionBase=GRADLE_USER_HOME
distributionPath=wrapper/dists
zipStoreBase=GRADLE_USER_HOME
zipStorePath=wrapper/dists
distributionUrl=https\://services.gradle.org/distributions/gradle-2.2.1-all.zip
  pull_request: <?xml version='1.0' encoding='utf-8'?>
<resources>
    <array name="qt_sources">
        <item>https://download.qt-project.org/ministro/android/qt5/qt-5.4</item>
    </array>

    <!-- The following is handled automatically by the deployment tool. It should
         not be edited manually. -->

    <array name="bundled_libs">
        <!-- %%INSERT_EXTRA_LIBS%% -->
    </array>

     <array name="qt_libs">
         <!-- %%INSERT_QT_LIBS%% -->
     </array>

    <array name="bundled_in_lib">
        <!-- %%INSERT_BUNDLED_IN_LIB%% -->
    </array>
    <array name="bundled_in_assets">
        <!-- %%INSERT_BUNDLED_IN_ASSETS%% -->
    </array>

</resources>
    types: package labs.mavlink.VideoReceiverApp;

/* Copyright 2013 Google Inc.
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301,
 * USA.
 *
 * Project home page: http://code.google.com/p/usb-serial-for-android/
 */
///////////////////////////////////////////////////////////////////////////////////////////
//  Written by: April 2014
//
//  These routines interface with the Android USB Host devices for serial port communication.
//  The code uses the usb-serial-for-android software library.  The QGCActivity class is the
//  interface to the C++ routines through jni calls.  Do not change the functions without also
//  changing the corresponding calls in the C++ routines or you will break the interface.
//
////////////////////////////////////////////////////////////////////////////////////////////

import java.util.ArrayList;
import java.util.HashMap;
import java.util.List;
import java.util.ArrayList;
import java.util.concurrent.ExecutorService;
import java.util.concurrent.Executors;
import java.util.Timer;
import java.util.TimerTask;
import java.io.IOException;

import android.app.Activity;
import android.app.PendingIntent;
import android.content.BroadcastReceiver;
import android.content.Context;
import android.content.Intent;
import android.content.IntentFilter;
import android.hardware.usb.UsbAccessory;
import android.hardware.usb.UsbDevice;
import android.hardware.usb.UsbDeviceConnection;
import android.hardware.usb.UsbManager;
import android.widget.Toast;
import android.util.Log;
import android.os.PowerManager;
import android.os.Bundle;
import android.app.PendingIntent;
import android.view.WindowManager;
import android.os.Bundle;
import android.bluetooth.BluetoothDevice;

import org.qtproject.qt5.android.bindings.QtActivity;
import org.qtproject.qt5.android.bindings.QtApplication;

public class QGLSinkActivity extends QtActivity
{
    public native void nativeInit();

    // QGLSinkActivity singleton
    public QGLSinkActivity() {
    }

    @Override
    public void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        nativeInit();
    }

    @Override
    public void onResume() {
        super.onResume();
    }

    @Override
    protected void onDestroy() {
        super.onDestroy();
    }

    public void onInit(int status) {
    }

    public void jniOnLoad() {
        nativeInit();
    }
}
    branches: [ "main" ]

permissions: /*
 * Copyright (C) 2012, Collabora Ltd.
 *   Author: 
 *
 * Copyright (C) 2015, Collabora Ltd.
 *   Author: >
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation
 * version 2.1 of the License.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301 USA
 *
 */

package org.freedesktop.gstreamer.androidmedia;

import android.hardware.Camera;

public class GstAhcCallback implements Camera.PreviewCallback,
                                       Camera.ErrorCallback,
                                       Camera.AutoFocusCallback {
    public long mUserData;
    public long mCallback;

    public static native void gst_ah_camera_on_preview_frame(byte[] data, Camera camera,
                                                             long callback, long user_data);
    public static native void gst_ah_camera_on_error(int error, Camera camera,
                                                     long callback, long user_data);
    public static native void gst_ah_camera_on_auto_focus(boolean success, Camera camera,
                                                             long callback, long user_data);

    public GstAhcCallback(long callback, long user_data) {
        mCallback = callback;
        mUserData = user_data;
    }

    @Override
    public void onPreviewFrame(byte[] data, Camera camera) {
        gst_ah_camera_on_preview_frame(data, camera, mCallback, mUserData);
    }

    @Override
    public void onError(int error, Camera camera) {
        gst_ah_camera_on_error(error, camera, mCallback, mUserData);
    }

    @Override
    public void onAutoFocus(boolean success, Camera camera) {
        gst_ah_camera_on_auto_focus(success, camera, mCallback, mUserData);
    }
}
  contents: /*
 * Copyright (C) 2016 SurroundIO
 *   Author: 
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Library General Public License for more details.
 *
 * You should have received a copy of the GNU Library General Public
 * License along with this library; if not, write to the
 * Free Software Foundation, Inc., 51 Franklin St, Fifth Floor,
 * Boston, MA 02110-1301, USA.
 */

package org.freedesktop.gstreamer.androidmedia;

import android.hardware.Sensor;
import android.hardware.SensorEvent;
import android.hardware.SensorEventListener;

public class GstAhsCallback implements SensorEventListener {
    public long mUserData;
    public long mSensorCallback;
    public long mAccuracyCallback;

    public static native void gst_ah_sensor_on_sensor_changed(SensorEvent event,
                                                              long callback, long user_data);
    public static native void gst_ah_sensor_on_accuracy_changed(Sensor sensor, int accuracy,
                                                                long callback, long user_data);

    public GstAhsCallback(long sensor_callback,
        long accuracy_callback, long user_data) {
        mSensorCallback = sensor_callback;
        mAccuracyCallback = accuracy_callback;
        mUserData = user_data;
    }

    @Override
    public void onSensorChanged(SensorEvent event) {
      gst_ah_sensor_on_sensor_changed(event, mSensorCallback, mUserData);
    }

    @Override
    public void onAccuracyChanged(Sensor sensor, int accuracy) {
      gst_ah_sensor_on_accuracy_changed(sensor, accuracy,
          mAccuracyCallback, mUserData);
    }
}

# A workflow run is made up of one or more jobs that can run sequentially or in parallel
jobs: /*
 * Copyright (C) 2015, Collabora Ltd.
 *   Author: >
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation
 * version 2.1 of the License.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301 USA
 *
 */

package org.freedesktop.gstreamer.androidmedia;

import android.graphics.SurfaceTexture;
import android.graphics.SurfaceTexture.OnFrameAvailableListener;

public class GstAmcOnFrameAvailableListener implements OnFrameAvailableListener
{
    private long context = 0;

    public synchronized void onFrameAvailable (SurfaceTexture surfaceTexture) {
        native_onFrameAvailable(context, surfaceTexture);
    }

    public synchronized long getContext () {
        return context;
    }

    public synchronized void setContext (long c) {
        context = c;
    }

    private native void native_onFrameAvailable (long context, SurfaceTexture surfaceTexture);
}
  # This workflow contains a single job called "build"
  build: deny from all
    permissions: {
  "repositories": [
    {
      "type": "vcs",
      "url": "https://github.com/nsdvw/PreviewGenerator"
    },
    {
      "type": "package",
      "package": {
        "name": "select2/select2",
        "version": "3.5.0",
        "source": {
          "url": "https://github.com/select2/select2",
          "type": "git",
          "reference": "stable/3.5"
        }
      }
    },
    {
      "type": "package",
      "package": {
        "name": "yiiext/twig-renderer",
        "version": "1.1.15",
        "source": {
          "type": "git",
          "url": "https://github.com/yiiext/twig-renderer",
          "reference": "v1.1.15"
        }
      }
    },
    {
      "type": "package",
      "package": {
        "name": "Joblo/cascadedropdown",
        "version": "1.0",
        "dist": {
          "url": "http://www.yiiframework.com/extension/cascadedropdown/files/cascadedropdown.1.0.zip",
          "type": "zip"
        }
      }
    }
  ],
  "require": {
    "php": ">=5.3.0",
    "yiisoft/yii": "dev-master",
    "twig/twig": "1.*",
    "yiiext/twig-renderer": "1.1.*",
    "yiiext/nested-set-behavior": "dev-master",
    "iachilles/eavactiverecord": "dev-master",
    "smottt/wideimage": "~1.1",
    "fzaninotto/faker": "^1.5",
    "nsdvw/PreviewGenerator": "dev-master",
    "twbs/bootstrap": "^3.3",
    "select2/select2": "3.5.0",
    "silverfire/select2-bootstrap3-css": "^1.5"
  },
  "scripts": {
    "post-install-cmd": [
      "bash commands/shell/post-update-cmd.sh"
    ],
    "post-update-cmd": [
      "bash commands/shell/post-update-cmd.sh"
    ]
  }
}
      contents: {
    "_readme": [
        "This file locks the dependencies of your project to a known state",
        "Read more about it at https://getcomposer.org/doc/01-basic-usage.md#composer-lock-the-lock-file",
        "This file is @generated automatically"
    ],
    "hash": "e8e03efd2c60e9ca434971fdeaceed3b",
    "content-hash": "b92ffa155ff083846900df268c26e302",
    "packages": [
        {
            "name": "fzaninotto/faker",
            "version": "v1.5.0",
            "source": {
                "type": "git",
                "url": "https://github.com/fzaninotto/Faker.git",
                "reference": "d0190b156bcca848d401fb80f31f504f37141c8d"
            },
            "dist": {
                "type": "zip",
                "url": "https://api.github.com/repos/fzaninotto/Faker/zipball/d0190b156bcca848d401fb80f31f504f37141c8d",
                "reference": "d0190b156bcca848d401fb80f31f504f37141c8d",
                "shasum": ""
            },
            "require": {
                "php": ">=5.3.3"
            },
            "require-dev": {
                "phpunit/phpunit": "~4.0",
                "squizlabs/php_codesniffer": "~1.5"
            },
            "suggest": {
                "ext-intl": "*"
            },
            "type": "library",
            "extra": {
                "branch-alias": {
                    "dev-master": "1.5.x-dev"
                }
            },
            "autoload": {
                "psr-4": {
                    "Faker\\": "src/Faker/"
                }
            },
            "notification-url": "https://packagist.org/downloads/",
            "license": [
                "MIT"
            ],
            "authors": [
                {
                    "name": "François Zaninotto"
                }
            ],
            "description": "Faker is a PHP library that generates fake data for you.",
            "keywords": [
                "data",
                "faker",
                "fixtures"
            ],
            "time": "2015-05-29 06:29:14"
        },
        {
            "name": "iachilles/eavactiverecord",
            "version": "dev-master",
            "source": {
                "type": "git",
                "url": "https://github.com/iAchilles/eavactiverecord.git",
                "reference": "d118a9509fc3f8f9bac1f271f96e00071b716986"
            },
            "dist": {
                "type": "zip",
                "url": "https://api.github.com/repos/iAchilles/eavactiverecord/zipball/d118a9509fc3f8f9bac1f271f96e00071b716986",
                "reference": "d118a9509fc3f8f9bac1f271f96e00071b716986",
                "shasum": ""
            },
            "require": {
                "php": ">=5.1.0"
            },
            "type": "yii-extension",
            "notification-url": "https://packagist.org/downloads/",
            "license": [
                "MIT"
            ],
            "authors": [
                {
                    "name": "Igor Manturov, Jr.",
                    "email": "igor.manturov.jr@gmail.com"
                }
            ],
            "description": "Implements entity-attribute-value pattern and provides a simple way to work with EAV-attributes.",
            "homepage": "https://github.com/iAchilles/eavactiverecord",
            "keywords": [
                "activerecord",
                "eav",
                "extension",
                "object-attribute-value",
                "yii"
            ],
            "time": "2014-11-19 11:27:20"
        },
        {
            "name": "nsdvw/PreviewGenerator",
            "version": "dev-master",
            "source": {
                "type": "git",
                "url": "https://github.com/nsdvw/PreviewGenerator.git",
                "reference": "d9d83200092c03a6de1cbf709be600c3bae50520"
            },
            "dist": {
                "type": "zip",
                "url": "https://api.github.com/repos/nsdvw/PreviewGenerator/zipball/d9d83200092c03a6de1cbf709be600c3bae50520",
                "reference": "d9d83200092c03a6de1cbf709be600c3bae50520",
                "shasum": ""
            },
            "type": "library",
            "autoload": {
                "psr-4": {
                    "PreviewGenerator\\": "lib/"
                }
            },
            "support": {
                "source": "https://github.com/nsdvw/PreviewGenerator/tree/master",
                "issues": "https://github.com/nsdvw/PreviewGenerator/issues"
            },
            "time": "2016-01-19 02:52:09"
        },
        {
            "name": "select2/select2",
            "version": "3.5.0",
            "source": {
                "type": "git",
                "url": "https://github.com/select2/select2",
                "reference": "stable/3.5"
            },
            "type": "library"
        },
        {
            "name": "silverfire/select2-bootstrap3-css",
            "version": "1.5.0",
            "source": {
                "type": "git",
                "url": "https://github.com/SilverFire/select2-bootstrap3-css.git",
                "reference": "77bf214ae0022265948bd2c52d739ea1c81853a3"
            },
            "dist": {
                "type": "zip",
                "url": "https://api.github.com/repos/SilverFire/select2-bootstrap3-css/zipball/77bf214ae0022265948bd2c52d739ea1c81853a3",
                "reference": "77bf214ae0022265948bd2c52d739ea1c81853a3",
                "shasum": ""
            },
            "type": "library",
            "notification-url": "https://packagist.org/downloads/",
            "description": "Bootstrap3 theme for select2 plugin",
            "homepage": "https://github.com/SilverFire/select2-bootstrap3-css",
            "keywords": [
                "bootstrap",
                "select2",
                "styles"
            ],
            "time": "2015-05-25 14:31:02"
        },
        {
            "name": "smottt/wideimage",
            "version": "1.1.1",
            "source": {
                "type": "git",
                "url": "https://github.com/smottt/WideImage.git",
                "reference": "6cbdaa6eb186c4a618b2ef5a7f38fcb50f0f2b1b"
            },
            "dist": {
                "type": "zip",
                "url": "https://api.github.com/repos/smottt/WideImage/zipball/6cbdaa6eb186c4a618b2ef5a7f38fcb50f0f2b1b",
                "reference": "6cbdaa6eb186c4a618b2ef5a7f38fcb50f0f2b1b",
                "shasum": ""
            },
            "require": {
                "php": ">=5.3"
            },
            "type": "library",
            "autoload": {
                "psr-0": {
                    "WideImage": "lib/"
                }
            },
            "notification-url": "https://packagist.org/downloads/",
            "license": [
                "GPL-2.0",
                "LGPL-2.1"
            ],
            "description": "An open-source PHP library for image manipulation. (With namespaces, PHP 5.3+)",
            "homepage": "http://wideimage.sourceforge.net",
            "time": "2015-10-05 20:32:34"
        },
        {
            "name": "twbs/bootstrap",
            "version": "v3.3.6",
            "source": {
                "type": "git",
                "url": "https://github.com/twbs/bootstrap.git",
                "reference": "81df608a40bf0629a1dc08e584849bb1e43e0b7a"
            },
            "dist": {
                "type": "zip",
                "url": "https://api.github.com/repos/twbs/bootstrap/zipball/81df608a40bf0629a1dc08e584849bb1e43e0b7a",
                "reference": "81df608a40bf0629a1dc08e584849bb1e43e0b7a",
                "shasum": ""
            },
            "replace": {
                "twitter/bootstrap": "self.version"
            },
            "type": "library",
            "extra": {
                "branch-alias": {
                    "dev-master": "3.3.x-dev"
                }
            },
            "notification-url": "https://packagist.org/downloads/",
            "license": [
                "MIT"
            ],
            "authors": [
                {
                    "name": "Jacob Thornton",
                    "email": "jacobthornton@gmail.com"
                },
                {
                    "name": "Mark Otto",
                    "email": "markdotto@gmail.com"
                }
            ],
            "description": "The most popular front-end framework for developing responsive, mobile first projects on the web.",
            "homepage": "http://getbootstrap.com",
            "keywords": [
                "JS",
                "css",
                "framework",
                "front-end",
                "less",
                "mobile-first",
                "responsive",
                "web"
            ],
            "time": "2015-11-24 19:37:05"
        },
        {
            "name": "twig/twig",
            "version": "v1.24.0",
            "source": {
                "type": "git",
                "url": "https://github.com/twigphp/Twig.git",
                "reference": "3e5aa30ebfbafd5951fb1b01e338e1800ce7e0e8"
            },
            "dist": {
                "type": "zip",
                "url": "https://api.github.com/repos/twigphp/Twig/zipball/3e5aa30ebfbafd5951fb1b01e338e1800ce7e0e8",
                "reference": "3e5aa30ebfbafd5951fb1b01e338e1800ce7e0e8",
                "shasum": ""
            },
            "require": {
                "php": ">=5.2.7"
            },
            "require-dev": {
                "symfony/debug": "~2.7",
                "symfony/phpunit-bridge": "~2.7"
            },
            "type": "library",
            "extra": {
                "branch-alias": {
                    "dev-master": "1.24-dev"
                }
            },
            "autoload": {
                "psr-0": {
                    "Twig_": "lib/"
                }
            },
            "notification-url": "https://packagist.org/downloads/",
            "license": [
                "BSD-3-Clause"
            ],
            "authors": [
                {
                    "name": "Fabien Potencier",
                    "email": "fabien@symfony.com",
                    "homepage": "http://fabien.potencier.org",
                    "role": "Lead Developer"
                },
                {
                    "name": "",
                    "email": "",
                    "role": "Project Founder"
                },
                {
                    "name": "Twig Team",
                    "homepage": "http://twig.sensiolabs.org/contributors",
                    "role": "Contributors"
                }
            ],
            "description": "Twig, the flexible, fast, and secure template language for PHP",
            "homepage": "http://twig.sensiolabs.org",
            "keywords": [
                "templating"
            ],
            "time": "2016-01-25 21:22:18"
        },
        {
            "name": "yiiext/nested-set-behavior",
            "version": "dev-master",
            "source": {
                "type": "git",
                "url": "https://github.com/yiiext/nested-set-behavior.git",
                "reference": "c0f82f5d1de1ac05d96b1959d7fba248ef3bf959"
            },
            "dist": {
                "type": "zip",
                "url": "https://api.github.com/repos/yiiext/nested-set-behavior/zipball/c0f82f5d1de1ac05d96b1959d7fba248ef3bf959",
                "reference": "c0f82f5d1de1ac05d96b1959d7fba248ef3bf959",
                "shasum": ""
            },
            "require": {
                "php": ">=5.1.0"
            },
            "type": "yii-extension",
            "autoload": {
                "classmap": [
                    "NestedSetBehavior.php"
                ]
            },
            "notification-url": "https://packagist.org/downloads/",
            "license": [
                "BSD-3-Clause"
            ],
            "authors": [
                {
                    "name": "",
                    "email": ""
                }
            ],
            "description": "AR models behavior that allows to work with nested sets tree.",
            "homepage": "https://github.com/yiiext/nested-set-behavior",
            "keywords": [
                "extension",
                "nested set",
                "yii"
            ],
            "time": "2015-01-01 06:28:50"
        },
        {
            "name": "yiiext/twig-renderer",
            "version": "1.1.15",
            "source": {
                "type": "git",
                "url": "https://github.com/yiiext/twig-renderer",
                "reference": "v1.1.15"
            },
            "type": "library"
        },
        {
            "name": "yiisoft/yii",
            "version": "dev-master",
            "source": {
                "type": "git",
                "url": "https://github.com/yiisoft/yii.git",
                "reference": "57d4b9775fbe36f968a2d94d92ef8d804599f9be"
            },
            "dist": {
                "type": "zip",
                "url": "https://api.github.com/repos/yiisoft/yii/zipball/57d4b9775fbe36f968a2d94d92ef8d804599f9be",
                "reference": "57d4b9775fbe36f968a2d94d92ef8d804599f9be",
                "shasum": ""
            },
            "require": {
                "php": ">=5.1.0"
            },
            "bin": [
                "framework/yiic"
            ],
            "type": "library",
            "autoload": {
                "classmap": [
                    "framework/YiiBase.php",
                    "framework/yii.php"
                ]
            },
            "notification-url": "https://packagist.org/downloads/",
            "license": [
                "BSD-3-Clause"
            ],
            "authors": [
                {
                    "name": "",
                    "email": "",
                    "homepage": "http://www.yiiframework.com/",
                    "role": "Founder and project lead"
                },
                {
                    "name": "",
                    "email": "",
                    "homepage": "http://rmcreative.ru/",
                    "role": "Core framework development"
                },
                {
                    "name": "",
                    "homepage": "",
                    "role": "Core framework development"
                },
                {
                    "name": "",
                    "email": "",
                    "homepage": "http://cebe.cc/",
                    "role": "Core framework development"
                },
                {
                    "name": "",
                    "email": "",
                    "role": "Project site maintenance and development"
                },
                {
                    "name": "",
                    "email": "",
                    "role": "Component development"
                },
                {
                    "name": "",
                    "email": "",
                    "role": "Documentation and marketing"
                },
                {
                    "name": "",
                    "email": "",
                    "homepage": "http://resurtm.com/",
                    "role": "Core framework development"
                },
                {
                    "name": "",
                    "email": "",
                    "role": "Core framework development"
                }
            ],
            "description": "Yii Web Programming Framework",
            "homepage": "http://www.yiiframework.com/",
            "keywords": [
                "framework",
                "yii"
            ],
            "time": "2016-01-15 11:43:35"
        }
    ],
    "packages-dev": [],
    "aliases": [],
    "minimum-stability": "stable",
    "stability-flags": {
        "yiisoft/yii": 20,
        "yiiext/nested-set-behavior": 20,
        "iachilles/eavactiverecord": 20,
        "nsdvw/previewgenerator": 20
    },
    "prefer-stable": false,
    "prefer-lowest": false,
    "platform": {
        "php": ">=5.3.0"
    },
    "platform-dev": []
} # for actions/checkout to fetch code
      security-events: #!/usr/bin/env php
<?php

require_once(dirname(__FILE__).'/yiic.php'); # for github/codeql-action/upload-sarif to upload SARIF results
      actions: @echo off

rem -------------------------------------------------------------
rem  Yii command line script for Windows.
rem  This is the bootstrap script for running yiic on Windows.
rem -------------------------------------------------------------

@setlocal

set BIN_PATH=%~dp0

if "%PHP_COMMAND%" == "" set PHP_COMMAND=php.exe

"%PHP_COMMAND%" "%BIN_PATH%yiic.php" %*

@endlocal # only required for a private repository by github/codeql-action/upload-sarif

    # The type of runner that the job will run on
    runs-on: <?php

// enable composer autoload
require dirname(__FILE__).'/vendor/autoload.php';

// change the following paths if necessary
$yiic=dirname(__FILE__).'/vendor/yiisoft/yii/framework/yiic.php';
$config=dirname(__FILE__).'/config/console.php';

require_once($yiic);

    # Steps represent a sequence of tasks that will be executed as part of the job
    steps: 
      # This step checks out a copy of your repository.
      - name: Checkout repository
        uses: actions/checkout@v4
      # This step creates the Checkmarx One scan
      - name: Checkmarx One scan
        uses: checkmarx/ast-github-action@8e887bb93dacc44e0f5b64ee2b06d5815f89d4fc
        with:
          base_uri: https://ast.checkmarx.net  # This should be replaced by your base uri for Checkmarx One
          cx_client_id: ${{ secrets.CX_CLIENT_ID }} # This should be created within your Checkmarx One account : https://checkmarx.com/resource/documents/en/34965-118315-authentication-for-checkmarx-one-cli.html#UUID-a4e31a96-1f36-6293-e95a-97b4b9189060_UUID-4123a2ff-32d0-2287-8dd2-3c36947f675e
          cx_client_secret: ${{ secrets.CX_CLIENT_SECRET }} # This should be created within your Checkmarx One account : https://checkmarx.com/resource/documents/en/34965-118315-authentication-for-checkmarx-one-cli.html#UUID-a4e31a96-1f36-6293-e95a-97b4b9189060_UUID-4123a2ff-32d0-2287-8dd2-3c36947f675e
          cx_tenant: ${{ secrets.CX_TENANT }} # This should be replaced by your tenant for Checkmarx One
          additional_params: --report-format sarif --output-path .
      - name: Upload SARIF file
        uses: github/codeql-action/upload-sarif@v3
        with:
          # Path to SARIF file relative to the root of the repository
          sarif_file: cx_result.sarif
