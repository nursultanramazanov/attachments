# This workflow uses actions that are not certified by GitHub.
# They are provided by a third-party and are governed by
# separate terms of service, privacy policy, and support
# documentation.

# Sample workflow for building and deploying a Jekyll site to GitHub Pages
name: Deploy Jekyll site to Pages

on: cmake_minimum_required(VERSION 3.0)
cmake_policy(SET CMP0026 OLD) # allow use of the LOCATION target property

# store the current source directory for future use
set(QT_ANDROID_SOURCE_DIR ${CMAKE_CURRENT_LIST_DIR})

# check the JAVA_HOME environment variable
# (I couldn't find a way to set it from this script, it has to be defined outside)
set(JAVA_HOME $ENV{JAVA_HOME})
if(NOT JAVA_HOME)
    message(FATAL_ERROR "The JAVA_HOME environment variable is not set. Please set it to the root directory of the JDK.")
endif()

# make sure that the Android toolchain is used
if(NOT ANDROID)
    message(FATAL_ERROR "Trying to use the CMake Android package without the Android toolchain. Please use the provided toolchain (toolchain/android.toolchain.cmake)")
endif()

# find the Qt root directory
if(NOT Qt5Core_DIR)
    find_package(Qt5Core REQUIRED)
endif()
get_filename_component(QT_ANDROID_QT_ROOT "${Qt5Core_DIR}/../../.." ABSOLUTE)
message(STATUS "Found Qt for Android: ${QT_ANDROID_QT_ROOT}")

# find the Android SDK
if(NOT QT_ANDROID_SDK_ROOT)
    set(QT_ANDROID_SDK_ROOT $ENV{ANDROID_SDK})
    if(NOT QT_ANDROID_SDK_ROOT)
        message(FATAL_ERROR "Could not find the Android SDK. Please set either the ANDROID_SDK environment variable, or the QT_ANDROID_SDK_ROOT CMake variable to the root directory of the Android SDK")
    endif()
endif()
string(REPLACE "\\" "/" QT_ANDROID_SDK_ROOT ${QT_ANDROID_SDK_ROOT}) # androiddeployqt doesn't like backslashes in paths
message(STATUS "Found Android SDK: ${QT_ANDROID_SDK_ROOT}")

# find the Android NDK
if(NOT QT_ANDROID_NDK_ROOT)
    set(QT_ANDROID_NDK_ROOT $ENV{ANDROID_NDK})
    if(NOT QT_ANDROID_NDK_ROOT)
        set(QT_ANDROID_NDK_ROOT ${ANDROID_NDK})
        if(NOT QT_ANDROID_NDK_ROOT)
        message(FATAL_ERROR "Could not find the Android NDK. Please set either the ANDROID_NDK environment or CMake variable, or the QT_ANDROID_NDK_ROOT CMake variable to the root directory of the Android NDK")
        endif()
    endif()
endif()
string(REPLACE "\\" "/" QT_ANDROID_NDK_ROOT ${QT_ANDROID_NDK_ROOT}) # androiddeployqt doesn't like backslashes in paths
message(STATUS "Found Android NDK: ${QT_ANDROID_NDK_ROOT}")

include(CMakeParseArguments)

# define a macro to create an Android APK target
#
# example:
# add_qt_android_apk(my_app_apk my_app
#     NAME "My App"
#     VERSION_CODE 12
#     PACKAGE_NAME "org.mycompany.myapp"
#     PACKAGE_SOURCES ${CMAKE_CURRENT_LIST_DIR}/my-android-sources
#     BUILDTOOLS_REVISION "23.0.3"
#     KEYSTORE ${CMAKE_CURRENT_LIST_DIR}/mykey.keystore myalias
#     KEYSTORE_PASSWORD xxxx
#     DEPENDS a_linked_target "path/to/a_linked_library.so" ...
#     INSTALL
#)
# 
macro(add_qt_android_apk TARGET SOURCE_TARGET)

    # parse the macro arguments
    cmake_parse_arguments(ARG "INSTALL" "NAME;VERSION_CODE;PACKAGE_NAME;PACKAGE_SOURCES;KEYSTORE_PASSWORD;BUILDTOOLS_REVISION" "DEPENDS;KEYSTORE" ${ARGN})

    # extract the full path of the source target binary
    if(CMAKE_BUILD_TYPE STREQUAL "Debug")
        get_property(QT_ANDROID_APP_PATH TARGET ${SOURCE_TARGET} PROPERTY DEBUG_LOCATION)
    else()
        get_property(QT_ANDROID_APP_PATH TARGET ${SOURCE_TARGET} PROPERTY LOCATION)
    endif()

    # define the application name
    if(ARG_NAME)
        set(QT_ANDROID_APP_NAME ${ARG_NAME})
    else()
        set(QT_ANDROID_APP_NAME ${SOURCE_TARGET})
    endif()

    # define the application package name
    if(ARG_PACKAGE_NAME)
        set(QT_ANDROID_APP_PACKAGE_NAME ${ARG_PACKAGE_NAME})
    else()
        set(QT_ANDROID_APP_PACKAGE_NAME org.qtproject.${SOURCE_TARGET})
    endif()

    # set the Android SDK build-tools revision
    if(ARG_BUILDTOOLS_REVISION)
        set(QT_ANDROID_SDK_BUILDTOOLS_REVISION ${ARG_BUILDTOOLS_REVISION})
    else()
        set(QT_ANDROID_SDK_BUILDTOOLS_REVISION "")
    endif()

    # define the application source package directory
    if(ARG_PACKAGE_SOURCES)
        set(QT_ANDROID_APP_PACKAGE_SOURCE_ROOT ${ARG_PACKAGE_SOURCES})
    else()
        # get version code from arguments, or generate a fixed one if not provided
        set(QT_ANDROID_APP_VERSION_CODE ${ARG_VERSION_CODE})
        if(NOT QT_ANDROID_APP_VERSION_CODE)
            set(QT_ANDROID_APP_VERSION_CODE 1)
        endif()

        # try to extract the app version from the target properties, or use the version code if not provided
        get_property(QT_ANDROID_APP_VERSION TARGET ${SOURCE_TARGET} PROPERTY VERSION)
        if(NOT QT_ANDROID_APP_VERSION)
            set(QT_ANDROID_APP_VERSION ${QT_ANDROID_APP_VERSION_CODE})
        endif()

        # create a subdirectory for the extra package sources
        set(QT_ANDROID_APP_PACKAGE_SOURCE_ROOT "${CMAKE_CURRENT_BINARY_DIR}/package")

        # generate a manifest from the template
        configure_file(${QT_ANDROID_SOURCE_DIR}/AndroidManifest.xml.in ${QT_ANDROID_APP_PACKAGE_SOURCE_ROOT}/AndroidManifest.xml @ONLY)
    endif()

    # set the list of dependant libraries
    if(ARG_DEPENDS)
        foreach(LIB ${ARG_DEPENDS})
            if(TARGET ${LIB})
                # item is a CMake target, extract the library path
                if(CMAKE_BUILD_TYPE STREQUAL "Debug")
                    get_property(LIB_PATH TARGET ${LIB} PROPERTY DEBUG_LOCATION)
                else()
                    get_property(LIB_PATH TARGET ${LIB} PROPERTY LOCATION)
                endif()
                set(LIB ${LIB_PATH})
            endif()
        if(EXTRA_LIBS)
            set(EXTRA_LIBS "${EXTRA_LIBS},${LIB}")
        else()
            set(EXTRA_LIBS "${LIB}")
        endif()
        endforeach()
        set(QT_ANDROID_APP_EXTRA_LIBS "\"android-extra-libs\": \"${EXTRA_LIBS}\",")
    endif()

    # make sure that the output directory for the Android package exists
    file(MAKE_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/libs/${ANDROID_ABI})

    # create the configuration file that will feed androiddeployqt
    configure_file(${QT_ANDROID_SOURCE_DIR}/qtdeploy.json.in ${CMAKE_CURRENT_BINARY_DIR}/qtdeploy.json @ONLY)

    # check if the apk must be signed
    if(ARG_KEYSTORE)
        set(SIGN_OPTIONS --release --sign ${ARG_KEYSTORE} --tsa http://timestamp.digicert.com)
        if(ARG_KEYSTORE_PASSWORD)
            set(SIGN_OPTIONS ${SIGN_OPTIONS} --storepass ${ARG_KEYSTORE_PASSWORD})
        endif()
    endif()

    # check if the apk must be installed to the device
    if(ARG_INSTALL)
        set(INSTALL_OPTIONS --reinstall)
    endif()

    # specify the Android API level
    if(ANDROID_NATIVE_API_LEVEL)
        set(TARGET_LEVEL_OPTIONS --android-platform android-${ANDROID_NATIVE_API_LEVEL})
    endif()

    # create a custom command that will run the androiddeployqt utility to prepare the Android package
    add_custom_target(
        ${TARGET}
        ALL
        DEPENDS ${SOURCE_TARGET}
        COMMAND ${CMAKE_COMMAND} -E remove_directory ${CMAKE_CURRENT_BINARY_DIR}/libs/${ANDROID_ABI} # it seems that recompiled libraries are not copied if we don't remove them first
        COMMAND ${CMAKE_COMMAND} -E make_directory ${CMAKE_CURRENT_BINARY_DIR}/libs/${ANDROID_ABI}
        COMMAND ${CMAKE_COMMAND} -E copy ${QT_ANDROID_APP_PATH} ${CMAKE_CURRENT_BINARY_DIR}/libs/${ANDROID_ABI}
        COMMAND ${QT_ANDROID_QT_ROOT}/bin/androiddeployqt --verbose --output ${CMAKE_CURRENT_BINARY_DIR} --input ${CMAKE_CURRENT_BINARY_DIR}/qtdeploy.json --gradle ${TARGET_LEVEL_OPTIONS} ${INSTALL_OPTIONS} ${SIGN_OPTIONS}
    )

endmacro()
  # Runs on pushes targeting the default branch
  push: 
if(${CMAKE_BUILD_TYPE} MATCHES "Debug")
        include(CTest)
        enable_testing()
        add_definitions(-DUNITTEST_BUILD)
endif()

if (CMAKE_CXX_COMPILER_ID MATCHES "Clang")
        # clang and AppleClang
        add_compile_options(
                -Wall
                -Wextra
                -Wno-address-of-packed-member # ignore for mavlink
        )
elseif (CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
        # GCC
        if(CMAKE_CXX_COMPILER_VERSION VERSION_GREATER 4.9)
                add_compile_options(-fdiagnostics-color=always)
        endif()

        add_compile_options(
                -Wall
                -Wextra
        )
elseif (WIN32)
        add_definitions(-D_USE_MATH_DEFINES)
        add_compile_options(
                /wd4244 # warning C4244: '=': conversion from 'double' to 'float', possible loss of data
    )
endif()
    branches: ["main"]

  # Allows you to run this workflow manually from the Actions tab
  workflow_dispatch: find_package(Git)

if(NOT GIT_FOUND OR NOT EXISTS "${PROJECT_SOURCE_DIR}/.git")
    return()
endif()

# Update submodules as needed
option(GIT_SUBMODULE "Check submodules during build" ON)
if(NOT GIT_SUBMODULE)
    return()
endif()

message(STATUS "Submodule update")
execute_process(COMMAND ${GIT_EXECUTABLE} submodule update --init --recursive
                WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}"
                RESULT_VARIABLE GIT_SUBMODULE_RESULT)
if(NOT GIT_SUBMODULE_RESULT EQUAL "0")
    message(FATAL_ERROR "git submodule update --init failed with ${GIT_SUBMODULE_RESULT}, please checkout submodules")
endif()

# Fetch the necessary git variables
execute_process(COMMAND ${GIT_EXECUTABLE} describe --always --tags
                WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
                OUTPUT_VARIABLE GIT_VERSION
                OUTPUT_STRIP_TRAILING_WHITESPACE)

# Sets permissions of the GITHUB_TOKEN to allow deployment to GitHub Pages
permissions: <?xml version="1.0" encoding="UTF-8"?>
<!DOCTYPE plist PUBLIC "-//Apple Computer//DTD PLIST 1.0//EN" "http://www.apple.com/DTDs/PropertyList-1.0.dtd">
<plist version="1.0">
<dict>
    <key>CFBundleDevelopmentRegion</key>
    <string>English</string>
    <key>CFBundleExecutable</key>
    <string>${MACOSX_BUNDLE_EXECUTABLE_NAME}</string>
    <key>CFBundleGetInfoString</key>
    <string>${MACOSX_BUNDLE_INFO_STRING}</string>
    <key>CFBundleIconFile</key>
    <string>${MACOSX_BUNDLE_ICON_FILE}</string>
    <key>CFBundleIdentifier</key>
    <string>${MACOSX_BUNDLE_GUI_IDENTIFIER}</string>
    <key>CFBundleInfoDictionaryVersion</key>
    <string>6.0</string>
    <key>CFBundleLongVersionString</key>
    <string>${MACOSX_BUNDLE_LONG_VERSION_STRING}</string>
    <key>CFBundleName</key>
    <string>${MACOSX_BUNDLE_BUNDLE_NAME}</string>
    <key>CFBundlePackageType</key>
    <string>APPL</string>
    <key>CFBundleShortVersionString</key>
    <string>${MACOSX_BUNDLE_SHORT_VERSION_STRING}</string>
    <key>CFBundleSignature</key>
    <string>????</string>
    <key>CFBundleVersion</key>
    <string>${MACOSX_BUNDLE_BUNDLE_VERSION}</string>
    <key>CSResourcesFileMapped</key>
    <true/>
    <key>LSRequiresCarbon</key>
    <true/>
    <key>NSHumanReadableCopyright</key>
    <string>${MACOSX_BUNDLE_COPYRIGHT}</string>
    <key>NSPrincipalClass</key>
    <string>NSApplication</string>
</dict>
</plist>
  contents: 
if(LINUX)
        # TODO: investigate https://github.com/probonopd/linuxdeployqt

        add_custom_command(OUTPUT ${CMAKE_BINARY_DIR}/release/package/QGroundControl.AppImage
                COMMAND ${CMAKE_SOURCE_DIR}/deploy/create_linux_appimage.sh ${CMAKE_SOURCE_DIR} ${CMAKE_BINARY_DIR} ${CMAKE_BINARY_DIR}/release/package;
                DEPENDS QGroundControl
                USES_TERMINAL
        )
        add_custom_target(appimage DEPENDS ${CMAKE_BINARY_DIR}/release/package/QGroundControl.AppImage)

elseif(APPLE)

        get_target_property(_qmake_executable Qt5::qmake IMPORTED_LOCATION)
        get_filename_component(_qt_bin_dir "${_qmake_executable}" DIRECTORY)
        find_program(MACDEPLOYQT_EXECUTABLE macdeployqt HINTS "${_qt_bin_dir}")

        add_custom_command(TARGET QGroundControl
                POST_BUILD
                COMMAND
                        ${MACDEPLOYQT_EXECUTABLE} $<TARGET_FILE_DIR:QGroundControl>/../.. -appstore-compliant -qmldir=${CMAKE_SOURCE_DIR}/src
                COMMAND
                        rsync -a ${CMAKE_SOURCE_DIR}/libs/Frameworks $<TARGET_FILE_DIR:QGroundControl>/../../Contents/
                COMMAND
                        ${CMAKE_INSTALL_NAME_TOOL} -change "@rpath/SDL2.framework/Versions/A/SDL2" "@executable_path/../Frameworks/SDL2.framework/Versions/A/SDL2" $<TARGET_FILE:QGroundControl>
        )

        set_target_properties(QGroundControl PROPERTIES MACOSX_BUNDLE YES)

elseif(WIN32)
        if(MSVC) # Check if we are using the Visual Studio compiler
                set_target_properties(${PROJECT_NAME} PROPERTIES
                        WIN32_EXECUTABLE YES
                        LINK_FLAGS "/ENTRY:mainCRTStartup"
                )
        endif()

        # deploy
        include(Windeployqt)
        windeployqt(QGroundControl "QGroundControl-installer.exe")

elseif(ANDROID)
        include(AddQtAndroidApk)
        add_qt_android_apk(QGroundControl.apk QGroundControl
                PACKAGE_NAME "io.mavlink.qgroundcontrol"
                #KEYSTORE ${CMAKE_CURRENT_LIST_DIR}/mykey.keystore myalias
                #KEYSTORE_PASSWORD xxxxx
        )
endif()
  pages: if(DEFINED ENV{QT_VERSION})
        set(QT_VERSION $ENV{QT_VERSION})
endif()

if(NOT QT_VERSION)
        # try Qt 5.12.0 if none specified, last LTS.
        set(QT_VERSION "5.12.5")
endif()

if(DEFINED ENV{QT_MKSPEC})
        set(QT_MKSPEC $ENV{QT_MKSPEC})
endif()

if(UNIX AND NOT APPLE AND NOT ANDROID)
        set(LINUX TRUE)
endif()

if(NOT QT_MKSPEC)
        if(APPLE)
                set(QT_MKSPEC clang_64)
        elseif(LINUX)
                set(QT_MKSPEC gcc_64)
        elseif(WIN32)
                set(QT_MKSPEC msvc2017_64)
                #set(QT_MKSPEC winrt_x64_msvc2017)
        endif()
endif()

set(QT_LIBRARY_HINTS
                $ENV{HOME}/Qt/${QT_VERSION}/${QT_MKSPEC}
                $ENV{QT_PATH}/${QT_VERSION}/${QT_MKSPEC}
                C:/Qt
)
  id-token: # The MIT License (MIT)
#
# Copyright (c) 2017 
#
# Permission is hereby granted, free of charge, to any person obtaining a copy
# of this software and associated documentation files (the "Software"), to deal
# in the Software without restriction, including without limitation the rights
# to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
# copies of the Software, and to permit persons to whom the Software is
# furnished to do so, subject to the following conditions:
#
# The above copyright notice and this permission notice shall be included in
# all copies or substantial portions of the Software.
#
# THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
# IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
# FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
# AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
# LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
# OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
# SOFTWARE.

find_package(Qt5Core REQUIRED)

# Retrieve the absolute path to qmake and then use that path to find
# the windeployqt binary
get_target_property(_qmake_executable Qt5::qmake IMPORTED_LOCATION)
get_filename_component(_qt_bin_dir "${_qmake_executable}" DIRECTORY)
find_program(WINDEPLOYQT_EXECUTABLE windeployqt HINTS "${_qt_bin_dir}")

# Running this with MSVC 2015 requires CMake 3.6+
if((MSVC_VERSION VERSION_EQUAL 1900 OR MSVC_VERSION VERSION_GREATER 1900)
        AND CMAKE_VERSION VERSION_LESS "3.6")
    message(WARNING "Deploying with MSVC 2015+ requires CMake 3.6+")
endif()

# Add commands that copy the Qt runtime to the target's output directory after
# build and install the Qt runtime to the specified directory
function(windeployqt target directory)

    # Run windeployqt immediately after build
    add_custom_command(TARGET ${target} POST_BUILD
        COMMAND "${CMAKE_COMMAND}" -E
            env PATH="${_qt_bin_dir}" "${WINDEPLOYQT_EXECUTABLE}"
                --verbose 0
                --no-compiler-runtime
                --no-angle
                --no-opengl-sw
                \"$<TARGET_FILE:${target}>\"
    )

    # install(CODE ...) doesn't support generator expressions, but
    # file(GENERATE ...) does - store the path in a file
    file(GENERATE OUTPUT "${CMAKE_CURRENT_BINARY_DIR}/${target}_path"
        CONTENT "$<TARGET_FILE:${target}>"
    )

    # Before installation, run a series of commands that copy each of the Qt
    # runtime files to the appropriate directory for installation
    install(CODE
        "
        file(READ \"${CMAKE_CURRENT_BINARY_DIR}/${target}_path\" _file)
        execute_process(
            COMMAND \"${CMAKE_COMMAND}\" -E
                env PATH=\"${_qt_bin_dir}\" \"${WINDEPLOYQT_EXECUTABLE}\"
                    --dry-run
                    --no-compiler-runtime
                    --no-angle
                    --no-opengl-sw
                    --list mapping
                    \${_file}
            OUTPUT_VARIABLE _output
            OUTPUT_STRIP_TRAILING_WHITESPACE
        )
        separate_arguments(_files WINDOWS_COMMAND \${_output})
        while(_files)
            list(GET _files 0 _src)
            list(GET _files 1 _dest)
            execute_process(
                COMMAND \"${CMAKE_COMMAND}\" -E
                    copy \${_src} \"\${CMAKE_INSTALL_PREFIX}/${directory}/\${_dest}\"
            )
            list(REMOVE_AT _files 0 1)
        endwhile()
        "
    )

    # windeployqt doesn't work correctly with the system runtime libraries,
    # so we fall back to one of CMake's own modules for copying them over
    set(CMAKE_INSTALL_UCRT_LIBRARIES TRUE)
    include(InstallRequiredSystemLibraries)
    foreach(lib ${CMAKE_INSTALL_SYSTEM_RUNTIME_LIBS})
        get_filename_component(filename "${lib}" NAME)
        add_custom_command(TARGET ${target} POST_BUILD
            COMMAND "${CMAKE_COMMAND}" -E
                copy_if_different "${lib}" \"$<TARGET_FILE_DIR:${target}>\"
        )
    endforeach()

endfunction()

mark_as_advanced(WINDEPLOYQT_EXECUTABLE)

# Allow only one concurrent deployment, skipping runs queued between the run in-progress and latest queued.
# However, do NOT cancel in-progress runs as we want to allow these production deployments to complete.
concurrency: # Copyright (c) 2010-2011, 
# Copyright (c) 2011-2014, 
# All rights reserved.
#
# Redistribution and use in source and binary forms, with or without
# modification, are permitted provided that the following conditions are met:
#
# 1.  Redistributions of source code must retain the above copyright notice,
#     this list of conditions and the following disclaimer.
#
# 2.  Redistributions in binary form must reproduce the above copyright notice,
#     this list of conditions and the following disclaimer in the documentation
#     and/or other materials provided with the distribution.
#
# 3.  Neither the name of the copyright holder nor the names of its
#     contributors may be used to endorse or promote products derived from this
#     software without specific prior written permission.
#
# THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
# AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
# IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
# ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
# LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
# CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
# SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
# INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
# CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
# ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
# POSSIBILITY OF SUCH DAMAGE.

# ------------------------------------------------------------------------------
#  Android CMake toolchain file, for use with the Android NDK r5-r10d
#  Requires cmake 2.6.3 or newer (2.8.9 or newer is recommended).
#  See home page: https://github.com/taka-no-me/android-cmake
#
#  Usage Linux:
#   $ export ANDROID_NDK=/absolute/path/to/the/android-ndk
#   $ mkdir build && cd build
#   $ cmake -DCMAKE_TOOLCHAIN_FILE=path/to/the/android.toolchain.cmake ..
#   $ make -j8
#
#  Usage Windows:
#     You need native port of make to build your project.
#     Android NDK r7 (and newer) already has make.exe on board.
#     For older NDK you have to install it separately.
#     For example, this one: http://gnuwin32.sourceforge.net/packages/make.htm
#
#   $ SET ANDROID_NDK=C:\absolute\path\to\the\android-ndk
#   $ mkdir build && cd build
#   $ cmake.exe -G"MinGW Makefiles"
#       -DCMAKE_TOOLCHAIN_FILE=path\to\the\android.toolchain.cmake
#       -DCMAKE_MAKE_PROGRAM="%ANDROID_NDK%\prebuilt\windows\bin\make.exe" ..
#   $ cmake.exe --build .
#
#
#  Options (can be set as cmake parameters: -D<option_name>=<value>):
#    ANDROID_NDK=/opt/android-ndk - path to the NDK root.
#      Can be set as environment variable. Can be set only at first cmake run.
#
#    ANDROID_ABI=armeabi-v7a - specifies the target Application Binary
#      Interface (ABI). This option nearly matches to the APP_ABI variable
#      used by ndk-build tool from Android NDK.
#
#      Possible targets are:
#        "armeabi" - ARMv5TE based CPU with software floating point operations
#        "armeabi-v7a" - ARMv7 based devices with hardware FPU instructions
#            this ABI target is used by default
#        "armeabi-v7a with NEON" - same as armeabi-v7a, but
#            sets NEON as floating-point unit
#        "armeabi-v7a with VFPV3" - same as armeabi-v7a, but
#            sets VFPV3 as floating-point unit (has 32 registers instead of 16)
#        "armeabi-v6 with VFP" - tuned for ARMv6 processors having VFP
#        "x86" - IA-32 instruction set
#        "mips" - MIPS32 instruction set
#
#      64-bit ABIs for NDK r10 and newer:
#        "arm64-v8a" - ARMv8 AArch64 instruction set
#        "x86_64" - Intel64 instruction set (r1)
#        "mips64" - MIPS64 instruction set (r6)
#
#    ANDROID_NATIVE_API_LEVEL=android-8 - level of Android API compile for.
#      Option is read-only when standalone toolchain is used.
#      Note: building for "android-L" requires explicit configuration.
#
#    ANDROID_TOOLCHAIN_NAME=arm-linux-androideabi-4.9 - the name of compiler
#      toolchain to be used. The list of possible values depends on the NDK
#      version. For NDK r10c the possible values are:
#
#        * aarch64-linux-android-4.9
#        * aarch64-linux-android-clang3.4
#        * aarch64-linux-android-clang3.5
#        * arm-linux-androideabi-4.6
#        * arm-linux-androideabi-4.8
#        * arm-linux-androideabi-4.9 (default)
#        * arm-linux-androideabi-clang3.4
#        * arm-linux-androideabi-clang3.5
#        * mips64el-linux-android-4.9
#        * mips64el-linux-android-clang3.4
#        * mips64el-linux-android-clang3.5
#        * mipsel-linux-android-4.6
#        * mipsel-linux-android-4.8
#        * mipsel-linux-android-4.9
#        * mipsel-linux-android-clang3.4
#        * mipsel-linux-android-clang3.5
#        * x86-4.6
#        * x86-4.8
#        * x86-4.9
#        * x86-clang3.4
#        * x86-clang3.5
#        * x86_64-4.9
#        * x86_64-clang3.4
#        * x86_64-clang3.5
#
#    ANDROID_FORCE_ARM_BUILD=OFF - set ON to generate 32-bit ARM instructions
#      instead of Thumb. Is not available for "armeabi-v6 with VFP"
#      (is forced to be ON) ABI.
#
#    ANDROID_NO_UNDEFINED=ON - set ON to show all undefined symbols as linker
#      errors even if they are not used.
#
#    ANDROID_SO_UNDEFINED=OFF - set ON to allow undefined symbols in shared
#      libraries. Automatically turned for NDK r5x and r6x due to GLESv2
#      problems.
#
#    ANDROID_STL=gnustl_static - specify the runtime to use.
#
#      Possible values are:
#        none           -> Do not configure the runtime.
#        system         -> Use the default minimal system C++ runtime library.
#                          Implies -fno-rtti -fno-exceptions.
#                          Is not available for standalone toolchain.
#        system_re      -> Use the default minimal system C++ runtime library.
#                          Implies -frtti -fexceptions.
#                          Is not available for standalone toolchain.
#        gabi++_static  -> Use the GAbi++ runtime as a static library.
#                          Implies -frtti -fno-exceptions.
#                          Available for NDK r7 and newer.
#                          Is not available for standalone toolchain.
#        gabi++_shared  -> Use the GAbi++ runtime as a shared library.
#                          Implies -frtti -fno-exceptions.
#                          Available for NDK r7 and newer.
#                          Is not available for standalone toolchain.
#        stlport_static -> Use the STLport runtime as a static library.
#                          Implies -fno-rtti -fno-exceptions for NDK before r7.
#                          Implies -frtti -fno-exceptions for NDK r7 and newer.
#                          Is not available for standalone toolchain.
#        stlport_shared -> Use the STLport runtime as a shared library.
#                          Implies -fno-rtti -fno-exceptions for NDK before r7.
#                          Implies -frtti -fno-exceptions for NDK r7 and newer.
#                          Is not available for standalone toolchain.
#        gnustl_static  -> Use the GNU STL as a static library.
#                          Implies -frtti -fexceptions.
#        gnustl_shared  -> Use the GNU STL as a shared library.
#                          Implies -frtti -fno-exceptions.
#                          Available for NDK r7b and newer.
#                          Silently degrades to gnustl_static if not available.
#
#    ANDROID_STL_FORCE_FEATURES=ON - turn rtti and exceptions support based on
#      chosen runtime. If disabled, then the user is responsible for settings
#      these options.
#
#  What?:
#    android-cmake toolchain searches for NDK/toolchain in the following order:
#      ANDROID_NDK - cmake parameter
#      ANDROID_NDK - environment variable
#      ANDROID_STANDALONE_TOOLCHAIN - cmake parameter
#      ANDROID_STANDALONE_TOOLCHAIN - environment variable
#      ANDROID_NDK - default locations
#      ANDROID_STANDALONE_TOOLCHAIN - default locations
#
#    Make sure to do the following in your scripts:
#      SET( CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${my_cxx_flags}" )
#      SET( CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${my_cxx_flags}" )
#    The flags will be prepopulated with critical flags, so don't loose them.
#    Also be aware that toolchain also sets configuration-specific compiler
#    flags and linker flags.
#
#    ANDROID and BUILD_ANDROID will be set to true, you may test any of these
#    variables to make necessary Android-specific configuration changes.
#
#    Also ARMEABI or ARMEABI_V7A or X86 or MIPS or ARM64_V8A or X86_64 or MIPS64
#    will be set true, mutually exclusive. NEON option will be set true
#    if VFP is set to NEON.
#
# ------------------------------------------------------------------------------

cmake_minimum_required( VERSION 2.6.3 )

if( DEFINED CMAKE_CROSSCOMPILING )
 # subsequent toolchain loading is not really needed
 return()
endif()

if( CMAKE_TOOLCHAIN_FILE )
 # touch toolchain variable to suppress "unused variable" warning
endif()

# inherit settings in recursive loads
get_property( _CMAKE_IN_TRY_COMPILE GLOBAL PROPERTY IN_TRY_COMPILE )
if( _CMAKE_IN_TRY_COMPILE )
 include( "${CMAKE_CURRENT_SOURCE_DIR}/../android.toolchain.config.cmake" OPTIONAL )
endif()

# this one is important
if( CMAKE_VERSION VERSION_GREATER "3.0.99" )
 set( CMAKE_SYSTEM_NAME Android )
else()
 set( CMAKE_SYSTEM_NAME Linux )
endif()

# this one not so much
set( CMAKE_SYSTEM_VERSION 1 )

# rpath makes low sense for Android
set( CMAKE_SHARED_LIBRARY_RUNTIME_C_FLAG "" )
set( CMAKE_SKIP_RPATH TRUE CACHE BOOL "If set, runtime paths are not added when using shared libraries." )

# NDK search paths
set( ANDROID_SUPPORTED_NDK_VERSIONS ${ANDROID_EXTRA_NDK_VERSIONS} -r10d -r10c -r10b -r10 -r9d -r9c -r9b -r9 -r8e -r8d -r8c -r8b -r8 -r7c -r7b -r7 -r6b -r6 -r5c -r5b -r5 "" )
if( NOT DEFINED ANDROID_NDK_SEARCH_PATHS )
 if( CMAKE_HOST_WIN32 )
  file( TO_CMAKE_PATH "$ENV{PROGRAMFILES}" ANDROID_NDK_SEARCH_PATHS )
  set( ANDROID_NDK_SEARCH_PATHS "${ANDROID_NDK_SEARCH_PATHS}" "$ENV{SystemDrive}/NVPACK" )
 else()
  file( TO_CMAKE_PATH "$ENV{HOME}" ANDROID_NDK_SEARCH_PATHS )
  set( ANDROID_NDK_SEARCH_PATHS /opt "${ANDROID_NDK_SEARCH_PATHS}/NVPACK" )
 endif()
endif()
if( NOT DEFINED ANDROID_STANDALONE_TOOLCHAIN_SEARCH_PATH )
 set( ANDROID_STANDALONE_TOOLCHAIN_SEARCH_PATH /opt/android-toolchain )
endif()

# known ABIs
set( ANDROID_SUPPORTED_ABIS_arm "armeabi-v7a;armeabi;armeabi-v7a with NEON;armeabi-v7a with VFPV3;armeabi-v6 with VFP" )
set( ANDROID_SUPPORTED_ABIS_arm64 "arm64-v8a" )
set( ANDROID_SUPPORTED_ABIS_x86 "x86" )
set( ANDROID_SUPPORTED_ABIS_x86_64 "x86_64" )
set( ANDROID_SUPPORTED_ABIS_mips "mips" )
set( ANDROID_SUPPORTED_ABIS_mips64 "mips64" )

# API level defaults
set( ANDROID_DEFAULT_NDK_API_LEVEL 8 )
set( ANDROID_DEFAULT_NDK_API_LEVEL_arm64 21 )
set( ANDROID_DEFAULT_NDK_API_LEVEL_x86 9 )
set( ANDROID_DEFAULT_NDK_API_LEVEL_x86_64 21 )
set( ANDROID_DEFAULT_NDK_API_LEVEL_mips 9 )
set( ANDROID_DEFAULT_NDK_API_LEVEL_mips64 21 )


macro( __LIST_FILTER listvar regex )
  if( ${listvar} )
    foreach( __val ${${listvar}} )
      if( __val MATCHES "${regex}" )
        list( REMOVE_ITEM ${listvar} "${__val}" )
      endif()
    endforeach()
  endif()
endmacro()

macro( __INIT_VARIABLE var_name )
  set( __test_path 0 )
  foreach( __var ${ARGN} )
    if( __var STREQUAL "PATH" )
      set( __test_path 1 )
      break()
    endif()
  endforeach()

  if( __test_path AND NOT EXISTS "${${var_name}}" )
    unset( ${var_name} CACHE )
  endif()

  if( " ${${var_name}}" STREQUAL " " )
    set( __values 0 )
    foreach( __var ${ARGN} )
      if( __var STREQUAL "VALUES" )
        set( __values 1 )
      elseif( NOT __var STREQUAL "PATH" )
        if( __var MATCHES "^ENV_.*$" )
          string( REPLACE "ENV_" "" __var "${__var}" )
          set( __value "$ENV{${__var}}" )
        elseif( DEFINED ${__var} )
          set( __value "${${__var}}" )
        elseif( __values )
          set( __value "${__var}" )
        else()
          set( __value "" )
        endif()

        if( NOT " ${__value}" STREQUAL " " AND (NOT __test_path OR EXISTS "${__value}") )
          set( ${var_name} "${__value}" )
          break()
        endif()
      endif()
    endforeach()
    unset( __value )
    unset( __values )
  endif()

  if( __test_path )
    file( TO_CMAKE_PATH "${${var_name}}" ${var_name} )
  endif()
  unset( __test_path )
endmacro()

macro( __DETECT_NATIVE_API_LEVEL _var _path )
  set( __ndkApiLevelRegex "^[\t ]*#define[\t ]+__ANDROID_API__[\t ]+([0-9]+)[\t ]*.*$" )
  file( STRINGS ${_path} __apiFileContent REGEX "${__ndkApiLevelRegex}" )
  if( NOT __apiFileContent )
    message( SEND_ERROR "Could not get Android native API level. Probably you have specified invalid level value, or your copy of NDK/toolchain is broken." )
  endif()
  string( REGEX REPLACE "${__ndkApiLevelRegex}" "\\1" ${_var} "${__apiFileContent}" )
  unset( __apiFileContent )
  unset( __ndkApiLevelRegex )
endmacro()

macro( __DETECT_TOOLCHAIN_MACHINE_NAME _var _root )
 if( EXISTS "${_root}" )
    file( GLOB __gccExePath RELATIVE "${_root}/bin/" "${_root}/bin/*-gcc${TOOL_OS_SUFFIX}" )
    __LIST_FILTER( __gccExePath "^[.].*" )
    list( LENGTH __gccExePath __gccExePathsCount )
    if( NOT __gccExePathsCount EQUAL 1  AND NOT _CMAKE_IN_TRY_COMPILE )
      message( WARNING "Could not determine machine name for compiler from ${_root}" )
      set( ${_var} "" )
    else()
      get_filename_component( __gccExeName "${__gccExePath}" NAME_WE )
      string( REPLACE "-gcc" "" ${_var} "${__gccExeName}" )
    endif()
    unset( __gccExePath )
    unset( __gccExePathsCount )
    unset( __gccExeName )
  else()
    set( ${_var} "" )
  endif()
endmacro()


# fight against cygwin
set( ANDROID_FORBID_SYGWIN TRUE CACHE BOOL "Prevent cmake from working under cygwin and using cygwin tools")
mark_as_advanced( ANDROID_FORBID_SYGWIN )
if( ANDROID_FORBID_SYGWIN )
 if( CYGWIN )
  message( FATAL_ERROR "Android NDK and android-cmake toolchain are not welcome Cygwin. It is unlikely that this cmake toolchain will work under cygwin. But if you want to try then you can set cmake variable ANDROID_FORBID_SYGWIN to FALSE and rerun cmake." )
 endif()

 if( CMAKE_HOST_WIN32 )
  # remove cygwin from PATH
  set( __new_path "$ENV{PATH}")
  __LIST_FILTER( __new_path "cygwin" )
  set(ENV{PATH} "${__new_path}")
  unset(__new_path)
 endif()
endif()


# detect current host platform
if( NOT DEFINED ANDROID_NDK_HOST_X64 AND (CMAKE_HOST_SYSTEM_PROCESSOR MATCHES "amd64|x86_64|AMD64" OR CMAKE_HOST_APPLE) )
 set( ANDROID_NDK_HOST_X64 1 CACHE BOOL "Try to use 64-bit compiler toolchain" )
 mark_as_advanced( ANDROID_NDK_HOST_X64 )
endif()

set( TOOL_OS_SUFFIX "" )
if( CMAKE_HOST_APPLE )
 set( ANDROID_NDK_HOST_SYSTEM_NAME "darwin-x86_64" )
 set( ANDROID_NDK_HOST_SYSTEM_NAME2 "darwin-x86" )
elseif( CMAKE_HOST_WIN32 )
 set( ANDROID_NDK_HOST_SYSTEM_NAME "windows-x86_64" )
 set( ANDROID_NDK_HOST_SYSTEM_NAME2 "windows" )
 set( TOOL_OS_SUFFIX ".exe" )
elseif( CMAKE_HOST_UNIX )
 set( ANDROID_NDK_HOST_SYSTEM_NAME "linux-x86_64" )
 set( ANDROID_NDK_HOST_SYSTEM_NAME2 "linux-x86" )
else()
 message( FATAL_ERROR "Cross-compilation on your platform is not supported by this cmake toolchain" )
endif()

if( NOT ANDROID_NDK_HOST_X64 )
 set( ANDROID_NDK_HOST_SYSTEM_NAME ${ANDROID_NDK_HOST_SYSTEM_NAME2} )
endif()

# see if we have path to Android NDK
if( NOT ANDROID_NDK AND NOT ANDROID_STANDALONE_TOOLCHAIN )
  __INIT_VARIABLE( ANDROID_NDK PATH ENV_ANDROID_NDK )
endif()
if( NOT ANDROID_NDK )
 # see if we have path to Android standalone toolchain
 __INIT_VARIABLE( ANDROID_STANDALONE_TOOLCHAIN PATH ENV_ANDROID_STANDALONE_TOOLCHAIN )

 if( NOT ANDROID_STANDALONE_TOOLCHAIN )
  #try to find Android NDK in one of the the default locations
  set( __ndkSearchPaths )
  foreach( __ndkSearchPath ${ANDROID_NDK_SEARCH_PATHS} )
   foreach( suffix ${ANDROID_SUPPORTED_NDK_VERSIONS} )
    list( APPEND __ndkSearchPaths "${__ndkSearchPath}/android-ndk${suffix}" )
   endforeach()
  endforeach()
  __INIT_VARIABLE( ANDROID_NDK PATH VALUES ${__ndkSearchPaths} )
  unset( __ndkSearchPaths )

  if( ANDROID_NDK )
   message( STATUS "Using default path for Android NDK: ${ANDROID_NDK}" )
   message( STATUS "  If you prefer to use a different location, please define a cmake or environment variable: ANDROID_NDK" )
  else()
   #try to find Android standalone toolchain in one of the the default locations
   __INIT_VARIABLE( ANDROID_STANDALONE_TOOLCHAIN PATH ANDROID_STANDALONE_TOOLCHAIN_SEARCH_PATH )

   if( ANDROID_STANDALONE_TOOLCHAIN )
    message( STATUS "Using default path for standalone toolchain ${ANDROID_STANDALONE_TOOLCHAIN}" )
    message( STATUS "  If you prefer to use a different location, please define the variable: ANDROID_STANDALONE_TOOLCHAIN" )
   endif( ANDROID_STANDALONE_TOOLCHAIN )
  endif( ANDROID_NDK )
 endif( NOT ANDROID_STANDALONE_TOOLCHAIN )
endif( NOT ANDROID_NDK )

# remember found paths
if( ANDROID_NDK )
 get_filename_component( ANDROID_NDK "${ANDROID_NDK}" ABSOLUTE )
 set( ANDROID_NDK "${ANDROID_NDK}" CACHE INTERNAL "Path of the Android NDK" FORCE )
 set( BUILD_WITH_ANDROID_NDK True )
 if( EXISTS "${ANDROID_NDK}/RELEASE.TXT" )
  file( STRINGS "${ANDROID_NDK}/RELEASE.TXT" ANDROID_NDK_RELEASE_FULL LIMIT_COUNT 1 REGEX "r[0-9]+[a-z]?" )
  string( REGEX MATCH "r([0-9]+)([a-z]?)" ANDROID_NDK_RELEASE "${ANDROID_NDK_RELEASE_FULL}" )
 else()
  set( ANDROID_NDK_RELEASE "r1x" )
  set( ANDROID_NDK_RELEASE_FULL "unreleased" )
 endif()
 string( REGEX REPLACE "r([0-9]+)([a-z]?)" "\\1*1000" ANDROID_NDK_RELEASE_NUM "${ANDROID_NDK_RELEASE}" )
 string( FIND " abcdefghijklmnopqastuvwxyz" "${CMAKE_MATCH_2}" __ndkReleaseLetterNum )
 math( EXPR ANDROID_NDK_RELEASE_NUM "${ANDROID_NDK_RELEASE_NUM}+${__ndkReleaseLetterNum}" )
elseif( ANDROID_STANDALONE_TOOLCHAIN )
 get_filename_component( ANDROID_STANDALONE_TOOLCHAIN "${ANDROID_STANDALONE_TOOLCHAIN}" ABSOLUTE )
 # try to detect change
 if( CMAKE_AR )
  string( LENGTH "${ANDROID_STANDALONE_TOOLCHAIN}" __length )
  string( SUBSTRING "${CMAKE_AR}" 0 ${__length} __androidStandaloneToolchainPreviousPath )
  if( NOT __androidStandaloneToolchainPreviousPath STREQUAL ANDROID_STANDALONE_TOOLCHAIN )
   message( FATAL_ERROR "It is not possible to change path to the Android standalone toolchain on subsequent run." )
  endif()
  unset( __androidStandaloneToolchainPreviousPath )
  unset( __length )
 endif()
 set( ANDROID_STANDALONE_TOOLCHAIN "${ANDROID_STANDALONE_TOOLCHAIN}" CACHE INTERNAL "Path of the Android standalone toolchain" FORCE )
 set( BUILD_WITH_STANDALONE_TOOLCHAIN True )
else()
 list(GET ANDROID_NDK_SEARCH_PATHS 0 ANDROID_NDK_SEARCH_PATH)
 message( FATAL_ERROR "Could not find neither Android NDK nor Android standalone toolchain.
    You should either set an environment variable:
      export ANDROID_NDK=~/my-android-ndk
    or
      export ANDROID_STANDALONE_TOOLCHAIN=~/my-android-toolchain
    or put the toolchain or NDK in the default path:
      sudo ln -s ~/my-android-ndk ${ANDROID_NDK_SEARCH_PATH}/android-ndk
      sudo ln -s ~/my-android-toolchain ${ANDROID_STANDALONE_TOOLCHAIN_SEARCH_PATH}" )
endif()

# android NDK layout
if( BUILD_WITH_ANDROID_NDK )
 if( NOT DEFINED ANDROID_NDK_LAYOUT )
  # try to automatically detect the layout
  if( EXISTS "${ANDROID_NDK}/RELEASE.TXT")
   set( ANDROID_NDK_LAYOUT "RELEASE" )
  elseif( EXISTS "${ANDROID_NDK}/../../linux-x86/toolchain/" )
   set( ANDROID_NDK_LAYOUT "LINARO" )
  elseif( EXISTS "${ANDROID_NDK}/../../gcc/" )
   set( ANDROID_NDK_LAYOUT "ANDROID" )
  endif()
 endif()
 set( ANDROID_NDK_LAYOUT "${ANDROID_NDK_LAYOUT}" CACHE STRING "The inner layout of NDK" )
 mark_as_advanced( ANDROID_NDK_LAYOUT )
 if( ANDROID_NDK_LAYOUT STREQUAL "LINARO" )
  set( ANDROID_NDK_HOST_SYSTEM_NAME ${ANDROID_NDK_HOST_SYSTEM_NAME2} ) # only 32-bit at the moment
  set( ANDROID_NDK_TOOLCHAINS_PATH "${ANDROID_NDK}/../../${ANDROID_NDK_HOST_SYSTEM_NAME}/toolchain" )
  set( ANDROID_NDK_TOOLCHAINS_SUBPATH  "" )
  set( ANDROID_NDK_TOOLCHAINS_SUBPATH2 "" )
 elseif( ANDROID_NDK_LAYOUT STREQUAL "ANDROID" )
  set( ANDROID_NDK_HOST_SYSTEM_NAME ${ANDROID_NDK_HOST_SYSTEM_NAME2} ) # only 32-bit at the moment
  set( ANDROID_NDK_TOOLCHAINS_PATH "${ANDROID_NDK}/../../gcc/${ANDROID_NDK_HOST_SYSTEM_NAME}/arm" )
  set( ANDROID_NDK_TOOLCHAINS_SUBPATH  "" )
  set( ANDROID_NDK_TOOLCHAINS_SUBPATH2 "" )
 else() # ANDROID_NDK_LAYOUT STREQUAL "RELEASE"
  set( ANDROID_NDK_TOOLCHAINS_PATH "${ANDROID_NDK}/toolchains" )
  set( ANDROID_NDK_TOOLCHAINS_SUBPATH  "/prebuilt/${ANDROID_NDK_HOST_SYSTEM_NAME}" )
  set( ANDROID_NDK_TOOLCHAINS_SUBPATH2 "/prebuilt/${ANDROID_NDK_HOST_SYSTEM_NAME2}" )
 endif()
 get_filename_component( ANDROID_NDK_TOOLCHAINS_PATH "${ANDROID_NDK_TOOLCHAINS_PATH}" ABSOLUTE )

 # try to detect change of NDK
 if( CMAKE_AR )
  string( LENGTH "${ANDROID_NDK_TOOLCHAINS_PATH}" __length )
  string( SUBSTRING "${CMAKE_AR}" 0 ${__length} __androidNdkPreviousPath )
  if( NOT __androidNdkPreviousPath STREQUAL ANDROID_NDK_TOOLCHAINS_PATH )
   message( FATAL_ERROR "It is not possible to change the path to the NDK on subsequent CMake run. You must remove all generated files from your build folder first.
   " )
  endif()
  unset( __androidNdkPreviousPath )
  unset( __length )
 endif()
endif()


# get all the details about standalone toolchain
if( BUILD_WITH_STANDALONE_TOOLCHAIN )
 __DETECT_NATIVE_API_LEVEL( ANDROID_SUPPORTED_NATIVE_API_LEVELS "${ANDROID_STANDALONE_TOOLCHAIN}/sysroot/usr/include/android/api-level.h" )
 set( ANDROID_STANDALONE_TOOLCHAIN_API_LEVEL ${ANDROID_SUPPORTED_NATIVE_API_LEVELS} )
 set( __availableToolchains "standalone" )
 __DETECT_TOOLCHAIN_MACHINE_NAME( __availableToolchainMachines "${ANDROID_STANDALONE_TOOLCHAIN}" )
 if( NOT __availableToolchainMachines )
  message( FATAL_ERROR "Could not determine machine name of your toolchain. Probably your Android standalone toolchain is broken." )
 endif()
 if( __availableToolchainMachines MATCHES x86_64 )
  set( __availableToolchainArchs "x86_64" )
 elseif( __availableToolchainMachines MATCHES i686 )
  set( __availableToolchainArchs "x86" )
 elseif( __availableToolchainMachines MATCHES aarch64 )
  set( __availableToolchainArchs "arm64" )
 elseif( __availableToolchainMachines MATCHES arm )
  set( __availableToolchainArchs "arm" )
 elseif( __availableToolchainMachines MATCHES mips64el )
  set( __availableToolchainArchs "mips64" )
 elseif( __availableToolchainMachines MATCHES mipsel )
  set( __availableToolchainArchs "mips" )
 endif()
 execute_process( COMMAND "${ANDROID_STANDALONE_TOOLCHAIN}/bin/${__availableToolchainMachines}-gcc${TOOL_OS_SUFFIX}" -dumpversion
                  OUTPUT_VARIABLE __availableToolchainCompilerVersions OUTPUT_STRIP_TRAILING_WHITESPACE )
 string( REGEX MATCH "[0-9]+[.][0-9]+([.][0-9]+)?" __availableToolchainCompilerVersions "${__availableToolchainCompilerVersions}" )
 if( EXISTS "${ANDROID_STANDALONE_TOOLCHAIN}/bin/clang${TOOL_OS_SUFFIX}" )
  list( APPEND __availableToolchains "standalone-clang" )
  list( APPEND __availableToolchainMachines ${__availableToolchainMachines} )
  list( APPEND __availableToolchainArchs ${__availableToolchainArchs} )
  list( APPEND __availableToolchainCompilerVersions ${__availableToolchainCompilerVersions} )
 endif()
endif()

macro( __GLOB_NDK_TOOLCHAINS __availableToolchainsVar __availableToolchainsLst __toolchain_subpath )
 foreach( __toolchain ${${__availableToolchainsLst}} )
  if( "${__toolchain}" MATCHES "-clang3[.][0-9]$" AND NOT EXISTS "${ANDROID_NDK_TOOLCHAINS_PATH}/${__toolchain}${__toolchain_subpath}" )
   SET( __toolchainVersionRegex "^TOOLCHAIN_VERSION[\t ]+:=[\t ]+(.*)$" )
   FILE( STRINGS "${ANDROID_NDK_TOOLCHAINS_PATH}/${__toolchain}/setup.mk" __toolchainVersionStr REGEX "${__toolchainVersionRegex}" )
   if( __toolchainVersionStr )
    string( REGEX REPLACE "${__toolchainVersionRegex}" "\\1" __toolchainVersionStr "${__toolchainVersionStr}" )
    string( REGEX REPLACE "-clang3[.][0-9]$" "-${__toolchainVersionStr}" __gcc_toolchain "${__toolchain}" )
   else()
    string( REGEX REPLACE "-clang3[.][0-9]$" "-4.6" __gcc_toolchain "${__toolchain}" )
   endif()
   unset( __toolchainVersionStr )
   unset( __toolchainVersionRegex )
  else()
   set( __gcc_toolchain "${__toolchain}" )
  endif()
  __DETECT_TOOLCHAIN_MACHINE_NAME( __machine "${ANDROID_NDK_TOOLCHAINS_PATH}/${__gcc_toolchain}${__toolchain_subpath}" )
  if( __machine )
   string( REGEX MATCH "[0-9]+[.][0-9]+([.][0-9x]+)?$" __version "${__gcc_toolchain}" )
   if( __machine MATCHES x86_64 )
    set( __arch "x86_64" )
   elseif( __machine MATCHES i686 )
    set( __arch "x86" )
   elseif( __machine MATCHES aarch64 )
    set( __arch "arm64" )
   elseif( __machine MATCHES arm )
    set( __arch "arm" )
   elseif( __machine MATCHES mips64el )
    set( __arch "mips64" )
   elseif( __machine MATCHES mipsel )
    set( __arch "mips" )
   else()
    set( __arch "" )
   endif()
   #message("machine: !${__machine}!\narch: !${__arch}!\nversion: !${__version}!\ntoolchain: !${__toolchain}!\n")
   if (__arch)
    list( APPEND __availableToolchainMachines "${__machine}" )
    list( APPEND __availableToolchainArchs "${__arch}" )
    list( APPEND __availableToolchainCompilerVersions "${__version}" )
    list( APPEND ${__availableToolchainsVar} "${__toolchain}" )
   endif()
  endif()
  unset( __gcc_toolchain )
 endforeach()
endmacro()

# get all the details about NDK
if( BUILD_WITH_ANDROID_NDK )
 file( GLOB ANDROID_SUPPORTED_NATIVE_API_LEVELS RELATIVE "${ANDROID_NDK}/platforms" "${ANDROID_NDK}/platforms/android-*" )
 string( REPLACE "android-" "" ANDROID_SUPPORTED_NATIVE_API_LEVELS "${ANDROID_SUPPORTED_NATIVE_API_LEVELS}" )
 set( __availableToolchains "" )
 set( __availableToolchainMachines "" )
 set( __availableToolchainArchs "" )
 set( __availableToolchainCompilerVersions "" )
 if( ANDROID_TOOLCHAIN_NAME AND EXISTS "${ANDROID_NDK_TOOLCHAINS_PATH}/${ANDROID_TOOLCHAIN_NAME}/" )
  # do not go through all toolchains if we know the name
  set( __availableToolchainsLst "${ANDROID_TOOLCHAIN_NAME}" )
  __GLOB_NDK_TOOLCHAINS( __availableToolchains __availableToolchainsLst "${ANDROID_NDK_TOOLCHAINS_SUBPATH}" )
  if( NOT __availableToolchains AND NOT ANDROID_NDK_TOOLCHAINS_SUBPATH STREQUAL ANDROID_NDK_TOOLCHAINS_SUBPATH2 )
   __GLOB_NDK_TOOLCHAINS( __availableToolchains __availableToolchainsLst "${ANDROID_NDK_TOOLCHAINS_SUBPATH2}" )
   if( __availableToolchains )
    set( ANDROID_NDK_TOOLCHAINS_SUBPATH ${ANDROID_NDK_TOOLCHAINS_SUBPATH2} )
   endif()
  endif()
 endif()
 if( NOT __availableToolchains )
  file( GLOB __availableToolchainsLst RELATIVE "${ANDROID_NDK_TOOLCHAINS_PATH}" "${ANDROID_NDK_TOOLCHAINS_PATH}/*" )
  if( __availableToolchains )
   list(SORT __availableToolchainsLst) # we need clang to go after gcc
  endif()
  __LIST_FILTER( __availableToolchainsLst "^[.]" )
  __LIST_FILTER( __availableToolchainsLst "llvm" )
  __LIST_FILTER( __availableToolchainsLst "renderscript" )
  __GLOB_NDK_TOOLCHAINS( __availableToolchains __availableToolchainsLst "${ANDROID_NDK_TOOLCHAINS_SUBPATH}" )
  if( NOT __availableToolchains AND NOT ANDROID_NDK_TOOLCHAINS_SUBPATH STREQUAL ANDROID_NDK_TOOLCHAINS_SUBPATH2 )
   __GLOB_NDK_TOOLCHAINS( __availableToolchains __availableToolchainsLst "${ANDROID_NDK_TOOLCHAINS_SUBPATH2}" )
   if( __availableToolchains )
    set( ANDROID_NDK_TOOLCHAINS_SUBPATH ${ANDROID_NDK_TOOLCHAINS_SUBPATH2} )
   endif()
  endif()
 endif()
 if( NOT __availableToolchains )
  message( FATAL_ERROR "Could not find any working toolchain in the NDK. Probably your Android NDK is broken." )
 endif()
endif()

# build list of available ABIs
set( ANDROID_SUPPORTED_ABIS "" )
set( __uniqToolchainArchNames ${__availableToolchainArchs} )
list( REMOVE_DUPLICATES __uniqToolchainArchNames )
list( SORT __uniqToolchainArchNames )
foreach( __arch ${__uniqToolchainArchNames} )
 list( APPEND ANDROID_SUPPORTED_ABIS ${ANDROID_SUPPORTED_ABIS_${__arch}} )
endforeach()
unset( __uniqToolchainArchNames )
if( NOT ANDROID_SUPPORTED_ABIS )
 message( FATAL_ERROR "No one of known Android ABIs is supported by this cmake toolchain." )
endif()

# choose target ABI
__INIT_VARIABLE( ANDROID_ABI VALUES ${ANDROID_SUPPORTED_ABIS} )
# verify that target ABI is supported
list( FIND ANDROID_SUPPORTED_ABIS "${ANDROID_ABI}" __androidAbiIdx )
if( __androidAbiIdx EQUAL -1 )
 string( REPLACE ";" "\", \"" PRINTABLE_ANDROID_SUPPORTED_ABIS  "${ANDROID_SUPPORTED_ABIS}" )
 message( FATAL_ERROR "Specified ANDROID_ABI = \"${ANDROID_ABI}\" is not supported by this cmake toolchain or your NDK/toolchain.
   Supported values are: \"${PRINTABLE_ANDROID_SUPPORTED_ABIS}\"
   " )
endif()
unset( __androidAbiIdx )

# set target ABI options
if( ANDROID_ABI STREQUAL "x86" )
 set( X86 true )
 set( ANDROID_NDK_ABI_NAME "x86" )
 set( ANDROID_ARCH_NAME "x86" )
 set( ANDROID_LLVM_TRIPLE "i686-none-linux-android" )
 set( CMAKE_SYSTEM_PROCESSOR "i686" )
elseif( ANDROID_ABI STREQUAL "x86_64" )
 set( X86 true )
 set( X86_64 true )
 set( ANDROID_NDK_ABI_NAME "x86_64" )
 set( ANDROID_ARCH_NAME "x86_64" )
 set( CMAKE_SYSTEM_PROCESSOR "x86_64" )
 set( ANDROID_LLVM_TRIPLE "x86_64-none-linux-android" )
elseif( ANDROID_ABI STREQUAL "mips64" )
 set( MIPS64 true )
 set( ANDROID_NDK_ABI_NAME "mips64" )
 set( ANDROID_ARCH_NAME "mips64" )
 set( ANDROID_LLVM_TRIPLE "mips64el-none-linux-android" )
 set( CMAKE_SYSTEM_PROCESSOR "mips64" )
elseif( ANDROID_ABI STREQUAL "mips" )
 set( MIPS true )
 set( ANDROID_NDK_ABI_NAME "mips" )
 set( ANDROID_ARCH_NAME "mips" )
 set( ANDROID_LLVM_TRIPLE "mipsel-none-linux-android" )
 set( CMAKE_SYSTEM_PROCESSOR "mips" )
elseif( ANDROID_ABI STREQUAL "arm64-v8a" )
 set( ARM64_V8A true )
 set( ANDROID_NDK_ABI_NAME "arm64-v8a" )
 set( ANDROID_ARCH_NAME "arm64" )
 set( ANDROID_LLVM_TRIPLE "aarch64-none-linux-android" )
 set( CMAKE_SYSTEM_PROCESSOR "aarch64" )
 set( VFPV3 true )
 set( NEON true )
elseif( ANDROID_ABI STREQUAL "armeabi" )
 set( ARMEABI true )
 set( ANDROID_NDK_ABI_NAME "armeabi" )
 set( ANDROID_ARCH_NAME "arm" )
 set( ANDROID_LLVM_TRIPLE "armv5te-none-linux-androideabi" )
 set( CMAKE_SYSTEM_PROCESSOR "armv5te" )
elseif( ANDROID_ABI STREQUAL "armeabi-v6 with VFP" )
 set( ARMEABI_V6 true )
 set( ANDROID_NDK_ABI_NAME "armeabi" )
 set( ANDROID_ARCH_NAME "arm" )
 set( ANDROID_LLVM_TRIPLE "armv5te-none-linux-androideabi" )
 set( CMAKE_SYSTEM_PROCESSOR "armv6" )
 # need always fallback to older platform
 set( ARMEABI true )
elseif( ANDROID_ABI STREQUAL "armeabi-v7a")
 set( ARMEABI_V7A true )
 set( ANDROID_NDK_ABI_NAME "armeabi-v7a" )
 set( ANDROID_ARCH_NAME "arm" )
 set( ANDROID_LLVM_TRIPLE "armv7-none-linux-androideabi" )
 set( CMAKE_SYSTEM_PROCESSOR "armv7-a" )
elseif( ANDROID_ABI STREQUAL "armeabi-v7a with VFPV3" )
 set( ARMEABI_V7A true )
 set( ANDROID_NDK_ABI_NAME "armeabi-v7a" )
 set( ANDROID_ARCH_NAME "arm" )
 set( ANDROID_LLVM_TRIPLE "armv7-none-linux-androideabi" )
 set( CMAKE_SYSTEM_PROCESSOR "armv7-a" )
 set( VFPV3 true )
elseif( ANDROID_ABI STREQUAL "armeabi-v7a with NEON" )
 set( ARMEABI_V7A true )
 set( ANDROID_NDK_ABI_NAME "armeabi-v7a" )
 set( ANDROID_ARCH_NAME "arm" )
 set( ANDROID_LLVM_TRIPLE "armv7-none-linux-androideabi" )
 set( CMAKE_SYSTEM_PROCESSOR "armv7-a" )
 set( VFPV3 true )
 set( NEON true )
else()
 message( SEND_ERROR "Unknown ANDROID_ABI=\"${ANDROID_ABI}\" is specified." )
endif()

if( CMAKE_BINARY_DIR AND EXISTS "${CMAKE_BINARY_DIR}${CMAKE_FILES_DIRECTORY}/CMakeSystem.cmake" )
 # really dirty hack
 # it is not possible to change CMAKE_SYSTEM_PROCESSOR after the first run...
 file( APPEND "${CMAKE_BINARY_DIR}${CMAKE_FILES_DIRECTORY}/CMakeSystem.cmake" "SET(CMAKE_SYSTEM_PROCESSOR \"${CMAKE_SYSTEM_PROCESSOR}\")\n" )
endif()

if( ANDROID_ARCH_NAME STREQUAL "arm" AND NOT ARMEABI_V6 )
 __INIT_VARIABLE( ANDROID_FORCE_ARM_BUILD VALUES OFF )
 set( ANDROID_FORCE_ARM_BUILD ${ANDROID_FORCE_ARM_BUILD} CACHE BOOL "Use 32-bit ARM instructions instead of Thumb-1" FORCE )
 mark_as_advanced( ANDROID_FORCE_ARM_BUILD )
else()
 unset( ANDROID_FORCE_ARM_BUILD CACHE )
endif()

# choose toolchain
if( ANDROID_TOOLCHAIN_NAME )
 list( FIND __availableToolchains "${ANDROID_TOOLCHAIN_NAME}" __toolchainIdx )
 if( __toolchainIdx EQUAL -1 )
  list( SORT __availableToolchains )
  string( REPLACE ";" "\n  * " toolchains_list "${__availableToolchains}" )
  set( toolchains_list "  * ${toolchains_list}")
  message( FATAL_ERROR "Specified toolchain \"${ANDROID_TOOLCHAIN_NAME}\" is missing in your NDK or broken. Please verify that your NDK is working or select another compiler toolchain.
To configure the toolchain set CMake variable ANDROID_TOOLCHAIN_NAME to one of the following values:\n${toolchains_list}\n" )
 endif()
 list( GET __availableToolchainArchs ${__toolchainIdx} __toolchainArch )
 if( NOT __toolchainArch STREQUAL ANDROID_ARCH_NAME )
  message( SEND_ERROR "Selected toolchain \"${ANDROID_TOOLCHAIN_NAME}\" is not able to compile binaries for the \"${ANDROID_ARCH_NAME}\" platform." )
 endif()
else()
 set( __toolchainIdx -1 )
 set( __applicableToolchains "" )
 set( __toolchainMaxVersion "0.0.0" )
 list( LENGTH __availableToolchains __availableToolchainsCount )
 math( EXPR __availableToolchainsCount "${__availableToolchainsCount}-1" )
 foreach( __idx RANGE ${__availableToolchainsCount} )
  list( GET __availableToolchainArchs ${__idx} __toolchainArch )
  if( __toolchainArch STREQUAL ANDROID_ARCH_NAME )
   list( GET __availableToolchainCompilerVersions ${__idx} __toolchainVersion )
   string( REPLACE "x" "99" __toolchainVersion "${__toolchainVersion}")
   if( __toolchainVersion VERSION_GREATER __toolchainMaxVersion )
    set( __toolchainMaxVersion "${__toolchainVersion}" )
    set( __toolchainIdx ${__idx} )
   endif()
  endif()
 endforeach()
 unset( __availableToolchainsCount )
 unset( __toolchainMaxVersion )
 unset( __toolchainVersion )
endif()
unset( __toolchainArch )
if( __toolchainIdx EQUAL -1 )
 message( FATAL_ERROR "No one of available compiler toolchains is able to compile for ${ANDROID_ARCH_NAME} platform." )
endif()
list( GET __availableToolchains ${__toolchainIdx} ANDROID_TOOLCHAIN_NAME )
list( GET __availableToolchainMachines ${__toolchainIdx} ANDROID_TOOLCHAIN_MACHINE_NAME )
list( GET __availableToolchainCompilerVersions ${__toolchainIdx} ANDROID_COMPILER_VERSION )

unset( __toolchainIdx )
unset( __availableToolchains )
unset( __availableToolchainMachines )
unset( __availableToolchainArchs )
unset( __availableToolchainCompilerVersions )

# choose native API level
__INIT_VARIABLE( ANDROID_NATIVE_API_LEVEL ENV_ANDROID_NATIVE_API_LEVEL ANDROID_API_LEVEL ENV_ANDROID_API_LEVEL ANDROID_STANDALONE_TOOLCHAIN_API_LEVEL ANDROID_DEFAULT_NDK_API_LEVEL_${ANDROID_ARCH_NAME} ANDROID_DEFAULT_NDK_API_LEVEL )
string( REPLACE "android-" "" ANDROID_NATIVE_API_LEVEL "${ANDROID_NATIVE_API_LEVEL}" )
string( STRIP "${ANDROID_NATIVE_API_LEVEL}" ANDROID_NATIVE_API_LEVEL )
# adjust API level
set( __real_api_level ${ANDROID_DEFAULT_NDK_API_LEVEL_${ANDROID_ARCH_NAME}} )
foreach( __level ${ANDROID_SUPPORTED_NATIVE_API_LEVELS} )
 if( (__level LESS ANDROID_NATIVE_API_LEVEL OR __level STREQUAL ANDROID_NATIVE_API_LEVEL) AND NOT __level LESS __real_api_level )
  set( __real_api_level ${__level} )
 endif()
endforeach()
if( __real_api_level AND NOT ANDROID_NATIVE_API_LEVEL STREQUAL __real_api_level )
 message( STATUS "Adjusting Android API level 'android-${ANDROID_NATIVE_API_LEVEL}' to 'android-${__real_api_level}'")
 set( ANDROID_NATIVE_API_LEVEL ${__real_api_level} )
endif()
unset(__real_api_level)
# validate
list( FIND ANDROID_SUPPORTED_NATIVE_API_LEVELS "${ANDROID_NATIVE_API_LEVEL}" __levelIdx )
if( __levelIdx EQUAL -1 )
 message( SEND_ERROR "Specified Android native API level 'android-${ANDROID_NATIVE_API_LEVEL}' is not supported by your NDK/toolchain." )
else()
 if( BUILD_WITH_ANDROID_NDK )
  __DETECT_NATIVE_API_LEVEL( __realApiLevel "${ANDROID_NDK}/platforms/android-${ANDROID_NATIVE_API_LEVEL}/arch-${ANDROID_ARCH_NAME}/usr/include/android/api-level.h" )
  if( NOT __realApiLevel EQUAL ANDROID_NATIVE_API_LEVEL AND NOT __realApiLevel GREATER 9000 )
   message( SEND_ERROR "Specified Android API level (${ANDROID_NATIVE_API_LEVEL}) does not match to the level found (${__realApiLevel}). Probably your copy of NDK is broken." )
  endif()
  unset( __realApiLevel )
 endif()
 set( ANDROID_NATIVE_API_LEVEL "${ANDROID_NATIVE_API_LEVEL}" CACHE STRING "Android API level for native code" FORCE )
 set( CMAKE_ANDROID_API ${ANDROID_NATIVE_API_LEVEL} )
 if( CMAKE_VERSION VERSION_GREATER "2.8" )
  list( SORT ANDROID_SUPPORTED_NATIVE_API_LEVELS )
  set_property( CACHE ANDROID_NATIVE_API_LEVEL PROPERTY STRINGS ${ANDROID_SUPPORTED_NATIVE_API_LEVELS} )
 endif()
endif()
unset( __levelIdx )


# remember target ABI
set( ANDROID_ABI "${ANDROID_ABI}" CACHE STRING "The target ABI for Android. If arm, then armeabi-v7a is recommended for hardware floating point." FORCE )
if( CMAKE_VERSION VERSION_GREATER "2.8" )
 list( SORT ANDROID_SUPPORTED_ABIS_${ANDROID_ARCH_NAME} )
 set_property( CACHE ANDROID_ABI PROPERTY STRINGS ${ANDROID_SUPPORTED_ABIS_${ANDROID_ARCH_NAME}} )
endif()


# runtime choice (STL, rtti, exceptions)
if( NOT ANDROID_STL )
  set( ANDROID_STL gnustl_static )
endif()
set( ANDROID_STL "${ANDROID_STL}" CACHE STRING "C++ runtime" )
set( ANDROID_STL_FORCE_FEATURES ON CACHE BOOL "automatically configure rtti and exceptions support based on C++ runtime" )
mark_as_advanced( ANDROID_STL ANDROID_STL_FORCE_FEATURES )

if( BUILD_WITH_ANDROID_NDK )
 if( NOT "${ANDROID_STL}" MATCHES "^(none|system|system_re|gabi\\+\\+_static|gabi\\+\\+_shared|stlport_static|stlport_shared|gnustl_static|gnustl_shared)$")
  message( FATAL_ERROR "ANDROID_STL is set to invalid value \"${ANDROID_STL}\".
The possible values are:
  none           -> Do not configure the runtime.
  system         -> Use the default minimal system C++ runtime library.
  system_re      -> Same as system but with rtti and exceptions.
  gabi++_static  -> Use the GAbi++ runtime as a static library.
  gabi++_shared  -> Use the GAbi++ runtime as a shared library.
  stlport_static -> Use the STLport runtime as a static library.
  stlport_shared -> Use the STLport runtime as a shared library.
  gnustl_static  -> (default) Use the GNU STL as a static library.
  gnustl_shared  -> Use the GNU STL as a shared library.
" )
 endif()
elseif( BUILD_WITH_STANDALONE_TOOLCHAIN )
 if( NOT "${ANDROID_STL}" MATCHES "^(none|gnustl_static|gnustl_shared)$")
  message( FATAL_ERROR "ANDROID_STL is set to invalid value \"${ANDROID_STL}\".
The possible values are:
  none           -> Do not configure the runtime.
  gnustl_static  -> (default) Use the GNU STL as a static library.
  gnustl_shared  -> Use the GNU STL as a shared library.
" )
 endif()
endif()

unset( ANDROID_RTTI )
unset( ANDROID_EXCEPTIONS )
unset( ANDROID_STL_INCLUDE_DIRS )
unset( __libstl )
unset( __libsupcxx )

if( NOT _CMAKE_IN_TRY_COMPILE AND ANDROID_NDK_RELEASE STREQUAL "r7b" AND ARMEABI_V7A AND NOT VFPV3 AND ANDROID_STL MATCHES "gnustl" )
 message( WARNING  "The GNU STL armeabi-v7a binaries from NDK r7b can crash non-NEON devices. The files provided with NDK r7b were not configured properly, resulting in crashes on Tegra2-based devices and others when trying to use certain floating-point functions (e.g., cosf, sinf, expf).
You are strongly recommended to switch to another NDK release.
" )
endif()

if( NOT _CMAKE_IN_TRY_COMPILE AND X86 AND ANDROID_STL MATCHES "gnustl" AND ANDROID_NDK_RELEASE STREQUAL "r6" )
  message( WARNING  "The x86 system header file from NDK r6 has incorrect definition for ptrdiff_t. You are recommended to upgrade to a newer NDK release or manually patch the header:
See https://android.googlesource.com/platform/development.git f907f4f9d4e56ccc8093df6fee54454b8bcab6c2
  diff --git a/ndk/platforms/android-9/arch-x86/include/machine/_types.h b/ndk/platforms/android-9/arch-x86/include/machine/_types.h
  index 5e28c64..65892a1 100644
  --- a/ndk/platforms/android-9/arch-x86/include/machine/_types.h
  +++ b/ndk/platforms/android-9/arch-x86/include/machine/_types.h
  @@ -51,7 +51,11 @@ typedef long int       ssize_t;
   #endif
   #ifndef _PTRDIFF_T
   #define _PTRDIFF_T
  -typedef long           ptrdiff_t;
  +#  ifdef __ANDROID__
  +     typedef int            ptrdiff_t;
  +#  else
  +     typedef long           ptrdiff_t;
  +#  endif
   #endif
" )
endif()


# setup paths and STL for standalone toolchain
if( BUILD_WITH_STANDALONE_TOOLCHAIN )
 set( ANDROID_TOOLCHAIN_ROOT "${ANDROID_STANDALONE_TOOLCHAIN}" )
 set( ANDROID_CLANG_TOOLCHAIN_ROOT "${ANDROID_STANDALONE_TOOLCHAIN}" )
 set( ANDROID_SYSROOT "${ANDROID_STANDALONE_TOOLCHAIN}/sysroot" )

 if( NOT ANDROID_STL STREQUAL "none" )
  set( ANDROID_STL_INCLUDE_DIRS "${ANDROID_STANDALONE_TOOLCHAIN}/include/c++/${ANDROID_COMPILER_VERSION}" )
  if( NOT EXISTS "${ANDROID_STL_INCLUDE_DIRS}" )
   # old location ( pre r8c )
   set( ANDROID_STL_INCLUDE_DIRS "${ANDROID_STANDALONE_TOOLCHAIN}/${ANDROID_TOOLCHAIN_MACHINE_NAME}/include/c++/${ANDROID_COMPILER_VERSION}" )
  endif()
  if( ARMEABI_V7A AND EXISTS "${ANDROID_STL_INCLUDE_DIRS}/${ANDROID_TOOLCHAIN_MACHINE_NAME}/${CMAKE_SYSTEM_PROCESSOR}/bits" )
   list( APPEND ANDROID_STL_INCLUDE_DIRS "${ANDROID_STL_INCLUDE_DIRS}/${ANDROID_TOOLCHAIN_MACHINE_NAME}/${CMAKE_SYSTEM_PROCESSOR}" )
  elseif( ARMEABI AND NOT ANDROID_FORCE_ARM_BUILD AND EXISTS "${ANDROID_STL_INCLUDE_DIRS}/${ANDROID_TOOLCHAIN_MACHINE_NAME}/thumb/bits" )
   list( APPEND ANDROID_STL_INCLUDE_DIRS "${ANDROID_STL_INCLUDE_DIRS}/${ANDROID_TOOLCHAIN_MACHINE_NAME}/thumb" )
  else()
   list( APPEND ANDROID_STL_INCLUDE_DIRS "${ANDROID_STL_INCLUDE_DIRS}/${ANDROID_TOOLCHAIN_MACHINE_NAME}" )
  endif()
  # always search static GNU STL to get the location of libsupc++.a
  if( ARMEABI_V7A AND NOT ANDROID_FORCE_ARM_BUILD AND EXISTS "${ANDROID_STANDALONE_TOOLCHAIN}/${ANDROID_TOOLCHAIN_MACHINE_NAME}/lib/${CMAKE_SYSTEM_PROCESSOR}/thumb/libstdc++.a" )
   set( __libstl "${ANDROID_STANDALONE_TOOLCHAIN}/${ANDROID_TOOLCHAIN_MACHINE_NAME}/lib/${CMAKE_SYSTEM_PROCESSOR}/thumb" )
  elseif( ARMEABI_V7A AND EXISTS "${ANDROID_STANDALONE_TOOLCHAIN}/${ANDROID_TOOLCHAIN_MACHINE_NAME}/lib/${CMAKE_SYSTEM_PROCESSOR}/libstdc++.a" )
   set( __libstl "${ANDROID_STANDALONE_TOOLCHAIN}/${ANDROID_TOOLCHAIN_MACHINE_NAME}/lib/${CMAKE_SYSTEM_PROCESSOR}" )
  elseif( ARMEABI AND NOT ANDROID_FORCE_ARM_BUILD AND EXISTS "${ANDROID_STANDALONE_TOOLCHAIN}/${ANDROID_TOOLCHAIN_MACHINE_NAME}/lib/thumb/libstdc++.a" )
   set( __libstl "${ANDROID_STANDALONE_TOOLCHAIN}/${ANDROID_TOOLCHAIN_MACHINE_NAME}/lib/thumb" )
  elseif( EXISTS "${ANDROID_STANDALONE_TOOLCHAIN}/${ANDROID_TOOLCHAIN_MACHINE_NAME}/lib/libstdc++.a" )
   set( __libstl "${ANDROID_STANDALONE_TOOLCHAIN}/${ANDROID_TOOLCHAIN_MACHINE_NAME}/lib" )
  endif()
  if( __libstl )
   set( __libsupcxx "${__libstl}/libsupc++.a" )
   set( __libstl    "${__libstl}/libstdc++.a" )
  endif()
  if( NOT EXISTS "${__libsupcxx}" )
   message( FATAL_ERROR "The required libstdsupc++.a is missing in your standalone toolchain.
 Usually it happens because of bug in make-standalone-toolchain.sh script from NDK r7, r7b and r7c.
 You need to either upgrade to newer NDK or manually copy
     $ANDROID_NDK/sources/cxx-stl/gnu-libstdc++/libs/${ANDROID_NDK_ABI_NAME}/libsupc++.a
 to
     ${__libsupcxx}
   " )
  endif()
  if( ANDROID_STL STREQUAL "gnustl_shared" )
   if( ARMEABI_V7A AND EXISTS "${ANDROID_STANDALONE_TOOLCHAIN}/${ANDROID_TOOLCHAIN_MACHINE_NAME}/lib/${CMAKE_SYSTEM_PROCESSOR}/libgnustl_shared.so" )
    set( __libstl "${ANDROID_STANDALONE_TOOLCHAIN}/${ANDROID_TOOLCHAIN_MACHINE_NAME}/lib/${CMAKE_SYSTEM_PROCESSOR}/libgnustl_shared.so" )
   elseif( ARMEABI AND NOT ANDROID_FORCE_ARM_BUILD AND EXISTS "${ANDROID_STANDALONE_TOOLCHAIN}/${ANDROID_TOOLCHAIN_MACHINE_NAME}/lib/thumb/libgnustl_shared.so" )
    set( __libstl "${ANDROID_STANDALONE_TOOLCHAIN}/${ANDROID_TOOLCHAIN_MACHINE_NAME}/lib/thumb/libgnustl_shared.so" )
   elseif( EXISTS "${ANDROID_STANDALONE_TOOLCHAIN}/${ANDROID_TOOLCHAIN_MACHINE_NAME}/lib/libgnustl_shared.so" )
    set( __libstl "${ANDROID_STANDALONE_TOOLCHAIN}/${ANDROID_TOOLCHAIN_MACHINE_NAME}/lib/libgnustl_shared.so" )
   endif()
  endif()
 endif()
endif()

# clang
if( "${ANDROID_TOOLCHAIN_NAME}" STREQUAL "standalone-clang" )
 set( ANDROID_COMPILER_IS_CLANG 1 )
 execute_process( COMMAND "${ANDROID_CLANG_TOOLCHAIN_ROOT}/bin/clang${TOOL_OS_SUFFIX}" --version OUTPUT_VARIABLE ANDROID_CLANG_VERSION OUTPUT_STRIP_TRAILING_WHITESPACE )
 string( REGEX MATCH "[0-9]+[.][0-9]+" ANDROID_CLANG_VERSION "${ANDROID_CLANG_VERSION}")
elseif( "${ANDROID_TOOLCHAIN_NAME}" MATCHES "-clang3[.][0-9]?$" )
 string( REGEX MATCH "3[.][0-9]$" ANDROID_CLANG_VERSION "${ANDROID_TOOLCHAIN_NAME}")
 string( REGEX REPLACE "-clang${ANDROID_CLANG_VERSION}$" "-${ANDROID_COMPILER_VERSION}" ANDROID_GCC_TOOLCHAIN_NAME "${ANDROID_TOOLCHAIN_NAME}" )
 if( NOT EXISTS "${ANDROID_NDK_TOOLCHAINS_PATH}/llvm-${ANDROID_CLANG_VERSION}${ANDROID_NDK_TOOLCHAINS_SUBPATH}/bin/clang${TOOL_OS_SUFFIX}" )
  message( FATAL_ERROR "Could not find the Clang compiler driver" )
 endif()
 set( ANDROID_COMPILER_IS_CLANG 1 )
 set( ANDROID_CLANG_TOOLCHAIN_ROOT "${ANDROID_NDK_TOOLCHAINS_PATH}/llvm-${ANDROID_CLANG_VERSION}${ANDROID_NDK_TOOLCHAINS_SUBPATH}" )
else()
 set( ANDROID_GCC_TOOLCHAIN_NAME "${ANDROID_TOOLCHAIN_NAME}" )
 unset( ANDROID_COMPILER_IS_CLANG CACHE )
endif()

string( REPLACE "." "" _clang_name "clang${ANDROID_CLANG_VERSION}" )
if( NOT EXISTS "${ANDROID_CLANG_TOOLCHAIN_ROOT}/bin/${_clang_name}${TOOL_OS_SUFFIX}" )
 set( _clang_name "clang" )
endif()


# setup paths and STL for NDK
if( BUILD_WITH_ANDROID_NDK )
 set( ANDROID_TOOLCHAIN_ROOT "${ANDROID_NDK_TOOLCHAINS_PATH}/${ANDROID_GCC_TOOLCHAIN_NAME}${ANDROID_NDK_TOOLCHAINS_SUBPATH}" )
 set( ANDROID_SYSROOT "${ANDROID_NDK}/platforms/android-${ANDROID_NATIVE_API_LEVEL}/arch-${ANDROID_ARCH_NAME}" )

 if( ANDROID_STL STREQUAL "none" )
  # do nothing
 elseif( ANDROID_STL STREQUAL "system" )
  set( ANDROID_RTTI             OFF )
  set( ANDROID_EXCEPTIONS       OFF )
  set( ANDROID_STL_INCLUDE_DIRS "${ANDROID_NDK}/sources/cxx-stl/system/include" )
 elseif( ANDROID_STL STREQUAL "system_re" )
  set( ANDROID_RTTI             ON )
  set( ANDROID_EXCEPTIONS       ON )
  set( ANDROID_STL_INCLUDE_DIRS "${ANDROID_NDK}/sources/cxx-stl/system/include" )
 elseif( ANDROID_STL MATCHES "gabi" )
  if( ANDROID_NDK_RELEASE_NUM LESS 7000 ) # before r7
   message( FATAL_ERROR "gabi++ is not awailable in your NDK. You have to upgrade to NDK r7 or newer to use gabi++.")
  endif()
  set( ANDROID_RTTI             ON )
  set( ANDROID_EXCEPTIONS       OFF )
  set( ANDROID_STL_INCLUDE_DIRS "${ANDROID_NDK}/sources/cxx-stl/gabi++/include" )
  set( __libstl                 "${ANDROID_NDK}/sources/cxx-stl/gabi++/libs/${ANDROID_NDK_ABI_NAME}/libgabi++_static.a" )
 elseif( ANDROID_STL MATCHES "stlport" )
  if( NOT ANDROID_NDK_RELEASE_NUM LESS 8004 ) # before r8d
   set( ANDROID_EXCEPTIONS       ON )
  else()
   set( ANDROID_EXCEPTIONS       OFF )
  endif()
  if( ANDROID_NDK_RELEASE_NUM LESS 7000 ) # before r7
   set( ANDROID_RTTI            OFF )
  else()
   set( ANDROID_RTTI            ON )
  endif()
  set( ANDROID_STL_INCLUDE_DIRS "${ANDROID_NDK}/sources/cxx-stl/stlport/stlport" )
  set( __libstl                 "${ANDROID_NDK}/sources/cxx-stl/stlport/libs/${ANDROID_NDK_ABI_NAME}/libstlport_static.a" )
 elseif( ANDROID_STL MATCHES "gnustl" )
  set( ANDROID_EXCEPTIONS       ON )
  set( ANDROID_RTTI             ON )
  if( EXISTS "${ANDROID_NDK}/sources/cxx-stl/gnu-libstdc++/${ANDROID_COMPILER_VERSION}" )
   if( ARMEABI_V7A AND ANDROID_COMPILER_VERSION VERSION_EQUAL "4.7" AND ANDROID_NDK_RELEASE STREQUAL "r8d" )
    # gnustl binary for 4.7 compiler is buggy :(
    # TODO: look for right fix
    set( __libstl                "${ANDROID_NDK}/sources/cxx-stl/gnu-libstdc++/4.6" )
   else()
    set( __libstl                "${ANDROID_NDK}/sources/cxx-stl/gnu-libstdc++/${ANDROID_COMPILER_VERSION}" )
   endif()
  else()
   set( __libstl                "${ANDROID_NDK}/sources/cxx-stl/gnu-libstdc++" )
  endif()
  set( ANDROID_STL_INCLUDE_DIRS "${__libstl}/include" "${__libstl}/libs/${ANDROID_NDK_ABI_NAME}/include" "${__libstl}/include/backward" )
  if( EXISTS "${__libstl}/libs/${ANDROID_NDK_ABI_NAME}/libgnustl_static.a" )
   set( __libstl                "${__libstl}/libs/${ANDROID_NDK_ABI_NAME}/libgnustl_static.a" )
  else()
   set( __libstl                "${__libstl}/libs/${ANDROID_NDK_ABI_NAME}/libstdc++.a" )
  endif()
 else()
  message( FATAL_ERROR "Unknown runtime: ${ANDROID_STL}" )
 endif()
 # find libsupc++.a - rtti & exceptions
 if( ANDROID_STL STREQUAL "system_re" OR ANDROID_STL MATCHES "gnustl" )
  set( __libsupcxx "${ANDROID_NDK}/sources/cxx-stl/gnu-libstdc++/${ANDROID_COMPILER_VERSION}/libs/${ANDROID_NDK_ABI_NAME}/libsupc++.a" ) # r8b or newer
  if( NOT EXISTS "${__libsupcxx}" )
   set( __libsupcxx "${ANDROID_NDK}/sources/cxx-stl/gnu-libstdc++/libs/${ANDROID_NDK_ABI_NAME}/libsupc++.a" ) # r7-r8
  endif()
  if( NOT EXISTS "${__libsupcxx}" ) # before r7
   if( ARMEABI_V7A )
    if( ANDROID_FORCE_ARM_BUILD )
     set( __libsupcxx "${ANDROID_TOOLCHAIN_ROOT}/${ANDROID_TOOLCHAIN_MACHINE_NAME}/lib/${CMAKE_SYSTEM_PROCESSOR}/libsupc++.a" )
    else()
     set( __libsupcxx "${ANDROID_TOOLCHAIN_ROOT}/${ANDROID_TOOLCHAIN_MACHINE_NAME}/lib/${CMAKE_SYSTEM_PROCESSOR}/thumb/libsupc++.a" )
    endif()
   elseif( ARMEABI AND NOT ANDROID_FORCE_ARM_BUILD )
    set( __libsupcxx "${ANDROID_TOOLCHAIN_ROOT}/${ANDROID_TOOLCHAIN_MACHINE_NAME}/lib/thumb/libsupc++.a" )
   else()
    set( __libsupcxx "${ANDROID_TOOLCHAIN_ROOT}/${ANDROID_TOOLCHAIN_MACHINE_NAME}/lib/libsupc++.a" )
   endif()
  endif()
  if( NOT EXISTS "${__libsupcxx}")
   message( ERROR "Could not find libsupc++.a for a chosen platform. Either your NDK is not supported or is broken.")
  endif()
 endif()
endif()


# case of shared STL linkage
if( ANDROID_STL MATCHES "shared" AND DEFINED __libstl )
 string( REPLACE "_static.a" "_shared.so" __libstl "${__libstl}" )
 # TODO: check if .so file exists before the renaming
endif()


# ccache support
__INIT_VARIABLE( _ndk_ccache NDK_CCACHE ENV_NDK_CCACHE )
if( _ndk_ccache )
 if( DEFINED NDK_CCACHE AND NOT EXISTS NDK_CCACHE )
  unset( NDK_CCACHE CACHE )
 endif()
 find_program( NDK_CCACHE "${_ndk_ccache}" DOC "The path to ccache binary")
else()
 unset( NDK_CCACHE CACHE )
endif()
unset( _ndk_ccache )


# setup the cross-compiler
if( NOT CMAKE_C_COMPILER )
 if( NDK_CCACHE AND NOT ANDROID_SYSROOT MATCHES "[ ;\"]" )
  set( CMAKE_C_COMPILER   "${NDK_CCACHE}" CACHE PATH "ccache as C compiler" )
  set( CMAKE_CXX_COMPILER "${NDK_CCACHE}" CACHE PATH "ccache as C++ compiler" )
  if( ANDROID_COMPILER_IS_CLANG )
   set( CMAKE_C_COMPILER_ARG1   "${ANDROID_CLANG_TOOLCHAIN_ROOT}/bin/${_clang_name}${TOOL_OS_SUFFIX}"   CACHE PATH "C compiler")
   set( CMAKE_CXX_COMPILER_ARG1 "${ANDROID_CLANG_TOOLCHAIN_ROOT}/bin/${_clang_name}++${TOOL_OS_SUFFIX}" CACHE PATH "C++ compiler")
  else()
   set( CMAKE_C_COMPILER_ARG1   "${ANDROID_TOOLCHAIN_ROOT}/bin/${ANDROID_TOOLCHAIN_MACHINE_NAME}-gcc${TOOL_OS_SUFFIX}" CACHE PATH "C compiler")
   set( CMAKE_CXX_COMPILER_ARG1 "${ANDROID_TOOLCHAIN_ROOT}/bin/${ANDROID_TOOLCHAIN_MACHINE_NAME}-g++${TOOL_OS_SUFFIX}" CACHE PATH "C++ compiler")
  endif()
 else()
  if( ANDROID_COMPILER_IS_CLANG )
   set( CMAKE_C_COMPILER   "${ANDROID_CLANG_TOOLCHAIN_ROOT}/bin/${_clang_name}${TOOL_OS_SUFFIX}"   CACHE PATH "C compiler")
   set( CMAKE_CXX_COMPILER "${ANDROID_CLANG_TOOLCHAIN_ROOT}/bin/${_clang_name}++${TOOL_OS_SUFFIX}" CACHE PATH "C++ compiler")
  else()
   set( CMAKE_C_COMPILER   "${ANDROID_TOOLCHAIN_ROOT}/bin/${ANDROID_TOOLCHAIN_MACHINE_NAME}-gcc${TOOL_OS_SUFFIX}"    CACHE PATH "C compiler" )
   set( CMAKE_CXX_COMPILER "${ANDROID_TOOLCHAIN_ROOT}/bin/${ANDROID_TOOLCHAIN_MACHINE_NAME}-g++${TOOL_OS_SUFFIX}"    CACHE PATH "C++ compiler" )
  endif()
 endif()
 set( CMAKE_ASM_COMPILER "${ANDROID_TOOLCHAIN_ROOT}/bin/${ANDROID_TOOLCHAIN_MACHINE_NAME}-gcc${TOOL_OS_SUFFIX}"     CACHE PATH "assembler" )
 set( CMAKE_STRIP        "${ANDROID_TOOLCHAIN_ROOT}/bin/${ANDROID_TOOLCHAIN_MACHINE_NAME}-strip${TOOL_OS_SUFFIX}"   CACHE PATH "strip" )
 set( CMAKE_AR           "${ANDROID_TOOLCHAIN_ROOT}/bin/${ANDROID_TOOLCHAIN_MACHINE_NAME}-ar${TOOL_OS_SUFFIX}"      CACHE PATH "archive" )
 set( CMAKE_LINKER       "${ANDROID_TOOLCHAIN_ROOT}/bin/${ANDROID_TOOLCHAIN_MACHINE_NAME}-ld${TOOL_OS_SUFFIX}"      CACHE PATH "linker" )
 set( CMAKE_NM           "${ANDROID_TOOLCHAIN_ROOT}/bin/${ANDROID_TOOLCHAIN_MACHINE_NAME}-nm${TOOL_OS_SUFFIX}"      CACHE PATH "nm" )
 set( CMAKE_OBJCOPY      "${ANDROID_TOOLCHAIN_ROOT}/bin/${ANDROID_TOOLCHAIN_MACHINE_NAME}-objcopy${TOOL_OS_SUFFIX}" CACHE PATH "objcopy" )
 set( CMAKE_OBJDUMP      "${ANDROID_TOOLCHAIN_ROOT}/bin/${ANDROID_TOOLCHAIN_MACHINE_NAME}-objdump${TOOL_OS_SUFFIX}" CACHE PATH "objdump" )
 set( CMAKE_RANLIB       "${ANDROID_TOOLCHAIN_ROOT}/bin/${ANDROID_TOOLCHAIN_MACHINE_NAME}-ranlib${TOOL_OS_SUFFIX}"  CACHE PATH "ranlib" )
endif()

set( _CMAKE_TOOLCHAIN_PREFIX "${ANDROID_TOOLCHAIN_MACHINE_NAME}-" )
if( CMAKE_VERSION VERSION_LESS 2.8.5 )
 set( CMAKE_ASM_COMPILER_ARG1 "-c" )
endif()
if( APPLE )
 find_program( CMAKE_INSTALL_NAME_TOOL NAMES install_name_tool )
 if( NOT CMAKE_INSTALL_NAME_TOOL )
  message( FATAL_ERROR "Could not find install_name_tool, please check your installation." )
 endif()
 mark_as_advanced( CMAKE_INSTALL_NAME_TOOL )
endif()

# Force set compilers because standard identification works badly for us
include( CMakeForceCompiler )
CMAKE_FORCE_C_COMPILER( "${CMAKE_C_COMPILER}" GNU )
if( ANDROID_COMPILER_IS_CLANG )
 set( CMAKE_C_COMPILER_ID Clang )
endif()
set( CMAKE_C_PLATFORM_ID Linux )
if( X86_64 OR MIPS64 OR ARM64_V8A )
 set( CMAKE_C_SIZEOF_DATA_PTR 8 )
else()
 set( CMAKE_C_SIZEOF_DATA_PTR 4 )
endif()
set( CMAKE_C_HAS_ISYSROOT 1 )
set( CMAKE_C_COMPILER_ABI ELF )
CMAKE_FORCE_CXX_COMPILER( "${CMAKE_CXX_COMPILER}" GNU )
if( ANDROID_COMPILER_IS_CLANG )
 set( CMAKE_CXX_COMPILER_ID Clang)
endif()
set( CMAKE_CXX_PLATFORM_ID Linux )
set( CMAKE_CXX_SIZEOF_DATA_PTR ${CMAKE_C_SIZEOF_DATA_PTR} )
set( CMAKE_CXX_HAS_ISYSROOT 1 )
set( CMAKE_CXX_COMPILER_ABI ELF )
set( CMAKE_CXX_SOURCE_FILE_EXTENSIONS cc cp cxx cpp CPP c++ C )
# force ASM compiler (required for CMake < 2.8.5)
set( CMAKE_ASM_COMPILER_ID_RUN TRUE )
set( CMAKE_ASM_COMPILER_ID GNU )
set( CMAKE_ASM_COMPILER_WORKS TRUE )
set( CMAKE_ASM_COMPILER_FORCED TRUE )
set( CMAKE_COMPILER_IS_GNUASM 1)
set( CMAKE_ASM_SOURCE_FILE_EXTENSIONS s S asm )

foreach( lang C CXX ASM )
 if( ANDROID_COMPILER_IS_CLANG )
  set( CMAKE_${lang}_COMPILER_VERSION ${ANDROID_CLANG_VERSION} )
 else()
  set( CMAKE_${lang}_COMPILER_VERSION ${ANDROID_COMPILER_VERSION} )
 endif()
endforeach()

# flags and definitions
remove_definitions( -DANDROID )
add_definitions( -DANDROID )

if( ANDROID_SYSROOT MATCHES "[ ;\"]" )
 if( CMAKE_HOST_WIN32 )
  # try to convert path to 8.3 form
  file( WRITE "${CMAKE_BINARY_DIR}${CMAKE_FILES_DIRECTORY}/cvt83.cmd" "@echo %~s1" )
  execute_process( COMMAND "$ENV{ComSpec}" /c "${CMAKE_BINARY_DIR}${CMAKE_FILES_DIRECTORY}/cvt83.cmd" "${ANDROID_SYSROOT}"
                   OUTPUT_VARIABLE __path OUTPUT_STRIP_TRAILING_WHITESPACE
                   RESULT_VARIABLE __result ERROR_QUIET )
  if( __result EQUAL 0 )
   file( TO_CMAKE_PATH "${__path}" ANDROID_SYSROOT )
   set( ANDROID_CXX_FLAGS "--sysroot=${ANDROID_SYSROOT}" )
  else()
   set( ANDROID_CXX_FLAGS "--sysroot=\"${ANDROID_SYSROOT}\"" )
  endif()
 else()
  set( ANDROID_CXX_FLAGS "'--sysroot=${ANDROID_SYSROOT}'" )
 endif()
 if( NOT _CMAKE_IN_TRY_COMPILE )
  # quotes can break try_compile and compiler identification
  message(WARNING "Path to your Android NDK (or toolchain) has non-alphanumeric symbols.\nThe build might be broken.\n")
 endif()
else()
 set( ANDROID_CXX_FLAGS "--sysroot=${ANDROID_SYSROOT}" )
endif()

# NDK flags
if (ARM64_V8A )
 set( ANDROID_CXX_FLAGS         "${ANDROID_CXX_FLAGS} -funwind-tables" )
 set( ANDROID_CXX_FLAGS_RELEASE "-fomit-frame-pointer -fstrict-aliasing" )
 set( ANDROID_CXX_FLAGS_DEBUG   "-fno-omit-frame-pointer -fno-strict-aliasing" )
 if( NOT ANDROID_COMPILER_IS_CLANG )
  set( ANDROID_CXX_FLAGS_RELEASE "${ANDROID_CXX_FLAGS_RELEASE} -funswitch-loops -finline-limit=300" )
 endif()
elseif( ARMEABI OR ARMEABI_V7A)
 set( ANDROID_CXX_FLAGS "${ANDROID_CXX_FLAGS} -funwind-tables" )
 if( NOT ANDROID_FORCE_ARM_BUILD AND NOT ARMEABI_V6 )
  set( ANDROID_CXX_FLAGS_RELEASE "-mthumb -fomit-frame-pointer -fno-strict-aliasing" )
  set( ANDROID_CXX_FLAGS_DEBUG   "-marm -fno-omit-frame-pointer -fno-strict-aliasing" )
  if( NOT ANDROID_COMPILER_IS_CLANG )
   set( ANDROID_CXX_FLAGS "${ANDROID_CXX_FLAGS} -finline-limit=64" )
  endif()
 else()
  # always compile ARMEABI_V6 in arm mode; otherwise there is no difference from ARMEABI
  set( ANDROID_CXX_FLAGS_RELEASE "-marm -fomit-frame-pointer -fstrict-aliasing" )
  set( ANDROID_CXX_FLAGS_DEBUG   "-marm -fno-omit-frame-pointer -fno-strict-aliasing" )
  if( NOT ANDROID_COMPILER_IS_CLANG )
   set( ANDROID_CXX_FLAGS "${ANDROID_CXX_FLAGS} -funswitch-loops -finline-limit=300" )
  endif()
 endif()
elseif( X86 OR X86_64 )
 set( ANDROID_CXX_FLAGS "${ANDROID_CXX_FLAGS} -funwind-tables" )
 if( NOT ANDROID_COMPILER_IS_CLANG )
  set( ANDROID_CXX_FLAGS "${ANDROID_CXX_FLAGS} -funswitch-loops -finline-limit=300" )
 endif()
 set( ANDROID_CXX_FLAGS_RELEASE "-fomit-frame-pointer -fstrict-aliasing" )
 set( ANDROID_CXX_FLAGS_DEBUG   "-fno-omit-frame-pointer -fno-strict-aliasing" )
elseif( MIPS OR MIPS64 )
 set( ANDROID_CXX_FLAGS         "${ANDROID_CXX_FLAGS} -fno-strict-aliasing -finline-functions -funwind-tables -fmessage-length=0" )
 set( ANDROID_CXX_FLAGS_RELEASE "-fomit-frame-pointer" )
 set( ANDROID_CXX_FLAGS_DEBUG   "-fno-omit-frame-pointer" )
 if( NOT ANDROID_COMPILER_IS_CLANG )
  set( ANDROID_CXX_FLAGS "${ANDROID_CXX_FLAGS} -fno-inline-functions-called-once -fgcse-after-reload -frerun-cse-after-loop -frename-registers" )
  set( ANDROID_CXX_FLAGS_RELEASE "${ANDROID_CXX_FLAGS_RELEASE} -funswitch-loops -finline-limit=300" )
 endif()
elseif()
 set( ANDROID_CXX_FLAGS_RELEASE "" )
 set( ANDROID_CXX_FLAGS_DEBUG   "" )
endif()

set( ANDROID_CXX_FLAGS "${ANDROID_CXX_FLAGS} -fsigned-char" ) # good/necessary when porting desktop libraries

if( NOT X86 AND NOT ANDROID_COMPILER_IS_CLANG )
 set( ANDROID_CXX_FLAGS "-Wno-psabi ${ANDROID_CXX_FLAGS}" )
endif()

if( NOT ANDROID_COMPILER_VERSION VERSION_LESS "4.6" )
 set( ANDROID_CXX_FLAGS "${ANDROID_CXX_FLAGS} -no-canonical-prefixes" ) # see https://android-review.googlesource.com/#/c/47564/
endif()

# ABI-specific flags
if( ARMEABI_V7A )
 set( ANDROID_CXX_FLAGS "${ANDROID_CXX_FLAGS} -march=armv7-a -mfloat-abi=softfp" )
 if( NEON )
  set( ANDROID_CXX_FLAGS "${ANDROID_CXX_FLAGS} -mfpu=neon" )
 elseif( VFPV3 )
  set( ANDROID_CXX_FLAGS "${ANDROID_CXX_FLAGS} -mfpu=vfpv3" )
 else()
  set( ANDROID_CXX_FLAGS "${ANDROID_CXX_FLAGS} -mfpu=vfpv3-d16" )
 endif()
elseif( ARMEABI_V6 )
 set( ANDROID_CXX_FLAGS "${ANDROID_CXX_FLAGS} -march=armv6 -mfloat-abi=softfp -mfpu=vfp" ) # vfp == vfpv2
elseif( ARMEABI )
 set( ANDROID_CXX_FLAGS "${ANDROID_CXX_FLAGS} -march=armv5te -mtune=xscale -msoft-float" )
endif()

if( ANDROID_STL MATCHES "gnustl" AND (EXISTS "${__libstl}" OR EXISTS "${__libsupcxx}") )
 set( CMAKE_CXX_CREATE_SHARED_LIBRARY "<CMAKE_C_COMPILER> <CMAKE_SHARED_LIBRARY_CXX_FLAGS> <LANGUAGE_COMPILE_FLAGS> <LINK_FLAGS> <CMAKE_SHARED_LIBRARY_CREATE_CXX_FLAGS> <CMAKE_SHARED_LIBRARY_SONAME_CXX_FLAG><TARGET_SONAME> -o <TARGET> <OBJECTS> <LINK_LIBRARIES>" )
 set( CMAKE_CXX_CREATE_SHARED_MODULE  "<CMAKE_C_COMPILER> <CMAKE_SHARED_LIBRARY_CXX_FLAGS> <LANGUAGE_COMPILE_FLAGS> <LINK_FLAGS> <CMAKE_SHARED_LIBRARY_CREATE_CXX_FLAGS> <CMAKE_SHARED_LIBRARY_SONAME_CXX_FLAG><TARGET_SONAME> -o <TARGET> <OBJECTS> <LINK_LIBRARIES>" )
 set( CMAKE_CXX_LINK_EXECUTABLE       "<CMAKE_C_COMPILER> <FLAGS> <CMAKE_CXX_LINK_FLAGS> <LINK_FLAGS> <OBJECTS> -o <TARGET> <LINK_LIBRARIES>" )
else()
 set( CMAKE_CXX_CREATE_SHARED_LIBRARY "<CMAKE_CXX_COMPILER> <CMAKE_SHARED_LIBRARY_CXX_FLAGS> <LANGUAGE_COMPILE_FLAGS> <LINK_FLAGS> <CMAKE_SHARED_LIBRARY_CREATE_CXX_FLAGS> <CMAKE_SHARED_LIBRARY_SONAME_CXX_FLAG><TARGET_SONAME> -o <TARGET> <OBJECTS> <LINK_LIBRARIES>" )
 set( CMAKE_CXX_CREATE_SHARED_MODULE  "<CMAKE_CXX_COMPILER> <CMAKE_SHARED_LIBRARY_CXX_FLAGS> <LANGUAGE_COMPILE_FLAGS> <LINK_FLAGS> <CMAKE_SHARED_LIBRARY_CREATE_CXX_FLAGS> <CMAKE_SHARED_LIBRARY_SONAME_CXX_FLAG><TARGET_SONAME> -o <TARGET> <OBJECTS> <LINK_LIBRARIES>" )
 set( CMAKE_CXX_LINK_EXECUTABLE       "<CMAKE_CXX_COMPILER> <FLAGS> <CMAKE_CXX_LINK_FLAGS> <LINK_FLAGS> <OBJECTS> -o <TARGET> <LINK_LIBRARIES>" )
endif()

# STL
if( EXISTS "${__libstl}" OR EXISTS "${__libsupcxx}" )
 if( EXISTS "${__libstl}" )
  set( CMAKE_CXX_CREATE_SHARED_LIBRARY "${CMAKE_CXX_CREATE_SHARED_LIBRARY} \"${__libstl}\"" )
  set( CMAKE_CXX_CREATE_SHARED_MODULE  "${CMAKE_CXX_CREATE_SHARED_MODULE} \"${__libstl}\"" )
  set( CMAKE_CXX_LINK_EXECUTABLE       "${CMAKE_CXX_LINK_EXECUTABLE} \"${__libstl}\"" )
 endif()
 if( EXISTS "${__libsupcxx}" )
  set( CMAKE_CXX_CREATE_SHARED_LIBRARY "${CMAKE_CXX_CREATE_SHARED_LIBRARY} \"${__libsupcxx}\"" )
  set( CMAKE_CXX_CREATE_SHARED_MODULE  "${CMAKE_CXX_CREATE_SHARED_MODULE} \"${__libsupcxx}\"" )
  set( CMAKE_CXX_LINK_EXECUTABLE       "${CMAKE_CXX_LINK_EXECUTABLE} \"${__libsupcxx}\"" )
  # C objects:
  set( CMAKE_C_CREATE_SHARED_LIBRARY "<CMAKE_C_COMPILER> <CMAKE_SHARED_LIBRARY_C_FLAGS> <LANGUAGE_COMPILE_FLAGS> <LINK_FLAGS> <CMAKE_SHARED_LIBRARY_CREATE_C_FLAGS> <CMAKE_SHARED_LIBRARY_SONAME_C_FLAG><TARGET_SONAME> -o <TARGET> <OBJECTS> <LINK_LIBRARIES>" )
  set( CMAKE_C_CREATE_SHARED_MODULE  "<CMAKE_C_COMPILER> <CMAKE_SHARED_LIBRARY_C_FLAGS> <LANGUAGE_COMPILE_FLAGS> <LINK_FLAGS> <CMAKE_SHARED_LIBRARY_CREATE_C_FLAGS> <CMAKE_SHARED_LIBRARY_SONAME_C_FLAG><TARGET_SONAME> -o <TARGET> <OBJECTS> <LINK_LIBRARIES>" )
  set( CMAKE_C_LINK_EXECUTABLE       "<CMAKE_C_COMPILER> <FLAGS> <CMAKE_C_LINK_FLAGS> <LINK_FLAGS> <OBJECTS> -o <TARGET> <LINK_LIBRARIES>" )
  set( CMAKE_C_CREATE_SHARED_LIBRARY "${CMAKE_C_CREATE_SHARED_LIBRARY} \"${__libsupcxx}\"" )
  set( CMAKE_C_CREATE_SHARED_MODULE  "${CMAKE_C_CREATE_SHARED_MODULE} \"${__libsupcxx}\"" )
  set( CMAKE_C_LINK_EXECUTABLE       "${CMAKE_C_LINK_EXECUTABLE} \"${__libsupcxx}\"" )
 endif()
 if( ANDROID_STL MATCHES "gnustl" )
  if( NOT EXISTS "${ANDROID_LIBM_PATH}" )
   set( ANDROID_LIBM_PATH -lm )
  endif()
  set( CMAKE_CXX_CREATE_SHARED_LIBRARY "${CMAKE_CXX_CREATE_SHARED_LIBRARY} ${ANDROID_LIBM_PATH}" )
  set( CMAKE_CXX_CREATE_SHARED_MODULE  "${CMAKE_CXX_CREATE_SHARED_MODULE} ${ANDROID_LIBM_PATH}" )
  set( CMAKE_CXX_LINK_EXECUTABLE       "${CMAKE_CXX_LINK_EXECUTABLE} ${ANDROID_LIBM_PATH}" )
 endif()
endif()

# variables controlling optional build flags
if( ANDROID_NDK_RELEASE_NUM LESS 7000 ) # before r7
 # libGLESv2.so in NDK's prior to r7 refers to missing external symbols.
 # So this flag option is required for all projects using OpenGL from native.
 __INIT_VARIABLE( ANDROID_SO_UNDEFINED                      VALUES ON )
else()
 __INIT_VARIABLE( ANDROID_SO_UNDEFINED                      VALUES OFF )
endif()
__INIT_VARIABLE( ANDROID_NO_UNDEFINED                       VALUES ON )
__INIT_VARIABLE( ANDROID_FUNCTION_LEVEL_LINKING             VALUES ON )
__INIT_VARIABLE( ANDROID_GOLD_LINKER                        VALUES ON )
__INIT_VARIABLE( ANDROID_NOEXECSTACK                        VALUES ON )
__INIT_VARIABLE( ANDROID_RELRO                              VALUES ON )

set( ANDROID_NO_UNDEFINED           ${ANDROID_NO_UNDEFINED}           CACHE BOOL "Show all undefined symbols as linker errors" )
set( ANDROID_SO_UNDEFINED           ${ANDROID_SO_UNDEFINED}           CACHE BOOL "Allows or disallows undefined symbols in shared libraries" )
set( ANDROID_FUNCTION_LEVEL_LINKING ${ANDROID_FUNCTION_LEVEL_LINKING} CACHE BOOL "Put each function in separate section and enable garbage collection of unused input sections at link time" )
set( ANDROID_GOLD_LINKER            ${ANDROID_GOLD_LINKER}            CACHE BOOL "Enables gold linker" )
set( ANDROID_NOEXECSTACK            ${ANDROID_NOEXECSTACK}            CACHE BOOL "Allows or disallows undefined symbols in shared libraries" )
set( ANDROID_RELRO                  ${ANDROID_RELRO}                  CACHE BOOL "Enables RELRO - a memory corruption mitigation technique" )
mark_as_advanced( ANDROID_NO_UNDEFINED ANDROID_SO_UNDEFINED ANDROID_FUNCTION_LEVEL_LINKING ANDROID_GOLD_LINKER ANDROID_NOEXECSTACK ANDROID_RELRO )

# linker flags
set( ANDROID_LINKER_FLAGS "" )

if( ARMEABI_V7A )
 # this is *required* to use the following linker flags that routes around
 # a CPU bug in some Cortex-A8 implementations:
 set( ANDROID_LINKER_FLAGS "${ANDROID_LINKER_FLAGS} -Wl,--fix-cortex-a8" )
endif()

if( ANDROID_NO_UNDEFINED )
 if( MIPS )
  # there is some sysroot-related problem in mips linker...
  if( NOT ANDROID_SYSROOT MATCHES "[ ;\"]" )
   set( ANDROID_LINKER_FLAGS "${ANDROID_LINKER_FLAGS} -Wl,--no-undefined -Wl,-rpath-link,${ANDROID_SYSROOT}/usr/lib" )
  endif()
 else()
  set( ANDROID_LINKER_FLAGS "${ANDROID_LINKER_FLAGS} -Wl,--no-undefined" )
 endif()
endif()

if( ANDROID_SO_UNDEFINED )
 set( ANDROID_LINKER_FLAGS "${ANDROID_LINKER_FLAGS} -Wl,-allow-shlib-undefined" )
endif()

if( ANDROID_FUNCTION_LEVEL_LINKING )
 set( ANDROID_CXX_FLAGS    "${ANDROID_CXX_FLAGS} -fdata-sections -ffunction-sections" )
 set( ANDROID_LINKER_FLAGS "${ANDROID_LINKER_FLAGS} -Wl,--gc-sections" )
endif()

if( ANDROID_COMPILER_VERSION VERSION_EQUAL "4.6" )
 if( ANDROID_GOLD_LINKER AND (CMAKE_HOST_UNIX OR ANDROID_NDK_RELEASE_NUM GREATER 8002) AND (ARMEABI OR ARMEABI_V7A OR X86) )
  set( ANDROID_LINKER_FLAGS "${ANDROID_LINKER_FLAGS} -fuse-ld=gold" )
 elseif( ANDROID_NDK_RELEASE_NUM GREATER 8002 ) # after r8b
  set( ANDROID_LINKER_FLAGS "${ANDROID_LINKER_FLAGS} -fuse-ld=bfd" )
 elseif( ANDROID_NDK_RELEASE STREQUAL "r8b" AND ARMEABI AND NOT _CMAKE_IN_TRY_COMPILE )
  message( WARNING "The default bfd linker from arm GCC 4.6 toolchain can fail with 'unresolvable R_ARM_THM_CALL relocation' error message. See https://code.google.com/p/android/issues/detail?id=35342
  On Linux and OS X host platform you can workaround this problem using gold linker (default).
  Rerun cmake with -DANDROID_GOLD_LINKER=ON option in case of problems.
" )
 endif()
endif() # version 4.6

if( ANDROID_NOEXECSTACK )
 if( ANDROID_COMPILER_IS_CLANG )
  set( ANDROID_CXX_FLAGS    "${ANDROID_CXX_FLAGS} -Xclang -mnoexecstack" )
 else()
  set( ANDROID_CXX_FLAGS    "${ANDROID_CXX_FLAGS} -Wa,--noexecstack" )
 endif()
 set( ANDROID_LINKER_FLAGS "${ANDROID_LINKER_FLAGS} -Wl,-z,noexecstack" )
endif()

if( ANDROID_RELRO )
 set( ANDROID_LINKER_FLAGS "${ANDROID_LINKER_FLAGS} -Wl,-z,relro -Wl,-z,now" )
endif()

if( ANDROID_COMPILER_IS_CLANG )
 set( ANDROID_CXX_FLAGS "-target ${ANDROID_LLVM_TRIPLE} -Qunused-arguments ${ANDROID_CXX_FLAGS}" )
 if( BUILD_WITH_ANDROID_NDK )
  set( ANDROID_CXX_FLAGS "-gcc-toolchain ${ANDROID_TOOLCHAIN_ROOT} ${ANDROID_CXX_FLAGS}" )
 endif()
endif()

# cache flags
set( CMAKE_CXX_FLAGS           ""                        CACHE STRING "c++ flags" )
set( CMAKE_C_FLAGS             ""                        CACHE STRING "c flags" )
set( CMAKE_CXX_FLAGS_RELEASE   "-O3 -DNDEBUG"            CACHE STRING "c++ Release flags" )
set( CMAKE_C_FLAGS_RELEASE     "-O3 -DNDEBUG"            CACHE STRING "c Release flags" )
set( CMAKE_CXX_FLAGS_DEBUG     "-O0 -g -DDEBUG -D_DEBUG" CACHE STRING "c++ Debug flags" )
set( CMAKE_C_FLAGS_DEBUG       "-O0 -g -DDEBUG -D_DEBUG" CACHE STRING "c Debug flags" )
set( CMAKE_SHARED_LINKER_FLAGS ""                        CACHE STRING "shared linker flags" )
set( CMAKE_MODULE_LINKER_FLAGS ""                        CACHE STRING "module linker flags" )
set( CMAKE_EXE_LINKER_FLAGS    "-Wl,-z,nocopyreloc"      CACHE STRING "executable linker flags" )

# put flags to cache (for debug purpose only)
set( ANDROID_CXX_FLAGS         "${ANDROID_CXX_FLAGS}"         CACHE INTERNAL "Android specific c/c++ flags" )
set( ANDROID_CXX_FLAGS_RELEASE "${ANDROID_CXX_FLAGS_RELEASE}" CACHE INTERNAL "Android specific c/c++ Release flags" )
set( ANDROID_CXX_FLAGS_DEBUG   "${ANDROID_CXX_FLAGS_DEBUG}"   CACHE INTERNAL "Android specific c/c++ Debug flags" )
set( ANDROID_LINKER_FLAGS      "${ANDROID_LINKER_FLAGS}"      CACHE INTERNAL "Android specific c/c++ linker flags" )

# finish flags
set( CMAKE_CXX_FLAGS           "${ANDROID_CXX_FLAGS} ${CMAKE_CXX_FLAGS}" )
set( CMAKE_C_FLAGS             "${ANDROID_CXX_FLAGS} ${CMAKE_C_FLAGS}" )
set( CMAKE_CXX_FLAGS_RELEASE   "${ANDROID_CXX_FLAGS_RELEASE} ${CMAKE_CXX_FLAGS_RELEASE}" )
set( CMAKE_C_FLAGS_RELEASE     "${ANDROID_CXX_FLAGS_RELEASE} ${CMAKE_C_FLAGS_RELEASE}" )
set( CMAKE_CXX_FLAGS_DEBUG     "${ANDROID_CXX_FLAGS_DEBUG} ${CMAKE_CXX_FLAGS_DEBUG}" )
set( CMAKE_C_FLAGS_DEBUG       "${ANDROID_CXX_FLAGS_DEBUG} ${CMAKE_C_FLAGS_DEBUG}" )
set( CMAKE_SHARED_LINKER_FLAGS "${ANDROID_LINKER_FLAGS} ${CMAKE_SHARED_LINKER_FLAGS}" )
set( CMAKE_MODULE_LINKER_FLAGS "${ANDROID_LINKER_FLAGS} ${CMAKE_MODULE_LINKER_FLAGS}" )
set( CMAKE_EXE_LINKER_FLAGS    "${ANDROID_LINKER_FLAGS} ${CMAKE_EXE_LINKER_FLAGS}" )

if( MIPS AND BUILD_WITH_ANDROID_NDK AND ANDROID_NDK_RELEASE STREQUAL "r8" )
 set( CMAKE_SHARED_LINKER_FLAGS "-Wl,-T,${ANDROID_NDK_TOOLCHAINS_PATH}/${ANDROID_GCC_TOOLCHAIN_NAME}/mipself.xsc ${CMAKE_SHARED_LINKER_FLAGS}" )
 set( CMAKE_MODULE_LINKER_FLAGS "-Wl,-T,${ANDROID_NDK_TOOLCHAINS_PATH}/${ANDROID_GCC_TOOLCHAIN_NAME}/mipself.xsc ${CMAKE_MODULE_LINKER_FLAGS}" )
 set( CMAKE_EXE_LINKER_FLAGS    "-Wl,-T,${ANDROID_NDK_TOOLCHAINS_PATH}/${ANDROID_GCC_TOOLCHAIN_NAME}/mipself.x ${CMAKE_EXE_LINKER_FLAGS}" )
endif()

# pie/pic
if( NOT (ANDROID_NATIVE_API_LEVEL LESS 16) AND (NOT DEFINED ANDROID_APP_PIE OR ANDROID_APP_PIE) AND (CMAKE_VERSION VERSION_GREATER 2.8.8) )
 set( CMAKE_POSITION_INDEPENDENT_CODE TRUE )
 set( CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -fPIE -pie")
else()
 set( CMAKE_POSITION_INDEPENDENT_CODE FALSE )
 set( CMAKE_CXX_FLAGS "-fpic ${CMAKE_CXX_FLAGS}" )
 set( CMAKE_C_FLAGS   "-fpic ${CMAKE_C_FLAGS}" )
endif()

# configure rtti
if( DEFINED ANDROID_RTTI AND ANDROID_STL_FORCE_FEATURES )
 if( ANDROID_RTTI )
  set( CMAKE_CXX_FLAGS "-frtti ${CMAKE_CXX_FLAGS}" )
 else()
  set( CMAKE_CXX_FLAGS "-fno-rtti ${CMAKE_CXX_FLAGS}" )
 endif()
endif()

# configure exceptios
if( DEFINED ANDROID_EXCEPTIONS AND ANDROID_STL_FORCE_FEATURES )
 if( ANDROID_EXCEPTIONS )
  set( CMAKE_CXX_FLAGS "-fexceptions ${CMAKE_CXX_FLAGS}" )
  set( CMAKE_C_FLAGS "-fexceptions ${CMAKE_C_FLAGS}" )
 else()
  set( CMAKE_CXX_FLAGS "-fno-exceptions ${CMAKE_CXX_FLAGS}" )
  set( CMAKE_C_FLAGS "-fno-exceptions ${CMAKE_C_FLAGS}" )
 endif()
endif()

# global includes and link directories
include_directories( SYSTEM "${ANDROID_SYSROOT}/usr/include" ${ANDROID_STL_INCLUDE_DIRS} )
get_filename_component(__android_install_path "${CMAKE_INSTALL_PREFIX}/libs/${ANDROID_NDK_ABI_NAME}" ABSOLUTE) # avoid CMP0015 policy warning
link_directories( "${__android_install_path}" )

# detect if need link crtbegin_so.o explicitly
if( NOT DEFINED ANDROID_EXPLICIT_CRT_LINK )
 set( __cmd "${CMAKE_CXX_CREATE_SHARED_LIBRARY}" )
 string( REPLACE "<CMAKE_CXX_COMPILER>" "${CMAKE_CXX_COMPILER} ${CMAKE_CXX_COMPILER_ARG1}" __cmd "${__cmd}" )
 string( REPLACE "<CMAKE_C_COMPILER>"   "${CMAKE_C_COMPILER} ${CMAKE_C_COMPILER_ARG1}"   __cmd "${__cmd}" )
 string( REPLACE "<CMAKE_SHARED_LIBRARY_CXX_FLAGS>" "${CMAKE_CXX_FLAGS}" __cmd "${__cmd}" )
 string( REPLACE "<LANGUAGE_COMPILE_FLAGS>" "" __cmd "${__cmd}" )
 string( REPLACE "<LINK_FLAGS>" "${CMAKE_SHARED_LINKER_FLAGS}" __cmd "${__cmd}" )
 string( REPLACE "<CMAKE_SHARED_LIBRARY_CREATE_CXX_FLAGS>" "-shared" __cmd "${__cmd}" )
 string( REPLACE "<CMAKE_SHARED_LIBRARY_SONAME_CXX_FLAG>" "" __cmd "${__cmd}" )
 string( REPLACE "<TARGET_SONAME>" "" __cmd "${__cmd}" )
 string( REPLACE "<TARGET>" "${CMAKE_BINARY_DIR}${CMAKE_FILES_DIRECTORY}/toolchain_crtlink_test.so" __cmd "${__cmd}" )
 string( REPLACE "<OBJECTS>" "\"${ANDROID_SYSROOT}/usr/lib/crtbegin_so.o\"" __cmd "${__cmd}" )
 string( REPLACE "<LINK_LIBRARIES>" "" __cmd "${__cmd}" )
 separate_arguments( __cmd )
 foreach( __var ANDROID_NDK ANDROID_NDK_TOOLCHAINS_PATH ANDROID_STANDALONE_TOOLCHAIN )
  if( ${__var} )
   set( __tmp "${${__var}}" )
   separate_arguments( __tmp )
   string( REPLACE "${__tmp}" "${${__var}}" __cmd "${__cmd}")
  endif()
 endforeach()
 string( REPLACE "'" "" __cmd "${__cmd}" )
 string( REPLACE "\"" "" __cmd "${__cmd}" )
 execute_process( COMMAND ${__cmd} RESULT_VARIABLE __cmd_result OUTPUT_QUIET ERROR_QUIET )
 if( __cmd_result EQUAL 0 )
  set( ANDROID_EXPLICIT_CRT_LINK ON )
 else()
  set( ANDROID_EXPLICIT_CRT_LINK OFF )
 endif()
endif()

if( ANDROID_EXPLICIT_CRT_LINK )
 set( CMAKE_CXX_CREATE_SHARED_LIBRARY "${CMAKE_CXX_CREATE_SHARED_LIBRARY} \"${ANDROID_SYSROOT}/usr/lib/crtbegin_so.o\"" )
 set( CMAKE_CXX_CREATE_SHARED_MODULE  "${CMAKE_CXX_CREATE_SHARED_MODULE} \"${ANDROID_SYSROOT}/usr/lib/crtbegin_so.o\"" )
endif()

# setup output directories
set( CMAKE_INSTALL_PREFIX "${ANDROID_TOOLCHAIN_ROOT}/user" CACHE STRING "path for installing" )

if( DEFINED LIBRARY_OUTPUT_PATH_ROOT
      OR EXISTS "${CMAKE_SOURCE_DIR}/AndroidManifest.xml"
      OR (EXISTS "${CMAKE_SOURCE_DIR}/../AndroidManifest.xml" AND EXISTS "${CMAKE_SOURCE_DIR}/../jni/") )
  set( LIBRARY_OUTPUT_PATH_ROOT ${CMAKE_SOURCE_DIR} CACHE PATH "Root for binaries output, set this to change where Android libs are installed to" )
  if( NOT _CMAKE_IN_TRY_COMPILE )
    if( EXISTS "${CMAKE_SOURCE_DIR}/jni/CMakeLists.txt" )
      set( EXECUTABLE_OUTPUT_PATH "${LIBRARY_OUTPUT_PATH_ROOT}/bin/${ANDROID_NDK_ABI_NAME}" CACHE PATH "Output directory for applications" )
    else()
      set( EXECUTABLE_OUTPUT_PATH "${LIBRARY_OUTPUT_PATH_ROOT}/bin" CACHE PATH "Output directory for applications" )
    endif()
    set( LIBRARY_OUTPUT_PATH "${LIBRARY_OUTPUT_PATH_ROOT}/libs/${ANDROID_NDK_ABI_NAME}" CACHE PATH "Output directory for Android libs" )
  endif()
endif()

# copy shaed stl library to build directory
if( NOT _CMAKE_IN_TRY_COMPILE AND __libstl MATCHES "[.]so$" AND DEFINED LIBRARY_OUTPUT_PATH )
  get_filename_component( __libstlname "${__libstl}" NAME )
  execute_process( COMMAND "${CMAKE_COMMAND}" -E copy_if_different "${__libstl}" "${LIBRARY_OUTPUT_PATH}/${__libstlname}" RESULT_VARIABLE __fileCopyProcess )
  if( NOT __fileCopyProcess EQUAL 0 OR NOT EXISTS "${LIBRARY_OUTPUT_PATH}/${__libstlname}")
    message( SEND_ERROR "Failed copying of ${__libstl} to the ${LIBRARY_OUTPUT_PATH}/${__libstlname}" )
  endif()
  unset( __fileCopyProcess )
  unset( __libstlname )
endif()


# set these global flags for cmake client scripts to change behavior
set( ANDROID True )
set( BUILD_ANDROID True )

# where is the target environment
set( CMAKE_FIND_ROOT_PATH "${ANDROID_TOOLCHAIN_ROOT}/bin" "${ANDROID_TOOLCHAIN_ROOT}/${ANDROID_TOOLCHAIN_MACHINE_NAME}" "${ANDROID_SYSROOT}" "${CMAKE_INSTALL_PREFIX}" "${CMAKE_INSTALL_PREFIX}/share" )

# only search for libraries and includes in the ndk toolchain
set( CMAKE_FIND_ROOT_PATH_MODE_PROGRAM ONLY )
set( CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY )
set( CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY )


# macro to find packages on the host OS
macro( find_host_package )
 set( CMAKE_FIND_ROOT_PATH_MODE_PROGRAM NEVER )
 set( CMAKE_FIND_ROOT_PATH_MODE_LIBRARY NEVER )
 set( CMAKE_FIND_ROOT_PATH_MODE_INCLUDE NEVER )
 if( CMAKE_HOST_WIN32 )
  SET( WIN32 1 )
  SET( UNIX )
 elseif( CMAKE_HOST_APPLE )
  SET( APPLE 1 )
  SET( UNIX )
 endif()
 find_package( ${ARGN} )
 SET( WIN32 )
 SET( APPLE )
 SET( UNIX 1 )
 set( CMAKE_FIND_ROOT_PATH_MODE_PROGRAM ONLY )
 set( CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY )
 set( CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY )
endmacro()


# macro to find programs on the host OS
macro( find_host_program )
 set( CMAKE_FIND_ROOT_PATH_MODE_PROGRAM NEVER )
 set( CMAKE_FIND_ROOT_PATH_MODE_LIBRARY NEVER )
 set( CMAKE_FIND_ROOT_PATH_MODE_INCLUDE NEVER )
 if( CMAKE_HOST_WIN32 )
  SET( WIN32 1 )
  SET( UNIX )
 elseif( CMAKE_HOST_APPLE )
  SET( APPLE 1 )
  SET( UNIX )
 endif()
 find_program( ${ARGN} )
 SET( WIN32 )
 SET( APPLE )
 SET( UNIX 1 )
 set( CMAKE_FIND_ROOT_PATH_MODE_PROGRAM ONLY )
 set( CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY )
 set( CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY )
endmacro()


# export toolchain settings for the try_compile() command
if( NOT _CMAKE_IN_TRY_COMPILE )
 set( __toolchain_config "")
 foreach( __var NDK_CCACHE  LIBRARY_OUTPUT_PATH_ROOT  ANDROID_FORBID_SYGWIN
                ANDROID_NDK_HOST_X64
                ANDROID_NDK
                ANDROID_NDK_LAYOUT
                ANDROID_STANDALONE_TOOLCHAIN
                ANDROID_TOOLCHAIN_NAME
                ANDROID_ABI
                ANDROID_NATIVE_API_LEVEL
                ANDROID_STL
                ANDROID_STL_FORCE_FEATURES
                ANDROID_FORCE_ARM_BUILD
                ANDROID_NO_UNDEFINED
                ANDROID_SO_UNDEFINED
                ANDROID_FUNCTION_LEVEL_LINKING
                ANDROID_GOLD_LINKER
                ANDROID_NOEXECSTACK
                ANDROID_RELRO
                ANDROID_LIBM_PATH
                ANDROID_EXPLICIT_CRT_LINK
                ANDROID_APP_PIE
                )
  if( DEFINED ${__var} )
   if( ${__var} MATCHES " ")
    set( __toolchain_config "${__toolchain_config}set( ${__var} \"${${__var}}\" CACHE INTERNAL \"\" )\n" )
   else()
    set( __toolchain_config "${__toolchain_config}set( ${__var} ${${__var}} CACHE INTERNAL \"\" )\n" )
   endif()
  endif()
 endforeach()
 file( WRITE "${CMAKE_BINARY_DIR}${CMAKE_FILES_DIRECTORY}/android.toolchain.config.cmake" "${__toolchain_config}" )
 unset( __toolchain_config )
endif()


# force cmake to produce / instead of \ in build commands for Ninja generator
if( CMAKE_GENERATOR MATCHES "Ninja" AND CMAKE_HOST_WIN32 )
 # it is a bad hack after all
 # CMake generates Ninja makefiles with UNIX paths only if it thinks that we are going to build with MinGW
 set( CMAKE_COMPILER_IS_MINGW TRUE ) # tell CMake that we are MinGW
 set( CMAKE_CROSSCOMPILING TRUE )    # stop recursion
 enable_language( C )
 enable_language( CXX )
 # unset( CMAKE_COMPILER_IS_MINGW ) # can't unset because CMake does not convert back-slashes in response files without it
 unset( MINGW )
endif()


# Variables controlling behavior or set by cmake toolchain:
#   ANDROID_ABI : "armeabi-v7a" (default), "armeabi", "armeabi-v7a with NEON", "armeabi-v7a with VFPV3", "armeabi-v6 with VFP", "x86", "mips", "arm64-v8a", "x86_64", "mips64"
#   ANDROID_NATIVE_API_LEVEL : 3,4,5,8,9,14,15,16,17,18,19,21 (depends on NDK version)
#   ANDROID_STL : gnustl_static/gnustl_shared/stlport_static/stlport_shared/gabi++_static/gabi++_shared/system_re/system/none
#   ANDROID_FORBID_SYGWIN : ON/OFF
#   ANDROID_NO_UNDEFINED : ON/OFF
#   ANDROID_SO_UNDEFINED : OFF/ON  (default depends on NDK version)
#   ANDROID_FUNCTION_LEVEL_LINKING : ON/OFF
#   ANDROID_GOLD_LINKER : ON/OFF
#   ANDROID_NOEXECSTACK : ON/OFF
#   ANDROID_RELRO : ON/OFF
#   ANDROID_FORCE_ARM_BUILD : ON/OFF
#   ANDROID_STL_FORCE_FEATURES : ON/OFF
#   ANDROID_LIBM_PATH : path to libm.so (set to something like $(TOP)/out/target/product/<product_name>/obj/lib/libm.so) to workaround unresolved `sincos`
# Can be set only at the first run:
#   ANDROID_NDK : path to your NDK install
#   NDK_CCACHE : path to your ccache executable
#   ANDROID_TOOLCHAIN_NAME : the NDK name of compiler toolchain
#   ANDROID_NDK_HOST_X64 : try to use x86_64 toolchain (default for x64 host systems)
#   ANDROID_NDK_LAYOUT : the inner NDK structure (RELEASE, LINARO, ANDROID)
#   LIBRARY_OUTPUT_PATH_ROOT : <any valid path>
#   ANDROID_STANDALONE_TOOLCHAIN
#
# Primary read-only variables:
#   ANDROID : always TRUE
#   ARMEABI : TRUE for arm v6 and older devices
#   ARMEABI_V6 : TRUE for arm v6
#   ARMEABI_V7A : TRUE for arm v7a
#   ARM64_V8A : TRUE for arm64-v8a
#   NEON : TRUE if NEON unit is enabled
#   VFPV3 : TRUE if VFP version 3 is enabled
#   X86 : TRUE if configured for x86
#   X86_64 : TRUE if configured for x86_64
#   MIPS : TRUE if configured for mips
#   MIPS64 : TRUE if configured for mips64
#   BUILD_WITH_ANDROID_NDK : TRUE if NDK is used
#   BUILD_WITH_STANDALONE_TOOLCHAIN : TRUE if standalone toolchain is used
#   ANDROID_NDK_HOST_SYSTEM_NAME : "windows", "linux-x86" or "darwin-x86" depending on host platform
#   ANDROID_NDK_ABI_NAME : "armeabi", "armeabi-v7a", "x86", "mips", "arm64-v8a", "x86_64", "mips64" depending on ANDROID_ABI
#   ANDROID_NDK_RELEASE : from r5 to r10d; set only for NDK
#   ANDROID_NDK_RELEASE_NUM : numeric ANDROID_NDK_RELEASE version (1000*major+minor)
#   ANDROID_ARCH_NAME : "arm", "x86", "mips", "arm64", "x86_64", "mips64" depending on ANDROID_ABI
#   ANDROID_SYSROOT : path to the compiler sysroot
#   TOOL_OS_SUFFIX : "" or ".exe" depending on host platform
#   ANDROID_COMPILER_IS_CLANG : TRUE if clang compiler is used
#
# Secondary (less stable) read-only variables:
#   ANDROID_COMPILER_VERSION : GCC version used (not Clang version)
#   ANDROID_CLANG_VERSION : version of clang compiler if clang is used
#   ANDROID_CXX_FLAGS : C/C++ compiler flags required by Android platform
#   ANDROID_SUPPORTED_ABIS : list of currently allowed values for ANDROID_ABI
#   ANDROID_TOOLCHAIN_MACHINE_NAME : "arm-linux-androideabi", "arm-eabi" or "i686-android-linux"
#   ANDROID_TOOLCHAIN_ROOT : path to the top level of toolchain (standalone or placed inside NDK)
#   ANDROID_CLANG_TOOLCHAIN_ROOT : path to clang tools
#   ANDROID_SUPPORTED_NATIVE_API_LEVELS : list of native API levels found inside NDK
#   ANDROID_STL_INCLUDE_DIRS : stl include paths
#   ANDROID_RTTI : if rtti is enabled by the runtime
#   ANDROID_EXCEPTIONS : if exceptions are enabled by the runtime
#   ANDROID_GCC_TOOLCHAIN_NAME : read-only, differs from ANDROID_TOOLCHAIN_NAME only if clang is used
#
# Defaults:
#   ANDROID_DEFAULT_NDK_API_LEVEL
#   ANDROID_DEFAULT_NDK_API_LEVEL_${ARCH}
#   ANDROID_NDK_SEARCH_PATHS
#   ANDROID_SUPPORTED_ABIS_${ARCH}
#   ANDROID_SUPPORTED_NDK_VERSIONS
  group: {
 "description": "This file is to be read by androiddeployqt",
 "qt": "@QT_ANDROID_QT_ROOT@",
 "sdk": "@QT_ANDROID_SDK_ROOT@",
 "ndk": "@QT_ANDROID_NDK_ROOT@",
 "sdkBuildToolsRevision": "@QT_ANDROID_SDK_BUILDTOOLS_REVISION@",
 "toolchain-prefix": "@ANDROID_TOOLCHAIN_MACHINE_NAME@",
 "tool-prefix": "@ANDROID_TOOLCHAIN_MACHINE_NAME@",
 "toolchain-version": "@ANDROID_COMPILER_VERSION@",
 "ndk-host": "@ANDROID_NDK_HOST_SYSTEM_NAME@",
 "target-architecture": "@ANDROID_ABI@",
 "application-binary": "@QT_ANDROID_APP_PATH@",
 "android-package": "@QT_ANDROID_APP_PACKAGE_NAME@",
 "android-app-name": "@QT_ANDROID_APP_NAME@",
 @QT_ANDROID_APP_EXTRA_LIBS@
 "android-package-source-directory": "@QT_ANDROID_APP_PACKAGE_SOURCE_ROOT@"
}
  cancel-in-progress: #include "winver.h"

IDI_ICON1               ICON    DISCARDABLE     "@ICON_FILE@"

VS_VERSION_INFO VERSIONINFO
 FILEVERSION @PROJECT_VERSION_MAJOR@,@PROJECT_VERSION_MINOR@,@PROJECT_VERSION_PATCH@,@PROJECT_VERSION_TWEAK@
 PRODUCTVERSION @PROJECT_VERSION_MAJOR@,@PROJECT_VERSION_MINOR@,@PROJECT_VERSION_PATCH@,@PROJECT_VERSION_TWEAK@
 FILEFLAGS 0x0L
 FILEFLAGSMASK 0x3fL
 FILEOS 0x00040004L
 FILETYPE 0x1L
 FILESUBTYPE 0x0L
BEGIN
    BLOCK "StringFileInfo"
    BEGIN
        BLOCK "000004b0"
        BEGIN
            VALUE "CompanyName", "@COMPANY@"
            VALUE "FileDescription", "@PROJECT_NAME@"
            VALUE "FileVersion", "@PROJECT_VERSION@"
            VALUE "LegalCopyright", "@COPYRIGHT@"
            VALUE "InternalName", "@PROJECT_NAME@"
            VALUE "OriginalFilename", "@PROJECT_NAME@.exe"
            VALUE "ProductName", "@PROJECT_NAME@"
            VALUE "ProductVersion", "@PROJECT_VERSION@"
        END
    END
END


jobs: Copyright (c) 2008, 
All rights reserved.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are
met:

    * Redistributions of source code must retain the above copyright
      notice, this list of conditions and the following disclaimer.

    * Redistributions in binary form must reproduce the above
      copyright notice, this list of conditions and the following
      disclaimer in the documentation and/or other materials provided
      with the distribution.

    * Neither the name of McGill University, nor the Input Devices and
      Music Interaction Lab, nor the names of its contributors may be
      used to endorse or promote products derived from this software
      without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
"AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
  # Build job
  build: 
foaw: foaw.o

run: foaw
        ./foaw | ./plot.py

velocity.so: velocity.c
        gcc -O3 -shared -fPIC -I/usr/include/python2.6 -o $@ $< -lpython2.6

velocity.c: velocity.pyx
        cython $<

cvelocity.so: cvelocity.c
        gcc -Wall -Werror -O3 -shared -fPIC -o cvelocity.so cvelocity.c
    runs-on: 
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>

typedef double TFLOAT;

/******** First-Order Adaptive Windowing *********/

/*
 * Perform the FOAW velocity estimation routine.
 * This algorithm is described here:
 *
 * ., "Discrete-time
 * adaptive windowing for velocity estimation," Control Systems
 * Technology, IEEE Transactions on , vol.8, no.6, pp.1003-1009, Nov
 * 2000
 *
 * http://www.cim.mcgill.ca/~haptic/pub/FS-VH-CSC-TCST-00.pdf 
 *
 * This implementation (C)2008, IDMIL, McGill
 * University.  This work is covered by the GPL-compatible version of
 * the BSD license, please see the following URL for more information:
 *
 * http://www.opensource.org/licenses/bsd-license.html
 *
 * The exact license is listed in the file COPYING, which you should
 * have received with this source code.
 */

static inline TFLOAT do_foaw_sample(TFLOAT *posbuf, int size, int *k,
                                    TFLOAT current_pos, int best,
                                    TFLOAT noise_max, TFLOAT T)
{
    int i, j, l, bad;
    TFLOAT b, ykj;
    TFLOAT velocity = 0;

    /* circular buffer */
    *k = (*k+1)%size;
    posbuf[*k] = current_pos;

    for (i=1; i<size; i++)
    {
        if (best)
        {
            // best-fit-FOAW
            b = 0;
            for (l=0; l<(i+1); l++)
                b +=  i*posbuf[(*k-l+size)%size]
                    - 2*posbuf[(*k-l+size)%size]*l;
            b = b / (T*i*(i+1)*(i+2)/6);
        }
        else
            // end-fit-FOAW
            b = (posbuf[*k]-posbuf[(*k-i+size)%size]) / (i*T);
        bad = 0;
        for (j=1; j<i; j++)
        {
            ykj = posbuf[*k]-(b*j*T);
            if (   (ykj < (posbuf[(*k-j+size)%size]-noise_max))
                || (ykj > (posbuf[(*k-j+size)%size]+noise_max)))
            {
                bad = 1;
                break;
            }
        }
        if (bad) break;
        velocity = b;
    }

    return velocity;
}

void foaw_best_fit(TFLOAT SR, int N, TFLOAT noise,
                   TFLOAT *input, TFLOAT *output,
                   int size)
{
    TFLOAT T = 1.0/SR;
    TFLOAT posbuf[N];
    int k=0;
    memset(posbuf, input[0], N*sizeof(TFLOAT));
    while (size--) {
        *(output++) = do_foaw_sample(posbuf, N, &k, *(input++), 1,
                                     noise, T);
    }
}

void foaw_end_fit(TFLOAT SR, int N, TFLOAT noise,
                  TFLOAT *input, TFLOAT *output,
                  int size)
{
    TFLOAT T = 1.0/SR;
    TFLOAT posbuf[N];
    int k=0;
    int i=0;
    for (i=0; i<N; i++)
        posbuf[i] = input[0];
    while (size--)
        *(output++) = do_foaw_sample(posbuf, N, &k, *(input++), 0, noise, T);
}

/******** Levant's differentiator *********/

/*
 * Levant's differentiator, from Levant A. (1998). "Robust exact
 * differentiation via sliding mode technique." Automatica, 34(3),
 * 379-384.  Suggested for use with force-feedback devices in Chawda et
 * al., "Application of Levant's Differentiator for Velocity Estimation
 * and Increased Z-Width in Haptic Interfaces", WHC 2011.

 * Note that it's not very well-suited to the test data in this file
 * because it is sensitive to an estimate of maximum acceleration,
 * which in the case of this highly discontinuous velocity is very
 * large.  On sinusoidal test data it fairs much better, and gets
 * better as sampling rate increases (as opposed to the other
 * techniques here).

 * Moreover, the papers suggest that Lambda and alpha variables can be
 * better-tuned.

 * Lipschitz's constant 'C' = maximum absolute acceleration, must be
 * provided.
*/

/* Table lookup for a 2nd-order fit to sqrt between 0 and 1 */
static TFLOAT sqrttbl[100][3] = {
    { -5.97546696e+02,   1.40461896e+01,   1.62673996e-02},
    { -7.02157732e+01,   6.22495658e+00,   4.49043263e-02},
    { -3.19746276e+01,   4.77081197e+00,   5.88286283e-02},
    { -1.91974969e+01,   4.02063743e+00,   6.98777678e-02},
    { -1.31389771e+01,   3.54176558e+00,   7.93591255e-02},
    { -9.71290158e+00,   3.20177791e+00,   8.78045862e-02},
    { -7.55515321e+00,   2.94422075e+00,   9.54971804e-02},
    { -6.09320979e+00,   2.74034500e+00,   1.02609728e-01},
    { -5.04885132e+00,   2.57374507e+00,   1.09257226e-01},
    { -4.27221921e+00,   2.43427995e+00,   1.15520891e-01},
    { -3.67616738e+00,   2.31529653e+00,   1.21460651e-01},
    { -3.20691652e+00,   2.21222372e+00,   1.27122246e-01},
    { -2.82966427e+00,   2.12180283e+00,   1.32541544e-01},
    { -2.52099609e+00,   2.04163945e+00,   1.37747291e-01},
    { -2.26463978e+00,   1.96992936e+00,   1.42762952e-01},
    { -2.04897170e+00,   1.90528363e+00,   1.47607977e-01},
    { -1.86549063e+00,   1.84661332e+00,   1.52298706e-01},
    { -1.70785035e+00,   1.79305090e+00,   1.56849024e-01},
    { -1.57122761e+00,   1.74389558e+00,   1.61270847e-01},
    { -1.45189802e+00,   1.69857423e+00,   1.65574492e-01},
    { -1.34694484e+00,   1.65661291e+00,   1.69768967e-01},
    { -1.25405475e+00,   1.61761590e+00,   1.73862188e-01},
    { -1.17137198e+00,   1.58124978e+00,   1.77861157e-01},
    { -1.09739237e+00,   1.54723138e+00,   1.81772104e-01},
    { -1.03088520e+00,   1.51531848e+00,   1.85600603e-01},
    { -9.70834857e-01,   1.48530245e+00,   1.89351660e-01},
    { -9.16396543e-01,   1.45700249e+00,   1.93029796e-01},
    { -8.66862460e-01,   1.43026106e+00,   1.96639107e-01},
    { -8.21635589e-01,   1.40494015e+00,   2.00183320e-01},
    { -7.80209213e-01,   1.38091829e+00,   2.03665837e-01},
    { -7.42150767e-01,   1.35808804e+00,   2.07089773e-01},
    { -7.07088992e-01,   1.33635404e+00,   2.10457989e-01},
    { -6.74703641e-01,   1.31563127e+00,   2.13773122e-01},
    { -6.44717165e-01,   1.29584365e+00,   2.17037606e-01},
    { -6.16887956e-01,   1.27692290e+00,   2.20253695e-01},
    { -5.91004816e-01,   1.25880751e+00,   2.23423479e-01},
    { -5.66882409e-01,   1.24144193e+00,   2.26548904e-01},
    { -5.44357491e-01,   1.22477581e+00,   2.29631781e-01},
    { -5.23285776e-01,   1.20876341e+00,   2.32673804e-01},
    { -5.03539310e-01,   1.19336309e+00,   2.35676554e-01},
    { -4.85004269e-01,   1.17853682e+00,   2.38641516e-01},
    { -4.67579100e-01,   1.16424980e+00,   2.41570081e-01},
    { -4.51172935e-01,   1.15047011e+00,   2.44463557e-01},
    { -4.35704256e-01,   1.13716841e+00,   2.47323178e-01},
    { -4.21099742e-01,   1.12431770e+00,   2.50150104e-01},
    { -4.07293281e-01,   1.11189305e+00,   2.52945432e-01},
    { -3.94225127e-01,   1.09987143e+00,   2.55710198e-01},
    { -3.81841163e-01,   1.08823151e+00,   2.58445385e-01},
    { -3.70092270e-01,   1.07695350e+00,   2.61151921e-01},
    { -3.58933769e-01,   1.06601903e+00,   2.63830689e-01},
    { -3.48324944e-01,   1.05541102e+00,   2.66482526e-01},
    { -3.38228619e-01,   1.04511352e+00,   2.69108229e-01},
    { -3.28610790e-01,   1.03511168e+00,   2.71708556e-01},
    { -3.19440298e-01,   1.02539161e+00,   2.74284228e-01},
    { -3.10688548e-01,   1.01594034e+00,   2.76835934e-01},
    { -3.02329251e-01,   1.00674569e+00,   2.79364330e-01},
    { -2.94338202e-01,   9.97796259e-01,   2.81870045e-01},
    { -2.86693084e-01,   9.89081335e-01,   2.84353678e-01},
    { -2.79373289e-01,   9.80590852e-01,   2.86815802e-01},
    { -2.72359760e-01,   9.72315340e-01,   2.89256968e-01},
    { -2.65634852e-01,   9.64245876e-01,   2.91677701e-01},
    { -2.59182206e-01,   9.56374050e-01,   2.94078506e-01},
    { -2.52986636e-01,   9.48691924e-01,   2.96459867e-01},
    { -2.47034029e-01,   9.41191998e-01,   2.98822250e-01},
    { -2.41311251e-01,   9.33867182e-01,   3.01166101e-01},
    { -2.35806068e-01,   9.26710766e-01,   3.03491849e-01},
    { -2.30507071e-01,   9.19716396e-01,   3.05799908e-01},
    { -2.25403608e-01,   9.12878045e-01,   3.08090675e-01},
    { -2.20485725e-01,   9.06189999e-01,   3.10364533e-01},
    { -2.15744109e-01,   8.99646831e-01,   3.12621851e-01},
    { -2.11170041e-01,   8.93243384e-01,   3.14862985e-01},
    { -2.06755348e-01,   8.86974756e-01,   3.17088279e-01},
    { -2.02492362e-01,   8.80836281e-01,   3.19298062e-01},
    { -1.98373883e-01,   8.74823517e-01,   3.21492656e-01},
    { -1.94393146e-01,   8.68932230e-01,   3.23672370e-01},
    { -1.90543785e-01,   8.63158384e-01,   3.25837501e-01},
    { -1.86819811e-01,   8.57498130e-01,   3.27988339e-01},
    { -1.83215578e-01,   8.51947789e-01,   3.30125163e-01},
    { -1.79725765e-01,   8.46503851e-01,   3.32248244e-01},
    { -1.76345350e-01,   8.41162958e-01,   3.34357843e-01},
    { -1.73069592e-01,   8.35921900e-01,   3.36454215e-01},
    { -1.69894009e-01,   8.30777606e-01,   3.38537604e-01},
    { -1.66814366e-01,   8.25727133e-01,   3.40608249e-01},
    { -1.63826651e-01,   8.20767664e-01,   3.42666382e-01},
    { -1.60927070e-01,   8.15896498e-01,   3.44712226e-01},
    { -1.58112024e-01,   8.11111046e-01,   3.46745998e-01},
    { -1.55378103e-01,   8.06408824e-01,   3.48767911e-01},
    { -1.52722072e-01,   8.01787445e-01,   3.50778169e-01},
    { -1.50140858e-01,   7.97244620e-01,   3.52776971e-01},
    { -1.47631544e-01,   7.92778149e-01,   3.54764511e-01},
    { -1.45191357e-01,   7.88385916e-01,   3.56740977e-01},
    { -1.42817660e-01,   7.84065886e-01,   3.58706553e-01},
    { -1.40507944e-01,   7.79816104e-01,   3.60661417e-01},
    { -1.38259821e-01,   7.75634686e-01,   3.62605741e-01},
    { -1.36071014e-01,   7.71519818e-01,   3.64539694e-01},
    { -1.33939356e-01,   7.67469754e-01,   3.66463441e-01},
    { -1.31862780e-01,   7.63482810e-01,   3.68377141e-01},
    { -1.29839313e-01,   7.59557363e-01,   3.70280951e-01},
    { -1.27867073e-01,   7.55691849e-01,   3.72175022e-01},
    { -1.25944263e-01,   7.51884758e-01,   3.74059502e-01},
};

#define sign(x) ((x)==0.0?0.0:((x)<0.0?-1.0:1.0))

static inline TFLOAT tbl_sqrt(TFLOAT x)
{
    if (x>=0.0 && x<1.0) {
        int i = (int)(x*100);
        return x*x*sqrttbl[i][0]+x*sqrttbl[i][1]+sqrttbl[i][2];
    }
    else
        return sqrt(x);
}

static inline void f(TFLOAT alpha, TFLOAT Lambda,
                     TFLOAT p, TFLOAT u1, TFLOAT x,
                     TFLOAT *du1, TFLOAT *dx)
{
    TFLOAT e = x-p;
    *du1 = -alpha * sign(e);
    *dx = u1-Lambda * tbl_sqrt(fabs(e)) * sign(e);
}

void levant(TFLOAT sr, TFLOAT C, int rk,
            TFLOAT *pos, TFLOAT *result, int size)
{
    TFLOAT T = 1.0/sr;

    // Coefficients derived from C;
    TFLOAT alpha = 1.1 * C;
    TFLOAT Lambda = sqrt(C);

    TFLOAT x = 0, u1 = 0, u;
    TFLOAT k1du1, k1dx, k2du1, k2dx, k3du1, k3dx, k4du1, k4dx, tu1, tx;

    int i;

    if (rk==4) {
        for (i=0; i<size; i++) {
            f(alpha,Lambda,pos[i], u1, x, &k1du1, &k1dx);
            f(alpha,Lambda,pos[i], u1+(T/2)*k1du1, x+(T/2)*k1dx, &k2du1, &k2dx);
            f(alpha,Lambda,pos[i], u1+(T/2)*k2du1, x+(T/2)*k2dx, &k3du1, &k3dx);
            f(alpha,Lambda,pos[i], u1+T*k3du1, x+T*k3dx, &k4du1, &k4dx);
            u1 = u1 + (T/6)*(k1du1 + 2*k2du1 + 2*k3du1 + k4du1);
            u = (1.0/6)*(k1dx + 2*k2dx + 2*k3dx + k4dx);
            x = x + u*T;
            result[i] = u;
        }
    }
    else if (rk==2) {
        for (i=0; i<size; i++) {
            f(alpha,Lambda,pos[i],u1,x,&k1du1,&k1dx);
            tu1 = u1 + k1du1*(T/2);
            tx = x + k1dx*(T/2);
            f(alpha,Lambda,pos[i],tu1,tx, &k2du1, &k2dx);
            u1 = u1 + k2du1*T;
            x = x + k2dx*T;
            result[i] = k2dx;
        }
    }
    else if (rk==1) {
        for (i=0; i<size; i++) {
            f(alpha,Lambda,pos[i],u1,x,&k1du1,&k1dx);
            u1 = u1 + k1du1*T;
            x = x + k1dx*T;
            result[i] = k1dx;
        }
    }
    else {
        printf("[levant] Unknown rk==%d\n", rk);
    }
}

/******** Median filter *********/

static int tfloatcomp(const void *a, const void *b)
{
    if (*(const TFLOAT*)a < *(const TFLOAT*)b)
        return -1;
    else
        return *(const TFLOAT*)a > *(const TFLOAT*)b;
}

void median_filter(unsigned int n, TFLOAT *pos, TFLOAT *result, int size)
{
    int i;
    TFLOAT buf[n], med[n];
    memset(buf, 0, sizeof(buf));
    for (i=0; i<size; i++) {
        buf[i%n] = pos[i];
        memcpy(med, buf, sizeof(buf));
        qsort(med, n, sizeof(TFLOAT), tfloatcomp);
        result[i] = med[n/2];
    }
}
    steps: 
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>

/*
 * Perform the FOAW velocity estimation routine.
 * This algorithm is described here:
 *
 * ., "Discrete-time
 * adaptive windowing for velocity estimation," Control Systems
 * Technology, IEEE Transactions on , vol.8, no.6, pp.1003-1009, Nov
 * 2000
 *
 * http://www.cim.mcgill.ca/~haptic/pub/FS-VH-CSC-TCST-00.pdf 
 *
 * This implementation (C)2008 , IDMIL, McGill
 * University.  This work is covered by the GPL-compatible version of
 * the BSD license, please see the following URL for more information:
 *
 * http://www.opensource.org/licenses/bsd-license.html
 *
 * The exact license is listed in the file COPYING, which you should
 * have received with this source code.
 */

float SR=1000;
#define T (1.0f/SR)
#define SIZE 1000
float NOISE=0;

float vel[SIZE];
float pos[SIZE];
float mpos[SIZE];
float fdvel[SIZE];
float foawvel[SIZE];
float dif[SIZE];

#ifndef min
#define min(X,Y) ((X) < (Y) ? (X) : (Y))
#endif

void generate_velocity()
{
    int k;
    for (k=0; k<50; k++)
        vel[k] = 0;
    for (; k<150; k++)
        vel[k] = 1;
    for (; k<450; k++)
        vel[k] = 2;
    for (; k<600; k++)
        vel[k] = 0;
    for (; k<700; k++)
        vel[k] = -1;
    for (; k<1000; k++)
        vel[k] = -2;
}

void integrate_position()
{
    int k;
    pos[0] = 0;
    for (k=1; k<SIZE; k++)
        pos[k] = (vel[k]*T+pos[k-1]);
}

void add_position_noise()
{
    int k;
    for (k=0; k<SIZE; k++)
        pos[k] += rand()/(float)RAND_MAX * NOISE;
}

void finite_difference()
{
    int k;
    fdvel[0] = 0;
    for (k=1; k<SIZE; k++)
        fdvel[k] = (pos[k]-pos[k-1])/T;
}

float do_foaw_sample(float *posbuf, int size, int *k,
                     float current_pos, int best)
{
    int i, j, l, bad;
    float b, ykj;
    float velocity = 0;
    float noise_max = NOISE;

    /* circular buffer */
    *k = (*k+1)%size;
    posbuf[*k] = current_pos;

    for (i=1; i<size; i++)
    {
        if (best)
        {
            // best-fit-FOAW
            b = 0;
            for (l=0; l<(i+1); l++)
                b +=  i*posbuf[(*k-l+size)%size]
                    - 2*posbuf[(*k-l+size)%size]*l;
            b = b / (T*i*(i+1)*(i+2)/6);
        }
        else
            // end-fit-FOAW
            b = (posbuf[*k]-posbuf[(*k-i+size)%size]) / (i*T);
        bad = 0;
        for (j=1; j<i; j++)
        {
            ykj = posbuf[*k]-(b*j*T);
            if (   (ykj < (posbuf[(*k-j+size)%size]-noise_max))
                || (ykj > (posbuf[(*k-j+size)%size]+noise_max)))
            {
                bad = 1;
                break;
            }
        }
        if (bad) break;
        velocity = b;
    }

    return velocity;
}

void subtract(float *result, float *from, float *what)
{
    int k;
    for (k=0; k<SIZE; k++)
        result[k] = from[k]-what[k];
}

void median_position(int n)
{
    int k, i, j, off=600;
    float *buf;
    buf = malloc(sizeof(float)*2*n);
    for (k=0; k<SIZE; k++)
    {
        int size = min(n,k);
        int a=n, b=n;
        buf[n] = pos[k];
        for (i=1; i<size; i++)
        {
            if (pos[k-i] > buf[b]) {
                buf[b+1] = pos[k-i];
                b++;
            }
            else if (pos[k-i] < buf[a]) {
                buf[a-1] = pos[k-i];
                a--;
            }
            else if (pos[k-i] > buf[b-1]) {
                buf[b+1] = buf[b];
                buf[b] = pos[k-i];
                b++;
            }
            else if (pos[k-i] < buf[a+1]) {
                buf[a-1] = buf[a];
                buf[a] = pos[k-i];
                a--;
            }
        }
        mpos[k] = buf[(b-a)/2+a];
    }
    free(buf);
}


int main(int argc, char *argv[])
{
    int n = 15;
    int best = 0;
    float *posbuf = 0;
    int k, i=0;
    float inp;

    if (argc > 1) {
        SR = (float)atoi(argv[1]);
        if (SR <= 0) {
            fprintf(stderr, "Bad sample rate.\n");
            return 1;
        }
    }
    else {
        printf("deriv <rate> <noise> <size> <bestfit=1,endfit=0>\n");
        exit(0);
    }

    if (argc > 2) {
        NOISE = atof(argv[2]);
        if (NOISE <= 0) {
            fprintf(stderr, "Bad noise margin.\n");
            return 1;
        }
    }
    else
        NOISE = (0.1*T);

    if (argc > 3) {
        n = atoi(argv[3]);
        if (n <= 0) {
            fprintf(stderr, "Bad N.\n");
            return 1;
        }
    }

    if (argc > 4) {
        best = atoi(argv[4]);
        if (best < 0 || best > 1) {
            fprintf(stderr, "Bad 'best'.\n");
            return 1;
        }
    }

    posbuf = malloc(sizeof(float)*n);
    memset(posbuf, 0, sizeof(float)*n);

    scanf("%g", &inp);
    do_foaw_sample(posbuf, n, &i, inp, best);
    while (scanf("%g", &inp)!=EOF) {
        printf("%g\n", do_foaw_sample(posbuf, n, &i, inp, best));
    }

    free(posbuf);
    return 0;
}
      - name: 
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>

/*
 * Perform the FOAW velocity estimation routine.
 * This algorithm is described here:
 *
 * ., "Discrete-time
 * adaptive windowing for velocity estimation," Control Systems
 * Technology, IEEE Transactions on , vol.8, no.6, pp.1003-1009, Nov
 * 2000
 *
 * http://www.cim.mcgill.ca/~haptic/pub/FS-VH-CSC-TCST-00.pdf 
 *
 * This implementation (C)2008 , IDMIL, McGill
 * University.  This work is covered by the GPL-compatible version of
 * the BSD license, please see the following URL for more information:
 *
 * http://www.opensource.org/licenses/bsd-license.html
 *
 * The exact license is listed in the file COPYING, which you should
 * have received with this source code.
 */

#define SR 1000
#define T (1.0f/SR)
#define SIZE 1000
#define NOISE (0.1*T)

float vel[SIZE];
float pos[SIZE];
float mpos[SIZE];
float fdvel[SIZE];
float foawvel[SIZE];
float dif[SIZE];

#ifndef min
#define min(X,Y) ((X) < (Y) ? (X) : (Y))
#endif

void generate_velocity()
{
    int k;
    for (k=0; k<50; k++)
        vel[k] = 0;
    for (; k<150; k++)
        vel[k] = 1;
    for (; k<450; k++)
        vel[k] = 2;
    for (; k<600; k++)
        vel[k] = 0;
    for (; k<700; k++)
        vel[k] = -1;
    for (; k<1000; k++)
        vel[k] = -2;
}

void integrate_position()
{
    int k;
    pos[0] = 0;
    for (k=1; k<SIZE; k++)
        pos[k] = (vel[k]*T+pos[k-1]);
}

void add_position_noise()
{
    int k;
    for (k=0; k<SIZE; k++)
        pos[k] += rand()/(float)RAND_MAX * NOISE;
}

void finite_difference()
{
    int k;
    fdvel[0] = 0;
    for (k=1; k<SIZE; k++)
        fdvel[k] = (pos[k]-pos[k-1])/T;
}

float do_foaw_sample(float *posbuf, int size, int *k,
                     float current_pos, int best)
{
    int i, j, l, bad;
    float b, ykj;
    float velocity = 0;
    float noise_max = NOISE;

    /* circular buffer */
    *k = (*k+1)%size;
    posbuf[*k] = current_pos;

    for (i=1; i<size; i++)
    {
        if (best)
        {
            // best-fit-FOAW
            b = 0;
            for (l=0; l<(i+1); l++)
                b +=  i*posbuf[(*k-l+size)%size]
                    - 2*posbuf[(*k-l+size)%size]*l;
            b = b / (T*i*(i+1)*(i+2)/6);
        }
        else
            // end-fit-FOAW
            b = (posbuf[*k]-posbuf[(*k-i+size)%size]) / (i*T);
        bad = 0;
        for (j=1; j<i; j++)
        {
            ykj = posbuf[*k]-(b*j*T);
            if (   (ykj < (posbuf[(*k-j+size)%size]-noise_max))
                || (ykj > (posbuf[(*k-j+size)%size]+noise_max)))
            {
                bad = 1;
                break;
            }
        }
        if (bad) break;
        velocity = b;
    }

    return velocity;
}

void do_foaw(float *pos, int n, int best)
{
    float *posbuf = malloc(sizeof(float)*n);
    int k, i=0;
    memset(posbuf, 0, sizeof(float)*n);
    for (k=0; k<SIZE; k++)
        foawvel[k] = do_foaw_sample(posbuf, n, &i, pos[k], best);
    free(posbuf);
}

void subtract(float *result, float *from, float *what)
{
    int k;
    for (k=0; k<SIZE; k++)
        result[k] = from[k]-what[k];
}

void median_position(int n)
{
    int k, i, j, off=600;
    float *buf;
    buf = malloc(sizeof(float)*2*n);
    for (k=0; k<SIZE; k++)
    {
        int size = min(n,k);
        int a=n, b=n;
        buf[n] = pos[k];
        for (i=1; i<size; i++)
        {
            if (pos[k-i] > buf[b]) {
                buf[b+1] = pos[k-i];
                b++;
            }
            else if (pos[k-i] < buf[a]) {
                buf[a-1] = pos[k-i];
                a--;
            }
            else if (pos[k-i] > buf[b-1]) {
                buf[b+1] = buf[b];
                buf[b] = pos[k-i];
                b++;
            }
            else if (pos[k-i] < buf[a+1]) {
                buf[a-1] = buf[a];
                buf[a] = pos[k-i];
                a--;
            }
        }
        mpos[k] = buf[(b-a)/2+a];
    }
    free(buf);
}

void dump(float *data)
{
    int k;
    for (k=0; k<SIZE; k++)
        printf("%f\n", data[k]);
}

int main()
{
    generate_velocity();
    integrate_position();
    add_position_noise();
    finite_difference();
    median_position(3);
    do_foaw(mpos, 15, 1);
#if 1
    dump(foawvel);
#else
    subtract(dif, foawvel, vel);
    dump(dif);
#endif
    return 0;
}
        uses: 
// From GLE
// http://developer.apple.com/library/mac/#samplecode/glut/Listings/gle_vvector_h.html

/*
 * vvector.h
 *
 * FUNCTION:
 * This file contains a number of utilities useful for handling
 * 3D vectors
 * 
 * HISTORY:
 * Written by , August 1991
 * Added 2D code, March 1993
 * Added Outer products, C++ proofed, Linas Vepstas October 1993
 */

#ifndef __GUTIL_VECTOR_H__
#define __GUTIL_VECTOR_H__

#if defined(__cplusplus) || defined(c_plusplus)
extern "C" {
#endif


#include <math.h>
//#include "port.h"

/* ========================================================== */
/* Zero out a 2D vector */

#define VEC_ZERO_2(a)               \
{                       \
   (a)[0] = (a)[1] = 0.0;           \
}

/* ========================================================== */
/* Zero out a 3D vector */

#define VEC_ZERO(a)             \
{                       \
   (a)[0] = (a)[1] = (a)[2] = 0.0;      \
}

/* ========================================================== */
/* Zero out a 4D vector */

#define VEC_ZERO_4(a)               \
{                       \
   (a)[0] = (a)[1] = (a)[2] = (a)[3] = 0.0; \
}

/* ========================================================== */
/* Vector copy */

#define VEC_COPY_2(b,a)             \
{                       \
   (b)[0] = (a)[0];             \
   (b)[1] = (a)[1];             \
}

/* ========================================================== */
/* Copy 3D vector */

#define VEC_COPY(b,a)               \
{                       \
   (b)[0] = (a)[0];             \
   (b)[1] = (a)[1];             \
   (b)[2] = (a)[2];             \
}

/* ========================================================== */
/* Copy 4D vector */

#define VEC_COPY_4(b,a)             \
{                       \
   (b)[0] = (a)[0];             \
   (b)[1] = (a)[1];             \
   (b)[2] = (a)[2];             \
   (b)[3] = (a)[3];             \
}

/* ========================================================== */
/* Vector difference */

#define VEC_DIFF_2(v21,v2,v1)           \
{                       \
   (v21)[0] = (v2)[0] - (v1)[0];        \
   (v21)[1] = (v2)[1] - (v1)[1];        \
}

/* ========================================================== */
/* Vector difference */

#define VEC_DIFF(v21,v2,v1)         \
{                       \
   (v21)[0] = (v2)[0] - (v1)[0];        \
   (v21)[1] = (v2)[1] - (v1)[1];        \
   (v21)[2] = (v2)[2] - (v1)[2];        \
}

/* ========================================================== */
/* Vector difference */

#define VEC_DIFF_4(v21,v2,v1)           \
{                       \
   (v21)[0] = (v2)[0] - (v1)[0];        \
   (v21)[1] = (v2)[1] - (v1)[1];        \
   (v21)[2] = (v2)[2] - (v1)[2];        \
   (v21)[3] = (v2)[3] - (v1)[3];        \
}

/* ========================================================== */
/* Vector sum */

#define VEC_SUM_2(v21,v2,v1)            \
{                       \
   (v21)[0] = (v2)[0] + (v1)[0];        \
   (v21)[1] = (v2)[1] + (v1)[1];        \
}

/* ========================================================== */
/* Vector sum */

#define VEC_SUM(v21,v2,v1)          \
{                       \
   (v21)[0] = (v2)[0] + (v1)[0];        \
   (v21)[1] = (v2)[1] + (v1)[1];        \
   (v21)[2] = (v2)[2] + (v1)[2];        \
}

/* ========================================================== */
/* Vector sum */

#define VEC_SUM_4(v21,v2,v1)            \
{                       \
   (v21)[0] = (v2)[0] + (v1)[0];        \
   (v21)[1] = (v2)[1] + (v1)[1];        \
   (v21)[2] = (v2)[2] + (v1)[2];        \
   (v21)[3] = (v2)[3] + (v1)[3];        \
}

/* ========================================================== */
/* scalar times vector */

#define VEC_SCALE_2(c,a,b)          \
{                       \
   (c)[0] = (a)*(b)[0];             \
   (c)[1] = (a)*(b)[1];             \
}

/* ========================================================== */
/* scalar times vector */

#define VEC_SCALE(c,a,b)            \
{                       \
   (c)[0] = (a)*(b)[0];             \
   (c)[1] = (a)*(b)[1];             \
   (c)[2] = (a)*(b)[2];             \
}

/* ========================================================== */
/* scalar times vector */

#define VEC_SCALE_4(c,a,b)          \
{                       \
   (c)[0] = (a)*(b)[0];             \
   (c)[1] = (a)*(b)[1];             \
   (c)[2] = (a)*(b)[2];             \
   (c)[3] = (a)*(b)[3];             \
}

/* ========================================================== */
/* accumulate scaled vector */

#define VEC_ACCUM_2(c,a,b)          \
{                       \
   (c)[0] += (a)*(b)[0];            \
   (c)[1] += (a)*(b)[1];            \
}

/* ========================================================== */
/* accumulate scaled vector */

#define VEC_ACCUM(c,a,b)            \
{                       \
   (c)[0] += (a)*(b)[0];            \
   (c)[1] += (a)*(b)[1];            \
   (c)[2] += (a)*(b)[2];            \
}

/* ========================================================== */
/* accumulate scaled vector */

#define VEC_ACCUM_4(c,a,b)          \
{                       \
   (c)[0] += (a)*(b)[0];            \
   (c)[1] += (a)*(b)[1];            \
   (c)[2] += (a)*(b)[2];            \
   (c)[3] += (a)*(b)[3];            \
}

/* ========================================================== */
/* Vector dot product */

#define VEC_DOT_PRODUCT_2(c,a,b)            \
{                           \
   c = (a)[0]*(b)[0] + (a)[1]*(b)[1];           \
}

/* ========================================================== */
/* Vector dot product */

#define VEC_DOT_PRODUCT(c,a,b)              \
{                           \
   c = (a)[0]*(b)[0] + (a)[1]*(b)[1] + (a)[2]*(b)[2];   \
}

/* ========================================================== */
/* Vector dot product */

#define VEC_DOT_PRODUCT_4(c,a,b)            \
{                           \
   c = (a)[0]*(b)[0] + (a)[1]*(b)[1] + (a)[2]*(b)[2] + (a)[3]*(b)[3] ;  \
}

/* ========================================================== */
/* vector impact parameter (squared) */

#define VEC_IMPACT_SQ(bsq,direction,position)       \
{                           \
   gleDouble vlen, llel;                    \
   VEC_DOT_PRODUCT (vlen, position, position);      \
   VEC_DOT_PRODUCT (llel, direction, position);     \
   bsq = vlen - llel*llel;              \
}

/* ========================================================== */
/* vector impact parameter */

#define VEC_IMPACT(bsq,direction,position)      \
{                           \
   VEC_IMPACT_SQ(bsq,direction,position);       \
   bsq = sqrt (bsq);                    \
}

/* ========================================================== */
/* Vector length */

#define VEC_LENGTH_2(vlen,a)            \
{                       \
   vlen = a[0]*a[0] + a[1]*a[1];            \
   vlen = sqrt (vlen);              \
}

/* ========================================================== */
/* Vector length */

#define VEC_LENGTH(vlen,a)          \
{                       \
   vlen = (a)[0]*(a)[0] + (a)[1]*(a)[1];        \
   vlen += (a)[2]*(a)[2];           \
   vlen = sqrt (vlen);              \
}

/* ========================================================== */
/* Vector length */

#define VEC_LENGTH_4(vlen,a)            \
{                       \
   vlen = (a)[0]*(a)[0] + (a)[1]*(a)[1];        \
   vlen += (a)[2]*(a)[2];           \
   vlen += (a)[3] * (a)[3];         \
   vlen = sqrt (vlen);              \
}

/* ========================================================== */
/* distance between two points */

#define VEC_DISTANCE(vlen,va,vb)            \
{                       \
    gleDouble tmp[4];               \
    VEC_DIFF (tmp, vb, va);         \
    VEC_LENGTH (vlen, tmp);         \
}

/* ========================================================== */
/* Vector length */

#define VEC_CONJUGATE_LENGTH(vlen,a)        \
{                       \
   vlen = 1.0 - a[0]*a[0] - a[1]*a[1] - a[2]*a[2];\
   vlen = sqrt (vlen);              \
}

/* ========================================================== */
/* Vector length */

#define VEC_NORMALIZE(a)            \
{                       \
   double vlen;                 \
   VEC_LENGTH (vlen,a);             \
   if (vlen != 0.0) {               \
      vlen = 1.0 / vlen;                \
      a[0] *= vlen;             \
      a[1] *= vlen;             \
      a[2] *= vlen;             \
   }                        \
}

/* ========================================================== */
/* Vector length */

#define VEC_RENORMALIZE(a,newlen)       \
{                       \
   double vlen;                 \
   VEC_LENGTH (vlen,a);             \
   if (vlen != 0.0) {               \
      vlen = newlen / vlen;             \
      a[0] *= vlen;             \
      a[1] *= vlen;             \
      a[2] *= vlen;             \
   }                        \
}

/* ========================================================== */
/* 3D Vector cross product yeilding vector */

#define VEC_CROSS_PRODUCT(c,a,b)        \
{                       \
   c[0] = (a)[1] * (b)[2] - (a)[2] * (b)[1];    \
   c[1] = (a)[2] * (b)[0] - (a)[0] * (b)[2];    \
   c[2] = (a)[0] * (b)[1] - (a)[1] * (b)[0];    \
}

/* ========================================================== */
/* Vector perp -- assumes that n is of unit length 
 * accepts vector v, subtracts out any component parallel to n */

#define VEC_PERP(vp,v,n)            \
{                       \
   double vdot;                 \
                        \
   VEC_DOT_PRODUCT (vdot, v, n);            \
   vp[0] = (v)[0] - (vdot) * (n)[0];        \
   vp[1] = (v)[1] - (vdot) * (n)[1];        \
   vp[2] = (v)[2] - (vdot) * (n)[2];        \
}

/* ========================================================== */
/* Vector parallel -- assumes that n is of unit length 
 * accepts vector v, subtracts out any component perpendicular to n */

#define VEC_PARALLEL(vp,v,n)            \
{                       \
   double vdot;                 \
                        \
   VEC_DOT_PRODUCT (vdot, v, n);            \
   vp[0] = (vdot) * (n)[0];         \
   vp[1] = (vdot) * (n)[1];         \
   vp[2] = (vdot) * (n)[2];         \
}

/* ========================================================== */
/* Vector reflection -- assumes n is of unit length */
/* Takes vector v, reflects it against reflector n, and returns vr */

#define VEC_REFLECT(vr,v,n)         \
{                       \
   double vdot;                 \
                        \
   VEC_DOT_PRODUCT (vdot, v, n);            \
   vr[0] = (v)[0] - 2.0 * (vdot) * (n)[0];  \
   vr[1] = (v)[1] - 2.0 * (vdot) * (n)[1];  \
   vr[2] = (v)[2] - 2.0 * (vdot) * (n)[2];  \
}

/* ========================================================== */
/* Vector blending */
/* Takes two vectors a, b, blends them together */ 

#define VEC_BLEND(vr,sa,a,sb,b)         \
{                       \
                        \
   vr[0] = (sa) * (a)[0] + (sb) * (b)[0];   \
   vr[1] = (sa) * (a)[1] + (sb) * (b)[1];   \
   vr[2] = (sa) * (a)[2] + (sb) * (b)[2];   \
}

/* ========================================================== */
/* Vector print */

#define VEC_PRINT_2(a)                  \
{                           \
   double vlen;                     \
   VEC_LENGTH_2 (vlen, a);                  \
   printf (" a is %f %f length of a is %f \n", a[0], a[1], vlen); \
}

/* ========================================================== */
/* Vector print */

#define VEC_PRINT(a)                    \
{                           \
   double vlen;                     \
   VEC_LENGTH (vlen, (a));              \
   printf (" a is %f %f %f length of a is %f \n", (a)[0], (a)[1], (a)[2], vlen); \
}

/* ========================================================== */
/* Vector print */

#define VEC_PRINT_4(a)                  \
{                           \
   double vlen;                     \
   VEC_LENGTH_4 (vlen, (a));                \
   printf (" a is %f %f %f %f length of a is %f \n",    \
       (a)[0], (a)[1], (a)[2], (a)[3], vlen);       \
}

/* ========================================================== */
/* print matrix */

#define MAT_PRINT_4X4(mmm) {                \
   int i,j;                     \
   printf ("matrix mmm is \n");             \
   if (mmm == NULL) {                   \
      printf (" Null \n");              \
   } else {                     \
      for (i=0; i<4; i++) {             \
         for (j=0; j<4; j++) {              \
            printf ("%f ", mmm[i][j]);          \
         }                      \
         printf (" \n");                \
      }                         \
   }                            \
}

/* ========================================================== */
/* print matrix */

#define MAT_PRINT_3X3(mmm) {                \
   int i,j;                     \
   printf ("matrix mmm is \n");             \
   if (mmm == NULL) {                   \
      printf (" Null \n");              \
   } else {                     \
      for (i=0; i<3; i++) {             \
         for (j=0; j<3; j++) {              \
            printf ("%f ", mmm[i][j]);          \
         }                      \
         printf (" \n");                \
      }                         \
   }                            \
}

/* ========================================================== */
/* print matrix */

#define MAT_PRINT_2X3(mmm) {                \
   int i,j;                     \
   printf ("matrix mmm is \n");             \
   if (mmm == NULL) {                   \
      printf (" Null \n");              \
   } else {                     \
      for (i=0; i<2; i++) {             \
         for (j=0; j<3; j++) {              \
            printf ("%f ", mmm[i][j]);          \
         }                      \
         printf (" \n");                \
      }                         \
   }                            \
}

/* ========================================================== */
/* initialize matrix */

#define IDENTIFY_MATRIX_3X3(m)          \
{                       \
   m[0][0] = 1.0;               \
   m[0][1] = 0.0;               \
   m[0][2] = 0.0;               \
                        \
   m[1][0] = 0.0;               \
   m[1][1] = 1.0;               \
   m[1][2] = 0.0;               \
                        \
   m[2][0] = 0.0;               \
   m[2][1] = 0.0;               \
   m[2][2] = 1.0;               \
}

/* ========================================================== */
/* initialize matrix */

#define IDENTIFY_MATRIX_4X4(m)          \
{                       \
   m[0][0] = 1.0;               \
   m[0][1] = 0.0;               \
   m[0][2] = 0.0;               \
   m[0][3] = 0.0;               \
                        \
   m[1][0] = 0.0;               \
   m[1][1] = 1.0;               \
   m[1][2] = 0.0;               \
   m[1][3] = 0.0;               \
                        \
   m[2][0] = 0.0;               \
   m[2][1] = 0.0;               \
   m[2][2] = 1.0;               \
   m[2][3] = 0.0;               \
                        \
   m[3][0] = 0.0;               \
   m[3][1] = 0.0;               \
   m[3][2] = 0.0;               \
   m[3][3] = 1.0;               \
}

/* ========================================================== */
/* matrix copy */

#define COPY_MATRIX_2X2(b,a)    \
{               \
   b[0][0] = a[0][0];       \
   b[0][1] = a[0][1];       \
                \
   b[1][0] = a[1][0];       \
   b[1][1] = a[1][1];       \
                \
}

/* ========================================================== */
/* matrix copy */

#define COPY_MATRIX_2X3(b,a)    \
{               \
   b[0][0] = a[0][0];       \
   b[0][1] = a[0][1];       \
   b[0][2] = a[0][2];       \
                \
   b[1][0] = a[1][0];       \
   b[1][1] = a[1][1];       \
   b[1][2] = a[1][2];       \
}

/* ========================================================== */
/* matrix copy */

#define COPY_MATRIX_3X3(b,a)    \
{               \
   b[0][0] = a[0][0];       \
   b[0][1] = a[0][1];       \
   b[0][2] = a[0][2];       \
                \
   b[1][0] = a[1][0];       \
   b[1][1] = a[1][1];       \
   b[1][2] = a[1][2];       \
                \
   b[2][0] = a[2][0];       \
   b[2][1] = a[2][1];       \
   b[2][2] = a[2][2];       \
}

/* ========================================================== */
/* matrix copy */

#define COPY_MATRIX_4X4(b,a)    \
{               \
   b[0][0] = a[0][0];       \
   b[0][1] = a[0][1];       \
   b[0][2] = a[0][2];       \
   b[0][3] = a[0][3];       \
                \
   b[1][0] = a[1][0];       \
   b[1][1] = a[1][1];       \
   b[1][2] = a[1][2];       \
   b[1][3] = a[1][3];       \
                \
   b[2][0] = a[2][0];       \
   b[2][1] = a[2][1];       \
   b[2][2] = a[2][2];       \
   b[2][3] = a[2][3];       \
                \
   b[3][0] = a[3][0];       \
   b[3][1] = a[3][1];       \
   b[3][2] = a[3][2];       \
   b[3][3] = a[3][3];       \
}

/* ========================================================== */
/* matrix transpose */

#define TRANSPOSE_MATRIX_2X2(b,a)   \
{               \
   b[0][0] = a[0][0];       \
   b[0][1] = a[1][0];       \
                \
   b[1][0] = a[0][1];       \
   b[1][1] = a[1][1];       \
}

/* ========================================================== */
/* matrix transpose */

#define TRANSPOSE_MATRIX_3X3(b,a)   \
{               \
   b[0][0] = a[0][0];       \
   b[0][1] = a[1][0];       \
   b[0][2] = a[2][0];       \
                \
   b[1][0] = a[0][1];       \
   b[1][1] = a[1][1];       \
   b[1][2] = a[2][1];       \
                \
   b[2][0] = a[0][2];       \
   b[2][1] = a[1][2];       \
   b[2][2] = a[2][2];       \
}

/* ========================================================== */
/* matrix transpose */

#define TRANSPOSE_MATRIX_4X4(b,a)   \
{               \
   b[0][0] = a[0][0];       \
   b[0][1] = a[1][0];       \
   b[0][2] = a[2][0];       \
   b[0][3] = a[3][0];       \
                \
   b[1][0] = a[0][1];       \
   b[1][1] = a[1][1];       \
   b[1][2] = a[2][1];       \
   b[1][3] = a[3][1];       \
                \
   b[2][0] = a[0][2];       \
   b[2][1] = a[1][2];       \
   b[2][2] = a[2][2];       \
   b[2][3] = a[3][2];       \
                \
   b[3][0] = a[0][3];       \
   b[3][1] = a[1][3];       \
   b[3][2] = a[2][3];       \
   b[3][3] = a[3][3];       \
}

/* ========================================================== */
/* multiply matrix by scalar */

#define SCALE_MATRIX_2X2(b,s,a)     \
{                   \
   b[0][0] = (s) * a[0][0];     \
   b[0][1] = (s) * a[0][1];     \
                    \
   b[1][0] = (s) * a[1][0];     \
   b[1][1] = (s) * a[1][1];     \
}

/* ========================================================== */
/* multiply matrix by scalar */

#define SCALE_MATRIX_3X3(b,s,a)     \
{                   \
   b[0][0] = (s) * a[0][0];     \
   b[0][1] = (s) * a[0][1];     \
   b[0][2] = (s) * a[0][2];     \
                    \
   b[1][0] = (s) * a[1][0];     \
   b[1][1] = (s) * a[1][1];     \
   b[1][2] = (s) * a[1][2];     \
                    \
   b[2][0] = (s) * a[2][0];     \
   b[2][1] = (s) * a[2][1];     \
   b[2][2] = (s) * a[2][2];     \
}

/* ========================================================== */
/* multiply matrix by scalar */

#define SCALE_MATRIX_4X4(b,s,a)     \
{                   \
   b[0][0] = (s) * a[0][0];     \
   b[0][1] = (s) * a[0][1];     \
   b[0][2] = (s) * a[0][2];     \
   b[0][3] = (s) * a[0][3];     \
                    \
   b[1][0] = (s) * a[1][0];     \
   b[1][1] = (s) * a[1][1];     \
   b[1][2] = (s) * a[1][2];     \
   b[1][3] = (s) * a[1][3];     \
                    \
   b[2][0] = (s) * a[2][0];     \
   b[2][1] = (s) * a[2][1];     \
   b[2][2] = (s) * a[2][2];     \
   b[2][3] = (s) * a[2][3];     \
                    \
   b[3][0] = s * a[3][0];       \
   b[3][1] = s * a[3][1];       \
   b[3][2] = s * a[3][2];       \
   b[3][3] = s * a[3][3];       \
}

/* ========================================================== */
/* multiply matrix by scalar */

#define ACCUM_SCALE_MATRIX_2X2(b,s,a)       \
{                   \
   b[0][0] += (s) * a[0][0];        \
   b[0][1] += (s) * a[0][1];        \
                    \
   b[1][0] += (s) * a[1][0];        \
   b[1][1] += (s) * a[1][1];        \
}

/* +========================================================== */
/* multiply matrix by scalar */

#define ACCUM_SCALE_MATRIX_3X3(b,s,a)       \
{                   \
   b[0][0] += (s) * a[0][0];        \
   b[0][1] += (s) * a[0][1];        \
   b[0][2] += (s) * a[0][2];        \
                    \
   b[1][0] += (s) * a[1][0];        \
   b[1][1] += (s) * a[1][1];        \
   b[1][2] += (s) * a[1][2];        \
                    \
   b[2][0] += (s) * a[2][0];        \
   b[2][1] += (s) * a[2][1];        \
   b[2][2] += (s) * a[2][2];        \
}

/* +========================================================== */
/* multiply matrix by scalar */

#define ACCUM_SCALE_MATRIX_4X4(b,s,a)       \
{                   \
   b[0][0] += (s) * a[0][0];        \
   b[0][1] += (s) * a[0][1];        \
   b[0][2] += (s) * a[0][2];        \
   b[0][3] += (s) * a[0][3];        \
                    \
   b[1][0] += (s) * a[1][0];        \
   b[1][1] += (s) * a[1][1];        \
   b[1][2] += (s) * a[1][2];        \
   b[1][3] += (s) * a[1][3];        \
                    \
   b[2][0] += (s) * a[2][0];        \
   b[2][1] += (s) * a[2][1];        \
   b[2][2] += (s) * a[2][2];        \
   b[2][3] += (s) * a[2][3];        \
                    \
   b[3][0] += (s) * a[3][0];        \
   b[3][1] += (s) * a[3][1];        \
   b[3][2] += (s) * a[3][2];        \
   b[3][3] += (s) * a[3][3];        \
}

/* +========================================================== */
/* matrix product */
/* c[x][y] = a[x][0]*b[0][y]+a[x][1]*b[1][y]+a[x][2]*b[2][y]+a[x][3]*b[3][y];*/

#define MATRIX_PRODUCT_2X2(c,a,b)       \
{                       \
   c[0][0] = a[0][0]*b[0][0]+a[0][1]*b[1][0];   \
   c[0][1] = a[0][0]*b[0][1]+a[0][1]*b[1][1];   \
                        \
   c[1][0] = a[1][0]*b[0][0]+a[1][1]*b[1][0];   \
   c[1][1] = a[1][0]*b[0][1]+a[1][1]*b[1][1];   \
                        \
}

/* ========================================================== */
/* matrix product */
/* c[x][y] = a[x][0]*b[0][y]+a[x][1]*b[1][y]+a[x][2]*b[2][y]+a[x][3]*b[3][y];*/

#define MATRIX_PRODUCT_3X3(c,a,b)               \
{                               \
   c[0][0] = a[0][0]*b[0][0]+a[0][1]*b[1][0]+a[0][2]*b[2][0];   \
   c[0][1] = a[0][0]*b[0][1]+a[0][1]*b[1][1]+a[0][2]*b[2][1];   \
   c[0][2] = a[0][0]*b[0][2]+a[0][1]*b[1][2]+a[0][2]*b[2][2];   \
                                \
   c[1][0] = a[1][0]*b[0][0]+a[1][1]*b[1][0]+a[1][2]*b[2][0];   \
   c[1][1] = a[1][0]*b[0][1]+a[1][1]*b[1][1]+a[1][2]*b[2][1];   \
   c[1][2] = a[1][0]*b[0][2]+a[1][1]*b[1][2]+a[1][2]*b[2][2];   \
                                \
   c[2][0] = a[2][0]*b[0][0]+a[2][1]*b[1][0]+a[2][2]*b[2][0];   \
   c[2][1] = a[2][0]*b[0][1]+a[2][1]*b[1][1]+a[2][2]*b[2][1];   \
   c[2][2] = a[2][0]*b[0][2]+a[2][1]*b[1][2]+a[2][2]*b[2][2];   \
}

/* ========================================================== */
/* matrix product */
/* c[x][y] = a[x][0]*b[0][y]+a[x][1]*b[1][y]+a[x][2]*b[2][y]+a[x][3]*b[3][y];*/

#define MATRIX_PRODUCT_4X4(c,a,b)       \
{                       \
   c[0][0] = a[0][0]*b[0][0]+a[0][1]*b[1][0]+a[0][2]*b[2][0]+a[0][3]*b[3][0];\
   c[0][1] = a[0][0]*b[0][1]+a[0][1]*b[1][1]+a[0][2]*b[2][1]+a[0][3]*b[3][1];\
   c[0][2] = a[0][0]*b[0][2]+a[0][1]*b[1][2]+a[0][2]*b[2][2]+a[0][3]*b[3][2];\
   c[0][3] = a[0][0]*b[0][3]+a[0][1]*b[1][3]+a[0][2]*b[2][3]+a[0][3]*b[3][3];\
                        \
   c[1][0] = a[1][0]*b[0][0]+a[1][1]*b[1][0]+a[1][2]*b[2][0]+a[1][3]*b[3][0];\
   c[1][1] = a[1][0]*b[0][1]+a[1][1]*b[1][1]+a[1][2]*b[2][1]+a[1][3]*b[3][1];\
   c[1][2] = a[1][0]*b[0][2]+a[1][1]*b[1][2]+a[1][2]*b[2][2]+a[1][3]*b[3][2];\
   c[1][3] = a[1][0]*b[0][3]+a[1][1]*b[1][3]+a[1][2]*b[2][3]+a[1][3]*b[3][3];\
                        \
   c[2][0] = a[2][0]*b[0][0]+a[2][1]*b[1][0]+a[2][2]*b[2][0]+a[2][3]*b[3][0];\
   c[2][1] = a[2][0]*b[0][1]+a[2][1]*b[1][1]+a[2][2]*b[2][1]+a[2][3]*b[3][1];\
   c[2][2] = a[2][0]*b[0][2]+a[2][1]*b[1][2]+a[2][2]*b[2][2]+a[2][3]*b[3][2];\
   c[2][3] = a[2][0]*b[0][3]+a[2][1]*b[1][3]+a[2][2]*b[2][3]+a[2][3]*b[3][3];\
                        \
   c[3][0] = a[3][0]*b[0][0]+a[3][1]*b[1][0]+a[3][2]*b[2][0]+a[3][3]*b[3][0];\
   c[3][1] = a[3][0]*b[0][1]+a[3][1]*b[1][1]+a[3][2]*b[2][1]+a[3][3]*b[3][1];\
   c[3][2] = a[3][0]*b[0][2]+a[3][1]*b[1][2]+a[3][2]*b[2][2]+a[3][3]*b[3][2];\
   c[3][3] = a[3][0]*b[0][3]+a[3][1]*b[1][3]+a[3][2]*b[2][3]+a[3][3]*b[3][3];\
}

/* ========================================================== */
/* matrix times vector */

#define MAT_DOT_VEC_2X2(p,m,v)                  \
{                               \
   p[0] = m[0][0]*v[0] + m[0][1]*v[1];              \
   p[1] = m[1][0]*v[0] + m[1][1]*v[1];              \
}

/* ========================================================== */
/* matrix times vector */

#define MAT_DOT_VEC_3X3(p,m,v)                  \
{                               \
   p[0] = m[0][0]*v[0] + m[0][1]*v[1] + m[0][2]*v[2];       \
   p[1] = m[1][0]*v[0] + m[1][1]*v[1] + m[1][2]*v[2];       \
   p[2] = m[2][0]*v[0] + m[2][1]*v[1] + m[2][2]*v[2];       \
}

/* ========================================================== */
/* matrix times vector */

#define MAT_DOT_VEC_4X4(p,m,v)                  \
{                               \
   p[0] = m[0][0]*v[0] + m[0][1]*v[1] + m[0][2]*v[2] + m[0][3]*v[3];    \
   p[1] = m[1][0]*v[0] + m[1][1]*v[1] + m[1][2]*v[2] + m[1][3]*v[3];    \
   p[2] = m[2][0]*v[0] + m[2][1]*v[1] + m[2][2]*v[2] + m[2][3]*v[3];    \
   p[3] = m[3][0]*v[0] + m[3][1]*v[1] + m[3][2]*v[2] + m[3][3]*v[3];    \
}

/* ========================================================== */
/* vector transpose times matrix */
/* p[j] = v[0]*m[0][j] + v[1]*m[1][j] + v[2]*m[2][j]; */

#define VEC_DOT_MAT_3X3(p,v,m)                  \
{                               \
   p[0] = v[0]*m[0][0] + v[1]*m[1][0] + v[2]*m[2][0];       \
   p[1] = v[0]*m[0][1] + v[1]*m[1][1] + v[2]*m[2][1];       \
   p[2] = v[0]*m[0][2] + v[1]*m[1][2] + v[2]*m[2][2];       \
}

/* ========================================================== */
/* affine matrix times vector */
/* The matrix is assumed to be an affine matrix, with last two 
 * entries representing a translation */

#define MAT_DOT_VEC_2X3(p,m,v)                  \
{                               \
   p[0] = m[0][0]*v[0] + m[0][1]*v[1] + m[0][2];        \
   p[1] = m[1][0]*v[0] + m[1][1]*v[1] + m[1][2];        \
}

/* ========================================================== */
/* inverse transpose of matrix times vector
 *
 * This macro computes inverse transpose of matrix m, 
 * and multiplies vector v into it, to yeild vector p
 *
 * DANGER !!! Do Not use this on normal vectors!!!
 * It will leave normals the wrong length !!!
 * See macro below for use on normals.
 */

#define INV_TRANSP_MAT_DOT_VEC_2X2(p,m,v)           \
{                               \
   gleDouble det;                       \
                                \
   det = m[0][0]*m[1][1] - m[0][1]*m[1][0];         \
   p[0] = m[1][1]*v[0] - m[1][0]*v[1];              \
   p[1] = - m[0][1]*v[0] + m[0][0]*v[1];            \
                                \
   /* if matrix not singular, and not orthonormal, then renormalize */ \
   if ((det!=1.0) && (det != 0.0)) {                \
      det = 1.0 / det;                      \
      p[0] *= det;                      \
      p[1] *= det;                      \
   }                                \
}

/* ========================================================== */
/* transform normal vector by inverse transpose of matrix 
 * and then renormalize the vector 
 *
 * This macro computes inverse transpose of matrix m, 
 * and multiplies vector v into it, to yeild vector p
 * Vector p is then normalized.
 */


#define NORM_XFORM_2X2(p,m,v)                   \
{                               \
   double mlen;                         \
                                \
   /* do nothing if off-diagonals are zero and diagonals are    \
    * equal */                          \
   if ((m[0][1] != 0.0) || (m[1][0] != 0.0) || (m[0][0] != m[1][1])) { \
      p[0] = m[1][1]*v[0] - m[1][0]*v[1];           \
      p[1] = - m[0][1]*v[0] + m[0][0]*v[1];         \
                                \
      mlen = p[0]*p[0] + p[1]*p[1];             \
      mlen = 1.0 / sqrt (mlen);                 \
      p[0] *= mlen;                     \
      p[1] *= mlen;                     \
   } else {                         \
      VEC_COPY_2 (p, v);                    \
   }                                \
}

/* ========================================================== */
/* outer product of vector times vector transpose 
 *
 * The outer product of vector v and vector transpose t yeilds 
 * dyadic matrix m.
 */

#define OUTER_PRODUCT_2X2(m,v,t)                \
{                               \
   m[0][0] = v[0] * t[0];                   \
   m[0][1] = v[0] * t[1];                   \
                                \
   m[1][0] = v[1] * t[0];                   \
   m[1][1] = v[1] * t[1];                   \
}

/* ========================================================== */
/* outer product of vector times vector transpose 
 *
 * The outer product of vector v and vector transpose t yeilds 
 * dyadic matrix m.
 */

#define OUTER_PRODUCT_3X3(m,v,t)                \
{                               \
   m[0][0] = v[0] * t[0];                   \
   m[0][1] = v[0] * t[1];                   \
   m[0][2] = v[0] * t[2];                   \
                                \
   m[1][0] = v[1] * t[0];                   \
   m[1][1] = v[1] * t[1];                   \
   m[1][2] = v[1] * t[2];                   \
                                \
   m[2][0] = v[2] * t[0];                   \
   m[2][1] = v[2] * t[1];                   \
   m[2][2] = v[2] * t[2];                   \
}

/* ========================================================== */
/* outer product of vector times vector transpose 
 *
 * The outer product of vector v and vector transpose t yeilds 
 * dyadic matrix m.
 */

#define OUTER_PRODUCT_4X4(m,v,t)                \
{                               \
   m[0][0] = v[0] * t[0];                   \
   m[0][1] = v[0] * t[1];                   \
   m[0][2] = v[0] * t[2];                   \
   m[0][3] = v[0] * t[3];                   \
                                \
   m[1][0] = v[1] * t[0];                   \
   m[1][1] = v[1] * t[1];                   \
   m[1][2] = v[1] * t[2];                   \
   m[1][3] = v[1] * t[3];                   \
                                \
   m[2][0] = v[2] * t[0];                   \
   m[2][1] = v[2] * t[1];                   \
   m[2][2] = v[2] * t[2];                   \
   m[2][3] = v[2] * t[3];                   \
                                \
   m[3][0] = v[3] * t[0];                   \
   m[3][1] = v[3] * t[1];                   \
   m[3][2] = v[3] * t[2];                   \
   m[3][3] = v[3] * t[3];                   \
}

/* +========================================================== */
/* outer product of vector times vector transpose 
 *
 * The outer product of vector v and vector transpose t yeilds 
 * dyadic matrix m.
 */

#define ACCUM_OUTER_PRODUCT_2X2(m,v,t)              \
{                               \
   m[0][0] += v[0] * t[0];                  \
   m[0][1] += v[0] * t[1];                  \
                                \
   m[1][0] += v[1] * t[0];                  \
   m[1][1] += v[1] * t[1];                  \
}

/* +========================================================== */
/* outer product of vector times vector transpose 
 *
 * The outer product of vector v and vector transpose t yeilds 
 * dyadic matrix m.
 */

#define ACCUM_OUTER_PRODUCT_3X3(m,v,t)              \
{                               \
   m[0][0] += v[0] * t[0];                  \
   m[0][1] += v[0] * t[1];                  \
   m[0][2] += v[0] * t[2];                  \
                                \
   m[1][0] += v[1] * t[0];                  \
   m[1][1] += v[1] * t[1];                  \
   m[1][2] += v[1] * t[2];                  \
                                \
   m[2][0] += v[2] * t[0];                  \
   m[2][1] += v[2] * t[1];                  \
   m[2][2] += v[2] * t[2];                  \
}

/* +========================================================== */
/* outer product of vector times vector transpose 
 *
 * The outer product of vector v and vector transpose t yeilds 
 * dyadic matrix m.
 */

#define ACCUM_OUTER_PRODUCT_4X4(m,v,t)              \
{                               \
   m[0][0] += v[0] * t[0];                  \
   m[0][1] += v[0] * t[1];                  \
   m[0][2] += v[0] * t[2];                  \
   m[0][3] += v[0] * t[3];                  \
                                \
   m[1][0] += v[1] * t[0];                  \
   m[1][1] += v[1] * t[1];                  \
   m[1][2] += v[1] * t[2];                  \
   m[1][3] += v[1] * t[3];                  \
                                \
   m[2][0] += v[2] * t[0];                  \
   m[2][1] += v[2] * t[1];                  \
   m[2][2] += v[2] * t[2];                  \
   m[2][3] += v[2] * t[3];                  \
                                \
   m[3][0] += v[3] * t[0];                  \
   m[3][1] += v[3] * t[1];                  \
   m[3][2] += v[3] * t[2];                  \
   m[3][3] += v[3] * t[3];                  \
}

/* +========================================================== */
/* determinant of matrix
 *
 * Computes determinant of matrix m, returning d
 */

#define DETERMINANT_2X2(d,m)                    \
{                               \
   d = m[0][0] * m[1][1] - m[0][1] * m[1][0];           \
}

/* ========================================================== */
/* determinant of matrix
 *
 * Computes determinant of matrix m, returning d
 */

#define DETERMINANT_3X3(d,m)                    \
{                               \
   d = m[0][0] * (m[1][1]*m[2][2] - m[1][2] * m[2][1]);     \
   d -= m[0][1] * (m[1][0]*m[2][2] - m[1][2] * m[2][0]);    \
   d += m[0][2] * (m[1][0]*m[2][1] - m[1][1] * m[2][0]);    \
}

/* ========================================================== */
/* i,j,th cofactor of a 4x4 matrix
 *
 */

#define COFACTOR_4X4_IJ(fac,m,i,j)              \
{                               \
   int ii[4], jj[4], k;                     \
                                \
   /* compute which row, columnt to skip */         \
   for (k=0; k<i; k++) ii[k] = k;               \
   for (k=i; k<3; k++) ii[k] = k+1;             \
   for (k=0; k<j; k++) jj[k] = k;               \
   for (k=j; k<3; k++) jj[k] = k+1;             \
                                \
   (fac) = m[ii[0]][jj[0]] * (m[ii[1]][jj[1]]*m[ii[2]][jj[2]]   \
                            - m[ii[1]][jj[2]]*m[ii[2]][jj[1]]); \
   (fac) -= m[ii[0]][jj[1]] * (m[ii[1]][jj[0]]*m[ii[2]][jj[2]]  \
                             - m[ii[1]][jj[2]]*m[ii[2]][jj[0]]);\
   (fac) += m[ii[0]][jj[2]] * (m[ii[1]][jj[0]]*m[ii[2]][jj[1]]  \
                             - m[ii[1]][jj[1]]*m[ii[2]][jj[0]]);\
                                \
   /* compute sign */                       \
   k = i+j;                         \
   if ( k != (k/2)*2) {                     \
      (fac) = -(fac);                       \
   }                                \
}

/* ========================================================== */
/* determinant of matrix
 *
 * Computes determinant of matrix m, returning d
 */

#define DETERMINANT_4X4(d,m)                    \
{                               \
   double cofac;                        \
   COFACTOR_4X4_IJ (cofac, m, 0, 0);                \
   d = m[0][0] * cofac;                     \
   COFACTOR_4X4_IJ (cofac, m, 0, 1);                \
   d += m[0][1] * cofac;                    \
   COFACTOR_4X4_IJ (cofac, m, 0, 2);                \
   d += m[0][2] * cofac;                    \
   COFACTOR_4X4_IJ (cofac, m, 0, 3);                \
   d += m[0][3] * cofac;                    \
}

/* ========================================================== */
/* cofactor of matrix
 *
 * Computes cofactor of matrix m, returning a
 */

#define COFACTOR_2X2(a,m)                   \
{                               \
   a[0][0] = (m)[1][1];                     \
   a[0][1] = - (m)[1][0];                       \
   a[1][0] = - (m)[0][1];                       \
   a[1][1] = (m)[0][0];                     \
}

/* ========================================================== */
/* cofactor of matrix
 *
 * Computes cofactor of matrix m, returning a
 */

#define COFACTOR_3X3(a,m)                   \
{                               \
   a[0][0] = m[1][1]*m[2][2] - m[1][2]*m[2][1];         \
   a[0][1] = - (m[1][0]*m[2][2] - m[2][0]*m[1][2]);     \
   a[0][2] = m[1][0]*m[2][1] - m[1][1]*m[2][0];         \
   a[1][0] = - (m[0][1]*m[2][2] - m[0][2]*m[2][1]);     \
   a[1][1] = m[0][0]*m[2][2] - m[0][2]*m[2][0];         \
   a[1][2] = - (m[0][0]*m[2][1] - m[0][1]*m[2][0]);     \
   a[2][0] = m[0][1]*m[1][2] - m[0][2]*m[1][1];         \
   a[2][1] = - (m[0][0]*m[1][2] - m[0][2]*m[1][0]);     \
   a[2][2] = m[0][0]*m[1][1] - m[0][1]*m[1][0]);        \
}

/* ========================================================== */
/* cofactor of matrix
 *
 * Computes cofactor of matrix m, returning a
 */

#define COFACTOR_4X4(a,m)                   \
{                               \
   int i,j;                         \
                                \
   for (i=0; i<4; i++) {                    \
      for (j=0; j<4; j++) {                 \
         COFACTOR_4X4_IJ (a[i][j], m, i, j);            \
      }                             \
   }                                \
}

/* ========================================================== */
/* adjoint of matrix
 *
 * Computes adjoint of matrix m, returning a
 * (Note that adjoint is just the transpose of the cofactor matrix)
 */

#define ADJOINT_2X2(a,m)                    \
{                               \
   a[0][0] = (m)[1][1];                     \
   a[1][0] = - (m)[1][0];                       \
   a[0][1] = - (m)[0][1];                       \
   a[1][1] = (m)[0][0];                     \
}

/* ========================================================== */
/* adjoint of matrix
 *
 * Computes adjoint of matrix m, returning a
 * (Note that adjoint is just the transpose of the cofactor matrix)
 */


#define ADJOINT_3X3(a,m)                    \
{                               \
   a[0][0] = m[1][1]*m[2][2] - m[1][2]*m[2][1];         \
   a[1][0] = - (m[1][0]*m[2][2] - m[2][0]*m[1][2]);     \
   a[2][0] = m[1][0]*m[2][1] - m[1][1]*m[2][0];         \
   a[0][1] = - (m[0][1]*m[2][2] - m[0][2]*m[2][1]);     \
   a[1][1] = m[0][0]*m[2][2] - m[0][2]*m[2][0];         \
   a[2][1] = - (m[0][0]*m[2][1] - m[0][1]*m[2][0]);     \
   a[0][2] = m[0][1]*m[1][2] - m[0][2]*m[1][1];         \
   a[1][2] = - (m[0][0]*m[1][2] - m[0][2]*m[1][0]);     \
   a[2][2] = m[0][0]*m[1][1] - m[0][1]*m[1][0]);        \
}

/* ========================================================== */
/* adjoint of matrix
 *
 * Computes adjoint of matrix m, returning a
 * (Note that adjoint is just the transpose of the cofactor matrix)
 */

#define ADJOINT_4X4(a,m)                    \
{                               \
   int i,j;                         \
                                \
   for (i=0; i<4; i++) {                    \
      for (j=0; j<4; j++) {                 \
         COFACTOR_4X4_IJ (a[j][i], m, i, j);            \
      }                             \
   }                                \
}

/* ========================================================== */
/* compute adjoint of matrix and scale
 *
 * Computes adjoint of matrix m, scales it by s, returning a
 */

#define SCALE_ADJOINT_2X2(a,s,m)                \
{                               \
   a[0][0] = (s) * m[1][1];                 \
   a[1][0] = - (s) * m[1][0];                   \
   a[0][1] = - (s) * m[0][1];                   \
   a[1][1] = (s) * m[0][0];                 \
}

/* ========================================================== */
/* compute adjoint of matrix and scale
 *
 * Computes adjoint of matrix m, scales it by s, returning a
 */

#define SCALE_ADJOINT_3X3(a,s,m)                \
{                               \
   a[0][0] = (s) * (m[1][1] * m[2][2] - m[1][2] * m[2][1]); \
   a[1][0] = (s) * (m[1][2] * m[2][0] - m[1][0] * m[2][2]); \
   a[2][0] = (s) * (m[1][0] * m[2][1] - m[1][1] * m[2][0]); \
                                \
   a[0][1] = (s) * (m[0][2] * m[2][1] - m[0][1] * m[2][2]); \
   a[1][1] = (s) * (m[0][0] * m[2][2] - m[0][2] * m[2][0]); \
   a[2][1] = (s) * (m[0][1] * m[2][0] - m[0][0] * m[2][1]); \
                                \
   a[0][2] = (s) * (m[0][1] * m[1][2] - m[0][2] * m[1][1]); \
   a[1][2] = (s) * (m[0][2] * m[1][0] - m[0][0] * m[1][2]); \
   a[2][2] = (s) * (m[0][0] * m[1][1] - m[0][1] * m[1][0]); \
}

/* ========================================================== */
/* compute adjoint of matrix and scale
 *
 * Computes adjoint of matrix m, scales it by s, returning a
 */

#define SCALE_ADJOINT_4X4(a,s,m)                \
{                               \
   int i,j;                         \
                                \
   for (i=0; i<4; i++) {                    \
      for (j=0; j<4; j++) {                 \
         COFACTOR_4X4_IJ (a[j][i], m, i, j);            \
         a[j][i] *= s;                      \
      }                             \
   }                                \
}

/* ========================================================== */
/* inverse of matrix 
 *
 * Compute inverse of matrix a, returning determinant m and 
 * inverse b
 */

#define INVERT_2X2(b,det,a)         \
{                       \
   double tmp;                  \
   DETERMINANT_2X2 (det, a);            \
   tmp = 1.0 / (det);               \
   SCALE_ADJOINT_2X2 (b, tmp, a);       \
}

/* ========================================================== */
/* inverse of matrix 
 *
 * Compute inverse of matrix a, returning determinant m and 
 * inverse b
 */

#define INVERT_3X3(b,det,a)         \
{                       \
   double tmp;                  \
   DETERMINANT_3X3 (det, a);            \
   tmp = 1.0 / (det);               \
   SCALE_ADJOINT_3X3 (b, tmp, a);       \
}

/* ========================================================== */
/* inverse of matrix 
 *
 * Compute inverse of matrix a, returning determinant m and 
 * inverse b
 */

#define INVERT_4X4(b,det,a)         \
{                       \
   double tmp;                  \
   DETERMINANT_4X4 (det, a);            \
   tmp = 1.0 / (det);               \
   SCALE_ADJOINT_4X4 (b, tmp, a);       \
}

/* ========================================================== */
#if defined(__cplusplus) || defined(c_plusplus)
}
#endif

#endif /* __GUTIL_VECTOR_H__ */
/* ===================== END OF FILE ======================== */
      - name: 
// Translation of kalman.py

#include <stdio.h>
#include <string.h>
#include "matvec.h"
#include "kalman.h"

void init_kalman3_pa(kalman3_pa_t *k, KALMAN_FLOAT sampleRate,
                     KALMAN_FLOAT q, KALMAN_FLOAT pR, KALMAN_FLOAT aR)
{
    memset(k, 0, sizeof(kalman3_pa_t));

    KALMAN_FLOAT T = k->T = 1.0f/sampleRate;

    kalman3_pa_set_covariance(k, sampleRate, q, pR, aR);

    // Initial error covariance
    IDENTIFY_MATRIX_3X3(k->P);

    // Motion matrix
    IDENTIFY_MATRIX_3X3(k->A);
    k->A[0][1] = T;
    k->A[0][2] = 0.5*T*T;
    k->A[1][2] = T;

    // Initial state
    k->x_hat[0] = k->x_hat[1] = k->x_hat[2] = 0;
    k->x_hat_est[0] = k->x_hat_est[1] = k->x_hat_est[2] = 0;

    // Measurement matrix, measured = H*state + noise
    k->H[0][0] = 1.0f;
    k->H[0][1] = k->H[0][2] = 0;
    k->H[1][2] = 1.0f;
    k->H[1][0] = k->H[1][1] = 0;
}

void kalman3_pa_set_covariance(kalman3_pa_t *k, KALMAN_FLOAT sampleRate,
                               KALMAN_FLOAT q, KALMAN_FLOAT pR,
                               KALMAN_FLOAT aR)
{
    KALMAN_FLOAT T = k->T = 1.0f/sampleRate;

    // Process covariance
    k->Q[0][0]                           = T*T*T*T*T / 4 * q;
    k->Q[0][1] = k->Q[1][0]              = T*T*T*T / 2 * q;
    k->Q[0][2] = k->Q[1][1] = k->Q[2][0] = T*T*T * q;
    k->Q[0][2] /= 2;
    k->Q[1][2] = k->Q[2][1]              = T*T * q;
    k->Q[2][2]                           = T * q;

    // Measurement covariance
    k->R[0][0] = pR;
    k->R[0][1] = 0;
    k->R[1][0] = 0;
    k->R[1][1] = aR;
}

void step_kalman3_pa(kalman3_pa_t *k, KALMAN_FLOAT pos, KALMAN_FLOAT acc)
{
    KALMAN_FLOAT obs[2] = { pos, acc };
    KALMAN_FLOAT tmp2[3][3], tmp3[3][3];
    KALMAN_FLOAT tmpv[3], det;
    KALMAN_FLOAT I[3][3]={{1,0,0},{0,1,0},{0,0,1}};

    // Make prediction
    MAT_DOT_VEC_3X3(k->x_hat_est, k->A, k->x_hat);  // result 1x3

    MATRIX_PRODUCT_3X3_3X3T(tmp2, k->P, k->A);
    MATRIX_PRODUCT_3X3(tmp3, k->A, tmp2);
    MATRIX_ADD_3X3(k->P_est, tmp3, k->Q);

    // Update estimate
    MAT_DOT_VEC_3X2(tmpv, k->H, k->x_hat_est);  // result 

    VEC_DIFF_2(k->error_x, obs, tmpv);

    MATRIX_PRODUCT_3X3_3X2T(tmp2, k->P, k->H);   // result 2x3
    MATRIX_PRODUCT_3X2_2X3(tmp3, k->H, tmp2);   // result 2x2
    MATRIX_ADD_2X2(k->error_P, tmp3, k->R); // result 2x2

    MATRIX_PRODUCT_3X3_3X2T(tmp3, k->P_est, k->H); // result 2x3
    MAT_SOLVE_AX_EQ_B_2X3_2X2(k->K, tmp3, k->error_P); // result 2x3

    MAT_DOT_VEC_2X3_FULL(tmpv, k->K, k->error_x);  // result 1x3
    VEC_SUM(k->x_hat, k->x_hat_est, tmpv);         // result 1x3

    MATRIX_PRODUCT_2X3_3X2(tmp2, k->K, k->H);
    MATRIX_SUB_3X3(tmp3, I, tmp2);
    MATRIX_PRODUCT_3X3(k->P, tmp3, k->P_est);
}

void run_kalman3_pa(KALMAN_FLOAT sampleRate,
                    KALMAN_FLOAT q, KALMAN_FLOAT pR, KALMAN_FLOAT aR,
                    KALMAN_FLOAT *pos, KALMAN_FLOAT *acc,
                    KALMAN_FLOAT *result, int N)
{
    kalman3_pa_t k;
    init_kalman3_pa(&k, sampleRate, q, pR, aR);

    while (N-- > 0) {
        step_kalman3_pa(&k, *(pos++), *(acc++));
        *(result++) = k.x_hat[0];
        *(result++) = k.x_hat[1];
        *(result++) = k.x_hat[2];
    }
}


void init_kalman2_p(kalman2_p_t *k, KALMAN_FLOAT sampleRate,
                    KALMAN_FLOAT q, KALMAN_FLOAT pR)
{
    memset(k, 0, sizeof(kalman2_p_t));

    KALMAN_FLOAT T = k->T = 1.0f/sampleRate;

    // Process covariance
    k->Q[0][0] = T*T*T / 3 * q;
    k->Q[0][1] = k->Q[1][0] = T*T / 2 * q;
    k->Q[1][1] = T * q;

    // Measurement covariance
    k->R = pR;

    // Initial error covariance
    IDENTIFY_MATRIX_2X2(k->P);

    // Motion matrix
    IDENTIFY_MATRIX_2X2(k->A);
    k->A[0][0] = 1;
    k->A[0][1] = T;
    k->A[1][1] = 1;

    // Initial state
    k->x_hat[0] = k->x_hat[1] = 0;
    k->x_hat_est[0] = k->x_hat_est[1] = 0;

    // Measurement matrix, measured = H*state + noise
    k->H[0] = 1.0f;
    k->H[1] = 0;
}

void step_kalman2_p(kalman2_p_t *k, KALMAN_FLOAT pos)
{
    KALMAN_FLOAT obs = pos;
    KALMAN_FLOAT tmp2[2][2], tmp3[2][2];
    KALMAN_FLOAT tmp, tmpv[2], det;
    KALMAN_FLOAT I[2][2]={{1,0},{0,1}};

    // Make prediction
    MAT_DOT_VEC_2X2(k->x_hat_est, k->A, k->x_hat);  // result 2x1

    MATRIX_PRODUCT_2X2_2X2T(tmp2, k->P, k->A);
    MATRIX_PRODUCT_2X2(tmp3, k->A, tmp2);
    MATRIX_ADD_2X2(k->P_est, tmp3, k->Q);

    // Update estimate
    DOT_VEC_1X2_2X1(tmp, k->H, k->x_hat_est);  // result 1x1

    k->error_x = obs - tmp;

    MAT_DOT_VEC_2X2(tmpv, k->P, k->H);   // result 2x1
    DOT_VEC_1X2_2X1(tmp, k->H, tmpv);   // result 1
    k->error_P = tmp + k->R;

    MATRIX_PRODUCT_2X2_2X1T(tmpv, k->P_est, k->H); // result 2x1
    MAT_SOLVE_AX_EQ_B_1X2_1X1(k->K, tmpv, k->error_P); // result 2x1

    VEC_SCALE_2(tmpv, k->error_x, k->K);  // result 2x1
    VEC_SUM(k->x_hat, k->x_hat_est, tmpv);         // result 2x1

    MATRIX_PRODUCT_2X1_1X2(tmp2, k->K, k->H); // result 2x2
    MATRIX_SUB_2X2(tmp3, I, tmp2);
    MATRIX_PRODUCT_2X2(k->P, tmp3, k->P_est);
}

void run_kalman2_p(KALMAN_FLOAT sampleRate,
                    KALMAN_FLOAT q, KALMAN_FLOAT pR,
                    KALMAN_FLOAT *pos,
                    KALMAN_FLOAT *result, int N)
{
    kalman2_p_t k;
    init_kalman2_p(&k, sampleRate, q, pR);

    while (N-- > 0) {
        step_kalman2_p(&k, *(pos++));
        *(result++) = k.x_hat[0];
        *(result++) = k.x_hat[1];
    }
}
        uses: #!/usr/bin/env python

from pylab import *
from scipy.signal import lfilter, butter
import velocity

# Test some Levant post-processing filters

sr = 10000.0
t = arange(int(sr))/sr
f0 = 10
p = cos(t*2*pi*f0)*exp(-t*0.5)
v = lfilter([1,-1],1,p)*sr
noise = uniform(-0.001,0.001,len(t))

C = max(lfilter([1,-1],1,v)*sr)
n = 70

lv = velocity.levant(C=C,sr=sr,pos=p+noise)
v2 = velocity.avgfilt(lv, n=n)
v3 = velocity.maxmin(lv, n=n)
v4 = lfilter(*butter(2,0.02,'low'),x=lv)

clf()
subplot(2,1,1)
plot(t,v)
plot(t,lv)

subplot(2,1,2)
plot(t[4900:5100],v[4900:5100], label='true vel')
plot(t[4900:5100],lv[4900:5100], label='levant')
plot(t[4900:5100],v2[4900:5100], label='avgfilt')
plot(t[4900:5100],v3[4900:5100], label='maxmin')
plot(t[4900:5100],v4[4900:5100], label='bw')
legend()

show() # v1.161.0
        with: 
// From GLE
// http://developer.apple.com/library/mac/#samplecode/glut/Listings/gle_vvector_h.html

/* ========================================================== */
/* inverse of matrix 
 *
 * Compute inverse of matrix a, returning determinant m and 
 * inverse b
 */

#define INVERT_3X3(b,det,a)         \
    {                       \
    double tmp;                  \
    DETERMINANT_3X3 (det, a);            \
    tmp = 1.0 / (det);               \
    SCALE_ADJOINT_3X3 (b, tmp, a);       \
    }
          ruby-version: 
#ifndef __MAT_VEC_H__
#define __MAT_VEC_H__

#include "gle_vvector.h"

/* Supplement gle_vvector.h with some more matrix/vector routines we
 * need to implement the Kalman filter. */

#define MATRIX_PRODUCT_3X3_3X2T(c,a,b)                              \
{                                                                   \
    c[0][0] = a[0][0]*b[0][0]+a[0][1]*b[0][1]+a[0][2]*b[0][2];      \
    c[0][1] = a[0][0]*b[1][0]+a[0][1]*b[1][1]+a[0][2]*b[1][2];      \
                                                                    \
    c[1][0] = a[1][0]*b[0][0]+a[1][1]*b[0][1]+a[1][2]*b[0][2];      \
    c[1][1] = a[1][0]*b[1][0]+a[1][1]*b[1][1]+a[1][2]*b[1][2];      \
                                                                    \
    c[2][0] = a[2][0]*b[0][0]+a[2][1]*b[0][1]+a[2][2]*b[0][2];      \
    c[2][1] = a[2][0]*b[1][0]+a[2][1]*b[1][1]+a[2][2]*b[1][2];      \
}

#define MATRIX_PRODUCT_2X3_2X2(c,a,b)           \
{                                               \
    c[0][0] = a[0][0]*b[0][0]+a[0][1]*b[0][1];  \
    c[0][1] = a[0][0]*b[1][0]+a[0][1]*b[1][1];  \
                                                \
    c[1][0] = a[1][0]*b[0][0]+a[1][1]*b[0][1];  \
    c[1][1] = a[1][0]*b[1][0]+a[1][1]*b[1][1];  \
                                                \
    c[2][0] = a[2][0]*b[0][0]+a[2][1]*b[0][1];  \
    c[2][1] = a[2][0]*b[1][0]+a[2][1]*b[1][1];  \
}

#define MATRIX_PRODUCT_3X3_3X3T(c,a,b)                          \
{                                                               \
    c[0][0] = a[0][0]*b[0][0]+a[0][1]*b[0][1]+a[0][2]*b[0][2];  \
    c[0][1] = a[0][0]*b[1][0]+a[0][1]*b[1][1]+a[0][2]*b[1][2];  \
    c[0][2] = a[0][0]*b[2][0]+a[0][1]*b[2][1]+a[0][2]*b[2][2];  \
                                                                \
    c[1][0] = a[1][0]*b[0][0]+a[1][1]*b[0][1]+a[1][2]*b[0][2];  \
    c[1][1] = a[1][0]*b[1][0]+a[1][1]*b[1][1]+a[1][2]*b[1][2];  \
    c[1][2] = a[1][0]*b[2][0]+a[1][1]*b[2][1]+a[1][2]*b[2][2];  \
                                                                \
    c[2][0] = a[2][0]*b[0][0]+a[2][1]*b[0][1]+a[2][2]*b[0][2];  \
    c[2][1] = a[2][0]*b[1][0]+a[2][1]*b[1][1]+a[2][2]*b[1][2];  \
    c[2][2] = a[2][0]*b[2][0]+a[2][1]*b[2][1]+a[2][2]*b[2][2];  \
}

#define MATRIX_PRODUCT_3X2_2X3(c,a,b)                           \
{                                                               \
    c[0][0] = a[0][0]*b[0][0]+a[0][1]*b[1][0]+a[0][2]*b[2][0];  \
    c[0][1] = a[0][0]*b[0][1]+a[0][1]*b[1][1]+a[0][2]*b[2][1];  \
                                                                \
    c[1][0] = a[1][0]*b[0][0]+a[1][1]*b[1][0]+a[1][2]*b[2][0];  \
    c[1][1] = a[1][0]*b[0][1]+a[1][1]*b[1][1]+a[1][2]*b[2][1];  \
}

#define MATRIX_PRODUCT_2X3_3X2(c,a,b)           \
{                                               \
    c[0][0] = a[0][0]*b[0][0]+a[0][1]*b[1][0];  \
    c[0][1] = a[0][0]*b[0][1]+a[0][1]*b[1][1];  \
    c[0][2] = a[0][0]*b[0][2]+a[0][1]*b[1][2];  \
                                                \
    c[1][0] = a[1][0]*b[0][0]+a[1][1]*b[1][0];  \
    c[1][1] = a[1][0]*b[0][1]+a[1][1]*b[1][1];  \
    c[1][2] = a[1][0]*b[0][2]+a[1][1]*b[1][2];  \
                                                \
    c[2][0] = a[2][0]*b[0][0]+a[2][1]*b[1][0];  \
    c[2][1] = a[2][0]*b[0][1]+a[2][1]*b[1][1];  \
    c[2][2] = a[2][0]*b[0][2]+a[2][1]*b[1][2];  \
}

// A few needed matrix functions
#define MATRIX_ADD_3X3(c,a,b)                   \
{                                               \
    c[0][0] = a[0][0] + b[0][0];                \
    c[1][0] = a[1][0] + b[1][0];                \
    c[2][0] = a[2][0] + b[2][0];                \
    c[0][1] = a[0][1] + b[0][1];                \
    c[1][1] = a[1][1] + b[1][1];                \
    c[2][1] = a[2][1] + b[2][1];                \
    c[0][2] = a[0][2] + b[0][2];                \
    c[1][2] = a[1][2] + b[1][2];                \
    c[2][2] = a[2][2] + b[2][2];                \
}

#define MATRIX_SUB_3X3(c,a,b)                   \
{                                               \
    c[0][0] = a[0][0] - b[0][0];                \
    c[1][0] = a[1][0] - b[1][0];                \
    c[2][0] = a[2][0] - b[2][0];                \
    c[0][1] = a[0][1] - b[0][1];                \
    c[1][1] = a[1][1] - b[1][1];                \
    c[2][1] = a[2][1] - b[2][1];                \
    c[0][2] = a[0][2] - b[0][2];                \
    c[1][2] = a[1][2] - b[1][2];                \
    c[2][2] = a[2][2] - b[2][2];                \
}

#define MATRIX_ADD_2X2(c,a,b)                   \
{                                               \
    c[0][0] = a[0][0] + b[0][0];                \
    c[1][0] = a[1][0] + b[1][0];                \
    c[0][1] = a[0][1] + b[0][1];                \
    c[1][1] = a[1][1] + b[1][1];                \
}

#define MATRIX_SUB_2X2(c,a,b)                   \
{                                               \
    c[0][0] = a[0][0] - b[0][0];                \
    c[1][0] = a[1][0] - b[1][0];                \
    c[0][1] = a[0][1] - b[0][1];                \
    c[1][1] = a[1][1] - b[1][1];                \
}

#define MAT_DOT_VEC_3X2(p,m,v)                              \
{                                                           \
    p[0] = m[0][0]*v[0] + m[0][1]*v[1] + m[0][2]*v[2];      \
    p[1] = m[1][0]*v[0] + m[1][1]*v[1] + m[1][2]*v[2];      \
}

#define MAT_DOT_VEC_2X3_FULL(p,m,v)          \
{                                            \
    p[0] = m[0][0]*v[0] + m[0][1]*v[1];      \
    p[1] = m[1][0]*v[0] + m[1][1]*v[1];      \
    p[2] = m[2][0]*v[0] + m[2][1]*v[1];      \
}

/* Solve for X without inverting A */
#define MAT_SOLVE_AX_EQ_B_2X3_2X2(x,b,a)                 \
{                                                        \
    x[0][0] =  - (a[1][1]*b[0][0] - a[1][0]*b[0][1])     \
               / (a[0][1]*a[1][0] - a[0][0]*a[1][1]);    \
    x[0][1] =    (a[0][1]*b[0][0] - a[0][0]*b[0][1])     \
               / (a[0][1]*a[1][0] - a[0][0]*a[1][1]);    \
    x[1][0] =    (a[1][0]*b[1][1] - a[1][1]*b[1][0])     \
               / (a[0][1]*a[1][0] - a[0][0]*a[1][1]);    \
    x[1][1] =  - (a[0][0]*b[1][1] - a[0][1]*b[1][0])     \
               / (a[0][1]*a[1][0] - a[0][0]*a[1][1]);    \
    x[2][0] =    (a[1][0]*b[2][1] - a[1][1]*b[2][0])     \
               / (a[0][1]*a[1][0] - a[0][0]*a[1][1]);    \
    x[2][1] =  - (a[0][0]*b[2][1] - a[0][1]*b[2][0])     \
               / (a[0][1]*a[1][0] - a[0][0]*a[1][1]);    \
}

#define IDENTIFY_MATRIX_2X2(m)              \
{                                           \
    m[0][0] = 1.0;                          \
    m[0][1] = 0.0;                          \
                                            \
    m[1][0] = 0.0;                          \
    m[1][1] = 1.0;                          \
}

#define MATRIX_PRODUCT_2X2_2X2T(c,a,b)                          \
{                                                               \
    c[0][0] = a[0][0]*b[0][0]+a[0][1]*b[0][1];                  \
    c[0][1] = a[0][0]*b[1][0]+a[0][1]*b[1][1];                  \
                                                                \
    c[1][0] = a[1][0]*b[0][0]+a[1][1]*b[0][1];                  \
    c[1][1] = a[1][0]*b[1][0]+a[1][1]*b[1][1];                  \
}

#define MATRIX_PRODUCT_2X1_1X2(c,a,b)                           \
{                                                               \
    c[0][0] = a[0]*b[0];                                        \
    c[0][1] = a[0]*b[1];                                        \
                                                                \
    c[1][0] = a[1]*b[0];                                        \
    c[1][1] = a[1]*b[1];                                        \
}

#define DOT_VEC_1X2_2X1(p,m,v)                          \
{                                                       \
    p = m[0]*v[0] + m[1]*v[1];                          \
}

#define MATRIX_PRODUCT_2X2_2X1T(p,m,v)          \
{                                               \
    p[0] = m[0][0]*v[0] + m[0][1]*v[1];         \
    p[1] = m[1][0]*v[0] + m[1][1]*v[1];         \
}

#define MAT_SOLVE_AX_EQ_B_1X2_1X1(x,b,a)             \
{                                                    \
    x[0] = b[0] / a;                                 \
    x[1] = b[1] / a;                                 \
}
#endif
          bundler-cache: 
Base input function:

(0.5+sin(time*50)*pow(2,-time*10)) + (0.2+sin(time*500)*0.2*pow(2,-time*10))

two sinusoids without noise
---------------------------

Lipschitz's constant C = 145.763207, sr = 1000.000000
Lipschitz's constant C = 148.904760, sr = 2000.000000
Lipschitz's constant C = 149.712539, sr = 4000.000000
Lipschitz's constant C = 149.866406, sr = 6000.000000
Lipschitz's constant C = 149.921593, sr = 8000.000000
Lipschitz's constant C = 149.947736, sr = 10000.000000
Lipschitz's constant C = 149.984333, sr = 20000.000000
Lipschitz's constant C = 149.994784, sr = 40000.000000
Lipschitz's constant C = 149.997104, sr = 60000.000000
Lipschitz's constant C = 149.998046, sr = 80000.000000
Lipschitz's constant C = 149.998542, sr = 100000.000000

{'bf-foaw16': [(1000.0, 1.9207730189275878e-12),
               (2000.0, 5.6443238626961199e-12),
               (4000.0, 1.5965134058402137e-11),
               (6000.0, 2.8905460474440368e-11),
               (8000.0, 4.489493575873919e-11),
               (10000.0, 6.2720531152837794e-11),
               (20000.0, 1.7588268202354665e-10),
               (40000.0, 4.9596996550731643e-10),
               (60000.0, 9.1780430752640958e-10),
               (80000.0, 1.408815458006852e-09),
               (100000.0, 1.9608065955558164e-09)],
 'bw2-300': [(1000.0, 0.79332339390660411),
             (2000.0, 1.5021697898938591),
             (4000.0, 2.2494696595646357),
             (6000.0, 2.7829274377161282),
             (8000.0, 3.2246838216407525),
             (10000.0, 3.6111124610532763),
             (20000.0, 5.1178198454105637),
             (40000.0, 7.2415513942063408),
             (60000.0, 8.8699276057777912),
             (80000.0, 10.242463309991376),
             (100000.0, 11.451604665418632)],
 'ef-foaw16': [(1000.0, 1.1389897924569677e-12),
               (2000.0, 3.2827864088082109e-12),
               (4000.0, 9.1832850290679495e-12),
               (6000.0, 1.6489333502645068e-11),
               (8000.0, 2.6023766400674431e-11),
               (10000.0, 3.4993289239452462e-11),
               (20000.0, 9.930479863801936e-11),
               (40000.0, 2.8047100544612683e-10),
               (60000.0, 5.2878395171146225e-10),
               (80000.0, 7.9091523321613844e-10),
               (100000.0, 1.1251487170677769e-09)],
 'fd': [(1000.0, 1.1389897924569677e-12),
        (2000.0, 3.2827864088082109e-12),
        (4000.0, 9.1832850290679495e-12),
        (6000.0, 1.6489333502645068e-11),
        (8000.0, 2.6023766400674431e-11),
        (10000.0, 3.4993289239452462e-11),
        (20000.0, 9.930479863801936e-11),
        (40000.0, 2.8047100544612683e-10),
        (60000.0, 5.2878395171146225e-10),
        (80000.0, 7.9091523321613844e-10),
        (100000.0, 1.1251487170677769e-09)],
 'levant1': [(1000.0, 5.3532592651329427),
             (2000.0, 3.4638473244463777),
             (4000.0, 2.4592524162291935),
             (6000.0, 2.0033481269187261),
             (8000.0, 1.7363317098065523),
             (10000.0, 1.5516348021776343),
             (20000.0, 1.0941113690141193),
             (40000.0, 0.77454963351780315),
             (60000.0, 0.63165764225299936),
             (80000.0, 0.54611989018664497),
             (100000.0, 0.48727946950881151)],
 'levant2': [(1000.0, 1.968443920140855),
             (2000.0, 1.3137290533619028),
             (4000.0, 0.95495512873039146),
             (6000.0, 0.766278023647327),
             (8000.0, 0.65782425876807615),
             (10000.0, 0.58901021267418696),
             (20000.0, 0.42002102427217936),
             (40000.0, 0.29738857752825248),
             (60000.0, 0.24355528509376637),
             (80000.0, 0.21182218814106818),
             (100000.0, 0.18921460659779996)],
 'levant4': [(1000.0, 1.0438762245654802),
             (2000.0, 0.73842079082421674),
             (4000.0, 0.52030223689016253),
             (6000.0, 0.42525388034023276),
             (8000.0, 0.37333941931600689),
             (10000.0, 0.33390477014062953),
             (20000.0, 0.24264954057598351),
             (40000.0, 0.18093187364480848),
             (60000.0, 0.15679057494767756),
             (80000.0, 0.13892626625252816),
             (100000.0, 0.1265743918363702)]}

two sinusoids with noise = 1e-05
--------------------------------

Lipschitz's constant C = 145.763207, sr = 1000.000000
Lipschitz's constant C = 148.904760, sr = 2000.000000
Lipschitz's constant C = 149.712539, sr = 4000.000000
Lipschitz's constant C = 149.866406, sr = 6000.000000
Lipschitz's constant C = 149.921593, sr = 8000.000000
Lipschitz's constant C = 149.947736, sr = 10000.000000
Lipschitz's constant C = 149.984333, sr = 20000.000000
Lipschitz's constant C = 149.994784, sr = 40000.000000
Lipschitz's constant C = 149.997104, sr = 60000.000000
Lipschitz's constant C = 149.998046, sr = 80000.000000
Lipschitz's constant C = 149.998542, sr = 100000.000000

{'bf-foaw16': [(1000.0, 0.13855257236123772),
               (2000.0, 0.50971375750428605),
               (4000.0, 1.013852894839435),
               (6000.0, 1.3691282497314048),
               (8000.0, 1.6379538118948036),
               (10000.0, 1.8556651573587188),
               (20000.0, 2.1682133221665159),
               (40000.0, 2.003942264641692),
               (60000.0, 2.4804349453661487),
               (80000.0, 3.4557653991494344),
               (100000.0, 4.67219356411952)],
 'bw2-300': [(1000.0, 0.7960075475619387),
             (2000.0, 1.502098882259139),
             (4000.0, 2.2500782216893342),
             (6000.0, 2.7847872926235451),
             (8000.0, 3.2265700257378853),
             (10000.0, 3.6122016628302847),
             (20000.0, 5.1188603839208273),
             (40000.0, 7.2401813041442979),
             (60000.0, 8.8689674513671015),
             (80000.0, 10.24282574305078),
             (100000.0, 11.450569092062379)],
 'ef-foaw16': [(1000.0, 0.13768592153845211),
               (2000.0, 0.50567528364252134),
               (4000.0, 0.97585695893319324),
               (6000.0, 1.300483231044592),
               (8000.0, 1.5292500017876693),
               (10000.0, 1.7234816110330808),
               (20000.0, 2.1774239772012107),
               (40000.0, 2.5645406305972469),
               (60000.0, 3.8157720340201129),
               (80000.0, 5.6651431526736484),
               (100000.0, 7.7707535235760181)],
 'fd': [(1000.0, 0.11729072837652837),
        (2000.0, 0.32745950994338141),
        (4000.0, 0.93342133743294708),
        (6000.0, 1.6771318808815932),
        (8000.0, 2.6248450189963104),
        (10000.0, 3.705283445459878),
        (20000.0, 10.28026224836429),
        (40000.0, 29.035153497217483),
        (60000.0, 53.518518752728532),
        (80000.0, 82.703044805998857),
        (100000.0, 115.60023565572079)],
 'levant1': [(1000.0, 5.411004739223249),
             (2000.0, 3.520851413595655),
             (4000.0, 2.5305453519803272),
             (6000.0, 2.2483883688816833),
             (8000.0, 2.1094857168152936),
             (10000.0, 2.1006890156896834),
             (20000.0, 2.4972615082516416),
             (40000.0, 3.3616615342630505),
             (60000.0, 4.0618973184099936),
             (80000.0, 4.6933330826044468),
             (100000.0, 5.2174884406564583)],
 'levant2': [(1000.0, 1.9433116172287856),
             (2000.0, 1.3515021690257414),
             (4000.0, 1.3407159633450316),
             (6000.0, 1.448395954879192),
             (8000.0, 1.5815601495578564),
             (10000.0, 1.7420158823223992),
             (20000.0, 2.3718256437188106),
             (40000.0, 3.3329558112167206),
             (60000.0, 4.0644241709434299),
             (80000.0, 4.7100975758216475),
             (100000.0, 5.247157303637529)],
 'levant4': [(1000.0, 1.035978619326354),
             (2000.0, 0.79754922485040258),
             (4000.0, 0.82341828099835646),
             (6000.0, 0.94905865493716035),
             (8000.0, 1.1337356367424236),
             (10000.0, 1.3137038533704255),
             (20000.0, 1.929754165390247),
             (40000.0, 2.8108748475381469),
             (60000.0, 3.5231308813493483),
             (80000.0, 4.1878639628794172),
             (100000.0, 4.7665472119730232)]}

two sinusoids with noise = 1e-06
--------------------------------

Lipschitz's constant C = 145.763207, sr = 1000.000000
Lipschitz's constant C = 148.904760, sr = 2000.000000
Lipschitz's constant C = 149.712539, sr = 4000.000000
Lipschitz's constant C = 149.866406, sr = 6000.000000
Lipschitz's constant C = 149.921593, sr = 8000.000000
Lipschitz's constant C = 149.947736, sr = 10000.000000
Lipschitz's constant C = 149.984333, sr = 20000.000000
Lipschitz's constant C = 149.994784, sr = 40000.000000
Lipschitz's constant C = 149.997104, sr = 60000.000000
Lipschitz's constant C = 149.998046, sr = 80000.000000
Lipschitz's constant C = 149.998542, sr = 100000.000000

{'bf-foaw16': [(1000.0, 0.011545579466175985),
               (2000.0, 0.03672197067064354),
               (4000.0, 0.13138204103046666),
               (6000.0, 0.27129745098892005),
               (8000.0, 0.3828308975853742),
               (10000.0, 0.47628464141710569),
               (20000.0, 0.8055805510867462),
               (40000.0, 1.1570882652553551),
               (60000.0, 1.2298154009753934),
               (80000.0, 1.1667668592734752),
               (100000.0, 1.1212472297014864)],
 'bw2-300': [(1000.0, 0.79343850174972952),
             (2000.0, 1.5022578475034818),
             (4000.0, 2.2494953098898214),
             (6000.0, 2.7831330645394505),
             (8000.0, 3.2247793264836258),
             (10000.0, 3.6110752782224704),
             (20000.0, 5.1179812882110349),
             (40000.0, 7.2412971595792426),
             (60000.0, 8.8700889230402016),
             (80000.0, 10.24247770445958),
             (100000.0, 11.451707272646665)],
 'ef-foaw16': [(1000.0, 0.011545579466085028),
               (2000.0, 0.036701170434162281),
               (4000.0, 0.1310289648921302),
               (6000.0, 0.26865598395559981),
               (8000.0, 0.37411964010370752),
               (10000.0, 0.46241784723629659),
               (20000.0, 0.76113479743787249),
               (40000.0, 1.078514345325345),
               (60000.0, 1.216403014112762),
               (80000.0, 1.2485908062478022),
               (100000.0, 1.291175916669393)],
 'fd': [(1000.0, 0.011324718743383148),
        (2000.0, 0.033712070085432637),
        (4000.0, 0.093445883376251412),
        (6000.0, 0.16970134733881415),
        (8000.0, 0.25941680051966615),
        (10000.0, 0.37178812298549208),
        (20000.0, 1.0353159608412408),
        (40000.0, 2.9063896240748197),
        (60000.0, 5.3839631760471374),
        (80000.0, 8.2201188877354419),
        (100000.0, 11.531686765066304)],
 'levant1': [(1000.0, 5.2971055719680455),
             (2000.0, 3.4443076445047542),
             (4000.0, 2.4783909820847945),
             (6000.0, 2.007593828166788),
             (8000.0, 1.7436392158950054),
             (10000.0, 1.5719589845059092),
             (20000.0, 1.2355614216764423),
             (40000.0, 1.218306870089966),
             (60000.0, 1.3789284184242319),
             (80000.0, 1.5309690001870924),
             (100000.0, 1.6974945286246166)],
 'levant2': [(1000.0, 1.9801957259347345),
             (2000.0, 1.3095509740435716),
             (4000.0, 0.96750629002218147),
             (6000.0, 0.79678376103013437),
             (8000.0, 0.73014812069927271),
             (10000.0, 0.72568635263000625),
             (20000.0, 0.82096878578849952),
             (40000.0, 1.0808137742823691),
             (60000.0, 1.302383517057998),
             (80000.0, 1.4811381051776098),
             (100000.0, 1.6599784997541625)],
 'levant4': [(1000.0, 1.0498818496195561),
             (2000.0, 0.743380477331121),
             (4000.0, 0.52849056107953352),
             (6000.0, 0.45653855444682484),
             (8000.0, 0.43205677714216961),
             (10000.0, 0.42542607496609303),
             (20000.0, 0.49314022195814461),
             (40000.0, 0.74294817596873841),
             (60000.0, 0.95388070456565188),
             (80000.0, 1.1148948589991587),
             (100000.0, 1.2611921125600813)]}

two sinusoids + noise=1e-05 + discontinuity
-------------------------------------------

Lipschitz's constant C = 434.526048, sr = 1000.000000
Lipschitz's constant C = 894.832618, sr = 2000.000000
Lipschitz's constant C = 1828.744202, sr = 4000.000000
Lipschitz's constant C = 2765.605701, sr = 6000.000000
Lipschitz's constant C = 3703.199063, sr = 8000.000000
Lipschitz's constant C = 4641.083906, sr = 10000.000000
Lipschitz's constant C = 9331.814732, sr = 20000.000000
Lipschitz's constant C = 18714.578277, sr = 40000.000000
Lipschitz's constant C = 28097.630203, sr = 60000.000000
Lipschitz's constant C = 37480.754116, sr = 80000.000000
Lipschitz's constant C = 46863.906805, sr = 100000.000000

{'bf-foaw16': [(1000.0, 0.087608835040594177),
               (2000.0, 0.31285526779975742),
               (4000.0, 0.64920509695757844),
               (6000.0, 0.88059056241488121),
               (8000.0, 1.0864294088982271),
               (10000.0, 1.2289086742977509),
               (20000.0, 1.5352236095092575),
               (40000.0, 1.6596141058655516),
               (60000.0, 2.3536438006938321),
               (80000.0, 3.4893588766525907),
               (100000.0, 4.6133741156512009)],
 'bw2-300': [(1000.0, 0.62238390417007794),
             (2000.0, 1.1581542999470409),
             (4000.0, 1.7227404463434066),
             (6000.0, 2.1308822728487629),
             (8000.0, 2.4706498808646824),
             (10000.0, 2.7649710751324283),
             (20000.0, 3.9172433428562679),
             (40000.0, 5.5400921164150798),
             (60000.0, 6.789896986134635),
             (80000.0, 7.8382841982471163),
             (100000.0, 8.7650944672929896)],
 'ef-foaw16': [(1000.0, 0.087239687419779388),
               (2000.0, 0.3127246586481211),
               (4000.0, 0.6329712566450405),
               (6000.0, 0.83970170385155918),
               (8000.0, 1.0278814405407846),
               (10000.0, 1.1506667957435572),
               (20000.0, 1.5784277847541053),
               (40000.0, 2.2962565520198299),
               (60000.0, 3.7153553938887351),
               (80000.0, 5.5871092869975634),
               (100000.0, 7.7662321846501401)],
 'fd': [(1000.0, 0.11314115445392407),
        (2000.0, 0.33063866581056545),
        (4000.0, 0.93623483038612831),
        (6000.0, 1.7198806664825328),
        (8000.0, 2.5826954941320985),
        (10000.0, 3.696189797549863),
        (20000.0, 10.318843830534835),
        (40000.0, 29.238753489914583),
        (60000.0, 53.814001509554309),
        (80000.0, 82.435446820759921),
        (100000.0, 115.98592122229002)],
 'levant1': [(1000.0, 15.302313835528899),
             (2000.0, 22.296803756032613),
             (4000.0, 32.14513592818367),
             (6000.0, 39.674855990419317),
             (8000.0, 45.954376325392595),
             (10000.0, 51.548574661193165),
             (20000.0, 73.402401472560101),
             (40000.0, 103.74154388532226),
             (60000.0, 127.00198255285412),
             (80000.0, 146.27344084601418),
             (100000.0, 166.52502407883696)],
 'levant2': [(1000.0, 8.0630495993103946),
             (2000.0, 11.653984695300373),
             (4000.0, 16.69277981027783),
             (6000.0, 20.624316492852895),
             (8000.0, 23.986599515023883),
             (10000.0, 26.669329522763725),
             (20000.0, 37.811880679212976),
             (40000.0, 58.021703441294825),
             (60000.0, 79.603140229692912),
             (80000.0, 100.43914490242508),
             (100000.0, 122.21267186512723)],
 'levant4': [(1000.0, 1.9393560720374421),
             (2000.0, 2.9898079663750057),
             (4000.0, 4.4207508739877799),
             (6000.0, 5.8779247434489399),
             (8000.0, 7.2919963382792821),
             (10000.0, 8.7930862262491978),
             (20000.0, 16.277240280343769),
             (40000.0, 37.212993540905117),
             (60000.0, 59.053187336713485),
             (80000.0, 80.380202903355652),
             (100000.0, 101.89518863513646)]}
          cache-version: #!/usr/bin/env python

from pylab import *
from scipy.signal import *
from velocity import *
from collections import defaultdict
from pprint import pprint

def do_rms_for_methods(sr, vel, noise_max):
    # Constants
    T = 1/sr;
    r = int(sr/100);

    # Integrate it to get position
    pos = lfilter([1], [1,-1], vel)*T

    # Add some noise
    pos = pos + rand(len(pos))*noise_max

    # Finite difference
    fdvel = lfilter([1,-1],[1],pos)/T

    # Butterworth 300 Hz
    [B,A] = butter(2, 300/(sr/2))
    bwvel = lfilter(B,A,fdvel)

    # FD skip 3
    dist = 3
    fd3vel = lfilter(array([1]+[0]*(dist-1)+[-1])/float(dist),[1],pos)/T

    lsvel = lfilter(leastsquared(15), 1, pos)/T

    levantC = C=max(abs(vel[1:]-vel[:-1]))/T
    print 'Lipschitz\'s constant C = %f, sr = %f'%(levantC,sr)
    levantvel1 = levant(pos, sr, C=levantC, rk=1)
    levantvel2 = levant(pos, sr, C=levantC, rk=2)
    levantvel4 = levant(pos, sr, C=levantC, rk=4)

    endfitfoawvel = foaw(pos, sr, noise_max, n=16, best=False)
    bestfitfoawvel = foaw(pos, sr, noise_max, n=16, best=True)
    mpos = median_filter(pos, n=3)
    endfitfoawvelm = foaw(mpos, sr, noise_max, n=16, best=False)
    bestfitfoawvelm = foaw(mpos, sr, noise_max, n=16, best=True)

    def rms(x):
        return sqrt(sum((x[r:] - vel[r:])*(x[r:] - vel[r:])))

    r = len(levantvel1)/5
    return {'bf-foaw16': (sr, rms(bestfitfoawvel)),
            'ef-foaw16': (sr, rms(endfitfoawvel)),
            'bw2-300': (sr, rms(bwvel)),
            'levant1': (sr, rms(levantvel1)),
            'levant2': (sr, rms(levantvel2)),
            'levant4': (sr, rms(levantvel4)),
            'fd': (sr, rms(fdvel))}

def do_plot(results, name):
    marks = 'o^s+D'*10
    for i, algo in enumerate(sort(results.keys())):
        loglog(*zip(*results[algo]), marker=marks[i], label=algo)
        xlim(1000, 100000)
        xlabel('sampling rate (Hz)')
        ylabel('RMS error')
        legend()
        title(name)

    print
    print name
    print '-'*len(name)
    print
    pprint(dict(results))
    savefig(name.replace(' ','_')+'.png')

if __name__=="__main__":
    rc('legend',fontsize=8)

    rates = [1000, 2000, 4000, 6000, 8000,
             10000, 20000, 40000, 60000, 80000, 100000]

    # Two sinusoids without noise
    def genvel(sr,t):
        time = arange(sr*t)/float(sr);
        return time, (((0.5+sin(time*50)*pow(2,-time*10))
                       + (0.2+sin(time*500)*0.2*pow(2,-time*10))))
    figure(1)
    clf()
    results = defaultdict(lambda: [])
    for sr in rates:
        time, vel = genvel(sr,1)
        res = do_rms_for_methods(float(sr), vel, noise_max=0)
        for r in res:
            results[r].append(res[r])
    do_plot(results, 'two sinusoids without noise')

    # Two sinusoids with noise
    def genvel(sr,t):
        time = arange(sr*t)/float(sr);
        return time, ((0.5+sin(time*50)*pow(2,-time*10))
                      + (0.2+sin(time*500)*0.2*pow(2,-time*10)))
    figure(2)
    clf()
    results = defaultdict(lambda: [])
    for sr in rates:
        time, vel = genvel(sr,1)
        res = do_rms_for_methods(float(sr), vel, noise_max=1e-05)
        for r in res:
            results[r].append(res[r])
    do_plot(results, 'two sinusoids with noise = 1e-05')

    # Two sinusoids with low noise
    def genvel(sr,t):
        time = arange(sr*t)/float(sr);
        return time, ((0.5+sin(time*50)*pow(2,-time*10))
                      + (0.2+sin(time*500)*0.2*pow(2,-time*10)))
    figure(3)
    clf()
    results = defaultdict(lambda: [])
    for sr in rates:
        time, vel = genvel(sr,1)
        res = do_rms_for_methods(float(sr), vel, noise_max=1e-06)
        for r in res:
            results[r].append(res[r])
    do_plot(results, 'two sinusoids with noise = 1e-06')

    # Two sinusoids + noise + discontinuity
    def genvel(sr,t):
        time = arange(sr*t)/float(sr);
        return time, (((0.5+sin(time*50)*pow(2,-time*10))
                       + (0.2+sin(time*500)*0.2*pow(2,-time*10)))
                      *concatenate((ones(len(time)/2),
                                    zeros(len(time)/2))))
    figure(4)
    clf()
    results = defaultdict(lambda: [])
    for sr in rates:
        time, vel = genvel(sr,1)
        res = do_rms_for_methods(float(sr), vel, noise_max=1e-05)
        for r in res:
            results[r].append(res[r])
    do_plot(results, 'two sinusoids + noise=1e-05 + discontinuity')

    show()
      - name: 
Base input function:

(0.5+sin(time*50)*pow(2,-time)) + (0.2+sin(time*500)*0.2*pow(2,-time))

two sinusoids without noise
---------------------------

Lipschitz's constant C = 144.856719, sr = 1000.000000
Lipschitz's constant C = 148.441026, sr = 2000.000000
Lipschitz's constant C = 149.479232, sr = 4000.000000
Lipschitz's constant C = 149.710668, sr = 6000.000000
Lipschitz's constant C = 149.804731, sr = 8000.000000
Lipschitz's constant C = 149.854223, sr = 10000.000000
Lipschitz's constant C = 149.937558, sr = 20000.000000
Lipschitz's constant C = 149.971393, sr = 40000.000000
Lipschitz's constant C = 149.981509, sr = 60000.000000
Lipschitz's constant C = 149.986350, sr = 80000.000000

{'bf-foaw16': [(1000.0, 1.9702258422138339e-12),
               (2000.0, 5.4388946385508004e-12),
               (4000.0, 1.5810413113850575e-11),
               (6000.0, 2.8573031344375584e-11),
               (8000.0, 4.5002539894573038e-11),
               (10000.0, 6.0522620757366041e-11),
               (20000.0, 1.7226568764137239e-10),
               (40000.0, 4.8905545953918461e-10),
               (60000.0, 9.0734466560047842e-10),
               (80000.0, 1.3873479008767754e-09),
               (100000.0, 1.9496105895388441e-09)],
 'bw2-300': [(1000.0, 0.08699625570268453),
             (2000.0, 0.16466086240678132),
             (4000.0, 0.24653010955768453),
             (6000.0, 0.30497526946194514),
             (8000.0, 0.35337534887832667),
             (10000.0, 0.39571444629082186),
             (20000.0, 0.56080169987501594),
             (40000.0, 0.79350120003699465),
             (60000.0, 0.97192610624643105),
             (80000.0, 1.1223186700699681),
             (100000.0, 1.2548079906685359)],
 'ef-foaw16': [(1000.0, 1.0926673691465038e-12),
               (2000.0, 3.0238192836052313e-12),
               (4000.0, 9.0090030857581497e-12),
               (6000.0, 1.5924843934242909e-11),
               (8000.0, 2.5644053840834361e-11),
               (10000.0, 3.5305716485773622e-11),
               (20000.0, 9.6803757284446716e-11),
               (40000.0, 2.7690003020104327e-10),
               (60000.0, 5.2453462424048428e-10),
               (80000.0, 7.8540103224450675e-10),
               (100000.0, 1.1166844459830958e-09)],
 'fd': [(1000.0, 1.0926673691465038e-12),
        (2000.0, 3.0238192836052313e-12),
        (4000.0, 9.0090030857581497e-12),
        (6000.0, 1.5924843934242909e-11),
        (8000.0, 2.5644053840834361e-11),
        (10000.0, 3.5305716485773622e-11),
        (20000.0, 9.6803757284446716e-11),
        (40000.0, 2.7690003020104327e-10),
        (60000.0, 5.2453462424048428e-10),
        (80000.0, 7.8540103224450675e-10),
        (100000.0, 1.1166844459830958e-09)],
 'levant1': [(1000.0, 4.4593133224054338),
             (2000.0, 3.2733377486767501),
             (4000.0, 2.5452303873700188),
             (6000.0, 1.9386258964698317),
             (8000.0, 1.7935798947114505),
             (10000.0, 1.5266958665073649),
             (20000.0, 1.1049712447051212),
             (40000.0, 0.77660777980101536),
             (60000.0, 0.63513419913779345),
             (80000.0, 0.55156800355793945),
             (100000.0, 0.49184059321794016)],
 'levant2': [(1000.0, 1.7446010351249253),
             (2000.0, 1.2763812265699315),
             (4000.0, 0.61472158450712),
             (6000.0, 0.71121420123325396),
             (8000.0, 0.45873509147327107),
             (10000.0, 0.52561730380245575),
             (20000.0, 0.33673810122216169),
             (40000.0, 0.24762270081035695),
             (60000.0, 0.20091788296875421),
             (80000.0, 0.17323067802498957),
             (100000.0, 0.15704465814833368)],
 'levant4': [(1000.0, 0.35904834852628609),
             (2000.0, 0.25353155504378749),
             (4000.0, 0.16557717459393706),
             (6000.0, 0.13902605893007317),
             (8000.0, 0.12201691031816589),
             (10000.0, 0.11249121358111883),
             (20000.0, 0.083411647540811137),
             (40000.0, 0.061665365782732857),
             (60000.0, 0.051547332008355894),
             (80000.0, 0.045471260408054041),
             (100000.0, 0.04113820117539136)]}

two sinusoids with noise = 1e-05
--------------------------------

Lipschitz's constant C = 144.856719, sr = 1000.000000
Lipschitz's constant C = 148.441026, sr = 2000.000000
Lipschitz's constant C = 149.479232, sr = 4000.000000
Lipschitz's constant C = 149.710668, sr = 6000.000000
Lipschitz's constant C = 149.804731, sr = 8000.000000
Lipschitz's constant C = 149.854223, sr = 10000.000000
Lipschitz's constant C = 149.937558, sr = 20000.000000
Lipschitz's constant C = 149.971393, sr = 40000.000000
Lipschitz's constant C = 149.981509, sr = 60000.000000
Lipschitz's constant C = 149.986350, sr = 80000.000000
Lipschitz's constant C = 149.989184, sr = 100000.000000

{'bf-foaw16': [(1000.0, 0.098129374812280248),
               (2000.0, 0.18615221695678102),
               (4000.0, 0.2769439545252419),
               (6000.0, 0.33014529895064226),
               (8000.0, 0.354022205682264),
               (10000.0, 0.35586247000067661),
               (20000.0, 0.47608550352789858),
               (40000.0, 1.1741667299467915),
               (60000.0, 2.1294567600766827),
               (80000.0, 3.2907903569908954),
               (100000.0, 4.5243321983812157)],
 'bw2-300': [(1000.0, 0.10951451160298188),
             (2000.0, 0.18499201290702597),
             (4000.0, 0.26558738254286485),
             (6000.0, 0.32226786798979412),
             (8000.0, 0.36892733105550779),
             (10000.0, 0.4101171391872101),
             (20000.0, 0.5732158997651543),
             (40000.0, 0.80234520600804327),
             (60000.0, 0.97929648817640746),
             (80000.0, 1.1289837922562076),
             (100000.0, 1.2582742528881179)],
 'ef-foaw16': [(1000.0, 0.097320582319363289),
               (2000.0, 0.17770401536625435),
               (4000.0, 0.26166303094132048),
               (6000.0, 0.32114078473284025),
               (8000.0, 0.36484393099657325),
               (10000.0, 0.40346097531473424),
               (20000.0, 0.73214605742912542),
               (40000.0, 1.9586650502685428),
               (60000.0, 3.5766259167358951),
               (80000.0, 5.5147315782279982),
               (100000.0, 7.6982081557649558)],
 'fd': [(1000.0, 0.11414622039540996),
        (2000.0, 0.32101803791882066),
        (4000.0, 0.94651817441007369),
        (6000.0, 1.7007774215117006),
        (8000.0, 2.6320758314035633),
        (10000.0, 3.6423032206713843),
        (20000.0, 10.339817377615743),
        (40000.0, 29.249989530056023),
        (60000.0, 54.050870333929083),
        (80000.0, 83.066155250004698),
        (100000.0, 115.14394053071636)],
 'levant1': [(1000.0, 4.4558933677529549),
             (2000.0, 3.2662551321977014),
             (4000.0, 2.5899071089729477),
             (6000.0, 2.2146049298891022),
             (8000.0, 2.1397198081463356),
             (10000.0, 2.1756689233164512),
             (20000.0, 2.5477654444228861),
             (40000.0, 3.5016987822522774),
             (60000.0, 4.2540608197622589),
             (80000.0, 4.9092544182949283),
             (100000.0, 5.4824359102207412)],
 'levant2': [(1000.0, 1.7448028474832742),
             (2000.0, 1.332839078469116),
             (4000.0, 1.3304804939439723),
             (6000.0, 1.4843850205313258),
             (8000.0, 1.624497092722043),
             (10000.0, 1.7923710311501233),
             (20000.0, 2.4145510369669005),
             (40000.0, 3.4365673837607607),
             (60000.0, 4.2080916362616625),
             (80000.0, 4.8946875124645199),
             (100000.0, 5.4992144981402884)],
 'levant4': [(1000.0, 0.37821640751112279),
             (2000.0, 0.32451050709978618),
             (4000.0, 0.51201758902785999),
             (6000.0, 0.83793537347883507),
             (8000.0, 1.1408313162123147),
             (10000.0, 1.3435449722480426),
             (20000.0, 1.9571539982845045),
             (40000.0, 2.8072239297956987),
             (60000.0, 3.5229543670302168),
             (80000.0, 4.2706243745531491),
             (100000.0, 4.9719071530276917)]}

two sinusoids with noise = 1e-06
--------------------------------

Lipschitz's constant C = 144.856719, sr = 1000.000000
Lipschitz's constant C = 148.441026, sr = 2000.000000
Lipschitz's constant C = 149.479232, sr = 4000.000000
Lipschitz's constant C = 149.710668, sr = 6000.000000
Lipschitz's constant C = 149.804731, sr = 8000.000000
Lipschitz's constant C = 149.854223, sr = 10000.000000
Lipschitz's constant C = 149.937558, sr = 20000.000000
Lipschitz's constant C = 149.971393, sr = 40000.000000
Lipschitz's constant C = 149.981509, sr = 60000.000000
Lipschitz's constant C = 149.986350, sr = 80000.000000
Lipschitz's constant C = 149.989184, sr = 100000.000000

{'bf-foaw16': [(1000.0, 0.01286731049973272),
               (2000.0, 0.034877156552935551),
               (4000.0, 0.075509920173455758),
               (6000.0, 0.10525618989262187),
               (8000.0, 0.1265672310735626),
               (10000.0, 0.14547023748182208),
               (20000.0, 0.1937371910142581),
               (40000.0, 0.20982788669874447),
               (60000.0, 0.25845986281309119),
               (80000.0, 0.34983198803138971),
               (100000.0, 0.46997769769812225)],
 'bw2-300': [(1000.0, 0.087393690955066189),
             (2000.0, 0.16491046611037183),
             (4000.0, 0.24660900371776698),
             (6000.0, 0.3052608522165941),
             (8000.0, 0.35347086986273346),
             (10000.0, 0.39601073857619457),
             (20000.0, 0.56092453443760704),
             (40000.0, 0.79376033543372393),
             (60000.0, 0.97212965395536588),
             (80000.0, 1.1224564539474606),
             (100000.0, 1.2550584591068938)],
 'ef-foaw16': [(1000.0, 0.012813343775780335),
               (2000.0, 0.034403946127347436),
               (4000.0, 0.073043117401682331),
               (6000.0, 0.10048448608310805),
               (8000.0, 0.12073512399289596),
               (10000.0, 0.13774523129973815),
               (20000.0, 0.19141965736622379),
               (40000.0, 0.26226899590269426),
               (60000.0, 0.38836369474251742),
               (80000.0, 0.56401893539009651),
               (100000.0, 0.77913491892609055)],
 'fd': [(1000.0, 0.011637635220858521),
        (2000.0, 0.032255032041536867),
        (4000.0, 0.091573689274900991),
        (6000.0, 0.16956643691616113),
        (8000.0, 0.25857898706624793),
        (10000.0, 0.36600189974386471),
        (20000.0, 1.026396014444767),
        (40000.0, 2.912739335930882),
        (60000.0, 5.373286087713355),
        (80000.0, 8.2625349341189374),
        (100000.0, 11.51500183411077)],
 'levant1': [(1000.0, 4.4656439822078511),
             (2000.0, 3.2656918251259937),
             (4000.0, 2.5457578035739914),
             (6000.0, 1.9411475545774499),
             (8000.0, 1.7953424097392094),
             (10000.0, 1.531141507630996),
             (20000.0, 1.2378138617056567),
             (40000.0, 1.2495172925215108),
             (60000.0, 1.4198017480461775),
             (80000.0, 1.5963295413025238),
             (100000.0, 1.7647629274405028)],
 'levant2': [(1000.0, 1.7422750467219059),
             (2000.0, 1.2780475956857205),
             (4000.0, 0.6400571636997977),
             (6000.0, 0.76199393036245289),
             (8000.0, 0.73611813491074607),
             (10000.0, 0.69993708222154216),
             (20000.0, 0.83821636436239655),
             (40000.0, 1.1092542944227899),
             (60000.0, 1.3486302578898148),
             (80000.0, 1.5445803554865845),
             (100000.0, 1.7267243867868454)],
 'levant4': [(1000.0, 0.35911498183422702),
             (2000.0, 0.25427198705674736),
             (4000.0, 0.17994982716890978),
             (6000.0, 0.16321155598735981),
             (8000.0, 0.16716058166411851),
             (10000.0, 0.18357161605339498),
             (20000.0, 0.29758938867258056),
             (40000.0, 0.76672890343354305),
             (60000.0, 1.0171958506413459),
             (80000.0, 1.1843343337293666),
             (100000.0, 1.3286525085348029)]}

two sinusoids + noise=1e-05 + discontinuity
-------------------------------------------

Lipschitz's constant C = 688.194179, sr = 1000.000000
Lipschitz's constant C = 1377.604799, sr = 2000.000000
Lipschitz's constant C = 2757.009157, sr = 4000.000000
Lipschitz's constant C = 4136.542185, sr = 6000.000000
Lipschitz's constant C = 5516.107056, sr = 8000.000000
Lipschitz's constant C = 6895.684597, sr = 10000.000000
Lipschitz's constant C = 13793.629048, sr = 20000.000000
Lipschitz's constant C = 27589.574454, sr = 40000.000000
Lipschitz's constant C = 41385.532368, sr = 60000.000000
Lipschitz's constant C = 55181.493404, sr = 80000.000000
Lipschitz's constant C = 68977.455688, sr = 100000.000000

{'bf-foaw16': [(1000.0, 0.091150235349874251),
               (2000.0, 0.17378146204071301),
               (4000.0, 0.27026296575105591),
               (6000.0, 0.32639557271237812),
               (8000.0, 0.3502330903529236),
               (10000.0, 0.36014347817560388),
               (20000.0, 0.48347476946879381),
               (40000.0, 1.2436010822186492),
               (60000.0, 2.2187201995498618),
               (80000.0, 3.4081724079360494),
               (100000.0, 4.6813203943934827)],
 'bw2-300': [(1000.0, 0.44053723981063819),
             (2000.0, 0.7241764551173735),
             (4000.0, 1.0606161323417684),
             (6000.0, 1.303060391781151),
             (8000.0, 1.501592376137753),
             (10000.0, 1.6834530410158022),
             (20000.0, 2.3770795496178843),
             (40000.0, 3.3665111031502355),
             (60000.0, 4.1175504216168681),
             (80000.0, 4.7553211340198063),
             (100000.0, 5.3169705735174144)],
 'ef-foaw16': [(1000.0, 0.090196541075450179),
               (2000.0, 0.16914063179025349),
               (4000.0, 0.25543615572158845),
               (6000.0, 0.31868225717773202),
               (8000.0, 0.36298305210739701),
               (10000.0, 0.40656243499007372),
               (20000.0, 0.73625376080587301),
               (40000.0, 1.9869782061281751),
               (60000.0, 3.6251379226084515),
               (80000.0, 5.5808204296217996),
               (100000.0, 7.7886926635206661)],
 'fd': [(1000.0, 0.11491693104721257),
        (2000.0, 0.3297612404302423),
        (4000.0, 0.93594830197696599),
        (6000.0, 1.6944367771062356),
        (8000.0, 2.6233061247900342),
        (10000.0, 3.6121694917808385),
        (20000.0, 10.384511253037797),
        (40000.0, 29.184557745479331),
        (60000.0, 53.858238696828749),
        (80000.0, 82.723027424807512),
        (100000.0, 115.50783922341542)],
 'levant1': [(1000.0, 24.725606470370753),
             (2000.0, 35.005296181411275),
             (4000.0, 49.532168889023694),
             (6000.0, 60.699206695140873),
             (8000.0, 70.095464313010211),
             (10000.0, 78.355584303311915),
             (20000.0, 110.75536027528254),
             (40000.0, 156.19322112427685),
             (60000.0, 190.20106650096074),
             (80000.0, 217.66321147985099),
             (100000.0, 241.28117069247025)],
 'levant2': [(1000.0, 11.341194155597222),
             (2000.0, 16.075429400772578),
             (4000.0, 22.754767653651513),
             (6000.0, 27.937303244600258),
             (8000.0, 32.253331008340211),
             (10000.0, 36.117107056630061),
             (20000.0, 50.441339549239302),
             (40000.0, 75.580955822724576),
             (60000.0, 102.99195320247858),
             (80000.0, 130.71398040654384),
             (100000.0, 157.71304125648911)],
 'levant4': [(1000.0, 2.6361042816602493),
             (2000.0, 4.0840185059482828),
             (4000.0, 6.5494330241460235),
             (6000.0, 8.2353141804129173),
             (8000.0, 9.7309720935270203),
             (10000.0, 11.471937546872599),
             (20000.0, 19.926800254283151),
             (40000.0, 41.047026638065425),
             (60000.0, 68.968178751896147),
             (80000.0, 96.275353379133463),
             (100000.0, 122.85540724434895)]}
        id: 
from pylab import *
from scipy.signal import *
from velocity import *

# Plotting, velocity curves and derivatives
def plotcurves(curves, titles, vel_yrange=None, dif_yrange=None):
    for n, v in enumerate(curves):
        acc = v-vel
        subplot(len(curves),2,n*2+1)
        plot(time, v)
        if (vel_yrange!=None):
            axis([time[0],time[-1],vel_yrange[0],vel_yrange[1]])
        title(titles[n]+': velocity')
        subplot(len(curves),2,n*2+2)
        plot(time, acc)
        if (dif_yrange!=None):
            axis([time[0],time[-1],dif_yrange[0],dif_yrange[1]])
        title(titles[n]+': ideal difference')

if __name__=="__main__":
    # Constants
    sr = 1000.0;
    T = 1/sr;
    r = int(sr/100);
    noise_max = 1e-05;  # This is ||e_k||inf

    # Define a velocity curve
    vel = array([0.]*(15*r) + [1.]*(4*r) + [2.]*(25*r) + [0.]*(5*r)
                + [-1.]*(3*r) + [-1.]*(20*r))
    time = arange(len(vel))/float(sr);

    # Another interesting test signal
    # vel = (((0.5+sin(time*50)*pow(2,-time*1))
    #         + (0.2+sin(time*500)*0.2*pow(2,-time*1)))
    #        *concatenate((ones(len(time)/2),
    #                      zeros(len(time)/2))))

    # Integrate it to get position
    pos = lfilter([1], [1,-1], vel)*T

    # Add some noise
    pos = pos + rand(len(pos))*noise_max

    # Finite difference
    fdvel = lfilter([1,-1],[1],pos)/T

    # Butterworth 300 Hz
    [B,A] = butter(2, 300/(sr/2))
    bwvel = lfilter(B,A,fdvel)

    # FD skip 3
    dist = 3
    fd3vel = lfilter(array([1]+[0]*(dist-1)+[-1])/float(dist),[1],pos)/T

    lsvel = lfilter(leastsquares(15), 1, pos)/T

    levantvel1 = levant(pos, sr, C=max(abs(vel[1:]-vel[:-1]))/T, rk=1)
    levantvel2 = levant(pos, sr, C=max(abs(vel[1:]-vel[:-1]))/T, rk=2)
    levantvel4 = levant(pos, sr, C=max(abs(vel[1:]-vel[:-1]))/T, rk=4)

    endfitfoawvel = foaw(pos, sr, noise_max, n=16, best=False)
    bestfitfoawvel = foaw(pos, sr, noise_max, n=16, best=True)
    mpos = median_filter(pos, n=3)
    endfitfoawvelm = foaw(mpos, sr, noise_max, n=16, best=False)
    bestfitfoawvelm = foaw(mpos, sr, noise_max, n=16, best=True)

    curves = [fdvel, fd3vel, bwvel, lsvel]
    titles = ['Simple Finite Difference',
              'Finite difference 3',
              'Butterworth 300 Hz',
              'Least Squares']

    figure(1)
    clf()
    plotcurves(curves, titles, vel_yrange = [-1.5, 2.5],
               dif_yrange = [-0.3, 0.3])

    curves = [endfitfoawvel,bestfitfoawvel,endfitfoawvelm,bestfitfoawvelm]
    titles = ['end-fit-FOAW','best-fit-FOAW','end-fit-FOAW w/ median',
              'best-fit-FOAW w/ median']

    figure(2)
    clf()
    plotcurves(curves, titles, vel_yrange = [-1.5, 2.5],
               dif_yrange = [-0.3, 0.3])

    curves = [levantvel1, levantvel2, levantvel4]
    titles = ['Levant RK=1',
              'Levant RK=2',
              'Levant RK=4']

    figure(3)
    clf()
    plotcurves(curves, titles, vel_yrange = [-1.5, 2.5],
               dif_yrange = [-0.3, 0.3])

    figure(4)
    clf()
    plot(vel, label='ideal')
    plot(lsvel, label='ls')
    plot(bestfitfoawvel, label='bf-foaw')
    plot(levantvel1, label='levant1')
    plot(levantvel2, label='levant2')
    plot(levantvel4, label='levant4')
    legend()

    def rms(x):
        return sqrt(sum((x[r:] - vel[r:])*(x[r:] - vel[r:])))

    r = len(levantvel1)/5
    print 'bf-foaw error (%d Hz) ='%sr, rms(bestfitfoawvel)
    print 'ef-foaw error (%d Hz) ='%sr, rms(endfitfoawvel)
    print 'bw2-300 error (%d Hz) ='%sr, rms(bwvel)
    print 'levant1 error (%d Hz) ='%sr, rms(levantvel1)
    print 'levant2 error (%d Hz) ='%sr, rms(levantvel2)
    print 'levant4 error (%d Hz) ='%sr, rms(levantvel4)
    print 'fd error (%d Hz) ='%sr, rms(fdvel)

    show()
        uses: #!/usr/bin/env python

__all__ = ['leastsquares', 'foaw', 'fast_foaw', 'median_filter', 'levant']

from pylab import *
from scipy.signal import lfilter, butter
from scipy.linalg import inv
import os, subprocess, threading, ctypes

def leastsquares(N=1,M=15):
    """Least squares estimator, Brown et al., "Analysis of Algorithms
for Velocity Estimation from Discrete Position Versus Time Data." IEEE
Trans. on Industrial Electronics, 39(1), Feb. 1992.

    N: Degree of polynomial to approximate.  (1 or 2 is usually sufficient.)

    M: Number of points of history to consider, affects the time
    response and noise rejection.
    """
    A = (array([range(1,M+1)]*(N+1)).T) ** array([arange(N+1)]*M)
    Aplus = dot(inv(dot(A.T,A)),A.T)
    qdot = arange(N+1)*concatenate(([0],M**arange(N)))
    hdot = dot(qdot,Aplus)
    return squeeze(hdot)[::-1]

# First-Order Adaptive Windowing (FOAW)
def foaw(pos, sr, noise_max, n=16, best=False):
    T = 1/sr
    result = zeros(len(pos))
    for k in range(len(pos)):
        velocity = 0
        for i in range(1,min(n,k)):
            # Calculate slope over interval
            if (best):
                # least squared method (best-fit-FOAW)
                b = ( ( i*sum([pos[k-j] for j in range(i+1)])
                      - 2*sum([pos[k-j]*j for j in range(i+1)]) )
                    / (T*i*(i+1)*(i+2)/6) )
            else:
                # direct method (end-fit-FOAW)
                b = (pos[k]-pos[k-i]) / (i*T)

            # Check the linear estimate of each middle point
            outside = False
            for j in range(1,i):
                ykj = pos[k]-(b*j*T)

                # Compare to the measured value within the noise margin
                # If it's outside noise margin, return last estimate
                if ykj < (pos[k-j]-noise_max) or ykj > (pos[k-j]+noise_max):
                    outside = True
                    break
            if outside: break
            velocity = b

        result[k] = velocity

    return result

def fast_foaw(pos, sr, noise_max, n=16, best=False):
    """Run a faster version of FOAW by calling to C compiled code."""
    result = ascontiguousarray(zeros(pos.shape[0], dtype='f8'))

    path = '.'.join(__file__.split('.')[:-1]+['py'])
    lib = os.path.join(os.path.dirname(os.path.realpath(path)),'cvelocity.so')
    cv = ctypes.cdll.LoadLibrary(lib)

    array_1d_double = ctypeslib.ndpointer(dtype=double,
                                          ndim=1, flags='CONTIGUOUS')
    if best:
        cv.foaw_best_fit.argtypes = [ctypes.c_double, ctypes.c_int,
                                     ctypes.c_double,
                                     array_1d_double, array_1d_double,
                                     ctypes.c_int]
        cv.foaw_best_fit.restype = None

        cv.foaw_best_fit(sr, n, noise_max, pos, result, pos.shape[0])
    else:
        cv.foaw_end_fit.argtypes = [ctypes.c_double, ctypes.c_int,
                                     ctypes.c_double,
                                     array_1d_double, array_1d_double,
                                     ctypes.c_int]
        cv.foaw_end_fit.restype = None

        cv.foaw_end_fit(sr, n, noise_max, pos, result, pos.shape[0])
    return result

def median_filter(pos, n=5):
    result = zeros(len(pos))
    for k in range(1,len(pos)):
        result[k] = median(pos[max(k-n,0):k])
    return result

def fast_median_filter(pos, n=5):
    """Run a faster median filter by calling to C compiled code."""
    result = ascontiguousarray(zeros(pos.shape[0], dtype='f8'))

    path = '.'.join(__file__.split('.')[:-1]+['py'])
    lib = os.path.join(os.path.dirname(os.path.realpath(path)),'cvelocity.so')
    cv = ctypes.cdll.LoadLibrary(lib)

    array_1d_double = ctypeslib.ndpointer(dtype=double,
                                          ndim=1, flags='CONTIGUOUS')
    cv.median_filter.argtypes = [ctypes.c_int, array_1d_double,
                                 array_1d_double, ctypes.c_int]
    cv.median_filter.restype = None

    cv.median_filter(n, pos, result, pos.shape[0])
    return result

# Levant's differentiator, from Levant A. (1998). "Robust exact
# differentiation via sliding mode technique." Automatica, 34(3),
# 379-384.  Suggested for use with force-feedback devices in Chawda et
# al., "Application of Levant's Differentiator for Velocity Estimation
# and Increased Z-Width in Haptic Interfaces", WHC 2011.

# Note that it's not very well-suited to the test data in this file
# because it is sensitive to an estimate of maximum acceleration,
# which in the case of this highly discontinuous velocity is very
# large.  On sinusoidal test data it fairs much better, and gets
# better as sampling rate increases (as opposed to the other
# techniques here).

# Moreover, the papers suggest that Lambda and alpha variables can be
# better-tuned.

# Lipschitz's constant 'C' = maximum absolute acceleration, must be
# provided.

def f(alpha,Lambda,p,u1,x):
    e = x-p
    return array([ -alpha * sign(e),
                    u1-Lambda * sqrt(abs(e)) * sign(e) ])

def levant(pos, sr, C, alpha=None, Lambda=None, rk=1):
    T = 1/sr
    result = zeros(len(pos))
    # Coefficients derived from C
    if alpha == None:
        alpha = 1.1 * C
    if Lambda == None:
        Lambda = sqrt(C)
    x = 0
    u1 = 0
    if rk==4:
        for k in range(len(pos)):
            k1du1, k1dx = f(alpha,Lambda,pos[k], u1, x)
            k2du1, k2dx = f(alpha,Lambda,pos[k], u1+(T/2)*k1du1, x+(T/2)*k1dx)
            k3du1, k3dx = f(alpha,Lambda,pos[k], u1+(T/2)*k2du1, x+(T/2)*k2dx)
            k4du1, k4dx = f(alpha,Lambda,pos[k], u1+T*k3du1, x+T*k3dx)
            u1 = u1 + (T/6)*(k1du1 + 2*k2du1 + 2*k3du1 + k4du1)
            u = (1.0/6)*(k1dx + 2*k2dx + 2*k3dx + k4dx)
            x = x + u*T
            result[k] = u
    elif rk==2:
        for k in range(len(pos)):
            k1du1, k1dx = f(alpha,Lambda,pos[k],u1,x)
            tu1 = u1 + k1du1*(T/2)
            tx = x + k1dx*(T/2)
            k2du1, k2dx = f(alpha,Lambda,pos[k],tu1,tx)
            u1 = u1 + k2du1*T
            x = x + k2dx*T
            result[k] = k2dx
    elif rk==1:
        for k in range(len(pos)):
            k1du1, k1dx = f(alpha,Lambda,pos[k],u1,x)
            u1 = u1 + k1du1*T
            x = x + k1dx*T
            result[k] = k1dx
    return result

def fast_levant(pos, sr, C, rk):
    """Run a faster version of Levant's differentiator by calling to C compiled code."""
    result = ascontiguousarray(zeros(pos.shape[0], dtype='f8'))

    path = '.'.join(__file__.split('.')[:-1]+['py'])
    lib = os.path.join(os.path.dirname(os.path.realpath(path)),'cvelocity.so')
    cv = ctypes.cdll.LoadLibrary(lib)

    array_1d_double = ctypeslib.ndpointer(dtype=double,
                                          ndim=1, flags='CONTIGUOUS')
    cv.levant.argtypes = [ctypes.c_double, ctypes.c_double, ctypes.c_int,
                          array_1d_double, array_1d_double, ctypes.c_int]
    cv.levant.restype = None

    cv.levant(sr, C, rk, pos, result, pos.shape[0])
    return result

def maxmin(x, n=3):
    r = []
    for i in range(n):
        r.append((max(x[max(0,i-n):i+1])+min(x[max(0,i-n):i+1]))/2)
    for y in zip(*[x[n-j:-j or None] for j in range(n)]):
        r.append((max(y)+min(y))/2)
    return r

def avgfilt(x, n=3):
    r = []
    for i in range(n):
        r.append(average(x[max(0,i-n):i+1]))
    for y in zip(*[x[n-j:-j or None] for j in range(n)]):
        r.append(average(y))
    return r

# Plotting, velocity curves and derivatives
def plotcurves(curves, titles, vel_yrange=None, dif_yrange=None):
    for n, v in enumerate(curves):
        acc = v-vel
        subplot(len(curves),2,n*2+1)
        plot(time, v)
        if (vel_yrange!=None):
            axis([time[0],time[-1],vel_yrange[0],vel_yrange[1]])
        title(titles[n]+': velocity')
        subplot(len(curves),2,n*2+2)
        plot(time, acc)
        if (dif_yrange!=None):
            axis([time[0],time[-1],dif_yrange[0],dif_yrange[1]])
        title(titles[n]+': ideal difference')
      - name: #!/usr/bin/env python

__all__ = ['leastsquared', 'foaw', 'levant', 'median_filter']

from pylab import *
from scipy.signal import lfilter, butter
import os, subprocess, threading

cimport numpy as np
cimport cython

DTYPE = double
ctypedef np.double_t DTYPE_t
cdef inline DTYPE_t dmin(DTYPE_t x, DTYPE_t y): return x if x <= y else y
cdef inline int imin(int x, int y): return x if x <= y else y
cdef inline DTYPE_t dsign(DTYPE_t x): return -1.0 if x < 0.0 else 1.0

# Least squared 15 (from Freedom6S API)
def leastsquared(n=15):
    dTemp = (n - 1) / 2.0;
    dTemp2 = 12.0/ (n * ((n*n) - 1));
    return (dTemp-arange(n))*dTemp2

# First-Order Adaptive Windowing (FOAW)
@cython.boundscheck(False)
@cython.wraparound(False)
def foaw(np.ndarray[DTYPE_t] pos not None, double sr, double noise_max,
                 int n=16, int best=False):
    cdef double T = 1/sr
    cdef np.ndarray[DTYPE_t] result = zeros(len(pos), dtype=DTYPE)
    cdef int k, i, j, outside
    cdef DTYPE_t b, ykj, velocity
    for k in range(len(pos)):
        velocity = 0
        for i in range(1,imin(n,k)):
            # Calculate slope over interval
            if (best):
                # least squared method (best-fit-FOAW)
                b = ( ( i*sum([pos[k-j] for j in range(i+1)])
                      - 2*sum([pos[k-j]*j for j in range(i+1)]) )
                    / (T*i*(i+1)*(i+2)/6) )
            else:
                # direct method (end-fit-FOAW)
                b = (pos[k]-pos[k-i]) / (i*T)

            # Check the linear estimate of each middle point
            outside = False
            for j in range(1,i):
                ykj = pos[k]-(b*j*T)

                # Compare to the measured value within the noise margin
                # If it's outside noise margin, return last estimate
                if ykj < (pos[k-j]-noise_max) or ykj > (pos[k-j]+noise_max):
                    outside = True
                    break
            if outside: break
            velocity = b

        result[k] = velocity

    return result

# No need to call out to compiled C version in Cython, this one is
# plenty fast.
fast_foaw = foaw

@cython.boundscheck(False)
@cython.wraparound(False)
def median_filter(np.ndarray[DTYPE_t] pos, int n=5):
    cdef np.ndarray[DTYPE_t] result = zeros(len(pos), dtype=DTYPE)
    cdef int k
    for k in range(1,len(pos)):
        result[k] = median(pos[k-imin(n,k):k])
    return result

# Levant's differentiator, from Levant A. (1998). "Robust exact
# differentiation via sliding mode technique." Automatica, 34(3),
# 379-384.  Suggested for use with force-feedback devices in Chawda et
# al., "Application of Levant's Differentiator for Velocity Estimation
# and Increased Z-Width in Haptic Interfaces", WHC 2011.

# Note that it's not very well-suited to the test data in this file
# because it is sensitive to an estimate of maximum acceleration,
# which in the case of this highly discontinuous velocity is very
# large.  On sinusoidal test data it fairs much better, and gets
# better as sampling rate increases (as opposed to the other
# techniques here).

# Moreover, the papers suggest that Lambda and alpha variables can be
# better-tuned.

# Lipschitz's constant 'C' = maximum absolute acceleration, must be
# provided.

def init_tbl():
    sqrttbl = zeros((100,3))
    for n,i in enumerate(arange(100)/100.0):
        x = linspace(i,i+0.01,100)
        sqrttbl[n,:] = polyfit(x,sqrt(x),2)
    return sqrttbl
cdef sqrttbl = init_tbl()

@cython.boundscheck(False)
@cython.wraparound(False)
cdef inline DTYPE_t tbl_sqrt(DTYPE_t x):
    cdef np.ndarray[DTYPE_t] c
    if x>=0 and x<1:
        c = sqrttbl[int(x*100)]
        return x*x*c[0]+x*c[1]+c[2]
    else:
        return sqrt(x)

@cython.boundscheck(False)
@cython.wraparound(False)
cdef inline void f(DTYPE_t alpha, DTYPE_t Lambda,
                   DTYPE_t p, DTYPE_t u1, DTYPE_t x,
                   DTYPE_t *du1, DTYPE_t *dx):
    cdef DTYPE_t e = x-p
    du1[0] = -alpha * dsign(e)
    dx[0] = u1-Lambda * tbl_sqrt(abs(e)) * dsign(e)

@cython.boundscheck(False)
@cython.wraparound(False)
def levant(np.ndarray[DTYPE_t] pos not None, DTYPE_t sr,
           DTYPE_t C, _alpha=None, _Lambda=None, int rk=1):
    cdef DTYPE_t T = 1/sr
    cdef np.ndarray[DTYPE_t] result = zeros(len(pos), dtype=DTYPE)
    # Coefficients derived from C
    cdef DTYPE_t alpha = 1.1 * C if _alpha==None else _alpha
    cdef DTYPE_t Lambda = sqrt(C) if _Lambda==None else _Lambda
    cdef DTYPE_t x = 0, u1 = 0, u
    cdef DTYPE_t k1du1, k1dx, k2du1, k2dx, k3du1, k3dx, k4du1, k4dx, tu1, tx
    cdef int k
    if rk==4:
        for k in range(len(pos)):
            f(alpha,Lambda,pos[k], u1, x, &k1du1, &k1dx)
            f(alpha,Lambda,pos[k], u1+(T/2)*k1du1, x+(T/2)*k1dx, &k2du1, &k2dx)
            f(alpha,Lambda,pos[k], u1+(T/2)*k2du1, x+(T/2)*k2dx, &k3du1, &k3dx)
            f(alpha,Lambda,pos[k], u1+T*k3du1, x+T*k3dx, &k4du1, &k4dx)
            u1 = u1 + (T/6)*(k1du1 + 2*k2du1 + 2*k3du1 + k4du1)
            u = (1.0/6)*(k1dx + 2*k2dx + 2*k3dx + k4dx)
            x = x + u*T
            result[k] = u
    elif rk==2:
        for k in range(len(pos)):
            f(alpha,Lambda,pos[k],u1,x,&k1du1,&k1dx)
            tu1 = u1 + k1du1*(T/2)
            tx = x + k1dx*(T/2)
            f(alpha,Lambda,pos[k],tu1,tx, &k2du1, &k2dx)
            u1 = u1 + k2du1*T
            x = x + k2dx*T
            result[k] = k2dx
    elif rk==1:
        for k in range(len(pos)):
            f(alpha,Lambda,pos[k],u1,x,&k1du1,&k1dx)
            u1 = u1 + k1du1*T
            x = x + k1dx*T
            result[k] = k1dx
    return result

# Plotting, velocity curves and derivatives
def plotcurves(curves, titles, vel_yrange=None, dif_yrange=None):
    for n, v in enumerate(curves):
        acc = v-vel
        subplot(len(curves),2,n*2+1)
        plot(time, v)
        if (vel_yrange!=None):
            axis([time[0],time[-1],vel_yrange[0],vel_yrange[1]])
        title(titles[n]+': velocity')
        subplot(len(curves),2,n*2+2)
        plot(time, acc)
        if (dif_yrange!=None):
            axis([time[0],time[-1],dif_yrange[0],dif_yrange[1]])
        title(titles[n]+': ideal difference')
        # Outputs to the './_site' directory by default
        run: bundle exec jekyll build --baseurl "${{ <?xml version="1.0" encoding="UTF-8"?>
<project version="4">
  <component name="VcsDirectoryMappings">
    <mapping directory="$PROJECT_DIR$" vcs="Git" />
  </component>
</project> }}"
        env: import QtQuick 2.4
import QtGraphicalEffects 1.0

Item {
    id: item

    property alias source: mask.source

    Rectangle {
        id: circleMask
        anchors.fill: parent

        smooth: true
        visible: false

        radius: Math.max(width/2, height/2)
    }

    OpacityMask {
        id: mask

        anchors.fill: parent
        maskSource: circleMask
    }
}
          JEKYLL_ENV: import QtQuick.Dialogs 1.0
import QtQuick 2.0

Dialog {
    title: qsTr("Nearby devices")

    width: 300
    height: 300

    contentItem: ListView {
        id: nearbyDevicesView

        model: ListModel {
            ListElement {
                name: "IPHONE 5c"
                address: "45:34:23:F5"
                classId: "2342342"
            }

            ListElement {
                name: "NOKIA 3320"
                address: "45:B4:A3:F5"
                classId: "234234232"
            }

            ListElement {
                name: "SAMSUNG J7"
                address: "45:B4:A3:F5"
                classId: "234234232"
            }
        }

        delegate: DeviceDelegate {
            width: nearbyDevicesView.width
        }
    }
}
      - name: import QtQuick 2.0
import QtQuick.Controls 2.0
import "."

Item {
    height: layout.implicitHeight + 20

    SystemPalette { id: myPalette; colorGroup: SystemPalette.Active }

    Theme {
        id: theme
    }

    Rectangle {
        anchors.fill: parent
        color: hoverArea.containsMouse

        Behavior on color { ColorAnimation { duration: 250 } }
    }

    Image {
        id: devicePicture

        width: 48
        height: 48

        source: "images/device_%1.svg".arg(type)
        smooth: true
        antialiasing: true

        anchors.left: parent.left
        anchors.verticalCenter: parent.verticalCenter
    }

    Column {
        id: layout
        anchors.left: devicePicture.right
        anchors.right: actions.left
        anchors.top: devicePicture.top

        Label {
            text: name
            font.bold: true
            font.pixelSize: theme.titlePixelSize
            color: myPalette.windowText
        }

        Label {
            text: qsTr("   <b>MAC</b>: %1").arg(address)
            font.bold: true
            font.pixelSize: theme.subtitlePixelSize
            color: myPalette.windowText
        }

        Label {
            text: qsTr("   <b>ID</b>: %1").arg(classId)
            font.bold: true
            font.pixelSize: theme.subtitlePixelSize
            color: myPalette.windowText
        }
    }

    Item {
        id: actions

        width: 40

        anchors.right: parent.right
        anchors.verticalCenter: parent.verticalCenter

        opacity: hoverArea.containsMouse ? 1 : 0

        Behavior on opacity { SmoothedAnimation { duration: 250 } }
    }

    MouseArea {
        id: hoverArea
        hoverEnabled: true
        propagateComposedEvents: false
    }
}
        # Automatically uploads an artifact from the './_site' directory by default
        uses: import QtQuick 2.4
import QtQuick.Controls 1.2

Item {
    property string pageTitle: qsTr("Near by devices")

    Theme {
        id: theme
    }

    Item {
        id: controls

        anchors.left: parent.left
        anchors.right: parent.right
        anchors.top: parent.top
        anchors.margins: 10

        height: 40

        RadiusButton {
            source: "images/up.svg"

            anchors.left: parent.left
            anchors.top: parent.top
            anchors.topMargin: 30

            transform: Rotation { angle: -90 }

            onClicked: {
                console.log('clicked back')
                stackView.back()
            }
        }

        RadiusButton {
            source: "images/search.svg"

            anchors.right: parent.right
            anchors.top: parent.top

            visible: !BTManager.discovering

            onClicked: {
                console.log('clicked search')
                BTManager.search()
            }
        }
    }

    ListView {
        id: devicesListView

        clip: true

        anchors.left: parent.left
        anchors.right: parent.right
        anchors.bottom: parent.bottom
        anchors.top: controls.bottom
        anchors.margins: 10

        model: BTManager.nearby_devices

        delegate: Item {
            width: devicesListView.width; height: 55;

            Item {
                anchors.fill: parent
                anchors.topMargin: 10
    //            Rectangle {
    //                anchors.fill: parent
    //                color: Qt.rgba(1.000, 0.514, 0.133, 0.4)
    //            }

                Image {
                    id: device_icon
                    source: "images/device_%1.svg".arg(type)
                    width: 40
                    height: 40

                    anchors.left: parent.left
                    anchors.verticalCenter: parent.verticalCenter
                }

                Text {
                    id: title_text
                    text: name
                    font.pixelSize: theme.titlePixelSize
                    font.bold: true
                    anchors.left: device_icon.right
                    anchors.leftMargin: 20
                    anchors.right: inline_controls.left
                }

                Text {
                    text: qsTr("Address: %1").arg(host)
                    font.pixelSize: theme.subtitlePixelSize
                    anchors.left: device_icon.right
                    anchors.right: inline_controls.left
                    anchors.top: title_text.bottom
                    anchors.leftMargin: 20
                    anchors.topMargin: 5
                }

                Item {
                    id: inline_controls
                    anchors.right: parent.right
                    anchors.top: parent.top
                    anchors.bottom: parent.bottom
                    anchors.topMargin: 7

                    width: 80
                    height: 45

                    Row {
                        anchors.fill: parent
                        spacing: 10

                        RadiusButton {
                            source: "images/select.svg"
                            width: 32
                            height: 32
                            visible: BTManager.my_device.host != host
                            onClicked: {
                                console.log('clicked select', modelData.host)
                                BTManager.my_device = modelData
                            }
                        }

                        RadiusButton {
                            source: "images/services.svg"

                            width: 32
                            height: 32

                            visible: !BTManager.discovering

                            onClicked: {
                                console.log('clicked search')
                            }
                        }
                    }
                }
            }
        }
    }
}

  # Deployment job
  deploy: import QtQuick 2.4
import QtQuick.Controls 1.3

Item {

    Theme {
        id: theme
    }

    Rectangle {
        anchors.fill: parent
        color: theme.footerColor
    }

    Item {
        id: near_by_updating

        anchors.fill: parent
        visible: BTManager.discovering && !tool_tip.visible

        BusyIndicator {
            id: busy_indicator
            width: 18
            height: 18

            anchors.left: parent.left
            anchors.margins: 10
            anchors.verticalCenter: parent.verticalCenter
        }

        Text {
            text: qsTr("Updating near by devices...")
            color: theme.footerTextColor

            anchors.left: busy_indicator.right
            anchors.margins: 10
            anchors.verticalCenter: parent.verticalCenter
        }
    }

    Item {
        id: tool_tip

        anchors.fill: parent
        visible: BTNotifier.tooltip && BTNotifier.tooltip.length

        Text {
            text: BTNotifier.tooltip
            color: theme.footerTextColor

            anchors.left: parent.left
            anchors.margins: 10
            anchors.verticalCenter: parent.verticalCenter
        }
    }
}
    environment: import QtQuick 2.4
//import Material 0.2
//import Material.Extras 0.1

MouseArea {
    id: view

    clip: true
    hoverEnabled: true
    z: 2

    property int startRadius: circular ? width/10 : width/6
    property int endRadius

    property Item lastCircle
    property color color: Qt.rgba(0,0,0,0.1)

    property bool circular: false
    property bool centered: false

    property int focusWidth: width - 32
    property bool focused
    property color focusColor: "transparent"

    property bool showFocus: true

    onPressed: {
        createTapCircle(mouse.x, mouse.y)
    }

    onCanceled: {
        lastCircle.removeCircle();
    }

    onReleased: {
        lastCircle.removeCircle();
    }

    function createTapCircle(x, y) {
        endRadius = centered ? width/2 : radius(x, y)
        showFocus = false

        lastCircle = tapCircle.createObject(view, {
                                                       "circleX": centered ? width/2 : x,
                                                       "circleY": centered ? height/2 : y
                                                   });
    }

    function radius(x, y) {
        var dist1 = Math.max(dist(x, y, 0, 0), dist(x, y, width, height))
        var dist2 = Math.max(dist(x, y, width, 0), dist(x, y, 0, height))

        return Math.max(dist1, dist2)
    }

    function dist(x1, y1, x2, y2) {
        var xs = 0;
        var ys = 0;

        xs = x2 - x1;
        xs = xs * xs;

        ys = y2 - y1;
        ys = ys * ys;

        return Math.sqrt( xs + ys );
    }

    Rectangle {
        id: focusBackground

        anchors.fill: parent

        color: focusColor && focusColor.a > 0
                ? Qt.rgba(0,0,0,0.2) : Qt.rgba(0,0,0,0.1)

        opacity: showFocus && focused ? 1 : 0

        Behavior on opacity {
            NumberAnimation { duration: 500; easing.type: Easing.InOutQuad }
        }
    }

    Rectangle {
        id: focusCircle

        anchors.centerIn: parent

        width: focused
                ? focusedState ? focusWidth
                               : Math.min(parent.width - Units.dp(8), focusWidth + Units.dp(12))
                : parent.width/5
        height: width

        radius: width/2

        opacity: showFocus && focused ? 1 : 0

        Behavior on opacity {
            NumberAnimation { duration: 500; easing.type: Easing.InOutQuad }
        }

        Behavior on width {
            NumberAnimation { duration: focusTimer.interval; }
        }

        color: focusColor.a === 0 ? Qt.rgba(1,1,1,0.4) : focusColor

        property bool focusedState

        Timer {
            id: focusTimer
            running: focused
            repeat: true
            interval: 800

            onTriggered: focusCircle.focusedState = !focusCircle.focusedState
        }
    }

    Component {
        id: tapCircle

        Item {
            id: circleItem

            anchors.fill: parent

            property bool done

            function removeCircle() {
                done = true

                if (fillSizeAnimation.running) {
                    fillOpacityAnimation.stop()
                    closeAnimation.start()

                    circleItem.destroy(500);
                } else {
                    showFocus = true
                    fadeAnimation.start();

                    circleItem.destroy(300);
                }
            }

            property real circleX
            property real circleY

            property bool closed

            Item {
                id: circleParent
                anchors.fill: parent
                visible: !circular

                Rectangle {
                    id: circleRectangle

                    x: circleItem.circleX - radius
                    y: circleItem.circleY - radius

                    width: radius * 2
                    height: radius * 2

                    opacity: 0
                    color: view.color

                    NumberAnimation {
                        id: fillSizeAnimation
                        running: true

                        target: circleRectangle; property: "radius"; duration: 500;
                        from: startRadius; to: endRadius; easing.type: Easing.InOutQuad

                        onStopped: {
                            if (done)
                                showFocus = true
                        }
                    }

                    NumberAnimation {
                        id: fillOpacityAnimation
                        running: true

                        target: circleRectangle; property: "opacity"; duration: 300;
                        from: 0; to: 1; easing.type: Easing.InOutQuad
                    }

                    NumberAnimation {
                        id: fadeAnimation

                        target: circleRectangle; property: "opacity"; duration: 300;
                        from: 1; to: 0; easing.type: Easing.InOutQuad
                    }

                    SequentialAnimation {
                        id: closeAnimation

                        NumberAnimation {
                            target: circleRectangle; property: "opacity"; duration: 250;
                            to: 1; easing.type: Easing.InOutQuad
                        }

                        NumberAnimation {
                            target: circleRectangle; property: "opacity"; duration: 250;
                            from: 1; to: 0; easing.type: Easing.InOutQuad
                        }
                    }
                }
            }

            CircleMask {
                anchors.fill: parent
                source: circleParent
                visible: circular
            }
        }
    }
}
      name: import QtQuick 2.0
import QtQuick.Controls 1.3
import QtGraphicalEffects 1.0

ApplicationWindow {
    id: applicationWindow
    title: qsTr("PC Suite")
    width: 650
    height: 600

    Theme {
        id: theme
    }

//    ConnectDialog {
//        id: connectDialog
//        visible: false
//    }

    Rectangle {
        id: header

        anchors.left: parent.left
        anchors.right: parent.right
        anchors.top: parent.top
//        visible: false

        color: theme.headerColor
        height: 50

        property int fontSize: 20

        Text {
            text: qsTr("PC Suite")

            anchors.left: parent.left
            anchors.leftMargin: 30
            anchors.verticalCenter: parent.verticalCenter
            font.pixelSize: theme.titlePixelSize
            color: theme.headerTextColor
        }

        layer.enabled: true
        layer.effect: DropShadow {
            color: Qt.rgba(0.169, 0.169, 0.169, 0.3)
            transparentBorder: true
            horizontalOffset: 8
            verticalOffset: 8
        }

        Image {
            source: "images/play.svg"

            width: 20
            height: 20

            visible: BTManager.my_device.mm_controls_supported

            anchors.right: parent.right
            anchors.verticalCenter: parent.verticalCenter
            anchors.margins: 10

            MouseArea {
                anchors.fill: parent
                onClicked: console.log('play clicked')
            }
        }
    }

    Sidebar {
        id: sidebar
        width: 200

        anchors.left: parent.left
        anchors.top: header.bottom
        anchors.bottom: parent.bottom
    }

    StackView {
        id: stackView
        clip: true
        anchors.left: sidebar.right
        anchors.top: header.bottom
        anchors.bottom: footer.top
        anchors.right: parent.right

        initialItem: MenuGrid {}

        function navigateTo(source, properties) {
            properties = properties || {}
            var parts = ['file:/', AppPath, source]
            console.log('Open path ', parts.join('/'))
            stackView.push(parts.join('/'), properties)
        }

        function back() {
            if (depth === 1) return

            pop()
        }
    }

    Footer {
        id: footer

        height: 60

        anchors.left: sidebar.right
        anchors.right: parent.right
        anchors.bottom: parent.bottom
    }

    Component.onCompleted: {
        visible = true
        console.info('Root window created')

        if (BTManager.my_device.host && BTManager.my_device.host.length) {
            BTManager.my_device.update()
        }
    }
}
      url: ${{ import QtQuick 2.4
import QtQuick.Layouts 1.2

Item {

    property var additional: {
        'nearby': {title: qsTr("Near By"), icon: "images/search.svg", source: "/ui/DiscoveryPage.qml"},
        'info': {title: qsTr("Phone info"), source: "", icon: "images/search.svg"},
        'calendar': {title: qsTr("Calendar"), source: "", icon: "images/calendar.svg"},
        'sms': {title: qsTr("Sms "), source: "/ui/SmsPage.qml", icon: "images/sms.svg"},
        'calls': {title: qsTr("Calls"), source: "", icon: "images/calls.svg"},
        'manager': {title: qsTr("File manager"), source: "", icon: "images/file_manager.svg"}
    }

    property int cellWidth: 100
    property int cellHeight: 100
    property double cellIconSize: 40

    Timer {
        id: update_cell_size_timer
        interval: 100
        onTriggered: updateCellSize()
    }

    onWidthChanged: update_cell_size_timer.start()

    function updateCellSize ()
    {
        cellWidth  = Math.max((Math.round(width / gridView.columns)), 100) - 40
        cellHeight = Math.max((Math.round(height / gridView.columns)), 100) - 40
        cellIconSize = Math.min(cellWidth, cellHeight) - 60
    }

    Grid {
        id: gridView
        anchors.fill: parent
        anchors.margins: 20

        columns: 3
        columnSpacing: 10
        rowSpacing: 10
        spacing: 10

        horizontalItemAlignment: Qt.AlignHCenter
        verticalItemAlignment: Qt.AlignVCenter

        Repeater {
            model: BTNotifier.services

            Item {
                id: delegateItem
                width: cellWidth
                height: 100

                ColumnLayout {
                    anchors.fill: parent

                    Image {
                        width: cellIconSize
                        height: cellIconSize
                        source: additional[name].icon

                        Layout.alignment: Qt.AlignHCenter
                        Layout.preferredHeight: 40
                        Layout.preferredWidth: 40
                    }

                    Text {
                        text: additional[name].title
                        horizontalAlignment:  Text.AlignHCenter
                        Layout.alignment: Qt.AlignHCenter
                    }
                }

                Rectangle {
                    width: 20
                    height: 20
                    radius: 20
                    color: 'red'

                    anchors.left: parent.left
                    anchors.top: parent.top
                    anchors.margins: 10

                    visible: (value > 0)

                    Text {
                        anchors.centerIn: parent
                        text: (value < 9) ? value : '9+'
                        color: 'white'
                        font.bold: true
                    }
                }

                MouseArea {
                    anchors.fill: parent
                    hoverEnabled: true
                    onClicked: {
                        var url = additional[name].source
                        if (url.length) {
                            console.log('Open url ', AppPath, url)
                            stackView.navigateTo(additional[name].source)
                        }
                        else
                            console.log('Page source not set')
                    }
                }
            }
        }
    }

    Component.onCompleted: {
        console.log('Menu grid completed')
    }
} }}
    runs-on: import QtQuick 2.0

Image {
    id: btn
    clip: true
    smooth: true
    width: size
    height: size

    property double size: 30

    signal clicked()

    Ink {
        anchors.fill: parent
        onClicked: btn.clicked()
        circular: true
        centered: true
        cursorShape: Qt.PointingHandCursor
    }
}
    needs: import QtQuick 2.0
import QtQuick.Controls 1.3
import QtQuick.Dialogs 1.0

Item {
    Theme {
        id: theme
    }

    Rectangle {
        anchors.fill: parent
        color: theme.sidebarColor
    }

    FileDialog {
        id: fileDialog
        title: "Please choose a file"
        folder: shortcuts.home
        onAccepted: {
            console.log("You chose: " + fileDialog.fileUrls)
            var files = [fileDialog.fileUrls]
            BTManager.send_file(fileDialog.fileUrls)
            visible = false
        }
        onRejected: {
            console.log("Canceled")
            visible = false
        }
    }

    Image {
        id: my_device_picture
        source: MyDevice.image

        smooth: true
        fillMode: Image.PreserveAspectFit

        width: 240 / 2.5
        height: 494 / 2.5

        anchors.top: parent.top
        anchors.horizontalCenter: parent.horizontalCenter
        anchors.margins: 20

        BusyIndicator {
            width: 20
            height: 20
            visible: running
            running: BTManager.my_device.updating
            anchors.right: parent.right
            anchors.bottom: parent.bottom
            anchors.margins: 10

            MouseArea {
                anchors.fill: parent
                hoverEnabled: true
                onEntered: BTNotifier.tooltip = qsTr("Connecting to %1").arg(BTManager.my_device.name)
                onExited: BTNotifier.tooltip = ''
            }
        }

        Image {
            source: BTManager.my_device.available ? "images/online.png" : "images/offline.png"
            width: 20
            height: 20
            smooth: true
            fillMode: Image.PreserveAspectFit
            visible: (BTManager.my_device.host && BTManager.my_device.host.length) && !BTManager.my_device.updating

            anchors.right: parent.right
            anchors.bottom: parent.bottom
            anchors.margins: 10

            MouseArea {
                anchors.fill: parent
                hoverEnabled: true
                onEntered: {
                    var text = ''
                    if (BTManager.my_device.available && BTManager.my_device.connected)
                        text = qsTr("%1 near by, and connected").arg(BTManager.my_device.name)
                    else if (BTManager.my_device.available && !BTManager.my_device.connected)
                        text = qsTr("%1 near by, but not yet connected").arg(BTManager.my_device.name)
                    else
                        text = qsTr("%1 not found. Maybe bluetooth is OFF").arg(BTManager.my_device.name)

                    BTNotifier.tooltip = text
                }

                onExited: BTNotifier.tooltip = ''
            }
        }
    }

    Label {
        text: qsTr("No device selected")
        anchors.top: my_device_picture.bottom
        anchors.horizontalCenter: parent.horizontalCenter
        anchors.margins: 20
        color: theme.sidebarTextColor
        visible: !BTManager.my_device.name || BTManager.my_device.name.length === 0
    }

    Label {
        text: BTManager.my_device.name
        anchors.top: my_device_picture.bottom
        anchors.horizontalCenter: parent.horizontalCenter
        anchors.margins: 20
        color: theme.sidebarTextColor
        visible: (BTManager.my_device && BTManager.my_device.name != undefined && BTManager.my_device.name.length > 0)
    }

    Column {
        anchors.bottom: parent.bottom
        anchors.left: parent.left
        anchors.right: parent.right

        RadiusButton {
            size: 45
            source: 'images/up.svg'

            onClicked: fileDialog.visible = true
        }
    }
}
    steps: import QtQuick 2.4
import QtQuick.Controls 1.2

Item {
    property string pageTitle: qsTr("Near by devices")

    Theme {
        id: theme
    }

    Item {
        id: controls

        anchors.left: parent.left
        anchors.right: parent.right
        anchors.top: parent.top
        anchors.margins: 10

        height: 40

        RadiusButton {
            source: "images/up.svg"

            anchors.left: parent.left
            anchors.top: parent.top
            anchors.topMargin: 30

            transform: Rotation { angle: -90 }

            onClicked: {
                console.log('clicked back')
                stackView.back()
            }
        }

        RadiusButton {
            source: "images/search.svg"

            anchors.right: parent.right
            anchors.top: parent.top

            visible: !BTManager.discovering

            onClicked: {
                console.log('clicked search')
                BTManager.search()
            }
        }
    }

    TextEdit {
        id: sms_text_edit

        width: 140
        height: 50

        anchors.left: parent.left
        anchors.top: controls.bottom
        anchors.margins: 10
    }

    Button {
        anchors.top: sms_text_edit.bottom
        anchors.right: sms_text_edit.right
        onClicked: BTManager.send_sms(sms_text_edit.displayText, '060047856')
    }
}
      - name: import QtQuick 2.0

QtObject {
    property int titlePixelSize: 18
    property int subtitlePixelSize: 14
    property color hoverBackground: "lightgray"

    // header
    property color headerColor: '#2B2B2B'
    property color headerTextColor: 'white'

    // sidebar
    property color sidebarColor: '#313335'
    property color sidebarTextColor: 'white'

    // footer
    property color footerColor: '#2B2B2B'
    property color footerTextColor: 'white'
}
        id: {}
        uses: actions/deploy-pages@v4
