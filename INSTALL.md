Building Mergin Maps mobile app from source - step by step


# Table of Contents
<!-- Table of contents generated with https://freelance-tech-writer.github.io/table-of-contents-generator/index.html -->

- [Table of Contents](#table-of-contents)
- [1. Introduction](#1-introduction)
- [2. Overview](#2-overview)
  - [2.1 Secrets](#21-secrets)
  - [2.2 Code formatting](#22-code-formatting)
  - [2.3 Required Qt packages](#23-required-qt-packages)
- [3. Building GNU/Ubuntu](#3-building-gnuubuntu)
- [4. Building Android (on Ubuntu/macOS/Windows)](#4-building-android-on-ubuntumacoswindows)
  - [4.1. Android on Ubuntu](#41-android-on-ubuntu)
  - [4.2. Android on macOS](#42-android-on-macos)
  - [4.3. Android on Windows](#43-android-on-windows)
- [5. Building iOS](#5-building-ios)
- [6. Building macOS](#6-building-macos)
- [7. Building Windows](#7-building-windows)
- [8. Auto Testing](#8-auto-testing)

# 1. Introduction

This document is the original installation guide of the described software
Mergin Maps Input. The software and hardware descriptions named in this
document are in most cases registered trademarks and are therefore subject
to the legal requirements. Mergin Maps Input is subject to the GNU General Public
License.

The details, that are given in this document have been written and verified
to the best of knowledge and responsibility of the editors. Nevertheless,
mistakes concerning the content are possible. Therefore, all data are not
liable to any duties or guarantees. The editors and publishers do not take
any responsibility or liability for failures and their consequences. You are
always welcome for indicating possible mistakes.

For user documentation, please read [merginmaps.com/docs](https://merginmaps.com/docs).

For code architecture of codebase, please see [docs](./docs/README.md).

**Note to document writers:** Please use this document as the central
place for describing build procedures. Please do not remove this notice.

# 2. Overview

Mergin Maps Input, like a number of major projects (e.g., KDE 4.0),
uses [CMake](https://www.cmake.org) for building from source.

It is C++ application build on top of [Qt](https://www.qt.io), [QGIS](https://www.qgis.org/en/site/)
and many other FOSS libraries. 

All required libraries (in release configuration) are packaged in the [Mobile-SDK](https://github.com/MerginMaps/mobile-sdk).
If you need to debug some error in the library, you need to compile Input-SDK in debug yourself locally. Otherwise
it is suggested to download required libraries from [Mobile-SDK tags](https://github.com/MerginMaps/mobile-sdk/tags)
Mobile-SDK uses vcpkg packaging system, so if the SDK for your target system/architecture you can build it yourself.

Generally, for building setup, you need the same versions of libraries/SDKs/NDKs/compilers as used in the official 
[GitHub Actions](https://github.com/MerginMaps/mobile/tree/master/.github/workflows).
Open workflow file for your platform/target and see the version of libraries used and replicate it in your setup.

## 2.1 Secrets

To communicate with MerginAPI, some endpoints need to attach `api_key`. To not leak API_KEY,
the source code that returns the API_KEYS is encrypted.

As a developer, if you want to develop against local Mergin Maps server, it is OK, but to
work with public or dev.dev server, you need to manually decrypt the file. Decrypted file is never
pushed to git!

The password for decryption is in out password manager.

if you want to change the secrets, decrypt, change and encrypt the file
[openssl](http://stackoverflow.com/questions/16056135/ddg#16056298)
make sure you update the keys in password manager and in the kubernetes
manifest files.

encrypt
```
cd core/
openssl aes-256-cbc -in merginsecrets.cpp -out merginsecrets.cpp.enc -md md5
```

decrypt
```
cd core/
openssl aes-256-cbc -d -in merginsecrets.cpp.enc -out merginsecrets.cpp -md md5
```

## 2.2 Code formatting

We use `astyle` to format CPP and Objective-C files. Format is similar to what QGIS has.
For more details about code conventions, please read [our code conventions doc](./docs/code_convention.md).

We also use software [pre-commit](https://pre-commit.com/) to automatically check format when doing a commit.
You need to install it via `brew`/`pip`, see [installation details](https://pre-commit.com/#installation).

In order to start using the `pre-commit`, run `pre-commit install` in the repository root folder.

To manually run the style check, run `pre-commit run --all-files` or optionally run script `format_cpp.sh` (we use this one in CI currently).

In case you want to skip execution of pre-commit hooks, add additional flag `--no-verify` to your commit command, e.g.: `git commit -m "nit" --no-verify`

## 2.3 Required Qt packages

Mergin Maps is built with Qt. If you are using Qt's `Maintenance tool`, make sure to install these packages:
 - `Android` -> when building for Andoroid
 - `iOS` -> when building for iOS
 - `macOS` -> or other desktop platform based on your host machine
 - `Qt 5 Compatibility Module`
 - `Qt Shader Tools`
 - `Additional libraries:`
   - `Qt Connectivity` -> used for Bluetooth
   - `Qt Multimedia`
   - `Qt Positioning`
   - `Qt Sensors`

# 3. Building GNU/Ubuntu

This guide is tested with Ubuntu 22.04, on other distros some steps may need some adjustments.

Steps to build and run Input:

1. Install some dependencies, critically bison and flex. See "Install Build Dependencies" step in `.github/workflows/linux.yml`

2. Get Input SDK - it contains pre-built dependencies of libraries used by Input

   - Check what SDK version is currently in use - look for `INPUT_SDK_VERSION` in `.github/workflows/linux.yml`
   - Download Input SDK for Ubuntu - go to https://github.com/merginmaps/input-sdk/releases and download the built SDK.
   - Unpack the downloaded .tar.gz to `~/input-sdk/x64-linux`

3. Get Qt libraries - Ubuntu's system libraries are too old, and currently Input SDK does not include Qt SDK.

   - Check what Qt version is currently in use - look for `QT_VERSION` in `.github/workflows/linux.yml`
   - Download Qt online installer from https://www.qt.io/download-open-source
   - Use the online installer to install Qt to `~/Qt`

4. Build Input (update CMake command with the correct Qt and Input SDK versions)

   ```
   mkdir build
   cd build
   cmake -G Ninja \
     -DCMAKE_PREFIX_PATH=~/Qt/6.6.3/gcc_64 \
     -DINPUT_SDK_PATH=~/input-sdk/x64-linux \
     -DQGIS_QUICK_DATA_PATH=../app/android/assets/qgis-data \
     -DUSE_MM_SERVER_API_KEY=FALSE \
     ..
   ninja
   ```

5. Run Input

   ```
   ./app/Input
   ```


# 4. Building Android (on Ubuntu/macOS/Windows)

For building ABIS see https://www.qt.io/blog/android-multi-abi-builds-are-back

If you have "error: undefined reference to 'stdout'" or so, make sure that in BUILD ENV you have ANDROID_NDK_PLATFORM=android-24 or later!

![image](https://user-images.githubusercontent.com/22449698/166630970-a776576f-c505-4265-b4c8-ffbe212c6745.png)

## 4.1. Android on Ubuntu

1. Install some dependencies, see requirements in `.github/workflows/android.yml`

   - Java >= 17 (on Ubuntu 22.04 do `sudo apt install openjdk-17-jdk` and make sure it is the default by checking `java --version`)
   - android SDK + build tools to `~/android`, android NDK to `~/android/ndk/<ver>`:
      - Get [Android command line tools](https://developer.android.com/studio/index.html#command-line-tools-only) and extract to `~/android/cmdline-tools`
      - `./cmdline-tools/bin/sdkmanager --sdk_root=./ "build-tools;33.0.1" "ndk;25.2.9519653" "platforms;android-33" platform-tools tools`
   - flex and bison

2. Get Input SDK - it contains pre-built dependencies of libraries used by Input

   - Check what SDK version is currently in use - look for `INPUT_SDK_VERSION` in `.github/workflows/android.yml`
   - Download TWO Input SDKs for android - go to https://github.com/merginmaps/input-sdk/releases and download the built SDK.
   - Unpack the downloaded .tar.gz to `~/input-sdk/arm-android` and `~/input-sdk/arm64-android`

3. Get Qt libraries - Ubuntu's system libraries are too old, and currently Input SDK does not include Qt SDK.
 
   - You need both linux and android Qt installed!
   - Check what Qt version is currently in use - look for `QT_VERSION` in `.github/workflows/android.yml`
   - Download Qt online installer from https://www.qt.io/download-open-source
   - Use the online installer to install Qt to `~/Qt`

4. Build Input (update CMake command with the correct Qt and Input SDK versions)
```
  mkdir build
  cd build

  export ANDROID_SDK_ROOT=~/android;
  export ANDROID_NDK_ROOT=~/android/ndk/25.2.9519653;
  export ANDROID_NDK_PLATFORM=android-25;
  export QT_BASE=~/Qt/6.6.3;
  export INPUT_SDK_ANDROID_BASE=~/input-sdk;
  
  cmake \
    -DCMAKE_BUILD_TYPE=Release \
    -DQT_ANDROID_ABIS="arm64-v8a" \
    -DQT_HOST_PATH=$QT_BASE/gcc_64 \
    -DCMAKE_TOOLCHAIN_FILE=$QT_BASE/android_arm64_v8a/lib/cmake/Qt6/qt.toolchain.cmake \
    -DUSE_MM_SERVER_API_KEY=FALSE \
    -GNinja \
    ../input/
  
  ninja apk
```

## 4.2. Android on macOS

1. Get Qt libraries
 
   - You need both macos and android Qt installed!
   - Check what Qt version is currently in use - look for `QT_VERSION` in `.github/workflows/android.yml`
   - Download Qt online installer from https://www.qt.io/download-open-source
   - Use the online installer to install Qt to `~/Qt`
   - Needed packages can be found in the [section 2.3](#23-required-qt-packages)

2. Install Java

   - `brew install openjdk@17`, then make this java version default ``export JAVA_HOME=`usr/libexec/java_home -v 17` ``. Check if it default by executing `java --version`

3. Setup Android SDK & NDK [Automatic, via QtCreator]
   - This step can now be performed via QtCreator, if it for some reason fails/does not work, skip this step and continue with manual setup

   - Open QtCreator and navigate to `settings -> devices -> Android`, here:
      - JDK location: Add Path to Java version, e.g. `/opt/homebrew/Cellar/openjdk@17/17.0.11/libexec/openjdk.jdk/Contents/Home`
      - Android SDK location: set path to some empty writeable directory, e.g. `~/android`
      - Hit `Set up SDK` and install the current SDK version (find the correct version in `.github/workflows/android.yml`)
      - Let QtCreator install NDK
      - Let QtCreator install openssl
   - QtCreator should now say `Android settings are OK.`

4. Setup Android SDK & NDK [Manual, via sdkmanager]
   - Proceed with this step only if the previous automatic step did not work for you or you do not want to use QtCreator

   - Get Android `sdkmanager` by following these steps https://developer.android.com/tools/sdkmanager
   - Now perform `./cmdline-tools/bin/sdkmanager --sdk_root=./ "build-tools;33.0.1" "ndk;25.2.9519653" "platforms;android-33" platform-tools tools` to install all needed Android tools, make sure to double-check if the version numbers are correct

5. Get MM mobile SDK - it contains pre-built dependencies of used libraries (QGIS, etc..)

   - Check what SDK version is currently in use - look for `INPUT_SDK_VERSION` in `.github/workflows/android.yml`
   - Download TWO SDKs for android (arm and arm64) - go to https://github.com/merginmaps/mobile-sdk/releases and download the built SDK.
   - Unpack the downloaded .tar.gz to `~/mobile-sdk/arm-android` and `~/mobile-sdk/arm64-android`
   - WARNING!! It is super important to have both SDKs in same subfolder (e.g. `~/mobile-sdk`) and have folder name `arm64-android` and `arm-android`

6. Build (update CMake command with the correct Qt and SDK versions)

```
  # Needed Android variables
  export ANDROID_SDK_ROOT=~/android;
  export ANDROID_NDK_ROOT=~/android/ndk/25.1.8937393;
  export ANDROID_NDK_PLATFORM=android-24;

  # INPUT_SDK_ANDROID_BASE is a path where you stored the two SDKs from the mobile-sdk repo
  export INPUT_SDK_ANDROID_BASE=~/mobile-sdk;

  # (optional, not needed often) add Java to PATH if you need to use other Java version than your default one
  export PATH=/opt/homebrew/Cellar/openjdk@17/17.0.11/libexec/openjdk.jdk/Contents/Home/bin:$PATH;

  cmake \
    -DCMAKE_BUILD_TYPE=Debug \
    -DQT_ANDROID_ABIS=arm64-v8a \
    -DQT_HOST_PATH=/opt/Qt/6.6.3/macos \
    -DCMAKE_TOOLCHAIN_FILE=/opt/Qt/6.6.3/android_arm64_v8a/lib/cmake/Qt6/qt.toolchain.cmake \
    -DUSE_MM_SERVER_API_KEY=FALSE \
    -GNinja \
    ../input/

  # If you need to build both ABIS, use -DQT_ANDROID_ABIS="arm64-v8a;armeabi-v7a"
```

## 4.3. Android on Windows

**Note to document writers:**: This section needs a proper rewrite

Even technically it should be possible, we haven't tried this setup yet. If you managed to compile 
Mergin Maps Input Android on Windows, please help us to update this section. 

# 5. Building iOS

- you have to run Release or RelWithDebInfo builds. Debug builds will usually crash on some Qt's assert
- if there is any problem running Input App from Qt Creator, open cmake-generated project in XCode directly
  
1. Setup development environment
   - XCode
   - build tools, see requirements in `.github/workflows/ios.yml`
   - if you want to build for production, you need development certificates. These are not needed for local development, signing is handled automatically (see IOS_USE_PRODUCTION_SIGNING cmake variable for more info). You can get the certificates by following:
       - Get device UDID: either iTunes or about this mac->system report->USB->find iPAD (Serial Number)
       - Create dev iOS certificate for development
       - Create provisioning profile for Input App + your certificate + your device (for this ask Lutra Apple development team)
   - ios-toolchain 
       - download ios.toolchain.cmake from https://github.com/leetal/ios-cmake to `~/input-sdk/ios.toolchain.cmake`
       - version from `.github/workflows/ios.yml`

2. Get Input SDK - it contains pre-built dependencies of libraries used by Input

   - Check what SDK version is currently in use - look for `INPUT_SDK_VERSION` in `.github/workflows/ios.yml`
   - Download Input SDKs for ios - go to https://github.com/merginmaps/input-sdk/releases and download the built SDK.
   - Unpack the downloaded .tar.gz to `~/input-sdk/arm64-ios`

3. Get Qt libraries
 
   - You need both macos and ios Qt installed!
   - Check what Qt version is currently in use - look for `QT_VERSION` in `.github/workflows/ios.yml`
   - Download Qt online installer from https://www.qt.io/download-open-source
   - Use the online installer to install Qt to `/opt/Qt`

4. Build Input (update CMake command with the correct Qt and Input SDK versions)

Now you can create a build (either on commmand line or by setting these variables in Qt Creator)

```
  mkdir build
  mkdir install
  cd build

  cmake \
    -DIOS=TRUE \
    -DCMAKE_PREFIX_PATH=/opt/Qt/6.6.3/ios \
    -DQT_HOST_PATH=/opt/Qt/6.6.3/macos \
    -DCMAKE_TOOLCHAIN_FILE:PATH="~/input-sdk/ios.toolchain.cmake" \
    -DCMAKE_INSTALL_PREFIX:PATH="../install" \
    -DUSE_SERVER_API_KEY=FALSE \
    -DINPUT_SDK_PATH=~/input-sdk/arm64-ios \
    -G "Xcode" \
    ../input
```

# 6. Building macOS

1. Install some dependencies, critically XCode, bison and flex. See "Install Build Dependencies" step in `.github/workflows/macos.yml`

2. Get MM mobile SDK - it contains pre-built dependencies of used libraries

   - Check what SDK version is currently in use - look for `INPUT_SDK_VERSION` in `.github/workflows/macos.yml`

   - 2.1. For Intel chips

     - Download mobile SDK for `osx` - go to https://github.com/merginmaps/mobile-sdk/releases and download the built SDK.
     - Unpack the downloaded .tar.gz to `~/mobile-sdk/x64-osx`

   - 2.2. For Apple M chips
     - You need to build SDK yourself, follow steps how to do it here https://github.com/merginmaps/mobile-sdk
     - Your SDK will be installed in the `build folder/vcpkg_installed/arm64-osx/`

3. Get Qt libraries - Mobile SDK does not include Qt SDK

   - Check what Qt version is currently in use - look for `QT_VERSION` in `.github/workflows/macos.yml`
   - Download Qt online installer from https://www.qt.io/download-open-source
   - Use the online installer to install Qt to `/opt/Qt`

4. Build (update CMake command with the correct Qt and paths)

```
mkdir build-desktop 
cd build-desktop

cmake \
  -DCMAKE_BUILD_TYPE=Debug \
  -DCMAKE_PREFIX_PATH=/opt/Qt/6.6.3/macos \
  -DINPUT_SDK_PATH=<path_to_mobile_sdk_from_step_2> \
  -DQGIS_QUICK_DATA_PATH=<path_to_mobile_repo>/app/android/assets/qgis-data \
  -GNinja \
  ..

ninja
```

5. Run the app
```
./app/Input.app/Contents/MacOS/Input
```

# 7. Building Windows

For version of the tools used, see `.github/workflows/win.yml`

- download input-sdk for win, extract to C:\projects\input-sdk\win-sdk
- download and install Qt
- download and install QtCreator for debugging executable
- download and install Visual Studio, SDK, Python, Cmake
- (optional) install ccache and add to PATH
- open cmd
- setup build environment
```
set ROOT_DIR=C:\Users\zilol\Projects
set Qt6_DIR=C:\Qt\6.6.3\msvc2019_64
set PATH=%QT_ROOT%\bin;C:\Program Files\CMake\bin\;%PATH%
"C:\Program Files (x86)\Microsoft Visual Studio\2019\Community\Common7\Tools\VsDevCmd.bat" -arch=x64
```

- configure
```
cd %ROOT_DIR%\build\input

cmake ^
   -DCMAKE_BUILD_TYPE=Release ^
   -DCMAKE_PREFIX_PATH:PATH=%Qt6_Dir%^
   -DCMAKE_INSTALL_PREFIX:PATH=%ROOT_DIR%\install\input ^
   -DINPUT_SDK_PATH:PATH=%ROOT_DIR%\input-sdk\x64-windows ^
   -G "NMake Makefiles" ^
   -DCMAKE_CXX_COMPILER_LAUNCHER=ccache ^
   -S %ROOT_DIR%\input ^
   -B .
```

- build 
```
set CL=/MP
nmake
```

# 8. Auto Testing

You need to add cmake define `-DENABLE_TESTING=TRUE` on your cmake configure line.
Also you need to open Passbolt and check for password for user `test_mobileapp` on `app.dev.merginmaps.com`, 
or you need some user with unlimited projects limit. First workspace from list is taken.

! Note that the same user cannot run tests in paraller ! This user is used for CI, consider creating your own account for local development !

now you need to set environment variables: 
```
TEST_MERGIN_URL=https://app.dev.merginmaps.com/
TEST_API_USERNAME=test_mobileapp
TEST_API_PASSWORD=<your_password>
```

Build binary and you can run tests either with `ctest` or you can run individual tests by adding `--test<TestName>`
e.g. ` ./input --testMerginApi`
