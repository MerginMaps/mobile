# Build Input on MacOS (for development)

Warnings: please change Qt and QGIS-Deps version according to your actual versions

1. Check qgis/QGIS INSTALL for MacOS on QGIS project github
2. Download and install Qt and QGIS-Mac-Deps from https://qgis.org/downloads/macos/deps
3. Download geodiff, compile geodiff with SQLITE3 from QGIS-Deps
```
mkdir build-geodiff-Desktop
cd build-geodiff-Desktop
cmake \
  -DCMAKE_BUILD_TYPE=Debug \
  -DWITH_INTERNAL_SQLITE3:BOOL=FALSE \
  -DSQLite3_ROOT:PATH=/opt/QGIS/qgis-deps-0.3.0/stage \
  -DENABLE_TESTS=FALSE \
  -DBUILD_TOOLS=OFF \
  -DCMAKE_INSTALL_PREFIX=~/Projects/quick/Applications \
  ../geodiff/geodiff
 
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
  -DQGIS_MAC_DEPS_DIR:INTERNAL=/opt/QGIS/qgis-deps-0.3.0/stage \
  -DCMAKE_INSTALL_PREFIX=~/Projects/quick/Applications \
  -DCMAKE_PREFIX_PATH=/opt/Qt/5.14.1/clang_64 \
  -DWITH_QGIS_PROCESS=OFF -DQGIS_MACAPP_BUNDLE=-1 \
  ../QGIS

make -j11 
cd ..
```

6. Change config.pri to point to your `~/Projects/quick/Applications` folder for geodiff/qgis
7. Open Input in Qt Creator
8. Compile and run
 