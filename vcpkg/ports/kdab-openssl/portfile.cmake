set(KDAB_OPENSSL_GIT_TAG "82c850cbc602ca4d20c82552e618b0a98b196f3c")

# on Android we need openssl shared libraries for some QT network modules
if(NOT VCPKG_TARGET_IS_ANDROID)
    message(FATAL_ERROR "KDAB-openssl shared libraries only needed on Android")
else()
    vcpkg_download_distfile(ARCHIVE
        URLS "https://github.com/KDAB/android_openssl/archive/${KDAB_OPENSSL_GIT_TAG}.tar.gz"
        FILENAME "kdab-openssl-${VERSION}.tar.gz"
        SHA512 7f61c1fc0f6f38eac5da81234194b9fe5bcbf575c968ae90b8a734466aa818ce87b167defc4a60af166266bfafeaa03c6e6b8cb27fd39cdf214d464613f0c4d8
    )
    
    vcpkg_extract_source_archive(
        SOURCE_PATH
        ARCHIVE "${ARCHIVE}"
        SOURCE_BASE "v${VERSION}"
    )
    
    if (${TARGET_TRIPLET} STREQUAL "arm-android")
        set(OPENSSL_ARCH "armeabi-v7a")
    elseif (${TARGET_TRIPLET} STREQUAL "arm64-android")
        set(OPENSSL_ARCH "arm64-v8a")
    else()
        message(FATAL_ERROR "KDAB-openssl unsupported platform ${TARGET_TRIPLET}")
    endif()

    file(INSTALL ${SOURCE_PATH}/ssl_3/include/openssl/ DESTINATION ${CURRENT_PACKAGES_DIR}/include/kdab-openssl/ )
    file(INSTALL ${SOURCE_PATH}/ssl_3/${OPENSSL_ARCH}/libssl_3.so DESTINATION ${CURRENT_PACKAGES_DIR}/lib/kdab-openssl/)
    file(INSTALL ${SOURCE_PATH}/ssl_3/${OPENSSL_ARCH}/libcrypto_3.so DESTINATION ${CURRENT_PACKAGES_DIR}/lib/kdab-openssl/)
endif()
