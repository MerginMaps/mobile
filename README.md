# QGIS QtQuick Components

Prototype of QGIS reusable components that can be used in Qt Quick applications.

The repository contains a module that compiles into Qt Quick plugin and a demo app that uses the plugin.

Available visual items:

- MapCanvas
- ScaleBar
- VertexMarker

Other components:

- MapLayer
- PositionEngine

<img src="https://raw.githubusercontent.com/lutraconsulting/qgis-quick-components/master/demoapp.png">

# Build (Linux)

Requirements:

- Qt5.x
- QGIS 3.x prerequsities

## QGIS
```
cd repo/QGIS/build-cmd
cmake \
   -GNinja \
   -DCMAKE_BUILD_TYPE=Debug \
   -DCMAKE_INSTALL_PREFIX=~/qmobile/apps \
   -DWITH_GUI=FALSE \
   -DWITH_QTWEBKIT=FALSE \
   -DENABLE_TESTS=FALSE \
   -DWITH_BINDINGS=FALSE \
   ..
ninja
ninja install
```

## QGIS-QUICK-COMPONENTS
```
cd repo/qgis-quick-components/build-cmd
cmake \
   -GNinja \
   -DCMAKE_BUILD_TYPE=Debug \
   -DCMAKE_INSTALL_PREFIX=~/qmobile/apps \
   -DQGIS_PATH=~/qmobile/apps
   ..
ninja
ninja install
```

# Run (Linux)

```
#!/bin/bash
APP=~/qmobile/apps

LD_LIBRARY_PATH=$APP/lib/ QML2_IMPORT_PATH=$APP/qml QGIS_PREFIX_PATH=$APP $APP/bin/qgis-quick-components-test
```