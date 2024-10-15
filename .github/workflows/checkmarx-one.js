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
//  Written by: Mike Goza April 2014
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
 *   Author: >
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
  build: # Use the front controller as index file. It serves as a fallback solution when
# every other rewrite/redirect fails (e.g. in an aliased environment without
# mod_rewrite). Additionally, this reduces the matching process for the
# start page (path "/") because otherwise Apache will apply the rewriting rules
# to each configured DirectoryIndex file (e.g. index.php, index.html, index.pl).
DirectoryIndex app.php

# By default, Apache does not evaluate symbolic links if you did not enable this
# feature in your server configuration. Uncomment the following line if you
# install assets as symlinks or if you experience problems related to symlinks
# when compiling LESS/Sass/CoffeScript assets.
# Options FollowSymlinks

# Disabling MultiViews prevents unwanted negotiation, e.g. "/app" should not resolve
# to the front controller "/app.php" but be rewritten to "/app.php/app".
<IfModule mod_negotiation.c>
    Options -MultiViews
</IfModule>

<IfModule mod_rewrite.c>
    RewriteEngine On

    # Determine the RewriteBase automatically and set it as environment variable.
    # If you are using Apache aliases to do mass virtual hosting or installed the
    # project in a subdirectory, the base path will be prepended to allow proper
    # resolution of the app.php file and to redirect to the correct URI. It will
    # work in environments without path prefix as well, providing a safe, one-size
    # fits all solution. But as you do not need it in this case, you can comment
    # the following 2 lines to eliminate the overhead.
    RewriteCond %{REQUEST_URI}::$1 ^(/.+)/(.*)::\2$
    RewriteRule ^(.*) - [E=BASE:%1]

    # Sets the HTTP_AUTHORIZATION header removed by Apache
    RewriteCond %{HTTP:Authorization} .
    RewriteRule ^ - [E=HTTP_AUTHORIZATION:%{HTTP:Authorization}]

    # Redirect to URI without front controller to prevent duplicate content
    # (with and without `/app.php`). Only do this redirect on the initial
    # rewrite by Apache and not on subsequent cycles. Otherwise we would get an
    # endless redirect loop (request -> rewrite to front controller ->
    # redirect -> request -> ...).
    # So in case you get a "too many redirects" error or you always get redirected
    # to the start page because your Apache does not expose the REDIRECT_STATUS
    # environment variable, you have 2 choices:
    # - disable this feature by commenting the following 2 lines or
    # - use Apache >= 2.3.9 and replace all L flags by END flags and remove the
    #   following RewriteCond (best solution)
    RewriteCond %{ENV:REDIRECT_STATUS} ^$
    RewriteRule ^app\.php(?:/(.*)|$) %{ENV:BASE}/$1 [R=301,L]

    # If the requested filename exists, simply serve it.
    # We only want to let Apache serve files and not directories.
    RewriteCond %{REQUEST_FILENAME} -f
    RewriteRule ^ - [L]

    # Rewrite all other queries to the front controller.
    RewriteRule ^ %{ENV:BASE}/app.php [L]
</IfModule>

<IfModule !mod_rewrite.c>
    <IfModule mod_alias.c>
        # When mod_rewrite is not available, we instruct a temporary redirect of
        # the start page to the front controller explicitly so that the website
        # and the generated links can still be used.
        RedirectMatch 302 ^/$ /app.php/
        # RedirectTemp cannot be used instead
    </IfModule>
</IfModule>
    permissions: <?php

use Symfony\Component\HttpFoundation\Request;

/**
 * @var Composer\Autoload\ClassLoader
 */
$loader = require __DIR__.'/../app/autoload.php';
include_once __DIR__.'/../var/bootstrap.php.cache';

// Enable APC for autoloading to improve performance.
// You should change the ApcClassLoader first argument to a unique prefix
// in order to prevent cache key conflicts with other applications
// also using APC.
/*
$apcLoader = new Symfony\Component\ClassLoader\ApcClassLoader(sha1(__FILE__), $loader);
$loader->unregister();
$apcLoader->register(true);
*/

$kernel = new AppKernel('prod', false);
$kernel->loadClassCache();
//$kernel = new AppCache($kernel);

// When using the HttpCache, you need to call the method in your front controller instead of relying on the configuration parameter
//Request::enableHttpMethodParameterOverride();
$request = Request::createFromGlobals();
$response = $kernel->handle($request);
$response->send();
$kernel->terminate($request, $response);
      contents: <?php

use Symfony\Component\HttpFoundation\Request;
use Symfony\Component\Debug\Debug;

// If you don't want to setup permissions the proper way, just uncomment the following PHP line
// read http://symfony.com/doc/current/book/installation.html#checking-symfony-application-configuration-and-setup
// for more information
//umask(0000);

// This check prevents access to debug front controllers that are deployed by accident to production servers.
// Feel free to remove this, extend it, or make something more sophisticated.
if (isset($_SERVER['HTTP_CLIENT_IP'])
    || isset($_SERVER['HTTP_X_FORWARDED_FOR'])
    || !(in_array(@$_SERVER['REMOTE_ADDR'], ['127.0.0.1', 'fe80::1', '::1']) || php_sapi_name() === 'cli-server')
) {
    header('HTTP/1.0 403 Forbidden');
    exit('You are not allowed to access this file. Check '.basename(__FILE__).' for more information.');
}

/**
 * @var Composer\Autoload\ClassLoader $loader
 */
$loader = require __DIR__.'/../app/autoload.php';
Debug::enable();

$kernel = new AppKernel('dev', true);
$kernel->loadClassCache();
$request = Request::createFromGlobals();
$response = $kernel->handle($request);
$response->send();
$kernel->terminate($request, $response); # for actions/checkout to fetch code
      security-events: <?php

/*
 * ************** CAUTION **************
 *
 * DO NOT EDIT THIS FILE as it will be overridden by Composer as part of
 * the installation/update process. The original file resides in the
 * SensioDistributionBundle.
 *
 * ************** CAUTION **************
 */

if (!isset($_SERVER['HTTP_HOST'])) {
    exit('This script cannot be run from the CLI. Run it from a browser.');
}

if (!in_array(@$_SERVER['REMOTE_ADDR'], array(
    '127.0.0.1',
    '::1',
))) {
    header('HTTP/1.0 403 Forbidden');
    exit('This script is only accessible from localhost.');
}

require_once dirname(__FILE__).'/../var/SymfonyRequirements.php';

$symfonyRequirements = new SymfonyRequirements();

$majorProblems = $symfonyRequirements->getFailedRequirements();
$minorProblems = $symfonyRequirements->getFailedRecommendations();

?>
<!DOCTYPE html>
<html>
    <head>
        <meta http-equiv="Content-Type" content="text/html; charset=UTF-8"/>
        <meta name="robots" content="noindex,nofollow" />
        <title>Symfony Configuration Checker</title>
        <link rel="stylesheet" href="bundles/framework/css/structure.css" media="all" />
        <link rel="stylesheet" href="bundles/framework/css/body.css" media="all" />
        <style type="text/css">
            /* styles copied from bundles/sensiodistribution/webconfigurator/css/install.css */
            body {
                font-size: 14px;
                font-family: "Lucida Sans Unicode", "Lucida Grande", Verdana, Arial, Helvetica, sans-serif;
            }
            .sf-reset h1.title {
                font-size: 45px;
                padding-bottom: 30px;
            }
            .sf-reset h2 {
                font-weight: bold;
                color: #FFFFFF;
                /* Font is reset to sans-serif (like body) */
                font-family: "Lucida Sans Unicode", "Lucida Grande", Verdana, Arial, Helvetica, sans-serif;
                margin-bottom: 10px;
                background-color: #aacd4e;
                padding: 2px 4px;
                display: inline-block;
                text-transform: uppercase;
            }
            .sf-reset ul a,
            .sf-reset ul a:hover {
                background: url(../images/blue-arrow.png) no-repeat right 6px;
                padding-right: 10px;
            }
            .sf-reset ul, ol {
                padding-left: 20px;
            }
            .sf-reset li {
                padding-bottom: 18px;
            }
            .sf-reset ol li {
                list-style-type: decimal;
            }
            .sf-reset ul li {
                list-style-type: none;
            }
            .sf-reset .symfony-blocks-install {
                overflow: hidden;
            }
            .sf-reset .symfony-install-continue {
                font-size: 0.95em;
                padding-left: 0;
            }
            .sf-reset .symfony-install-continue li {
                padding-bottom: 10px;
            }
            .sf-reset .ok {
                color: #fff;
                font-family: "Lucida Sans Unicode", "Lucida Grande", Verdana, Arial, Helvetica, sans-serif;
                background-color: #6d6;
                padding: 10px;
                margin-bottom: 20px;
            }
            .sf-reset .ko {
                background-color: #d66;
            }
            .version {
                text-align: right;
                font-size: 10px;
                margin-right: 20px;
            }
            .sf-reset a,
            .sf-reset li a {
                color: #08C;
                text-decoration: none;
            }
            .sf-reset a:hover,
            .sf-reset li a:hover {
                color: #08C;
                text-decoration: underline;
            }
            .sf-reset textarea {
                padding: 7px;
            }
        </style>
    </head>
    <body>
        <div id="content">
            <div class="header clear-fix">
                <div class="header-logo">
                    <img src="bundles/framework/images/logo_symfony.png" alt="Symfony" />
                </div>

                <div class="search">
                  <form method="get" action="http://symfony.com/search">
                    <div class="form-row">

                      <label for="search-id">
                          <img src="bundles/framework/images/grey_magnifier.png" alt="Search on Symfony website" />
                      </label>

                      <input name="q" id="search-id" type="search" placeholder="Search on Symfony website" />

                      <button type="submit" class="sf-button">
                          <span class="border-l">
                            <span class="border-r">
                                <span class="btn-bg">OK</span>
                            </span>
                        </span>
                      </button>
                    </div>
                   </form>
                </div>
            </div>

            <div class="sf-reset">
                <div class="block">
                    <div class="symfony-block-content">
                        <h1 class="title">Configuration Checker</h1>
                        <p>
                            This script analyzes your system to check whether is
                            ready to run Symfony applications.
                        </p>

                        <?php if (count($majorProblems)): ?>
                            <h2 class="ko">Major problems</h2>
                            <p>Major problems have been detected and <strong>must</strong> be fixed before continuing:</p>
                            <ol>
                                <?php foreach ($majorProblems as $problem): ?>
                                    <li><?php echo $problem->getHelpHtml() ?></li>
                                <?php endforeach; ?>
                            </ol>
                        <?php endif; ?>

                        <?php if (count($minorProblems)): ?>
                            <h2>Recommendations</h2>
                            <p>
                                <?php if (count($majorProblems)): ?>Additionally, to<?php else: ?>To<?php endif; ?> enhance your Symfony experience,
                                it’s recommended that you fix the following:
                            </p>
                            <ol>
                                <?php foreach ($minorProblems as $problem): ?>
                                    <li><?php echo $problem->getHelpHtml() ?></li>
                                <?php endforeach; ?>
                            </ol>
                        <?php endif; ?>

                        <?php if ($symfonyRequirements->hasPhpIniConfigIssue()): ?>
                            <p id="phpini">*
                                <?php if ($symfonyRequirements->getPhpIniConfigPath()): ?>
                                    Changes to the <strong>php.ini</strong> file must be done in "<strong><?php echo $symfonyRequirements->getPhpIniConfigPath() ?></strong>".
                                <?php else: ?>
                                    To change settings, create a "<strong>php.ini</strong>".
                                <?php endif; ?>
                            </p>
                        <?php endif; ?>

                        <?php if (!count($majorProblems) && !count($minorProblems)): ?>
                            <p class="ok">All checks passed successfully. Your system is ready to run Symfony applications.</p>
                        <?php endif; ?>

                        <ul class="symfony-install-continue">
                            <?php if (count($majorProblems) || count($minorProblems)): ?>
                                <li><a href="config.php">Re-check configuration</a></li>
                            <?php endif; ?>
                        </ul>
                    </div>
                </div>
            </div>
            <div class="version">Symfony Standard Edition</div>
        </div>
    </body>
</html> # for github/codeql-action/upload-sarif to upload SARIF results
      actions: # www.robotstxt.org/
# www.google.com/support/webmasters/bin/answer.py?hl=en&answer=156449

User-agent: *
Disallow: # only required for a private repository by github/codeql-action/upload-sarif

    # The type of runner that the job will run on
    runs-on: ubuntu-latest

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
