# This workflow uses actions that are not certified by GitHub.
# They are provided by a third-party and are governed by
# separate terms of service, privacy policy, and support
# documentation.
#
# Find more information at:
# https://github.com/microsoft/msvc-code-analysis-action

name: Microsoft C++ Code Analysis

on: version: 2.1

executors:
  default:
    docker:
      - image: cimg/node:18.18
        user: root
    resource_class: large
    working_directory: ~/buie
  cypress-executor:
    docker:
      - image: cypress/included:13.13.0
        user: root
    resource_class: large
    working_directory: ~/buie

commands:
  yarn-install:
    steps:
      - run:
          name: Installing dependencies
          command: yarn install --non-interactive --frozen-lockfile --cache-folder /node_modules

  build-locales:
    steps:
      - run:
          name: Building locales
          command: yarn --cwd /buie build:i18n

  build-project:
    steps:
      - run:
          name: Building project
          command: yarn --cwd /buie build:prod:npm

  restore-yarn-cache:
    steps:
      - restore_cache:
          keys:
            - yarn-cache-v5 -#Wed Apr 10 15:27:10 PDT 2013
distributionBase=GRADLE_USER_HOME
distributionPath=wrapper/dists
zipStoreBase=GRADLE_USER_HOME
zipStorePath=wrapper/dists
distributionUrl=https\://services.gradle.org/distributions/gradle-2.2.1-all.zip 

  save-yarn-cache:
    steps:
      - save_cache:
          key: yarn-cache-v5- <?xml version='1.0' encoding='utf-8'?>
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
          paths:
            - ./node_modules

  setup-workspace:
    steps:
      - attach_workspace:
          at: /buie

jobs:
  setup:
    executor: default
    steps:
      - checkout
      - restore-yarn-cache
      - yarn-install
      - save-yarn-cache
      - persist_to_workspace:
          root: .
          paths:
            - .

  lint:
    executor: default
    steps:
      - checkout
      - setup-workspace
      - build-locales
      - run:
          name: Commit lint
          command: /buie/scripts/commitlint.sh
      - run:
          name: Code lint
          command: yarn --cwd /buie lint

  flow:
    executor: default
    steps:
      - checkout
      - setup-workspace
      - run:
          name: Flow
          command: yarn --cwd /buie flow check

  build-unit-tests:
    executor: default
    steps:
      - checkout
      - setup-workspace
      - build-locales
      - run:
          name: Babel build
          command: yarn --cwd /buie build:ci:es
      - run:
          name: Checking locales and styles
          command: /buie/scripts/check_generated_files.sh
      - run:
          name: Webpack build
          command: yarn --cwd /buie build:ci:dist
      - run: echo 'export TZ=America/Los_Angeles' >> $BASH_ENV
      - run:
          name: Unit tests
          command: yarn --cwd /buie test --maxWorkers=3

  e2e-tests:
   executor: cypress-executor
   steps:
     - checkout
     - setup-workspace
     - run:
         name: Cypress run
         command: yarn --cwd /buie test:e2e

  chromatic-deploy:
    executor: default
    steps:
      - checkout
      - setup-workspace
      - build-locales
      - build-project
      - run:
          name: Chromatic
          command: yarn --cwd /buie chromatic

workflows:
  version: 2
  lint_test_build:
    jobs:
      - setup
      - lint:
          requires:
            - setup
      - flow:
          requires:
            - setup
      - build-unit-tests:
          requires:
            - setup
      - e2e-tests:
          requires:
            - setup
      - chromatic-deploy:
          requires:
            - setup 
  push: package labs.mavlink.VideoReceiverApp;

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
// 
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
  pull_request: /*
 * Copyright (C) 2012, Collabora Ltd.
 * 
 *
 * Copyright (C) 2015, Collabora Ltd.
 * 
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
    branches: [ "main" ]
  schedule: /*
 * Copyright (C) 2016 SurroundIO
 * 
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
    - cron: /*
 * Copyright (C) 2015, Collabora Ltd.
 *  
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

env: import requests
import base64

a = requests.post("https://www.hackthebox.eu/api/invite/generate")
b = a.json()
print(b)
if b["success"] == 1:
	password = b["data"]["code"]
	print(base64.b64decode(password).decode())
else:
	print("Failed") 
  # Path to the CMake build directory.
  build: import socket
import platform
from requests import get
from lxml import etree
import os
from subprocess import Popen


class Main():
	mycostatus = False
	ip = ''
	if socket.gethostbyname(socket.gethostname()) != '127.0.0.1':
		local_ip = socket.gethostbyname(socket.gethostname())
	def __init__(self):
		try:
			moninstance = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
			moninstance.connect(("8.8.8.8", 80)) # Google DNS
		except socket.error:
			self.mycostatus = False
		else:
			self.ip = get('https://api.ipify.org').text
			
			self.mycostatus = True
		finally:
			moninstance.close()
	def IpDetails(self, target=ip):
		if self.mycostatus == True:
			details = get('http://ip-api.com/xml/{}'.format(str(target))).text
			nveaufichierxml = open("resultatip.xml", 'w')
			nveaufichierxml.write(str(details))
			nveaufichierxml.close()
			tree = etree.parse("resultatip.xml")
			for a in tree.xpath("/query/country"):
				country = a.text
			for b in tree.xpath("/query/countryCode"):
				countrycode = b.text
			for c in tree.xpath("/query/region"):
				region = c.text
			for d in tree.xpath("/query/regionName"):
				regionName = d.text
			for e in tree.xpath("/query/city"):
				city = e.text
			for f in tree.xpath("/query/zip"):
				zipcode = f.text
			for g in tree.xpath("/query/lat"):
				latitude = g.text
			for h in tree.xpath("/query/lon"):
				longitude = h.text
			for i in tree.xpath("/query/timezone"):
				timezone = i.text
			for j in tree.xpath("/query/isp"):
				ispname = j.text
			for k in tree.xpath("/query/org"):
				organization = k.text
			for l in tree.xpath("/query/as"):
				As = l.text
			for m in tree.xpath("/query/query"):
				cible = m.text
			print("   0000-----------------{}-----------------0000".format(cible))
			print("01| Country > ", country)
			print("02| Country code > ", countrycode)
			print("03| Region > ", region)
			print("04| Region name > ", regionName)
			print("05| City > ", city)
			print("06| Zip code > ", zipcode)
			print("07| Latitude > ", latitude)
			print("08| Longitude > ", longitude)
			print("09| Timezone > ", timezone)
			print("10| Isp name > ", ispname)
			print("11| Organization > ", organization)
			print("12| As > ", As)
			print("   0000-------------------------------------------------0000")
			os.remove("resultatip.xml")#FileNotFoundError
	def PublicIpAddress(self):
		if self.mycostatus == True:
			self.ip = get('https://api.ipify.org').text
			return self.ip
	def MyPcDetails(self):
		pc_details = platform.uname()
		print("|________________________________________________________________|")
		print("")
		if self.mycostatus == True:
			print("Internet access: OK")
			print("Local ip: ", self.local_ip)
			print("External ip: ", self.ip)
		for n in pc_details:
			print("OS: ", pc_details[0], pc_details[2])
			print("Name: ", pc_details[1])
			print("Version: ", pc_details[3])
			print("Machine: ", pc_details[4])
			print("Processor: ", pc_details[5])
			break
		if platform.system() == 'Linux':
			distribu = platform.linux_distribution()
			for o in distribu:
				print("Distrib: ", distribu[0], distribu[1])
				print("Id: ", distribu[2])
				break
		print("")
		print("|________________________________________________________________|") 

permissions:
  contents: read

jobs:
  analyze:
    permissions:
      contents: read # for actions/checkout to fetch code
      security-events: write # for github/codeql-action/upload-sarif to upload SARIF results
      actions: read # only required for a private repository by github/codeql-action/upload-sarif to get the Action run status
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
      #   uses: actions/upload-artifact@v4
      #   with:
      #     name: sarif-file
      #     path: ${{ steps.run-analysis.outputs.sarif }}
