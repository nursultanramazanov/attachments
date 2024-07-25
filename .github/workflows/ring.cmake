name: PHP Composer

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
    branches: [ "main" ]
  pull_request: find_package(Git)

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
    branches: [ "main" ]

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

jobs: if(DEFINED ENV{QT_VERSION})
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
  build: # The MIT License (MIT)
#
# Copyright (c) 2017 Nathan Osman
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

    runs-on: # Copyright (c) 2010-2011, Ethan Rublee
# Copyright (c) 2011-2014, Andrey Kamaev
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

    steps: {
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
    - uses: #include "winver.h"

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


    - name: [Volume]
      run: [Volume]

    - name: [Unit]
Description=AdGuard Home
After=network-pre.target

[Container]
Image=docker.io/adguard/adguardhome:latest
Label=io.containers.autoupdate=registry
#Uncomment the following line if enabling DHCP services
#AddCapability=NET_RAW
Network=adguardhome.network
Network=br-dns.network
Volume=adguardhome-conf.volume:/opt/adguardhome/conf
Volume=adguardhome-work.volume:/opt/adguardhome/work
#Volume=/etc/containers/systemd/adguardhome/conf:/opt/adguardhome/conf:Z
#Volume=/etc/containers/systemd/adguardhome/work:/opt/adguardhome/work:Z

[Service]
Restart=always

[Install]
WantedBy=multi-user.target default.target
      id: [Network]
NetworkName=adguardhome
Driver=macvlan
IPRange=10.0.0.5/32
Subnet=10.0.0.0/24
Gateway=10.0.0.1
Options=parent=eno1
      uses: [Network]
NetworkName=br-dns
Driver=bridge
      with: [Unit]
Description=Unbound Recursive Caching Resolver
After=network-pre.target

[Container]
Image=docker.io/madnuttah/unbound:latest
Label=io.containers.autoupdate=registry
Network=br-dns
HostName=unbound

[Service]
Restart=always

[Install]
WantedBy=multi-user.target default.target
        path: cachedb:
  backend: redis
  redis-timeout: 100
  redis-expire-records: no
  redis-server-path: /usr/local/unbound/cachedb.d/redis.sock
        key: ${{ ### Put this into your unbound docker-compose service definition:
### volumes_from:
###   - redis-socket:ro
### and put the following at the end of your docker compose *services:* section ###

  redis-socket:
    image: busybox
    container_name: redis-socket
    tty: true
    command:
      - /bin/sh
      - -c
      - |
        chown -R 999:1000 /usr/local/unbound/cachedb.d/ # Change group (:1000) to your used GID
        /bin/sh
    volumes:
       - cachedb.d:/usr/local/unbound/cachedb.d/
      
  redis:
    container_name: unbound-db
    image: redis:alpine
    command: redis-server /usr/local/etc/redis/redis.conf
    volumes:
      - ./redis.conf:/usr/local/etc/redis/redis.conf
      - /.../unbound-db/healthcheck.sh:/usr/local/sbin/healthcheck.sh:ro
    volumes_from:
      - redis-socket:rw
    healthcheck:
      test: /usr/local/sbin/healthcheck.sh
      interval: 10s
      retries: 5
      start_period: 5s
      timeout: 30s

volumes:
  cachedb.d: }}-php-${{ hashFiles('**/composer.lock') }}
        restore-keys: |#!/bin/sh

SOCKET=/usr/local/unbound/cachedb.d/redis.sock
if [[ ! -S "$SOCKET" ]]; then
  echo "⚠️ Unix Socket not found"
  exit 1    
else 
  echo "✅ Unix Socket found"
  exit 0
fi
          ${{ # Redis configuration file example.
    #
    # Note that in order to read the configuration file, Redis must be
    # started with the file path as first argument:
    #
    # ./redis-server /path/to/redis.conf
    
    # Note on units: when memory size is needed, it is possible to specify
    # it in the usual form of 1k 5GB 4M and so forth:
    #
    # 1k => 1000 bytes
    # 1kb => 1024 bytes
    # 1m => 1000000 bytes
    # 1mb => 1024*1024 bytes
    # 1g => 1000000000 bytes
    # 1gb => 1024*1024*1024 bytes
    #
    # units are case insensitive so 1GB 1Gb 1gB are all the same.
    
    ################################## INCLUDES ###################################
    
    # Include one or more other config files here.  This is useful if you
    # have a standard template that goes to all Redis servers but also need
    # to customize a few per-server settings.  Include files can include
    # other files, so use this wisely.
    #
    # Note that option "include" won't be rewritten by command "CONFIG REWRITE"
    # from admin or Redis Sentinel. Since Redis always uses the last processed
    # line as value of a configuration directive, you'd better put includes
    # at the beginning of this file to avoid overwriting config change at runtime.
    #
    # If instead you are interested in using includes to override configuration
    # options, it is better to use include as the last line.
    #
    # Included paths may contain wildcards. All files matching the wildcards will
    # be included in alphabetical order.
    # Note that if an include path contains a wildcards but no files match it when
    # the server is started, the include statement will be ignored and no error will
    # be emitted.  It is safe, therefore, to include wildcard files from empty
    # directories.
    #
    # include /path/to/local.conf
    # include /path/to/other.conf
    # include /path/to/fragments/*.conf
    #
    
    ################################## MODULES #####################################
    
    # Load modules at startup. If the server is not able to load modules
    # it will abort. It is possible to use multiple loadmodule directives.
    #
    # loadmodule /path/to/my_module.so
    # loadmodule /path/to/other_module.so
    
    ################################## NETWORK #####################################
    
    # By default, if no "bind" configuration directive is specified, Redis listens
    # for connections from all available network interfaces on the host machine.
    # It is possible to listen to just one or multiple selected interfaces using
    # the "bind" configuration directive, followed by one or more IP addresses.
    # Each address can be prefixed by "-", which means that redis will not fail to
    # start if the address is not available. Being not available only refers to
    # addresses that does not correspond to any network interface. Addresses that
    # are already in use will always fail, and unsupported protocols will always BE
    # silently skipped.
    #
    # Examples:
    #
    # bind 192.168.1.100 10.0.0.1     # listens on two specific IPv4 addresses
    # bind 127.0.0.1 ::1              # listens on loopback IPv4 and IPv6
    # bind * -::*                     # like the default, all available interfaces
    #
    # ~~~ WARNING ~~~ If the computer running Redis is directly exposed to the
    # internet, binding to all the interfaces is dangerous and will expose the
    # instance to everybody on the internet. So by default we uncomment the
    # following bind directive, that will force Redis to listen only on the
    # IPv4 and IPv6 (if available) loopback interface addresses (this means Redis
    # will only be able to accept client connections from the same host that it is
    # running on).
    #
    # IF YOU ARE SURE YOU WANT YOUR INSTANCE TO LISTEN TO ALL THE INTERFACES
    # COMMENT OUT THE FOLLOWING LINE.
    #
    # You will also need to set a password unless you explicitly disable protected
    # mode.
    # ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    #bind 127.0.0.1 -::1
    #bind * -::*
    
    # By default, outgoing connections (from replica to master, from Sentinel to
    # instances, cluster bus, etc.) are not bound to a specific local address. In
    # most cases, this means the operating system will handle that based on routing
    # and the interface through which the connection goes out.
    #
    # Using bind-source-addr it is possible to configure a specific address to bind
    # to, which may also affect how the connection gets routed.
    #
    # Example:
    #
    # bind-source-addr 10.0.0.1
    
    # Protected mode is a layer of security protection, in order to avoid that
    # Redis instances left open on the internet are accessed and exploited.
    #
    # When protected mode is on and the default user has no password, the server
    # only accepts local connections from the IPv4 address (127.0.0.1), IPv6 address
    # (::1) or Unix domain sockets.
    #
    # By default protected mode is enabled. You should disable it only if
    # you are sure you want clients from other hosts to connect to Redis
    # even if no authentication is configured.
    protected-mode yes
    
    # Redis uses default hardened security configuration directives to reduce the
    # attack surface on innocent users. Therefore, several sensitive configuration
    # directives are immutable, and some potentially-dangerous commands are blocked.
    #
    # Configuration directives that control files that Redis writes to (e.g., 'dir'
    # and 'dbfilename') and that aren't usually modified during runtime
    # are protected by making them immutable.
    #
    # Commands that can increase the attack surface of Redis and that aren't usually
    # called by users are blocked by default.
    #
    # These can be exposed to either all connections or just local ones by setting
    # each of the configs listed below to either of these values:
    #
    # no    - Block for any connection (remain immutable)
    # yes   - Allow for any connection (no protection)
    # local - Allow only for local connections. Ones originating from the
    #         IPv4 address (127.0.0.1), IPv6 address (::1) or Unix domain sockets.
    #
    # enable-protected-configs no
    # enable-debug-command no
    # enable-module-command no
    
    # Accept connections on the specified port, default is 6379 (IANA #815344).
    # If port 0 is specified Redis will not listen on a TCP socket.
    #port 6379
    port 0
    
    # TCP listen() backlog.
    #
    # In high requests-per-second environments you need a high backlog in order
    # to avoid slow clients connection issues. Note that the Linux kernel
    # will silently truncate it to the value of /proc/sys/net/core/somaxconn so
    # make sure to raise both the value of somaxconn and tcp_max_syn_backlog
    # in order to get the desired effect.
    tcp-backlog 4096
    
    # Unix socket.
    #
    # Specify the path for the Unix socket that will be used to listen for
    # incoming connections. There is no default, so Redis will not listen
    # on a unix socket when not specified.
    #
    unixsocket /usr/local/unbound/cachedb.d/redis.sock
    unixsocketperm 770
    
    # Close the connection after a client is idle for N seconds (0 to disable)
    timeout 0
    
    # TCP keepalive.
    #
    # If non-zero, use SO_KEEPALIVE to send TCP ACKs to clients in absence
    # of communication. This is useful for two reasons:
    #
    # 1) Detect dead peers.
    # 2) Force network equipment in the middle to consider the connection to be
    #    alive.
    #
    # On Linux, the specified value (in seconds) is the period used to send ACKs.
    # Note that to close the connection the double of the time is needed.
    # On other kernels the period depends on the kernel configuration.
    #
    # A reasonable value for this option is 300 seconds, which is the new
    # Redis default starting with Redis 3.2.1.
    tcp-keepalive 300
    
    # Apply OS-specific mechanism to mark the listening socket with the specified
    # ID, to support advanced routing and filtering capabilities.
    #
    # On Linux, the ID represents a connection mark.
    # On FreeBSD, the ID represents a socket cookie ID.
    # On OpenBSD, the ID represents a route table ID.
    #
    # The default value is 0, which implies no marking is required.
    # socket-mark-id 0
    
    ################################# TLS/SSL #####################################
    
    # By default, TLS/SSL is disabled. To enable it, the "tls-port" configuration
    # directive can be used to define TLS-listening ports. To enable TLS on the
    # default port, use:
    #
    # port 0
    # tls-port 6379
    
    # Configure a X.509 certificate and private key to use for authenticating the
    # server to connected clients, masters or cluster peers.  These files should be
    # PEM formatted.
    #
    # tls-cert-file redis.crt
    # tls-key-file redis.key
    #
    # If the key file is encrypted using a passphrase, it can be included here
    # as well.
    #
    # tls-key-file-pass secret
    
    # Normally Redis uses the same certificate for both server functions (accepting
    # connections) and client functions (replicating from a master, establishing
    # cluster bus connections, etc.).
    #
    # Sometimes certificates are issued with attributes that designate them as
    # client-only or server-only certificates. In that case it may be desired to use
    # different certificates for incoming (server) and outgoing (client)
    # connections. To do that, use the following directives:
    #
    # tls-client-cert-file client.crt
    # tls-client-key-file client.key
    #
    # If the key file is encrypted using a passphrase, it can be included here
    # as well.
    #
    # tls-client-key-file-pass secret
    
    # Configure a DH parameters file to enable Diffie-Hellman (DH) key exchange,
    # required by older versions of OpenSSL (<3.0). Newer versions do not require
    # this configuration and recommend against it.
    #
    # tls-dh-params-file redis.dh
    
    # Configure a CA certificate(s) bundle or directory to authenticate TLS/SSL
    # clients and peers.  Redis requires an explicit configuration of at least one
    # of these, and will not implicitly use the system wide configuration.
    #
    # tls-ca-cert-file ca.crt
    # tls-ca-cert-dir /etc/ssl/certs
    
    # By default, clients (including replica servers) on a TLS port are required
    # to authenticate using valid client side certificates.
    #
    # If "no" is specified, client certificates are not required and not accepted.
    # If "optional" is specified, client certificates are accepted and must be
    # valid if provided, but are not required.
    #
    # tls-auth-clients no
    # tls-auth-clients optional
    
    # By default, a Redis replica does not attempt to establish a TLS connection
    # with its master.
    #
    # Use the following directive to enable TLS on replication links.
    #
    # tls-replication yes
    
    # By default, the Redis Cluster bus uses a plain TCP connection. To enable
    # TLS for the bus protocol, use the following directive:
    #
    # tls-cluster yes
    
    # By default, only TLSv1.2 and TLSv1.3 are enabled and it is highly recommended
    # that older formally deprecated versions are kept disabled to reduce the attack surface.
    # You can explicitly specify TLS versions to support.
    # Allowed values are case insensitive and include "TLSv1", "TLSv1.1", "TLSv1.2",
    # "TLSv1.3" (OpenSSL >= 1.1.1) or any combination.
    # To enable only TLSv1.2 and TLSv1.3, use:
    #
    # tls-protocols "TLSv1.2 TLSv1.3"
    
    # Configure allowed ciphers.  See the ciphers(1ssl) manpage for more information
    # about the syntax of this string.
    #
    # Note: this configuration applies only to <= TLSv1.2.
    #
    # tls-ciphers DEFAULT:!MEDIUM
    
    # Configure allowed TLSv1.3 ciphersuites.  See the ciphers(1ssl) manpage for more
    # information about the syntax of this string, and specifically for TLSv1.3
    # ciphersuites.
    #
    # tls-ciphersuites TLS_CHACHA20_POLY1305_SHA256
    
    # When choosing a cipher, use the server's preference instead of the client
    # preference. By default, the server follows the client's preference.
    #
    # tls-prefer-server-ciphers yes
    
    # By default, TLS session caching is enabled to allow faster and less expensive
    # reconnections by clients that support it. Use the following directive to disable
    # caching.
    #
    # tls-session-caching no
    
    # Change the default number of TLS sessions cached. A zero value sets the cache
    # to unlimited size. The default size is 20480.
    #
    # tls-session-cache-size 5000
    
    # Change the default timeout of cached TLS sessions. The default timeout is 300
    # seconds.
    #
    # tls-session-cache-timeout 60
    
    ################################# GENERAL #####################################
    
    # By default Redis does not run as a daemon. Use 'yes' if you need it.
    # Note that Redis will write a pid file in /var/run/redis.pid when daemonized.
    # When Redis is supervised by upstart or systemd, this parameter has no impact.
    daemonize no
    
    # If you run Redis from upstart or systemd, Redis can interact with your
    # supervision tree. Options:
    #   supervised no      - no supervision interaction
    #   supervised upstart - signal upstart by putting Redis into SIGSTOP mode
    #                        requires "expect stop" in your upstart job config
    #   supervised systemd - signal systemd by writing READY=1 to $NOTIFY_SOCKET
    #                        on startup, and updating Redis status on a regular
    #                        basis.
    #   supervised auto    - detect upstart or systemd method based on
    #                        UPSTART_JOB or NOTIFY_SOCKET environment variables
    # Note: these supervision methods only signal "process is ready."
    #       They do not enable continuous pings back to your supervisor.
    #
    # The default is "no". To run under upstart/systemd, you can simply uncomment
    # the line below:
    #
    # supervised auto
    
    # If a pid file is specified, Redis writes it where specified at startup
    # and removes it at exit.
    #
    # When the server runs non daemonized, no pid file is created if none is
    # specified in the configuration. When the server is daemonized, the pid file
    # is used even if not specified, defaulting to "/var/run/redis.pid".
    #
    # Creating a pid file is best effort: if Redis is not able to create it
    # nothing bad happens, the server will start and run normally.
    #
    # Note that on modern Linux systems "/run/redis.pid" is more conforming
    # and should be used instead.
    #pidfile /run/redis.pid
    
    # Specify the server verbosity level.
    # This can be one of:
    # debug (a lot of information, useful for development/testing)
    # verbose (many rarely useful info, but not a mess like the debug level)
    # notice (moderately verbose, what you want in production probably)
    # warning (only very important / critical messages are logged)
    # nothing (nothing is logged)
    loglevel notice
    
    # Specify the log file name. Also the empty string can be used to force
    # Redis to log on the standard output. Note that if you use standard
    # output for logging but daemonize, logs will be sent to /dev/null
    logfile ""
    
    # To enable logging to the system logger, just set 'syslog-enabled' to yes,
    # and optionally update the other syslog parameters to suit your needs.
    # syslog-enabled no
    
    # Specify the syslog identity.
    # syslog-ident redis
    
    # Specify the syslog facility. Must be USER or between LOCAL0-LOCAL7.
    # syslog-facility local0
    
    # To disable the built in crash log, which will possibly produce cleaner core
    # dumps when they are needed, uncomment the following:
    #
    # crash-log-enabled no
    
    # To disable the fast memory check that's run as part of the crash log, which
    # will possibly let redis terminate sooner, uncomment the following:
    #
    # crash-memcheck-enabled no
    
    # Set the number of databases. The default database is DB 0, you can select
    # a different one on a per-connection basis using SELECT <dbid> where
    # dbid is a number between 0 and 'databases'-1
    databases 16
    
    # By default Redis shows an ASCII art logo only when started to log to the
    # standard output and if the standard output is a TTY and syslog logging is
    # disabled. Basically this means that normally a logo is displayed only in
    # interactive sessions.
    #
    # However it is possible to force the pre-4.0 behavior and always show a
    # ASCII art logo in startup logs by setting the following option to yes.
    always-show-logo no
    
    # By default, Redis modifies the process title (as seen in 'top' and 'ps') to
    # provide some runtime information. It is possible to disable this and leave
    # the process name as executed by setting the following to no.
    set-proc-title yes
    
    # When changing the process title, Redis uses the following template to construct
    # the modified title.
    #
    # Template variables are specified in curly brackets. The following variables are
    # supported:
    #
    # {title}           Name of process as executed if parent, or type of child process.
    # {listen-addr}     Bind address or '*' followed by TCP or TLS port listening on, or
    #                   Unix socket if only that's available.
    # {server-mode}     Special mode, i.e. "[sentinel]" or "[cluster]".
    # {port}            TCP port listening on, or 0.
    # {tls-port}        TLS port listening on, or 0.
    # {unixsocket}      Unix domain socket listening on, or "".
    # {config-file}     Name of configuration file used.
    #
    proc-title-template "{title} {listen-addr} {server-mode}"
    
    # Set the local environment which is used for string comparison operations, and 
    # also affect the performance of Lua scripts. Empty String indicates the locale 
    # is derived from the environment variables.
    locale-collate ""
    
    ################################ SNAPSHOTTING  ################################
    
    # Save the DB to disk.
    #
    # save <seconds> <changes> [<seconds> <changes> ...]
    #
    # Redis will save the DB if the given number of seconds elapsed and it
    # surpassed the given number of write operations against the DB.
    #
    # Snapshotting can be completely disabled with a single empty string argument
    # as in following example:
    #
    # save ""
    #
    # Unless specified otherwise, by default Redis will save the DB:
    #   * After 3600 seconds (an hour) if at least 1 change was performed
    #   * After 300 seconds (5 minutes) if at least 100 changes were performed
    #   * After 60 seconds if at least 10000 changes were performed
    #
    # You can set these explicitly by uncommenting the following line.
    #
    # save 3600 1 300 100 60 10000
    
    # By default Redis will stop accepting writes if RDB snapshots are enabled
    # (at least one save point) and the latest background save failed.
    # This will make the user aware (in a hard way) that data is not persisting
    # on disk properly, otherwise chances are that no one will notice and some
    # disaster will happen.
    #
    # If the background saving process will start working again Redis will
    # automatically allow writes again.
    #
    # However if you have setup your proper monitoring of the Redis server
    # and persistence, you may want to disable this feature so that Redis will
    # continue to work as usual even if there are problems with disk,
    # permissions, and so forth.
    stop-writes-on-bgsave-error yes
    
    # Compress string objects using LZF when dump .rdb databases?
    # By default compression is enabled as it's almost always a win.
    # If you want to save some CPU in the saving child set it to 'no' but
    # the dataset will likely be bigger if you have compressible values or keys.
    rdbcompression yes
    
    # Since version 5 of RDB a CRC64 checksum is placed at the end of the file.
    # This makes the format more resistant to corruption but there is a performance
    # hit to pay (around 10%) when saving and loading RDB files, so you can disable it
    # for maximum performances.
    #
    # RDB files created with checksum disabled have a checksum of zero that will
    # tell the loading code to skip the check.
    rdbchecksum yes
    
    # Enables or disables full sanitization checks for ziplist and listpack etc when
    # loading an RDB or RESTORE payload. This reduces the chances of a assertion or
    # crash later on while processing commands.
    # Options:
    #   no         - Never perform full sanitization
    #   yes        - Always perform full sanitization
    #   clients    - Perform full sanitization only for user connections.
    #                Excludes: RDB files, RESTORE commands received from the master
    #                connection, and client connections which have the
    #                skip-sanitize-payload ACL flag.
    # The default should be 'clients' but since it currently affects cluster
    # resharding via MIGRATE, it is temporarily set to 'no' by default.
    #
    # sanitize-dump-payload no
    
    # The filename where to dump the DB
    dbfilename dump.rdb
    
    # Remove RDB files used by replication in instances without persistence
    # enabled. By default this option is disabled, however there are environments
    # where for regulations or other security concerns, RDB files persisted on
    # disk by masters in order to feed replicas, or stored on disk by replicas
    # in order to load them for the initial synchronization, should be deleted
    # ASAP. Note that this option ONLY WORKS in instances that have both AOF
    # and RDB persistence disabled, otherwise is completely ignored.
    #
    # An alternative (and sometimes better) way to obtain the same effect is
    # to use diskless replication on both master and replicas instances. However
    # in the case of replicas, diskless is not always an option.
    rdb-del-sync-files no
    
    # The working directory.
    #
    # The DB will be written inside this directory, with the filename specified
    # above using the 'dbfilename' configuration directive.
    #
    # The Append Only File will also be created inside this directory.
    #
    # Note that you must specify a directory here, not a file name.
    dir ./
    
    ################################# REPLICATION #################################
    
    # Master-Replica replication. Use replicaof to make a Redis instance a copy of
    # another Redis server. A few things to understand ASAP about Redis replication.
    #
    #   +------------------+      +---------------+
    #   |      Master      | ---> |    Replica    |
    #   | (receive writes) |      |  (exact copy) |
    #   +------------------+      +---------------+
    #
    # 1) Redis replication is asynchronous, but you can configure a master to
    #    stop accepting writes if it appears to be not connected with at least
    #    a given number of replicas.
    # 2) Redis replicas are able to perform a partial resynchronization with the
    #    master if the replication link is lost for a relatively small amount of
    #    time. You may want to configure the replication backlog size (see the next
    #    sections of this file) with a sensible value depending on your needs.
    # 3) Replication is automatic and does not need user intervention. After a
    #    network partition replicas automatically try to reconnect to masters
    #    and resynchronize with them.
    #
    # replicaof <masterip> <masterport>
    
    # If the master is password protected (using the "requirepass" configuration
    # directive below) it is possible to tell the replica to authenticate before
    # starting the replication synchronization process, otherwise the master will
    # refuse the replica request.
    #
    # masterauth <master-password>
    #
    # However this is not enough if you are using Redis ACLs (for Redis version
    # 6 or greater), and the default user is not capable of running the PSYNC
    # command and/or other commands needed for replication. In this case it's
    # better to configure a special user to use with replication, and specify the
    # masteruser configuration as such:
    #
    # masteruser <username>
    #
    # When masteruser is specified, the replica will authenticate against its
    # master using the new AUTH form: AUTH <username> <password>.
    
    # When a replica loses its connection with the master, or when the replication
    # is still in progress, the replica can act in two different ways:
    #
    # 1) if replica-serve-stale-data is set to 'yes' (the default) the replica will
    #    still reply to client requests, possibly with out of date data, or the
    #    data set may just be empty if this is the first synchronization.
    #
    # 2) If replica-serve-stale-data is set to 'no' the replica will reply with error
    #    "MASTERDOWN Link with MASTER is down and replica-serve-stale-data is set to 'no'"
    #    to all data access commands, excluding commands such as:
    #    INFO, REPLICAOF, AUTH, SHUTDOWN, REPLCONF, ROLE, CONFIG, SUBSCRIBE,
    #    UNSUBSCRIBE, PSUBSCRIBE, PUNSUBSCRIBE, PUBLISH, PUBSUB, COMMAND, POST,
    #    HOST and LATENCY.
    #
    replica-serve-stale-data yes
    
    # You can configure a replica instance to accept writes or not. Writing against
    # a replica instance may be useful to store some ephemeral data (because data
    # written on a replica will be easily deleted after resync with the master) but
    # may also cause problems if clients are writing to it because of a
    # misconfiguration.
    #
    # Since Redis 2.6 by default replicas are read-only.
    #
    # Note: read only replicas are not designed to be exposed to untrusted clients
    # on the internet. It's just a protection layer against misuse of the instance.
    # Still a read only replica exports by default all the administrative commands
    # such as CONFIG, DEBUG, and so forth. To a limited extent you can improve
    # security of read only replicas using 'rename-command' to shadow all the
    # administrative / dangerous commands.
    replica-read-only yes
    
    # Replication SYNC strategy: disk or socket.
    #
    # New replicas and reconnecting replicas that are not able to continue the
    # replication process just receiving differences, need to do what is called a
    # "full synchronization". An RDB file is transmitted from the master to the
    # replicas.
    #
    # The transmission can happen in two different ways:
    #
    # 1) Disk-backed: The Redis master creates a new process that writes the RDB
    #                 file on disk. Later the file is transferred by the parent
    #                 process to the replicas incrementally.
    # 2) Diskless: The Redis master creates a new process that directly writes the
    #              RDB file to replica sockets, without touching the disk at all.
    #
    # With disk-backed replication, while the RDB file is generated, more replicas
    # can be queued and served with the RDB file as soon as the current child
    # producing the RDB file finishes its work. With diskless replication instead
    # once the transfer starts, new replicas arriving will be queued and a new
    # transfer will start when the current one terminates.
    #
    # When diskless replication is used, the master waits a configurable amount of
    # time (in seconds) before starting the transfer in the hope that multiple
    # replicas will arrive and the transfer can be parallelized.
    #
    # With slow disks and fast (large bandwidth) networks, diskless replication
    # works better.
    repl-diskless-sync yes
    
    # When diskless replication is enabled, it is possible to configure the delay
    # the server waits in order to spawn the child that transfers the RDB via socket
    # to the replicas.
    #
    # This is important since once the transfer starts, it is not possible to serve
    # new replicas arriving, that will be queued for the next RDB transfer, so the
    # server waits a delay in order to let more replicas arrive.
    #
    # The delay is specified in seconds, and by default is 5 seconds. To disable
    # it entirely just set it to 0 seconds and the transfer will start ASAP.
    repl-diskless-sync-delay 5
    
    # When diskless replication is enabled with a delay, it is possible to let
    # the replication start before the maximum delay is reached if the maximum
    # number of replicas expected have connected. Default of 0 means that the
    # maximum is not defined and Redis will wait the full delay.
    repl-diskless-sync-max-replicas 0
    
    # -----------------------------------------------------------------------------
    # WARNING: Since in this setup the replica does not immediately store an RDB on
    # disk, it may cause data loss during failovers. RDB diskless load + Redis
    # modules not handling I/O reads may cause Redis to abort in case of I/O errors
    # during the initial synchronization stage with the master.
    # -----------------------------------------------------------------------------
    #
    # Replica can load the RDB it reads from the replication link directly from the
    # socket, or store the RDB to a file and read that file after it was completely
    # received from the master.
    #
    # In many cases the disk is slower than the network, and storing and loading
    # the RDB file may increase replication time (and even increase the master's
    # Copy on Write memory and replica buffers).
    # However, when parsing the RDB file directly from the socket, in order to avoid
    # data loss it's only safe to flush the current dataset when the new dataset is
    # fully loaded in memory, resulting in higher memory usage.
    # For this reason we have the following options:
    #
    # "disabled"    - Don't use diskless load (store the rdb file to the disk first)
    # "swapdb"      - Keep current db contents in RAM while parsing the data directly
    #                 from the socket. Replicas in this mode can keep serving current
    #                 dataset while replication is in progress, except for cases where
    #                 they can't recognize master as having a data set from same
    #                 replication history.
    #                 Note that this requires sufficient memory, if you don't have it,
    #                 you risk an OOM kill.
    # "on-empty-db" - Use diskless load only when current dataset is empty. This is 
    #                 safer and avoid having old and new dataset loaded side by side
    #                 during replication.
    repl-diskless-load disabled
    
    # Master send PINGs to its replicas in a predefined interval. It's possible to
    # change this interval with the repl_ping_replica_period option. The default
    # value is 10 seconds.
    #
    # repl-ping-replica-period 10
    
    # The following option sets the replication timeout for:
    #
    # 1) Bulk transfer I/O during SYNC, from the point of view of replica.
    # 2) Master timeout from the point of view of replicas (data, pings).
    # 3) Replica timeout from the point of view of masters (REPLCONF ACK pings).
    #
    # It is important to make sure that this value is greater than the value
    # specified for repl-ping-replica-period otherwise a timeout will be detected
    # every time there is low traffic between the master and the replica. The default
    # value is 60 seconds.
    #
    # repl-timeout 60
    
    # Disable TCP_NODELAY on the replica socket after SYNC?
    #
    # If you select "yes" Redis will use a smaller number of TCP packets and
    # less bandwidth to send data to replicas. But this can add a delay for
    # the data to appear on the replica side, up to 40 milliseconds with
    # Linux kernels using a default configuration.
    #
    # If you select "no" the delay for data to appear on the replica side will
    # be reduced but more bandwidth will be used for replication.
    #
    # By default we optimize for low latency, but in very high traffic conditions
    # or when the master and replicas are many hops away, turning this to "yes" may
    # be a good idea.
    repl-disable-tcp-nodelay no
    
    # Set the replication backlog size. The backlog is a buffer that accumulates
    # replica data when replicas are disconnected for some time, so that when a
    # replica wants to reconnect again, often a full resync is not needed, but a
    # partial resync is enough, just passing the portion of data the replica
    # missed while disconnected.
    #
    # The bigger the replication backlog, the longer the replica can endure the
    # disconnect and later be able to perform a partial resynchronization.
    #
    # The backlog is only allocated if there is at least one replica connected.
    #
    # repl-backlog-size 1mb
    
    # After a master has no connected replicas for some time, the backlog will be
    # freed. The following option configures the amount of seconds that need to
    # elapse, starting from the time the last replica disconnected, for the backlog
    # buffer to be freed.
    #
    # Note that replicas never free the backlog for timeout, since they may be
    # promoted to masters later, and should be able to correctly "partially
    # resynchronize" with other replicas: hence they should always accumulate backlog.
    #
    # A value of 0 means to never release the backlog.
    #
    # repl-backlog-ttl 3600
    
    # The replica priority is an integer number published by Redis in the INFO
    # output. It is used by Redis Sentinel in order to select a replica to promote
    # into a master if the master is no longer working correctly.
    #
    # A replica with a low priority number is considered better for promotion, so
    # for instance if there are three replicas with priority 10, 100, 25 Sentinel
    # will pick the one with priority 10, that is the lowest.
    #
    # However a special priority of 0 marks the replica as not able to perform the
    # role of master, so a replica with priority of 0 will never be selected by
    # Redis Sentinel for promotion.
    #
    # By default the priority is 100.
    replica-priority 100
    
    # The propagation error behavior controls how Redis will behave when it is
    # unable to handle a command being processed in the replication stream from a master
    # or processed while reading from an AOF file. Errors that occur during propagation
    # are unexpected, and can cause data inconsistency. However, there are edge cases
    # in earlier versions of Redis where it was possible for the server to replicate or persist
    # commands that would fail on future versions. For this reason the default behavior
    # is to ignore such errors and continue processing commands.
    #
    # If an application wants to ensure there is no data divergence, this configuration
    # should be set to 'panic' instead. The value can also be set to 'panic-on-replicas'
    # to only panic when a replica encounters an error on the replication stream. One of
    # these two panic values will become the default value in the future once there are
    # sufficient safety mechanisms in place to prevent false positive crashes.
    #
    # propagation-error-behavior ignore
    
    # Replica ignore disk write errors controls the behavior of a replica when it is
    # unable to persist a write command received from its master to disk. By default,
    # this configuration is set to 'no' and will crash the replica in this condition.
    # It is not recommended to change this default, however in order to be compatible
    # with older versions of Redis this config can be toggled to 'yes' which will just
    # log a warning and execute the write command it got from the master.
    #
    # replica-ignore-disk-write-errors no
    
    # -----------------------------------------------------------------------------
    # By default, Redis Sentinel includes all replicas in its reports. A replica
    # can be excluded from Redis Sentinel's announcements. An unannounced replica
    # will be ignored by the 'sentinel replicas <master>' command and won't be
    # exposed to Redis Sentinel's clients.
    #
    # This option does not change the behavior of replica-priority. Even with
    # replica-announced set to 'no', the replica can be promoted to master. To
    # prevent this behavior, set replica-priority to 0.
    #
    # replica-announced yes
    
    # It is possible for a master to stop accepting writes if there are less than
    # N replicas connected, having a lag less or equal than M seconds.
    #
    # The N replicas need to be in "online" state.
    #
    # The lag in seconds, that must be <= the specified value, is calculated from
    # the last ping received from the replica, that is usually sent every second.
    #
    # This option does not GUARANTEE that N replicas will accept the write, but
    # will limit the window of exposure for lost writes in case not enough replicas
    # are available, to the specified number of seconds.
    #
    # For example to require at least 3 replicas with a lag <= 10 seconds use:
    #
    # min-replicas-to-write 3
    # min-replicas-max-lag 10
    #
    # Setting one or the other to 0 disables the feature.
    #
    # By default min-replicas-to-write is set to 0 (feature disabled) and
    # min-replicas-max-lag is set to 10.
    
    # A Redis master is able to list the address and port of the attached
    # replicas in different ways. For example the "INFO replication" section
    # offers this information, which is used, among other tools, by
    # Redis Sentinel in order to discover replica instances.
    # Another place where this info is available is in the output of the
    # "ROLE" command of a master.
    #
    # The listed IP address and port normally reported by a replica is
    # obtained in the following way:
    #
    #   IP: The address is auto detected by checking the peer address
    #   of the socket used by the replica to connect with the master.
    #
    #   Port: The port is communicated by the replica during the replication
    #   handshake, and is normally the port that the replica is using to
    #   listen for connections.
    #
    # However when port forwarding or Network Address Translation (NAT) is
    # used, the replica may actually be reachable via different IP and port
    # pairs. The following two options can be used by a replica in order to
    # report to its master a specific set of IP and port, so that both INFO
    # and ROLE will report those values.
    #
    # There is no need to use both the options if you need to override just
    # the port or the IP address.
    #
    # replica-announce-ip 5.5.5.5
    # replica-announce-port 1234
    
    ############################### KEYS TRACKING #################################
    
    # Redis implements server assisted support for client side caching of values.
    # This is implemented using an invalidation table that remembers, using
    # a radix key indexed by key name, what clients have which keys. In turn
    # this is used in order to send invalidation messages to clients. Please
    # check this page to understand more about the feature:
    #
    #   https://redis.io/topics/client-side-caching
    #
    # When tracking is enabled for a client, all the read only queries are assumed
    # to be cached: this will force Redis to store information in the invalidation
    # table. When keys are modified, such information is flushed away, and
    # invalidation messages are sent to the clients. However if the workload is
    # heavily dominated by reads, Redis could use more and more memory in order
    # to track the keys fetched by many clients.
    #
    # For this reason it is possible to configure a maximum fill value for the
    # invalidation table. By default it is set to 1M of keys, and once this limit
    # is reached, Redis will start to evict keys in the invalidation table
    # even if they were not modified, just to reclaim memory: this will in turn
    # force the clients to invalidate the cached values. Basically the table
    # maximum size is a trade off between the memory you want to spend server
    # side to track information about who cached what, and the ability of clients
    # to retain cached objects in memory.
    #
    # If you set the value to 0, it means there are no limits, and Redis will
    # retain as many keys as needed in the invalidation table.
    # In the "stats" INFO section, you can find information about the number of
    # keys in the invalidation table at every given moment.
    #
    # Note: when key tracking is used in broadcasting mode, no memory is used
    # in the server side so this setting is useless.
    #
    # tracking-table-max-keys 1000000
    
    ################################## SECURITY ###################################
    
    # Warning: since Redis is pretty fast, an outside user can try up to
    # 1 million passwords per second against a modern box. This means that you
    # should use very strong passwords, otherwise they will be very easy to break.
    # Note that because the password is really a shared secret between the client
    # and the server, and should not be memorized by any human, the password
    # can be easily a long string from /dev/urandom or whatever, so by using a
    # long and unguessable password no brute force attack will be possible.
    
    # Redis ACL users are defined in the following format:
    #
    #   user <username> ... acl rules ...
    #
    # For example:
    #
    #   user worker +@list +@connection ~jobs:* on >ffa9203c493aa99
    #
    # The special username "default" is used for new connections. If this user
    # has the "nopass" rule, then new connections will be immediately authenticated
    # as the "default" user without the need of any password provided via the
    # AUTH command. Otherwise if the "default" user is not flagged with "nopass"
    # the connections will start in not authenticated state, and will require
    # AUTH (or the HELLO command AUTH option) in order to be authenticated and
    # start to work.
    #
    # The ACL rules that describe what a user can do are the following:
    #
    #  on           Enable the user: it is possible to authenticate as this user.
    #  off          Disable the user: it's no longer possible to authenticate
    #               with this user, however the already authenticated connections
    #               will still work.
    #  skip-sanitize-payload    RESTORE dump-payload sanitization is skipped.
    #  sanitize-payload         RESTORE dump-payload is sanitized (default).
    #  +<command>   Allow the execution of that command.
    #               May be used with `|` for allowing subcommands (e.g "+config|get")
    #  -<command>   Disallow the execution of that command.
    #               May be used with `|` for blocking subcommands (e.g "-config|set")
    #  +@<category> Allow the execution of all the commands in such category
    #               with valid categories are like @admin, @set, @sortedset, ...
    #               and so forth, see the full list in the server.c file where
    #               the Redis command table is described and defined.
    #               The special category @all means all the commands, but currently
    #               present in the server, and that will be loaded in the future
    #               via modules.
    #  +<command>|first-arg  Allow a specific first argument of an otherwise
    #                        disabled command. It is only supported on commands with
    #                        no sub-commands, and is not allowed as negative form
    #                        like -SELECT|1, only additive starting with "+". This
    #                        feature is deprecated and may be removed in the future.
    #  allcommands  Alias for +@all. Note that it implies the ability to execute
    #               all the future commands loaded via the modules system.
    #  nocommands   Alias for -@all.
    #  ~<pattern>   Add a pattern of keys that can be mentioned as part of
    #               commands. For instance ~* allows all the keys. The pattern
    #               is a glob-style pattern like the one of KEYS.
    #               It is possible to specify multiple patterns.
    # %R~<pattern>  Add key read pattern that specifies which keys can be read 
    #               from.
    # %W~<pattern>  Add key write pattern that specifies which keys can be
    #               written to. 
    #  allkeys      Alias for ~*
    #  resetkeys    Flush the list of allowed keys patterns.
    #  &<pattern>   Add a glob-style pattern of Pub/Sub channels that can be
    #               accessed by the user. It is possible to specify multiple channel
    #               patterns.
    #  allchannels  Alias for &*
    #  resetchannels            Flush the list of allowed channel patterns.
    #  ><password>  Add this password to the list of valid password for the user.
    #               For example >mypass will add "mypass" to the list.
    #               This directive clears the "nopass" flag (see later).
    #  <<password>  Remove this password from the list of valid passwords.
    #  nopass       All the set passwords of the user are removed, and the user
    #               is flagged as requiring no password: it means that every
    #               password will work against this user. If this directive is
    #               used for the default user, every new connection will be
    #               immediately authenticated with the default user without
    #               any explicit AUTH command required. Note that the "resetpass"
    #               directive will clear this condition.
    #  resetpass    Flush the list of allowed passwords. Moreover removes the
    #               "nopass" status. After "resetpass" the user has no associated
    #               passwords and there is no way to authenticate without adding
    #               some password (or setting it as "nopass" later).
    #  reset        Performs the following actions: resetpass, resetkeys, resetchannels,
    #               allchannels (if acl-pubsub-default is set), off, clearselectors, -@all.
    #               The user returns to the same state it has immediately after its creation.
    # (<options>)   Create a new selector with the options specified within the
    #               parentheses and attach it to the user. Each option should be 
    #               space separated. The first character must be ( and the last 
    #               character must be ).
    # clearselectors            Remove all of the currently attached selectors. 
    #                           Note this does not change the "root" user permissions,
    #                           which are the permissions directly applied onto the
    #                           user (outside the parentheses).
    #
    # ACL rules can be specified in any order: for instance you can start with
    # passwords, then flags, or key patterns. However note that the additive
    # and subtractive rules will CHANGE MEANING depending on the ordering.
    # For instance see the following example:
    #
    #   user alice on +@all -DEBUG ~* >somepassword
    #
    # This will allow "alice" to use all the commands with the exception of the
    # DEBUG command, since +@all added all the commands to the set of the commands
    # alice can use, and later DEBUG was removed. However if we invert the order
    # of two ACL rules the result will be different:
    #
    #   user alice on -DEBUG +@all ~* >somepassword
    #
    # Now DEBUG was removed when alice had yet no commands in the set of allowed
    # commands, later all the commands are added, so the user will be able to
    # execute everything.
    #
    # Basically ACL rules are processed left-to-right.
    #
    # The following is a list of command categories and their meanings:
    # * keyspace - Writing or reading from keys, databases, or their metadata 
    #     in a type agnostic way. Includes DEL, RESTORE, DUMP, RENAME, EXISTS, DBSIZE,
    #     KEYS, EXPIRE, TTL, FLUSHALL, etc. Commands that may modify the keyspace,
    #     key or metadata will also have `write` category. Commands that only read
    #     the keyspace, key or metadata will have the `read` category.
    # * read - Reading from keys (values or metadata). Note that commands that don't
    #     interact with keys, will not have either `read` or `write`.
    # * write - Writing to keys (values or metadata)
    # * admin - Administrative commands. Normal applications will never need to use
    #     these. Includes REPLICAOF, CONFIG, DEBUG, SAVE, MONITOR, ACL, SHUTDOWN, etc.
    # * dangerous - Potentially dangerous (each should be considered with care for
    #     various reasons). This includes FLUSHALL, MIGRATE, RESTORE, SORT, KEYS,
    #     CLIENT, DEBUG, INFO, CONFIG, SAVE, REPLICAOF, etc.
    # * connection - Commands affecting the connection or other connections.
    #     This includes AUTH, SELECT, COMMAND, CLIENT, ECHO, PING, etc.
    # * blocking - Potentially blocking the connection until released by another
    #     command.
    # * fast - Fast O(1) commands. May loop on the number of arguments, but not the
    #     number of elements in the key.
    # * slow - All commands that are not Fast.
    # * pubsub - PUBLISH / SUBSCRIBE related
    # * transaction - WATCH / MULTI / EXEC related commands.
    # * scripting - Scripting related.
    # * set - Data type: sets related.
    # * sortedset - Data type: zsets related.
    # * list - Data type: lists related.
    # * hash - Data type: hashes related.
    # * string - Data type: strings related.
    # * bitmap - Data type: bitmaps related.
    # * hyperloglog - Data type: hyperloglog related.
    # * geo - Data type: geo related.
    # * stream - Data type: streams related.
    #
    # For more information about ACL configuration please refer to
    # the Redis web site at https://redis.io/topics/acl
    
    # ACL LOG
    #
    # The ACL Log tracks failed commands and authentication events associated
    # with ACLs. The ACL Log is useful to troubleshoot failed commands blocked
    # by ACLs. The ACL Log is stored in memory. You can reclaim memory with
    # ACL LOG RESET. Define the maximum entry length of the ACL Log below.
    acllog-max-len 128
    
    # Using an external ACL file
    #
    # Instead of configuring users here in this file, it is possible to use
    # a stand-alone file just listing users. The two methods cannot be mixed:
    # if you configure users here and at the same time you activate the external
    # ACL file, the server will refuse to start.
    #
    # The format of the external ACL user file is exactly the same as the
    # format that is used inside redis.conf to describe users.
    #
    # aclfile /etc/redis/users.acl
    
    # IMPORTANT NOTE: starting with Redis 6 "requirepass" is just a compatibility
    # layer on top of the new ACL system. The option effect will be just setting
    # the password for the default user. Clients will still authenticate using
    # AUTH <password> as usually, or more explicitly with AUTH default <password>
    # if they follow the new protocol: both will work.
    #
    # The requirepass is not compatible with aclfile option and the ACL LOAD
    # command, these will cause requirepass to be ignored.
    #
    # requirepass foobared
    
    # New users are initialized with restrictive permissions by default, via the
    # equivalent of this ACL rule 'off resetkeys -@all'. Starting with Redis 6.2, it
    # is possible to manage access to Pub/Sub channels with ACL rules as well. The
    # default Pub/Sub channels permission if new users is controlled by the
    # acl-pubsub-default configuration directive, which accepts one of these values:
    #
    # allchannels: grants access to all Pub/Sub channels
    # resetchannels: revokes access to all Pub/Sub channels
    #
    # From Redis 7.0, acl-pubsub-default defaults to 'resetchannels' permission.
    #
    # acl-pubsub-default resetchannels
    
    # Command renaming (DEPRECATED).
    #
    # ------------------------------------------------------------------------
    # WARNING: avoid using this option if possible. Instead use ACLs to remove
    # commands from the default user, and put them only in some admin user you
    # create for administrative purposes.
    # ------------------------------------------------------------------------
    #
    # It is possible to change the name of dangerous commands in a shared
    # environment. For instance the CONFIG command may be renamed into something
    # hard to guess so that it will still be available for internal-use tools
    # but not available for general clients.
    #
    # Example:
    #
    # rename-command CONFIG b840fc02d524045429941cc15f59e41cb7be6c52
    #
    # It is also possible to completely kill a command by renaming it into
    # an empty string:
    #
    # rename-command CONFIG ""
    #
    # Please note that changing the name of commands that are logged into the
    # AOF file or transmitted to replicas may cause problems.
    
    ################################### CLIENTS ####################################
    
    # Set the max number of connected clients at the same time. By default
    # this limit is set to 10000 clients, however if the Redis server is not
    # able to configure the process file limit to allow for the specified limit
    # the max number of allowed clients is set to the current file limit
    # minus 32 (as Redis reserves a few file descriptors for internal uses).
    #
    # Once the limit is reached Redis will close all the new connections sending
    # an error 'max number of clients reached'.
    #
    # IMPORTANT: When Redis Cluster is used, the max number of connections is also
    # shared with the cluster bus: every node in the cluster will use two
    # connections, one incoming and another outgoing. It is important to size the
    # limit accordingly in case of very large clusters.
    #
    # maxclients 10000
    
    ############################## MEMORY MANAGEMENT ################################
    
    # Set a memory usage limit to the specified amount of bytes.
    # When the memory limit is reached Redis will try to remove keys
    # according to the eviction policy selected (see maxmemory-policy).
    #
    # If Redis can't remove keys according to the policy, or if the policy is
    # set to 'noeviction', Redis will start to reply with errors to commands
    # that would use more memory, like SET, LPUSH, and so on, and will continue
    # to reply to read-only commands like GET.
    #
    # This option is usually useful when using Redis as an LRU or LFU cache, or to
    # set a hard memory limit for an instance (using the 'noeviction' policy).
    #
    # WARNING: If you have replicas attached to an instance with maxmemory on,
    # the size of the output buffers needed to feed the replicas are subtracted
    # from the used memory count, so that network problems / resyncs will
    # not trigger a loop where keys are evicted, and in turn the output
    # buffer of replicas is full with DELs of keys evicted triggering the deletion
    # of more keys, and so forth until the database is completely emptied.
    #
    # In short... if you have replicas attached it is suggested that you set a lower
    # limit for maxmemory so that there is some free RAM on the system for replica
    # output buffers (but this is not needed if the policy is 'noeviction').
    #
    # maxmemory <bytes>
    
    # MAXMEMORY POLICY: how Redis will select what to remove when maxmemory
    # is reached. You can select one from the following behaviors:
    #
    # volatile-lru -> Evict using approximated LRU, only keys with an expire set.
    # allkeys-lru -> Evict any key using approximated LRU.
    # volatile-lfu -> Evict using approximated LFU, only keys with an expire set.
    # allkeys-lfu -> Evict any key using approximated LFU.
    # volatile-random -> Remove a random key having an expire set.
    # allkeys-random -> Remove a random key, any key.
    # volatile-ttl -> Remove the key with the nearest expire time (minor TTL)
    # noeviction -> Don't evict anything, just return an error on write operations.
    #
    # LRU means Least Recently Used
    # LFU means Least Frequently Used
    #
    # Both LRU, LFU and volatile-ttl are implemented using approximated
    # randomized algorithms.
    #
    # Note: with any of the above policies, when there are no suitable keys for
    # eviction, Redis will return an error on write operations that require
    # more memory. These are usually commands that create new keys, add data or
    # modify existing keys. A few examples are: SET, INCR, HSET, LPUSH, SUNIONSTORE,
    # SORT (due to the STORE argument), and EXEC (if the transaction includes any
    # command that requires memory).
    #
    # The default is:
    #
    maxmemory-policy lru
    
    # LRU, LFU and minimal TTL algorithms are not precise algorithms but approximated
    # algorithms (in order to save memory), so you can tune it for speed or
    # accuracy. By default Redis will check five keys and pick the one that was
    # used least recently, you can change the sample size using the following
    # configuration directive.
    #
    # The default of 5 produces good enough results. 10 Approximates very closely
    # true LRU but costs more CPU. 3 is faster but not very accurate.
    #
    # maxmemory-samples 5
    
    # Eviction processing is designed to function well with the default setting.
    # If there is an unusually large amount of write traffic, this value may need to
    # be increased.  Decreasing this value may reduce latency at the risk of
    # eviction processing effectiveness
    #   0 = minimum latency, 10 = default, 100 = process without regard to latency
    #
    # maxmemory-eviction-tenacity 10
    
    # Starting from Redis 5, by default a replica will ignore its maxmemory setting
    # (unless it is promoted to master after a failover or manually). It means
    # that the eviction of keys will be just handled by the master, sending the
    # DEL commands to the replica as keys evict in the master side.
    #
    # This behavior ensures that masters and replicas stay consistent, and is usually
    # what you want, however if your replica is writable, or you want the replica
    # to have a different memory setting, and you are sure all the writes performed
    # to the replica are idempotent, then you may change this default (but be sure
    # to understand what you are doing).
    #
    # Note that since the replica by default does not evict, it may end using more
    # memory than the one set via maxmemory (there are certain buffers that may
    # be larger on the replica, or data structures may sometimes take more memory
    # and so forth). So make sure you monitor your replicas and make sure they
    # have enough memory to never hit a real out-of-memory condition before the
    # master hits the configured maxmemory setting.
    #
    # replica-ignore-maxmemory yes
    
    # Redis reclaims expired keys in two ways: upon access when those keys are
    # found to be expired, and also in background, in what is called the
    # "active expire key". The key space is slowly and interactively scanned
    # looking for expired keys to reclaim, so that it is possible to free memory
    # of keys that are expired and will never be accessed again in a short time.
    #
    # The default effort of the expire cycle will try to avoid having more than
    # ten percent of expired keys still in memory, and will try to avoid consuming
    # more than 25% of total memory and to add latency to the system. However
    # it is possible to increase the expire "effort" that is normally set to
    # "1", to a greater value, up to the value "10". At its maximum value the
    # system will use more CPU, longer cycles (and technically may introduce
    # more latency), and will tolerate less already expired keys still present
    # in the system. It's a tradeoff between memory, CPU and latency.
    #
    # active-expire-effort 1
    
    ############################# LAZY FREEING ####################################
    
    # Redis has two primitives to delete keys. One is called DEL and is a blocking
    # deletion of the object. It means that the server stops processing new commands
    # in order to reclaim all the memory associated with an object in a synchronous
    # way. If the key deleted is associated with a small object, the time needed
    # in order to execute the DEL command is very small and comparable to most other
    # O(1) or O(log_N) commands in Redis. However if the key is associated with an
    # aggregated value containing millions of elements, the server can block for
    # a long time (even seconds) in order to complete the operation.
    #
    # For the above reasons Redis also offers non blocking deletion primitives
    # such as UNLINK (non blocking DEL) and the ASYNC option of FLUSHALL and
    # FLUSHDB commands, in order to reclaim memory in background. Those commands
    # are executed in constant time. Another thread will incrementally free the
    # object in the background as fast as possible.
    #
    # DEL, UNLINK and ASYNC option of FLUSHALL and FLUSHDB are user-controlled.
    # It's up to the design of the application to understand when it is a good
    # idea to use one or the other. However the Redis server sometimes has to
    # delete keys or flush the whole database as a side effect of other operations.
    # Specifically Redis deletes objects independently of a user call in the
    # following scenarios:
    #
    # 1) On eviction, because of the maxmemory and maxmemory policy configurations,
    #    in order to make room for new data, without going over the specified
    #    memory limit.
    # 2) Because of expire: when a key with an associated time to live (see the
    #    EXPIRE command) must be deleted from memory.
    # 3) Because of a side effect of a command that stores data on a key that may
    #    already exist. For example the RENAME command may delete the old key
    #    content when it is replaced with another one. Similarly SUNIONSTORE
    #    or SORT with STORE option may delete existing keys. The SET command
    #    itself removes any old content of the specified key in order to replace
    #    it with the specified string.
    # 4) During replication, when a replica performs a full resynchronization with
    #    its master, the content of the whole database is removed in order to
    #    load the RDB file just transferred.
    #
    # In all the above cases the default is to delete objects in a blocking way,
    # like if DEL was called. However you can configure each case specifically
    # in order to instead release memory in a non-blocking way like if UNLINK
    # was called, using the following configuration directives.
    
    lazyfree-lazy-eviction no
    lazyfree-lazy-expire no
    lazyfree-lazy-server-del no
    replica-lazy-flush no
    
    # It is also possible, for the case when to replace the user code DEL calls
    # with UNLINK calls is not easy, to modify the default behavior of the DEL
    # command to act exactly like UNLINK, using the following configuration
    # directive:
    
    lazyfree-lazy-user-del no
    
    # FLUSHDB, FLUSHALL, SCRIPT FLUSH and FUNCTION FLUSH support both asynchronous and synchronous
    # deletion, which can be controlled by passing the [SYNC|ASYNC] flags into the
    # commands. When neither flag is passed, this directive will be used to determine
    # if the data should be deleted asynchronously.
    
    lazyfree-lazy-user-flush no
    
    ################################ THREADED I/O #################################
    
    # Redis is mostly single threaded, however there are certain threaded
    # operations such as UNLINK, slow I/O accesses and other things that are
    # performed on side threads.
    #
    # Now it is also possible to handle Redis clients socket reads and writes
    # in different I/O threads. Since especially writing is so slow, normally
    # Redis users use pipelining in order to speed up the Redis performances per
    # core, and spawn multiple instances in order to scale more. Using I/O
    # threads it is possible to easily speedup two times Redis without resorting
    # to pipelining nor sharding of the instance.
    #
    # By default threading is disabled, we suggest enabling it only in machines
    # that have at least 4 or more cores, leaving at least one spare core.
    # Using more than 8 threads is unlikely to help much. We also recommend using
    # threaded I/O only if you actually have performance problems, with Redis
    # instances being able to use a quite big percentage of CPU time, otherwise
    # there is no point in using this feature.
    #
    # So for instance if you have a four cores boxes, try to use 2 or 3 I/O
    # threads, if you have a 8 cores, try to use 6 threads. In order to
    # enable I/O threads use the following configuration directive:
    #
    # io-threads 4
    #
    # Setting io-threads to 1 will just use the main thread as usual.
    # When I/O threads are enabled, we only use threads for writes, that is
    # to thread the write(2) syscall and transfer the client buffers to the
    # socket. However it is also possible to enable threading of reads and
    # protocol parsing using the following configuration directive, by setting
    # it to yes:
    #
    # io-threads-do-reads no
    #
    # Usually threading reads doesn't help much.
    #
    # NOTE 1: This configuration directive cannot be changed at runtime via
    # CONFIG SET. Also, this feature currently does not work when SSL is
    # enabled.
    #
    # NOTE 2: If you want to test the Redis speedup using redis-benchmark, make
    # sure you also run the benchmark itself in threaded mode, using the
    # --threads option to match the number of Redis threads, otherwise you'll not
    # be able to notice the improvements.
    
    ############################ KERNEL OOM CONTROL ##############################
    
    # On Linux, it is possible to hint the kernel OOM killer on what processes
    # should be killed first when out of memory.
    #
    # Enabling this feature makes Redis actively control the oom_score_adj value
    # for all its processes, depending on their role. The default scores will
    # attempt to have background child processes killed before all others, and
    # replicas killed before masters.
    #
    # Redis supports these options:
    #
    # no:       Don't make changes to oom-score-adj (default).
    # yes:      Alias to "relative" see below.
    # absolute: Values in oom-score-adj-values are written as is to the kernel.
    # relative: Values are used relative to the initial value of oom_score_adj when
    #           the server starts and are then clamped to a range of -1000 to 1000.
    #           Because typically the initial value is 0, they will often match the
    #           absolute values.
    oom-score-adj no
    
    # When oom-score-adj is used, this directive controls the specific values used
    # for master, replica and background child processes. Values range -2000 to
    # 2000 (higher means more likely to be killed).
    #
    # Unprivileged processes (not root, and without CAP_SYS_RESOURCE capabilities)
    # can freely increase their value, but not decrease it below its initial
    # settings. This means that setting oom-score-adj to "relative" and setting the
    # oom-score-adj-values to positive values will always succeed.
    oom-score-adj-values 0 200 800
    
    
    #################### KERNEL transparent hugepage CONTROL ######################
    
    # Usually the kernel Transparent Huge Pages control is set to "madvise" or
    # or "never" by default (/sys/kernel/mm/transparent_hugepage/enabled), in which
    # case this config has no effect. On systems in which it is set to "always",
    # redis will attempt to disable it specifically for the redis process in order
    # to avoid latency problems specifically with fork(2) and CoW.
    # If for some reason you prefer to keep it enabled, you can set this config to
    # "no" and the kernel global to "always".
    
    disable-thp yes
    
    ############################## APPEND ONLY MODE ###############################
    
    # By default Redis asynchronously dumps the dataset on disk. This mode is
    # good enough in many applications, but an issue with the Redis process or
    # a power outage may result into a few minutes of writes lost (depending on
    # the configured save points).
    #
    # The Append Only File is an alternative persistence mode that provides
    # much better durability. For instance using the default data fsync policy
    # (see later in the config file) Redis can lose just one second of writes in a
    # dramatic event like a server power outage, or a single write if something
    # wrong with the Redis process itself happens, but the operating system is
    # still running correctly.
    #
    # AOF and RDB persistence can be enabled at the same time without problems.
    # If the AOF is enabled on startup Redis will load the AOF, that is the file
    # with the better durability guarantees.
    #
    # Note that changing this value in a config file of an existing database and
    # restarting the server can lead to data loss. A conversion needs to be done
    # by setting it via CONFIG command on a live server first.
    #
    # Please check https://redis.io/topics/persistence for more information.
    
    appendonly no
    
    # The base name of the append only file.
    #
    # Redis 7 and newer use a set of append-only files to persist the dataset
    # and changes applied to it. There are two basic types of files in use:
    #
    # - Base files, which are a snapshot representing the complete state of the
    #   dataset at the time the file was created. Base files can be either in
    #   the form of RDB (binary serialized) or AOF (textual commands).
    # - Incremental files, which contain additional commands that were applied
    #   to the dataset following the previous file.
    #
    # In addition, manifest files are used to track the files and the order in
    # which they were created and should be applied.
    #
    # Append-only file names are created by Redis following a specific pattern.
    # The file name's prefix is based on the 'appendfilename' configuration
    # parameter, followed by additional information about the sequence and type.
    #
    # For example, if appendfilename is set to appendonly.aof, the following file
    # names could be derived:
    #
    # - appendonly.aof.1.base.rdb as a base file.
    # - appendonly.aof.1.incr.aof, appendonly.aof.2.incr.aof as incremental files.
    # - appendonly.aof.manifest as a manifest file.
    
    appendfilename "appendonly.aof"
    
    # For convenience, Redis stores all persistent append-only files in a dedicated
    # directory. The name of the directory is determined by the appenddirname
    # configuration parameter.
    
    appenddirname "appendonlydir"
    
    # The fsync() call tells the Operating System to actually write data on disk
    # instead of waiting for more data in the output buffer. Some OS will really flush
    # data on disk, some other OS will just try to do it ASAP.
    #
    # Redis supports three different modes:
    #
    # no: don't fsync, just let the OS flush the data when it wants. Faster.
    # always: fsync after every write to the append only log. Slow, Safest.
    # everysec: fsync only one time every second. Compromise.
    #
    # The default is "everysec", as that's usually the right compromise between
    # speed and data safety. It's up to you to understand if you can relax this to
    # "no" that will let the operating system flush the output buffer when
    # it wants, for better performances (but if you can live with the idea of
    # some data loss consider the default persistence mode that's snapshotting),
    # or on the contrary, use "always" that's very slow but a bit safer than
    # everysec.
    #
    # More details please check the following article:
    # http://antirez.com/post/redis-persistence-demystified.html
    #
    # If unsure, use "everysec".
    
    # appendfsync always
    appendfsync everysec
    # appendfsync no
    
    # When the AOF fsync policy is set to always or everysec, and a background
    # saving process (a background save or AOF log background rewriting) is
    # performing a lot of I/O against the disk, in some Linux configurations
    # Redis may block too long on the fsync() call. Note that there is no fix for
    # this currently, as even performing fsync in a different thread will block
    # our synchronous write(2) call.
    #
    # In order to mitigate this problem it's possible to use the following option
    # that will prevent fsync() from being called in the main process while a
    # BGSAVE or BGREWRITEAOF is in progress.
    #
    # This means that while another child is saving, the durability of Redis is
    # the same as "appendfsync no". In practical terms, this means that it is
    # possible to lose up to 30 seconds of log in the worst scenario (with the
    # default Linux settings).
    #
    # If you have latency problems turn this to "yes". Otherwise leave it as
    # "no" that is the safest pick from the point of view of durability.
    
    no-appendfsync-on-rewrite no
    
    # Automatic rewrite of the append only file.
    # Redis is able to automatically rewrite the log file implicitly calling
    # BGREWRITEAOF when the AOF log size grows by the specified percentage.
    #
    # This is how it works: Redis remembers the size of the AOF file after the
    # latest rewrite (if no rewrite has happened since the restart, the size of
    # the AOF at startup is used).
    #
    # This base size is compared to the current size. If the current size is
    # bigger than the specified percentage, the rewrite is triggered. Also
    # you need to specify a minimal size for the AOF file to be rewritten, this
    # is useful to avoid rewriting the AOF file even if the percentage increase
    # is reached but it is still pretty small.
    #
    # Specify a percentage of zero in order to disable the automatic AOF
    # rewrite feature.
    
    auto-aof-rewrite-percentage 100
    auto-aof-rewrite-min-size 64mb
    
    # An AOF file may be found to be truncated at the end during the Redis
    # startup process, when the AOF data gets loaded back into memory.
    # This may happen when the system where Redis is running
    # crashes, especially when an ext4 filesystem is mounted without the
    # data=ordered option (however this can't happen when Redis itself
    # crashes or aborts but the operating system still works correctly).
    #
    # Redis can either exit with an error when this happens, or load as much
    # data as possible (the default now) and start if the AOF file is found
    # to be truncated at the end. The following option controls this behavior.
    #
    # If aof-load-truncated is set to yes, a truncated AOF file is loaded and
    # the Redis server starts emitting a log to inform the user of the event.
    # Otherwise if the option is set to no, the server aborts with an error
    # and refuses to start. When the option is set to no, the user requires
    # to fix the AOF file using the "redis-check-aof" utility before to restart
    # the server.
    #
    # Note that if the AOF file will be found to be corrupted in the middle
    # the server will still exit with an error. This option only applies when
    # Redis will try to read more data from the AOF file but not enough bytes
    # will be found.
    aof-load-truncated yes
    
    # Redis can create append-only base files in either RDB or AOF formats. Using
    # the RDB format is always faster and more efficient, and disabling it is only
    # supported for backward compatibility purposes.
    aof-use-rdb-preamble yes
    
    # Redis supports recording timestamp annotations in the AOF to support restoring
    # the data from a specific point-in-time. However, using this capability changes
    # the AOF format in a way that may not be compatible with existing AOF parsers.
    aof-timestamp-enabled no
    
    ################################ SHUTDOWN #####################################
    
    # Maximum time to wait for replicas when shutting down, in seconds.
    #
    # During shut down, a grace period allows any lagging replicas to catch up with
    # the latest replication offset before the master exists. This period can
    # prevent data loss, especially for deployments without configured disk backups.
    #
    # The 'shutdown-timeout' value is the grace period's duration in seconds. It is
    # only applicable when the instance has replicas. To disable the feature, set
    # the value to 0.
    #
    # shutdown-timeout 10
    
    # When Redis receives a SIGINT or SIGTERM, shutdown is initiated and by default
    # an RDB snapshot is written to disk in a blocking operation if save points are configured.
    # The options used on signaled shutdown can include the following values:
    # default:  Saves RDB snapshot only if save points are configured.
    #           Waits for lagging replicas to catch up.
    # save:     Forces a DB saving operation even if no save points are configured.
    # nosave:   Prevents DB saving operation even if one or more save points are configured.
    # now:      Skips waiting for lagging replicas.
    # force:    Ignores any errors that would normally prevent the server from exiting.
    #
    # Any combination of values is allowed as long as "save" and "nosave" are not set simultaneously.
    # Example: "nosave force now"
    #
    # shutdown-on-sigint default
    # shutdown-on-sigterm default
    
    ################ NON-DETERMINISTIC LONG BLOCKING COMMANDS #####################
    
    # Maximum time in milliseconds for EVAL scripts, functions and in some cases
    # modules' commands before Redis can start processing or rejecting other clients.
    #
    # If the maximum execution time is reached Redis will start to reply to most
    # commands with a BUSY error.
    #
    # In this state Redis will only allow a handful of commands to be executed.
    # For instance, SCRIPT KILL, FUNCTION KILL, SHUTDOWN NOSAVE and possibly some
    # module specific 'allow-busy' commands.
    #
    # SCRIPT KILL and FUNCTION KILL will only be able to stop a script that did not
    # yet call any write commands, so SHUTDOWN NOSAVE may be the only way to stop
    # the server in the case a write command was already issued by the script when
    # the user doesn't want to wait for the natural termination of the script.
    #
    # The default is 5 seconds. It is possible to set it to 0 or a negative value
    # to disable this mechanism (uninterrupted execution). Note that in the past
    # this config had a different name, which is now an alias, so both of these do
    # the same:
    # lua-time-limit 5000
    # busy-reply-threshold 5000
    
    ################################ REDIS CLUSTER  ###############################
    
    # Normal Redis instances can't be part of a Redis Cluster; only nodes that are
    # started as cluster nodes can. In order to start a Redis instance as a
    # cluster node enable the cluster support uncommenting the following:
    #
    # cluster-enabled yes
    
    # Every cluster node has a cluster configuration file. This file is not
    # intended to be edited by hand. It is created and updated by Redis nodes.
    # Every Redis Cluster node requires a different cluster configuration file.
    # Make sure that instances running in the same system do not have
    # overlapping cluster configuration file names.
    #
    # cluster-config-file nodes-6379.conf
    
    # Cluster node timeout is the amount of milliseconds a node must be unreachable
    # for it to be considered in failure state.
    # Most other internal time limits are a multiple of the node timeout.
    #
    # cluster-node-timeout 15000
    
    # The cluster port is the port that the cluster bus will listen for inbound connections on. When set 
    # to the default value, 0, it will be bound to the command port + 10000. Setting this value requires 
    # you to specify the cluster bus port when executing cluster meet.
    # cluster-port 0
    
    # A replica of a failing master will avoid to start a failover if its data
    # looks too old.
    #
    # There is no simple way for a replica to actually have an exact measure of
    # its "data age", so the following two checks are performed:
    #
    # 1) If there are multiple replicas able to failover, they exchange messages
    #    in order to try to give an advantage to the replica with the best
    #    replication offset (more data from the master processed).
    #    Replicas will try to get their rank by offset, and apply to the start
    #    of the failover a delay proportional to their rank.
    #
    # 2) Every single replica computes the time of the last interaction with
    #    its master. This can be the last ping or command received (if the master
    #    is still in the "connected" state), or the time that elapsed since the
    #    disconnection with the master (if the replication link is currently down).
    #    If the last interaction is too old, the replica will not try to failover
    #    at all.
    #
    # The point "2" can be tuned by user. Specifically a replica will not perform
    # the failover if, since the last interaction with the master, the time
    # elapsed is greater than:
    #
    #   (node-timeout * cluster-replica-validity-factor) + repl-ping-replica-period
    #
    # So for example if node-timeout is 30 seconds, and the cluster-replica-validity-factor
    # is 10, and assuming a default repl-ping-replica-period of 10 seconds, the
    # replica will not try to failover if it was not able to talk with the master
    # for longer than 310 seconds.
    #
    # A large cluster-replica-validity-factor may allow replicas with too old data to failover
    # a master, while a too small value may prevent the cluster from being able to
    # elect a replica at all.
    #
    # For maximum availability, it is possible to set the cluster-replica-validity-factor
    # to a value of 0, which means, that replicas will always try to failover the
    # master regardless of the last time they interacted with the master.
    # (However they'll always try to apply a delay proportional to their
    # offset rank).
    #
    # Zero is the only value able to guarantee that when all the partitions heal
    # the cluster will always be able to continue.
    #
    # cluster-replica-validity-factor 10
    
    # Cluster replicas are able to migrate to orphaned masters, that are masters
    # that are left without working replicas. This improves the cluster ability
    # to resist to failures as otherwise an orphaned master can't be failed over
    # in case of failure if it has no working replicas.
    #
    # Replicas migrate to orphaned masters only if there are still at least a
    # given number of other working replicas for their old master. This number
    # is the "migration barrier". A migration barrier of 1 means that a replica
    # will migrate only if there is at least 1 other working replica for its master
    # and so forth. It usually reflects the number of replicas you want for every
    # master in your cluster.
    #
    # Default is 1 (replicas migrate only if their masters remain with at least
    # one replica). To disable migration just set it to a very large value or
    # set cluster-allow-replica-migration to 'no'.
    # A value of 0 can be set but is useful only for debugging and dangerous
    # in production.
    #
    # cluster-migration-barrier 1
    
    # Turning off this option allows to use less automatic cluster configuration.
    # It both disables migration to orphaned masters and migration from masters
    # that became empty.
    #
    # Default is 'yes' (allow automatic migrations).
    #
    # cluster-allow-replica-migration yes
    
    # By default Redis Cluster nodes stop accepting queries if they detect there
    # is at least a hash slot uncovered (no available node is serving it).
    # This way if the cluster is partially down (for example a range of hash slots
    # are no longer covered) all the cluster becomes, eventually, unavailable.
    # It automatically returns available as soon as all the slots are covered again.
    #
    # However sometimes you want the subset of the cluster which is working,
    # to continue to accept queries for the part of the key space that is still
    # covered. In order to do so, just set the cluster-require-full-coverage
    # option to no.
    #
    # cluster-require-full-coverage yes
    
    # This option, when set to yes, prevents replicas from trying to failover its
    # master during master failures. However the replica can still perform a
    # manual failover, if forced to do so.
    #
    # This is useful in different scenarios, especially in the case of multiple
    # data center operations, where we want one side to never be promoted if not
    # in the case of a total DC failure.
    #
    # cluster-replica-no-failover no
    
    # This option, when set to yes, allows nodes to serve read traffic while the
    # cluster is in a down state, as long as it believes it owns the slots.
    #
    # This is useful for two cases.  The first case is for when an application
    # doesn't require consistency of data during node failures or network partitions.
    # One example of this is a cache, where as long as the node has the data it
    # should be able to serve it.
    #
    # The second use case is for configurations that don't meet the recommended
    # three shards but want to enable cluster mode and scale later. A
    # master outage in a 1 or 2 shard configuration causes a read/write outage to the
    # entire cluster without this option set, with it set there is only a write outage.
    # Without a quorum of masters, slot ownership will not change automatically.
    #
    # cluster-allow-reads-when-down no
    
    # This option, when set to yes, allows nodes to serve pubsub shard traffic while
    # the cluster is in a down state, as long as it believes it owns the slots.
    #
    # This is useful if the application would like to use the pubsub feature even when
    # the cluster global stable state is not OK. If the application wants to make sure only
    # one shard is serving a given channel, this feature should be kept as yes.
    #
    # cluster-allow-pubsubshard-when-down yes
    
    # Cluster link send buffer limit is the limit on the memory usage of an individual
    # cluster bus link's send buffer in bytes. Cluster links would be freed if they exceed
    # this limit. This is to primarily prevent send buffers from growing unbounded on links
    # toward slow peers (E.g. PubSub messages being piled up).
    # This limit is disabled by default. Enable this limit when 'mem_cluster_links' INFO field
    # and/or 'send-buffer-allocated' entries in the 'CLUSTER LINKS` command output continuously increase.
    # Minimum limit of 1gb is recommended so that cluster link buffer can fit in at least a single
    # PubSub message by default. (client-query-buffer-limit default value is 1gb)
    #
    # cluster-link-sendbuf-limit 0
    
    # Clusters can configure their announced hostname using this config. This is a common use case for 
    # applications that need to use TLS Server Name Indication (SNI) or dealing with DNS based
    # routing. By default this value is only shown as additional metadata in the CLUSTER SLOTS
    # command, but can be changed using 'cluster-preferred-endpoint-type' config. This value is 
    # communicated along the clusterbus to all nodes, setting it to an empty string will remove 
    # the hostname and also propagate the removal.
    #
    # cluster-announce-hostname ""
    
    # Clusters can configure an optional nodename to be used in addition to the node ID for
    # debugging and admin information. This name is broadcasted between nodes, so will be used
    # in addition to the node ID when reporting cross node events such as node failures.
    # cluster-announce-human-nodename ""
    
    # Clusters can advertise how clients should connect to them using either their IP address,
    # a user defined hostname, or by declaring they have no endpoint. Which endpoint is
    # shown as the preferred endpoint is set by using the cluster-preferred-endpoint-type
    # config with values 'ip', 'hostname', or 'unknown-endpoint'. This value controls how
    # the endpoint returned for MOVED/ASKING requests as well as the first field of CLUSTER SLOTS. 
    # If the preferred endpoint type is set to hostname, but no announced hostname is set, a '?' 
    # will be returned instead.
    #
    # When a cluster advertises itself as having an unknown endpoint, it's indicating that
    # the server doesn't know how clients can reach the cluster. This can happen in certain 
    # networking situations where there are multiple possible routes to the node, and the 
    # server doesn't know which one the client took. In this case, the server is expecting
    # the client to reach out on the same endpoint it used for making the last request, but use
    # the port provided in the response.
    #
    # cluster-preferred-endpoint-type ip
    
    # In order to setup your cluster make sure to read the documentation
    # available at https://redis.io web site.
    
    ########################## CLUSTER DOCKER/NAT support  ########################
    
    # In certain deployments, Redis Cluster nodes address discovery fails, because
    # addresses are NAT-ted or because ports are forwarded (the typical case is
    # Docker and other containers).
    #
    # In order to make Redis Cluster working in such environments, a static
    # configuration where each node knows its public address is needed. The
    # following four options are used for this scope, and are:
    #
    # * cluster-announce-ip
    # * cluster-announce-port
    # * cluster-announce-tls-port
    # * cluster-announce-bus-port
    #
    # Each instructs the node about its address, client ports (for connections
    # without and with TLS) and cluster message bus port. The information is then
    # published in the header of the bus packets so that other nodes will be able to
    # correctly map the address of the node publishing the information.
    #
    # If tls-cluster is set to yes and cluster-announce-tls-port is omitted or set
    # to zero, then cluster-announce-port refers to the TLS port. Note also that
    # cluster-announce-tls-port has no effect if tls-cluster is set to no.
    #
    # If the above options are not used, the normal Redis Cluster auto-detection
    # will be used instead.
    #
    # Note that when remapped, the bus port may not be at the fixed offset of
    # clients port + 10000, so you can specify any port and bus-port depending
    # on how they get remapped. If the bus-port is not set, a fixed offset of
    # 10000 will be used as usual.
    #
    # Example:
    #
    # cluster-announce-ip 10.1.1.5
    # cluster-announce-tls-port 6379
    # cluster-announce-port 0
    # cluster-announce-bus-port 6380
    
    ################################## SLOW LOG ###################################
    
    # The Redis Slow Log is a system to log queries that exceeded a specified
    # execution time. The execution time does not include the I/O operations
    # like talking with the client, sending the reply and so forth,
    # but just the time needed to actually execute the command (this is the only
    # stage of command execution where the thread is blocked and can not serve
    # other requests in the meantime).
    #
    # You can configure the slow log with two parameters: one tells Redis
    # what is the execution time, in microseconds, to exceed in order for the
    # command to get logged, and the other parameter is the length of the
    # slow log. When a new command is logged the oldest one is removed from the
    # queue of logged commands.
    
    # The following time is expressed in microseconds, so 1000000 is equivalent
    # to one second. Note that a negative number disables the slow log, while
    # a value of zero forces the logging of every command.
    slowlog-log-slower-than 10000
    
    # There is no limit to this length. Just be aware that it will consume memory.
    # You can reclaim memory used by the slow log with SLOWLOG RESET.
    slowlog-max-len 128
    
    ################################ LATENCY MONITOR ##############################
    
    # The Redis latency monitoring subsystem samples different operations
    # at runtime in order to collect data related to possible sources of
    # latency of a Redis instance.
    #
    # Via the LATENCY command this information is available to the user that can
    # print graphs and obtain reports.
    #
    # The system only logs operations that were performed in a time equal or
    # greater than the amount of milliseconds specified via the
    # latency-monitor-threshold configuration directive. When its value is set
    # to zero, the latency monitor is turned off.
    #
    # By default latency monitoring is disabled since it is mostly not needed
    # if you don't have latency issues, and collecting data has a performance
    # impact, that while very small, can be measured under big load. Latency
    # monitoring can easily be enabled at runtime using the command
    # "CONFIG SET latency-monitor-threshold <milliseconds>" if needed.
    latency-monitor-threshold 0
    
    ################################ LATENCY TRACKING ##############################
    
    # The Redis extended latency monitoring tracks the per command latencies and enables
    # exporting the percentile distribution via the INFO latencystats command,
    # and cumulative latency distributions (histograms) via the LATENCY command.
    #
    # By default, the extended latency monitoring is enabled since the overhead
    # of keeping track of the command latency is very small.
    # latency-tracking yes
    
    # By default the exported latency percentiles via the INFO latencystats command
    # are the p50, p99, and p999.
    # latency-tracking-info-percentiles 50 99 99.9
    
    ############################# EVENT NOTIFICATION ##############################
    
    # Redis can notify Pub/Sub clients about events happening in the key space.
    # This feature is documented at https://redis.io/topics/notifications
    #
    # For instance if keyspace events notification is enabled, and a client
    # performs a DEL operation on key "foo" stored in the Database 0, two
    # messages will be published via Pub/Sub:
    #
    # PUBLISH __keyspace@0__:foo del
    # PUBLISH __keyevent@0__:del foo
    #
    # It is possible to select the events that Redis will notify among a set
    # of classes. Every class is identified by a single character:
    #
    #  K     Keyspace events, published with __keyspace@<db>__ prefix.
    #  E     Keyevent events, published with __keyevent@<db>__ prefix.
    #  g     Generic commands (non-type specific) like DEL, EXPIRE, RENAME, ...
    #  $     String commands
    #  l     List commands
    #  s     Set commands
    #  h     Hash commands
    #  z     Sorted set commands
    #  x     Expired events (events generated every time a key expires)
    #  e     Evicted events (events generated when a key is evicted for maxmemory)
    #  n     New key events (Note: not included in the 'A' class)
    #  t     Stream commands
    #  d     Module key type events
    #  m     Key-miss events (Note: It is not included in the 'A' class)
    #  A     Alias for g$lshzxetd, so that the "AKE" string means all the events
    #        (Except key-miss events which are excluded from 'A' due to their
    #         unique nature).
    #
    #  The "notify-keyspace-events" takes as argument a string that is composed
    #  of zero or multiple characters. The empty string means that notifications
    #  are disabled.
    #
    #  Example: to enable list and generic events, from the point of view of the
    #           event name, use:
    #
    #  notify-keyspace-events Elg
    #
    #  Example 2: to get the stream of the expired keys subscribing to channel
    #             name __keyevent@0__:expired use:
    #
    #  notify-keyspace-events Ex
    #
    #  By default all notifications are disabled because most users don't need
    #  this feature and the feature has some overhead. Note that if you don't
    #  specify at least one of K or E, no events will be delivered.
    notify-keyspace-events ""
    
    ############################### ADVANCED CONFIG ###############################
    
    # Hashes are encoded using a memory efficient data structure when they have a
    # small number of entries, and the biggest entry does not exceed a given
    # threshold. These thresholds can be configured using the following directives.
    hash-max-listpack-entries 512
    hash-max-listpack-value 64
    
    # Lists are also encoded in a special way to save a lot of space.
    # The number of entries allowed per internal list node can be specified
    # as a fixed maximum size or a maximum number of elements.
    # For a fixed maximum size, use -5 through -1, meaning:
    # -5: max size: 64 Kb  <-- not recommended for normal workloads
    # -4: max size: 32 Kb  <-- not recommended
    # -3: max size: 16 Kb  <-- probably not recommended
    # -2: max size: 8 Kb   <-- good
    # -1: max size: 4 Kb   <-- good
    # Positive numbers mean store up to _exactly_ that number of elements
    # per list node.
    # The highest performing option is usually -2 (8 Kb size) or -1 (4 Kb size),
    # but if your use case is unique, adjust the settings as necessary.
    list-max-listpack-size -2
    
    # Lists may also be compressed.
    # Compress depth is the number of quicklist ziplist nodes from *each* side of
    # the list to *exclude* from compression.  The head and tail of the list
    # are always uncompressed for fast push/pop operations.  Settings are:
    # 0: disable all list compression
    # 1: depth 1 means "don't start compressing until after 1 node into the list,
    #    going from either the head or tail"
    #    So: [head]->node->node->...->node->[tail]
    #    [head], [tail] will always be uncompressed; inner nodes will compress.
    # 2: [head]->[next]->node->node->...->node->[prev]->[tail]
    #    2 here means: don't compress head or head->next or tail->prev or tail,
    #    but compress all nodes between them.
    # 3: [head]->[next]->[next]->node->node->...->node->[prev]->[prev]->[tail]
    # etc.
    list-compress-depth 0
    
    # Sets have a special encoding when a set is composed
    # of just strings that happen to be integers in radix 10 in the range
    # of 64 bit signed integers.
    # The following configuration setting sets the limit in the size of the
    # set in order to use this special memory saving encoding.
    set-max-intset-entries 512
    
    # Sets containing non-integer values are also encoded using a memory efficient
    # data structure when they have a small number of entries, and the biggest entry
    # does not exceed a given threshold. These thresholds can be configured using
    # the following directives.
    set-max-listpack-entries 128
    set-max-listpack-value 64
    
    # Similarly to hashes and lists, sorted sets are also specially encoded in
    # order to save a lot of space. This encoding is only used when the length and
    # elements of a sorted set are below the following limits:
    zset-max-listpack-entries 128
    zset-max-listpack-value 64
    
    # HyperLogLog sparse representation bytes limit. The limit includes the
    # 16 bytes header. When a HyperLogLog using the sparse representation crosses
    # this limit, it is converted into the dense representation.
    #
    # A value greater than 16000 is totally useless, since at that point the
    # dense representation is more memory efficient.
    #
    # The suggested value is ~ 3000 in order to have the benefits of
    # the space efficient encoding without slowing down too much PFADD,
    # which is O(N) with the sparse encoding. The value can be raised to
    # ~ 10000 when CPU is not a concern, but space is, and the data set is
    # composed of many HyperLogLogs with cardinality in the 0 - 15000 range.
    hll-sparse-max-bytes 3000
    
    # Streams macro node max size / items. The stream data structure is a radix
    # tree of big nodes that encode multiple items inside. Using this configuration
    # it is possible to configure how big a single node can be in bytes, and the
    # maximum number of items it may contain before switching to a new node when
    # appending new stream entries. If any of the following settings are set to
    # zero, the limit is ignored, so for instance it is possible to set just a
    # max entries limit by setting max-bytes to 0 and max-entries to the desired
    # value.
    stream-node-max-bytes 4096
    stream-node-max-entries 100
    
    # Active rehashing uses 1 millisecond every 100 milliseconds of CPU time in
    # order to help rehashing the main Redis hash table (the one mapping top-level
    # keys to values). The hash table implementation Redis uses (see dict.c)
    # performs a lazy rehashing: the more operation you run into a hash table
    # that is rehashing, the more rehashing "steps" are performed, so if the
    # server is idle the rehashing is never complete and some more memory is used
    # by the hash table.
    #
    # The default is to use this millisecond 10 times every second in order to
    # actively rehash the main dictionaries, freeing memory when possible.
    #
    # If unsure:
    # use "activerehashing no" if you have hard latency requirements and it is
    # not a good thing in your environment that Redis can reply from time to time
    # to queries with 2 milliseconds delay.
    #
    # use "activerehashing yes" if you don't have such hard requirements but
    # want to free memory asap when possible.
    activerehashing yes
    
    # The client output buffer limits can be used to force disconnection of clients
    # that are not reading data from the server fast enough for some reason (a
    # common reason is that a Pub/Sub client can't consume messages as fast as the
    # publisher can produce them).
    #
    # The limit can be set differently for the three different classes of clients:
    #
    # normal -> normal clients including MONITOR clients
    # replica -> replica clients
    # pubsub -> clients subscribed to at least one pubsub channel or pattern
    #
    # The syntax of every client-output-buffer-limit directive is the following:
    #
    # client-output-buffer-limit <class> <hard limit> <soft limit> <soft seconds>
    #
    # A client is immediately disconnected once the hard limit is reached, or if
    # the soft limit is reached and remains reached for the specified number of
    # seconds (continuously).
    # So for instance if the hard limit is 32 megabytes and the soft limit is
    # 16 megabytes / 10 seconds, the client will get disconnected immediately
    # if the size of the output buffers reach 32 megabytes, but will also get
    # disconnected if the client reaches 16 megabytes and continuously overcomes
    # the limit for 10 seconds.
    #
    # By default normal clients are not limited because they don't receive data
    # without asking (in a push way), but just after a request, so only
    # asynchronous clients may create a scenario where data is requested faster
    # than it can read.
    #
    # Instead there is a default limit for pubsub and replica clients, since
    # subscribers and replicas receive data in a push fashion.
    #
    # Note that it doesn't make sense to set the replica clients output buffer
    # limit lower than the repl-backlog-size config (partial sync will succeed
    # and then replica will get disconnected).
    # Such a configuration is ignored (the size of repl-backlog-size will be used).
    # This doesn't have memory consumption implications since the replica client
    # will share the backlog buffers memory.
    #
    # Both the hard or the soft limit can be disabled by setting them to zero.
    client-output-buffer-limit normal 0 0 0
    client-output-buffer-limit replica 256mb 64mb 60
    client-output-buffer-limit pubsub 32mb 8mb 60
    
    # Client query buffers accumulate new commands. They are limited to a fixed
    # amount by default in order to avoid that a protocol desynchronization (for
    # instance due to a bug in the client) will lead to unbound memory usage in
    # the query buffer. However you can configure it here if you have very special
    # needs, such us huge multi/exec requests or alike.
    #
    # client-query-buffer-limit 1gb
    
    # In some scenarios client connections can hog up memory leading to OOM
    # errors or data eviction. To avoid this we can cap the accumulated memory
    # used by all client connections (all pubsub and normal clients). Once we
    # reach that limit connections will be dropped by the server freeing up
    # memory. The server will attempt to drop the connections using the most 
    # memory first. We call this mechanism "client eviction".
    #
    # Client eviction is configured using the maxmemory-clients setting as follows:
    # 0 - client eviction is disabled (default)
    #
    # A memory value can be used for the client eviction threshold,
    # for example:
    # maxmemory-clients 1g
    #
    # A percentage value (between 1% and 100%) means the client eviction threshold
    # is based on a percentage of the maxmemory setting. For example to set client
    # eviction at 5% of maxmemory:
    # maxmemory-clients 5%
    
    # In the Redis protocol, bulk requests, that are, elements representing single
    # strings, are normally limited to 512 mb. However you can change this limit
    # here, but must be 1mb or greater
    #
    # proto-max-bulk-len 512mb
    
    # Redis calls an internal function to perform many background tasks, like
    # closing connections of clients in timeout, purging expired keys that are
    # never requested, and so forth.
    #
    # Not all tasks are performed with the same frequency, but Redis checks for
    # tasks to perform according to the specified "hz" value.
    #
    # By default "hz" is set to 10. Raising the value will use more CPU when
    # Redis is idle, but at the same time will make Redis more responsive when
    # there are many keys expiring at the same time, and timeouts may be
    # handled with more precision.
    #
    # The range is between 1 and 500, however a value over 100 is usually not
    # a good idea. Most users should use the default of 10 and raise this up to
    # 100 only in environments where very low latency is required.
    hz 10
    
    # Normally it is useful to have an HZ value which is proportional to the
    # number of clients connected. This is useful in order, for instance, to
    # avoid too many clients are processed for each background task invocation
    # in order to avoid latency spikes.
    #
    # Since the default HZ value by default is conservatively set to 10, Redis
    # offers, and enables by default, the ability to use an adaptive HZ value
    # which will temporarily raise when there are many connected clients.
    #
    # When dynamic HZ is enabled, the actual configured HZ will be used
    # as a baseline, but multiples of the configured HZ value will be actually
    # used as needed once more clients are connected. In this way an idle
    # instance will use very little CPU time while a busy instance will be
    # more responsive.
    dynamic-hz yes
    
    # When a child rewrites the AOF file, if the following option is enabled
    # the file will be fsync-ed every 4 MB of data generated. This is useful
    # in order to commit the file to the disk more incrementally and avoid
    # big latency spikes.
    aof-rewrite-incremental-fsync yes
    
    # When redis saves RDB file, if the following option is enabled
    # the file will be fsync-ed every 4 MB of data generated. This is useful
    # in order to commit the file to the disk more incrementally and avoid
    # big latency spikes.
    rdb-save-incremental-fsync yes
    
    # Redis LFU eviction (see maxmemory setting) can be tuned. However it is a good
    # idea to start with the default settings and only change them after investigating
    # how to improve the performances and how the keys LFU change over time, which
    # is possible to inspect via the OBJECT FREQ command.
    #
    # There are two tunable parameters in the Redis LFU implementation: the
    # counter logarithm factor and the counter decay time. It is important to
    # understand what the two parameters mean before changing them.
    #
    # The LFU counter is just 8 bits per key, it's maximum value is 255, so Redis
    # uses a probabilistic increment with logarithmic behavior. Given the value
    # of the old counter, when a key is accessed, the counter is incremented in
    # this way:
    #
    # 1. A random number R between 0 and 1 is extracted.
    # 2. A probability P is calculated as 1/(old_value*lfu_log_factor+1).
    # 3. The counter is incremented only if R < P.
    #
    # The default lfu-log-factor is 10. This is a table of how the frequency
    # counter changes with a different number of accesses with different
    # logarithmic factors:
    #
    # +--------+------------+------------+------------+------------+------------+
    # | factor | 100 hits   | 1000 hits  | 100K hits  | 1M hits    | 10M hits   |
    # +--------+------------+------------+------------+------------+------------+
    # | 0      | 104        | 255        | 255        | 255        | 255        |
    # +--------+------------+------------+------------+------------+------------+
    # | 1      | 18         | 49         | 255        | 255        | 255        |
    # +--------+------------+------------+------------+------------+------------+
    # | 10     | 10         | 18         | 142        | 255        | 255        |
    # +--------+------------+------------+------------+------------+------------+
    # | 100    | 8          | 11         | 49         | 143        | 255        |
    # +--------+------------+------------+------------+------------+------------+
    #
    # NOTE: The above table was obtained by running the following commands:
    #
    #   redis-benchmark -n 1000000 incr foo
    #   redis-cli object freq foo
    #
    # NOTE 2: The counter initial value is 5 in order to give new objects a chance
    # to accumulate hits.
    #
    # The counter decay time is the time, in minutes, that must elapse in order
    # for the key counter to be decremented.
    #
    # The default value for the lfu-decay-time is 1. A special value of 0 means we
    # will never decay the counter.
    #
    # lfu-log-factor 10
    # lfu-decay-time 1
    
    ########################### ACTIVE DEFRAGMENTATION #######################
    #
    # What is active defragmentation?
    # -------------------------------
    #
    # Active (online) defragmentation allows a Redis server to compact the
    # spaces left between small allocations and deallocations of data in memory,
    # thus allowing to reclaim back memory.
    #
    # Fragmentation is a natural process that happens with every allocator (but
    # less so with Jemalloc, fortunately) and certain workloads. Normally a server
    # restart is needed in order to lower the fragmentation, or at least to flush
    # away all the data and create it again. However thanks to this feature
    # implemented by Oran Agra for Redis 4.0 this process can happen at runtime
    # in a "hot" way, while the server is running.
    #
    # Basically when the fragmentation is over a certain level (see the
    # configuration options below) Redis will start to create new copies of the
    # values in contiguous memory regions by exploiting certain specific Jemalloc
    # features (in order to understand if an allocation is causing fragmentation
    # and to allocate it in a better place), and at the same time, will release the
    # old copies of the data. This process, repeated incrementally for all the keys
    # will cause the fragmentation to drop back to normal values.
    #
    # Important things to understand:
    #
    # 1. This feature is disabled by default, and only works if you compiled Redis
    #    to use the copy of Jemalloc we ship with the source code of Redis.
    #    This is the default with Linux builds.
    #
    # 2. You never need to enable this feature if you don't have fragmentation
    #    issues.
    #
    # 3. Once you experience fragmentation, you can enable this feature when
    #    needed with the command "CONFIG SET activedefrag yes".
    #
    # The configuration parameters are able to fine tune the behavior of the
    # defragmentation process. If you are not sure about what they mean it is
    # a good idea to leave the defaults untouched.
    
    # Active defragmentation is disabled by default
    # activedefrag no
    
    # Minimum amount of fragmentation waste to start active defrag
    # active-defrag-ignore-bytes 100mb
    
    # Minimum percentage of fragmentation to start active defrag
    # active-defrag-threshold-lower 10
    
    # Maximum percentage of fragmentation at which we use maximum effort
    # active-defrag-threshold-upper 100
    
    # Minimal effort for defrag in CPU percentage, to be used when the lower
    # threshold is reached
    # active-defrag-cycle-min 1
    
    # Maximal effort for defrag in CPU percentage, to be used when the upper
    # threshold is reached
    # active-defrag-cycle-max 25
    
    # Maximum number of set/hash/zset/list fields that will be processed from
    # the main dictionary scan
    # active-defrag-max-scan-fields 1000
    
    # Jemalloc background thread for purging will be enabled by default
    jemalloc-bg-thread yes
    
    # It is possible to pin different threads and processes of Redis to specific
    # CPUs in your system, in order to maximize the performances of the server.
    # This is useful both in order to pin different Redis threads in different
    # CPUs, but also in order to make sure that multiple Redis instances running
    # in the same host will be pinned to different CPUs.
    #
    # Normally you can do this using the "taskset" command, however it is also
    # possible to this via Redis configuration directly, both in Linux and FreeBSD.
    #
    # You can pin the server/IO threads, bio threads, aof rewrite child process, and
    # the bgsave child process. The syntax to specify the cpu list is the same as
    # the taskset command:
    #
    # Set redis server/io threads to cpu affinity 0,2,4,6:
    # server-cpulist 0-7:2
    #
    # Set bio threads to cpu affinity 1,3:
    # bio-cpulist 1,3
    #
    # Set aof rewrite child process to cpu affinity 8,9,10,11:
    # aof-rewrite-cpulist 8-11
    #
    # Set bgsave child process to cpu affinity 1,10,11
    # bgsave-cpulist 1,10-11
    
    # In some cases redis will emit warnings and even refuse to start if it detects
    # that the system is in bad state, it is possible to suppress these warnings
    # by setting the following config which takes a space delimited list of warnings
    # to suppress
    #
    # ignore-warnings ARM64-COW-BUG }}-php-

    - name: Install dependencies
      run: composer install --prefer-dist --no-progress

    # Add a test script to composer.json, for instance: "test": "vendor/bin/phpunit"
    # Docs: https://getcomposer.org/doc/articles/scripts.md

    # - name: Run test suite
    #   run: composer run-script test




       
