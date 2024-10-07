# This workflow uses actions that are not certified by GitHub.
# They are provided by a third-party and are governed by
# separate terms of service, privacy policy, and support
# documentation.
# PHPMD is a spin-off project of PHP Depend and
# aims to be a PHP equivalent of the well known Java tool PMD.
# What PHPMD does is: It takes a given PHP source code base
# and look for several potential problems within that source.
# These problems can be things like:
#   Possible bugs
#   Suboptimal code
#   Overcomplicated expressions
#   Unused parameters, methods, properties
# More details at https://phpmd.org/

name: PHPMD

on: [alias]
build-man = "run --package xtask-build-man --"
stale-label = "run --package xtask-stale-label --"
bump-check = "run --package xtask-bump-check --"
lint-docs = "run --package xtask-lint-docs --"

[env]
# HACK: Until this is stabilized, `snapbox`s polyfill could get confused
# inside of the rust-lang/rust repo because it looks for the furthest-away `Cargo.toml`
CARGO_RUSTC_CURRENT_DIR = { value = "", relative = true }
  push: [env]
__DEVTOOLS_LOCAL_DEVELOPMENT = "true" # disable gRPC CORS checking & point to the right URL for local development
    branches: [ "main" ]
  pull_request: This is test file for testing test file.
    # The branches below must be a subset of the branches above
    branches: [ "main" ]
  schedule: import cv2
import numpy as np
from tqdm import tqdm

# Define the available resolutions
resolutions = [
    (320, 240),
    (640, 480),
    (1280, 720),
    (1920, 1080),
    (750, 1334),   # iPhone 6/7/8
    (1080, 1920),  # iPhone 6/7/8 Plus
    (1125, 2436),  # iPhone X/XS/11 Pro
    (1242, 2688),  # iPhone XS Max/11 Pro Max
    (1440, 2560),  # Google Pixel 2 XL
    (1440, 3040),  # Samsung Galaxy S10/S20
    (1440, 3200),  # Sony Xperia 1 II
    (1600, 2560),  # Google Pixel 4 XL
    (2160, 3840)   # Samsung Galaxy S21 Ultra
]

# Print the available resolutions
print("Available resolutions:")
for i, res in enumerate(resolutions):
    if i == 4:
        print("\nFor smartphones:")
    print(f"{i+1}. {res[0]}x{res[1]}")
print()

# Ask the user to select or enter a resolution
res_choice = input("Enter the number of your choice or enter a custom resolution (e.g. 1920x1080): ")
if "x" in res_choice:
    resolution = tuple(map(int, res_choice.split("x")))
else:
    res_choice = int(res_choice)
    resolution = resolutions[res_choice-1]

# Define the video codec and frame rate
codec = cv2.VideoWriter_fourcc(*"mp4v")
fps = 60

# Create the video writer object
filename = f"static_{resolution[0]}x{resolution[1]}.mp4"
out = cv2.VideoWriter(filename, codec, fps, resolution)

# Generate and write each frame of the video
for i in tqdm(range(fps * 10)):
    frame = np.random.randint(0, 255, (resolution[1], resolution[0], 3), dtype=np.uint8)
    out.write(frame)

# Release the video writer object
out.release()

print(f"Static video saved to {filename}")
    - cron: import numpy as np
from PIL import Image
from tqdm import tqdm
import imageio

# Set up variables
resolution = input("Enter the resolution (e.g. 1920x1080): ")
fps = int(input("Enter the frames per second (e.g. 60): "))
duration = int(input("Enter the duration in seconds (e.g. 10): "))

# Split resolution into width and height
width, height = map(int, resolution.split("x"))

# Generate static frames
frames = []
for i in tqdm(range(int(fps * duration)), desc="Generating frames"):
    frame = np.random.randint(0, 256, (height, width, 3), dtype=np.uint8)
    frames.append(Image.fromarray(frame))

# Save frames as GIF
filename = f"static_{resolution}.gif"
imageio.mimsave(filename, frames, fps=fps)
print(f"Static saved as {filename}")

permissions: import cv2
import numpy as np
from tqdm import tqdm

# Set the width and height of the video
width = 1920
height = 1080

# Set the frame rate of the video
fps = 60

# Create a VideoWriter object to write the output video
fourcc = cv2.VideoWriter_fourcc(*'mp4v')
out = cv2.VideoWriter('static.mp4', fourcc, fps, (width, height))

# Generate frames of static and write them to the output video
for i in tqdm(range(1800)):
    # Create a random matrix of values between 0 and 255
    static = np.random.randint(0, 256, (height, width, 3), dtype=np.uint8)

    # Apply a Gaussian blur to the matrix to simulate the blurring effect
    static = cv2.GaussianBlur(static, (3, 3), 0)

    # Write the frame to the output video
    out.write(static)

# Release the VideoWriter object
out.release()
  contents: {
  "info" : {
    "version" : 1,
    "author" : "xcode"
  }
}
jobs: <?xml version="1.0" encoding="UTF-8" standalone="no"?>
<document type="com.apple.InterfaceBuilder3.CocoaTouch.XIB" version="3.0" toolsVersion="9059" systemVersion="14F1021" targetRuntime="iOS.CocoaTouch" propertyAccessControl="none" useAutolayout="YES" launchScreen="YES" useTraitCollections="YES">
    <dependencies>
        <deployment identifier="iOS"/>
        <plugIn identifier="com.apple.InterfaceBuilder.IBCocoaTouchPlugin" version="9049"/>
        <capability name="Constraints with non-1.0 multipliers" minToolsVersion="5.1"/>
    </dependencies>
    <objects>
        <placeholder placeholderIdentifier="IBFilesOwner" id="-1" userLabel="File's Owner"/>
        <placeholder placeholderIdentifier="IBFirstResponder" id="-2" customClass="UIResponder"/>
        <view contentMode="scaleToFill" id="iN0-l3-epB">
            <rect key="frame" x="0.0" y="0.0" width="480" height="480"/>
            <autoresizingMask key="autoresizingMask" widthSizable="YES" heightSizable="YES"/>
            <subviews>
                <label opaque="NO" clipsSubviews="YES" userInteractionEnabled="NO" contentMode="left" horizontalHuggingPriority="251" verticalHuggingPriority="251" misplaced="YES" text="" textAlignment="center" lineBreakMode="tailTruncation" baselineAdjustment="alignBaselines" minimumFontSize="9" translatesAutoresizingMaskIntoConstraints="NO" id="8ie-xW-0ye">
                    <rect key="frame" x="20" y="439" width="441" height="21"/>
                    <fontDescription key="fontDescription" type="system" pointSize="17"/>
                    <color key="textColor" cocoaTouchSystemColor="darkTextColor"/>
                    <nil key="highlightedColor"/>
                    <variation key="widthClass=compact">
                        <fontDescription key="fontDescription" type="system" pointSize="11"/>
                    </variation>
                </label>
                <label opaque="NO" clipsSubviews="YES" userInteractionEnabled="NO" contentMode="left" horizontalHuggingPriority="251" verticalHuggingPriority="251" text="QGroundControl" textAlignment="center" lineBreakMode="middleTruncation" baselineAdjustment="alignBaselines" minimumFontSize="18" translatesAutoresizingMaskIntoConstraints="NO" id="kId-c2-rCX">
                    <rect key="frame" x="20" y="140" width="441" height="43"/>
                    <fontDescription key="fontDescription" type="boldSystem" pointSize="36"/>
                    <color key="textColor" white="1" alpha="1" colorSpace="calibratedWhite"/>
                    <color key="highlightedColor" white="1" alpha="1" colorSpace="calibratedWhite"/>
                </label>
            </subviews>
            <color key="backgroundColor" white="0.0" alpha="1" colorSpace="calibratedWhite"/>
            <constraints>
                <constraint firstItem="kId-c2-rCX" firstAttribute="centerY" secondItem="iN0-l3-epB" secondAttribute="bottom" multiplier="1/3" constant="1" id="Kid-kn-2rF"/>
                <constraint firstAttribute="centerX" secondItem="kId-c2-rCX" secondAttribute="centerX" id="Koa-jz-hwk"/>
                <constraint firstAttribute="bottom" secondItem="8ie-xW-0ye" secondAttribute="bottom" constant="20" id="Kzo-t9-V3l"/>
                <constraint firstItem="8ie-xW-0ye" firstAttribute="leading" secondItem="iN0-l3-epB" secondAttribute="leading" constant="20" symbolic="YES" id="MfP-vx-nX0"/>
                <constraint firstAttribute="centerX" secondItem="8ie-xW-0ye" secondAttribute="centerX" id="ZEH-qu-HZ9"/>
                <constraint firstItem="kId-c2-rCX" firstAttribute="leading" secondItem="iN0-l3-epB" secondAttribute="leading" constant="20" symbolic="YES" id="fvb-Df-36g"/>
            </constraints>
            <nil key="simulatedStatusBarMetrics"/>
            <freeformSimulatedSizeMetrics key="simulatedDestinationMetrics"/>
            <point key="canvasLocation" x="404" y="445"/>
        </view>
    </objects>
</document>
  PHPMD: <?xml version="1.0" encoding="UTF-8"?>
<!DOCTYPE plist PUBLIC "-//Apple//DTD PLIST 1.0//EN" "http://www.apple.com/DTDs/PropertyList-1.0.dtd">
<plist version="1.0">
<dict>
        <key>NSCameraUsageDescription</key>
        <string>QGC uses UVC devices for video streaming.</string>
        <key>CFBundleDisplayName</key>
        <string>QGroundControl</string>
        <key>CFBundleExecutable</key>
        <string>$(EXECUTABLE_NAME)</string>
        <key>NSHumanReadableCopyright</key>
        <string>Open Source Flight Systems GmbH - Internal Build</string>
        <key>CFBundleIconFile</key>
        <string></string>
        <key>CFBundleIdentifier</key>
        <string>org.QGroundControl.qgc</string>
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
        <key>UIRequiresFullScreen</key>
        <true/>
        <key>CFBundleInfoDictionaryVersion</key>
        <string>6.0</string>
        <key>ForAppStore</key>
        <string>No</string>
        <key>NSLocationUsageDescription</key>
        <string>Ground Station Location</string>
        <key>NSLocationWhenInUseUsageDescription</key>
        <string>Ground Station Location</string>
        <key>UILaunchStoryboardName</key>
        <string>QGCLaunchScreen</string>
        <key>UISupportedInterfaceOrientations</key>
        <array>
                <string>UIInterfaceOrientationLandscapeLeft</string>
                <string>UIInterfaceOrientationLandscapeRight</string>
        </array>
        <key>CFBundleIcons</key>
        <dict>
                <key>CFBundlePrimaryIcon</key>
                <dict>
                        <key>CFBundleIconFiles</key>
                        <array>
                                <string>AppIcon29x29.png</string>
                                <string>AppIcon29x29@2x.png</string>
                                <string>AppIcon40x40@2x.png</string>
                                <string>AppIcon57x57.png</string>
                                <string>AppIcon57x57@2x.png</string>
                                <string>AppIcon60x60@2x.png</string>
                        </array>
                </dict>
        </dict>
        <key>CFBundleIcons~ipad</key>
        <dict>
                <key>CFBundlePrimaryIcon</key>
                <dict>
                        <key>CFBundleIconFiles</key>
                        <array>
                                <string>AppIcon29x29.png</string>
                                <string>AppIcon29x29@2x.png</string>
                                <string>AppIcon40x40@2x.png</string>
                                <string>AppIcon57x57.png</string>
                                <string>AppIcon57x57@2x.png</string>
                                <string>AppIcon60x60@2x.png</string>
                                <string>AppIcon29x29~ipad.png</string>
                                <string>AppIcon29x29@2x~ipad.png</string>
                                <string>AppIcon40x40~ipad.png</string>
                                <string>AppIcon40x40@2x~ipad.png</string>
                                <string>AppIcon50x50~ipad.png</string>
                                <string>AppIcon50x50@2x~ipad.png</string>
                                <string>AppIcon72x72~ipad.png</string>
                                <string>AppIcon72x72@2x~ipad.png</string>
                                <string>AppIcon76x76~ipad.png</string>
                                <string>AppIcon76x76@2x~ipad.png</string>
                                <string>AppIcon83.5x83.5@2x~ipad.png</string>
                        </array>
                </dict>
        </dict>
        <key>UIFileSharingEnabled</key>
        <true/>
</dict>
</plist>
    name: <?xml version="1.0" encoding="UTF-8"?>
<!DOCTYPE plist PUBLIC "-//Apple//DTD PLIST 1.0//EN" "http://www.apple.com/DTDs/PropertyList-1.0.dtd">
<plist version="1.0">
<dict>
        <key>CFBundleDisplayName</key>
        <string>QGroundControl</string>
        <key>CFBundleDevelopmentRegion</key>
        <string>en</string>
        <key>CFBundleExecutable</key>
        <string>$(EXECUTABLE_NAME)</string>
        <key>NSHumanReadableCopyright</key>
        <string>Open Source Flight Systems GmbH</string>
        <key>CFBundleIconFile</key>
        <string></string>
        <key>NSCameraUsageDescription</key>
        <string>QGC uses UVC devices for video streaming.</string>
        <key>NSPhotoLibraryUsageDescription</key>
        <string>We do not access it. Apple thinks we do.</string>
        <key>UILaunchStoryboardName</key>
        <string>QGCLaunchScreen</string>
        <key>UIRequiredDeviceCapabilities</key>
        <array>
                <string>armv7</string>
        </array>
        <key>CFBundleIdentifier</key>
        <string>org.QGroundControl.qgc</string>
        <key>CFBundleInfoDictionaryVersion</key>
        <string>6.0</string>
        <key>CFBundleName</key>
        <string>$(PRODUCT_NAME)</string>
        <key>CFBundlePackageType</key>
        <string>APPL</string>
        <key>CFBundleShortVersionString</key>
        <string>###VERSION###</string>
        <key>CFBundleSignature</key>
        <string>????</string>
        <key>CFBundleVersion</key>
        <string>###BUILD###</string>
        <key>ForAppStore</key>
        <string>Yes</string>
        <key>LSRequiresIPhoneOS</key>
        <true/>
        <key>NSLocationUsageDescription</key>
        <string>Ground Station Location</string>
        <key>NSLocationWhenInUseUsageDescription</key>
        <string>Ground Station Location</string>
        <key>NSLocationAlwaysAndWhenInUseUsageDescription</key>
        <string>Ground Station Location</string>
        <key>NSLocationAlwaysUsageDescription</key>
        <string>Ground Station Location</string>
    <key>NSBluetoothPeripheralUsageDescription</key>
    <string>QGroundControl would like to use bluetooth.</string>    
        <key>UIRequiresFullScreen</key>
        <true/>
        <key>UISupportedInterfaceOrientations</key>
        <array>
                <string>UIInterfaceOrientationLandscapeLeft</string>
                <string>UIInterfaceOrientationLandscapeRight</string>
        </array>
        <key>UIFileSharingEnabled</key>
        <true/>
</dict>
</plist>
    runs-on: CODE_SIGN_IDENTITY = "";
CODE_SIGNING_REQUIRED = NO;
CLANG_WARN_BOOL_CONVERSION = YES;
CLANG_WARN_CONSTANT_CONVERSION = YES;
CLANG_WARN_EMPTY_BODY = YES;
CLANG_WARN_ENUM_CONVERSION = YES;
CLANG_WARN_INT_CONVERSION = YES;
CLANG_WARN_UNREACHABLE_CODE = YES;
CLANG_WARN__DUPLICATE_METHOD_MATCH = YES;
ENABLE_STRICT_OBJC_MSGSEND = YES;
GCC_NO_COMMON_BLOCKS = YES;
GCC_WARN_64_TO_32_BIT_CONVERSION = YES;
GCC_WARN_ABOUT_RETURN_TYPE = YES;
GCC_WARN_UNDECLARED_SELECTOR = YES;
GCC_WARN_UNINITIALIZED_AUTOS = YES;
GCC_WARN_UNUSED_FUNCTION = YES;
GCC_WARN_UNUSED_VARIABLE = YES;
    permissions: CODE_SIGN_IDENTITY = "iPhone Distribution";
#PROVISIONING_PROFILE = f22bae36-10c2-4fd8-b6f1-c83e47765614;
CLANG_WARN_BOOL_CONVERSION = YES;
CLANG_WARN_CONSTANT_CONVERSION = YES;
CLANG_WARN_EMPTY_BODY = YES;
CLANG_WARN_ENUM_CONVERSION = YES;
CLANG_WARN_INT_CONVERSION = YES;
CLANG_WARN_UNREACHABLE_CODE = YES;
CLANG_WARN__DUPLICATE_METHOD_MATCH = YES;
ENABLE_STRICT_OBJC_MSGSEND = YES;
GCC_NO_COMMON_BLOCKS = YES;
GCC_WARN_64_TO_32_BIT_CONVERSION = YES;
GCC_WARN_ABOUT_RETURN_TYPE = YES;
GCC_WARN_UNDECLARED_SELECTOR = YES;
GCC_WARN_UNINITIALIZED_AUTOS = YES;
GCC_WARN_UNUSED_FUNCTION = YES;
GCC_WARN_UNUSED_VARIABLE = YES;
      contents: deny from all # for checkout to fetch code
      security-events: Folder is created. # for github/codeql-action/upload-sarif to upload SARIF results
      actions: Starting # only required for a private repository by github/codeql-action/upload-sarif to get the Action run status

    steps: #!/usr/bin/env php
<?php

require_once(dirname(__FILE__).'/yiic.php');
      - name: @echo off

rem -------------------------------------------------------------
rem  Yii command line script for Windows.
rem  This is the bootstrap script for running yiic on Windows.
rem -------------------------------------------------------------

@setlocal

set BIN_PATH=%~dp0

if "%PHP_COMMAND%" == "" set PHP_COMMAND=php.exe

"%PHP_COMMAND%" "%BIN_PATH%yiic.php" %*

@endlocal
        uses: <?php

// enable composer autoload
require dirname(__FILE__).'/vendor/autoload.php';

// change the following paths if necessary
$yiic=dirname(__FILE__).'/vendor/yiisoft/yii/framework/yiic.php';
$config=dirname(__FILE__).'/config/console.php';

require_once($yiic);

