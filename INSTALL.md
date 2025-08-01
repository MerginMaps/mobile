# Table of Contents
<!-- Table of contents generated with https://freelance-tech-writer.github.io/table-of-contents-generator/index.html -->

- [Table of Contents](#table-of-contents)
- [1. Introduction](#1-introduction)
- [2. Overview](#2-overview)
  - [2.1 Secrets](#21-secrets)
  - [2.2 Code formatting](#22-code-formatting)
- [3. Building GNU/Linux](#3-building-gnulinux)
- [4. Building Android (on Linux/macOS/Windows)](#4-building-android-on-linuxmacoswindows)
  - [4.1. Android on Ubuntu](#41-android-on-ubuntu)
  - [4.2. Android on macOS](#42-android-on-macos)
  - [4.3. Android on Windows](#43-android-on-windows)
- [5. Building iOS](#5-building-ios)
- [6. Building macOS](#6-building-macos)
- [7. Building Windows](#7-building-windows)
- [8. Common problems](#8-common-problems)
- [9. Auto Testing](#9-auto-testing)

# 1. Introduction

This document is the original installation guide of the described software
Mergin Maps mobile app. The software and hardware descriptions named in this
document are in most cases registered trademarks and are therefore subject
to the legal requirements. Mergin Maps mobile app is subject to the GNU General Public
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

Mobile app, like a number of major projects (e.g., KDE),
uses [CMake](https://www.cmake.org) for building from source.

It is C++ application build on top of [Qt](https://www.qt.io), [QGIS](https://www.qgis.org/en/site/)
and many other FOSS libraries. 

All required libraries are build with [vcpkg](https://vcpkg.io/en/) C/C++ package manager as part of the CMake's configure step.

Generally, for building setup, we recommend to use the same versions of libraries/SDKs/NDKs/compilers as used in the official 
[GitHub Actions](https://github.com/MerginMaps/mobile/tree/master/.github/workflows).
Open workflow file for your platform/target and see the version of libraries used and replicate it in your setup.

## 2.1 Secrets

To communicate with MerginAPI, some endpoints need to attach `api_key`. To not leak API_KEY,
the source code that returns the API_KEYS is encrypted.

As a developer, if you want to develop against local Mergin Maps server, it is OK, but to
work with public or dev server, you need to manually decrypt the file. Decrypted file is never
pushed to git!

The password for decryption is in Lutra's password manager.

if you want to change the secrets, decrypt, change and encrypt the file with
[openssl](http://stackoverflow.com/questions/16056135/ddg#16056298), also
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
We use `cmake-format` to format CMake files. 

All the scripts are located in `scripts/format_*` and you can check 
[GitHub Actions](https://github.com/MerginMaps/mobile/tree/master/.github/workflows/code_style.yml) to see
their usage

For more details about code conventions, please read our [code conventions doc](./docs/code_convention.md).

## 2.3 Qt packages

Mergin Maps Mobile app is built with Qt. Qt is build with vcpkg as part of the configure step, but it is recommended
to install QtCreator and Qt on your host to be able to release translations. 

## 2.4 Vcpkg

Dependencies are build with vcpkg. To fix the version of libraries, you need to download vcpkg and checkout to git commit specified
in the file `VCPKG_BASELINE` in the repository. 

   ```
   mkdir -p vcpkg
   cd vcpkg
   git init
   git remote add origin https://github.com/microsoft/vcpkg.git
   git pull origin master
   VCPKG_TAG=`cat VCPKG_BASELINE`
   git checkout ${VCPKG_TAG}
   cd "${{ env.VCPKG_ROOT }}"
   chmod +x ./bootstrap-vcpkg.sh
   ./bootstrap-vcpkg.sh
   ```

## 2.4 ccache

Install and configure ccache for development. It speeds up the development significantly.
 
# 3. Building GNU/Linux

## 3.1 Ubuntu 22.04

Steps to build and run mobile app:

1. Install some dependencies, critically bison and flex. See "Install Build Dependencies" step in `.github/workflows/linux.yml`

   ```
   sudo apt-get install -y \
      gperf autopoint '^libxcb.*-dev' libx11-xcb-dev libegl1-mesa libegl1-mesa-dev \
      libglu1-mesa-dev libxrender-dev libxi-dev libxkbcommon-dev libxkbcommon-x11-dev \
      autoconf-archive libgstreamer-gl1.0-0 libgstreamer-plugins-base1.0-0 libfuse2 \
      bison flex lcov nasm libxrandr-dev xvfb
   ```
   
   Also install CMake 3.x, Ninja and ccache

2. Install vcpkg and checkout to correct version from file `VCPKG_BASELINE`
   Read [vcpkg](#vcpkg) section.
   
 
3. Configure mobile app
   We assume the structure on the system:
   ```
   mm1/
     build/
     vcpkg/
     mobile/ 
   ```
   
   This is command line to setup build system. As part of the cmake configure step it will compile all the deps (Qt, GDAL, QGIS), so it 
   can take considerable time (e.g. an hour). Subsequent runs will be faster as the libraries without change will be taken from local 
   binary vcpkg cache.
   
   Alternatively you can open QtCreator and add cmake defines to the QtCreator Project setup table and configure from QtCreator (recommended for
   development and debugging)
   
   To use USE_MM_SERVER_API_KEY read [Secrets](#Secrets) section.
   
   ```
   mkdir -p build
   cd build
   cmake \
      -DCMAKE_BUILD_TYPE=Debug \
      -DVCPKG_TARGET_TRIPLET=x64-linux \
      -DCMAKE_TOOLCHAIN_FILE=../vcpkg/scripts/buildsystems/vcpkg.cmake \
      -DQGIS_QUICK_DATA_PATH=${{ github.workspace }}/mobile/app/android/assets/qgis-data \
      -DUSE_MM_SERVER_API_KEY=FALSE \
      -DCMAKE_CXX_COMPILER_LAUNCHER=ccache \
      -DENABLE_TESTS=TRUE \
      -GNinja \
      -S ../mobile
   ```
4. Build application 
   
   ```
   ninja
   ```
   
5. Run mobile app

   ```
   ./app/Input
   ```
   
   For testing read [Auto Testing](#AutoTesting) section.

# 4. Building Android (on Linux/macOS/Windows)

For building ABIs see https://www.qt.io/blog/android-multi-abi-builds-are-back

## 4.1. Android on Linux

1. Install some dependencies, see requirements in `.github/workflows/android.yml`

   - Java 17 (on Ubuntu 22.04 do `sudo apt install openjdk-17-jdk` and make sure it is the default by checking `java --version`)
   - android SDK + build tools to `~/android`, android NDK to `~/android/ndk/<ver>`:
      - Get [Android command line tools](https://developer.android.com/studio/index.html#command-line-tools-only) and extract to `~/android/cmdline-tools`
      - See current versions of build tools (`SDK_BUILD_TOOLS`), ndk (`NDK_VERSION`) and platform (`SDK_PLATFORM`) in `.github/workflows/android.yml`
      - `./cmdline-tools/bin/sdkmanager --sdk_root=./ "build-tools;<current_version>" "ndk;<current_version>" "platforms;<current_version>" platform-tools tools`
   - flex and bison
   - set up your own developer keystore. Creating the key(store) can be done either with Android studio or on command line 
     with `keytool -genkeypair`.

2. Build mobile app (update CMake command with the correct Qt and Android NDK versions)

   We recommended to have **RelWithDebInfo** builds, which requires signing the APK with your key. 

   We assume the structure on the system:

   ```
   mm1/
     build/
     vcpkg/
     mobile/ 
   ```

   This is command line to setup build system. As part of the cmake configure step it will compile all the deps (Qt, GDAL, QGIS), so it
   can take considerable time (e.g. an hour). Subsequent runs will be faster as the libraries without change will be taken from local
   binary vcpkg cache.

   ```
     export ANDROID_NDK_HOME=/home/<user>/android/ndk/<current_version>
     export ANDROID_SDK_ROOT=/home/<user>/android
     export QT_ANDROID_KEYSTORE_ALIAS=<local-alias>
     export QT_ANDROID_KEYSTORE_KEY_PASS=<password>
     export QT_ANDROID_KEYSTORE_STORE_PASS=<password>
     export QT_ANDROID_KEYSTORE_PATH=<keystore-path>
   
     cmake \
       -DCMAKE_BUILD_TYPE=RelWithDebInfo \
       -DANDROID_ABI=arm64-v8a \
       -DQT_ANDROID_ABIS=arm64-v8a \
       -DVCPKG_HOST_TRIPLET=x64-linux \
       -DVCPKG_TARGET_TRIPLET=arm64-android \
       -DCMAKE_TOOLCHAIN_FILE=<path-to-directory>/vcpkg/scripts/buildsystems/vcpkg.cmake \
       -DVCPKG_INSTALL_OPTIONS="--allow-unsupported" \
       -DUSE_MM_SERVER_API_KEY=TRUE \
       -DCMAKE_CXX_COMPILER_LAUNCHER=ccache \
       -DANDROID_SDK_ROOT=/home/<user>/android \
       -DQT_ANDROID_SIGN_APK=Yes
       -GNinja \
       -S ../mobile \
       -B ./
   ```

  Alternatively you can open QtCreator and add cmake defines to the QtCreator Project setup table and configure from QtCreator (recommended for
  development and debugging)


  Add this to build env.
     
  ```
       PATH=+/Users/<user>/Projects/quick/build/vcpkg
       ANDROID_NDK_HOME=/Users/<user>/android/ndk/<current_version>
       ANDROID_SDK_ROOT=/Users/<user>/android
       QT_ANDROID_KEYSTORE_ALIAS=<local-alias>
       QT_ANDROID_KEYSTORE_KEY_PASS=<password>
       QT_ANDROID_KEYSTORE_STORE_PASS=<password>
       QT_ANDROID_KEYSTORE_PATH=<keystore-path>
  ```
  And this to cmake options
     
  ```
       ANDROID_ABI=arm64-v8a 
       QT_ANDROID_ABIS=arm64-v8a 
       VCPKG_HOST_TRIPLET=x64-linux 
       VCPKG_TARGET_TRIPLET=arm64-android 
       CMAKE_TOOLCHAIN_FILE="../vcpkg/scripts/buildsystems/vcpkg.cmake" 
       VCPKG_INSTALL_OPTIONS="--allow-unsupported" 
       CMAKE_CXX_COMPILER_LAUNCHER=ccache 
       ANDROID_SDK_ROOT=/Users/<user>/android 
       QT_ANDROID_SIGN_APK=Yes 
  ```
  
  
  To use USE_MM_SERVER_API_KEY read [Secrets](#Secrets) section.


## 4.2. Android on macOS
1. Install Java

   - `brew install openjdk@17`, then make this java version default ``export JAVA_HOME=`usr/libexec/java_home -v 17` ``. Check if it's default by executing `java --version`

2. Setup Android SDK & NDK 
   - This step can now be performed via QtCreator, if it for some reason fails/does not work, skip this step and continue with manual setup

   - Open QtCreator and navigate to `settings -> devices -> Android`, here:
      - JDK location: Add Path to Java version, e.g. `/opt/homebrew/Cellar/openjdk@17/17.0.11/libexec/openjdk.jdk/Contents/Home` (or /opt/homebrew/Cellar/openjdk@17/17.0.15/libexec/openjdk.jdk/Contents/Home/)
      - Android SDK location: set path to some empty writeable directory, e.g. `~/android`
      - Hit `Set up SDK` and install the current SDK version (find the correct version in `.github/workflows/android.yml`)
      - Let QtCreator install NDK
      - Let QtCreator install openssl
   - QtCreator should now say `Android settings are OK.`

   - If the previous automatic step did not work for you or you do not want to use QtCreator
     - Get Android `sdkmanager` by following these steps https://developer.android.com/tools/sdkmanager
     - See current versions of build tools (`SDK_BUILD_TOOLS`), ndk (`NDK_VERSION`) and platform (`SDK_PLATFORM`) in `.github/workflows/android.yml`
     - Now perform `./cmdline-tools/bin/sdkmanager --sdk_root=./ "build-tools;<current_version>" "ndk;<current_version>" "platforms;<current_version>" platform-tools tools` to install all needed Android tools, make sure to double-check if the version numbers are correct

3. Configure 
  
   We recommended to have **RelWithDebInfo** builds

   We assume the structure on the system:
   
   ```
   mm1/
     build/
     vcpkg/
     mobile/ 
   ```
   
   This is command line to setup build system. As part of the cmake configure step it will compile all the deps (Qt, GDAL, QGIS), so it 
   can take considerable time (e.g. an hour). Subsequent runs will be faster as the libraries without change will be taken from local 
   binary vcpkg cache.
   
   ```
     export ANDROID_NDK_HOME=/Users/<user>/android/ndk/<current_version>
     export ANDROID_SDK_ROOT=/Users/<user>/android
     export QT_ANDROID_KEYSTORE_ALIAS=<local-alias>
     export QT_ANDROID_KEYSTORE_KEY_PASS=<password>
     export QT_ANDROID_KEYSTORE_STORE_PASS=<password>
     export QT_ANDROID_KEYSTORE_PATH=<keystore-path>

     cmake \
       -DCMAKE_BUILD_TYPE=RelWithDebInfo \
       -DANDROID_ABI=arm64-v8a \
       -DQT_ANDROID_ABIS=arm64-v8a \
       -DVCPKG_HOST_TRIPLET=arm64-osx \
       -DVCPKG_TARGET_TRIPLET=arm64-android \
       -DCMAKE_TOOLCHAIN_FILE="../vcpkg/scripts/buildsystems/vcpkg.cmake" \
       -DVCPKG_INSTALL_OPTIONS="--allow-unsupported" \
       -DCMAKE_CXX_COMPILER_LAUNCHER=ccache \
       -DANDROID_SDK_ROOT=/Users/<user>/android \
       -DQT_ANDROID_SIGN_APK=Yes \
       -GNinja \
       -S ../mobile \
       -B ./
   ```
   
   Alternatively you can open QtCreator and add cmake defines to the QtCreator Project setup table and configure from QtCreator (recommended for
   development and debugging)
   

   Add this to build env.
   ```
     PATH=+/opt/homebrew/bin
     PATH=+/opt/homebrew/Cellar/flex/2.6.4_2/bin
     PATH=+/opt/homebrew/Cellar/bison/3.8.2/bin
     PATH=+/Users/<user>/Projects/quick/build/vcpkg
     ANDROID_NDK_HOME=/Users/<user>/android/ndk/<current_version>
     ANDROID_SDK_ROOT=/Users/<user>/android
     QT_ANDROID_KEYSTORE_ALIAS=<local-alias>
     QT_ANDROID_KEYSTORE_KEY_PASS=<password>
     QT_ANDROID_KEYSTORE_STORE_PASS=<password>
     QT_ANDROID_KEYSTORE_PATH=<keystore-path>
   ```
   And this to cmake options
   ```
     ANDROID_ABI=arm64-v8a 
     QT_ANDROID_ABIS=arm64-v8a 
     VCPKG_HOST_TRIPLET=arm64-osx 
     VCPKG_TARGET_TRIPLET=arm64-android 
     CMAKE_TOOLCHAIN_FILE="../vcpkg/scripts/buildsystems/vcpkg.cmake" 
     VCPKG_INSTALL_OPTIONS="--allow-unsupported" 
     CMAKE_CXX_COMPILER_LAUNCHER=ccache 
     ANDROID_SDK_ROOT=/Users/<user>/android 
     QT_ANDROID_SIGN_APK=Yes 
   ```
   
   
   To use USE_MM_SERVER_API_KEY read [Secrets](#Secrets) section.
   

## 4.3. Android on Windows

Even technically it should be possible, we haven't tried this setup yet. If you managed to compile 
mobile app for Android on Windows, please help us to update this section. 

# 5. Building iOS

- you have to run Release or RelWithDebInfo builds. Debug builds will usually crash on some Qt's assert
- if there is any problem running mobile app from Qt Creator, open cmake-generated project in XCode directly
  
1. Setup development environment
   - XCode
   - install deps, see requirements in `.github/workflows/ios.yml`. Most specifically cmake 3.x, ninja, bison and flex (on PATH)
   - if you want to build for production, you need development certificates. These are not needed for local development, signing is handled automatically (see IOS_USE_PRODUCTION_SIGNING cmake variable for more info). You can get the certificates by following:
       - Get device UDID: either iTunes or about this mac->system report->USB->find iPAD (Serial Number)
       - Create dev iOS certificate for development
       - Create provisioning profile for mobile app + your certificate + your device (for this ask Lutra Apple development team)

2. Install vcpkg and checkout to correct version from file `VCPKG_BASELINE`
   Read [vcpkg](#vcpkg) section.
 
3. Configure mobile app
   We assume the structure on the system:
   
   ```
   mm1/
     build/
     vcpkg/
     mobile/ 
   ```
   
   This is command line to setup build system. As part of the cmake configure step it will compile all the deps (Qt, GDAL, QGIS), so it can take considerable time (e.g. an hour). Subsequent runs will be faster as the libraries without change will be taken from local binary vcpkg cache.
   
   Alternatively you can open QtCreator and add cmake defines to the QtCreator Project setup table and configure from QtCreator (recommended for development and debugging)
   
   To use USE_MM_SERVER_API_KEY read [Secrets](#Secrets) section.
   
   Note: make sure you adjust VCPKG_HOST_TRIPLET and CMAKE_SYSTEM_PROCESSOR if you use x64-osx host machine.
   
   ```
   cd build
   
   export PATH=$(brew --prefix flex)/bin:$(brew --prefix bison)/bin:$(brew --prefix gettext)/bin:$PATH;\
   export PATH=${PWD}/../vcpkg:$PATH;\
   PATH=/Applications/CMake.app/Contents/bin/:$PATH
   
   cmake \
     -DVCPKG_HOST_TRIPLET=arm64-osx \
     -DCMAKE_SYSTEM_PROCESSOR=aarch64 \
     -DVCPKG_TARGET_TRIPLET=arm64-ios \
     -DCMAKE_TOOLCHAIN_FILE=../vcpkg/scripts/buildsystems/vcpkg.cmake \
     -D ENABLE_BITCODE=OFF \
     -D ENABLE_ARC=ON \
     -D CMAKE_CXX_VISIBILITY_PRESET=hidden \
     -D CMAKE_SYSTEM_NAME=iOS \
     -DIOS=TRUE \
     -DUSE_MM_SERVER_API_KEY=FALSE \
     -G "Xcode" \
     -DCMAKE_CXX_COMPILER_LAUNCHER=ccache \
     -S ../mobile \
     -B ./
   ```

4. Build mobile app

Now you can create a build (either on command line or by setting these variables in Qt Creator)

```
  cd build

  xcodebuild \
    -project Input.xcodeproj/ \
    -scheme Input \
    -sdk iphoneos \
    -configuration Release \
    archive -archivePath Input.xcarchive
```

# 6. Building macOS

1. Install some dependencies, critically XCode, bison and flex. See "Install Build Dependencies" step in `.github/workflows/macos.yml`
```
   brew install cmake automake bison flex gnu-sed autoconf-archive libtool ninja pkg-config
```
   install cmake 3.x
   
   Note: 
   - cmake 4.0.1 is broken (empty -isysroot)
   - TODO test with cmake 4.0.3 which is today's 

2. Install vcpkg and checkout to correct version from file `VCPKG_BASELINE`
   Read [vcpkg](#vcpkg) section.
 
3. Configure mobile app
   We assume the structure on the system:
   
   ```
   mm1/
     build/
     vcpkg/
     mobile/
   ```
   
   This is command line to setup build system. As part of the cmake configure step it will compile all the deps (Qt, GDAL, QGIS), so it can take considerable time (e.g. an hour). Subsequent runs will be faster as the libraries without change will be taken from local binary vcpkg cache.
   
   Alternatively you can open QtCreator and add cmake defines to the QtCreator Project setup table and configure from QtCreator (recommended for development and debugging)
   
   To use USE_MM_SERVER_API_KEY read [Secrets](#Secrets) section.
    
   Note: for x64-osx (intel laptops) build use VCPKG_TARGET_TRIPLET instead of arm64-osx (Mx laptops)
   
   ```
   cd build
   
   export PATH=$(brew --prefix flex)/bin:$(brew --prefix bison)/bin:$(brew --prefix gettext)/bin:$PATH;\
   export PATH=${PWD}/../vcpkg:$PATH;\
   PATH=/Applications/CMake.app/Contents/bin/:$PATH

   cmake \
      -DCMAKE_BUILD_TYPE=Debug \
      -DVCPKG_TARGET_TRIPLET=arm64-osx \
      -DCMAKE_TOOLCHAIN_FILE=../vcpkg/scripts/buildsystems/vcpkg.cmake \
      -DUSE_MM_SERVER_API_KEY=FALSE \
      -DQGIS_QUICK_DATA_PATH=../mobile/app/android/assets/qgis-data \
      -DCMAKE_CXX_COMPILER_LAUNCHER=ccache \
      -DENABLE_TESTS=TRUE \
      -GNinja \
      -DCMAKE_MAKE_PROGRAM=ninja \
      -S ../mobile
   ```
 
4. Build application

   ```
   ninja
   ```

5. Run the mobile app
   ```
   ./app/Input.app/Contents/MacOS/Input
   ```

# 7. Building Windows

1. Install some dependencies. See `.github/workflows/win.yml`
  Critically Visual Studio, cmake, bison and flex. Setup build VS environment (adjust to your version)
  ```
  "C:\Program Files\Microsoft Visual Studio\2022\Enterprise\Common7\Tools\VsDevCmd.bat" -arch=x64
  ```

2. Install vcpkg and checkout to correct version from file `VCPKG_BASELINE`
   Read [vcpkg](#vcpkg) section.

3. Configure mobile app
   We assume the structure on the system:
   
   ```
   mm1/
     build/
     vcpkg/
     mobile/ 
   ```
   
   This is command line to setup build system. As part of the cmake configure step it will compile all the deps (Qt, GDAL, QGIS), so it 
   can take considerable time (e.g. an hour). Subsequent runs will be faster as the libraries without change will be taken from local 
   binary vcpkg cache.
   
   Alternatively you can open QtCreator and add cmake defines to the QtCreator Project setup table and configure from QtCreator (recommended for
   development and debugging)
   
   To use USE_MM_SERVER_API_KEY read [Secrets](#Secrets) section.
   
   ```
   mkdir build
   cd build

   cmake ^
     -DCMAKE_BUILD_TYPE=Debug ^
     -DCMAKE_TOOLCHAIN_FILE:PATH="../vcpkg/scripts/buildsystems/vcpkg.cmake" ^
     -G "Visual Studio 17 2022" ^
     -A x64 ^
     -DVCPKG_TARGET_TRIPLET=x64-windows ^
     -DUSE_MM_SERVER_API_KEY=FALSE ^
     -DCMAKE_CXX_COMPILER_LAUNCHER=ccache ^
     -S ../mobile ^
     -B .
   ```
 
4. Build application

   ```
   cd build
   cmake --build . --config Release --verbose
   ```

5. Run the mobile app
   ```
   ./app/input.exe
   ```

# 8. Common problems

- If you have "error: undefined reference to 'stdout'" or so, make sure that in BUILD ENV you have ANDROID_NDK_PLATFORM=android-24 or later!
    ![image](https://user-images.githubusercontent.com/22449698/166630970-a776576f-c505-4265-b4c8-ffbe212c6745.png)
- If for all projects the OSM layer fails to load the `QGIS_QUICK_DATA_PATH` is probably wrong
  - Check where the projects are getting created
  - Either change the path to point to `app/android/assets/qgis-data` of the project directory

# 9. Auto Testing

You need to add cmake define `-DENABLE_TESTING=TRUE` on your cmake configure line.
Also, you need to open Passbolt and check for password for user `test_mobileapp` on `app.dev.merginmaps.com`, 
or you need some user with unlimited projects limit. First workspace from list is taken.

! Note that the same user cannot run tests in parallel ! This user is used for CI, consider creating your own account for local development !

now you need to set environment variables: 
```
TEST_MERGIN_URL=https://app.dev.merginmaps.com/
TEST_API_USERNAME=test_mobileapp
TEST_API_PASSWORD=<your_password>
```

Build binary, and you can run tests either with `ctest` or you can run individual tests by adding `--test<TestName>`
e.g. ` ./input --testMerginApi`