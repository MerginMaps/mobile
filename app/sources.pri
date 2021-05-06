SOURCES += \
attributepreviewcontroller.cpp \
inputhelp.cpp \
activelayer.cpp \
fieldsmodel.cpp \
layersmodel.cpp \
layersproxymodel.cpp \
main.cpp \
projectwizard.cpp \
loader.cpp \
digitizingcontroller.cpp \
mapthemesmodel.cpp \
appsettings.cpp \
androidutils.cpp \
inputexpressionfunctions.cpp \
inpututils.cpp \
positiondirection.cpp \
purchasing.cpp \
variablesmanager.cpp \
ios/iosimagepicker.cpp \
ios/iosutils.cpp \
inputprojutils.cpp \
codefilter.cpp \
qrdecoder.cpp \
projectsmodel.cpp \
projectsproxymodel.cpp \
compass.cpp \

HEADERS += \
attributepreviewcontroller.h \
inputhelp.h \
activelayer.h \
fieldsmodel.h \
layersmodel.h \
layersproxymodel.h \
projectwizard.h \
loader.h \
digitizingcontroller.h \
mapthemesmodel.h \
appsettings.h \
androidutils.h \
inputexpressionfunctions.h \
inpututils.h \
positiondirection.h \
purchasing.h \
variablesmanager.h \
ios/iosimagepicker.h \
ios/iosutils.h \
inputprojutils.h \
codefilter.h \
qrdecoder.h \
projectsmodel.h \
projectsproxymodel.h \
compass.h \

contains(DEFINES, INPUT_TEST) {

  contains(DEFINES, APPLE_PURCHASING) {
    error("invalid combination of flags. INPUT_TEST cannot be defined with APPLE_PURCHASING")
   }

  SOURCES += \
      test/testutils.cpp \
      test/testutilsfunctions.cpp \
      test/testmerginapi.cpp \
      test/testingpurchasingbackend.cpp \
      test/testpurchasing.cpp \
      test/testlinks.cpp

  HEADERS += \
      test/testutils.h \
      test/testutilsfunctions.h \
      test/testmerginapi.h \
      test/testingpurchasingbackend.h \
      test/testpurchasing.h \
      test/testlinks.h
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
