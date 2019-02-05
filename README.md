# Input

<img align="right" src="https://raw.githubusercontent.com/lutraconsulting/input/d781624aa2927d3c92432905de441d4fa83980f1/app/img/input.png">

[![Build Status](https://travis-ci.com/lutraconsulting/input.svg?branch=master)](https://travis-ci.com/lutraconsulting/input)

Mobile application based on QGIS's library QgsQuick.

# Development

Tested: Linux (Ubuntu, Manjaro), MacOs (Desktop only), Android.

## Development Linux Cross-Compilation for Android
Requirements Android:
- Qt5.11.2
- QGIS 3.x prerequsities
- Docker
- OSGeo4A
- Minimum 20GB free space (For NDK, SDK, Qt, OsGeo4A and others)
I recommend you to setup Lubuntu to Virtual Box with 80GB space.

Now you need to edit input/config.pri with paths to your OSGeo4A installation. 

## OSGeo4A Installation from Docker
- First install Docker, you can install it from software manager or command line (Ubuntu: ```sudo apt-get install docker.io```)
- Building OSGeo4A:
```
cd OSGeo4A
sudo docker build -t osgeo4a .
```
It will take for a long time to build everything.
## Running Input for Android
Clone Input:
```
git clone https://github.com/lutraconsulting/input.git
```
You need to copy config.pri.default file in /input/app folder, rename it to config.pri and paste it in the current folder.
Example:
```
android {
  OSGEO4A_DIR = /home/osgeo4a
  OSGEO4A_STAGE_DIR = $${OSGEO4A_DIR}
  QGIS_INSTALL_PATH = $${OSGEO4A_STAGE_DIR}/$$ANDROID_TARGET_ARCH
  QGIS_QUICK_DATA_PATH = INPUT # should be relative path
}
!android {
}
```

To compite input from docker directly, use this :
```
cd Input
docker run -v $(pwd):/usr/src/input -e "BUILD_FOLDER=build-armv7" -e "ARCH=armv7" -it osgeo4a
```
After you built it, open Android-Build folder in Input path. Find APK file in output folder. And install it for Android.

If you wanna customize the app with Qt Creator without docker, copy all OsGeo4A dependencies to your local host.
To do so, use these commands:
first get the image id number of OSGeo4A. Copy the ID
```
sudo docker ps
```
example:
```
CONTAINER ID        IMAGE               COMMAND             CREATED           
1b4ad9311e93        bamos/openface      "/bin/bash"         33 minutes ago 
```
Then copy the necessary folders in OSGeo4A's docker image to the host folder:
```
docker cp <containerId>:/file/path/within/container /host/path/target
```
The folder copied from docker has root permission, so we cannot compile an app with Qt Creator with root files. To change permission folder copied from docker image, it should be user’s permission, not root:
```
sudo chown -R edip:edip ./osgeo4a/
```

Note: Before you develop the app for Android with Qt Creator, you should create a new folder in Android's SD Card named "INPUT" and copy your projects and data manually to INPUT folder under your SD Card. Then you can compile the app with Qt Creator.

