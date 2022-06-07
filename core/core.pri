
SOURCES += \
  $$PWD/coreutils.cpp \
  $$PWD/merginapi.cpp \
  $$PWD/merginapistatus.cpp \
  $$PWD/merginsubscriptioninfo.cpp \
  $$PWD/merginsubscriptionstatus.cpp \
  $$PWD/merginsubscriptiontype.cpp \
  $$PWD/merginprojectstatusmodel.cpp \
  $$PWD/merginuserauth.cpp \
  $$PWD/merginuserinfo.cpp \
  $$PWD/localprojectsmanager.cpp \
  $$PWD/merginprojectmetadata.cpp \
  $$PWD/project.cpp \
  $$PWD/geodiffutils.cpp

HEADERS += \
  $$PWD/coreutils.h \
  $$PWD/merginapi.h \
  $$PWD/merginapistatus.h \
  $$PWD/merginsubscriptioninfo.h \
  $$PWD/merginsubscriptionstatus.h \
  $$PWD/merginsubscriptiontype.h \
  $$PWD/merginprojectstatusmodel.h \
  $$PWD/merginuserauth.h \
  $$PWD/merginuserinfo.h \
  $$PWD/localprojectsmanager.h \
  $$PWD/merginprojectmetadata.h \
  $$PWD/project.h \
  $$PWD/geodiffutils.h

exists($$PWD/merginsecrets.cpp) {
  message("Using production Mergin API_KEYS")
  SOURCES += $$PWD/merginsecrets.cpp
} else {
  message("Using development (dummy) Mergin API_KEY")
  DEFINES += USE_MERGIN_DUMMY_API_KEY
}
