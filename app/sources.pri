SOURCES += \
browsedatalayersmodel.cpp \
featuresmodel.cpp \
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
merginprojectstatusmodel.cpp \
androidutils.cpp \
inpututils.cpp \
geodiffutils.cpp \
positiondirection.cpp \
ios/iosimagepicker.cpp \
ios/iosutils.cpp \

exists(merginsecrets.cpp) {
  message("Using production Mergin API_KEYS")
  SOURCES += merginsecrets.cpp
} else {
  message("Using development (dummy) Mergin API_KEY")
  DEFINES += USE_MERGIN_DUMMY_API_KEY
}

HEADERS += \
browsedatalayersmodel.h \
featuresmodel.h \
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
merginprojectstatusmodel.h \
androidutils.h \
inpututils.h \
geodiffutils.h \
positiondirection.h \
ios/iosimagepicker.h \
ios/iosutils.h \

contains(DEFINES, INPUT_TEST) {
  SOURCES += test/testmerginapi.cpp
  HEADERS += test/testmerginapi.h
}

RESOURCES += \
    img/pics.qrc \
    qml/qml.qrc \
    fonts/fonts.qrc \
    i18n/input_i18n.qrc

TRANSLATIONS = $$files(i18n/*.ts)

# this makes the manifest visible from Qt Creator
DISTFILES += \
    qml/MapThemePanel.qml \
    qml/Notification.qml
