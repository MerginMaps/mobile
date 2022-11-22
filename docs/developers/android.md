looking for android [publishing](./publishing.md)

# Development for Android

Tested: Linux, macOS

Notes:

1. If you have "error: undefined reference to 'stdout'" or so, make sure that in BUILD ENV you have ANDROID_NDK_PLATFORM=android-24 or later!

![image](https://user-images.githubusercontent.com/22449698/166630970-a776576f-c505-4265-b4c8-ffbe212c6745.png)

## Development Linux Cross-Compilation for Android

Requirements Android:
- input-sdk for Android
- Qt6 for android
- android-ndk
- android-sdk

```
  mkdir build
  cd build

  export ANDROID_SDK_ROOT=/home/martin/android;
  export ANDROID_NDK_ROOT=/home/martin/android/ndk/23.1.7779620;
  export QT_BASE=/opt/Qt/6.4.1;
  export INPUT_SDK_ANDROID_BASE=/home/martin/input-sdk/android-macOS-20221117-129;
  
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

## Development MacOS Cross-Compilation for Android

Same requirements as for Linux Cross-Compilation for Android

Quick guide:
- `brew install openjdk@11`
- `brew install ant`
- `brew install bison`
    
- download SDK command line tools and unzip to `/opt/android-sdk`
- sdk: install lldb, build tools, platform android X, cmake, platform-tools
- download QT armv7 to `/opt/Qt`
- download NDK and install to `/opt/android-ndk-<ver>`
- compile input-SDK
- open QtCreator -> Manage Kits -> add SDK and NDK. compilers should be autodetected
- enable connection on the device from MacOS when requested

For building ABIS see https://www.qt.io/blog/android-multi-abi-builds-are-back
```
  export ANDROID_SDK_ROOT=/opt/Android/android-sdk;
  export ANDROID_NDK_ROOT=/opt/Android/android-sdk/ndk/23.1.7779620;
  export QT_BASE=/opt/Qt/6.4.1;
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

-DANDROID=ON \
-DCMAKE_TOOLCHAIN_FILE=${ANDROID_NDK}/build/cmake/android.toolchain.cmake \
-DQT_ANDROID_BASE=${QT_BASE} \

## Running tests

You need to set few env variables:
- TEST_MERGIN_URL - URL of the test server, e.g. `http://127.0.0.1:5000/`
- TEST_API_USERNAME
- TEST_API_PASSWORD

Run:
```
./scripts/run_all_tests.bash input
```
