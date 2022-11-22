# Build Input on MacOS (for development)

1. Download / build Input-SDK for your desired target platform (macos/android/ios)
2. Create Build system
 
```
mkdir build-input-desktop
cd build-input-desktop
BASE_DIR=~/Projects/quick;
cmake \
  -DCMAKE_BUILD_TYPE=Debug \
  -DCMAKE_PREFIX_PATH=/opt/Qt/6.4.1/macos \
  -DCMAKE_INSTALL_PREFIX:PATH=$BASE_DIR/install-macos \
  -DINPUT_SDK_PATH=$BASE_DIR/input-sdk/build/macos/stage/mac \
  -GNinja \
  -DQGIS_QUICK_DATA_PATH=$BASE_DIR/input/app/android/assets/qgis-data \
  $BASE_DIR/input

ninja
cd ..
```

3. Open Input in Qt Creator
4. Compile and run
