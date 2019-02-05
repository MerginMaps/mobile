# Input

<img align="right" src="https://raw.githubusercontent.com/lutraconsulting/input/d781624aa2927d3c92432905de441d4fa83980f1/app/img/input.png">

[![Build Status](https://travis-ci.com/lutraconsulting/input.svg?branch=master)](https://travis-ci.com/lutraconsulting/input)

Mobile application based on QGIS's library QgsQuick.

# Development

Tested: Linux (Ubuntu, Manjaro), MacOs (Desktop only), Android.

You need to copy config.pri.default file in /app folder, rename it to config.pri and paste in the current folder.

## Development Linux Cross-Compilation for Android
Requirements Android:
- Qt5.11.2
- QGIS 3.x prerequsities
- Docker
- OSGeo4A
- Minimum 20GB free space (For NDK, SDK, Qt, OsGeo4A and others)

Now you need to edit input/config.pri with paths to your OSGeo4A installation. 

# OSGeo4A Installation from Docker
- First install Docker, you can install it from software manager or command line (Ubuntu: sudo apt-get install docker.io)
- Build OsGeo4A:
```
cd OSGeo4A
sudo docker build -t osgeo4a .
```

Clone Input:
```
git clone https://github.com/lutraconsulting/input.git
cd Input
```
To compite input from docker directly, use this :
docker run -v $(pwd):/usr/src/input -e "BUILD_FOLDER=build-armv7" -e "ARCH=armv7" -it osgeo4a
After you built it, open Android-Build folder in Input. Find APK file in output folder.

Another way is to copy all OsGeo4A dependencies to host machine.
To do so, use these commands:
first get the image id number and copy 
sudo docker ps
example:
```
CONTAINER ID        IMAGE               COMMAND             CREATED             STATUS              PORTS                                            NAMES
1b4ad9311e93        bamos/openface      "/bin/bash"         33 minutes ago 
```
Then copy the image folder to the host folder:
```
docker cp <containerId>:/file/path/within/container /host/path/target
```

The folder copied from docker has root permission, so we cannot compile an app with QtCreator with root files. To change permission folder copied from docker image, it should be user’s permission, not root:
```
sudo chown -R edip:edip ./osgeo4a/
```

Before you develop the app for Android with Qt Creator, you should create a new folder named "INPUT" and copy your projects and data manually to INPUT folder under your SD Card. Then you can compile the app with Qt Creator.

