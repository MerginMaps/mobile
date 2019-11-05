SOURCES += \
localprojectsmanager.cpp \
main.cpp \
merginprojectmetadata.cpp \
projectsmodel.cpp \
layersmodel.cpp \
loader.cpp \
digitizingcontroller.cpp \
mapthemesmodel.cpp \
appsettings.cpp \
merginapi.cpp \
merginapistatus.cpp \
merginprojectmodel.cpp \
androidutils.cpp \
inpututils.cpp \
ios/iosdevice.cpp \
ios/iosimagepicker.cpp \
ios/iossystemdispatcher.cpp \
ios/iosutils.cpp \

HEADERS += \
localprojectsmanager.h \
merginprojectmetadata.h \
projectsmodel.h \
layersmodel.h \
loader.h \
digitizingcontroller.h \
mapthemesmodel.h \
appsettings.h \
merginapi.h \
merginapistatus.h \
merginprojectmodel.h \
androidutils.h \
inpututils.h \
ios/iosdevice.h \
ios/iosimagepicker.h \
ios/iossystemdispatcher.h \
ios/iosutils.h \
ios/iosviewdelegate.h \

QMAKE_CXXFLAGS += -fobjc-arc

OBJECTIVE_SOURCES += \
ios/iosviewdelegate.mm \
ios/iossystemutils.mm \
ios/iosdevice.mm \

contains(DEFINES, INPUT_TEST) {
  SOURCES += test/testmerginapi.cpp
  HEADERS += test/testmerginapi.h
}

RESOURCES += \
    img/pics.qrc \
    qml/qml.qrc \
    fonts/fonts.qrc

TRANSLATIONS +=

# this makes the manifest visible from Qt Creator
DISTFILES += \
    qml/MapThemePanel.qml \
    qml/Notification.qml
