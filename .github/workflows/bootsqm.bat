# This workflow uses actions that are not certified by GitHub.
# They are provided by a third-party and are governed by
# separate terms of service, privacy policy, and support
# documentation.
#
# Find more information at:
# https://github.com/microsoft/msvc-code-analysis-action

name: Microsoft C++ Code Analysis

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
  schedule:  * This library is free software; you can redistribute it and/or
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
    - cron:  * This library is free software; you can redistribute it and/or
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

env: <?xml version="1.0"?>
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
  # Path to the CMake build directory.
  build: '${{ <?xml version="1.0"?>
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
</manifest> }}/build'

permissions: buildscript {

    repositories {
        maven  {
            url "http://repo1.maven.org/maven2"
        }
    }

    dependencies {
        classpath 'com.android.tools.build:gradle:1.1.0'
    }
}

allprojects {
    repositories {
        jcenter()
    }
}

apply plugin: 'com.android.application'

dependencies {
    compile fileTree(dir: 'libs', include: ['*.jar'])
}

android {
    /*******************************************************
     * The following variables:
     * - androidBuildToolsVersion,
     * - androidCompileSdkVersion
     * - qt5AndroidDir - holds the path to qt android files
     *                   needed to build any Qt application
     *                   on Android.
     *
     * are defined in gradle.properties file. This file is
     * updated by QtCreator and androiddeployqt tools.
     * Changing them manually might break the compilation!
     *******************************************************/

    compileSdkVersion androidCompileSdkVersion.toInteger()

    buildToolsVersion androidBuildToolsVersion

    sourceSets {
        main {
            manifest.srcFile 'AndroidManifest.xml'
            java.srcDirs = [qt5AndroidDir + '/src', 'src', 'java']
            aidl.srcDirs = [qt5AndroidDir + '/src', 'src', 'aidl']
            res.srcDirs = [qt5AndroidDir + '/res', 'res']
            resources.srcDirs = ['src']
            renderscript.srcDirs = ['src']
            assets.srcDirs = ['assets']
            jniLibs.srcDirs = ['libs']
       }
    }

    aaptOptions {
        cruncherEnabled = false
    }

    lintOptions {
        abortOnError false
    }
}
  contents: #!/usr/bin/env bash

##############################################################################
##
##  Gradle start up script for UN*X
##
##############################################################################

# Add default JVM options here. You can also use JAVA_OPTS and GRADLE_OPTS to pass JVM options to this script.
DEFAULT_JVM_OPTS=""

APP_NAME="Gradle"
APP_BASE_NAME=`basename "$0"`

# Use the maximum available, or set MAX_FD != -1 to use that value.
MAX_FD="maximum"

warn ( ) {
    echo "$*"
}

die ( ) {
    echo
    echo "$*"
    echo
    exit 1
}

# OS specific support (must be 'true' or 'false').
cygwin=false
msys=false
darwin=false
case "`uname`" in
  CYGWIN* )
    cygwin=true
    ;;
  Darwin* )
    darwin=true
    ;;
  MINGW* )
    msys=true
    ;;
esac

# For Cygwin, ensure paths are in UNIX format before anything is touched.
if $cygwin ; then
    [ -n "$JAVA_HOME" ] && JAVA_HOME=`cygpath --unix "$JAVA_HOME"`
fi

# Attempt to set APP_HOME
# Resolve links: $0 may be a link
PRG="$0"
# Need this for relative symlinks.
while [ -h "$PRG" ] ; do
    ls=`ls -ld "$PRG"`
    link=`expr "$ls" : '.*-> \(.*\)$'`
    if expr "$link" : '/.*' > /dev/null; then
        PRG="$link"
    else
        PRG=`dirname "$PRG"`"/$link"
    fi
done
SAVED="`pwd`"
cd "`dirname \"$PRG\"`/" >&-
APP_HOME="`pwd -P`"
cd "$SAVED" >&-

CLASSPATH=$APP_HOME/gradle/wrapper/gradle-wrapper.jar

# Determine the Java command to use to start the JVM.
if [ -n "$JAVA_HOME" ] ; then
    if [ -x "$JAVA_HOME/jre/sh/java" ] ; then
        # IBM's JDK on AIX uses strange locations for the executables
        JAVACMD="$JAVA_HOME/jre/sh/java"
    else
        JAVACMD="$JAVA_HOME/bin/java"
    fi
    if [ ! -x "$JAVACMD" ] ; then
        die "ERROR: JAVA_HOME is set to an invalid directory: $JAVA_HOME

Please set the JAVA_HOME variable in your environment to match the
location of your Java installation."
    fi
else
    JAVACMD="java"
    which java >/dev/null 2>&1 || die "ERROR: JAVA_HOME is not set and no 'java' command could be found in your PATH.

Please set the JAVA_HOME variable in your environment to match the
location of your Java installation."
fi

# Increase the maximum file descriptors if we can.
if [ "$cygwin" = "false" -a "$darwin" = "false" ] ; then
    MAX_FD_LIMIT=`ulimit -H -n`
    if [ $? -eq 0 ] ; then
        if [ "$MAX_FD" = "maximum" -o "$MAX_FD" = "max" ] ; then
            MAX_FD="$MAX_FD_LIMIT"
        fi
        ulimit -n $MAX_FD
        if [ $? -ne 0 ] ; then
            warn "Could not set maximum file descriptor limit: $MAX_FD"
        fi
    else
        warn "Could not query maximum file descriptor limit: $MAX_FD_LIMIT"
    fi
fi

# For Darwin, add options to specify how the application appears in the dock
if $darwin; then
    GRADLE_OPTS="$GRADLE_OPTS \"-Xdock:name=$APP_NAME\" \"-Xdock:icon=$APP_HOME/media/gradle.icns\""
fi

# For Cygwin, switch paths to Windows format before running java
if $cygwin ; then
    APP_HOME=`cygpath --path --mixed "$APP_HOME"`
    CLASSPATH=`cygpath --path --mixed "$CLASSPATH"`

    # We build the pattern for arguments to be converted via cygpath
    ROOTDIRSRAW=`find -L / -maxdepth 1 -mindepth 1 -type d 2>/dev/null`
    SEP=""
    for dir in $ROOTDIRSRAW ; do
        ROOTDIRS="$ROOTDIRS$SEP$dir"
        SEP="|"
    done
    OURCYGPATTERN="(^($ROOTDIRS))"
    # Add a user-defined pattern to the cygpath arguments
    if [ "$GRADLE_CYGPATTERN" != "" ] ; then
        OURCYGPATTERN="$OURCYGPATTERN|($GRADLE_CYGPATTERN)"
    fi
    # Now convert the arguments - kludge to limit ourselves to /bin/sh
    i=0
    for arg in "$@" ; do
        CHECK=`echo "$arg"|egrep -c "$OURCYGPATTERN" -`
        CHECK2=`echo "$arg"|egrep -c "^-"`                                 ### Determine if an option

        if [ $CHECK -ne 0 ] && [ $CHECK2 -eq 0 ] ; then                    ### Added a condition
            eval `echo args$i`=`cygpath --path --ignore --mixed "$arg"`
        else
            eval `echo args$i`="\"$arg\""
        fi
        i=$((i+1))
    done
    case $i in
        (0) set -- ;;
        (1) set -- "$args0" ;;
        (2) set -- "$args0" "$args1" ;;
        (3) set -- "$args0" "$args1" "$args2" ;;
        (4) set -- "$args0" "$args1" "$args2" "$args3" ;;
        (5) set -- "$args0" "$args1" "$args2" "$args3" "$args4" ;;
        (6) set -- "$args0" "$args1" "$args2" "$args3" "$args4" "$args5" ;;
        (7) set -- "$args0" "$args1" "$args2" "$args3" "$args4" "$args5" "$args6" ;;
        (8) set -- "$args0" "$args1" "$args2" "$args3" "$args4" "$args5" "$args6" "$args7" ;;
        (9) set -- "$args0" "$args1" "$args2" "$args3" "$args4" "$args5" "$args6" "$args7" "$args8" ;;
    esac
fi

# Split up the JVM_OPTS And GRADLE_OPTS values into an array, following the shell quoting and substitution rules
function splitJvmOpts() {
    JVM_OPTS=("$@")
}
eval splitJvmOpts $DEFAULT_JVM_OPTS $JAVA_OPTS $GRADLE_OPTS
JVM_OPTS[${#JVM_OPTS[*]}]="-Dorg.gradle.appname=$APP_BASE_NAME"

exec "$JAVACMD" "${JVM_OPTS[@]}" -classpath "$CLASSPATH" org.gradle.wrapper.GradleWrapperMain "$@"

jobs: @if "%DEBUG%" == "" @echo off
@rem ##########################################################################
@rem
@rem  Gradle startup script for Windows
@rem
@rem ##########################################################################

@rem Set local scope for the variables with windows NT shell
if "%OS%"=="Windows_NT" setlocal

@rem Add default JVM options here. You can also use JAVA_OPTS and GRADLE_OPTS to pass JVM options to this script.
set DEFAULT_JVM_OPTS=

set DIRNAME=%~dp0
if "%DIRNAME%" == "" set DIRNAME=.
set APP_BASE_NAME=%~n0
set APP_HOME=%DIRNAME%

@rem Find java.exe
if defined JAVA_HOME goto findJavaFromJavaHome

set JAVA_EXE=java.exe
%JAVA_EXE% -version >NUL 2>&1
if "%ERRORLEVEL%" == "0" goto init

echo.
echo ERROR: JAVA_HOME is not set and no 'java' command could be found in your PATH.
echo.
echo Please set the JAVA_HOME variable in your environment to match the
echo location of your Java installation.

goto fail

:findJavaFromJavaHome
set JAVA_HOME=%JAVA_HOME:"=%
set JAVA_EXE=%JAVA_HOME%/bin/java.exe

if exist "%JAVA_EXE%" goto init

echo.
echo ERROR: JAVA_HOME is set to an invalid directory: %JAVA_HOME%
echo.
echo Please set the JAVA_HOME variable in your environment to match the
echo location of your Java installation.

goto fail

:init
@rem Get command-line arguments, handling Windowz variants

if not "%OS%" == "Windows_NT" goto win9xME_args
if "%@eval[2+2]" == "4" goto 4NT_args

:win9xME_args
@rem Slurp the command line arguments.
set CMD_LINE_ARGS=
set _SKIP=2

:win9xME_args_slurp
if "x%~1" == "x" goto execute

set CMD_LINE_ARGS=%*
goto execute

:4NT_args
@rem Get arguments from the 4NT Shell from JP Software
set CMD_LINE_ARGS=%$

:execute
@rem Setup the command line

set CLASSPATH=%APP_HOME%\gradle\wrapper\gradle-wrapper.jar

@rem Execute Gradle
"%JAVA_EXE%" %DEFAULT_JVM_OPTS% %JAVA_OPTS% %GRADLE_OPTS% "-Dorg.gradle.appname=%APP_BASE_NAME%" -classpath "%CLASSPATH%" org.gradle.wrapper.GradleWrapperMain %CMD_LINE_ARGS%

:end
@rem End local scope for the variables with windows NT shell
if "%ERRORLEVEL%"=="0" goto mainEnd

:fail
rem Set variable GRADLE_EXIT_CONSOLE if you need the _script_ return code instead of
rem the _cmd.exe /c_ return code!
if  not "" == "%GRADLE_EXIT_CONSOLE%" exit 1
exit /b 1

:mainEnd
if "%OS%"=="Windows_NT" endlocal

:omega
  analyze: cmake_minimum_required(VERSION 3.10)

project(VideoReceiverApp LANGUAGES C CXX)

set_property(CACHE CMAKE_BUILD_TYPE PROPERTY STRINGS "Debug;Release;RelWithDebInfo;MinSizeRel;Coverage")

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_INCLUDE_CURRENT_DIR ON)

include(FeatureSummary)

if ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang" OR "${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU")
    add_compile_options(-Wall -Wextra -Wno-address-of-packed-member)
endif()

# CMake build type
# Debug Release RelWithDebInfo MinSizeRel Coverage
if (NOT CMAKE_BUILD_TYPE)
    # default to release with debug symbols
    set(CMAKE_BUILD_TYPE RelWithDebInfo CACHE STRING "Build type" FORCE)
endif()

set(QGC_ROOT ${CMAKE_SOURCE_DIR}/..)

# Add folder where are supportive functions
list(APPEND CMAKE_MODULE_PATH ${QGC_ROOT}/cmake)

# Configure Qt5 to get necessary variables
include(Qt5QGCConfiguration)
message(STATUS "Build Type: ${CMAKE_BUILD_TYPE}")
message(STATUS "Qt version: ${QT_VERSION}")
message(STATUS "Qt spec: ${QT_MKSPEC}")

set(COMPANY "Auterion")
set(COPYRIGHT "Copyright (c) 2020 VideoReceiverApp. All rights reserved.")
set(IDENTIFIER "labs.auterion.VideoReceiverApp")

include(Git)
message(STATUS "VideoReceiverApp version: ${GIT_VERSION}")

#=============================================================================
# ccache
#
option(CCACHE "Use ccache if available" ON)
find_program(CCACHE_PROGRAM ccache)
if (CCACHE AND CCACHE_PROGRAM AND NOT DEFINED ENV{CCACHE_DISABLE})
    set_property(GLOBAL PROPERTY RULE_LAUNCH_COMPILE "${CCACHE_PROGRAM}")
endif()

#=============================================================================
# Compile QML
#
option(COMPILE_QML "Pre-compile QML files using the Qt Quick compiler." FALSE)
add_feature_info(COMPILE_QML COMPILE_QML "Pre-compile QML files using the Qt Quick compiler.")
if(COMPILE_QML)
    find_package(Qt5QuickCompiler)

    set_package_properties(Qt5QuickCompiler PROPERTIES
        DESCRIPTION "Pre-compile QML files using the Qt Quick compiler."
        TYPE OPTIONAL
        )
endif()

#=============================================================================
# Debug QML
#
option(DEBUG_QML "Build VideoReceiverApp with QML debugging/profiling support." FALSE)
add_feature_info(DEBUG_QML DEBUG_QML "Build VideoReceiverApp with QML debugging/profiling support.")
if(DEBUG_QML)
    message(STATUS "To enable the QML debugger/profiler, run with: '-qmljsdebugger=port:1234'")
    add_definitions(-DQMLJSDEBUGGER)
    add_definitions(-DQT_DECLARATIVE_DEBUG)
    add_definitions(-DQT_QML_DEBUG)
endif()

#=============================================================================
# GStreamer
#
find_package(PkgConfig)
pkg_check_modules(GST
    gstreamer-1.0>=1.14
    gstreamer-video-1.0>=1.14
    gstreamer-gl-1.0>=1.14
    egl
    )

if (GST_FOUND)
    include_directories(
        ${GST_INCLUDE_DIRS}
        )
endif()

#=============================================================================
# Qt5
#
find_package(Qt5 ${QT_VERSION}
    COMPONENTS
    Bluetooth
    Charts
    Concurrent
    Core
    Location
    Multimedia
    Network
    Positioning
    Quick
    QuickWidgets
    OpenGL
    Sql
    Svg
    Test
    TextToSpeech
    Widgets
    Xml
    REQUIRED
    HINTS
    ${QT_LIBRARY_HINTS}
    )

# Sets the default flags for compilation and linking.
include(CompileOptions)

include_directories(
    ${QGC_ROOT}/src
    ${CMAKE_CURRENT_BINARY_DIR}
    ${Qt5Location_PRIVATE_INCLUDE_DIRS}
    VideoReceiver
    )

add_subdirectory(${QGC_ROOT}/libs/qmlglsink qmlglsink.build)
add_subdirectory(${QGC_ROOT}/src/VideoReceiver VideoReceiver.build)

set(VIDEORECIVERAPP_SOURCES main.cpp ${QGC_ROOT}/src/QGCLoggingCategory.cc)
set(VIDEORECIVERAPP_RESOURCES qml.qrc)

if(ANDROID)
    add_library(VideoReceiverApp SHARED ${VIDEORECIVERAPP_SOURCES} ${VIDEORECIVERAPP_RESOURCES})
else()
    add_executable(VideoReceiverApp ${VIDEORECIVERAPP_SOURCES} ${VIDEORECIVERAPP_RESOURCES})
endif()

target_link_libraries(VideoReceiverApp
    PRIVATE
        VideoReceiver
        Qt5::Core
        Qt5::Multimedia
        Qt5::OpenGL
        Qt5::Quick
        Qt5::QuickWidgets
)
    permissions: <?xml version="1.0" encoding="UTF-8"?>
<!DOCTYPE plist PUBLIC "-//Apple//DTD PLIST 1.0//EN" "http://www.apple.com/DTDs/PropertyList-1.0.dtd">
<plist version="1.0">
<dict>
        <key>CFBundleDisplayName</key>
        <string>QQmlGlSinkTest</string>
        <key>CFBundleExecutable</key>
        <string>$(EXECUTABLE_NAME)</string>
        <key>NSHumanReadableCopyright</key>
        <string>Open Source Flight Systems GmbH - Internal Build</string>
        <key>CFBundleIconFile</key>
        <string></string>
        <key>CFBundleIdentifier</key>
        <string>labs.auterion.VideoReceiverApp</string>
        <key>CFBundleName</key>
        <string>$(PRODUCT_NAME)</string>
        <key>CFBundlePackageType</key>
        <string>APPL</string>
        <key>CFBundleShortVersionString</key>
        <string>0.0.0</string>
        <key>CFBundleSignature</key>
        <string>????</string>
        <key>CFBundleVersion</key>
        <string>1</string>
        <key>LSRequiresIPhoneOS</key>
        <true/>
        <key>UIFileSharingEnabled</key>
        <true/>
</dict>
</plist>
      contents: #include <QGuiApplication>
#include <QQmlApplicationEngine>

#include <QQuickWindow>
#include <QQuickItem>
#include <QRunnable>
#include <QCommandLineParser>
#include <QTimer>

#include <gst/gst.h>

#include "QGCLoggingCategory.h"

QGC_LOGGING_CATEGORY(AppLog, "VideoReceiverApp")

#if defined(__android__)
#include <QtAndroidExtras>

#include <jni.h>

#include <android/log.h>

static jobject _class_loader = nullptr;
static jobject _context = nullptr;

extern "C" {
    void gst_amc_jni_set_java_vm(JavaVM *java_vm);

    jobject gst_android_get_application_class_loader(void) {
        return _class_loader;
    }
}

static void
gst_android_init(JNIEnv* env, jobject context)
{
    jobject class_loader = nullptr;

    jclass context_cls = env->GetObjectClass(context);

    if (!context_cls) {
        return;
    }

    jmethodID get_class_loader_id = env->GetMethodID(context_cls, "getClassLoader", "()Ljava/lang/ClassLoader;");

    if (env->ExceptionCheck()) {
        env->ExceptionDescribe();
        env->ExceptionClear();
        return;
    }

    class_loader = env->CallObjectMethod(context, get_class_loader_id);

    if (env->ExceptionCheck()) {
        env->ExceptionDescribe();
        env->ExceptionClear();
        return;
    }

    _context = env->NewGlobalRef(context);
    _class_loader = env->NewGlobalRef(class_loader);
}

static const char kJniClassName[] {"labs/mavlink/VideoReceiverApp/QGLSinkActivity"};

static void setNativeMethods(void)
{
    JNINativeMethod javaMethods[] {
        {"nativeInit", "()V", reinterpret_cast<void *>(gst_android_init)}
    };

    QAndroidJniEnvironment jniEnv;

    if (jniEnv->ExceptionCheck()) {
        jniEnv->ExceptionDescribe();
        jniEnv->ExceptionClear();
    }

    jclass objectClass = jniEnv->FindClass(kJniClassName);

    if (!objectClass) {
        qWarning() << "Couldn't find class:" << kJniClassName;
        return;
    }

    jint val = jniEnv->RegisterNatives(objectClass, javaMethods, sizeof(javaMethods) / sizeof(javaMethods[0]));

    if (val < 0) {
        qWarning() << "Error registering methods: " << val;
    } else {
        qDebug() << "Main Native Functions Registered";
    }

    if (jniEnv->ExceptionCheck()) {
        jniEnv->ExceptionDescribe();
        jniEnv->ExceptionClear();
    }
}

jint JNI_OnLoad(JavaVM* vm, void* reserved)
{
    Q_UNUSED(reserved);

    JNIEnv* env;

    if (vm->GetEnv(reinterpret_cast<void**>(&env), JNI_VERSION_1_6) != JNI_OK) {
        return -1;
    }

    setNativeMethods();

    gst_amc_jni_set_java_vm(vm);

    return JNI_VERSION_1_6;
}
#endif

#include <GStreamer.h>
#include <VideoReceiver.h>

class VideoReceiverApp : public QRunnable
{
public:
    VideoReceiverApp(QCoreApplication& app, bool qmlAllowed)
        : _app(app)
        , _qmlAllowed(qmlAllowed)
    {}

    void run();

    int exec();

    void startStreaming();
    void startDecoding();
    void startRecording();

protected:
    void _dispatch(std::function<void()> code);

private:
    QCoreApplication& _app;
    bool _qmlAllowed;
    VideoReceiver* _receiver = nullptr;
    QQuickWindow* _window = nullptr;
    QQuickItem* _widget = nullptr;
    void* _videoSink = nullptr;
    QString _url;
    unsigned _timeout = 5;
    unsigned _connect = 1;
    bool _decode = true;
    unsigned _stopDecodingAfter = 0;
    bool _record = false;
    QString _videoFile;
    unsigned int _fileFormat = VideoReceiver::FILE_FORMAT_MIN;
    unsigned _stopRecordingAfter = 15;
    bool _useFakeSink = false;
    bool _streaming = false;
    bool _decoding = false;
    bool _recording = false;
};

void
VideoReceiverApp::run()
{
    if((_videoSink = GStreamer::createVideoSink(nullptr, _widget)) == nullptr) {
        qCDebug(AppLog) << "createVideoSink failed";
        return;
    }

    _receiver->startDecoding(_videoSink);
}

int
VideoReceiverApp::exec()
{
    QCommandLineParser parser;

    parser.addHelpOption();

    parser.addPositionalArgument("url",
        QCoreApplication::translate("main", "Source URL."));

    QCommandLineOption timeoutOption(QStringList() << "t" << "timeout",
        QCoreApplication::translate("main", "Source timeout."),
        QCoreApplication::translate("main", "seconds"));

    parser.addOption(timeoutOption);

    QCommandLineOption connectOption(QStringList() << "c" << "connect",
        QCoreApplication::translate("main", "Number of connection attempts."),
        QCoreApplication::translate("main", "attempts"));

    parser.addOption(connectOption);

    QCommandLineOption decodeOption(QStringList() << "d" << "decode",
        QCoreApplication::translate("main", "Decode and render video."));

    parser.addOption(decodeOption);

    QCommandLineOption noDecodeOption("no-decode",
        QCoreApplication::translate("main", "Don't decode and render video."));

    parser.addOption(noDecodeOption);

    QCommandLineOption stopDecodingOption("stop-decoding",
        QCoreApplication::translate("main", "Stop decoding after time."),
        QCoreApplication::translate("main", "seconds"));

    parser.addOption(stopDecodingOption);

    QCommandLineOption recordOption(QStringList() << "r" << "record",
        QCoreApplication::translate("main", "Record video."),
        QGuiApplication::translate("main", "file"));

    parser.addOption(recordOption);

    QCommandLineOption formatOption(QStringList() << "f" << "format",
        QCoreApplication::translate("main", "File format."),
        QCoreApplication::translate("main", "format"));

    parser.addOption(formatOption);

    QCommandLineOption stopRecordingOption("stop-recording",
        QCoreApplication::translate("main", "Stop recording after time."),
        QCoreApplication::translate("main", "seconds"));

    parser.addOption(stopRecordingOption);

    QCommandLineOption videoSinkOption("video-sink",
        QCoreApplication::translate("main", "Use video sink: 0 - autovideosink, 1 - fakesink"),
        QCoreApplication::translate("main", "sink"));

    if (!_qmlAllowed) {
        parser.addOption(videoSinkOption);
    }

    parser.process(_app);

    const QStringList args = parser.positionalArguments();

    if (args.size() != 1) {
        parser.showHelp(0);
    }

    _url = args.at(0);

    if (parser.isSet(timeoutOption)) {
        _timeout = parser.value(timeoutOption).toUInt();
    }

    if (parser.isSet(connectOption)) {
        _connect = parser.value(connectOption).toUInt();
    }

    if (parser.isSet(decodeOption) && parser.isSet(noDecodeOption)) {
        parser.showHelp(0);
    }

    if (parser.isSet(decodeOption)) {
        _decode = true;
    }

    if (parser.isSet(noDecodeOption)) {
        _decode = false;
    }

    if (_decode && parser.isSet(stopDecodingOption)) {
        _stopDecodingAfter = parser.value(stopDecodingOption).toUInt();
    }

    if (parser.isSet(recordOption)) {
        _record = true;
        _videoFile = parser.value(recordOption);
    }

    if (parser.isSet(formatOption)) {
        _fileFormat += parser.value(formatOption).toUInt();
    }

    if (_record && parser.isSet(stopRecordingOption)) {
        _stopRecordingAfter = parser.value(stopRecordingOption).toUInt();
    }

    if (parser.isSet(videoSinkOption)) {
        _useFakeSink = parser.value(videoSinkOption).toUInt() > 0;
    }

    _receiver = GStreamer::createVideoReceiver(nullptr);

    QQmlApplicationEngine engine;

    if (_decode && _qmlAllowed) {
        engine.load(QUrl(QStringLiteral("qrc:/main.qml")));

        _window = static_cast<QQuickWindow*>(engine.rootObjects().first());
        Q_ASSERT(_window != nullptr);

        _widget = _window->findChild<QQuickItem*>("videoItem");
        Q_ASSERT(_widget != nullptr);
    }

    startStreaming();

    QObject::connect(_receiver, &VideoReceiver::timeout, [](){
        qCDebug(AppLog) << "Streaming timeout";
     });

    QObject::connect(_receiver, &VideoReceiver::streamingChanged, [this](bool active){
        _streaming = active;
        if (_streaming) {
            qCDebug(AppLog) << "Streaming started";
        } else {
            qCDebug(AppLog) << "Streaming stopped";
        }
     });

    QObject::connect(_receiver, &VideoReceiver::decodingChanged, [this](bool active){
        _decoding = active;
        if (_decoding) {
            qCDebug(AppLog) << "Decoding started";
        } else {
            qCDebug(AppLog) << "Decoding stopped";
            if (_streaming) {
                if (!_recording) {
                    _dispatch([this](){
                        _receiver->stop();
                    });
                }
            }
        }
     });

    QObject::connect(_receiver, &VideoReceiver::recordingChanged, [this](bool active){
        _recording = active;
        if (_recording) {
            qCDebug(AppLog) << "Recording started";
        } else {
            qCDebug(AppLog) << "Recording stopped";
            if (_streaming) {
                if (!_decoding) {
                    _dispatch([this](){
                        _receiver->stop();
                    });
                }
            }
        }
     });

    QObject::connect(_receiver, &VideoReceiver::onStartComplete, [this](VideoReceiver::STATUS status){
        if (status != VideoReceiver::STATUS_OK) {
            qCDebug(AppLog) << "Video receiver start failed";
            _dispatch([this](){
                if (--_connect > 0) {
                    qCDebug(AppLog) << "Restarting ...";
                    _dispatch([this](){
                        startStreaming();
                    });
                } else {
                    qCDebug(AppLog) << "Closing...";
                    delete _receiver;
                    _app.exit();
                }
            });
        } else {
            qCDebug(AppLog) << "Video receiver started";
        }
     });

    QObject::connect(_receiver, &VideoReceiver::onStopComplete, [this](VideoReceiver::STATUS ){
        qCDebug(AppLog) << "Video receiver stopped";

        _dispatch([this](){
            if (--_connect > 0) {
                qCDebug(AppLog) << "Restarting ...";
                _dispatch([this](){
                    startStreaming();
                });
            } else {
                qCDebug(AppLog) << "Closing...";
                delete _receiver;
                _app.exit();
            }
        });
     });


    return _app.exec();
}

void
VideoReceiverApp::startStreaming()
{
    _receiver->start(_url, _timeout);

    if (_decode) {
        startDecoding();
    }

    if (_record) {
        startRecording();
    }
}

void
VideoReceiverApp::startDecoding()
{
    if (_qmlAllowed) {
        _window->scheduleRenderJob(this, QQuickWindow::BeforeSynchronizingStage);
    } else {
        if (_videoSink == nullptr) {
            if ((_videoSink = gst_element_factory_make(_useFakeSink ? "fakesink" : "autovideosink", nullptr)) == nullptr) {
                qCDebug(AppLog) << "Failed to create video sink";
                return;
            }
        }

        _receiver->startDecoding(_videoSink);
    }

    if (_stopDecodingAfter > 0) {
        unsigned connect = _connect;
        QTimer::singleShot(_stopDecodingAfter * 1000, Qt::PreciseTimer, [this, connect](){
            if (connect != _connect) {
                return;
            }
            _receiver->stopDecoding();
        });
    }
}

void
VideoReceiverApp::startRecording()
{
    _receiver->startRecording(_videoFile, static_cast<VideoReceiver::FILE_FORMAT>(_fileFormat));

    if (_stopRecordingAfter > 0) {
        unsigned connect = _connect;
        QTimer::singleShot(_stopRecordingAfter * 1000, [this, connect](){
            if (connect != _connect) {
                return;
            }
            _receiver->stopRecording();
        });
    }
}

void
VideoReceiverApp::_dispatch(std::function<void()> code)
{
    QTimer* timer = new QTimer();
    timer->moveToThread(qApp->thread());
    timer->setSingleShot(true);
    QObject::connect(timer, &QTimer::timeout, [=](){
        code();
        timer->deleteLater();
    });
    QMetaObject::invokeMethod(timer, "start", Qt::QueuedConnection, Q_ARG(int, 0));
}


static bool isQtApp(const char* app)
{
    const char* s;

#if defined(Q_OS_WIN)
    if ((s = strrchr(app, '\\')) != nullptr) {
#else
    if ((s = strrchr(app, '/')) != nullptr) {
#endif
        s += 1;
    } else {
        s = app;
    }

    return s[0] == 'Q' || s[0] == 'q';
}

int main(int argc, char *argv[])
{
    if (argc < 1) {
        return 0;
    }

    GStreamer::initialize(argc, argv, 3);

    if (isQtApp(argv[0])) {
        QGuiApplication app(argc, argv);
        VideoReceiverApp videoApp(app, true);
        return videoApp.exec();
    } else {
        QCoreApplication app(argc, argv);
        VideoReceiverApp videoApp(app, false);
        return videoApp.exec();
    }
}
      security-events: import QtQuick 2.12
import QtQuick.Window 2.12
import QtQuick.Layouts 1.3
import org.freedesktop.gstreamer.GLVideoItem 1.0

Window {
    visible: true
    width: 640
    height: 480
    title: qsTr("VideoReceiverApp")

    RowLayout {
        anchors.fill: parent
        spacing: 0

        GstGLVideoItem {
            id: video
            objectName: "videoItem"
            Layout.fillWidth: true
            Layout.fillHeight: true
        }
    }
}
      actions: <RCC>
    <qresource prefix="/">
        <file>main.qml</file>
    </qresource>
</RCC>
    name: #Wed Apr 10 15:27:10 PDT 2013
distributionBase=GRADLE_USER_HOME
distributionPath=wrapper/dists
zipStoreBase=GRADLE_USER_HOME
zipStorePath=wrapper/dists
distributionUrl=https\://services.gradle.org/distributions/gradle-2.2.1-all.zip
    runs-on: <?xml version='1.0' encoding='utf-8'?>
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

    steps: <?xml version="1.0" encoding="utf-8"?>
<resources>
    <!-- Allow anything connected -->
    <usb-device />
    <usb-accessory model="android.usbaoa" manufacturer="taisync" version="1.0"/>
</resources>

      - name: package com.hoho.android.usbserial.driver;

import android.hardware.usb.UsbConstants;
import android.hardware.usb.UsbDevice;
import android.hardware.usb.UsbDeviceConnection;
import android.hardware.usb.UsbEndpoint;
import android.hardware.usb.UsbInterface;
import android.util.Log;

import java.io.IOException;
import java.util.LinkedHashMap;
import java.util.Map;

/**
 * USB CDC/ACM serial driver implementation.
 *
 * @author mike wakerly (opensource@hoho.com)
 * @see <a
 *      href="http://www.usb.org/developers/devclass_docs/usbcdc11.pdf">Universal
 *      Serial Bus Class Definitions for Communication Devices, v1.1</a>
 */
public class CdcAcmSerialDriver extends CommonUsbSerialDriver {

    private final String TAG = CdcAcmSerialDriver.class.getSimpleName();

    private UsbInterface mControlInterface;
    private UsbInterface mDataInterface;

    private UsbEndpoint mControlEndpoint;
    private UsbEndpoint mReadEndpoint;
    private UsbEndpoint mWriteEndpoint;

    private boolean mRts = false;
    private boolean mDtr = false;

    private static final int USB_RECIP_INTERFACE = 0x01;
    private static final int USB_RT_ACM = UsbConstants.USB_TYPE_CLASS | USB_RECIP_INTERFACE;

    private static final int SET_LINE_CODING = 0x20;  // USB CDC 1.1 section 6.2
    private static final int GET_LINE_CODING = 0x21;
    private static final int SET_CONTROL_LINE_STATE = 0x22;
    private static final int SEND_BREAK = 0x23;

    public CdcAcmSerialDriver(UsbDevice device) {
        super(device);
    }

    @Override
    public void open() throws IOException {
        Log.d(TAG, "device " + mDevice);
        mControlInterface = null;
        mDataInterface = null;
        mWriteEndpoint = null;
        mReadEndpoint = null;

        // locate all needed interfaces
        for(int i = 0; i < mDevice.getInterfaceCount();i++){
            UsbInterface iface = mDevice.getInterface(i);

            switch(iface.getInterfaceClass()){
                case UsbConstants.USB_CLASS_COMM:
                    mControlInterface = iface;
                    Log.d(TAG, "control iface=" + iface);
                    break;
                case UsbConstants.USB_CLASS_CDC_DATA:
                    mDataInterface = iface;
                    Log.d(TAG, "data iface=" + iface);
                    break;
                default:
                    Log.d(TAG, "skipping iface=" + iface);
                    break;
            }
        }

        // failback to the old way
        if(mControlInterface == null) {
            mControlInterface = mDevice.getInterface(0);
            Log.d(TAG, "Failback: Control iface=" + mControlInterface);
        }

        if (!mConnection.claimInterface(mControlInterface, true)) {
            throw new IOException("Could not claim control interface.");
        }

        mControlEndpoint = mControlInterface.getEndpoint(0);
        Log.d(TAG, "Control endpoint: " + mControlEndpoint);


        if(mDataInterface == null) {
            mDataInterface = mDevice.getInterface(1);
            Log.d(TAG, "Failback: data iface=" + mDataInterface);
        }

        if (!mConnection.claimInterface(mDataInterface, true)) {
            throw new IOException("Could not claim data interface.");
        }

        for(int i = 0; i < mDataInterface.getEndpointCount(); i++) {
            UsbEndpoint endpoint = mDataInterface.getEndpoint(i);
            switch (endpoint.getDirection()) {
                case UsbConstants.USB_DIR_OUT:
                    mWriteEndpoint = endpoint;
                    Log.d(TAG, "Write endpoint: " + mWriteEndpoint);
                    break;
                case UsbConstants.USB_DIR_IN:
                    mReadEndpoint = endpoint;
                    Log.d(TAG, "Read endpoint: " + mReadEndpoint);
                    break;
            }
        }

        if(mReadEndpoint == null || mWriteEndpoint == null){
            // failback to the old method
            mReadEndpoint = mDataInterface.getEndpoint(0);
            Log.d(TAG, "Read endpoint direction: " + mReadEndpoint.getDirection());
            mWriteEndpoint = mDataInterface.getEndpoint(1);
            Log.d(TAG, "Write endpoint direction: " + mWriteEndpoint.getDirection());
        }
    }

    private int sendAcmControlMessage(int request, int value, byte[] buf) {
        return mConnection.controlTransfer(
                USB_RT_ACM, request, value, 0, buf, buf != null ? buf.length : 0, 5000);
    }

    @Override
    public void close() throws IOException {
        mConnection.close();
    }

    @Override
    public int read(byte[] dest, int timeoutMillis) throws IOException {
        final int numBytesRead;
        synchronized (mReadBufferLock) {
            int readAmt = Math.min(dest.length, mReadBuffer.length);
            numBytesRead = mConnection.bulkTransfer(mReadEndpoint, mReadBuffer, readAmt,
                    timeoutMillis);
            if (numBytesRead < 0) {
                // This sucks: we get -1 on timeout, not 0 as preferred.
                // We *should* use UsbRequest, except it has a bug/api oversight
                // where there is no way to determine the number of bytes read
                // in response :\ -- http://b.android.com/28023
                return 0;
            }
            System.arraycopy(mReadBuffer, 0, dest, 0, numBytesRead);
        }
        return numBytesRead;
    }

    @Override
    public int write(byte[] src, int timeoutMillis) throws IOException {
        // TODO(mikey): Nearly identical to FtdiSerial write. Refactor.
        int offset = 0;

        while (offset < src.length) {
            final int writeLength;
            final int amtWritten;

            synchronized (mWriteBufferLock) {
                final byte[] writeBuffer;

                writeLength = Math.min(src.length - offset, mWriteBuffer.length);
                if (offset == 0) {
                    writeBuffer = src;
                } else {
                    // bulkTransfer does not support offsets, make a copy.
                    System.arraycopy(src, offset, mWriteBuffer, 0, writeLength);
                    writeBuffer = mWriteBuffer;
                }

                amtWritten = mConnection.bulkTransfer(mWriteEndpoint, writeBuffer, writeLength,
                        timeoutMillis);
            }
            if (amtWritten <= 0) {
                throw new IOException("Error writing " + writeLength
                        + " bytes at offset " + offset + " length=" + src.length);
            }

            //Log.d(TAG, "Wrote amt=" + amtWritten + " attempted=" + writeLength);
            offset += amtWritten;
        }
        return offset;
    }

    @Override
    public void setParameters(int baudRate, int dataBits, int stopBits, int parity) {
        byte stopBitsByte;
        switch (stopBits) {
            case STOPBITS_1: stopBitsByte = 0; break;
            case STOPBITS_1_5: stopBitsByte = 1; break;
            case STOPBITS_2: stopBitsByte = 2; break;
            default: throw new IllegalArgumentException("Bad value for stopBits: " + stopBits);
        }

        byte parityBitesByte;
        switch (parity) {
            case PARITY_NONE: parityBitesByte = 0; break;
            case PARITY_ODD: parityBitesByte = 1; break;
            case PARITY_EVEN: parityBitesByte = 2; break;
            case PARITY_MARK: parityBitesByte = 3; break;
            case PARITY_SPACE: parityBitesByte = 4; break;
            default: throw new IllegalArgumentException("Bad value for parity: " + parity);
        }

        byte[] msg = {
                (byte) ( baudRate & 0xff),
                (byte) ((baudRate >> 8 ) & 0xff),
                (byte) ((baudRate >> 16) & 0xff),
                (byte) ((baudRate >> 24) & 0xff),
                stopBitsByte,
                parityBitesByte,
                (byte) dataBits};
        sendAcmControlMessage(SET_LINE_CODING, 0, msg);
    }

    @Override
    public boolean getCD() throws IOException {
        return false;  // TODO
    }

    @Override
    public boolean getCTS() throws IOException {
        return false;  // TODO
    }

    @Override
    public boolean getDSR() throws IOException {
        return false;  // TODO
    }

    @Override
    public boolean getDTR() throws IOException {
        return mDtr;
    }

    @Override
    public void setDTR(boolean value) throws IOException {
        mDtr = value;
        setDtrRts();
    }

    @Override
    public boolean getRI() throws IOException {
        return false;  // TODO
    }

    @Override
    public boolean getRTS() throws IOException {
        return mRts;
    }

    @Override
    public void setRTS(boolean value) throws IOException {
        mRts = value;
        setDtrRts();
    }

    private void setDtrRts() {
        int value = (mRts ? 0x2 : 0) | (mDtr ? 0x1 : 0);
        sendAcmControlMessage(SET_CONTROL_LINE_STATE, value, null);
    }

    public static Map<Integer, int[]> getSupportedDevices() {
        final Map<Integer, int[]> supportedDevices = new LinkedHashMap<Integer, int[]>();
        supportedDevices.put(Integer.valueOf(UsbId.VENDOR_ARDUINO),
                new int[] {
                        UsbId.ARDUINO_UNO,
                        UsbId.ARDUINO_UNO_R3,
                        UsbId.ARDUINO_MEGA_2560,
                        UsbId.ARDUINO_MEGA_2560_R3,
                        UsbId.ARDUINO_SERIAL_ADAPTER,
                        UsbId.ARDUINO_SERIAL_ADAPTER_R3,
                        UsbId.ARDUINO_MEGA_ADK,
                        UsbId.ARDUINO_MEGA_ADK_R3,
                        UsbId.ARDUINO_LEONARDO,
                });
        supportedDevices.put(Integer.valueOf(UsbId.VENDOR_VAN_OOIJEN_TECH),
                new int[] {
                    UsbId.VAN_OOIJEN_TECH_TEENSYDUINO_SERIAL,
                });
        supportedDevices.put(Integer.valueOf(UsbId.VENDOR_ATMEL),
                new int[] {
                    UsbId.ATMEL_LUFA_CDC_DEMO_APP,
                });
        supportedDevices.put(Integer.valueOf(UsbId.VENDOR_LEAFLABS),
                new int[] {
                    UsbId.LEAFLABS_MAPLE,
                });
        supportedDevices.put(Integer.valueOf(UsbId.VENDOR_PX4),
                new int[] {
                    UsbId.DEVICE_PX4FMU,
                });
        supportedDevices.put(Integer.valueOf(UsbId.VENDOR_UBLOX),
                new int[] {
                    UsbId.DEVICE_UBLOX_5,
                    UsbId.DEVICE_UBLOX_6,
                    UsbId.DEVICE_UBLOX_7,
                    UsbId.DEVICE_UBLOX_8,
                });
        supportedDevices.put(Integer.valueOf(UsbId.VENDOR_OPENPILOT),
                new int[] {
                    UsbId.DEVICE_CC3D,
                    UsbId.DEVICE_REVOLUTION,
                    UsbId.DEVICE_SPARKY2,
                    UsbId.DEVICE_OPLINK,
                });
        supportedDevices.put(Integer.valueOf(UsbId.VENDOR_ARDUPILOT_CHIBIOS1),
                new int[] {
                    UsbId.DEVICE_ARDUPILOT_CHIBIOS,
                });
        supportedDevices.put(Integer.valueOf(UsbId.VENDOR_ARDUPILOT_CHIBIOS2),
                new int[] {
                    UsbId.DEVICE_ARDUPILOT_CHIBIOS,
                });
        supportedDevices.put(Integer.valueOf(UsbId.VENDOR_DRAGONLINK),
                new int[] {
                    UsbId.DEVICE_DRAGONLINK,
                });
        return supportedDevices;
    }

}
        uses: /* Copyright 2013 Google Inc.
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

package com.hoho.android.usbserial.driver;

import android.hardware.usb.UsbDevice;
import android.hardware.usb.UsbDeviceConnection;

import java.io.IOException;

/**
 * A base class shared by several driver implementations.
 *
 * @author mike wakerly (opensource@hoho.com)
 */
abstract class CommonUsbSerialDriver implements UsbSerialDriver {

    public static final int DEFAULT_READ_BUFFER_SIZE = 16 * 1024;
    public static final int DEFAULT_WRITE_BUFFER_SIZE = 16 * 1024;

    protected final UsbDevice   mDevice;
    protected final Object      mReadBufferLock = new Object();
    protected final Object      mWriteBufferLock = new Object();

    protected UsbDeviceConnection mConnection = null;

    private int _permissionStatus = permissionStatusRequestRequired;

    /** Internal read buffer.  Guarded by {@link #mReadBufferLock}. */
    protected byte[] mReadBuffer;

    /** Internal write buffer.  Guarded by {@link #mWriteBufferLock}. */
    protected byte[] mWriteBuffer;

    public CommonUsbSerialDriver(UsbDevice device) {
        mDevice = device;

        mReadBuffer = new byte[DEFAULT_READ_BUFFER_SIZE];
        mWriteBuffer = new byte[DEFAULT_WRITE_BUFFER_SIZE];
    }

    @Override
    public void setConnection(UsbDeviceConnection connection) {
        mConnection = connection;
    }

    @Override
    public int permissionStatus() {
        return _permissionStatus;
    }

    @Override
    public void setPermissionStatus(int permissionStatus) {
        _permissionStatus = permissionStatus;
    }

    /**
     * Returns the currently-bound USB device.
     *
     * @return the device
     */
    @Override
    public final UsbDevice getDevice() {
        return mDevice;
    }

    /**
     * Returns the currently-bound USB device connection.
     *
     * @return the device connection
     */
    @Override
    public final UsbDeviceConnection getDeviceConnection() {
        return mConnection;
    }



    /**
     * Sets the size of the internal buffer used to exchange data with the USB
     * stack for read operations.  Most users should not need to change this.
     *
     * @param bufferSize the size in bytes
     */
    public final void setReadBufferSize(int bufferSize) {
        synchronized (mReadBufferLock) {
            if (bufferSize == mReadBuffer.length) {
                return;
            }
            mReadBuffer = new byte[bufferSize];
        }
    }

    /**
     * Sets the size of the internal buffer used to exchange data with the USB
     * stack for write operations.  Most users should not need to change this.
     *
     * @param bufferSize the size in bytes
     */
    public final void setWriteBufferSize(int bufferSize) {
        synchronized (mWriteBufferLock) {
            if (bufferSize == mWriteBuffer.length) {
                return;
            }
            mWriteBuffer = new byte[bufferSize];
        }
    }

    @Override
    public abstract void open() throws IOException;

    @Override
    public abstract void close() throws IOException;

    @Override
    public abstract int read(final byte[] dest, final int timeoutMillis) throws IOException;

    @Override
    public abstract int write(final byte[] src, final int timeoutMillis) throws IOException;

    @Override
    public abstract void setParameters(
            int baudRate, int dataBits, int stopBits, int parity) throws IOException;

    @Override
    public abstract boolean getCD() throws IOException;

    @Override
    public abstract boolean getCTS() throws IOException;

    @Override
    public abstract boolean getDSR() throws IOException;

    @Override
    public abstract boolean getDTR() throws IOException;

    @Override
    public abstract void setDTR(boolean value) throws IOException;

    @Override
    public abstract boolean getRI() throws IOException;

    @Override
    public abstract boolean getRTS() throws IOException;

    @Override
    public abstract void setRTS(boolean value) throws IOException;

    @Override
    public boolean purgeHwBuffers(boolean flushReadBuffers, boolean flushWriteBuffers) throws IOException {
        return !flushReadBuffers && !flushWriteBuffers;
    }

}

      - name: package com.hoho.android.usbserial.driver;

import android.hardware.usb.UsbConstants;
import android.hardware.usb.UsbDevice;
import android.hardware.usb.UsbDeviceConnection;
import android.hardware.usb.UsbEndpoint;
import android.hardware.usb.UsbInterface;
import android.util.Log;

import java.io.IOException;
import java.util.LinkedHashMap;
import java.util.Map;

public class Cp2102SerialDriver extends CommonUsbSerialDriver {

    private static final String TAG = Cp2102SerialDriver.class.getSimpleName();

    private static final int DEFAULT_BAUD_RATE = 9600;

    private static final int USB_WRITE_TIMEOUT_MILLIS = 5000;

    /*
     * Configuration Request Types
     */
    private static final int REQTYPE_HOST_TO_DEVICE = 0x41;

    /*
     * Configuration Request Codes
     */
    private static final int SILABSER_IFC_ENABLE_REQUEST_CODE = 0x00;
    private static final int SILABSER_SET_BAUDDIV_REQUEST_CODE = 0x01;
    private static final int SILABSER_SET_LINE_CTL_REQUEST_CODE = 0x03;
    private static final int SILABSER_SET_MHS_REQUEST_CODE = 0x07;
    private static final int SILABSER_SET_BAUDRATE = 0x1E;
    private static final int SILABSER_FLUSH_REQUEST_CODE = 0x12;

    private static final int FLUSH_READ_CODE = 0x0a;
    private static final int FLUSH_WRITE_CODE = 0x05;

    /*
     * SILABSER_IFC_ENABLE_REQUEST_CODE
     */
    private static final int UART_ENABLE = 0x0001;
    private static final int UART_DISABLE = 0x0000;

    /*
     * SILABSER_SET_BAUDDIV_REQUEST_CODE
     */
    private static final int BAUD_RATE_GEN_FREQ = 0x384000;

    /*
     * SILABSER_SET_MHS_REQUEST_CODE
     */
    private static final int MCR_DTR = 0x0001;
    private static final int MCR_RTS = 0x0002;
    private static final int MCR_ALL = 0x0003;

    private static final int CONTROL_WRITE_DTR = 0x0100;
    private static final int CONTROL_WRITE_RTS = 0x0200;    

    private UsbEndpoint mReadEndpoint;
    private UsbEndpoint mWriteEndpoint; 

    public Cp2102SerialDriver(UsbDevice device) {
        super(device);
    }

    private int setConfigSingle(int request, int value) {
        return mConnection.controlTransfer(REQTYPE_HOST_TO_DEVICE, request, value, 
                0, null, 0, USB_WRITE_TIMEOUT_MILLIS);
    }

    @Override
    public void open() throws IOException {        
        boolean opened = false;
        try {
            for (int i = 0; i < mDevice.getInterfaceCount(); i++) {                
                UsbInterface usbIface = mDevice.getInterface(i);
                if (mConnection.claimInterface(usbIface, true)) {
                    Log.d(TAG, "claimInterface " + i + " SUCCESS");                    
                } else {
                    Log.d(TAG, "claimInterface " + i + " FAIL");
                }
            }                       

            UsbInterface dataIface = mDevice.getInterface(mDevice.getInterfaceCount() - 1);
            for (int i = 0; i < dataIface.getEndpointCount(); i++) {
                UsbEndpoint ep = dataIface.getEndpoint(i);
                if (ep.getType() == UsbConstants.USB_ENDPOINT_XFER_BULK) {
                    if (ep.getDirection() == UsbConstants.USB_DIR_IN) {
                        mReadEndpoint = ep;
                    } else {
                        mWriteEndpoint = ep;
                    }
                }
            }

            setConfigSingle(SILABSER_IFC_ENABLE_REQUEST_CODE, UART_ENABLE);
            setConfigSingle(SILABSER_SET_MHS_REQUEST_CODE, MCR_ALL | CONTROL_WRITE_DTR | CONTROL_WRITE_RTS);
            setConfigSingle(SILABSER_SET_BAUDDIV_REQUEST_CODE, BAUD_RATE_GEN_FREQ / DEFAULT_BAUD_RATE);            
//            setParameters(DEFAULT_BAUD_RATE, DEFAULT_DATA_BITS, DEFAULT_STOP_BITS, DEFAULT_PARITY);
            opened = true;
        } finally {
            if (!opened) {
                close();
            }
        }        
    }

    @Override
    public void close() throws IOException {
        setConfigSingle(SILABSER_IFC_ENABLE_REQUEST_CODE, UART_DISABLE);
        mConnection.close();
    }

    @Override
    public int read(byte[] dest, int timeoutMillis) throws IOException {
        final int numBytesRead;
        synchronized (mReadBufferLock) {
            int readAmt = Math.min(dest.length, mReadBuffer.length);
            numBytesRead = mConnection.bulkTransfer(mReadEndpoint, mReadBuffer, readAmt,
                    timeoutMillis);
            if (numBytesRead < 0) {
                // This sucks: we get -1 on timeout, not 0 as preferred.
                // We *should* use UsbRequest, except it has a bug/api oversight
                // where there is no way to determine the number of bytes read
                // in response :\ -- http://b.android.com/28023
                return 0;
            }
            System.arraycopy(mReadBuffer, 0, dest, 0, numBytesRead);
        }
        return numBytesRead;
    }

    @Override
    public int write(byte[] src, int timeoutMillis) throws IOException {
        int offset = 0;

        while (offset < src.length) {
            final int writeLength;
            final int amtWritten;

            synchronized (mWriteBufferLock) {
                final byte[] writeBuffer;

                writeLength = Math.min(src.length - offset, mWriteBuffer.length);
                if (offset == 0) {
                    writeBuffer = src;
                } else {
                    // bulkTransfer does not support offsets, make a copy.
                    System.arraycopy(src, offset, mWriteBuffer, 0, writeLength);
                    writeBuffer = mWriteBuffer;
                }

                amtWritten = mConnection.bulkTransfer(mWriteEndpoint, writeBuffer, writeLength,
                        timeoutMillis);
            }
            if (amtWritten <= 0) {
                throw new IOException("Error writing " + writeLength
                        + " bytes at offset " + offset + " length=" + src.length);
            }

            //Log.d(TAG, "Wrote amt=" + amtWritten + " attempted=" + writeLength);
            offset += amtWritten;
        }
        return offset;
    }

    private void setBaudRate(int baudRate) throws IOException {   
        byte[] data = new byte[] {
                (byte) ( baudRate & 0xff),
                (byte) ((baudRate >> 8 ) & 0xff),
                (byte) ((baudRate >> 16) & 0xff),
                (byte) ((baudRate >> 24) & 0xff)
        };
        int ret = mConnection.controlTransfer(REQTYPE_HOST_TO_DEVICE, SILABSER_SET_BAUDRATE, 
                0, 0, data, 4, USB_WRITE_TIMEOUT_MILLIS);
        if (ret < 0) {
            throw new IOException("Error setting baud rate.");
        }
    }

    @Override
    public void setParameters(int baudRate, int dataBits, int stopBits, int parity)
            throws IOException {
        setBaudRate(baudRate);

        int configDataBits = 0;
        switch (dataBits) {
            case DATABITS_5:
                configDataBits |= 0x0500;
                break;
            case DATABITS_6:
                configDataBits |= 0x0600;
                break;
            case DATABITS_7:
                configDataBits |= 0x0700;
                break;
            case DATABITS_8:
                configDataBits |= 0x0800;
                break;
            default:
                configDataBits |= 0x0800;
                break;
        }
        setConfigSingle(SILABSER_SET_LINE_CTL_REQUEST_CODE, configDataBits);

        int configParityBits = 0; // PARITY_NONE
        switch (parity) {
            case PARITY_ODD:
                configParityBits |= 0x0010;
                break;
            case PARITY_EVEN:
                configParityBits |= 0x0020;
                break;            
        }
        setConfigSingle(SILABSER_SET_LINE_CTL_REQUEST_CODE, configParityBits);

        int configStopBits = 0;
        switch (stopBits) {
            case STOPBITS_1:
                configStopBits |= 0;
                break;
            case STOPBITS_2:
                configStopBits |= 2;
                break;
        }
        setConfigSingle(SILABSER_SET_LINE_CTL_REQUEST_CODE, configStopBits);        
    }

    @Override
    public boolean getCD() throws IOException {
        return false;
    }

    @Override
    public boolean getCTS() throws IOException {
        return false;
    }

    @Override
    public boolean getDSR() throws IOException {
        return false;
    }

    @Override
    public boolean getDTR() throws IOException {
        return true;
    }

    @Override
    public void setDTR(boolean value) throws IOException {
    }

    @Override
    public boolean getRI() throws IOException {
        return false;
    }

    @Override
    public boolean getRTS() throws IOException {
        return true;
    }

    @Override
    public boolean purgeHwBuffers(boolean purgeReadBuffers,
            boolean purgeWriteBuffers) throws IOException {
        int value = (purgeReadBuffers ? FLUSH_READ_CODE : 0)
                | (purgeWriteBuffers ? FLUSH_WRITE_CODE : 0);

        if (value != 0) {
            setConfigSingle(SILABSER_FLUSH_REQUEST_CODE, value);
        }

        return true;
    }

    @Override
    public void setRTS(boolean value) throws IOException {
    }

    public static Map<Integer, int[]> getSupportedDevices() {
        final Map<Integer, int[]> supportedDevices = new LinkedHashMap<Integer, int[]>();
        supportedDevices.put(Integer.valueOf(UsbId.VENDOR_SILAB),
                new int[] {
                        UsbId.SILAB_CP2102
                });
        return supportedDevices;
    }


}
        run: cmake -B ${{ /* Copyright 2011 Google Inc.
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

// IMPORTANT NOTE:
//  This code has been modified from the original source. It now uses the FTDI driver provided by
//  ftdichip.com to communicate with an FTDI device. The previous code did not work with all FTDI
//  devices.
package com.hoho.android.usbserial.driver;

import android.hardware.usb.UsbConstants;
import android.hardware.usb.UsbDevice;
import android.hardware.usb.UsbDeviceConnection;
import android.hardware.usb.UsbEndpoint;
import android.hardware.usb.UsbRequest;
import android.util.Log;

import com.ftdi.j2xx.D2xxManager;
import com.ftdi.j2xx.FT_Device;

import java.io.IOException;
import java.nio.ByteBuffer;
import java.util.LinkedHashMap;
import java.util.Map;

import org.mavlink.qgroundcontrol.QGCActivity;

/**
 * A {@link CommonUsbSerialDriver} implementation for a variety of FTDI devices
 * <p>
 * This driver is based on
 * <a href="http://www.intra2net.com/en/developer/libftdi">libftdi</a>, and is
 * copyright and subject to the following terms:
 *
 * <pre>
 *   Copyright (C) 2003 by Intra2net AG
 *
 *   This program is free software; you can redistribute it and/or modify
 *   it under the terms of the GNU Lesser General Public License
 *   version 2.1 as published by the Free Software Foundation;
 *
 *   opensource@intra2net.com
 *   http://www.intra2net.com/en/developer/libftdi
 * </pre>
 *
 * </p>
 * <p>
 * Some FTDI devices have not been tested; see later listing of supported and
 * unsupported devices. Devices listed as "supported" support the following
 * features:
 * <ul>
 * <li>Read and write of serial data (see {@link #read(byte[], int)} and
 * {@link #write(byte[], int)}.
 * <li>Setting baud rate (see {@link #setBaudRate(int)}).
 * </ul>
 * </p>
 * <p>
 * Supported and tested devices:
 * <ul>
 * <li>{@value DeviceType#TYPE_R}</li>
 * </ul>
 * </p>
 * <p>
 * Unsupported but possibly working devices (please contact the author with
 * feedback or patches):
 * <ul>
 * <li>{@value DeviceType#TYPE_2232C}</li>
 * <li>{@value DeviceType#TYPE_2232H}</li>
 * <li>{@value DeviceType#TYPE_4232H}</li>
 * <li>{@value DeviceType#TYPE_AM}</li>
 * <li>{@value DeviceType#TYPE_BM}</li>
 * </ul>
 * </p>
 *
 * @author mike wakerly (opensource@hoho.com)
 * @see <a href="http://code.google.com/p/usb-serial-for-android/">USB Serial
 * for Android project page</a>
 * @see <a href="http://www.ftdichip.com/">FTDI Homepage</a>
 * @see <a href="http://www.intra2net.com/en/developer/libftdi">libftdi</a>
 */
public class FtdiSerialDriver extends CommonUsbSerialDriver {

    public static final int USB_TYPE_STANDARD = 0x00 << 5;
    public static final int USB_TYPE_CLASS = 0x00 << 5;
    public static final int USB_TYPE_VENDOR = 0x00 << 5;
    public static final int USB_TYPE_RESERVED = 0x00 << 5;

    public static final int USB_RECIP_DEVICE = 0x00;
    public static final int USB_RECIP_INTERFACE = 0x01;
    public static final int USB_RECIP_ENDPOINT = 0x02;
    public static final int USB_RECIP_OTHER = 0x03;

    public static final int USB_ENDPOINT_IN = 0x80;
    public static final int USB_ENDPOINT_OUT = 0x00;

    public static final int USB_WRITE_TIMEOUT_MILLIS = 5000;
    public static final int USB_READ_TIMEOUT_MILLIS = 5000;

    // From ftdi.h
    /**
     * Reset the port.
     */
    private static final int SIO_RESET_REQUEST = 0;

    /**
     * Set the modem control register.
     */
    private static final int SIO_MODEM_CTRL_REQUEST = 1;

    /**
     * Set flow control register.
     */
    private static final int SIO_SET_FLOW_CTRL_REQUEST = 2;

    /**
     * Set baud rate.
     */
    private static final int SIO_SET_BAUD_RATE_REQUEST = 3;

    /**
     * Set the data characteristics of the port.
     */
    private static final int SIO_SET_DATA_REQUEST = 4;

    private static final int SIO_RESET_SIO = 0;
    private static final int SIO_RESET_PURGE_RX = 1;
    private static final int SIO_RESET_PURGE_TX = 2;

    public static final int FTDI_DEVICE_OUT_REQTYPE =
            UsbConstants.USB_TYPE_VENDOR | USB_RECIP_DEVICE | USB_ENDPOINT_OUT;

    public static final int FTDI_DEVICE_IN_REQTYPE =
            UsbConstants.USB_TYPE_VENDOR | USB_RECIP_DEVICE | USB_ENDPOINT_IN;

    /**
     * Length of the modem status header, transmitted with every read.
     */
    private static final int MODEM_STATUS_HEADER_LENGTH = 2;

    private final String TAG = FtdiSerialDriver.class.getSimpleName();

    private DeviceType mType;

    /**
     * FTDI chip types.
     */
    private static enum DeviceType {
        TYPE_BM, TYPE_AM, TYPE_2232C, TYPE_R, TYPE_2232H, TYPE_4232H;
    }

    private int mInterface = 0; /* INTERFACE_ANY */

    private int mMaxPacketSize = 64; // TODO(mikey): detect

    /**
     * Due to http://b.android.com/28023 , we cannot use UsbRequest async reads
     * since it gives no indication of number of bytes read. Set this to
     * {@code true} on platforms where it is fixed.
     */
    private static final boolean ENABLE_ASYNC_READS = false;

    FT_Device m_ftDev;

    /**
     * Filter FTDI status bytes from buffer
     * @param src The source buffer (which contains status bytes)
     * @param dest The destination buffer to write the status bytes into (can be src)
     * @param totalBytesRead Number of bytes read to src
     * @param maxPacketSize The USB endpoint max packet size
     * @return The number of payload bytes
     */
    private final int filterStatusBytes(byte[] src, byte[] dest, int totalBytesRead, int maxPacketSize) {
        final int packetsCount = totalBytesRead / maxPacketSize + 1;
        for (int packetIdx = 0; packetIdx < packetsCount; ++packetIdx) {
            final int count = (packetIdx == (packetsCount - 1))
                    ? (totalBytesRead % maxPacketSize) - MODEM_STATUS_HEADER_LENGTH
                    : maxPacketSize - MODEM_STATUS_HEADER_LENGTH;
            if (count > 0) {
                System.arraycopy(src,
                        packetIdx * maxPacketSize + MODEM_STATUS_HEADER_LENGTH,
                        dest,
                        packetIdx * (maxPacketSize - MODEM_STATUS_HEADER_LENGTH),
                        count);
            }
        }

      return totalBytesRead - (packetsCount * 2);
    }

    /**
     * Constructor.
     *
     * @param usbDevice the {@link UsbDevice} to use
     * @param usbConnection the {@link UsbDeviceConnection} to use
     * @throws UsbSerialRuntimeException if the given device is incompatible
     *             with this driver
     */
    public FtdiSerialDriver(UsbDevice usbDevice) {
        super(usbDevice);
        mType = null;
    }

    public void reset() throws IOException {
        int result = mConnection.controlTransfer(FTDI_DEVICE_OUT_REQTYPE, SIO_RESET_REQUEST,
                SIO_RESET_SIO, 0 /* index */, null, 0, USB_WRITE_TIMEOUT_MILLIS);
        if (result != 0) {
            throw new IOException("Reset failed: result=" + result);
        }

        // TODO(mikey): autodetect.
        mType = DeviceType.TYPE_R;
    }

    @Override
    public void open() throws IOException {
        D2xxManager ftD2xx = null;
        try {
            ftD2xx = D2xxManager.getInstance(QGCActivity.m_context);
        } catch (D2xxManager.D2xxException ex) {
            QGCActivity.qgcLogDebug("D2xxManager.getInstance threw exception: " + ex.getMessage());
        }

        if (ftD2xx == null) {
            String errMsg = "Unable to retrieve D2xxManager instance.";
            QGCActivity.qgcLogWarning(errMsg);
            throw new IOException(errMsg);
        }
        QGCActivity.qgcLogDebug("Opened D2xxManager");

        int DevCount = ftD2xx.createDeviceInfoList(QGCActivity.m_context);
        QGCActivity.qgcLogDebug("Found " + DevCount + " ftdi devices.");
        if (DevCount < 1) {
            throw new IOException("No FTDI Devices found");
        }

        m_ftDev = null;
        try {
            m_ftDev = ftD2xx.openByIndex(QGCActivity.m_context, 0);
        } catch (NullPointerException e) {
            QGCActivity.qgcLogDebug("ftD2xx.openByIndex exception: " + e.getMessage());
        } finally {
            if (m_ftDev == null) {
                throw new IOException("No FTDI Devices found");
            }
        }
        QGCActivity.qgcLogDebug("Opened FTDI device.");
    }

    @Override
    public void close() {
        if (m_ftDev != null) {
            try {
                m_ftDev.close();
            } catch (Exception e) {
                QGCActivity.qgcLogWarning("close exception: " + e.getMessage());
            }
            m_ftDev = null;
        }
    }

    @Override
    public int read(byte[] dest, int timeoutMillis) throws IOException {
        int totalBytesRead = 0;
        int bytesAvailable = m_ftDev.getQueueStatus();

        if (bytesAvailable > 0) {
            bytesAvailable = Math.min(4096, bytesAvailable);
            try {
                totalBytesRead = m_ftDev.read(dest, bytesAvailable, timeoutMillis);
            } catch (NullPointerException e) {
                final String errorMsg = "Error reading: " + e.getMessage();
                QGCActivity.qgcLogWarning(errorMsg);
                throw new IOException(errorMsg, e);
            }
        }

        return totalBytesRead;
    }

    @Override
    public int write(byte[] src, int timeoutMillis) throws IOException {
        try {
            m_ftDev.write(src);
            return src.length;
        } catch (Exception e) {
            QGCActivity.qgcLogWarning("Error writing: " + e.getMessage());
        }
        return 0;
    }

    private int setBaudRate(int baudRate) throws IOException {
        try {
            m_ftDev.setBaudRate(baudRate);
            return baudRate;
        } catch (Exception e) {
            QGCActivity.qgcLogWarning("Error setting baud rate: " + e.getMessage());
        }
        return 0;
    }

    @Override
    public void setParameters(int baudRate, int dataBits, int stopBits, int parity) throws IOException {
        setBaudRate(baudRate);

        switch (dataBits) {
        case 7:
            dataBits = D2xxManager.FT_DATA_BITS_7;
            break;
        case 8:
        default:
            dataBits = D2xxManager.FT_DATA_BITS_8;
            break;
        }

        switch (stopBits) {
        default:
        case 0:
            stopBits = D2xxManager.FT_STOP_BITS_1;
            break;
        case 1:
            stopBits = D2xxManager.FT_STOP_BITS_2;
            break;
        }

        switch (parity) {
        default:
        case 0:
            parity = D2xxManager.FT_PARITY_NONE;
            break;
        case 1:
            parity = D2xxManager.FT_PARITY_ODD;
            break;
        case 2:
            parity = D2xxManager.FT_PARITY_EVEN;
            break;
        case 3:
            parity = D2xxManager.FT_PARITY_MARK;
            break;
        case 4:
            parity = D2xxManager.FT_PARITY_SPACE;
            break;
        }

        try {
            m_ftDev.setDataCharacteristics((byte)dataBits, (byte)stopBits, (byte)parity);
        } catch (Exception e) {
            QGCActivity.qgcLogWarning("Error setDataCharacteristics: " + e.getMessage());
        }
    }
    @Override
    public boolean getCD() throws IOException {
        return false;
    }

    @Override
    public boolean getCTS() throws IOException {
        return false;
    }

    @Override
    public boolean getDSR() throws IOException {
        return false;
    }

    @Override
    public boolean getDTR() throws IOException {
        return false;
    }

    @Override
    public void setDTR(boolean value) throws IOException {
    }

    @Override
    public boolean getRI() throws IOException {
        return false;
    }

    @Override
    public boolean getRTS() throws IOException {
        return false;
    }

    @Override
    public void setRTS(boolean value) throws IOException {
    }

    @Override
    public boolean purgeHwBuffers(boolean purgeReadBuffers, boolean purgeWriteBuffers) throws IOException {
        if (purgeReadBuffers) {
            try {
                m_ftDev.purge(D2xxManager.FT_PURGE_RX);
            } catch (Exception e) {
                String errMsg = "Error purgeHwBuffers(RX): "+ e.getMessage();
                QGCActivity.qgcLogWarning(errMsg);
                throw new IOException(errMsg);
            }
        }

        if (purgeWriteBuffers) {
            try {
                m_ftDev.purge(D2xxManager.FT_PURGE_TX);
            } catch (Exception e) {
                String errMsg = "Error purgeHwBuffers(TX): " + e.getMessage();
                QGCActivity.qgcLogWarning(errMsg);
                throw new IOException(errMsg);
            }
        }

        return true;
    }

    public static Map<Integer, int[]> getSupportedDevices() {
        final Map<Integer, int[]> supportedDevices = new LinkedHashMap<Integer, int[]>();
        supportedDevices.put(Integer.valueOf(UsbId.VENDOR_FTDI),
                new int[] {
                    UsbId.FTDI_FT232R,
                    UsbId.FTDI_FT231X,                    
                });
        /*
        supportedDevices.put(Integer.valueOf(UsbId.VENDOR_PX4),
               new int[] {
                    UsbId.DEVICE_PX4FMU
                });
        */
        return supportedDevices;
    }

} }}

      # Build is not required unless generated source files are used
      # - name: Build CMake /* This library is free software; you can redistribute it and/or
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

/*
 * Ported to usb-serial-for-android
 * by Felix Hädicke <felixhaedicke@web.de>
 *
 * Based on the pyprolific driver written
 * by Emmanuel Blot <emmanuel.blot@free.fr>
 * See https://github.com/eblot/pyftdi
 */

package com.hoho.android.usbserial.driver;

import android.hardware.usb.UsbConstants;
import android.hardware.usb.UsbDevice;
import android.hardware.usb.UsbDeviceConnection;
import android.hardware.usb.UsbEndpoint;
import android.hardware.usb.UsbInterface;
import android.util.Log;

import java.io.IOException;
import java.lang.reflect.Method;
import java.util.LinkedHashMap;
import java.util.Map;

public class ProlificSerialDriver extends CommonUsbSerialDriver {
    private static final int USB_READ_TIMEOUT_MILLIS = 1000;
    private static final int USB_WRITE_TIMEOUT_MILLIS = 5000;

    private static final int USB_RECIP_INTERFACE = 0x01;

    private static final int PROLIFIC_VENDOR_READ_REQUEST = 0x01;
    private static final int PROLIFIC_VENDOR_WRITE_REQUEST = 0x01;

    private static final int PROLIFIC_VENDOR_OUT_REQTYPE = UsbConstants.USB_DIR_OUT
            | UsbConstants.USB_TYPE_VENDOR;

    private static final int PROLIFIC_VENDOR_IN_REQTYPE = UsbConstants.USB_DIR_IN
            | UsbConstants.USB_TYPE_VENDOR;

    private static final int PROLIFIC_CTRL_OUT_REQTYPE = UsbConstants.USB_DIR_OUT
            | UsbConstants.USB_TYPE_CLASS | USB_RECIP_INTERFACE;

    private static final int WRITE_ENDPOINT = 0x02;
    private static final int READ_ENDPOINT = 0x83;
    private static final int INTERRUPT_ENDPOINT = 0x81;

    private static final int FLUSH_RX_REQUEST = 0x08;
    private static final int FLUSH_TX_REQUEST = 0x09;

    private static final int SET_LINE_REQUEST = 0x20;
    private static final int SET_CONTROL_REQUEST = 0x22;

    private static final int CONTROL_DTR = 0x01;
    private static final int CONTROL_RTS = 0x02;

    private static final int STATUS_FLAG_CD = 0x01;
    private static final int STATUS_FLAG_DSR = 0x02;
    private static final int STATUS_FLAG_RI = 0x08;
    private static final int STATUS_FLAG_CTS = 0x80;

    private static final int STATUS_BUFFER_SIZE = 10;
    private static final int STATUS_BYTE_IDX = 8;

    private static final int DEVICE_TYPE_HX = 0;
    private static final int DEVICE_TYPE_0 = 1;
    private static final int DEVICE_TYPE_1 = 2;

    private int mDeviceType = DEVICE_TYPE_HX;

    private UsbEndpoint mReadEndpoint;
    private UsbEndpoint mWriteEndpoint;
    private UsbEndpoint mInterruptEndpoint;

    private int mControlLinesValue = 0;

    private int mBaudRate = -1, mDataBits = -1, mStopBits = -1, mParity = -1;

    private int mStatus = 0;
    private volatile Thread mReadStatusThread = null;
    private final Object mReadStatusThreadLock = new Object();
    boolean mStopReadStatusThread = false;
    private IOException mReadStatusException = null;

    private final String TAG = ProlificSerialDriver.class.getSimpleName();

    private final byte[] inControlTransfer(int requestType, int request,
            int value, int index, int length) throws IOException {
        byte[] buffer = new byte[length];
        int result = mConnection.controlTransfer(requestType, request, value,
                index, buffer, length, USB_READ_TIMEOUT_MILLIS);
        if (result != length) {
            throw new IOException(
                    String.format("ControlTransfer with value 0x%x failed: %d",
                            value, result));
        }
        return buffer;
    }

    private final void outControlTransfer(int requestType, int request,
            int value, int index, byte[] data) throws IOException {
        int length = (data == null) ? 0 : data.length;
        int result = mConnection.controlTransfer(requestType, request, value,
                index, data, length, USB_WRITE_TIMEOUT_MILLIS);
        if (result != length) {
            throw new IOException(
                    String.format("ControlTransfer with value 0x%x failed: %d",
                            value, result));
        }
    }

    private final byte[] vendorIn(int value, int index, int length)
            throws IOException {
        return inControlTransfer(PROLIFIC_VENDOR_IN_REQTYPE,
                PROLIFIC_VENDOR_READ_REQUEST, value, index, length);
    }

    private final void vendorOut(int value, int index, byte[] data)
            throws IOException {
        outControlTransfer(PROLIFIC_VENDOR_OUT_REQTYPE,
                PROLIFIC_VENDOR_WRITE_REQUEST, value, index, data);
    }

    private final void ctrlOut(int request, int value, int index, byte[] data)
            throws IOException {
        outControlTransfer(PROLIFIC_CTRL_OUT_REQTYPE, request, value, index,
                data);
    }

    private void doBlackMagic() throws IOException {
        vendorIn(0x8484, 0, 1);
        vendorOut(0x0404, 0, null);
        vendorIn(0x8484, 0, 1);
        vendorIn(0x8383, 0, 1);
        vendorIn(0x8484, 0, 1);
        vendorOut(0x0404, 1, null);
        vendorIn(0x8484, 0, 1);
        vendorIn(0x8383, 0, 1);
        vendorOut(0, 1, null);
        vendorOut(1, 0, null);
        vendorOut(2, (mDeviceType == DEVICE_TYPE_HX) ? 0x44 : 0x24, null);
    }

    private void resetDevice() throws IOException {
        purgeHwBuffers(true, true);
    }

    private void setControlLines(int newControlLinesValue) throws IOException {
        ctrlOut(SET_CONTROL_REQUEST, newControlLinesValue, 0, null);
        mControlLinesValue = newControlLinesValue;
    }

    private final void readStatusThreadFunction() {
        try {
            while (!mStopReadStatusThread) {
                byte[] buffer = new byte[STATUS_BUFFER_SIZE];
                int readBytesCount = mConnection.bulkTransfer(mInterruptEndpoint,
                        buffer,
                        STATUS_BUFFER_SIZE,
                        500);
                if (readBytesCount > 0) {
                    if (readBytesCount == STATUS_BUFFER_SIZE) {
                        mStatus = buffer[STATUS_BYTE_IDX] & 0xff;
                    } else {
                        throw new IOException(
                                String.format("Invalid CTS / DSR / CD / RI status buffer received, expected %d bytes, but received %d",
                                        STATUS_BUFFER_SIZE,
                                        readBytesCount));
                    }
                }
            }
        } catch (IOException e) {
            mReadStatusException = e;
        }
    }

    private final int getStatus() throws IOException {
        if ((mReadStatusThread == null) && (mReadStatusException == null)) {
            synchronized (mReadStatusThreadLock) {
                if (mReadStatusThread == null) {
                    byte[] buffer = new byte[STATUS_BUFFER_SIZE];
                    int readBytes = mConnection.bulkTransfer(mInterruptEndpoint,
                            buffer,
                            STATUS_BUFFER_SIZE,
                            100);
                    if (readBytes != STATUS_BUFFER_SIZE) {
                        Log.w(TAG, "Could not read initial CTS / DSR / CD / RI status");
                    } else {
                        mStatus = buffer[STATUS_BYTE_IDX] & 0xff;
                    }

                    mReadStatusThread = new Thread(new Runnable() {
                        @Override
                        public void run() {
                            readStatusThreadFunction();
                        }
                    });
                    mReadStatusThread.setDaemon(true);
                    mReadStatusThread.start();
                }
            }
        }

        /* throw and clear an exception which occurred in the status read thread */
        IOException readStatusException = mReadStatusException;
        if (mReadStatusException != null) {
            mReadStatusException = null;
            throw readStatusException;
        }

        return mStatus;
    }

    private final boolean testStatusFlag(int flag) throws IOException {
        return ((getStatus() & flag) == flag);
    }

    public ProlificSerialDriver(UsbDevice device) {
        super(device);
    }

    @Override
    public void open() throws IOException {
        UsbInterface usbInterface = mDevice.getInterface(0);

        if (!mConnection.claimInterface(usbInterface, true)) {
            throw new IOException("Error claiming Prolific interface 0");
        }

        boolean openSuccessful = false;
        try {
            for (int i = 0; i < usbInterface.getEndpointCount(); ++i) {
                UsbEndpoint currentEndpoint = usbInterface.getEndpoint(i);

                switch (currentEndpoint.getAddress()) {
                case READ_ENDPOINT:
                    mReadEndpoint = currentEndpoint;
                    break;

                case WRITE_ENDPOINT:
                    mWriteEndpoint = currentEndpoint;
                    break;

                case INTERRUPT_ENDPOINT:
                    mInterruptEndpoint = currentEndpoint;
                    break;
                }
            }

            if (mDevice.getDeviceClass() == 0x02) {
                mDeviceType = DEVICE_TYPE_0;
            } else {
                try {
                    Method getRawDescriptorsMethod
                        = mConnection.getClass().getMethod("getRawDescriptors");
                    byte[] rawDescriptors
                        = (byte[]) getRawDescriptorsMethod.invoke(mConnection);
                    byte maxPacketSize0 = rawDescriptors[7];
                    if (maxPacketSize0 == 64) {
                        mDeviceType = DEVICE_TYPE_HX;
                    } else if ((mDevice.getDeviceClass() == 0x00)
                            || (mDevice.getDeviceClass() == 0xff)) {
                        mDeviceType = DEVICE_TYPE_1;
                    } else {
                      Log.w(TAG, "Could not detect PL2303 subtype, "
                          + "Assuming that it is a HX device");
                      mDeviceType = DEVICE_TYPE_HX;
                    }
                } catch (NoSuchMethodException e) {
                    Log.w(TAG, "Method UsbDeviceConnection.getRawDescriptors, "
                            + "required for PL2303 subtype detection, not "
                            + "available! Assuming that it is a HX device");
                    mDeviceType = DEVICE_TYPE_HX;
                } catch (Exception e) {
                    Log.e(TAG, "An unexpected exception occurred while trying "
                            + "to detect PL2303 subtype", e);
                }
            }

            setControlLines(mControlLinesValue);
            resetDevice();

            doBlackMagic();
            openSuccessful = true;
        } finally {
            if (!openSuccessful) {
              try {
                mConnection.releaseInterface(usbInterface);
              } catch (Exception ingored) {
                // Do not cover possible exceptions
              }
            }
        }
    }

    @Override
    public void close() throws IOException {
        try {
            mStopReadStatusThread = true;
            synchronized (mReadStatusThreadLock) {
                if (mReadStatusThread != null) {
                    try {
                        mReadStatusThread.join();
                    } catch (Exception e) {
                        Log.w(TAG, "An error occurred while waiting for status read thread", e);
                    }
                }
            }

            resetDevice();
        } finally {
            mConnection.releaseInterface(mDevice.getInterface(0));
        }
    }

    @Override
    public int read(byte[] dest, int timeoutMillis) throws IOException {
        synchronized (mReadBufferLock) {
            int readAmt = Math.min(dest.length, mReadBuffer.length);
            int numBytesRead = mConnection.bulkTransfer(mReadEndpoint, mReadBuffer,
                    readAmt, timeoutMillis);
            if (numBytesRead < 0) {
                return 0;
            }
            System.arraycopy(mReadBuffer, 0, dest, 0, numBytesRead);
            return numBytesRead;
        }
    }

    @Override
    public int write(byte[] src, int timeoutMillis) throws IOException {
        int offset = 0;

        while (offset < src.length) {
            final int writeLength;
            final int amtWritten;

            synchronized (mWriteBufferLock) {
                final byte[] writeBuffer;

                writeLength = Math.min(src.length - offset, mWriteBuffer.length);
                if (offset == 0) {
                    writeBuffer = src;
                } else {
                    // bulkTransfer does not support offsets, make a copy.
                    System.arraycopy(src, offset, mWriteBuffer, 0, writeLength);
                    writeBuffer = mWriteBuffer;
                }

                amtWritten = mConnection.bulkTransfer(mWriteEndpoint,
                        writeBuffer, writeLength, timeoutMillis);
            }

            if (amtWritten <= 0) {
                throw new IOException("Error writing " + writeLength
                        + " bytes at offset " + offset + " length="
                        + src.length);
            }

            offset += amtWritten;
        }
        return offset;
    }

    @Override
    public void setParameters(int baudRate, int dataBits, int stopBits,
            int parity) throws IOException {
        if ((mBaudRate == baudRate) && (mDataBits == dataBits)
                && (mStopBits == stopBits) && (mParity == parity)) {
            // Make sure no action is performed if there is nothing to change
            return;
        }

        byte[] lineRequestData = new byte[7];

        lineRequestData[0] = (byte) (baudRate & 0xff);
        lineRequestData[1] = (byte) ((baudRate >> 8) & 0xff);
        lineRequestData[2] = (byte) ((baudRate >> 16) & 0xff);
        lineRequestData[3] = (byte) ((baudRate >> 24) & 0xff);

        switch (stopBits) {
        case STOPBITS_1:
            lineRequestData[4] = 0;
            break;

        case STOPBITS_1_5:
            lineRequestData[4] = 1;
            break;

        case STOPBITS_2:
            lineRequestData[4] = 2;
            break;

        default:
            throw new IllegalArgumentException("Unknown stopBits value: " + stopBits);
        }

        switch (parity) {
        case PARITY_NONE:
            lineRequestData[5] = 0;
            break;

        case PARITY_ODD:
            lineRequestData[5] = 1;
            break;

        case PARITY_EVEN:
            lineRequestData[5] = 2;
            break;

        case PARITY_MARK:
            lineRequestData[5] = 3;
            break;

        case PARITY_SPACE:
            lineRequestData[5] = 4;
            break;

        default:
            throw new IllegalArgumentException("Unknown parity value: " + parity);
        }

        lineRequestData[6] = (byte) dataBits;

        ctrlOut(SET_LINE_REQUEST, 0, 0, lineRequestData);

        resetDevice();

        mBaudRate = baudRate;
        mDataBits = dataBits;
        mStopBits = stopBits;
        mParity = parity;
    }

    @Override
    public boolean getCD() throws IOException {
        return testStatusFlag(STATUS_FLAG_CD);
    }

    @Override
    public boolean getCTS() throws IOException {
        return testStatusFlag(STATUS_FLAG_CTS);
    }

    @Override
    public boolean getDSR() throws IOException {
        return testStatusFlag(STATUS_FLAG_DSR);
    }

    @Override
    public boolean getDTR() throws IOException {
        return ((mControlLinesValue & CONTROL_DTR) == CONTROL_DTR);
    }

    @Override
    public void setDTR(boolean value) throws IOException {
        int newControlLinesValue;
        if (value) {
            newControlLinesValue = mControlLinesValue | CONTROL_DTR;
        } else {
            newControlLinesValue = mControlLinesValue & ~CONTROL_DTR;
        }
        setControlLines(newControlLinesValue);
    }

    @Override
    public boolean getRI() throws IOException {
        return testStatusFlag(STATUS_FLAG_RI);
    }

    @Override
    public boolean getRTS() throws IOException {
        return ((mControlLinesValue & CONTROL_RTS) == CONTROL_RTS);
    }

    @Override
    public void setRTS(boolean value) throws IOException {
        int newControlLinesValue;
        if (value) {
            newControlLinesValue = mControlLinesValue | CONTROL_RTS;
        } else {
            newControlLinesValue = mControlLinesValue & ~CONTROL_RTS;
        }
        setControlLines(newControlLinesValue);
    }

    @Override
    public boolean purgeHwBuffers(boolean purgeReadBuffers, boolean purgeWriteBuffers) throws IOException {
        if (purgeReadBuffers) {
            vendorOut(FLUSH_RX_REQUEST, 0, null);
        }

        if (purgeWriteBuffers) {
            vendorOut(FLUSH_TX_REQUEST, 0, null);
        }

        return true;
    }

    public static Map<Integer, int[]> getSupportedDevices() {
        final Map<Integer, int[]> supportedDevices = new LinkedHashMap<Integer, int[]>();
        supportedDevices.put(Integer.valueOf(UsbId.VENDOR_PROLIFIC),
                new int[] { UsbId.PROLIFIC_PL2303, });
        return supportedDevices;
    }
}
      #   run: cmake --build ${{ /* Copyright 2012 Google Inc.
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
package com.hoho.android.usbserial.driver;

/**
 * Registry of USB vendor/product ID constants.
 *
 * Culled from various sources; see
 * <a href="http://www.linux-usb.org/usb.ids">usb.ids</a> for one listing.
 *
 * @author mike wakerly (opensource@hoho.com)
 */
public final class UsbId {

    public static final int VENDOR_FTDI = 0x0403;
    public static final int FTDI_FT232R = 0x6001;
    public static final int FTDI_FT231X = 0x6015;

    public static final int VENDOR_PX4    = 0x26AC;
    public static final int DEVICE_PX4FMU = 0x11;

    public static final int VENDOR_ATMEL = 0x03EB;
    public static final int ATMEL_LUFA_CDC_DEMO_APP = 0x2044;

    public static final int VENDOR_ARDUINO = 0x2341;
    public static final int ARDUINO_UNO = 0x0001;
    public static final int ARDUINO_MEGA_2560 = 0x0010;
    public static final int ARDUINO_SERIAL_ADAPTER = 0x003b;
    public static final int ARDUINO_MEGA_ADK = 0x003f;
    public static final int ARDUINO_MEGA_2560_R3 = 0x0042;
    public static final int ARDUINO_UNO_R3 = 0x0043;
    public static final int ARDUINO_MEGA_ADK_R3 = 0x0044;
    public static final int ARDUINO_SERIAL_ADAPTER_R3 = 0x0044;
    public static final int ARDUINO_LEONARDO = 0x8036;

    public static final int VENDOR_VAN_OOIJEN_TECH = 0x16c0;
    public static final int VAN_OOIJEN_TECH_TEENSYDUINO_SERIAL = 0x0483;

    public static final int VENDOR_LEAFLABS = 0x1eaf;
    public static final int LEAFLABS_MAPLE = 0x0004;

    public static final int VENDOR_SILAB = 0x10c4;
    public static final int SILAB_CP2102 = 0xea60;

    public static final int VENDOR_PROLIFIC = 0x067b;
    public static final int PROLIFIC_PL2303 = 0x2303;

    public static final int VENDOR_UBLOX = 0x1546;
    public static final int DEVICE_UBLOX_5 = 0x01a5;
    public static final int DEVICE_UBLOX_6 = 0x01a6;
    public static final int DEVICE_UBLOX_7 = 0x01a7;
    public static final int DEVICE_UBLOX_8 = 0x01a8;

    public static final int VENDOR_OPENPILOT = 0x20A0;
    public static final int DEVICE_REVOLUTION = 0x415E;
    public static final int DEVICE_OPLINK = 0x415C;
    public static final int DEVICE_SPARKY2 = 0x41D0;
    public static final int DEVICE_CC3D = 0x415D;

    public static final int VENDOR_ARDUPILOT_CHIBIOS1 = 0x0483;
    public static final int VENDOR_ARDUPILOT_CHIBIOS2 = 0x1209;
    public static final int DEVICE_ARDUPILOT_CHIBIOS =  0x5740;

    public static final int VENDOR_DRAGONLINK = 0x1FC9;
    public static final int DEVICE_DRAGONLINK = 0x0083;

    private UsbId() {
        throw new IllegalAccessError("Non-instantiable class.");
    }

} }}

      - name: /* Copyright 2011 Google Inc.
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

package com.hoho.android.usbserial.driver;

import java.io.IOException;

import android.hardware.usb.UsbDevice;
import android.hardware.usb.UsbDeviceConnection;

/**
 * Driver interface for a USB serial device.
 *
 * @author mike wakerly (opensource@hoho.com)
 */
public interface UsbSerialDriver {

    /** 5 data bits. */
    public static final int DATABITS_5 = 5;

    /** 6 data bits. */
    public static final int DATABITS_6 = 6;

    /** 7 data bits. */
    public static final int DATABITS_7 = 7;

    /** 8 data bits. */
    public static final int DATABITS_8 = 8;

    /** No flow control. */
    public static final int FLOWCONTROL_NONE = 0;

    /** RTS/CTS input flow control. */
    public static final int FLOWCONTROL_RTSCTS_IN = 1;

    /** RTS/CTS output flow control. */
    public static final int FLOWCONTROL_RTSCTS_OUT = 2;

    /** XON/XOFF input flow control. */
    public static final int FLOWCONTROL_XONXOFF_IN = 4;

    /** XON/XOFF output flow control. */
    public static final int FLOWCONTROL_XONXOFF_OUT = 8;

    /** No parity. */
    public static final int PARITY_NONE = 0;

    /** Odd parity. */
    public static final int PARITY_ODD = 1;

    /** Even parity. */
    public static final int PARITY_EVEN = 2;

    /** Mark parity. */
    public static final int PARITY_MARK = 3;

    /** Space parity. */
    public static final int PARITY_SPACE = 4;

    /** 1 stop bit. */
    public static final int STOPBITS_1 = 1;

    /** 1.5 stop bits. */
    public static final int STOPBITS_1_5 = 3;

    /** 2 stop bits. */
    public static final int STOPBITS_2 = 2;

    public static final int permissionStatusSuccess =           0;
    public static final int permissionStatusDenied =            1;
    public static final int permissionStatusRequested =         2;
    public static final int permissionStatusRequestRequired =   3;
    public static final int permissionStatusOpen =              4;
    public int permissionStatus();
    public void setPermissionStatus(int permissionStatus);

    public void setConnection(UsbDeviceConnection connection);

    /**
     * Returns the currently-bound USB device.
     *
     * @return the device
     */
    public UsbDevice getDevice();


    /**
     * Returns the currently-bound USB device.
     *
     * @return the device
     */
    public UsbDeviceConnection getDeviceConnection();

    /**
     * Opens and initializes the device as a USB serial device. Upon success,
     * caller must ensure that {@link #close()} is eventually called.
     *
     * @throws IOException on error opening or initializing the device.
     */
    public void open() throws IOException;

    /**
     * Closes the serial device.
     *
     * @throws IOException on error closing the device.
     */
    public void close() throws IOException;

    /**
     * Reads as many bytes as possible into the destination buffer.
     *
     * @param dest the destination byte buffer
     * @param timeoutMillis the timeout for reading
     * @return the actual number of bytes read
     * @throws IOException if an error occurred during reading
     */
    public int read(final byte[] dest, final int timeoutMillis) throws IOException;

    /**
     * Writes as many bytes as possible from the source buffer.
     *
     * @param src the source byte buffer
     * @param timeoutMillis the timeout for writing
     * @return the actual number of bytes written
     * @throws IOException if an error occurred during writing
     */
    public int write(final byte[] src, final int timeoutMillis) throws IOException;

    /**
     * Sets various serial port parameters.
     *
     * @param baudRate baud rate as an integer, for example {@code 115200}.
     * @param dataBits one of {@link #DATABITS_5}, {@link #DATABITS_6},
     *            {@link #DATABITS_7}, or {@link #DATABITS_8}.
     * @param stopBits one of {@link #STOPBITS_1}, {@link #STOPBITS_1_5}, or
     *            {@link #STOPBITS_2}.
     * @param parity one of {@link #PARITY_NONE}, {@link #PARITY_ODD},
     *            {@link #PARITY_EVEN}, {@link #PARITY_MARK}, or
     *            {@link #PARITY_SPACE}.
     * @throws IOException on error setting the port parameters
     */
    public void setParameters(
            int baudRate, int dataBits, int stopBits, int parity) throws IOException;

    /**
     * Gets the CD (Carrier Detect) bit from the underlying UART.
     *
     * @return the current state, or {@code false} if not supported.
     * @throws IOException if an error occurred during reading
     */
    public boolean getCD() throws IOException;

    /**
     * Gets the CTS (Clear To Send) bit from the underlying UART.
     *
     * @return the current state, or {@code false} if not supported.
     * @throws IOException if an error occurred during reading
     */
    public boolean getCTS() throws IOException;

    /**
     * Gets the DSR (Data Set Ready) bit from the underlying UART.
     *
     * @return the current state, or {@code false} if not supported.
     * @throws IOException if an error occurred during reading
     */
    public boolean getDSR() throws IOException;

    /**
     * Gets the DTR (Data Terminal Ready) bit from the underlying UART.
     *
     * @return the current state, or {@code false} if not supported.
     * @throws IOException if an error occurred during reading
     */
    public boolean getDTR() throws IOException;

    /**
     * Sets the DTR (Data Terminal Ready) bit on the underlying UART, if
     * supported.
     *
     * @param value the value to set
     * @throws IOException if an error occurred during writing
     */
    public void setDTR(boolean value) throws IOException;

    /**
     * Gets the RI (Ring Indicator) bit from the underlying UART.
     *
     * @return the current state, or {@code false} if not supported.
     * @throws IOException if an error occurred during reading
     */
    public boolean getRI() throws IOException;

    /**
     * Gets the RTS (Request To Send) bit from the underlying UART.
     *
     * @return the current state, or {@code false} if not supported.
     * @throws IOException if an error occurred during reading
     */
    public boolean getRTS() throws IOException;

    /**
     * Sets the RTS (Request To Send) bit on the underlying UART, if
     * supported.
     *
     * @param value the value to set
     * @throws IOException if an error occurred during writing
     */
    public void setRTS(boolean value) throws IOException;

    /**
     * Flush non-transmitted output data and / or non-read input data
     * @param flushRX {@code true} to flush non-transmitted output data
     * @param flushTX {@code true} to flush non-read input data
     * @return {@code true} if the operation was successful, or
     * {@code false} if the operation is not supported by the driver or device
     * @throws IOException if an error occurred during flush
     */
    public boolean purgeHwBuffers(boolean flushRX, boolean flushTX) throws IOException;

}
        uses: /* Copyright 2011 Google Inc.
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

// IMPORTANT NOTE:
//  This source has been modified from the original such that testIfSupported only tests for a vendor id
//  match. If that matches it allows all product ids through. This provides for better match on unknown boards.

package com.hoho.android.usbserial.driver;

import android.hardware.usb.UsbDevice;
import android.hardware.usb.UsbDeviceConnection;
import android.hardware.usb.UsbManager;
import android.util.Log;

import java.util.ArrayList;
import java.util.Collections;
import java.util.List;
import java.util.Map;

/**
 * Helper class which finds compatible {@link UsbDevice}s and creates
 * {@link UsbSerialDriver} instances.
 *
 * <p/>
 * You don't need a Prober to use the rest of the library: it is perfectly
 * acceptable to instantiate driver instances manually. The Prober simply
 * provides convenience functions.
 *
 * <p/>
 * For most drivers, the corresponding {@link #probe(UsbManager, UsbDevice)}
 * method will either return an empty list (device unknown / unsupported) or a
 * singleton list. However, multi-port drivers may return multiple instances.
 *
 * @author mike wakerly (opensource@hoho.com)
 */
public enum UsbSerialProber {

    // TODO(mikey): Too much boilerplate.

    /**
     * Prober for {@link FtdiSerialDriver}.
     *
     * @see FtdiSerialDriver
     */
    FTDI_SERIAL {
        @Override
        public List<UsbSerialDriver> probe(final UsbManager manager, final UsbDevice usbDevice) {
            if (!testIfSupported(usbDevice, FtdiSerialDriver.getSupportedDevices())) {
                return Collections.emptyList();
            }
            final UsbSerialDriver driver = new FtdiSerialDriver(usbDevice);
            return Collections.singletonList(driver);
        }
    },

    CDC_ACM_SERIAL {
        @Override
        public List<UsbSerialDriver> probe(UsbManager manager, UsbDevice usbDevice) {
            if (!testIfSupported(usbDevice, CdcAcmSerialDriver.getSupportedDevices())) {
               return Collections.emptyList();
            }
            final UsbSerialDriver driver = new CdcAcmSerialDriver(usbDevice);
            return Collections.singletonList(driver);
        }
    },

    SILAB_SERIAL {
        @Override
        public List<UsbSerialDriver> probe(final UsbManager manager, final UsbDevice usbDevice) {
            if (!testIfSupported(usbDevice, Cp2102SerialDriver.getSupportedDevices())) {
                return Collections.emptyList();
            }
            final UsbSerialDriver driver = new Cp2102SerialDriver(usbDevice);
            return Collections.singletonList(driver);
        }
    },

    PROLIFIC_SERIAL {
        @Override
        public List<UsbSerialDriver> probe(final UsbManager manager, final UsbDevice usbDevice) {
            if (!testIfSupported(usbDevice, ProlificSerialDriver.getSupportedDevices())) {
                return Collections.emptyList();
            }
            final UsbSerialDriver driver = new ProlificSerialDriver(usbDevice);
            return Collections.singletonList(driver);
        }
    };

    /**
     * Tests the supplied {@link UsbDevice} for compatibility with this enum
     * member, returning one or more driver instances if compatible.
     *
     * @param manager the {@link UsbManager} to use
     * @param usbDevice the raw {@link UsbDevice} to use
     * @return zero or more {@link UsbSerialDriver}, depending on compatibility
     *         (never {@code null}).
     */
    protected abstract List<UsbSerialDriver> probe(final UsbManager manager, final UsbDevice usbDevice);

    /**
     * Creates and returns a new {@link UsbSerialDriver} instance for the first
     * compatible {@link UsbDevice} found on the bus.  If none are found,
     * returns {@code null}.
     *
     * <p/>
     * The order of devices is undefined, therefore if there are multiple
     * devices on the bus, the chosen device may not be predictable (clients
     * should use {@link #findAllDevices(UsbManager)} instead).
     *
     * @param usbManager the {@link UsbManager} to use.
     * @return the first available {@link UsbSerialDriver}, or {@code null} if
     *         none are available.
     */
    public static UsbSerialDriver findFirstDevice(final UsbManager usbManager) {
        for (final UsbDevice usbDevice : usbManager.getDeviceList().values()) {
            for (final UsbSerialProber prober : values()) {
                final List<UsbSerialDriver> probedDevices = prober.probe(usbManager, usbDevice);
                if (!probedDevices.isEmpty()) {
                    return probedDevices.get(0);
                }
            }
        }
        return null;
    }

    /**
     * Creates a new {@link UsbSerialDriver} instance for all compatible
     * {@link UsbDevice}s found on the bus. If no compatible devices are found,
     * the list will be empty.
     *
     * @param usbManager
     * @return
     */
    public static List<UsbSerialDriver> findAllDevices(final UsbManager usbManager) {
        final List<UsbSerialDriver> result = new ArrayList<UsbSerialDriver>();
        //Log.i("QGC_UsbSerialProber", "Looking for USB devices");
        // For each UsbDevice, call probe() for each prober.
        for (final UsbDevice usbDevice : usbManager.getDeviceList().values()) {
            //Log.i("QGC_UsbSerialProber", "Probing device: " + usbDevice.getDeviceName() + " mid: " + usbDevice.getVendorId() + " pid: " + usbDevice.getDeviceId());
            result.addAll(probeSingleDevice(usbManager, usbDevice));
        }
        return result;
    }

    /**
     * Special method for testing a specific device for driver support,
     * returning any compatible driver(s).
     *
     * <p/>
     * Clients should ordinarily use {@link #findAllDevices(UsbManager)}, which
     * operates against the entire bus of devices. This method is useful when
     * testing against only a single target is desired.
     *
     * @param usbManager the {@link UsbManager} to use.
     * @param usbDevice the device to test against.
     * @return a list containing zero or more {@link UsbSerialDriver} instances.
     */
    public static List<UsbSerialDriver> probeSingleDevice(final UsbManager usbManager,
            UsbDevice usbDevice)
    {
        final List<UsbSerialDriver> result = new ArrayList<UsbSerialDriver>();
        for (final UsbSerialProber prober : values()) {
            final List<UsbSerialDriver> probedDevices = prober.probe(usbManager, usbDevice);
            result.addAll(probedDevices);
        }
        return result;
    }

    /**
     * Deprecated; Use {@link #findFirstDevice(UsbManager)}.
     *
     * @param usbManager
     * @return
     */
    @Deprecated
    public static UsbSerialDriver acquire(final UsbManager usbManager) {
        return findFirstDevice(usbManager);
    }

    /**
     * Deprecated; use {@link #probeSingleDevice(UsbManager, UsbDevice)}.
     *
     * @param usbManager
     * @param usbDevice
     * @return
     */
    @Deprecated
    public static UsbSerialDriver acquire(final UsbManager usbManager, final UsbDevice usbDevice) {
        final List<UsbSerialDriver> probedDevices = probeSingleDevice(usbManager, usbDevice);
        if (!probedDevices.isEmpty()) {
            return probedDevices.get(0);
        }
        return null;
    }

    /**
     * Returns {@code true} if the given device is found in the driver's
     * vendor/product map.
     *
     * @param usbDevice the device to test
     * @param supportedDevices map of vendor IDs to product ID(s)
     * @return {@code true} if supported
     */
    private static boolean testIfSupported(final UsbDevice usbDevice, final Map<Integer, int[]> supportedDevices) {
        return supportedDevices.containsKey(usbDevice.getVendorId());
    }
}
        # Provide a unique ID to access the sarif output path
        id: /*
 * Copyright 2011 Google Inc.
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
 */

package com.hoho.android.usbserial.driver;

/**
 * Generic unchecked exception for the usbserial package.
 *
 * @author mike wakerly (opensource@hoho.com)
 */
@SuppressWarnings("serial")
public class UsbSerialRuntimeException extends RuntimeException {

    public UsbSerialRuntimeException() {
        super();
    }

    public UsbSerialRuntimeException(String detailMessage, Throwable throwable) {
        super(detailMessage, throwable);
    }

    public UsbSerialRuntimeException(String detailMessage) {
        super(detailMessage);
    }

    public UsbSerialRuntimeException(Throwable throwable) {
        super(throwable);
    }

}
        with:  * This library is free software; you can redistribute it and/or
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
          cmakeBuildDirectory: ${{  * This library is free software; you can redistribute it and/or
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
} }}
          # Ruleset file that will determine what checks will be run
          ruleset:  * This library is free software; you can redistribute it and/or
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

      # Upload SARIF file to GitHub Code Scanning Alerts
      - name: package org.mavlink.qgroundcontrol;

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

import com.hoho.android.usbserial.driver.*;
import org.qtproject.qt5.android.bindings.QtActivity;
import org.qtproject.qt5.android.bindings.QtApplication;

public class QGCActivity extends QtActivity
{
    public  static int                                  BAD_DEVICE_ID = 0;
    private static QGCActivity                          _instance = null;
    private static UsbManager                           _usbManager = null;
    private static List<UsbSerialDriver>                _drivers;
    private static HashMap<Integer, UsbIoManager>       m_ioManager;
    private static HashMap<Integer, Long>               _userDataHashByDeviceId;
    private static final String                         TAG = "QGC_QGCActivity";
    private static PowerManager.WakeLock                _wakeLock;
    private static final String                         ACTION_USB_PERMISSION = "org.mavlink.qgroundcontrol.action.USB_PERMISSION";
    private static PendingIntent                        _usbPermissionIntent = null;
    private TaiSync                                     taiSync = null;
    private Timer                                       probeAccessoriesTimer = null;

    public static Context m_context;

    private final static ExecutorService m_Executor = Executors.newSingleThreadExecutor();

    private final static UsbIoManager.Listener m_Listener =
            new UsbIoManager.Listener()
            {
                @Override
                public void onRunError(Exception eA, long userData)
                {
                    Log.e(TAG, "onRunError Exception");
                    nativeDeviceException(userData, eA.getMessage());
                }

                @Override
                public void onNewData(final byte[] dataA, long userData)
                {
                    nativeDeviceNewData(userData, dataA);
                }
            };

    private final BroadcastReceiver mOpenAccessoryReceiver =
        new BroadcastReceiver()
        {
            @Override
            public void onReceive(Context context, Intent intent) {
                String action = intent.getAction();
                if (ACTION_USB_PERMISSION.equals(action)) {
                    UsbAccessory accessory = intent.getParcelableExtra(UsbManager.EXTRA_ACCESSORY);
                    if (accessory != null && intent.getBooleanExtra(UsbManager.EXTRA_PERMISSION_GRANTED, false)) {
                        openAccessory(accessory);
                    }
                } else if (UsbManager.ACTION_USB_ACCESSORY_DETACHED.equals(action)) {
                    UsbAccessory accessory = intent.getParcelableExtra(UsbManager.EXTRA_ACCESSORY);
                    if (accessory != null) {
                        closeAccessory(accessory);
                    }
                }
            }
        };

    private static UsbSerialDriver _findDriverByDeviceId(int deviceId) {
        for (UsbSerialDriver driver: _drivers) {
            if (driver.getDevice().getDeviceId() == deviceId) {
                return driver;
            }
        }
        return null;
    }

    private static UsbSerialDriver _findDriverByDeviceName(String deviceName) {
        for (UsbSerialDriver driver: _drivers) {
            if (driver.getDevice().getDeviceName().equals(deviceName)) {
                return driver;
            }
        }
        return null;
    }

    private final static BroadcastReceiver _usbReceiver = new BroadcastReceiver() {
            public void onReceive(Context context, Intent intent) {
                String action = intent.getAction();
                Log.i(TAG, "BroadcastReceiver USB action " + action);

                if (ACTION_USB_PERMISSION.equals(action)) {
                    synchronized (_instance) {
                        UsbDevice device = (UsbDevice)intent.getParcelableExtra(UsbManager.EXTRA_DEVICE);
                        if (device != null) {
                            UsbSerialDriver driver = _findDriverByDeviceId(device.getDeviceId());

                            if (intent.getBooleanExtra(UsbManager.EXTRA_PERMISSION_GRANTED, false)) {
                                qgcLogDebug("Permission granted to " + device.getDeviceName());
                                driver.setPermissionStatus(UsbSerialDriver.permissionStatusSuccess);
                            } else {
                                qgcLogDebug("Permission denied for " + device.getDeviceName());
                                driver.setPermissionStatus(UsbSerialDriver.permissionStatusDenied);
                            }
                        }
                    }
                } else if (UsbManager.ACTION_USB_DEVICE_DETACHED.equals(action)) {
                    UsbDevice device = (UsbDevice)intent.getParcelableExtra(UsbManager.EXTRA_DEVICE);
                    if (device != null) {
                        if (_userDataHashByDeviceId.containsKey(device.getDeviceId())) {
                            nativeDeviceHasDisconnected(_userDataHashByDeviceId.get(device.getDeviceId()));
                        }
                    }
                }

                try {
                    nativeUpdateAvailableJoysticks();
                } catch(Exception e) {
                    Log.e(TAG, "Exception nativeUpdateAvailableJoysticks()");
                }
            }
        };

    // Native C++ functions which connect back to QSerialPort code
    private static native void nativeDeviceHasDisconnected(long userData);
    private static native void nativeDeviceException(long userData, String messageA);
    private static native void nativeDeviceNewData(long userData, byte[] dataA);
    private static native void nativeUpdateAvailableJoysticks();

    // Native C++ functions called to log output
    public static native void qgcLogDebug(String message);
    public static native void qgcLogWarning(String message);

    public native void nativeInit();

    // QGCActivity singleton
    public QGCActivity()
    {
        _instance =                 this;
        _drivers =                  new ArrayList<UsbSerialDriver>();
        _userDataHashByDeviceId =   new HashMap<Integer, Long>();
        m_ioManager =               new HashMap<Integer, UsbIoManager>();
    }

    @Override
    public void onCreate(Bundle savedInstanceState)
    {
        super.onCreate(savedInstanceState);
        nativeInit();
        PowerManager pm = (PowerManager)_instance.getSystemService(Context.POWER_SERVICE);
        _wakeLock = pm.newWakeLock(PowerManager.SCREEN_BRIGHT_WAKE_LOCK, "QGroundControl");
        if(_wakeLock != null) {
            _wakeLock.acquire();
        } else {
            Log.i(TAG, "SCREEN_BRIGHT_WAKE_LOCK not acquired!!!");
        }
        _instance.getWindow().addFlags(WindowManager.LayoutParams.FLAG_KEEP_SCREEN_ON);

        _usbManager = (UsbManager)_instance.getSystemService(Context.USB_SERVICE);

        // Register for USB Detach and USB Permission intent
        IntentFilter filter = new IntentFilter();
        filter.addAction(UsbManager.ACTION_USB_DEVICE_ATTACHED);
        filter.addAction(UsbManager.ACTION_USB_DEVICE_DETACHED);
        filter.addAction(ACTION_USB_PERMISSION);
        filter.addAction(BluetoothDevice.ACTION_ACL_CONNECTED);
        filter.addAction(BluetoothDevice.ACTION_ACL_DISCONNECTED);
        _instance.registerReceiver(_instance._usbReceiver, filter);

        // Create intent for usb permission request
        _usbPermissionIntent = PendingIntent.getBroadcast(_instance, 0, new Intent(ACTION_USB_PERMISSION), 0);

        try {
            taiSync = new TaiSync();

            IntentFilter accessoryFilter = new IntentFilter(ACTION_USB_PERMISSION);
            filter.addAction(UsbManager.ACTION_USB_ACCESSORY_DETACHED);
            registerReceiver(mOpenAccessoryReceiver, accessoryFilter);

            probeAccessoriesTimer = new Timer();
            probeAccessoriesTimer.schedule(new TimerTask() {
                @Override
                public void run()
                {
                    probeAccessories();
                }
            }, 0, 3000);
        } catch(Exception e) {
           Log.e(TAG, "Exception: " + e);
        }
    }

    @Override
    public void onResume() {
        super.onResume();

        // Plug in of USB ACCESSORY triggers only onResume event.
        // Then we scan if there is actually anything new
        probeAccessories();
    }

    @Override
    protected void onDestroy()
    {
        if (probeAccessoriesTimer != null) {
            probeAccessoriesTimer.cancel();
        }
        unregisterReceiver(mOpenAccessoryReceiver);
        try {
            if(_wakeLock != null) {
                _wakeLock.release();
            }
        } catch(Exception e) {
           Log.e(TAG, "Exception onDestroy()");
        }
        super.onDestroy();
    }

    public void onInit(int status) {
    }

    /// Incrementally updates the list of drivers connected to the device
    private static void updateCurrentDrivers()
    {
        List<UsbSerialDriver> currentDrivers = UsbSerialProber.findAllDevices(_usbManager);

        // Remove stale drivers
        for (int i=_drivers.size()-1; i>=0; i--) {
            boolean found = false;
            for (UsbSerialDriver currentDriver: currentDrivers) {
                if (_drivers.get(i).getDevice().getDeviceId() == currentDriver.getDevice().getDeviceId()) {
                    found = true;
                    break;
                }
            }

            if (!found) {
                qgcLogDebug("Remove stale driver " + _drivers.get(i).getDevice().getDeviceName());
                _drivers.remove(i);
            }
        }

        // Add new drivers
        for (int i=0; i<currentDrivers.size(); i++) {
            boolean found = false;
            for (int j=0; j<_drivers.size(); j++) {
                if (currentDrivers.get(i).getDevice().getDeviceId() == _drivers.get(j).getDevice().getDeviceId()) {
                    found = true;
                    break;
                }
            }

            if (!found) {
                UsbSerialDriver newDriver =     currentDrivers.get(i);
                UsbDevice       device =        newDriver.getDevice();
                String          deviceName =    device.getDeviceName();

                _drivers.add(newDriver);
                qgcLogDebug("Adding new driver " + deviceName);

                // Request permission if needed
                if (_usbManager.hasPermission(device)) {
                    qgcLogDebug("Already have permission to use device " + deviceName);
                    newDriver.setPermissionStatus(UsbSerialDriver.permissionStatusSuccess);
                } else {
                    qgcLogDebug("Requesting permission to use device " + deviceName);
                    newDriver.setPermissionStatus(UsbSerialDriver.permissionStatusRequested);
                    _usbManager.requestPermission(device, _usbPermissionIntent);
                }
            }
        }
    }

    /// Returns array of device info for each unopened device.
    /// @return Device info format DeviceName:Company:ProductId:VendorId
    public static String[] availableDevicesInfo()
    {
        updateCurrentDrivers();

        if (_drivers.size() <= 0) {
            return null;
        }

        List<String> deviceInfoList = new ArrayList<String>();

        for (int i=0; i<_drivers.size(); i++) {
            String          deviceInfo;
            UsbSerialDriver driver = _drivers.get(i);

            if (driver.permissionStatus() != UsbSerialDriver.permissionStatusSuccess) {
                continue;
            }

            UsbDevice device = driver.getDevice();

            deviceInfo = device.getDeviceName() + ":";

            if (driver instanceof FtdiSerialDriver) {
                deviceInfo = deviceInfo + "FTDI:";
            } else if (driver instanceof CdcAcmSerialDriver) {
                deviceInfo = deviceInfo + "Cdc Acm:";
            } else if (driver instanceof Cp2102SerialDriver) {
                deviceInfo = deviceInfo + "Cp2102:";
            } else if (driver instanceof ProlificSerialDriver) {
                deviceInfo = deviceInfo + "Prolific:";
            } else {
                deviceInfo = deviceInfo + "Unknown:";
            }

            deviceInfo = deviceInfo + Integer.toString(device.getProductId()) + ":";
            deviceInfo = deviceInfo + Integer.toString(device.getVendorId()) + ":";

            deviceInfoList.add(deviceInfo);
        }

        String[] rgDeviceInfo = new String[deviceInfoList.size()];
        for (int i=0; i<deviceInfoList.size(); i++) {
            rgDeviceInfo[i] = deviceInfoList.get(i);
        }

        return rgDeviceInfo;
    }

    /// Open the specified device
    ///     @param userData Data to associate with device and pass back through to native calls.
    /// @return Device id
    public static int open(Context parentContext, String deviceName, long userData)
    {
        int deviceId = BAD_DEVICE_ID;

        m_context = parentContext;

        UsbSerialDriver driver = _findDriverByDeviceName(deviceName);
        if (driver == null) {
            qgcLogWarning("Attempt to open unknown device " + deviceName);
            return BAD_DEVICE_ID;
        }

        if (driver.permissionStatus() != UsbSerialDriver.permissionStatusSuccess) {
            qgcLogWarning("Attempt to open device with incorrect permission status " + deviceName + " " + driver.permissionStatus());
            return BAD_DEVICE_ID;
        }

        UsbDevice device = driver.getDevice();
        deviceId = device.getDeviceId();

        try {
            driver.setConnection(_usbManager.openDevice(device));
            driver.open();
            driver.setPermissionStatus(UsbSerialDriver.permissionStatusOpen);

            _userDataHashByDeviceId.put(deviceId, userData);

            UsbIoManager ioManager = new UsbIoManager(driver, m_Listener, userData);
            m_ioManager.put(deviceId, ioManager);
            m_Executor.submit(ioManager);

            qgcLogDebug("Port open successful");
        } catch(IOException exA) {
            driver.setPermissionStatus(UsbSerialDriver.permissionStatusRequestRequired);
            _userDataHashByDeviceId.remove(deviceId);

            if(m_ioManager.get(deviceId) != null) {
                m_ioManager.get(deviceId).stop();
                m_ioManager.remove(deviceId);
            }
            qgcLogWarning("Port open exception: " + exA.getMessage());
            return BAD_DEVICE_ID;
        }

        return deviceId;
    }

    public static void startIoManager(int idA)
    {
        if (m_ioManager.get(idA) != null)
            return;

        UsbSerialDriver driverL = _findDriverByDeviceId(idA);

        if (driverL == null)
            return;

        UsbIoManager managerL = new UsbIoManager(driverL, m_Listener, _userDataHashByDeviceId.get(idA));
        m_ioManager.put(idA, managerL);
        m_Executor.submit(managerL);
    }

    public static void stopIoManager(int idA)
    {
        if(m_ioManager.get(idA) == null)
            return;

        m_ioManager.get(idA).stop();
        m_ioManager.remove(idA);
    }

    ///////////////////////////////////////////////////////////////////////////////////////////////////////
    //
    //  Sets the parameters on an open port.
    //
    //  Args:   idA - ID number from the open command
    //          baudRateA - Decimal value of the baud rate.  I.E. 9600, 57600, 115200, etc.
    //          dataBitsA - number of data bits.  Valid numbers are 5, 6, 7, 8
    //          stopBitsA - number of stop bits.  Valid numbers are 1, 2
    //          parityA - No Parity=0, Odd Parity=1, Even Parity=2
    //
    //  Returns:  T/F Success/Failure
    //
    ////////////////////////////////////////////////////////////////////////////////////////////////////////
    public static boolean setParameters(int idA, int baudRateA, int dataBitsA, int stopBitsA, int parityA)
    {
        UsbSerialDriver driverL = _findDriverByDeviceId(idA);

        if (driverL == null)
            return false;

        try
        {
            driverL.setParameters(baudRateA, dataBitsA, stopBitsA, parityA);
            return true;
        }
        catch(IOException eA)
        {
            return false;
        }
    }



    ////////////////////////////////////////////////////////////////////////////////////////////////////////
    //
    //  Close the device.
    //
    //  Args:  idA - ID number from the open command
    //
    //  Returns:  T/F Success/Failure
    //
    ////////////////////////////////////////////////////////////////////////////////////////////////////////
    public static boolean close(int idA)
    {
        UsbSerialDriver driverL = _findDriverByDeviceId(idA);

        if (driverL == null)
            return false;

        try
        {
            stopIoManager(idA);
            _userDataHashByDeviceId.remove(idA);
            driverL.setPermissionStatus(UsbSerialDriver.permissionStatusRequestRequired);
            driverL.close();

            return true;
        }
        catch(IOException eA)
        {
            return false;
        }
    }



    //////////////////////////////////////////////////////////////////////////////////////////////////////
    //
    //  Write data to the device.
    //
    //  Args:   idA - ID number from the open command
    //          sourceA - byte array of data to write
    //          timeoutMsecA - amount of time in milliseconds to wait for the write to occur
    //
    //  Returns:  number of bytes written
    //
    /////////////////////////////////////////////////////////////////////////////////////////////////////
    public static int write(int idA, byte[] sourceA, int timeoutMSecA)
    {
        UsbSerialDriver driverL = _findDriverByDeviceId(idA);

        if (driverL == null)
            return 0;

        try
        {
            return driverL.write(sourceA, timeoutMSecA);
        }
        catch(IOException eA)
        {
            return 0;
        }
        /*
        UsbIoManager managerL = m_ioManager.get(idA);

        if(managerL != null)
        {
            managerL.writeAsync(sourceA);
            return sourceA.length;
        }
        else
            return 0;
        */
    }

    public static boolean isDeviceNameValid(String nameA)
    {
        for (UsbSerialDriver driver: _drivers) {
            if (driver.getDevice().getDeviceName() == nameA)
                return true;
        }

        return false;
    }

    public static boolean isDeviceNameOpen(String nameA)
    {
        for (UsbSerialDriver driverL: _drivers) {
            if (nameA.equals(driverL.getDevice().getDeviceName()) && driverL.permissionStatus() == UsbSerialDriver.permissionStatusOpen) {
                return true;
            }
        }

        return false;
    }



    /////////////////////////////////////////////////////////////////////////////////////////////////////
    //
    //  Set the Data Terminal Ready flag on the device
    //
    //  Args:   idA - ID number from the open command
    //          onA - on=T, off=F
    //
    //  Returns:  T/F Success/Failure
    //
    ////////////////////////////////////////////////////////////////////////////////////////////////////
    public static boolean setDataTerminalReady(int idA, boolean onA)
    {
        try
        {
            UsbSerialDriver driverL = _findDriverByDeviceId(idA);

            if (driverL == null)
                return false;

            driverL.setDTR(onA);
            return true;
        }
        catch(IOException eA)
        {
            return false;
        }
    }



    ////////////////////////////////////////////////////////////////////////////////////////////
    //
    //  Set the Request to Send flag
    //
    //  Args:   idA - ID number from the open command
    //          onA - on=T, off=F
    //
    //  Returns:  T/F Success/Failure
    //
    ////////////////////////////////////////////////////////////////////////////////////////////
    public static boolean setRequestToSend(int idA, boolean onA)
    {
        try
        {
            UsbSerialDriver driverL = _findDriverByDeviceId(idA);

            if (driverL == null)
                return false;

            driverL.setRTS(onA);
            return true;
        }
        catch(IOException eA)
        {
            return false;
        }
    }



    ///////////////////////////////////////////////////////////////////////////////////////////////
    //
    //  Purge the hardware buffers based on the input and output flags
    //
    //  Args:   idA - ID number from the open command
    //          inputA - input buffer purge.  purge=T
    //          outputA - output buffer purge.  purge=T
    //
    //  Returns:  T/F Success/Failure
    //
    ///////////////////////////////////////////////////////////////////////////////////////////////
    public static boolean purgeBuffers(int idA, boolean inputA, boolean outputA)
    {
        try
        {
            UsbSerialDriver driverL = _findDriverByDeviceId(idA);

            if (driverL == null)
                return false;

            return driverL.purgeHwBuffers(inputA, outputA);
        }
        catch(IOException eA)
        {
            return false;
        }
    }



    //////////////////////////////////////////////////////////////////////////////////////////
    //
    //  Get the native device handle (file descriptor)
    //
    //  Args:   idA - ID number from the open command
    //
    //  Returns:  device handle
    //
    ///////////////////////////////////////////////////////////////////////////////////////////
    public static int getDeviceHandle(int idA)
    {
        UsbSerialDriver driverL = _findDriverByDeviceId(idA);

        if (driverL == null)
            return -1;

        UsbDeviceConnection connectL = driverL.getDeviceConnection();
        if (connectL == null)
            return -1;
        else
            return connectL.getFileDescriptor();
    }

    UsbAccessory openUsbAccessory = null;
    Object openAccessoryLock = new Object();

    private void openAccessory(UsbAccessory usbAccessory)
    {
        Log.i(TAG, "openAccessory: " + usbAccessory.getSerial());
        try {
            synchronized(openAccessoryLock) {
                if ((openUsbAccessory != null && !taiSync.isRunning()) || openUsbAccessory == null) {
                    openUsbAccessory = usbAccessory;
                    taiSync.open(_usbManager.openAccessory(usbAccessory));
                }
            }
        } catch (IOException e) {
            Log.e(TAG, "openAccessory exception: " + e);
            taiSync.close();
            closeAccessory(openUsbAccessory);
        }
    }

    private void closeAccessory(UsbAccessory usbAccessory)
    {
        Log.i(TAG, "closeAccessory");

        synchronized(openAccessoryLock) {
            if (openUsbAccessory != null && usbAccessory == openUsbAccessory && taiSync.isRunning()) {
                taiSync.close();
                openUsbAccessory = null;
            }
        }
    }

    Object probeAccessoriesLock = new Object();

    private void probeAccessories()
    {
        final PendingIntent pendingIntent = PendingIntent.getBroadcast(this, 0, new Intent(ACTION_USB_PERMISSION), 0);
        new Thread(new Runnable() {
            public void run() {
                synchronized(openAccessoryLock) {
//                    Log.i(TAG, "probeAccessories");
                    UsbAccessory[] accessories = _usbManager.getAccessoryList();
                    if (accessories != null) {
                       for (UsbAccessory usbAccessory : accessories) {
                           if (usbAccessory == null) {
                               continue;
                           }
                           if (_usbManager.hasPermission(usbAccessory)) {
                               openAccessory(usbAccessory);
                           }
                       }
                    }
                }
            }
        }).start();
    }
}
        uses: package org.mavlink.qgroundcontrol;

import java.util.concurrent.ExecutorService;
import java.util.concurrent.Executors;

import java.io.InputStream;
import java.io.OutputStream;
import java.io.FileDescriptor;
import java.io.FileInputStream;
import java.io.FileOutputStream;
import java.io.IOException;

import java.net.DatagramPacket;
import java.net.DatagramSocket;
import java.net.Socket;
import java.net.InetAddress;

import android.os.ParcelFileDescriptor;
import android.util.Log;

public class TaiSync
{
    private static final int HEADER_SIZE = 0x1C;

    private static final byte PROTOCOL_REQUEST_CONNECTION = 0x00;
    private static final byte PROTOCOL_VERSION = 0x01;
    private static final byte PROTOCOL_CHANNEL = 0x02;
    private static final byte PROTOCOL_DATA = 0x03;

    private static final int VIDEO_PORT = 5600;
    private static final int TAISYNC_VIDEO_PORT = 8000;
    private static final int TAISYNC_SETTINGS_PORT = 8200;
    private static final int TAISYNC_TELEMETRY_PORT = 8400;

    private Object runLock;
    private boolean running = false;
    private DatagramSocket udpSocket = null;
    private Socket tcpSettingsSocket = null;
    private InputStream settingsInStream = null;
    private OutputStream settingsOutStream = null;
    private Socket tcpTelemetrySocket = null;
    private InputStream telemetryInStream = null;
    private OutputStream telemetryOutStream = null;
    private ParcelFileDescriptor mParcelFileDescriptor;
    private FileInputStream mFileInputStream;
    private FileOutputStream mFileOutputStream;
    private ExecutorService mThreadPool;
    private byte[] mBytes = new byte[32 * 1024];
    private byte vMaj = 0;

    public TaiSync()
    {
        runLock = new Object();
        mThreadPool = Executors.newFixedThreadPool(3);
    }

    public boolean isRunning()
    {
        synchronized (runLock) {
            return running;
        }
    }

    public void open(ParcelFileDescriptor parcelFileDescriptor) throws IOException
    {
//        Log.i("QGC_TaiSync", "Open");

        synchronized (runLock) {
            if (running) {
                return;
            }
            running = true;
        }

        mParcelFileDescriptor = parcelFileDescriptor;
        if (mParcelFileDescriptor == null) {
            throw new IOException("parcelFileDescriptor is null");
        }

        FileDescriptor fileDescriptor = mParcelFileDescriptor.getFileDescriptor();
        mFileInputStream = new FileInputStream(fileDescriptor);
        mFileOutputStream = new FileOutputStream(fileDescriptor);

        udpSocket = new DatagramSocket();
        final InetAddress address = InetAddress.getByName("localhost");
        tcpTelemetrySocket = new Socket(address, TAISYNC_TELEMETRY_PORT);
        telemetryInStream = tcpTelemetrySocket.getInputStream();
        telemetryOutStream = tcpTelemetrySocket.getOutputStream();
        tcpSettingsSocket = new Socket(address, TAISYNC_SETTINGS_PORT);
        settingsInStream = tcpSettingsSocket.getInputStream();
        settingsOutStream = tcpSettingsSocket.getOutputStream();

        // Request connection packet
        sendTaiSyncMessage(PROTOCOL_REQUEST_CONNECTION, 0, null, 0);

        // Read multiplexed data stream coming from TaiSync accessory
        mThreadPool.execute(new Runnable() {
            @Override
            public void run()
            {
                int bytesRead = 0;

                try {
                    while (bytesRead >= 0) {
                        synchronized (runLock) {
                            if (!running) {
                                break;
                            }
                        }

                        bytesRead = mFileInputStream.read(mBytes);

                        if (bytesRead > 0)
                        {
                            if (mBytes[3] == PROTOCOL_VERSION)
                            {
                                vMaj = mBytes[19];
                                // Log.i("QGC_TaiSync", "Got protocol version message vMaj = " + mBytes[19]);
                                sendTaiSyncMessage(PROTOCOL_VERSION, 0, null, 0);
                            }
                            else if (mBytes[3] == PROTOCOL_CHANNEL) {
                                int dPort = ((mBytes[4] & 0xff)<< 24) | ((mBytes[5]&0xff) << 16) | ((mBytes[6]&0xff) << 8) | (mBytes[7] &0xff);
                                int dLength = ((mBytes[8] & 0xff)<< 24) | ((mBytes[9]&0xff) << 16) | ((mBytes[10]&0xff) << 8) | (mBytes[11] &0xff);
                                // Log.i("QGC_TaiSync", "Read 2 port = " + dPort + " length = " + dLength);
                                sendTaiSyncMessage(PROTOCOL_CHANNEL, dPort, null, 0);
                            }
                            else if (mBytes[3] == PROTOCOL_DATA) {
                                int dPort = ((mBytes[4] & 0xff)<< 24) | ((mBytes[5]&0xff) << 16) | ((mBytes[6]&0xff) << 8) | (mBytes[7] &0xff);
                                int dLength = ((mBytes[8] & 0xff)<< 24) | ((mBytes[9]&0xff) << 16) | ((mBytes[10]&0xff) << 8) | (mBytes[11] &0xff);

                                int payloadOffset = HEADER_SIZE;
                                int payloadLength = bytesRead - payloadOffset;

                                byte[] sBytes = new byte[payloadLength];
                                System.arraycopy(mBytes, payloadOffset, sBytes, 0, payloadLength);

                                if (dPort == TAISYNC_VIDEO_PORT) {
                                    DatagramPacket packet = new DatagramPacket(sBytes, sBytes.length, address, VIDEO_PORT);
                                    udpSocket.send(packet);
                                } else if (dPort == TAISYNC_SETTINGS_PORT) {
                                    settingsOutStream.write(sBytes);
                                } else if (dPort == TAISYNC_TELEMETRY_PORT) {
                                    telemetryOutStream.write(sBytes);
                                }
                            }
                        }
                    }
                } catch (IOException e) {
                    Log.e("QGC_TaiSync", "Exception: " + e);
                    e.printStackTrace();
                } finally {
                    close();
                }
            }
        });

        // Read command & control packets to be sent to telemetry port
        mThreadPool.execute(new Runnable() {
            @Override
            public void run()
            {
                byte[] inbuf = new byte[256];

                try {
                    while (true) {
                        synchronized (runLock) {
                            if (!running) {
                                break;
                            }
                        }

                        int bytesRead = telemetryInStream.read(inbuf, 0, inbuf.length);
                        if (bytesRead > 0) {
                            sendTaiSyncMessage(PROTOCOL_DATA, TAISYNC_TELEMETRY_PORT, inbuf, bytesRead);
                        }
                    }
                } catch (IOException e) {
                    Log.e("QGC_TaiSync", "Exception: " + e);
                    e.printStackTrace();
                } finally {
                    close();
                }
            }
        });

        // Read incoming requests for settings socket
        mThreadPool.execute(new Runnable() {
            @Override
            public void run()
            {
                byte[] inbuf = new byte[1024];

                try {
                    while (true) {
                        synchronized (runLock) {
                            if (!running) {
                                break;
                            }
                        }

                        int bytesRead = settingsInStream.read(inbuf, 0, inbuf.length);
                        if (bytesRead > 0) {
                            sendTaiSyncMessage(PROTOCOL_DATA, TAISYNC_SETTINGS_PORT, inbuf, bytesRead);
                        }
                    }
                } catch (IOException e) {
                    Log.e("QGC_TaiSync", "Exception: " + e);
                    e.printStackTrace();
                } finally {
                    close();
                }
            }
        });
   }

    private void sendTaiSyncMessage(byte protocol, int dataPort, byte[] data, int dataLen) throws IOException
    {
        byte portMSB = (byte)((dataPort >> 8) & 0xFF);
        byte portLSB = (byte)(dataPort & 0xFF);

        byte[] lA = new byte[4];
        int len = HEADER_SIZE + dataLen;
        // Log.i("QGC_TaiSync", "Sending to " + dataPort + " length = " + len);
        byte[] buffer = new byte[len];

        for (int i = 3; i >= 0; i--) {
            lA[i] = (byte)(len & 0xFF);
            len >>= 8;
        }

        byte[] header = { 0x00, 0x00, 0x00, protocol,   // uint32 - protocol
                          0x00, 0x00, portMSB, portLSB, // uint32 - dport
                          lA[0], lA[1], lA[2], lA[3],   // uint32 - length
                          0x00, 0x00, 0x00, 0x00,       // uint32 - magic
                          0x00, 0x00, 0x00, vMaj,       // uint32 - version major
                          0x00, 0x00, 0x00, 0x00,       // uint32 - version minor
                          0x00, 0x00, 0x00, 0x00 };     // uint32 - padding

        System.arraycopy(header, 0, buffer, 0, header.length);
        if (data != null && dataLen > 0) {
            System.arraycopy(data, 0, buffer, header.length, dataLen);
        }

        synchronized (runLock) {
            mFileOutputStream.write(buffer);
        }
    }

    public void close()
    {
//        Log.i("QGC_TaiSync", "Close");
        synchronized (runLock) {
            running = false;
        }
        try {
            if (udpSocket != null) {
                udpSocket.close();
            }
        } catch (Exception e) {
        }
        try {
            if (tcpTelemetrySocket != null) {
                tcpTelemetrySocket.close();
            }
        } catch (Exception e) {
        }
        try {
            if (tcpSettingsSocket != null) {
                tcpSettingsSocket.close();
            }
        } catch (Exception e) {
        }
        try {
            if (mParcelFileDescriptor != null) {
                mParcelFileDescriptor.close();
            }
        } catch (Exception e) {
        }
        udpSocket = null;
        tcpSettingsSocket = null;
        tcpTelemetrySocket = null;
        settingsInStream = null;
        settingsOutStream = null;
        mParcelFileDescriptor = null;
    }
}
        with: package org.mavlink.qgroundcontrol;

/* Copyright 2011 Google Inc.
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


import com.hoho.android.usbserial.driver.*;

import java.io.IOException;
import java.nio.ByteBuffer;
import android.util.Log;

/**
* Utility class which services a {@link UsbSerialDriver} in its {@link #run()}
* method.
*
* Original author mike wakerly (opensource@hoho.com)
* Modified by Mike Goza
*/
public class UsbIoManager implements Runnable {
    private static final int READ_WAIT_MILLIS = 100;
    private static final int BUFSIZ = 4096;
    private static final String TAG = "QGC_UsbIoManager";

    private final UsbSerialDriver mDriver;
    private long mUserData;
    private final ByteBuffer mReadBuffer = ByteBuffer.allocate(BUFSIZ);
    private final ByteBuffer mWriteBuffer = ByteBuffer.allocate(BUFSIZ);

    private enum State
    {
        STOPPED,
        RUNNING,
        STOPPING
    }

    // Synchronized by 'this'
    private State mState = State.STOPPED;

    // Synchronized by 'this'
    private Listener mListener;

    public interface Listener
    {
        /**
         * Called when new incoming data is available.
         */
        public void onNewData(byte[] data, long userData);

        /**
         * Called when {@link SerialInputOutputManager#run()} aborts due to an
         * error.
         */
        public void onRunError(Exception e, long userData);
    }



    /**
     * Creates a new instance with no listener.
     */
    public UsbIoManager(UsbSerialDriver driver)
    {
        this(driver, null, 0);
        Log.i(TAG, "Instance created");
    }



    /**
     * Creates a new instance with the provided listener.
     */
    public UsbIoManager(UsbSerialDriver driver, Listener listener, long userData)
    {
        mDriver = driver;
        mListener = listener;
        mUserData = userData;
    }



    public synchronized void setListener(Listener listener)
    {
        mListener = listener;
    }



    public synchronized Listener getListener()
    {
        return mListener;
    }



    public void writeAsync(byte[] data)
    {
        synchronized (mWriteBuffer)
        {
            mWriteBuffer.put(data);
        }
    }



    public synchronized void stop()
    {
        if (getState() == State.RUNNING)
        {
            mState = State.STOPPING;
            mUserData = 0;
        }
    }



    private synchronized State getState()
    {
        return mState;
    }



    /**
     * Continuously services the read and write buffers until {@link #stop()} is
     * called, or until a driver exception is raised.
     */
    @Override
    public void run()
    {
        synchronized (this)
        {
            if (mState != State.STOPPED)
                throw new IllegalStateException("Already running.");

            mState = State.RUNNING;
        }

        try
        {
            while (true)
            {
                if (mState != State.RUNNING)
                    break;

                step();
            }
        }
        catch (Exception e)
        {
            final Listener listener = getListener();
            if (listener != null)
                listener.onRunError(e, mUserData);
        }
        finally
        {
            synchronized (this)
            {
                mState = State.STOPPED;
            }
        }
    }



    private void step() throws IOException
    {
        // Handle incoming data.
        int len = mDriver.read(mReadBuffer.array(), READ_WAIT_MILLIS);

        if (len > 0)
        {
            final Listener listener = getListener();
            if (listener != null)
            {
                final byte[] data = new byte[len];
                mReadBuffer.get(data, 0, len);
                listener.onNewData(data, mUserData);
            }
            mReadBuffer.clear();
        }
/*
        // Handle outgoing data.
        byte[] outBuff = null;
        synchronized (mWriteBuffer)
        {
            if (mWriteBuffer.position() > 0)
            {
                len = mWriteBuffer.position();
                outBuff = new byte[len];
                mWriteBuffer.rewind();
                mWriteBuffer.get(outBuff, 0, len);
                mWriteBuffer.clear();
            }
        }
        if (outBuff != null)
            mDriver.write(outBuff, READ_WAIT_MILLIS);
*/
    }
}
          sarif_file: ${{ <?xml version="1.0"?>
<manifest package="org.mavlink.qgroundcontrol" xmlns:android="http://schemas.android.com/apk/res/android" android:versionName="3.0.0-243-gd759437" android:versionCode="300243" android:installLocation="auto">
    <application android:hardwareAccelerated="true" android:name="org.qtproject.qt5.android.bindings.QtApplication" android:label="-- %%INSERT_APP_NAME%% --" android:icon="@drawable/icon">
        <activity android:configChanges="orientation|uiMode|screenLayout|screenSize|smallestScreenSize|locale|fontScale|keyboard|keyboardHidden|navigation" android:name="org.mavlink.qgroundcontrol.QGCActivity" android:label="-- %%INSERT_APP_NAME%% --" android:screenOrientation="sensorLandscape" android:launchMode="singleTask" android:keepScreenOn="true">
            <intent-filter>
                <action android:name="android.intent.action.MAIN"/>
                <category android:name="android.intent.category.LAUNCHER"/>
                <action android:name="android.hardware.usb.action.USB_DEVICE_ATTACHED"/>
                <action android:name="android.hardware.usb.action.USB_DEVICE_DETACHED"/>
                <action android:name="android.bluetooth.device.action.ACL_CONNECTED" />
                <action android:name="android.bluetooth.device.action.ACL_DISCONNECTED" />
                <action android:name="android.hardware.usb.action.USB_ACCESSORY_ATTACHED"/>
            </intent-filter>

            <meta-data android:resource="@xml/device_filter" android:name="android.hardware.usb.action.USB_DEVICE_ATTACHED"/>
            <meta-data android:resource="@xml/device_filter" android:name="android.hardware.usb.action.USB_DEVICE_DETACHED"/>
            <meta-data android:resource="@xml/device_filter" android:name="android.hardware.usb.action.USB_ACCESSORY_ATTACHED"/>
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
    <uses-permission android:name="android.permission.WAKE_LOCK"/>

    <!-- The following comment will be replaced upon deployment with default permissions based on the dependencies of the application.
         Remove the comment if you do not require these default permissions. -->
    <!-- %%INSERT_PERMISSIONS -->

    <!-- Support devices without USB host mode since there are other connection types -->
    <uses-feature android:name="android.hardware.usb.host" android:required="false"/>

    <!-- Support devices without Bluetooth since there are other connection types -->
    <uses-feature android:name="android.hardware.bluetooth" android:required="false"/>

    <!-- Support devices that don't have location services -->
    <uses-feature android:name="android.hardware.location.gps" android:required="false" />
    <uses-feature android:name="android.hardware.location.network" android:required="false"/>
    <uses-feature android:name="android.hardware.location" android:required="false"/>
    <uses-feature android:name="android.hardware.usb.accessory"/>

        <uses-permission android:name="android.permission.INTERNET" />
        <uses-permission android:name="android.permission.WRITE_INTERNAL_STORAGE" />
        <uses-permission android:name="android.permission.WRITE_EXTERNAL_STORAGE" />

    <!-- The following comment will be replaced upon deployment with default features based on the dependencies of the application.
         Remove the comment if you do not require these default features. -->
    <!-- %%INSERT_FEATURES -->

</manifest> }}

      # Upload SARIF file as an Artifact to download and view
      # - name: <?xml version="1.0"?>
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
      #   uses: buildscript {

    repositories {
        maven  {
            url "https://repo1.maven.org/maven2"
        }
    }

    dependencies {
        classpath 'com.android.tools.build:gradle:1.1.0'
    }
}

allprojects {
    repositories {
        jcenter()
    }
}

apply plugin: 'com.android.application'

dependencies {
    compile fileTree(dir: 'libs', include: ['*.jar'])
}

android {
    /*******************************************************
     * The following variables:
     * - androidBuildToolsVersion,
     * - androidCompileSdkVersion
     * - qt5AndroidDir - holds the path to qt android files
     *                   needed to build any Qt application
     *                   on Android.
     *
     * are defined in gradle.properties file. This file is
     * updated by QtCreator and androiddeployqt tools.
     * Changing them manually might break the compilation!
     *******************************************************/

    compileSdkVersion androidCompileSdkVersion.toInteger()

    buildToolsVersion androidBuildToolsVersion

    sourceSets {
        main {
            manifest.srcFile 'AndroidManifest.xml'
            java.srcDirs = [qt5AndroidDir + '/src', 'src', 'java']
            aidl.srcDirs = [qt5AndroidDir + '/src', 'src', 'aidl']
            res.srcDirs = [qt5AndroidDir + '/res', 'res']
            resources.srcDirs = ['src']
            renderscript.srcDirs = ['src']
            assets.srcDirs = ['assets']
            jniLibs.srcDirs = ['libs']
       }
    }

    aaptOptions {
        cruncherEnabled = false
    }

    lintOptions {
        abortOnError false
    }
}
      #   with: #!/usr/bin/env bash

##############################################################################
##
##  Gradle start up script for UN*X
##
##############################################################################

# Add default JVM options here. You can also use JAVA_OPTS and GRADLE_OPTS to pass JVM options to this script.
DEFAULT_JVM_OPTS=""

APP_NAME="Gradle"
APP_BASE_NAME=`basename "$0"`

# Use the maximum available, or set MAX_FD != -1 to use that value.
MAX_FD="maximum"

warn ( ) {
    echo "$*"
}

die ( ) {
    echo
    echo "$*"
    echo
    exit 1
}

# OS specific support (must be 'true' or 'false').
cygwin=false
msys=false
darwin=false
case "`uname`" in
  CYGWIN* )
    cygwin=true
    ;;
  Darwin* )
    darwin=true
    ;;
  MINGW* )
    msys=true
    ;;
esac

# For Cygwin, ensure paths are in UNIX format before anything is touched.
if $cygwin ; then
    [ -n "$JAVA_HOME" ] && JAVA_HOME=`cygpath --unix "$JAVA_HOME"`
fi

# Attempt to set APP_HOME
# Resolve links: $0 may be a link
PRG="$0"
# Need this for relative symlinks.
while [ -h "$PRG" ] ; do
    ls=`ls -ld "$PRG"`
    link=`expr "$ls" : '.*-> \(.*\)$'`
    if expr "$link" : '/.*' > /dev/null; then
        PRG="$link"
    else
        PRG=`dirname "$PRG"`"/$link"
    fi
done
SAVED="`pwd`"
cd "`dirname \"$PRG\"`/" >&-
APP_HOME="`pwd -P`"
cd "$SAVED" >&-

CLASSPATH=$APP_HOME/gradle/wrapper/gradle-wrapper.jar

# Determine the Java command to use to start the JVM.
if [ -n "$JAVA_HOME" ] ; then
    if [ -x "$JAVA_HOME/jre/sh/java" ] ; then
        # IBM's JDK on AIX uses strange locations for the executables
        JAVACMD="$JAVA_HOME/jre/sh/java"
    else
        JAVACMD="$JAVA_HOME/bin/java"
    fi
    if [ ! -x "$JAVACMD" ] ; then
        die "ERROR: JAVA_HOME is set to an invalid directory: $JAVA_HOME

Please set the JAVA_HOME variable in your environment to match the
location of your Java installation."
    fi
else
    JAVACMD="java"
    which java >/dev/null 2>&1 || die "ERROR: JAVA_HOME is not set and no 'java' command could be found in your PATH.

Please set the JAVA_HOME variable in your environment to match the
location of your Java installation."
fi

# Increase the maximum file descriptors if we can.
if [ "$cygwin" = "false" -a "$darwin" = "false" ] ; then
    MAX_FD_LIMIT=`ulimit -H -n`
    if [ $? -eq 0 ] ; then
        if [ "$MAX_FD" = "maximum" -o "$MAX_FD" = "max" ] ; then
            MAX_FD="$MAX_FD_LIMIT"
        fi
        ulimit -n $MAX_FD
        if [ $? -ne 0 ] ; then
            warn "Could not set maximum file descriptor limit: $MAX_FD"
        fi
    else
        warn "Could not query maximum file descriptor limit: $MAX_FD_LIMIT"
    fi
fi

# For Darwin, add options to specify how the application appears in the dock
if $darwin; then
    GRADLE_OPTS="$GRADLE_OPTS \"-Xdock:name=$APP_NAME\" \"-Xdock:icon=$APP_HOME/media/gradle.icns\""
fi

# For Cygwin, switch paths to Windows format before running java
if $cygwin ; then
    APP_HOME=`cygpath --path --mixed "$APP_HOME"`
    CLASSPATH=`cygpath --path --mixed "$CLASSPATH"`

    # We build the pattern for arguments to be converted via cygpath
    ROOTDIRSRAW=`find -L / -maxdepth 1 -mindepth 1 -type d 2>/dev/null`
    SEP=""
    for dir in $ROOTDIRSRAW ; do
        ROOTDIRS="$ROOTDIRS$SEP$dir"
        SEP="|"
    done
    OURCYGPATTERN="(^($ROOTDIRS))"
    # Add a user-defined pattern to the cygpath arguments
    if [ "$GRADLE_CYGPATTERN" != "" ] ; then
        OURCYGPATTERN="$OURCYGPATTERN|($GRADLE_CYGPATTERN)"
    fi
    # Now convert the arguments - kludge to limit ourselves to /bin/sh
    i=0
    for arg in "$@" ; do
        CHECK=`echo "$arg"|egrep -c "$OURCYGPATTERN" -`
        CHECK2=`echo "$arg"|egrep -c "^-"`                                 ### Determine if an option

        if [ $CHECK -ne 0 ] && [ $CHECK2 -eq 0 ] ; then                    ### Added a condition
            eval `echo args$i`=`cygpath --path --ignore --mixed "$arg"`
        else
            eval `echo args$i`="\"$arg\""
        fi
        i=$((i+1))
    done
    case $i in
        (0) set -- ;;
        (1) set -- "$args0" ;;
        (2) set -- "$args0" "$args1" ;;
        (3) set -- "$args0" "$args1" "$args2" ;;
        (4) set -- "$args0" "$args1" "$args2" "$args3" ;;
        (5) set -- "$args0" "$args1" "$args2" "$args3" "$args4" ;;
        (6) set -- "$args0" "$args1" "$args2" "$args3" "$args4" "$args5" ;;
        (7) set -- "$args0" "$args1" "$args2" "$args3" "$args4" "$args5" "$args6" ;;
        (8) set -- "$args0" "$args1" "$args2" "$args3" "$args4" "$args5" "$args6" "$args7" ;;
        (9) set -- "$args0" "$args1" "$args2" "$args3" "$args4" "$args5" "$args6" "$args7" "$args8" ;;
    esac
fi

# Split up the JVM_OPTS And GRADLE_OPTS values into an array, following the shell quoting and substitution rules
function splitJvmOpts() {
    JVM_OPTS=("$@")
}
eval splitJvmOpts $DEFAULT_JVM_OPTS $JAVA_OPTS $GRADLE_OPTS
JVM_OPTS[${#JVM_OPTS[*]}]="-Dorg.gradle.appname=$APP_BASE_NAME"

exec "$JAVACMD" "${JVM_OPTS[@]}" -classpath "$CLASSPATH" org.gradle.wrapper.GradleWrapperMain "$@"
      #     name: @if "%DEBUG%" == "" @echo off
@rem ##########################################################################
@rem
@rem  Gradle startup script for Windows
@rem
@rem ##########################################################################

@rem Set local scope for the variables with windows NT shell
if "%OS%"=="Windows_NT" setlocal

@rem Add default JVM options here. You can also use JAVA_OPTS and GRADLE_OPTS to pass JVM options to this script.
set DEFAULT_JVM_OPTS=

set DIRNAME=%~dp0
if "%DIRNAME%" == "" set DIRNAME=.
set APP_BASE_NAME=%~n0
set APP_HOME=%DIRNAME%

@rem Find java.exe
if defined JAVA_HOME goto findJavaFromJavaHome

set JAVA_EXE=java.exe
%JAVA_EXE% -version >NUL 2>&1
if "%ERRORLEVEL%" == "0" goto init

echo.
echo ERROR: JAVA_HOME is not set and no 'java' command could be found in your PATH.
echo.
echo Please set the JAVA_HOME variable in your environment to match the
echo location of your Java installation.

goto fail

:findJavaFromJavaHome
set JAVA_HOME=%JAVA_HOME:"=%
set JAVA_EXE=%JAVA_HOME%/bin/java.exe

if exist "%JAVA_EXE%" goto init

echo.
echo ERROR: JAVA_HOME is set to an invalid directory: %JAVA_HOME%
echo.
echo Please set the JAVA_HOME variable in your environment to match the
echo location of your Java installation.

goto fail

:init
@rem Get command-line arguments, handling Windowz variants

if not "%OS%" == "Windows_NT" goto win9xME_args
if "%@eval[2+2]" == "4" goto 4NT_args

:win9xME_args
@rem Slurp the command line arguments.
set CMD_LINE_ARGS=
set _SKIP=2

:win9xME_args_slurp
if "x%~1" == "x" goto execute

set CMD_LINE_ARGS=%*
goto execute

:4NT_args
@rem Get arguments from the 4NT Shell from JP Software
set CMD_LINE_ARGS=%$

:execute
@rem Setup the command line

set CLASSPATH=%APP_HOME%\gradle\wrapper\gradle-wrapper.jar

@rem Execute Gradle
"%JAVA_EXE%" %DEFAULT_JVM_OPTS% %JAVA_OPTS% %GRADLE_OPTS% "-Dorg.gradle.appname=%APP_BASE_NAME%" -classpath "%CLASSPATH%" org.gradle.wrapper.GradleWrapperMain %CMD_LINE_ARGS%

:end
@rem End local scope for the variables with windows NT shell
if "%ERRORLEVEL%"=="0" goto mainEnd

:fail
rem Set variable GRADLE_EXIT_CONSOLE if you need the _script_ return code instead of
rem the _cmd.exe /c_ return code!
if  not "" == "%GRADLE_EXIT_CONSOLE%" exit 1
exit /b 1

:mainEnd
if "%OS%"=="Windows_NT" endlocal

:omega
      #     path: ${{ steps.run-analysis.outputs.sarif }}