# Input

[![Build Status](https://travis-ci.com/lutraconsulting/input.svg?branch=master)](https://travis-ci.com/lutraconsulting/input)

Mobile application based on QGIS's library QgsQuick.

# Development

Tested: Linux, MacOs (Desktop only)
In general you need `qgis_core` and `qgis_quick` libraries build for target platform.

You need to copy and edit config.in with your paths!
```
cd app
cp config.pri.default config.pri
# nano config.pri
```

## Development Linux - Desktop

Requirements:

- Qt5.x
- QGIS 3.x prerequsities

You can either build qgis_quick library or use ony from QGIS 3.4+ installation
For building QGIS use these flags WITH_QUICK=TRUE, WITH_GUI=FALSE, WITH_DESKTOP=FALSE, WITH_BINDINGS=FALSE

```
cd repo/QGIS/build-cmd
cmake \
   -GNinja \
   -DCMAKE_BUILD_TYPE=Release \
   -DCMAKE_INSTALL_PREFIX=~/qmobile/apps \
   -DWITH_GUI=FALSE \
   -DWITH_QUICK=TRUE \
   -DWITH_QTWEBKIT=FALSE \
   -DENABLE_TESTS=FALSE \
   -DWITH_BINDINGS=FALSE \
   ..
ninja
ninja install
```

Now you need to edit input/config.pri with paths to your QGIS installation and build with qmake

And run

```
#!/bin/bash
APP=~/qmobile/apps

LD_LIBRARY_PATH=$APP/lib/ \
QML2_IMPORT_PATH=$APP/qml \
QGIS_PREFIX_PATH=$APP \
$APP/bin/input
```

## Developement Linux cross compile for Android

Same requirements as for Linux Desktop

Requirements Android:
- OSGeo4A

Build `qgis_core` and `qgis_quick` libraries with OSGeo4A.
Now you need to edit input/config.pri with paths to your OSGeo4A installation and build with qmake

## Development MacOS Desktop

So far only working if you want to build desktop version of the application

Requirements:
 - All QGIS dependencies for qgis-3 receipt from https://github.com/OSGeo/homebrew-osgeo4mac

You can either build qgis_quick library or use ony from QGIS 3.4+ installation. Use same flags as for Linux
Now you need to edit input/config.pri with paths to your QGIS installation and build with qmake

To run the application from build tree, you need to:

```
#!/bin/bash
APP=~/qmobile/Applications

DYLD_FRAMEWORK_PATH=$DYLD_FRAMEWORK_PATH:$APP/QGIS.app/Contents/MacOS/lib:$APP/QGIS.app/Contents/Frameworks \
QML2_IMPORT_PATH=$APP/QGIS.app/Contents/MacOS/qml \
QGIS_PREFIX_PATH=$APP/QGIS.app/Contents/MacOS \
$APP/bin/qgis-quick-components-test
```

1.append QGIS Frameworks paths to `DYLD_FRAMEWORK_PATH`

```
export
```

2. append QML path for `qgis_quick` qml dir
