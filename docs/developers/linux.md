# Build Input on Linux (for development)

This guide is tested with Ubuntu 20.04, on other distros some steps may need some adjustments.

At the time of writing this guide, we are using:
 - Qt 5.14.2
 - Input SDK with tag `ubuntu-2004-20211201-34`

Steps to build and run Input:

1. Install some dependencies (not all of them are strictly necessary to build/run Input):

   ```
   sudo apt install \
        bison ca-certificates ccache cmake cmake-curses-gui dh-python expect flex flip gdal-bin \
        git graphviz libexiv2-dev libexpat1-dev libfcgi-dev libgdal-dev libgeos-dev libgeos++-dev \
        libgsl-dev libpq-dev libproj-dev libprotobuf-dev \
        libspatialindex-dev libspatialite-dev libsqlite3-dev libsqlite3-mod-spatialite libyaml-tiny-perl \
        libzip-dev libzstd-dev lighttpd locales ninja-build \
        pkg-config poppler-utils protobuf-compiler python3-all-dev \
        spawn-fcgi xauth xfonts-100dpi xfonts-75dpi xfonts-base xfonts-scalable xvfb
   ```

2. Get Input SDK - it contains pre-built dependencies of libraries used by Input

   - Download the latest Input SDK for Ubuntu - go to https://github.com/lutraconsulting/input-sdk/releases and download the built SDK. For `ubuntu-2004-20211201-34` the asset to download is `input-sdk-ubuntu-2004-20211201-34.tar.gz`.

   - Unpack the downloaded .tar.gz to `~/input-sdk-ubuntu-2004-20211201-34`

3. Get Qt libraries - Ubuntu's system libraries are too old, and currently Input SDK does not include Qt SDK.

   - Download Qt online installer from https://www.qt.io/download-open-source
   - Use the online installer to install Qt 5.14.2 to `~/Qt`

4. Create `app/config.pri` file - assuming your Input source code is in `/home/john/input`

   ```
   unix:!macx:!android {
     INPUT_SDK_PATH = /home/john/input-sdk-ubuntu-2004-20211201-34
     QGIS_QUICK_DATA_PATH = /home/john/input/app/android/assets/qgis-data
   }
   ```

5. Build Input

   ```
   mkdir build
   cd build
   ~/Qt/5.14.2/gcc_64/bin/qmake ../app/input.pro
   make -j4
   ```

6. Run Input

   ```
   LD_LIBRARY_PATH=~/Qt/5.14.2/gcc_64/lib ./Input
   ```

## Using Qt Creator

TODO: how to build/run from Qt Creator
