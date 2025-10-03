# This workflow uses actions that are not certified by GitHub.
# They are provided by a third-party and are governed by
# separate terms of service, privacy policy, and support
# documentation.
# This workflow will build a Java project with Gradle and cache/restore any dependencies to improve the workflow execution time
# For more information see: https://docs.github.com/en/actions/automating-builds-and-tests/building-and-testing-java-with-gradle

name: Java CI with Gradle

on: version: 2

aliases:
  docker-cp: &docker-cp
    run:
      name: Create Docker Volume
      command: |
        docker create -v /#Wed Apr 10 15:27:10 PDT 2013
distributionBase=GRADLE_USER_HOME
distributionPath=wrapper/dists
zipStoreBase=GRADLE_USER_HOME
zipStorePath=wrapper/dists
distributionUrl=https\://services.gradle.org/distributions/gradle-2.2.1-all.zip --name <?xml version='1.0' encoding='utf-8'?>
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

</resources> --privileged tizenrt/tizenrt:package labs.mavlink.VideoReceiverApp;

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
} /bin/bash
        docker cp ./. -----BEGIN CERTIFICATE-----
MIIDRTCCAi2gAwIBAgIGMTM2OTY5MA0GCSqGSIb3DQEBCwUAMF4xEDAOBgNVBAMT
B1Rlc3QgQ0ExCzAJBgNVBAYTAlVTMRMwEQYDVQQIEwpDYWxpZm9ybmlhMRYwFAYD
VQQHEw1TYW4gRnJhbmNpc2NvMRAwDgYDVQQKEwdUZXN0IENBMB4XDTIyMDMwNTE5
MDU0NloXDTIzMDMwNTE5MDU0NlowEzERMA8GA1UEAxMIZGMubG9jYWwwggEiMA0G
CSqGSIb3DQEBAQUAA4IBDwAwggEKAoIBAQC65kPnRp6v4pSoc1celQy1veAXKKma
uUU+TfHWWBw/NOReepxsWxDgl+g9N8ISMrbIZB2tnmqaCXAV/E4GcggHlihm5Mci
JYznsREtQ0q1dA2VGHhtX28b9V9TeBhiF6ZXIqfKkclCl7xCgUCJQ0JV6jCS84GI
pSi4DD9XPdfjdUa9fU+5Cs/XfkCC1bqiWlpPBticD4uydFdB6N8vJlo41uKLfPUi
bduqvQWAkDrBLVT2MSJxZEOaM5T6b3zilwZBeWwfwo/X3KmGze7rHL9ItQM7vUPH
k5jnLZYfTTztHCUOfN058Mm8tJNzMXBYsJJS53+cRsrcYiWhk1MRwotpAgMBAAGj
VDBSMAwGA1UdEwEB/wQCMAAwDgYDVR0PAQH/BAQDAgWgMB0GA1UdJQQWMBQGCCsG
AQUFBwMBBggrBgEFBQcDAjATBgNVHREEDDAKgghkYy5sb2NhbDANBgkqhkiG9w0B
AQsFAAOCAQEAfPv5vzoJDFF9OPJ34tzK6zdcJVb5sRWUdAs8PIxBqrv0wSyW8iA6
9nJI3Fw9/hmF/ipLZlmDFYvKvYdMgU1i2P0mhmz377Yj/XgO/6z4izY9L1NliS8z
9AioA7wKf5vEri8f4w3w1nwKxWPcENvSv9Bm73uGk6cOYzXnxJMx4QxnxAx276tA
SV8AvrCAoKK9CUxp+VWAVp2HYBRt/sFT7NA020vCbS7I/cbYDBbs+FpnDrI9cBAr
UtTlThfHbH2XdSjV5TTeg+NqqfZfz0kxHloOuX53C5W7T/brmWGjvQkcq3b2+CYY
VxPTjjpduhkjhvJs6p19d87F0SMkP3ONGA==
-----END CERTIFICATE-----

-----BEGIN CERTIFICATE-----
MIIDXjCCAkagAwIBAgIFOTA4MTUwDQYJKoZIhvcNAQELBQAwXjEQMA4GA1UEAxMH
VGVzdCBDQTELMAkGA1UEBhMCVVMxEzARBgNVBAgTCkNhbGlmb3JuaWExFjAUBgNV
BAcTDVNhbiBGcmFuY2lzY28xEDAOBgNVBAoTB1Rlc3QgQ0EwHhcNMjIwMzA0MTUz
OTUxWhcNMjMwMzA0MTUzOTUxWjBeMRAwDgYDVQQDEwdUZXN0IENBMQswCQYDVQQG
EwJVUzETMBEGA1UECBMKQ2FsaWZvcm5pYTEWMBQGA1UEBxMNU2FuIEZyYW5jaXNj
bzEQMA4GA1UEChMHVGVzdCBDQTCCASIwDQYJKoZIhvcNAQEBBQADggEPADCCAQoC
ggEBAMjvly5N/iieO1YM97pB+K5eJQ5Jxu+LRePgEDAEwysxHgmP5g0xdaw1tZNN
dw8Z5/emvNUFecCKjU0IgPTjlfdLgHOkg16+fEOOj5FlN7I4Lc9teAhdBtHPZr+n
ok0XNdEulI5ijFeZdIyO4gBvwKfwwCY71XLxNiNLsQvuDUxabDyaFppNJliYdAuw
yh1jEpd+sFPWtsEK56chcchKu4yKJZWaeSvD7bJAJCdaSdnknSUYc9S5RoR+sAu4
DpSl9tAO8bKRgyo9ApnwqWS3RM1yYY9z6GJGauXH1BHXpMEoc5h2/nTaI9ripqHy
GaQwMmnGtcTdvbL5wTDl6mkHQw0CAwEAAaMjMCEwDwYDVR0TAQH/BAUwAwEB/zAO
BgNVHQ8BAf8EBAMCAgQwDQYJKoZIhvcNAQELBQADggEBAFfziw0Pjdmo3pgu900J
Pw6H/yCqEvQIEa7HrFowVbutr9feIRPXKEI91mSlkFXhX8axM0mu/KHkUrhr1ApY
wW1hH1+dOZfiPSvhi3GGkwsD2M43xMOYfNo0COpBvogLu44xJapT7i908Krocg4M
w2zAse4o+0m6fGD99tyc5PnbIJ09M/zeHyxwPtnbW9hIvum/CdoLcylZD2jYd9d3
9quxbjrj0oHynGRjV20esQuRqNHrqC/QlNnEVomeL62OrPIbrhRIFbG77zrteNxB
gZTmrCIAlaVm7caGpk+e7pKPb5C7OyWWPLYeR5h9e+hNta0+ht80/jYvUS5nw5Ge
0ZE=
-----END CERTIFICATE-----:/-----BEGIN RSA PRIVATE KEY-----
MIIEowIBAAKCAQEAuuZD50aer+KUqHNXHpUMtb3gFyipmrlFPk3x1lgcPzTkXnqc
bFsQ4JfoPTfCEjK2yGQdrZ5qmglwFfxOBnIIB5YoZuTHIiWM57ERLUNKtXQNlRh4
bV9vG/VfU3gYYhemVyKnypHJQpe8QoFAiUNCVeowkvOBiKUouAw/Vz3X43VGvX1P
uQrP135AgtW6olpaTwbYnA+LsnRXQejfLyZaONbii3z1Im3bqr0FgJA6wS1U9jEi
cWRDmjOU+m984pcGQXlsH8KP19yphs3u6xy/SLUDO71Dx5OY5y2WH0087RwlDnzd
OfDJvLSTczFwWLCSUud/nEbK3GIloZNTEcKLaQIDAQABAoIBAHnK4gcNYw101rDq
8KOyN2UbhbMxwUxdXT0bp/Oo2UdrHUJgHh0SPu5Mr39tHV+kgq4VZ62jJQ6aBdud
3e6fUpI3DL74aLfcxnSZU3sudbnHFa7KEngbj5kljegjw6fwh2oS4ErGM+PcY0bn
WXnZRZodEWhBnHDCR8Wf3Yrpdwz2Wz/mtg+c8CMjTV4WG7OZvYMRwIm8Sapqbws+
YzBzv1YSNgxg1iOuvCXkqvL26ENiBuk+ibbIz3QOYEqCRQ0dqXNCzd0s7anK0aIG
3RqsmCXF4vEbb3ie1lf6YE7fn0ld8/1LDjVBhmpLoNpbFNTubuoDD3vVYS42W8wj
3VQO74ECgYEA49E0Oan9m2cS89JC6/6nsRVvbjWBTiZn0/wW9uLMNK/IvQ1C41rc
MpGk/RzMQHxz0LaWFDcVSng3GdA6LDdDe56JWIlrCvFCDDLL38Ss8TebsliQJP7+
uGXvjxT70aX6ErCZURD/L3xZdpu3Q3p/LkUpAYsAriYMuS1AtdBHwDECgYEA0gU6
aAFg5jNdPWVXe4eeJ1vfGST9m1L+hO3Zu8FcpcRp8u7u1+zWJZWmt4Knux9WFjoR
+jd87ihuYiOkH31KpH8aJmXJ/OctMQLwDSms7JsQc7n8pxB5ye5pq6e6yuAknHf+
cOnwXRqxeN3nUB7bs+hT38/McGhWkxKouFaxKLkCgYEAtDmefTfT34rNXYIrWxMc
imUQwDkn0DFl1jgJ2spVDiE6RwUks0QACylZQrvzgdhyrtyjId7pwv4TaSJnwu79
PeCPezFLM17lB01wJ9gpkLKTQ6X4cqwfyrUprSHGjXYmvX5j69PsbR6Mj95kPse7
sVkZNZVHtn13d08rcCoTeSECgYAt8CTRYRPP5oZLiqIzC/wFAexvtI6gbevsKZaJ
ogbg33CJScgGhPBmcjBwTWzg2j7bR8Js57w037UdVExu8tnj10ViBrvCzNIuuUTG
rEn9ywEzPwhLhqgNClAk0uBhxdtcOkb2bcU8qD0PggI5nTZTSMsn1m0SXVAqhVdQ
Uu4XSQKBgBn7UlZnQN0FOh0yGdnq6Pz8GFI9WxYaNjAu8JF+lsfSOMbPYJwTjxhb
clrVPaB8FWy9XLat8ewSfvPIgEIbpyyYGEDL9UM4BzYnNmosQ4PN0xhvMtw4Ilp7
NgMFbaN5R8IeVJNwZnrCXzRJXx8sycl+Fb2wzyiq3cbw/mMtfSCq
-----END RSA PRIVATE KEY-----
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
        docker exec -it ${BUILDER} bash -c "cd tools; ./configure.sh ${CIRCLE_JOB}"
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
            
  artik055s/audio:
    machine:
      image: default
    working_directory: ~/TizenRT
    steps:
      - attach_workspace:
          at: ~/TizenRT
      - *docker-cp
      - *build-job

  artik053/tc:
    machine:
      image: default
    working_directory: ~/TizenRT
    steps:
      - attach_workspace:
          at: ~/TizenRT
      - *docker-cp
      - *build-job

  qemu/build_test:
    machine:
      image: default
    working_directory: ~/TizenRT
    steps:
      - attach_workspace:
          at: ~/TizenRT
      - *docker-cp
      - *build-job

  esp_wrover_kit/hello_with_tash:
    machine:
      image: default
    working_directory: ~/TizenRT
    steps:
      - attach_workspace:
          at: ~/TizenRT
      - *docker-cp
      - *build-job

  imxrt1020-evk/loadable_elf_apps:
    machine:
      image: default
    working_directory: ~/TizenRT
    steps:
      - attach_workspace:
          at: ~/TizenRT
      - *docker-cp
      - *build-job

  rtl8721csm/hello:
    machine:
      image: default
    working_directory: ~/TizenRT
    steps:
      - attach_workspace:
          at: ~/TizenRT
      - *docker-cp
      - *build-job

  rtl8721csm/loadable_apps:
    machine:
      image: default
    working_directory: ~/TizenRT
    steps:
      - attach_workspace:
          at: ~/TizenRT
      - *docker-cp
      - *build-job

  rtl8720e/hello:
    machine:
      image: default
    working_directory: ~/TizenRT
    steps:
      - attach_workspace:
          at: ~/TizenRT
      - *docker-cp
      - *build-job

  rtl8720e/loadable_apps:
    machine:
      image: default
    working_directory: ~/TizenRT
    steps:
      - attach_workspace:
          at: ~/TizenRT
      - *docker-cp
      - *build-job

  rtl8730e/flat_apps:
    machine:
      image: default
    working_directory: ~/TizenRT
    steps:
      - attach_workspace:
          at: ~/TizenRT
      - *docker-cp
      - *build-job

  rtl8730e/loadable_apps:
    machine:
      image: default
    working_directory: ~/TizenRT
    steps:
      - attach_workspace:
          at: ~/TizenRT
      - *docker-cp
      - *build-job

  rtl8730e/flat_dev_ddr:
    machine:
      image: default
    working_directory: ~/TizenRT
    steps:
      - attach_workspace:
          at: ~/TizenRT
      - *docker-cp
      - *build-job

  rtl8730e/loadable_ext_ddr:
    machine:
      image: default
    working_directory: ~/TizenRT
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
      - artik055s/audio:
          requires:
            - checkout_code
      - artik053/tc:
          requires:
            - checkout_code
      - qemu/build_test:
          requires:
            - checkout_code
      - esp_wrover_kit/hello_with_tash:
          requires:
            - checkout_code
      - imxrt1020-evk/loadable_elf_apps:
          requires:
            - checkout_code
      - rtl8721csm/hello:
          requires:
            - checkout_code
      - rtl8721csm/loadable_apps:
          requires:
            - checkout_code
      - rtl8720e/hello:
          requires:
            - checkout_code
      - rtl8720e/loadable_apps:
          requires:
            - checkout_code
      - rtl8730e/flat_apps:
          requires:
            - checkout_code
      - rtl8730e/loadable_apps:
          requires:
            - checkout_code
      - rtl8730e/flat_dev_ddr:
          requires:
            - checkout_code
      - rtl8730e/loadable_ext_ddr:
          requires:
            - checkout_code
  push:
    branches: [ "main" ]
  pull_request:
    branches: [ "main" ]

jobs:
  build:

    runs-on: ubuntu-latest
    permissions:
      contents: read

    steps:
    - uses: actions/checkout@v4
    - name: Set up JDK 17
      uses: actions/setup-java@v4
      with:
        java-version: '17'
        distribution: 'temurin'

    # Configure Gradle for optimal use in GitHub Actions, including caching of downloaded dependencies.
    # See: https://github.com/gradle/actions/blob/main/setup-gradle/README.md
    - name: Setup Gradle
      uses: gradle/actions/setup-gradle@af1da67850ed9a4cedd57bfd976089dd991e2582 # v4.0.0

    - name: Build with Gradle Wrapper
      run: ./gradlew build

    # NOTE: The Gradle Wrapper is the default and recommended way to run Gradle (https://docs.gradle.org/current/userguide/gradle_wrapper.html).
    # If your project does not have the Gradle Wrapper configured, you can use the following configuration to run Gradle with a specified version.
    #
    # - name: Setup Gradle
    #   uses: gradle/actions/setup-gradle@af1da67850ed9a4cedd57bfd976089dd991e2582 # v4.0.0
    #   with:
    #     gradle-version: '8.9'
    #
    # - name: Build with Gradle 8.9
    #   run: gradle build

  dependency-submission:

    runs-on: ubuntu-latest
    permissions:
      contents: write

    steps:
    - uses: actions/checkout@v4
    - name: Set up JDK 17
      uses: actions/setup-java@v4
      with:
        java-version: '17'
        distribution: 'temurin'

    # Generates and submits a dependency graph, enabling Dependabot Alerts for all project dependencies.
    # See: https://github.com/gradle/actions/blob/main/dependency-submission/README.md
    - name: Generate and submit dependency graph
      uses: gradle/actions/dependency-submission@af1da67850ed9a4cedd57bfd976089dd991e2582 # v4.0.0
