Building Mergin Maps Input from source - step by step


# Table of Contents
<!-- Table of contents generated with https://freelance-tech-writer.github.io/table-of-contents-generator/index.html -->

* [1. Introduction](#1-introduction)
* [2. Overview](#2-overview)
   * [2.1 Secrets](#21-secrets)
   * [2.2 Code formatting](#22-code-formatting)
* [3. Building GNU/Ubuntu](#3-building-gnuubuntu)
* [4. Building Android (on Ubuntu/macOS/Windows)](#4-building-android-on-ubuntumacoswindows)
   * [4.1. Android on Ubuntu](#41-android-on-ubuntu)
   * [4.2. Android on macOS](#42-android-on-macos)
   * [4.3. Android on Windows](#43-android-on-windows)
* [5. Building iOS](#5-building-ios)
* [6. Building macOS](#6-building-macos)
* [7. Building Windows](#7-building-windows)


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

All required libraries (in release configuration) are packaged in the [Input-SDK](https://github.com/MerginMaps/input-sdk).
If you need to debug some error in the library, you need to compile Input-SDK in debug yourself locally. Otherwise
it is suggested to download required libraries from [Input-SDK tags](https://github.com/MerginMaps/input-sdk/tags)
Input-SDK uses vcpkg packaging system, so if the SDK for your target system/architecture you can build it yourself.

Generally, for building setup, you need the same versions of libraries/SDKs/NDKs/compilers as used in the official 
[GitHub Actions](https://github.com/MerginMaps/input/tree/master/.github/workflows).
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

For QML files we use latest version of `jesperhh/qmlfmt`. To install, follow instruction on the GitHub of the project. 

We also use software [pre-commit](https://pre-commit.com/) to automatically check format when doing a commit.
You need to install it via `brew`/`pip`, see [installation details](https://pre-commit.com/#installation).

In order to start using the `pre-commit`, run `pre-commit install` in the repository root folder.

To manually run the style check, run `pre-commit run --all-files` or optionally run script `format_cpp.sh` (we use this one in CI currently).

In case you want to skip execution of pre-commit hooks, add additional flag `--no-verify` to your commit command, e.g.: `git commit -m "nit" --no-verify`

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
     -DCMAKE_PREFIX_PATH=~/Qt/6.5.2/gcc_64 \
     -DINPUT_SDK_PATH=~/input-sdk/x64-linux \
     -DQGIS_QUICK_DATA_PATH=~/input/app/android/assets/qgis-data \
     ..
   ninja
   ```

5. Run Input

   ```
   ./app/input
   ```


# 4. Building Android (on Ubuntu/macOS/Windows)

For building ABIS see https://www.qt.io/blog/android-multi-abi-builds-are-back

If you have "error: undefined reference to 'stdout'" or so, make sure that in BUILD ENV you have ANDROID_NDK_PLATFORM=android-24 or later!

![image](https://user-images.githubusercontent.com/22449698/166630970-a776576f-c505-4265-b4c8-ffbe212c6745.png)

## 4.1. Android on Ubuntu

1. Install some dependencies, see requirements in `.github/workflows/android.yml`

   - android SDK + build tools to `~/android`
   - android NDK to `~/android/ndk/<ver>`
   - JAVA
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
  export ANDROID_NDK_ROOT=~/android/ndk/25.1.8937393;
  export ANDROID_NDK_PLATFORM=android-24;
  export QT_BASE=/opt/Qt/6.5.2;
  export INPUT_SDK_ANDROID_BASE=~/input-sdk;
  
  cmake \
    -DCMAKE_BUILD_TYPE=Release \
    -DQT_ANDROID_ABIS="arm64-v8a" \
    -DQT_HOST_PATH=$QT_BASE/macos \
    -DCMAKE_TOOLCHAIN_FILE=$QT_BASE/android_arm64_v8a/lib/cmake/Qt6/qt.toolchain.cmake \
    -DUSE_SERVER_API_KEY=FALSE \
    -GNinja \
    ../input/
  
    ninja apk
```

## 4.2. Android on macOS

1. Install some dependencies, see requirements in `.github/workflows/android.yml`

   - android SDK + build tools to `~/android`
   - android NDK to `~/android/ndk/<ver>`
   - JAVA
   - flex and bison (add to PATH)

2. Get Input SDK - it contains pre-built dependencies of libraries used by Input

   - Check what SDK version is currently in use - look for `INPUT_SDK_VERSION` in `.github/workflows/android.yml`
   - Download TWO Input SDKs for android - go to https://github.com/merginmaps/input-sdk/releases and download the built SDK.
   - Unpack the downloaded .tar.gz to `~/input-sdk/arm-android` and `~/input-sdk/arm64-android`
   - WARNING!! It is super important to have both SDKs in same subfolder (e.g. `~/input-sdk`) and have folder name `arm64-android` and `arm-android`

3. Get Qt libraries
 
   - You need both macos and android Qt installed!
   - Check what Qt version is currently in use - look for `QT_VERSION` in `.github/workflows/android.yml`
   - Download Qt online installer from https://www.qt.io/download-open-source
   - Use the online installer to install Qt to `~/Qt`

4. Build Input (update CMake command with the correct Qt and Input SDK versions)
```
  export ANDROID_SDK_ROOT=/opt/Android/android-sdk;
  export ANDROID_NDK_ROOT=/opt/Android/android-sdk/ndk/25.1.8937393;
  export QT_BASE=/opt/Qt/6.5.2;
  export ANDROID_NDK_PLATFORM=android-24;
  export INPUT_SDK_ANDROID_BASE=~/Projects/quick/input-sdk/build/android/stage;
  export PATH=/usr/local/Cellar/openjdk\@11/11.0.16.1_1/bin/:$PATH;
  cmake \
    -DCMAKE_BUILD_TYPE=Release \
    -DQT_ANDROID_ABIS="arm64-v8a;armeabi-v7a" \
    -DQT_HOST_PATH=$QT_BASE/macos \
    -DCMAKE_TOOLCHAIN_FILE=$QT_BASE/android_arm64_v8a/lib/cmake/Qt6/qt.toolchain.cmake \
    -DUSE_MM_SERVER_API_KEY=FALSE \
    -GNinja \
    ../input/
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
   - to install app to your iOS device, you need development certificate
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
    -DCMAKE_PREFIX_PATH=/opt/Qt/6.5.2/ios \
    -DQT_HOST_PATH=/opt/Qt/6.5.2/macos \
    -DCMAKE_TOOLCHAIN_FILE:PATH="~/input-sdk/ios.toolchain.cmake" \
    -DCMAKE_INSTALL_PREFIX:PATH="../install" \
    -DUSE_SERVER_API_KEY=FALSE \
    -DINPUT_SDK_PATH=~/input-sdk/arm64-ios \
    -G "Xcode" \
    ../input
```

# 6. Building macOS

1. Install some dependencies, critically XCode, bison and flex. See "Install Build Dependencies" step in `.github/workflows/macos.yml`

2. Get Input SDK - it contains pre-built dependencies of libraries used by Input

   - Check what SDK version is currently in use - look for `INPUT_SDK_VERSION` in `.github/workflows/macos.yml`
   - Download Input SDK for Ubuntu - go to https://github.com/merginmaps/input-sdk/releases and download the built SDK.
   - Unpack the downloaded .tar.gz to `~/input-sdk/x64-osx`

3. Get Qt libraries - Ubuntu's system libraries are too old, and currently Input SDK does not include Qt SDK.

   - Check what Qt version is currently in use - look for `QT_VERSION` in `.github/workflows/macos.yml`
   - Download Qt online installer from https://www.qt.io/download-open-source
   - Use the online installer to install Qt to `/opt/Qt`

4. Build Input (update CMake command with the correct Qt and Input SDK versions)
 
```
mkdir build-input-desktop
cd build-input-desktop

export PATH=$(brew --prefix flex)/bin:$(brew --prefix bison)/bin:$(brew --prefix gettext)/bin:$PATH;\
export BASE_DIR=~/Projects/quick;

cmake \
  -DCMAKE_BUILD_TYPE=Debug \
  -DCMAKE_PREFIX_PATH=/opt/Qt/6.5.2/macos \
  -DCMAKE_INSTALL_PREFIX:PATH=$BASE_DIR/install-macos \
  -DINPUT_SDK_PATH=$BASE_DIR/sdk/x64-osx \
  -GNinja \
  -DQGIS_QUICK_DATA_PATH=$BASE_DIR/input/app/android/assets/qgis-data \
  $BASE_DIR/input

ninja
```

5. Run Input
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
set Qt6_DIR=C:\Qt\6.5.2\msvc2019_64
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

