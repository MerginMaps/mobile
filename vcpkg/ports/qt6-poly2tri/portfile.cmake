# on iOS (static) Qt ships Poly2Tri ( but without headers :( )
if(NOT VCPKG_TARGET_IS_IOS)
    message(FATAL_ERROR "qt poly2tri only needed on iOS")
else()
    vcpkg_download_distfile(ARCHIVE
        URLS "https://download.qt.io/archive/qt/6.8/${VERSION}/submodules/qtpositioning-everywhere-src-${VERSION}.zip"
        FILENAME "qtpositioning-${VERSION}.zip"
        SHA512 cd216c94ef2d35e48bd9b8d7cf3d5c71f8afba379044601737e062f0de88c10a5c931a63b0f05436355ab464918400f7704908ec9c46c79eee74d790e678ae47 
    )
    
    vcpkg_extract_source_archive(
        SOURCE_PATH
        ARCHIVE "${ARCHIVE}"
        SOURCE_BASE "v${VERSION}"
    )
    
    file(INSTALL ${SOURCE_PATH}/src/3rdparty/poly2tri DESTINATION ${CURRENT_PACKAGES_DIR}/include FILES_MATCHING PATTERN "*.h")
endif()
