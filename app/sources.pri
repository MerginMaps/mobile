SOURCES += \
activelayer.cpp \
layersmodel.cpp \
layersproxymodel.cpp \
localprojectsmanager.cpp \
main.cpp \
merginprojectmetadata.cpp \
projectsmodel.cpp \
loader.cpp \
digitizingcontroller.cpp \
mapthemesmodel.cpp \
appsettings.cpp \
merginapi.cpp \
merginapistatus.cpp \
merginsubscriptionstatus.cpp \
merginsubscriptiontype.cpp \
merginprojectmodel.cpp \
merginprojectstatusmodel.cpp \
merginuserauth.cpp \
merginuserinfo.cpp \
androidutils.cpp \
inpututils.cpp \
geodiffutils.cpp \
positiondirection.cpp \
purchasing.cpp \
variablesmanager.cpp \
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
activelayer.h \
layersmodel.h \
layersproxymodel.h \
localprojectsmanager.h \
merginprojectmetadata.h \
projectsmodel.h \
loader.h \
digitizingcontroller.h \
mapthemesmodel.h \
appsettings.h \
merginapi.h \
merginapistatus.h \
merginsubscriptionstatus.h \
merginsubscriptiontype.h \
merginprojectmodel.h \
merginprojectstatusmodel.h \
merginuserauth.h \
merginuserinfo.h \
androidutils.h \
inpututils.h \
geodiffutils.h \
positiondirection.h \
purchasing.h \
variablesmanager.h \
ios/iosimagepicker.h \
ios/iosutils.h \

contains(DEFINES, INPUT_TEST) {

  contains(DEFINES, APPLE_PURCHASING) {
    error("invalid combination of flags. INPUT_TEST cannot be defined with APPLE_PURCHASING")
   }

  SOURCES += \
      test/testutils.cpp \
      test/testmerginapi.cpp \
      test/testingpurchasingbackend.cpp \
      test/testpurchasing.cpp

  HEADERS += \
      test/testutils.h \
      test/testmerginapi.h \
      test/testingpurchasingbackend.h \
      test/testpurchasing.h
}

contains(DEFINES, APPLE_PURCHASING) {
  HEADERS += \
      ios/iospurchasing.h

  OBJECTIVE_SOURCES += \
      ios/iospurchasing.mm
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
