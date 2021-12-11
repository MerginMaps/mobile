looking for android [publishing](./publishing.md)

# Development

Tested: Linux, MacOs, iOS
In general you need `qgis_core` and `qgis_quick` libraries build for target platform.

You need to copy and edit config.in with your paths!
```
cd app
cp config.pri.default config.pri
# nano config.pri
```

Notes:

1. If you have "error: undefined reference to 'stdout'" or so, make sure that in BUILD ENV you have ANDROID_NDK_PLATFORM=android-24 or later!

2. You may need to export env variable INPUT_ONLY_TARGET_ARCH to select builds to one platform only

## Development Linux Cross-Compilation for Android

Requirements Android:
- input-sdk for Android
- Qt5 for android
- android-ndk
- android-sdk

How to compile Android:

  1. edit `config.pri` file with your paths (Qt, input-sdk)



## Development MacOS Cross-Compilation for Android

Same requirements as for Linux Cross-Compilation for Android

Quick guide:
- `brew tap caskroom/versions`
- `brew cask install java8`
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

## Running tests

You need to set few env variables:
- TEST_MERGIN_URL - URL of the test server, e.g. `http://127.0.0.1:5000/`
- TEST_API_USERNAME
- TEST_API_PASSWORD

Run:
```
./scripts/run_all_tests.bash input
```
