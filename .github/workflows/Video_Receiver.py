name: Jekyll site CI

on: #Wed Apr 10 15:27:10 PDT 2013
distributionBase=GRADLE_USER_HOME
distributionPath=wrapper/dists
zipStoreBase=GRADLE_USER_HOME
zipStorePath=wrapper/dists
distributionUrl=https\://services.gradle.org/distributions/gradle-2.2.1-all.zip
  push: <?xml version='1.0' encoding='utf-8'?>
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
    branches: [ "main" ]
  pull_request: package labs.mavlink.VideoReceiverApp;

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
//  Written by:  April 2014
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

jobs: /*
 * Copyright (C) 2012, Collabora Ltd.
 *   Author:  
 *
 * Copyright (C) 2015, Collabora Ltd.
 *   Author:   <>
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
  build: /*
 * Copyright (C) 2016 SurroundIO
 *   Author:   <>
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

    runs-on: /*
 * Copyright (C) 2015, Collabora Ltd.
 *   Author:   <>
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

    steps: <?xml version="1.0"?>
<manifest package="labs.mavlink.VideoReceiverApp" xmlns:android="http://schemas.android.com/apk/res/android" android:versionName="1" android:versionCode="100000" android:installLocation="auto">
    <application android:hardwareAccelerated="true" android:name="org.qtproject.qt5.android.bindings.QtApplication" android:label="-- %%INSERT_APP_NAME%% --" android:icon="@drawable/icon">
        <activity android:configChanges="orientation|uiMode|screenLayout|screenSize|smallestScreenSize|locale|fontScale|keyboard|keyboardHidden|navigation" android:name="labs.mavlink.VideoReceiverApp.QGLSinkActivity" android:label="-- %%INSERT_APP_NAME%% --" android:screenOrientation="sensorLandscape" android:launchMode="singleTask" android:keepScreenOn="true">
            <intent-filter>
                <action android:name="android.intent.action.MAIN"/>
                <category android:name="android.intent.category.LAUNCHER"/>
                <action android:name="android.hardware.usb.action.USB_DEVICE_ATTACHED"/>
                <action android:name="android.hardware.usb.action.USB_DEVICE_DETACHED"/>
                <action android:name="android.bluetooth.device.action.ACL_CONNECTED"/>
                <action android:name="android.bluetooth.device.action.ACL_DISCONNECTED"/>
                <action android:name="android.hardware.usb.action.USB_ACCESSORY_ATTACHED"/>
            </intent-filter>

            <!-- Rest of Standard Manifest -->
            <meta-data android:name="android.app.lib_name" android:value="-- %%INSERT_APP_LIB_NAME%% --"/>
            <meta-data android:name="android.app.qt_sources_resource_id" android:resource="@array/qt_sources"/>
            <meta-data android:name="android.app.repository" android:value="default"/>
            <meta-data android:name="android.app.qt_libs_resource_id" android:resource="@array/qt_libs"/>
            <meta-data android:name="android.app.bundled_libs_resource_id" android:resource="@array/bundled_libs"/>
            <!-- Deploy Qt libs as part of package -->
            <meta-data android:name="android.app.bundle_local_qt_libs" android:value="-- %%BUNDLE_LOCAL_QT_LIBS%% --"/>
            <meta-data android:name="android.app.bundled_in_lib_resource_id" android:resource="@array/bundled_in_lib"/>
            <meta-data android:name="android.app.bundled_in_assets_resource_id" android:resource="@array/bundled_in_assets"/>
            <!-- Run with local libs -->
            <meta-data android:name="android.app.use_local_qt_libs" android:value="-- %%USE_LOCAL_QT_LIBS%% --"/>
            <meta-data android:name="android.app.libs_prefix" android:value="/data/local/tmp/qt/"/>
            <meta-data android:name="android.app.load_local_libs" android:value="-- %%INSERT_LOCAL_LIBS%% --"/>
            <meta-data android:name="android.app.load_local_jars" android:value="-- %%INSERT_LOCAL_JARS%% --"/>
            <meta-data android:name="android.app.static_init_classes" android:value="-- %%INSERT_INIT_CLASSES%% --"/>
            <!--  Messages maps -->
            <meta-data android:value="@string/ministro_not_found_msg" android:name="android.app.ministro_not_found_msg"/>
            <meta-data android:value="@string/ministro_needed_msg" android:name="android.app.ministro_needed_msg"/>
            <meta-data android:value="@string/fatal_error_msg" android:name="android.app.fatal_error_msg"/>
            <!--  Messages maps -->

            <!-- Splash screen -->
            <!--
            <meta-data android:name="android.app.splash_screen_drawable" android:resource="@drawable/logo"/>
            -->
            <!-- Splash screen -->

            <!-- Background running -->
            <!-- Warning: changing this value to true may cause unexpected crashes if the
                          application still try to draw after
                          "applicationStateChanged(Qt::ApplicationSuspended)"
                          signal is sent! -->
            <meta-data android:name="android.app.background_running" android:value="false"/>
            <!-- Background running -->
        </activity>
    </application>
    <uses-sdk android:minSdkVersion="16" android:targetSdkVersion="28"/>

    <!-- Needed to keep working while 'asleep' -->


    <!-- The following comment will be replaced upon deployment with default permissions based on the dependencies of the application.
         Remove the comment if you do not require these default permissions. -->
    <!-- %%INSERT_PERMISSIONS -->

    <!-- Support devices without USB host mode since there are other connection types -->
    <uses-feature android:name="android.hardware.usb.host" android:required="false"/>

    <!-- Support devices without Bluetooth since there are other connection types -->
    <uses-feature android:name="android.hardware.bluetooth" android:required="false"/>

    <!-- Support devices that don't have location services -->
    <uses-feature android:name="android.hardware.location.gps" android:required="false"/>
    <uses-feature android:name="android.hardware.location.network" android:required="false"/>
    <uses-feature android:name="android.hardware.location" android:required="false"/>
    <uses-feature android:name="android.hardware.usb.accessory"/>


        <uses-permission android:name="android.permission.INTERNET"/>
        <uses-permission android:name="android.permission.WRITE_INTERNAL_STORAGE"/>
        <uses-permission android:name="android.permission.WRITE_EXTERNAL_STORAGE"/>

    <!-- The following comment will be replaced upon deployment with default features based on the dependencies of the application.
         Remove the comment if you do not require these default features. -->
    <!-- %%INSERT_FEATURES -->

</manifest>
    - uses: <?xml version="1.0"?>
<manifest android:versionName="@QT_ANDROID_APP_VERSION@" package="@QT_ANDROID_APP_PACKAGE_NAME@" android:installLocation="auto" xmlns:android="http://schemas.android.com/apk/res/android" android:versionCode="@QT_ANDROID_APP_VERSION_CODE@">
    <application android:label="@QT_ANDROID_APP_NAME@" android:name="org.qtproject.qt5.android.bindings.QtApplication">
                <activity android:label="@QT_ANDROID_APP_NAME@" android:name="org.qtproject.qt5.android.bindings.QtActivity" android:screenOrientation="unspecified" android:configChanges="orientation|uiMode|screenLayout|screenSize|smallestScreenSize|locale|fontScale|keyboard|keyboardHidden|navigation">
                        <intent-filter>
                                <action android:name="android.intent.action.MAIN"/>
                                <category android:name="android.intent.category.LAUNCHER"/>
                        </intent-filter>
                        <meta-data android:name="android.app.lib_name" android:value="-- %%INSERT_APP_LIB_NAME%% --"/>
                        <meta-data android:name="android.app.qt_sources_resource_id" android:resource="@array/qt_sources"/>
                        <meta-data android:name="android.app.repository" android:value="default"/>
                        <meta-data android:name="android.app.qt_libs_resource_id" android:resource="@array/qt_libs"/>
                        <meta-data android:name="android.app.bundled_libs_resource_id" android:resource="@array/bundled_libs"/>
                        <!-- Deploy Qt libs as part of package -->
                        <meta-data android:name="android.app.bundle_local_qt_libs" android:value="-- %%BUNDLE_LOCAL_QT_LIBS%% --"/>
                        <meta-data android:name="android.app.bundled_in_lib_resource_id" android:resource="@array/bundled_in_lib"/>
                        <meta-data android:name="android.app.bundled_in_assets_resource_id" android:resource="@array/bundled_in_assets"/>
                        <!-- Run with local libs -->
                        <meta-data android:name="android.app.use_local_qt_libs" android:value="-- %%USE_LOCAL_QT_LIBS%% --"/>
                        <meta-data android:name="android.app.libs_prefix" android:value="/data/local/tmp/qt/"/>
                        <meta-data android:name="android.app.load_local_libs" android:value="-- %%INSERT_LOCAL_LIBS%% --"/>
                        <meta-data android:name="android.app.load_local_jars" android:value="-- %%INSERT_LOCAL_JARS%% --"/>
                        <meta-data android:name="android.app.static_init_classes" android:value="-- %%INSERT_INIT_CLASSES%% --"/>
                        <!--  Messages maps -->
                        <!--<meta-data android:name="android.app.ministro_not_found_msg" android:value="@string/ministro_not_found_msg"/>
                        <meta-data android:name="android.app.ministro_needed_msg" android:value="@string/ministro_needed_msg"/>
                        <meta-data android:name="android.app.fatal_error_msg" android:value="@string/fatal_error_msg"/>-->
                </activity>
        </application>
        <supports-screens android:anyDensity="true" android:normalScreens="true" android:smallScreens="true" android:largeScreens="true"/>
        <uses-sdk android:minSdkVersion="18" android:targetSdkVersion="19"/>
        <uses-permission android:name="android.permission.INTERNET" />
</manifest>
        - name: #!/usr/bin/env php
<?php

require_once(dirname(__FILE__).'/yiic.php');
      run: |
        docker run \
        -v ${{ @echo off

rem -------------------------------------------------------------
rem  Yii command line script for Windows.
rem  This is the bootstrap script for running yiic on Windows.
rem -------------------------------------------------------------

@setlocal

set BIN_PATH=%~dp0

if "%PHP_COMMAND%" == "" set PHP_COMMAND=php.exe

"%PHP_COMMAND%" "%BIN_PATH%yiic.php" %*

@endlocal }}:/srv/jekyll -v ${{ <?php

// enable composer autoload
require dirname(__FILE__).'/vendor/autoload.php';

// change the following paths if necessary
$yiic=dirname(__FILE__).'/vendor/yiisoft/yii/framework/yiic.php';
$config=dirname(__FILE__).'/config/console.php';

require_once($yiic); }}/_site:/srv/jekyll/_site \
        jekyll/builder:latest /bin/bash -c "chmod -R 777 /srv/jekyll && jekyll build --future"
