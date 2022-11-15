# Build Input on MacOS (for development)

1. Download / build Input-SDK for your desired target platform (macos/android/ios)
2. Create Build system
 
```
mkdir build-input-desktop
cd build-input-desktop
cmake \
  -DCMAKE_BUILD_TYPE=Release \
  -DCMAKE_PREFIX_PATH=/opt/Qt/6.4.0/macos \
  -DCMAKE_INSTALL_PREFIX:PATH=~/install \
  -DUSE_MM_SERVER_API_KEY=FALSE \
  -DMM_SDK_PATH=~/input-sdk/build/macos/stage/mac
  ../input

make
cd ..
```

3. Open Input in Qt Creator
4. Compile and run

