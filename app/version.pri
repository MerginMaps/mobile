# CI builds have version_code already assigned

# Set version_name
VERSION_MAJOR = 1
VERSION_MINOR = 2
VERSION_FIX = 0

INPUT_VERSION = '$${VERSION_MAJOR}.$${VERSION_MINOR}.$${VERSION_FIX}'

ESCAPED_VERSTR = $$replace( INPUT_VERSION, ' ', '\ ' )
DEFINES += "VERSTR=\\\"$${ESCAPED_VERSTR}\\\""
DEFINES += "INPUT_VERSION=$${INPUT_VERSION}"

ANDROID_VERSION_NAME = $$INPUT_VERSION

# Set version_code
message( version code: $$(ANDROID_VERSION_CODE) )
isEmpty( $$(ANDROID_VERSION_CODE) ) {
    message(version code is empty)

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

    VERSIONCODE = $$format_number($$format_number($${VERSION_MAJOR}, width=2 zeropad)$$format_number($${VERSION_MINOR}, width=2 zeropad)$$format_number($${VERSION_FIX}, width=2 zeropad)$$format_number($${ANDROID_VERSION_SUFFIX}))\
    ANDROID_VERSION_CODE = $$VERSIONCODE
}
else {
    message(version code has already been assigned)
    VERSIONCODE = $$(ANDROID_VERSION_CODE)
    ANDROID_VERSION_CODE = $$(ANDROID_VERSION_CODE)
}

message( 'Building $${QT_ARCH} version $${INPUT_VERSION} ($${VERSIONCODE})' )

#ios
QMAKE_FULL_VERSION=VERSIONCODE
QMAKE_SHORT_VERSION=INPUT_VERSION
