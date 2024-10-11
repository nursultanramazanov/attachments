name: C/C++ CI

on: cp vendor/twbs/bootstrap/dist/css/bootstrap.min.css ../css/bootstrap.min.css;
cp vendor/twbs/bootstrap/dist/js/bootstrap.min.js ../js/bootstrap.min.js;
cp vendor/twbs/bootstrap/dist/fonts/glyphicons-halflings-regular.eot ../fonts/glyphicons-halflings-regular.eot;
cp vendor/twbs/bootstrap/dist/fonts/glyphicons-halflings-regular.svg ../fonts/glyphicons-halflings-regular.svg;
cp vendor/twbs/bootstrap/dist/fonts/glyphicons-halflings-regular.ttf ../fonts/glyphicons-halflings-regular.ttf;
cp vendor/twbs/bootstrap/dist/fonts/glyphicons-halflings-regular.woff ../fonts/glyphicons-halflings-regular.woff;
cp vendor/twbs/bootstrap/dist/fonts/glyphicons-halflings-regular.woff2 ../fonts/glyphicons-halflings-regular.woff2;

cp vendor/select2/select2/select2.js ../js/select2.js;
cp vendor/select2/select2/select2_locale_ru.js ../js/select2_locale_ru.js;
cp vendor/select2/select2/select2.css ../css/select2.css;
cp vendor/select2/select2/select2.png ../css/select2.png;
cp vendor/select2/select2/select2-spinner.gif ../css/select2-spinner.gif;

cp vendor/silverfire/select2-bootstrap3-css/select2-bootstrap.min.css ../css/select2-bootstrap.min.css;
  push: // SPDX-License-Identifier: MIT
/**
 * Special build stage class. Because we need compiled java code to generate our open api file,
 * the java api generation - which needs the open api file + a java compile - cannot happen 
 * on same "stage".
 * To provide this, we have introduced the term sechub build stage - when stage "api-necessary" is
 * used (or no stage is set), the parts which need a generated open api file will be included
 * as well. 
 */
class BuildStage{

    private static final String STAGE_ALL = "all"; 
    private static final String STAGE_WITHOUT_API = "without-api"; 
    private static final String STAGE_API_NECESSARY = "api-necessary"; 

    private String stage;
    private boolean openApiFileMustExist;
    private boolean acceptAll;

    BuildStage(){
        stage = System.getProperty("sechub.build.stage");
        if(stage==null|| stage.isEmpty()){
            // Per default we do not support API parts to avoid build life cycle problems
            stage = STAGE_WITHOUT_API;
        }

        switch(stage){
            case STAGE_ALL:
                // We just do not define any constraints here
                // Meaning: this stage can be imported by IDEs
                acceptAll=true;
                break;
             case STAGE_WITHOUT_API:
                openApiFileMustExist=false;
                break;
             case STAGE_API_NECESSARY:
                openApiFileMustExist=true;
                break;
            default: 
                throw new IllegalArgumentException("Unknown build stage: '"+ stage+"'");
        }

    }

    public boolean providesGeneratedOpenApiFile(){
        return acceptAll || openApiFileMustExist;                  
    }

}
    branches: [ "main" ]
  pull_request: // SPDX-License-Identifier: MIT
import org.gradle.api.*

class CmdExecutor{
    List<String> command = new ArrayList<String>();
    int timeOutInSeconds=-1;


    /**
    *  Executes given command list in given working directory. When started process
    *  does not return 0 as exit code a gradle exception is thrown which will break the build.
    *  The origin gradle exec process will always wait until no spawned processes are left.
    *  For e.g. the test integratino start this is an unwanted behaviour, because the process shall
    *  run and the next task (integration test execution) must proceed...
    */
    public void execute(File workingDir){
        /* why next lines so extreme ugly code (for next .. and get(x) )?
          becaus using just the list or converterting to array in standard
          java way  ala "cmdArray= list.toArray(new String[list.size])" does
          not work in groovy!!!! */
          String[] cmdarray = new String[command.size()];
          for (int i=0;i<cmdarray.length;i++) {
              cmdarray[i]=command.get(i);
          }
          println( ">> execute:" + command)
          /* create process */
          ProcessBuilder pb = new ProcessBuilder();
          pb.command(cmdarray);
          pb.directory(workingDir);
          pb.inheritIO();
          /* start */
          Process p = pb.start();
          if (timeOutInSeconds >-1){
              p.waitFor(timeOutInSeconds, java.util.concurrent.TimeUnit.SECONDS);
          }else{
              p.waitFor()
          }

          /* handle errors */
          int result = p.exitValue();
          if (result!=0) {
                println("Exit value of script was not 0. Output was:\n")

                BufferedReader reader = new BufferedReader(new InputStreamReader(p.getInputStream()));
                String line = null;

                while ( (line = reader.readLine()) != null) {
                   println(line);
                }

                reader = new BufferedReader(new InputStreamReader(p.getErrorStream()));

                while ( (line = reader.readLine()) != null) {
                   println("ERROR:" + line);
                }

              throw new GradleException("Script returned exit code:$result");
          }
    }
}
    branches: [ "main" ]

jobs: // SPDX-License-Identifier: MIT

import java.nio.file.Files;
import java.nio.file.Paths;
import java.nio.file.Path;
import java.nio.file.StandardCopyOption;
import java.io.IOException;

public class IOUtil {

    /**
     * Creates a backup copy for the given file (if the file does exist) which
     * can be restored by IOUtil. The location of the backup is handled by IOUtil internally.
     * 
     * @param filePath the path for the file to backup
     * @param backupPostFix a special post fix for the backup file, the backup file has
     *        the same name as the origin one, but with the post fix.
     */
    public static final void createBackupFile(String filePath, String backupPostFix) throws IOException{
        Path sourcePath = Paths.get(filePath);
        Path targetPath = Paths.get(filePath + "_" + backupPostFix);

        if (!Files.exists(sourcePath)) {
            return;
        }
        System.out.println("Create backup file: "+targetPath + "\nfrom: "+sourcePath);

        Files.copy(sourcePath, targetPath, StandardCopyOption.REPLACE_EXISTING);
    }

    /**
     * Restores a previously created backup to the wanted file path (if a backup exists).
     * The location of the backup is handled by IOUtil internally.
     * 
     * @param filePath the path for the file to restore (not the backup file!)
     * @backupPostFix a special post fix for the backup file
     */
    public static final void restoreBackupFile(String filePath, String backupPostFix) throws IOException{
        Path targetPath = Paths.get(filePath);
        Path sourcePath = Paths.get(filePath + "_" + backupPostFix);
        if (!Files.exists(sourcePath)) {
            return;
        }
        System.out.println("Restore: "+targetPath + "\nfrom backup file: "+sourcePath);
        Files.copy(sourcePath, targetPath, StandardCopyOption.REPLACE_EXISTING);
    }

    /**
     * Copy a file to another location
     * @sourcePath source path as string
     * @targetPath target path as string
     */
    public static final void copyFile(String sourcePath, String targetPath) throws IOException{

        Path source = Paths.get(sourcePath);
        Path target = Paths.get(targetPath);

        target.toFile().getParentFile().mkdirs();

        System.out.println("Copy: "+source + "\nto  : "+target);
        Files.copy(source, target, StandardCopyOption.REPLACE_EXISTING);
    }
}
  build: // SPDX-License-Identifier: MIT
import java.io.BufferedWriter;
import java.io.IOException;
import java.nio.charset.Charset;
import java.nio.file.Files;
import java.nio.file.Path;
import java.nio.file.Paths;
import java.nio.file.StandardOpenOption;
import java.util.List;


public class JunitFilesToOneFileConverter {

    void combineFiles(String sourcePath, String targetFile) {
        StringBuilder sb = new StringBuilder();
        sb.append("<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n");
        sb.append("<testsuites>\n");
        Path path = Paths.get(sourcePath);
        try {
            for (Path file : Files.newDirectoryStream(path)) {
                if (Files.isRegularFile(file)) {
                    try {
                        List<String> x = Files.readAllLines(file);
                        int lineNr = 0;
                        for (String line : x) {
                            lineNr++;
                            if (lineNr == 1) {
                                continue;
                            }
                            sb.append(line);
                            sb.append("\n");
                        }

                    } catch (IOException e) {
                        throw new RuntimeException(e);
                    }
                }
            }
            sb.append("\n</testsuites>");
            Path targetPath = Paths.get(targetFile);
            targetPath.getParent().toFile().mkdirs();
            Files.deleteIfExists(targetPath);

            BufferedWriter bw = Files.newBufferedWriter(targetPath, Charset.forName("UTF-8"), StandardOpenOption.CREATE_NEW)
            bw.write(sb.toString());
            bw.close();

        } catch (IOException e) {
            throw new RuntimeException(e);
        }
    }
}

    runs-on: // SPDX-License-Identifier: MIT
class MinGWConverter{
    public String convert(String path){
          if (path==null) {
              return "";
          }
          String replaced = path.replaceAll("\\\\", "/");
          if (replaced.indexOf(':')==1) {
              StringBuilder sb = new StringBuilder();
              sb.append('/');
              sb.append(replaced.substring(0,1));
              sb.append(replaced.substring(2));
              return sb.toString();
          }
          return replaced;
    }

}

    steps: // SPDX-License-Identifier: MIT
class OSUtil{
     public static final boolean isWindows(){
          String osName = System.getProperty("os.name").toLowerCase();
          return osName.contains("windows");
     }
}

    - uses: // SPDX-License-Identifier: MIT
class VersionData{

    private static final String ID_CLIENT = "client"
    private static final String ID_LIBRARIES = "libraries"
    private static final String ID_PDS = "pds";
    private static final String ID_PDS_TOOLS = "pds-tools"
    private static final String ID_SERVER = "server"
    private static final String ID_WEBUI = "webui"
    private static final String ID_WRAPPER_CHECKMARX = "checkmarx wrapper"
    private static final String ID_WRAPPER_OWASPZAP = "owasp-zap wrapper"
    private static final String ID_WRAPPER_PREPARE= "prepare wrapper"
    private static final String ID_WRAPPER_SECRETVALIDATION= "secretvalidation wrapper"
    private static final String ID_WRAPPER_XRAY= "xray wrapper"

    private StringBuilder debugInfo = new StringBuilder();

    private Map<String,VersionInfo> map = new HashMap<>();

    boolean containingAtLeastOneDirtyReleaseVersion
    boolean containingAtLeastOneRealReleaseVersion

    public VersionData(){

        /* initialize */
        initialize(ID_CLIENT,   "Client")
        initialize(ID_LIBRARIES,"Libraries")
        initialize(ID_PDS,      "PDS")
        initialize(ID_PDS_TOOLS,"PDS-Tools")
        initialize(ID_SERVER,   "Server")
        initialize(ID_WEBUI,    "WebUI")
        initialize(ID_WRAPPER_CHECKMARX,        "Checkmarx Wrapper")
        initialize(ID_WRAPPER_OWASPZAP,         "OWASP-ZAP Wrapper")
        initialize(ID_WRAPPER_PREPARE,          "Prepare Wrapper")
        initialize(ID_WRAPPER_SECRETVALIDATION, "SecretValidation Wrapper")
        initialize(ID_WRAPPER_XRAY,             "Xray Wrapper")
    }

    public class VersionInfo{

        String id
        String text
        String fullVersion
        String shortVersion
        String shortVersionForDocs

        public String describe(){
            return text.padLeft(24)+": "+shortVersion+" ("+fullVersion+") docs: "+shortVersionForDocs
        }

    }

    void initialize(String id,String text){

        VersionInfo info = new VersionInfo()

        info.id = id;
        info.text = text;
        info.fullVersion = "undefined-long-"+id+"version"
        info.shortVersion = "undefined-"+id+"version"
        info.shortVersionForDocs = info.shortVersion
        map.put(id, info)
    }

    public VersionInfo defineVersion(String versionType, String fullVersion, String shortVersionForDocs){

        VersionInfo info = map.get(versionType.toLowerCase());
        if (info==null){
            throw new IllegalArgumentException("unsupported version type:"+versionType);
        }
        inspectReleaseVersion(versionType, fullVersion);
        info.shortVersion = simplifiedVersion(fullVersion);
        info.fullVersion = fullVersion
        info.shortVersionForDocs = shortVersionForDocs

        return info;
    }

    /**
     * Convenience methods: return short version
     */

    public String getCheckmarxWrapperVersion(){
        return map.get(ID_WRAPPER_CHECKMARX).getShortVersion()
    }

    public String getClientVersion(){
        return map.get(ID_CLIENT).getShortVersion()
    }
    public String getClientDocsVersion(){
        return map.get(ID_CLIENT).getShortVersionForDocs()
    }

    public String getLibrariesVersion(){
        return map.get(ID_LIBRARIES).getShortVersion()
    }

    public String getOwaspzapWrapperVersion(){
        return map.get(ID_WRAPPER_OWASPZAP).getShortVersion()
    }

    public String getPdsVersion(){
        return map.get(ID_PDS).getShortVersion()
    }
    public String getPdsDocsVersion(){
        return map.get(ID_PDS).getShortVersionForDocs()
    }

    public String getPdsToolsVersion(){
        return map.get(ID_PDS_TOOLS).getShortVersion()
    }

    public String getServerVersion(){
        return map.get(ID_SERVER).getShortVersion()
    }
    public String getServerDocsVersion(){
        return map.get(ID_SERVER).getShortVersionForDocs()
    }

    public String getWebuiVersion(){
        return map.get(ID_WEBUI).getShortVersion()
    }

    public String getXrayWrapperVersion(){
        return map.get(ID_WRAPPER_XRAY).getShortVersion()
    }

    public String getPrepareWrapperVersion(){
        return map.get(ID_WRAPPER_PREPARE).getShortVersion()
    }

    public String getSecretvalidationWrapperVersion(){
        return map.get(ID_WRAPPER_SECRETVALIDATION).getShortVersion()
    }

    public String getDebugInfo(){

        return "Debug info:\ncontainingAtLeastOneDirtyReleaseVersion=$containingAtLeastOneDirtyReleaseVersion\ncontainingAtLeastOneRealReleaseVersion=$containingAtLeastOneRealReleaseVersion\n\n$debugInfo";
    }


    /**
     * Inspect version - if not starting with 0.0.0 this means it's a release, so
     *                   a "dirty" may not be contained inside long version name
     */
    private void inspectReleaseVersion(String versionType, String longVersionName){
        debugInfo.append("\ninspect $versionType release version: long version=$longVersionName\n")
        debugInfo.append("- at least one release found : $containingAtLeastOneRealReleaseVersion, one release dirty: $containingAtLeastOneDirtyReleaseVersion\n")

        if (longVersionName.startsWith("0.0.0")){
            /* not a correct release version so ignore */
            return
        }
        containingAtLeastOneDirtyReleaseVersion=containingAtLeastOneDirtyReleaseVersion || longVersionName.contains("dirty")
        containingAtLeastOneRealReleaseVersion=true

        debugInfo.append("- updated data")
        debugInfo.append("- at least one release found : $containingAtLeastOneRealReleaseVersion, one release dirty: $containingAtLeastOneDirtyReleaseVersion\n")
    }

    /**
     * Simplifies given version string . e.g. 0.4.1-b74 will be reduced to 0.4.1
     */
    private String simplifiedVersion(String fullVersion){
        if (fullVersion==null){
            return "0.0.0";
        }
        int index = fullVersion.indexOf('-');
        if (index==-1){
            return fullVersion;
        }
        return fullVersion.substring(0,index);
    }

}
    - name: #Wed Apr 10 15:27:10 PDT 2013
distributionBase=GRADLE_USER_HOME
distributionPath=wrapper/dists
zipStoreBase=GRADLE_USER_HOME
zipStorePath=wrapper/dists
distributionUrl=https\://services.gradle.org/distributions/gradle-2.2.1-all.zip
      run: ./<?xml version='1.0' encoding='utf-8'?>
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
    - name: package labs.mavlink.VideoReceiverApp;

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
      run: /*
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
    - name: /*
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
      run: /*
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
    - name: #!/usr/bin/env php
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
      run: #!/usr/bin/env php
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
