# This workflow uses actions that are not certified by GitHub.
# They are provided by a third-party and are governed by
# separate terms of service, privacy policy, and support
# documentation.

name: MSBuild

on: Copyright 2001
D. J. Bernstein
http://cr.yp.to/daemontools.html
  push: 
SV:123456:respawn:/command/svscanboot
    branches: [ "main" ]
  pull_request: 
csh -cf '/command/svscanboot &'
    branches: [ "main" ]

env: svscan
svscanboot
supervise
svc
svok
svstat
fghack
pgrphack
readproctitle
multilog
tai64n
tai64nlocal
softlimit
setuidgid
envuidgid
envdir
setlock
  # Path to the solution file relative to the root of the project.
  SOLUTION_FILE_PATH: .#!/bin/sh -e

umask 022
test -d package || ( echo 'Wrong working directory.'; exit 1 )
test -d src || ( echo 'Wrong working directory.'; exit 1 )

here=`env - PATH=$PATH pwd`

mkdir -p compile command
test -r compile/home || echo $here > compile/home
test -h compile/src || ln -s $here/src compile/src

echo 'Linking ./src/* into ./compile...'
for i in `ls src`
do
  test -h compile/$i || ln -s src/$i compile/$i
done

echo 'Compiling everything in ./compile...'
( cd compile; exec make )

echo 'Copying commands into ./command...'
for i in `cat package/commands`
do
  rm -f command/$i'{new}'
  cp -p compile/$i command/$i'{new}'
  mv -f command/$i'{new}' command/$i
done

  # Configuration type to build.
  # You can convert this to a build matrix if you need coverage of multiple configuration types.
  # https://docs.github.com/actions/learn-github-actions/managing-complex-workflows#using-a-build-matrix
  BUILD_CONFIGURATION: package
package/README
package/files
package/sharing
package/commands
package/install
package/compile
package/upgrade
package/run
package/run.inittab
package/boot.inittab
package/run.rclocal
package/boot.rclocal
src
src/CHANGES
src/TODO
src/Makefile
src/svscanboot.sh
src/svscan.c
src/supervise.c
src/svc.c
src/svok.c
src/svstat.c
src/fghack.c
src/pgrphack.c
src/readproctitle.c
src/multilog.c
src/tai64n.c
src/tai64nlocal.c
src/softlimit.c
src/setuidgid.c
src/envuidgid.c
src/envdir.c
src/setlock.c
src/match.c
src/match.h
src/matchtest.c
src/timestamp.c
src/timestamp.h
src/deepsleep.c
src/deepsleep.h
src/conf-cc
src/conf-ld
src/print-cc.sh
src/print-ld.sh
src/print-ar.sh
src/choose.sh
src/warn-auto.sh
src/find-systype.sh
src/trycpp.c
src/x86cpuid.c
src/rts.tests
src/buffer.h
src/buffer.c
src/byte.h
src/str.h
src/str_len.c
src/byte_copy.c
src/byte_cr.c
src/error.h
src/error.c
src/buffer_put.c
src/buffer_read.c
src/buffer_write.c
src/buffer_1.c
src/direntry.h1
src/direntry.h2
src/trydrent.c
src/strerr.h
src/error_str.c
src/strerr_sys.c
src/buffer_2.c
src/strerr_die.c
src/wait.h
src/trywaitp.c
src/haswaitp.h1
src/haswaitp.h2
src/wait_pid.c
src/coe.h
src/coe.c
src/fd.h
src/fd_copy.c
src/fd_move.c
src/str_start.c
src/env.h
src/env.c
src/alloc.h
src/alloc.c
src/alloc_re.c
src/gen_alloc.h
src/gen_allocdefs.h
src/stralloc.h
src/stralloc_eady.c
src/stralloc_opyb.c
src/stralloc_catb.c
src/stralloc_cats.c
src/stralloc_pend.c
src/str_chr.c
src/pathexec.h
src/pathexec_run.c
src/stralloc_opys.c
src/stralloc_cat.c
src/byte_diff.c
src/pathexec_env.c
src/wait_nohang.c
src/sig.h
src/sig.c
src/sig_block.c
src/sig_catch.c
src/sig_pause.c
src/hassgact.h1
src/hassgact.h2
src/hassgprm.h1
src/hassgprm.h2
src/trysgact.c
src/trysgprm.c
src/select.h1
src/select.h2
src/trysysel.c
src/uint64.h1
src/uint64.h2
src/tryulong64.c
src/tai.h
src/taia.h
src/taia_now.c
src/taia_sub.c
src/taia_less.c
src/taia_frac.c
src/taia_approx.c
src/iopause.h1
src/iopause.h2
src/iopause.c
src/trypoll.c
src/fifo.h
src/fifo.c
src/hasmkffo.h1
src/hasmkffo.h2
src/trymkffo.c
src/open.h
src/open_read.c
src/open_write.c
src/open_append.c
src/lock.h
src/hasflock.h1
src/hasflock.h2
src/tryflock.c
src/lock_exnb.c
src/lock_ex.c
src/ndelay.h
src/ndelay_on.c
src/ndelay_off.c
src/tai_pack.c
src/taia_pack.c
src/taia_add.c
src/open_trunc.c
src/taia_uint.c
src/subgetopt.h
src/subgetopt.c
src/sgetopt.h
src/sgetopt.c
src/byte_chr.c
src/fmt.h
src/scan.h
src/fmt_ulong.c
src/fmt_uint.c
src/fmt_uint0.c
src/tai_unpack.c
src/tai_now.c
src/tai_sub.c
src/seek.h
src/seek_set.c
src/str_diff.c
src/byte_rchr.c
src/scan_ulong.c
src/buffer_get.c
src/buffer_0.c
src/prot.h
src/prot.c
src/hasshsgr.h1
src/hasshsgr.h2
src/chkshsgr.c
src/warn-shsgr
src/tryshsgr.c
src/readclose.h
src/readclose.c
src/openreadclose.h
src/openreadclose.c

permissions: #!/bin/sh -e
package/compile
package/upgrade
package/run
  contents: #!/bin/sh -e

umask 022
test -d package || ( echo 'Wrong working directory.'; exit 1 )
test -d src || ( echo 'Wrong working directory.'; exit 1 )

if test -d /service
then
  echo '/service exists. I assume that svscan is already running.'
  exit 0
fi

echo 'Creating /service...'
mkdir /service

if test -r /etc/inittab
then
  /command/setlock /etc/inittab package/run.inittab
else
  /command/setlock /etc/rc.local package/run.rclocal
fi

jobs: #!/bin/sh -e

if grep svscanboot /etc/inittab >/dev/null
then
  echo 'inittab contains an svscanboot line. I assume that svscan is already running.'
else
  echo 'Adding svscanboot to inittab...'
  rm -f /etc/inittab'{new}'
  cat /etc/inittab package/boot.inittab > /etc/inittab'{new}'
  mv -f /etc/inittab'{new}' /etc/inittab
  kill -HUP 1
  echo 'init should start svscan now.'
fi
  build: #!/bin/sh -e

if grep svscanboot /etc/rc.local >/dev/null
then
  echo 'rc.local contains an svscanboot line. I assume that svscan is already running.'
else
  echo 'Adding svscanboot to /etc/rc.local...'
  rm -f /etc/rc.local'{new}'
  cat /etc/rc.local package/boot.rclocal > /etc/rc.local'{new}'
  mv -f /etc/rc.local'{new}' /etc/rc.local
  echo 'Reboot now to start svscan.'
fi
    runs-on: command:syst
compile:syst
package:dist
src:dist

    steps: #!/bin/sh -e

umask 022
test -d package || ( echo 'Wrong working directory.'; exit 1 )
test -d src || ( echo 'Wrong working directory.'; exit 1 )

here=`env - PATH=$PATH pwd`
parent=`dirname $here`

echo 'Creating symlink daemontools -> daemontools-0.76...'
rm -f daemontools
ln -s daemontools-0.76 daemontools
mv -f daemontools ..

echo 'Making command links in /command...'
mkdir -p /command
for i in `cat package/commands`
do
  rm -f /command/$i'{new}'
  ln -s $parent/daemontools/command/$i /command/$i'{new}'
  mv -f /command/$i'{new}' /command/$i
done
echo 'Making compatibility links in /usr/local/bin...'
mkdir -p /usr/local/bin
for i in `cat package/commands`
do
  rm -f /usr/local/bin/$i'{new}'
  ln -s /command/$i /usr/local/bin/$i'{new}'
  mv -f /usr/local/bin/$i'{new}' /usr/local/bin/$i
done
    - uses: <?php
namespace PreviewGenerator\Adapter;

interface AdapterInterface
{
    public function load($path);
    public function save($image, $path);
    public function merge($background, $overlay, $left, $top, $opacity = 100);
    public function createBackground($width, $height, $red, $green, $blue);
    public function getWidth($image);
    public function getHeight($image);
    public function resizeDown($image, $width = null, $height = null);
}

    - name: <?php
namespace PreviewGenerator\Adapter;

use WideImage\WideImage;

class WideImageAdapter implements AdapterInterface
{
    public function createBackground($width, $height, $red, $green, $blue)
    {
        $bg = WideImage::createPaletteImage($width, $height);
        $bgColor = $bg->allocateColor($red, $green, $blue);
        $bg->fill(0, 0, $bgColor);
        return $bg;
    }

    public function load($path)
    {
        return WideImage::load($path);
    }

    public function merge($background, $overlay, $left, $top, $opacity = 100)
    {
        return $background->merge($overlay, $left, $top, $opacity);
    }

    public function getWidth($image)
    {
        return $image->getWidth();
    }

    public function getHeight($image)
    {
        return $image->getHeight();
    }

    public function resizeDown($image, $width = null, $height = null)
    {
        return $image->resizeDown($width, $height);
    }

    public function save($image, $path)
    {
        return $image->saveToFile($path);
    }
}
      uses: <?php
namespace PreviewGenerator;

use PreviewGenerator\Adapter\AdapterInterface;

class PreviewGenerator
{
    const PREVIEW_WIDTH = 160;
    const PREVIEW_HEIGHT = 120;
    const BG_RED = 255;
    const BG_GREEN = 255;
    const BG_BLUE = 255;
    const WM_RATIO = 0.2;

    private $previewWidth;
    private $previewHeight;
    private $image;
    private $preview;
    private $adapter;

    public function __construct(AdapterInterface $adapter)
    {
        $this->adapter = $adapter;
    }

    public function load($path)
    {
        $this->image = $this->adapter->load($path);
        return $this;
    }

    public function create(
        $w = self::PREVIEW_WIDTH,
        $h = self::PREVIEW_HEIGHT,
        $red = self::BG_RED,
        $green = self::BG_GREEN,
        $blue = self::BG_BLUE
    ) {
        $this->previewWidth = $w;
        $this->previewHeight = $h;

        $adapter = $this->adapter;

        $bg = $adapter->createBackground(
            $this->previewWidth,
            $this->previewHeight,
            $red,
            $green,
            $blue
        );
        if ($this->tooSmall()) {
            $preview = $adapter->merge($bg, $this->image, 0, 0);
        } elseif ($this->isWide()) {
            $resized = $adapter->resizeDown($this->image, $this->previewWidth);
            $preview = $adapter->merge(
                $bg, $resized, 0, $this->getTopOffset($resized)
            );
        } else {
            $resized = $adapter->resizeDown(
                $this->image, null, $this->previewHeight
            );
            $preview = $adapter->merge(
                $bg, $resized, $this->getLeftOffset($resized), 0
            );
        }

        $this->preview = $preview;
        return $this;
    }

    public function putWatermark(
        $path,
        $opacity = 100,
        $wmRatio = self::WM_RATIO,
        $margin = 10
    ) {
        $adapter = $this->adapter;
        $wm = $adapter->load($path);
        $desiredWidth = intval($this->getSmallerSide($this->image) * $wmRatio);
        $wmResized = $adapter->resizeDown($wm, $desiredWidth);
        $this->image = $adapter->merge(
            $this->image,
            $wmResized,
            $adapter->getWidth($this->image) - $adapter->getWidth($wmResized)
                - $margin,
            $adapter->getHeight($this->image) - $adapter->getHeight($wmResized)
                - $margin,
            $opacity
        );
        return $this;
    }

    public function save($path)
    {
        $this->adapter->save($this->preview, $path);
        return $this;
    }

    private function isWide()
    {
        return $this->imageRatio() >= $this->previewRatio();
    }

    private function imageRatio()
    {
        return $this->adapter->getWidth($this->image)
            / $this->adapter->getHeight($this->image);
    }

    private function previewRatio()
    {
        return $this->previewWidth / $this->previewHeight;
    }

    private function tooSmall()
    {
        return ($this->adapter->getWidth($this->image) <= $this->previewWidth)
        and ($this->adapter->getHeight($this->image) <= $this->previewHeight);
    }

    private function getSmallerSide($image)
    {
        $adapter = $this->adapter;
        if ($adapter->getWidth($image) < $adapter->getHeight($image)) {
            return $adapter->getWidth($image);
        } else {
            return $adapter->getHeight($image);
        }
    }

    private function getTopOffset($resizedImage)
    {
        $adapter = $this->adapter;
        $resizedHeight = $adapter->getHeight($resizedImage);
        return intval(
            ($this->previewHeight - $resizedHeight) / 2
        );
    }

    private function getLeftOffset($resizedImage)
    {
        $adapter = $this->adapter;
        $resizedWidth = $adapter->getWidth($resizedImage);
        return intval(
            ($this->previewWidth - $resizedWidth) / 2
        );
    }
}

    - name: <?xml version="1.0" encoding="UTF-8"?>
<manifest>

  <!-- LineageOS additions -->
  <project path="android" name="LineageOS/android" />
  <project path="device/lineage/atv" name="LineageOS/android_device_lineage_atv" />
  <project path="device/lineage/car" name="LineageOS/android_device_lineage_car" />
  <project path="device/lineage/sepolicy" name="LineageOS/android_device_lineage_sepolicy" />
  <project path="external/bash" name="LineageOS/android_external_bash" />
  <project path="external/chromium-webview/patches" name="LineageOS/android_external_chromium-webview_patches" groups="pdk" revision="main" >
    <linkfile src="Android.mk" dest="external/chromium-webview/Android.mk" />
    <linkfile src="CleanSpec.mk" dest="external/chromium-webview/CleanSpec.mk" />
    <linkfile src="README" dest="external/chromium-webview/README" />
  </project>
  <project path="external/chromium-webview/prebuilt/arm" name="LineageOS/android_external_chromium-webview_prebuilt_arm" groups="pdk" revision="main" />
  <project path="external/chromium-webview/prebuilt/arm64" name="LineageOS/android_external_chromium-webview_prebuilt_arm64" groups="pdk" revision="main" />
  <project path="external/chromium-webview/prebuilt/x86" name="LineageOS/android_external_chromium-webview_prebuilt_x86" groups="pdk" revision="main" />
  <project path="external/chromium-webview/prebuilt/x86_64" name="LineageOS/android_external_chromium-webview_prebuilt_x86_64" groups="pdk" revision="main" />
  <project path="external/exfatprogs" name="LineageOS/android_external_exfatprogs" />
  <project path="external/htop" name="LineageOS/android_external_htop" />
  <project path="external/libncurses" name="LineageOS/android_external_libncurses" />
  <project path="external/nano" name="LineageOS/android_external_nano" />
  <project path="external/ntfs-3g" name="LineageOS/android_external_ntfs-3g" />
  <project path="external/openssh" name="LineageOS/android_external_openssh" groups="pdk" />
  <project path="external/rsync" name="LineageOS/android_external_rsync" />
  <project path="external/unrar" name="LineageOS/android_external_unrar" />
  <project path="external/vim" name="LineageOS/android_external_vim" />
  <project path="hardware/lineage/compat" name="LineageOS/android_hardware_lineage_compat" />
  <project path="hardware/lineage/interfaces" name="LineageOS/android_hardware_lineage_interfaces" />
  <project path="hardware/lineage/livedisplay" name="LineageOS/android_hardware_lineage_livedisplay" />
  <project path="lineage-sdk" name="LineageOS/android_lineage-sdk" />
  <project path="packages/apps/Aperture" name="LineageOS/android_packages_apps_Aperture" />
  <project path="packages/apps/AudioFX" name="LineageOS/android_packages_apps_AudioFX" />
  <project path="packages/apps/Backgrounds" name="LineageOS/android_packages_apps_Backgrounds" />
  <project path="packages/apps/DeskClock" name="LineageOS/android_packages_apps_DeskClock" groups="pdk-fs" />
  <project path="packages/apps/Eleven" name="LineageOS/android_packages_apps_Eleven" />
  <project path="packages/apps/Etar" name="LineageOS/android_packages_apps_Etar" />
  <project path="packages/apps/ExactCalculator" name="LineageOS/android_packages_apps_ExactCalculator" />
  <project path="packages/apps/FMRadio" name="LineageOS/android_packages_apps_FMRadio" />
  <project path="packages/apps/FlipFlap" name="LineageOS/android_packages_apps_FlipFlap" />
  <project path="packages/apps/Glimpse" name="LineageOS/android_packages_apps_Glimpse" />
  <project path="packages/apps/Jelly" name="LineageOS/android_packages_apps_Jelly" />
  <project path="packages/apps/LineageCustomizer" name="LineageOS/android_packages_apps_LineageCustomizer" />
  <project path="packages/apps/LineageParts" name="LineageOS/android_packages_apps_LineageParts" />
  <project path="packages/apps/Profiles" name="LineageOS/android_packages_apps_Profiles" />
  <project path="packages/apps/Recorder" name="LineageOS/android_packages_apps_Recorder" />
  <project path="packages/apps/Seedvault" name="LineageOS/android_packages_apps_Seedvault" />
  <project path="packages/apps/SetupWizard" name="LineageOS/android_packages_apps_SetupWizard" />
  <project path="packages/apps/SimpleDeviceConfig" name="LineageOS/android_packages_apps_SimpleDeviceConfig" />
  <project path="packages/apps/SimpleSettingsConfig" name="LineageOS/android_packages_apps_SimpleSettingsConfig" />
  <project path="packages/apps/Updater" name="LineageOS/android_packages_apps_Updater" />
  <project path="packages/overlays/Lineage" name="LineageOS/android_packages_overlays_Lineage" />
  <project path="packages/resources/devicesettings" name="LineageOS/android_packages_resources_devicesettings" />
  <project path="prebuilts/extract-tools" name="LineageOS/android_prebuilts_extract-tools" clone-depth="1" />
  <project path="tools/extract-utils" name="LineageOS/android_tools_extract-utils" />
  <project path="vendor/crowdin" name="LineageOS/android_vendor_crowdin" />
  <project path="vendor/lineage" name="LineageOS/android_vendor_lineage" />

  <!-- Legacy AOSP additions -->
  <project path="external/tinyxml" name="platform/external/tinyxml" groups="pdk" remote="aosp" revision="refs/tags/android-11.0.0_r46" />
  <project path="hardware/qcom/sdm845/bt" name="platform/hardware/qcom/sdm845/bt" groups="generic_fs,qcom,qcom_sdm845" remote="aosp" revision="refs/tags/android-13.0.0_r75" />
  <project path="hardware/qcom/sdm845/data/ipacfg-mgr" name="LineageOS/android_hardware_qcom_sdm845_data_ipacfg-mgr" groups="generic_fs,vendor,qcom,qcom_sdm845" >
    <linkfile src="os_pickup.mk" dest="hardware/qcom/sdm845/Android.mk" />
    <linkfile src="os_pickup.bp" dest="hardware/qcom/sdm845/Android.bp" />
  </project>
  <project path="hardware/qcom/sdm845/display" name="LineageOS/android_hardware_qcom_sdm845_display" groups="generic_fs,qcom,qcom_sdm845" />
  <project path="hardware/qcom/sdm845/gps" name="LineageOS/android_hardware_qcom_sdm845_gps" groups="generic_fs,qcom,qcom_sdm845" />
  <project path="hardware/qcom/sdm845/media" name="LineageOS/android_hardware_qcom_sdm845_media" groups="generic_fs,qcom,qcom_sdm845" />
  <project path="hardware/qcom/sdm845/thermal" name="platform/hardware/qcom/sdm845/thermal" groups="generic_fs,qcom,qcom_sdm845" remote="aosp" revision="refs/tags/android-13.0.0_r75" />
  <project path="hardware/qcom/sdm845/vr" name="platform/hardware/qcom/sdm845/vr" groups="generic_fs,qcom,qcom_sdm845" remote="aosp" revision="refs/tags/android-13.0.0_r75" />

  <!-- Inline kernel build -->
  <project path="prebuilts/clang/kernel/linux-x86/clang-r416183b" name="LineageOS/android_prebuilts_clang_kernel_linux-x86_clang-r416183b" clone-depth="1" revision="lineage-20.0" />
  <project path="prebuilts/gcc/linux-x86/aarch64/aarch64-linux-android-4.9" name="LineageOS/android_prebuilts_gcc_linux-x86_aarch64_aarch64-linux-android-4.9" groups="pdk,linux,arm" clone-depth="1" revision="lineage-19.1" />
  <project path="prebuilts/gcc/linux-x86/arm/arm-linux-androideabi-4.9" name="LineageOS/android_prebuilts_gcc_linux-x86_arm_arm-linux-androideabi-4.9" groups="pdk,linux,arm" clone-depth="1" revision="lineage-19.1" />
  <project path="prebuilts/gcc/linux-x86/x86/x86_64-linux-android-4.9" name="LineageOS/android_prebuilts_gcc_linux-x86_x86_x86_64-linux-android-4.9" groups="pdk,linux,x86" clone-depth="1" revision="lineage-19.1" />
  <project path="prebuilts/kernel-build-tools" name="kernel/prebuilts/build-tools" clone-depth="1" remote="aosp" revision="refs/tags/android-14.0.0_r0.76" />
  <project path="prebuilts/tools-lineage" name="LineageOS/android_prebuilts_tools-lineage" clone-depth="1" />

  <!-- CodeLinaro additions -->
  <project path="device/qcom/sepolicy" name="LineageOS/android_device_qcom_sepolicy" groups="qcom,qssi" />
  <project path="device/qcom/sepolicy-legacy-um" name="LineageOS/android_device_qcom_sepolicy" groups="qcom,sdm660" revision="lineage-21.0-legacy-um" />
  <project path="device/qcom/sepolicy_vndr/legacy-um" name="LineageOS/android_device_qcom_sepolicy_vndr" groups="qcom,lahaina" revision="lineage-21.0-legacy-um" />
  <project path="device/qcom/sepolicy_vndr/sm8450" name="LineageOS/android_device_qcom_sepolicy_vndr" groups="qcom,waipio-vendor" revision="lineage-21.0-caf-sm8450" />
  <project path="device/qcom/sepolicy_vndr/sm8550" name="LineageOS/android_device_qcom_sepolicy_vndr" groups="qcom,kailua-vendor" revision="lineage-21.0-caf-sm8550" />
  <project path="external/ant-wireless/ant_client" name="LineageOS/android_external_ant-wireless_ant_client" groups="qcom" />
  <project path="external/ant-wireless/ant_native" name="LineageOS/android_external_ant-wireless_ant_native" groups="qcom" />
  <project path="external/ant-wireless/ant_service" name="LineageOS/android_external_ant-wireless_ant_service" groups="qcom" />
  <project path="external/ant-wireless/hidl" name="LineageOS/android_external_ant-wireless_hidl" groups="qcom,qssi" />
  <project path="external/json-c" name="LineageOS/android_external_json-c" groups="qcom,qssi" />
  <project path="hardware/qcom-caf/bootctrl" name="LineageOS/android_hardware_qcom_bootctrl" groups="qcom,kailua-vendor" revision="lineage-21.0-caf" />
  <project path="hardware/qcom-caf/bt" name="LineageOS/android_hardware_qcom_bt" groups="qcom,legacy-qcom" revision="lineage-21.0-caf" />
  <project path="hardware/qcom-caf/common" name="LineageOS/android_hardware_qcom-caf_common" groups="qcom" >
    <!-- add guard for AOSP hardware/qcom dir -->
    <linkfile src="os_pickup_aosp.mk" dest="hardware/qcom/Android.mk" />
    <!-- add guards for CAF repositories -->
    <linkfile src="os_pickup_sepolicy_vndr.mk" dest="device/qcom/sepolicy_vndr/SEPolicy.mk" />
    <linkfile src="os_pickup_qssi.bp" dest="hardware/qcom-caf/msm8953/Android.bp" />
    <linkfile src="os_pickup.mk" dest="hardware/qcom-caf/msm8953/Android.mk" />
    <linkfile src="os_pickup.bp" dest="hardware/qcom-caf/msm8996/Android.bp" />
    <linkfile src="os_pickup.mk" dest="hardware/qcom-caf/msm8996/Android.mk" />
    <linkfile src="os_pickup.bp" dest="hardware/qcom-caf/msm8998/Android.bp" />
    <linkfile src="os_pickup.mk" dest="hardware/qcom-caf/msm8998/Android.mk" />
    <linkfile src="os_pickup_qssi.bp" dest="hardware/qcom-caf/sdm660/Android.bp" />
    <linkfile src="os_pickup.mk" dest="hardware/qcom-caf/sdm660/Android.mk" />
    <linkfile src="os_pickup_qssi.bp" dest="hardware/qcom-caf/sdm845/Android.bp" />
    <linkfile src="os_pickup.mk" dest="hardware/qcom-caf/sdm845/Android.mk" />
    <linkfile src="os_pickup_qssi.bp" dest="hardware/qcom-caf/sm8150/Android.bp" />
    <linkfile src="os_pickup.mk" dest="hardware/qcom-caf/sm8150/Android.mk" />
    <linkfile src="os_pickup_qssi.bp" dest="hardware/qcom-caf/sm8250/Android.bp" />
    <linkfile src="os_pickup.mk" dest="hardware/qcom-caf/sm8250/Android.mk" />
    <linkfile src="os_pickup_qssi.bp" dest="hardware/qcom-caf/sm8350/Android.bp" />
    <linkfile src="os_pickup.mk" dest="hardware/qcom-caf/sm8350/Android.mk" />
    <linkfile src="os_pickup_audio-ar.mk" dest="hardware/qcom-caf/sm8450/audio/Android.mk" />
    <linkfile src="os_pickup_qssi.bp" dest="hardware/qcom-caf/sm8450/Android.bp" />
    <linkfile src="os_pickup.mk" dest="hardware/qcom-caf/sm8450/Android.mk" />
    <linkfile src="os_pickup_audio-ar.mk" dest="hardware/qcom-caf/sm8550/audio/Android.mk" />
    <linkfile src="os_pickup_qssi.bp" dest="hardware/qcom-caf/sm8550/Android.bp" />
    <linkfile src="os_pickup.mk" dest="hardware/qcom-caf/sm8550/Android.mk" />
  </project>
  <project path="hardware/qcom-caf/msm8953/audio" name="LineageOS/android_hardware_qcom_audio" groups="qcom,msm8953" revision="lineage-21.0-caf-msm8953" />
  <project path="hardware/qcom-caf/msm8953/display" name="LineageOS/android_hardware_qcom_display" groups="qcom" revision="lineage-21.0-caf-msm8953" />
  <project path="hardware/qcom-caf/msm8953/media" name="LineageOS/android_hardware_qcom_media" groups="qcom,msm8953" revision="lineage-21.0-caf-msm8953" />
  <project path="hardware/qcom-caf/msm8996/audio" name="LineageOS/android_hardware_qcom_audio" groups="qcom,legacy-qcom" revision="lineage-21.0-caf-msm8996" />
  <project path="hardware/qcom-caf/msm8996/display" name="LineageOS/android_hardware_qcom_display" groups="qcom,legacy-qcom" revision="lineage-21.0-caf-msm8996" />
  <project path="hardware/qcom-caf/msm8996/media" name="LineageOS/android_hardware_qcom_media" groups="qcom,legacy-qcom" revision="lineage-21.0-caf-msm8996" />
  <project path="hardware/qcom-caf/msm8998/audio" name="LineageOS/android_hardware_qcom_audio" groups="qcom,legacy-qcom" revision="lineage-21.0-caf-msm8998" />
  <project path="hardware/qcom-caf/msm8998/display" name="LineageOS/android_hardware_qcom_display" groups="qcom,legacy-qcom" revision="lineage-21.0-caf-msm8998" />
  <project path="hardware/qcom-caf/msm8998/media" name="LineageOS/android_hardware_qcom_media" groups="qcom,legacy-qcom" revision="lineage-21.0-caf-msm8998" />
  <project path="hardware/qcom-caf/sdm660/audio" name="LineageOS/android_hardware_qcom_audio" groups="qcom,sdm660" revision="lineage-21.0-caf-sdm660" />
  <project path="hardware/qcom-caf/sdm660/display" name="LineageOS/android_hardware_qcom_display" groups="qcom" revision="lineage-21.0-caf-sdm660" />
  <project path="hardware/qcom-caf/sdm660/media" name="LineageOS/android_hardware_qcom_media" groups="qcom,sdm660" revision="lineage-21.0-caf-sdm660" />
  <project path="hardware/qcom-caf/sdm845/audio" name="LineageOS/android_hardware_qcom_audio" groups="qcom,sdm845" revision="lineage-21.0-caf-sdm845" />
  <project path="hardware/qcom-caf/sdm845/display" name="LineageOS/android_hardware_qcom_display" groups="qcom" revision="lineage-21.0-caf-sdm845" />
  <project path="hardware/qcom-caf/sdm845/media" name="LineageOS/android_hardware_qcom_media" groups="qcom,sdm845" revision="lineage-21.0-caf-sdm845" />
  <project path="hardware/qcom-caf/sm8150/audio" name="LineageOS/android_hardware_qcom_audio" groups="qcom,msmnile" revision="lineage-21.0-caf-sm8150" />
  <project path="hardware/qcom-caf/sm8150/display" name="LineageOS/android_hardware_qcom_display" groups="qcom" revision="lineage-21.0-caf-sm8150" />
  <project path="hardware/qcom-caf/sm8150/media" name="LineageOS/android_hardware_qcom_media" groups="qcom,msmnile" revision="lineage-21.0-caf-sm8150" />
  <project path="hardware/qcom-caf/sm8250/audio" name="LineageOS/android_hardware_qcom_audio" groups="qcom,kona" revision="lineage-21.0-caf-sm8250" />
  <project path="hardware/qcom-caf/sm8250/display" name="LineageOS/android_hardware_qcom_display" groups="qcom" revision="lineage-21.0-caf-sm8250" />
  <project path="hardware/qcom-caf/sm8250/media" name="LineageOS/android_hardware_qcom_media" groups="qcom,kona" revision="lineage-21.0-caf-sm8250" />
  <project path="hardware/qcom-caf/sm8350/audio" name="LineageOS/android_hardware_qcom_audio" groups="qcom,lahaina" revision="lineage-21.0-caf-sm8350" />
  <project path="hardware/qcom-caf/sm8350/display" name="LineageOS/android_hardware_qcom_display" groups="qcom" revision="lineage-21.0-caf-sm8350" />
  <project path="hardware/qcom-caf/sm8350/media" name="LineageOS/android_hardware_qcom_media" groups="qcom,lahaina" revision="lineage-21.0-caf-sm8350" />
  <project path="hardware/qcom-caf/sm8450/audio/agm" name="LineageOS/android_vendor_qcom_opensource_agm" groups="qcom,waipio-vendor" revision="lineage-21.0-caf-sm8450" />
  <project path="hardware/qcom-caf/sm8450/audio/pal" name="LineageOS/android_vendor_qcom_opensource_arpal-lx" groups="qcom,waipio-vendor" revision="lineage-21.0-caf-sm8450" />
  <project path="hardware/qcom-caf/sm8450/audio/primary-hal" name="LineageOS/android_hardware_qcom_audio-ar" groups="qcom,waipio-vendor" revision="lineage-21.0-caf-sm8450" />
  <project path="hardware/qcom-caf/sm8450/data-ipa-cfg-mgr" name="LineageOS/android_vendor_qcom_opensource_data-ipa-cfg-mgr" groups="qcom,waipio-vendor" revision="lineage-21.0-caf-sm8450" />
  <project path="hardware/qcom-caf/sm8450/display" name="LineageOS/android_hardware_qcom_display" groups="qcom" revision="lineage-21.0-caf-sm8450" />
  <project path="hardware/qcom-caf/sm8450/media" name="LineageOS/android_hardware_qcom_media" groups="qcom,waipio-vendor" revision="lineage-21.0-caf-sm8450" />
  <project path="hardware/qcom-caf/sm8550/audio/agm" name="LineageOS/android_vendor_qcom_opensource_agm" groups="qcom,kailua-audio" revision="lineage-21.0-caf-sm8550" />
  <project path="hardware/qcom-caf/sm8550/audio/pal" name="LineageOS/android_vendor_qcom_opensource_arpal-lx" groups="qcom,kailua-audio" revision="lineage-21.0-caf-sm8550" />
  <project path="hardware/qcom-caf/sm8550/audio/primary-hal" name="LineageOS/android_hardware_qcom_audio-ar" groups="qcom,kailua-audio" revision="lineage-21.0-caf-sm8550" />
  <project path="hardware/qcom-caf/sm8550/data-ipa-cfg-mgr" name="LineageOS/android_vendor_qcom_opensource_data-ipa-cfg-mgr" groups="qcom,kailua-vendor" revision="lineage-21.0-caf-sm8550" />
  <project path="hardware/qcom-caf/sm8550/dataipa" name="LineageOS/android_vendor_qcom_opensource_dataipa" groups="qcom,kailua-vendor" revision="lineage-21.0-caf-sm8550" />
  <project path="hardware/qcom-caf/sm8550/display" name="LineageOS/android_hardware_qcom_display" groups="qcom" revision="lineage-21.0-caf-sm8550" />
  <project path="hardware/qcom-caf/sm8550/media" name="LineageOS/android_hardware_qcom_media" groups="qcom,kailua-vendor" revision="lineage-21.0-caf-sm8550" />
  <project path="hardware/qcom-caf/thermal" name="LineageOS/android_hardware_qcom_thermal" groups="qcom,kailua-vendor" />
  <project path="hardware/qcom-caf/vr" name="LineageOS/android_hardware_qcom_vr" groups="qcom,legacy-qcom" />
  <project path="hardware/qcom-caf/wlan" name="LineageOS/android_hardware_qcom_wlan" groups="qcom,kailua-vendor" revision="lineage-21.0-caf" />
  <project path="vendor/codeaurora/telephony" name="LineageOS/android_vendor_codeaurora_telephony" groups="qcom,qssi" />
  <project path="vendor/qcom/opensource/audio-hal/st-hal" name="LineageOS/android_vendor_qcom_opensource_audio-hal_st-hal" groups="qcom,lahaina" />
  <project path="vendor/qcom/opensource/audio-hal/st-hal-ar" name="LineageOS/android_vendor_qcom_opensource_audio-hal_st-hal-ar" groups="qcom,kailua-audio" />
  <project path="vendor/qcom/opensource/commonsys-intf/display" name="LineageOS/android_vendor_qcom_opensource_display-commonsys-intf" groups="qcom,qssi" />
  <project path="vendor/qcom/opensource/commonsys/display" name="LineageOS/android_vendor_qcom_opensource_display-commonsys" groups="qcom,qssi" />
  <project path="vendor/qcom/opensource/commonsys/fm" name="LineageOS/android_vendor_qcom_opensource_fm-commonsys" groups="qcom,qssi" />
  <project path="vendor/qcom/opensource/commonsys/wfd" name="LineageOS/android_vendor_qcom_opensource_wfd-commonsys" groups="qcom,kailua-video" />
  <project path="vendor/qcom/opensource/core-utils-sys" name="LineageOS/android_vendor_qcom_opensource_core-utils-sys" groups="qcom,qssi" />
  <project path="vendor/qcom/opensource/core-utils-vendor" name="LineageOS/android_vendor_qcom_opensource_core-utils-vendor" groups="qcom,kailua-vendor" />
  <project path="vendor/qcom/opensource/data-ipa-cfg-mgr-legacy-um" name="LineageOS/android_vendor_qcom_opensource_data-ipa-cfg-mgr" groups="qcom,lahaina" revision="lineage-21.0-legacy-um" />
  <project path="vendor/qcom/opensource/dataservices" name="LineageOS/android_vendor_qcom_opensource_dataservices" groups="qcom,kailua-vendor" />
  <project path="vendor/qcom/opensource/display" name="LineageOS/android_vendor_qcom_opensource_display" groups="qcom" />
  <project path="vendor/qcom/opensource/healthd-ext" name="LineageOS/android_vendor_qcom_opensource_healthd-ext" groups="qcom,kailua-vendor" />
  <project path="vendor/qcom/opensource/interfaces" name="LineageOS/android_vendor_qcom_opensource_interfaces" groups="qcom,qssi" />
  <project path="vendor/qcom/opensource/libfmjni" name="LineageOS/android_vendor_qcom_opensource_libfmjni" groups="qcom" />
  <project path="vendor/qcom/opensource/power" name="LineageOS/android_vendor_qcom_opensource_power" groups="qcom,kailua-vendor" />
  <project path="vendor/qcom/opensource/thermal-engine" name="LineageOS/android_vendor_qcom_opensource_thermal-engine" groups="qcom,kailua-vendor" />
  <project path="vendor/qcom/opensource/usb" name="LineageOS/android_vendor_qcom_opensource_usb" groups="qcom,kailua-vendor" />
  <project path="vendor/qcom/opensource/vibrator" name="LineageOS/android_vendor_qcom_opensource_vibrator" groups="qcom,kailua-vendor" />

  <!-- NXP additions -->
  <!--
  <project path="vendor/nxp/nfc" name="LineageOS/android_vendor_nxp_nfc" groups="nxp" />
  <project path="vendor/nxp/secure_element" name="LineageOS/android_vendor_nxp_secure_element" groups="nxp" />
  <project path="vendor/nxp/secure_element_extns" name="LineageOS/android_vendor_nxp_secure_element_extns" groups="nxp" />
  -->

  <!-- Infrastructure -->
  <project path="lineage/charter" name="LineageOS/charter" groups="infra" revision="main" />
  <project path="lineage/crowdin" name="LineageOS/cm_crowdin" groups="infra" revision="main" />
  <project path="lineage/hudson" name="LineageOS/hudson" groups="infra" revision="main" />
  <project path="lineage/mirror" name="LineageOS/mirror" groups="infra" revision="main" />
  <project path="lineage/website" name="LineageOS/www" groups="infra" revision="main" />
  <project path="lineage/wiki" name="LineageOS/lineage_wiki" groups="infra" revision="main" />

  <!-- Tools -->
  <project path="lineage/contributors-cloud-generator" name="LineageOS/contributors-cloud-generator" groups="tools" revision="main" />
  <project path="lineage/scripts" name="LineageOS/scripts" groups="tools" revision="main" />
</manifest>
      working-directory: <?xml version="1.0" encoding="UTF-8"?>
<manifest>

  <remote  name="cyborg"
           fetch="https://github.com/nubia-development/"           
           review="..."
           revision="lineage-21" />

  <!-- Nubia SDM660 Devices Start -->
  <!-- Nubia Z18 Mini/nx611j -->
  <project path="device/nubia/nx611j" name="android_device_nubia_nx611j" groups="device" remote="cyborg"/>
  <project path="vendor/nubia/nx611j" name="proprietary_vendor_nubia_nx611j" groups="device" remote="cyborg"/>

  <!-- Nubia SDM660 Common Kernel -->
  <project path="kernel/nubia/sdm660" name="android_kernel_nubia_sdm660" remote="cyborg"/>
  <!-- Nubia SDM660 Devices End -->

  <!-- Nubia MSM8998 Devices Start -->
  <!-- Nubia Z17/nx563j -->
  <project path="device/nubia/nx563j" name="android_device_nubia_nx563j" groups="device" remote="cyborg"/>
  <project path="vendor/nubia/nx563j" name="proprietary_vendor_nubia_nx563j" groups="device" remote="cyborg"/>

  <!-- Nubia Red Magic/nx609j -->
  <project path="device/nubia/nx609j" name="android_device_nubia_nx609j" groups="device" remote="cyborg"/>
  <project path="vendor/nubia/nx609j" name="proprietary_vendor_nubia_nx609j" groups="device" remote="cyborg"/>

  <!-- Nubia MSM8998 Common DT&VT -->
  <project path="device/nubia/msm8998-common" name="android_device_nubia_msm8998-common" groups="device" remote="cyborg"/>
  <project path="vendor/nubia/msm8998-common" name="proprietary_vendor_nubia_msm8998-common" groups="device" remote="cyborg"/>

  <!-- Nubia MSM8998 Common Kernel -->
  <project path="kernel/nubia/msm8998" name="android_kernel_nubia_msm8998" remote="cyborg"/>
  <!-- Nubia MSM8998 Devices End -->

  <!-- Nubia SDM845 Devices Start -->
  <!-- Nubia Z18/nx606j -->
  <project path="device/nubia/nx606j" name="android_device_nubia_nx606j" groups="device" remote="cyborg"/>
  <project path="vendor/nubia/nx606j" name="proprietary_vendor_nubia_nx606j" groups="device" remote="cyborg"/>

  <!-- Nubia Magic Mars/nx619j -->
  <project path="device/nubia/nx619j" name="android_device_nubia_nx619j" groups="device" remote="cyborg"/>
  <project path="vendor/nubia/nx619j" name="proprietary_vendor_nubia_nx619j" groups="device" remote="cyborg"/>

  <!-- Nubia SDM845 Common DT&VT -->
  <project path="device/nubia/sdm845-common" name="android_device_nubia_sdm845-common" groups="device" remote="cyborg"/>
  <project path="vendor/nubia/sdm845-common" name="proprietary_vendor_nubia_sdm845-common" groups="device" remote="cyborg"/>

  <!-- Nubia SDM845 Common Kernel -->
  <project path="kernel/nubia/sdm845" name="android_kernel_nubia_sdm845" remote="cyborg"/>
  <!-- Nubia SDM845 Devices End -->

  <!-- Nubia SM7250 Devices Start -->
  <!-- Nubia Play 5G/nx651j & Red Magic 5G Lite/nx651j_eea -->
  <project path="device/nubia/nx651j" name="android_device_nubia_nx651j" groups="device" remote="cyborg"/>

  <!-- Nubia SM7250 Common DT&VT -->
  <project path="device/nubia/sm7250-common" name="android_device_nubia_sm7250-common" groups="device" remote="cyborg"/>
  <project path="vendor/nubia/sm7250-common" name="proprietary_vendor_nubia_sm7250-common" groups="device" remote="cyborg"/>

  <!-- Nubia SM7250 Common Kernel -->
  <project path="kernel/nubia/sm7250" name="android_kernel_nubia_sm7250" remote="cyborg"/>
  <!-- Nubia SM7250 Devices End -->

  <!-- Nubia hardware -->
  <project path="hardware/nubia" name="LineageOS/android_hardware_nubia" remote="github" revision="lineage-21"/>

  <!-- Firmware -->
  <project path="vendor/firmware" name="proprietary_vendor_firmware" groups="device" remote="cyborg"/>

</manifest> }}
      run: nuget restore <?xml version="1.0" encoding="UTF-8"?>
<manifest>

  <!-- Pixel 6/6 Pro -->
  <remote name="aosp-raviole" fetch="https://android.googlesource.com" review="android-review.googlesource.com" revision="refs/tags/android-14.0.0_r34" clone-depth="1" />
  <!-- Pixel 6a -->
  <remote name="aosp-bluejay" fetch="https://android.googlesource.com" review="android-review.googlesource.com" revision="refs/tags/android-14.0.0_r34" clone-depth="1" />

  <!-- Pixel 7/7 Pro -->
  <remote name="aosp-pantah" fetch="https://android.googlesource.com" review="android-review.googlesource.com" revision="refs/tags/android-14.0.0_r35" clone-depth="1" />
  <!-- Pixel 7a -->
  <remote name="aosp-lynx" fetch="https://android.googlesource.com" review="android-review.googlesource.com" revision="refs/tags/android-14.0.0_r35" clone-depth="1" />
  <!-- Pixel Tablet -->
  <remote name="aosp-tangorpro" fetch="https://android.googlesource.com" review="android-review.googlesource.com" revision="refs/tags/android-14.0.0_r34" clone-depth="1" />
  <!-- Pixel Fold -->
  <remote name="aosp-felix" fetch="https://android.googlesource.com" review="android-review.googlesource.com" revision="refs/tags/android-14.0.0_r35" clone-depth="1" />

  <!-- Pixel 8/8 Pro -->
  <remote name="aosp-shusky" fetch="https://android.googlesource.com" review="android-review.googlesource.com" revision="refs/tags/android-14.0.0_r35" clone-depth="1" />

</manifest> }}

    - name: Build
      working-directory: ${{env.GITHUB_WORKSPACE}}
      # Add additional options to the MSBuild command line here (like platform or verbosity level).
      # See https://docs.microsoft.com/visualstudio/msbuild/msbuild-command-line-reference
      run: msbuild /m /p:Configuration=${{env.BUILD_CONFIGURATION}} ${{env.SOLUTION_FILE_PATH}}
