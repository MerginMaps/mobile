# Build Input on Linux (for development)

This guide is tested with Ubuntu 22.04, on other distros some steps may need some adjustments.

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

   - Check what SDK version is currently in use - look for `INPUT_SDK_VERSION` in `.github/workflows/linux.yml`
   - Download Input SDK for Ubuntu - go to https://github.com/merginmaps/input-sdk/releases and download the built SDK.
   - Unpack the downloaded .tar.gz 

3. Get Qt libraries - Ubuntu's system libraries are too old, and currently Input SDK does not include Qt SDK.

   - Check what Qt version is currently in use - look for `QT_VERSION` in `.github/workflows/linux.yml`
   - Download Qt online installer from https://www.qt.io/download-open-source
   - Use the online installer to install Qt to `~/Qt`

4. Build Input (update CMake command with the correct Qt and Input SDK versions)

   ```
   mkdir build
   cd build
   cmake -G Ninja \
     -DCMAKE_PREFIX_PATH=/home/martin/Qt/6.4.1/gcc_64 \
     -DINPUT_SDK_PATH=/home/martin/input-sdk/ubuntu-2204-20221117-103 \
     -DQGIS_QUICK_DATA_PATH=/home/martin/input/app/android/assets/qgis-data \
     ..
   ninja
   ```

5. Run Input

   ```
   LD_LIBRARY_PATH=/home/martin/input-sdk/ubuntu-2204-20221117-103/lib ldd ./app/merginmaps
   ```
