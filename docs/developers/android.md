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

You may need to export env variable ANDROID_TARGET_ARCH to select builds to one platform only

## Development Linux Cross-Compilation for Android

Requirements Android:
- input-sdk for Android
- Qt5 for android
- android-ndk
- android-sdk

How to compile Android:

  1. edit `config.pri` file with your paths (Qt, Qt Android, Qgis, Geodiff)
  2. copy and edit `scripts/build-qgsquick-android.bash` with your paths
  3. run the script (it will build qgsquick in input source directory)
  4. add variable `QGSQUICK_INSTALL_PATH` with path to builded qgsquick into `config.pri` android section

Generally check input-sdk requirements and also Dockerfile how it is build in CI

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
input --test
```
