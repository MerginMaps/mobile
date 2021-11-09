# Build Input on Linux (for development)

Warnings: please change Qt and QGIS-Deps version according to your actual versions

1. Check qgis/QGIS INSTALL for Linux on QGIS project github
2. Download and install Qt (currently used version is 5.14)
3. Download geodiff, compile geodiff
```
mkdir build-geodiff-Desktop
cd build-geodiff-Desktop
cmake \
  -DCMAKE_BUILD_TYPE=Debug \
  -DWITH_INTERNAL_SQLITE3:BOOL=FALSE \
  -DENABLE_TESTS=FALSE \
  -DBUILD_TOOLS=OFF \
  ../../geodiff/geodiff

make -j<num of cores; best to use num of cores - 1>
cd ..
```
4. Compile QGIS
```
mkdir build-QGIS-Desktop
cd build-QGIS-Desktop
cmake \
  -DCMAKE_BUILD_TYPE=Debug \
  -DWITH_GUI=OFF \
  -DWITH_DESKTOP=OFF \
  -DWITH_BINDINGS=OFF \
  -DENABLE_TESTS=OFF \
  -DWITH_QUICK=OFF \
  -DWITH_GRASS=OFF \
  -DWITH_QTWEBKIT=OFF \
  -DWITH_ANALYSIS=OFF \
  -DWITH_QWTPOLAR=OFF \
  -DWITH_APIDOC=OFF \
  -DWITH_ASTYLE=OFF \
  -DWITH_QSPATIALITE=OFF \
  -DWITH_QGIS_PROCESS=OFF \
  ../../QGIS

make -j<num of cores; best to use num of cores - 1>
cd ..
```
> To speed up compilation time install [Ninja build system](https://ninja-build.org/) and add `-GNinja` to cmake arguments

5. Download and compile ZXing https://github.com/nu-book/zxing-cpp (v1.1.1)
```
mkdir build-ZXING
cd build-ZXING
cmake \
  -DBUILD_EXAMPLES=OFF \
  -DBUILD_BLACKBOX_TESTS=OFF \
  -DBUILD_SHARED_LIBS=ON \
  -DBUILD_UNIT_TESTS=OFF \
  -DCMAKE_INSTALL_PREFIX=`pwd`/install \
  ../../zxing-cpp

make -j<num of cores; best to use num of cores - 1>
make install
```

5. Install whole bunch of packages

```bash
sudo apt-get install -y qtpositioning5-dev qml-module-qtmultimedia qml-module-qtpositioning qml-module-qtqml-models2 qml-module-qtgraphicaleffects qml-module-qtquick-dialogs qml-module-qtquick-controls2 qml-module-qtquick-window2 qml-module-qtquick-layouts qml-module-qtquick-shapes qml-module-qtquick-controls qtquickcontrols2-5-dev libqt5sensors5-dev qtmultimedia5-dev
sudo apt-get install qml-module-qt-labs-*
sudo apt install sqlite3
```

6. Copy `app/config.pri.default` to `app/config.pri` and edit it so that it points to your build folders for geodiff, qgis and zxing. You need to set build and include directories and path to `gdal.h`
8. Open `app/input.pro` in Qt Creator (Navigate to `File > Open File or Project...` and select `input.pro` file)
9. Compile Input and run
