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
  -DSQLite3_ROOT:PATH=/opt/QGIS/qgis-deps-0.5.3/stage \
  -DENABLE_TESTS=FALSE \
  -DBUILD_TOOLS=OFF \
  -DCMAKE_INSTALL_PREFIX=~/Projects/quick/Applications \
  -GNinja -DCMAKE_MAKE_PROGRAM=/usr/local/bin/ninja \
  ../geodiff/geodiff
 
ninja
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
  -DQGIS_MAC_DEPS_DIR:INTERNAL=/opt/QGIS/qgis-deps-0.5.3/stage \
  -DCMAKE_INSTALL_PREFIX=~/Projects/quick/Applications \
  -DCMAKE_PREFIX_PATH=/opt/Qt/5.14.2/clang_64 \
  -DWITH_QGIS_PROCESS=OFF -DQGIS_MACAPP_BUNDLE=-1 \
  -GNinja -DCMAKE_MAKE_PROGRAM=/usr/local/bin/ninja \
  ../QGIS

ninja
cd ..
```

5. Change config.pri to point to your `~/Projects/quick` folder for geodiff/qgis
6. Open Input in Qt Creator
7. Compile and run
8. in Run environment, you may need to:
```
DYLD_FRAMEWORK_PATH=/Users/peter/Projects/quick/build-Desktop/build-geodiff:/opt/Qt/5.14.2/clang_64/lib:/Users/peter/Projects/quick/build-Desktop/build-qgsquick/output/lib
DYLD_INSERT_LIBRARIES=/opt/QGIS/qgis-deps-0.6.0/stage/lib/libsqlite3.dylib
DYLD_FALLBACK_LIBRARY_PATH=...
```

