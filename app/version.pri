# CI builds have variable CI_VERSION_CODE already assigned

# ANDROID_VERSION_NAME
VERSION_MAJOR = 1
VERSION_MINOR = 3
VERSION_FIX = 1

INPUT_VERSION = '$${VERSION_MAJOR}.$${VERSION_MINOR}.$${VERSION_FIX}'

ESCAPED_VERSTR = $$replace( INPUT_VERSION, ' ', '\ ' )
DEFINES += "VERSTR=\\\"$${ESCAPED_VERSTR}\\\""
DEFINES += "INPUT_VERSION=$${INPUT_VERSION}"

ANDROID_VERSION_NAME = $$INPUT_VERSION

# ANDROID_VERSION_CODE
ANDROID_VERSION_CODE=$$(CI_VERSION_CODE)

isEmpty(ANDROID_VERSION_CODE) {
    ANDROID_VERSION_SUFFIX = 0
    QT_ARCH = $$QT_ARCH$$

    equals ( QT_ARCH, 'x86' ) {
      ANDROID_VERSION_SUFFIX = 1
    }
    equals ( QT_ARCH, 'x86_64' ) {
      ANDROID_VERSION_SUFFIX = 2
    }
    equals ( QT_ARCH, 'armeabi-v7a' ) {
      ANDROID_VERSION_SUFFIX = 3
    }
    equals ( QT_ARCH, 'arm64-v8a' ) {
      ANDROID_VERSION_SUFFIX = 4
    }

    VERSIONCODE = $$format_number($$format_number($${VERSION_MAJOR}, width=2 zeropad)$$format_number($${VERSION_MINOR}, width=2 zeropad)$$format_number($${VERSION_FIX}, width=2 zeropad)$$format_number($${ANDROID_VERSION_SUFFIX}))
    ANDROID_VERSION_CODE = $$VERSIONCODE
}
else {
    VERSIONCODE = $$(ANDROID_VERSION_CODE)
}

message('Building $${QT_ARCH} with version name $${INPUT_VERSION} and version code $${VERSIONCODE}')

#ios
QMAKE_FULL_VERSION=VERSIONCODE
QMAKE_SHORT_VERSION=INPUT_VERSION
