looking for android [publishing](./publishing.md)

# Development for Android

Tested: Linux, macOS

Notes:

1. If you have "error: undefined reference to 'stdout'" or so, make sure that in BUILD ENV you have ANDROID_NDK_PLATFORM=android-24 or later!

![image](https://user-images.githubusercontent.com/22449698/166630970-a776576f-c505-4265-b4c8-ffbe212c6745.png)

2. You may need to export env variable INPUT_ONLY_TARGET_ARCH to select builds to one platform only

## Development Linux Cross-Compilation for Android

Requirements Android:
- input-sdk for Android
- Qt6 for android
- android-ndk
- android-sdk

```
mkdir build
cd build
cmake -G Ninja \
  -DANDROID_ABI=arm64-v8a \
  -DANDROID_PLATFORM=android-24 \
  -DANDROID_SDK_ROOT=/home/martin/android \
  -DANDROID_NDK=/home/martin/android/ndk/23.1.7779620 \
  -DCMAKE_TOOLCHAIN_FILE=/home/martin/android/ndk/23.1.7779620/build/cmake/android.toolchain.cmake \
  -DINPUT_SDK_PATH=/home/martin/input-sdk/android-macOS-20221117-129/arm64-v8a \
  -DCMAKE_PREFIX_PATH=/home/martin/Qt/6.4.1/android_arm64_v8a \
  ..
ninja
```

## Development MacOS Cross-Compilation for Android

Same requirements as for Linux Cross-Compilation for Android

Quick guide:
- `brew install openjdk@11`
- `brew install ant`
- `brew install bison`
- `sudo mkdir -p /opt; sudo chown <your name>:admin /opt`
- download SDK command line tools and unzip to `/opt/android-sdk`
- sdk: install lldb, build tools, platform android X, cmake, platform-tools
- download QT armv7 to `/opt/Qt`
- download NDK and install to `/opt/android-ndk-<ver>`
- compile input-SDK
- open QtCreator -> Manage Kits -> add SDK and NDK. compilers should be autodetected
- enable connection on the device from MacOS when requested

```
  export ANDROID_SDK_ROOT=/opt/Android/android-sdk;
  export ANDROID_NDK_ROOT=/opt/Android/android-sdk/ndk/23.1.7779620;
  export ANDROID_NDK=${ANDROID_NDK_ROOT};
  export QT_BASE=/opt/Qt/6.4.1/android_arm64_v8a;
  export QT_HOST_PATH=/opt/Qt/6.4.1/macos;
  export ANDROIDAPI=24;
  export PATH="$ANDROID_NDK/toolchains/llvm/prebuilt/darwin-x86_64/bin/:$PATH";
  export ARCH=arm64-v8a;
  cmake \
    -DCMAKE_BUILD_TYPE=Release \
    -DCMAKE_CXX_FLAGS_RELEASE=-g0 \
    -DCMAKE_FIND_ROOT_PATH:PATH="$ANDROID_NDK;$QT_BASE" \
    -DINPUT_SDK_PATH=~/Projects/quick/input-sdk/build/android/stage/arm64-v8a \
    -DCMAKE_PREFIX_PATH=${QT_BASE} \
    -DANDROID_ABI=$ARCH \
    -DANDROID_NDK=$ANDROID_NDK \
    -DANDROID_SDK_ROOT=$ANDROID_SDK_ROOT \
    -DANDROID_PLATFORM=android-$ANDROIDAPI \
    -DANDROID=ON \
    -DANDROID_STL=c++_shared \
    -DQt6_DIR:PATH=$QT_BASE/lib/cmake -DQT_HOST_PATH=$QT_HOST_PATH \
    -DCMAKE_TOOLCHAIN_FILE=${ANDROID_NDK}/build/cmake/android.toolchain.cmake \
    -DUSE_SERVER_API_KEY=FALSE \
    -GNinja \
    ../input/
```

## Running tests

You need to set few env variables:
- TEST_MERGIN_URL - URL of the test server, e.g. `http://127.0.0.1:5000/`
- TEST_API_USERNAME
- TEST_API_PASSWORD

Run:
```
./scripts/run_all_tests.bash input
```
