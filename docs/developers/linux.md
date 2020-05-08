# Build Input on Linux (for development)

Warnings: please change Qt and QGIS-Deps version according to your actual versions

1. Check qgis/QGIS INSTALL for Linux on QGIS project github
2. Download and install Qt
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
 
make -j11
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
  -DWITH_QUICK=ON \
  -DWITH_GRASS=OFF \
  -DWITH_QTWEBKIT=OFF \
  -DWITH_ANALYSIS=OFF \
  -DWITH_QWTPOLAR=OFF \
  -DWITH_APIDOC=OFF \
  -DWITH_ASTYLE=OFF \
  -DWITH_QSPATIALITE=OFF \
  -DWITH_QGIS_PROCESS=OFF \
  ../../QGIS

make -j<num of cores>
cd ..
```

6. Change config.pri to point to your `~/Projects/quick` folder for geodiff/qgis
7. Open Input in Qt Creator
8. Install whole bunch of qt packages

```bash
sudo apt-get install -y qtpositioning5-dev qml-module-qtmultimedia qml-module-qtpositioning qml-module-qtqml-models2 qml-module-qtgraphicaleffects qml-module-qtquick-dialogs qml-module-qtquick-controls2 qml-module-qtquick-window2 qml-module-qtquick-layouts qml-module-qtquick-shapes qml-module-qtquick-controls 
sudo apt-get install qml-module-qt-labs-*
sudo apt install sqlite3
```

9. Compile and run
