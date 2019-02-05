# Input

<img align="right" src="https://raw.githubusercontent.com/lutraconsulting/input/d781624aa2927d3c92432905de441d4fa83980f1/app/img/input.png">

[![Build Status](https://travis-ci.com/lutraconsulting/input.svg?branch=master)](https://travis-ci.com/lutraconsulting/input)

Mobile application based on QGIS's library QgsQuick.

# Development

Tested: Linux (Ubuntu, Manjaro), MacOs (Desktop only), Android.

## Development Linux Cross-Compilation for Android
Requirements Android:
- Qt5.11.2
- Docker
- OSGeo4A
- Minimum 20GB free space (For NDK, SDK, Qt, OsGeo4A and others)
I recommend you to setup Lubuntu to Virtual Box with 80GB space.

Now you need to edit input/config.pri with paths to your OSGeo4A installation. 

## OSGeo4A Installation from Docker
- First install Docker, you can install it from software manager or command line (Ubuntu: ```sudo apt-get install docker.io```)
- Building OSGeo4A:
```
git clone https://github.com/opengisch/OSGeo4A.git
cd OSGeo4A
sudo docker build -t osgeo4a .
```
It will take for a long time to build everything.
## Running Input for Android
### Building with Docker
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

To compile Input from docker directly, use this :
```
cd Input
docker run -v $(pwd):/usr/src/input -e "BUILD_FOLDER=build-armv7" -e "ARCH=armv7" -it osgeo4a
```
After you built it, open Android-Build folder in Input path. Find APK file in output folder. And install it for Android.
### Building with Qt Creator
- If you wanna customize the app with Qt Creator without docker, copy all OsGeo4A dependencies to your local host.
- To do so, use these commands:
first get the image id number of OSGeo4A. Copy the ID of osgeo4a:
```
cd OSGeo4A
sudo docker image ls
```
example output:
```
REPOSITORY             TAG                 IMAGE ID            CREATED             SIZE
osgeo4a                latest              71eef3e8038a        7 days ago          10.5GB
<none>                 <none>              8a34148c5af2        8 days ago          11.2GB
<none>                 <none>              bd72457905ca        8 days ago          9.71GB
opengisch/qt-crystax   5.11.3_v2           8db89632ab85        12 days ago         9.7GB
```
- Then copy the necessary folders in OSGeo4A's docker image to the host folder:
```
docker cp 71eef3e8038a:/home/osgeo4a /home/apps
docker cp 71eef3e8038a:/opt/android-ndk /home/apps/android
docker cp 71eef3e8038a:/opt/android-sdk /home/apps/android
```
- The folder copied from docker has root permission, so we cannot compile any app in Qt Creator with root files. Change these folder's permissions to your user name.
- Configure config.pri according to new osgeo path:
Example:
```
android {
  OSGEO4A_DIR = /home/apps/osgeo4a
  OSGEO4A_STAGE_DIR = $${OSGEO4A_DIR}
  QGIS_INSTALL_PATH = $${OSGEO4A_STAGE_DIR}/$$ANDROID_TARGET_ARCH
  QGIS_QUICK_DATA_PATH = INPUT # should be relative path
}
!android {
}
```
- Then you are ready to build it with Qt Creator! Set up SDK and NDK path in Qt Creator's Android settings, install ant and and Java JDK 8. Open Input's pro file and build it :)

Note: Before you develop the app for Android with Qt Creator, you should create a new folder in Android's SD Card named "INPUT" and copy your projects and data manually to INPUT folder under your SD Card. Unless you don't do it, Input may not see any project.

