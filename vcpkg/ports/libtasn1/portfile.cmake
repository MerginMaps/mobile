vcpkg_download_distfile(ARCHIVE
    https://ftp.gnu.org/pub/gnu/libtasn1/libtasn1-${VERSION_libtasn1}.tar.gz
    
    URLS "https://ftp.gnu.org/gnu/libtasn1/libtasn1-${VERSION}.tar.gz"
         "https://www.mirrorservice.org/sites/ftp.gnu.org/gnu/libtasn1/libtasn1-${VERSION}.tar.gz"
    FILENAME "libtasn1-${VERSION}.tar.gz"
    SHA512 287f5eddfb5e21762d9f14d11997e56b953b980b2b03a97ed4cd6d37909bda1ed7d2cdff9da5d270a21d863ab7e54be6b85c05f1075ac5d8f0198997cf335ef4
)
vcpkg_extract_source_archive(SOURCE_PATH
    ARCHIVE "${ARCHIVE}"
    SOURCE_BASE "v${VERSION}"
)

string(APPEND VCPKG_C_FLAGS " -Wno-implicit-function-declaration")
string(APPEND VCPKG_CXX_FLAGS " -Wno-implicit-function-declaration")
    
vcpkg_list(SET OPTIONS)
vcpkg_configure_make(
    SOURCE_PATH "${SOURCE_PATH}"
    DETERMINE_BUILD_TRIPLET
    USE_WRAPPERS
    OPTIONS
        --enable-extra-encodings
        --disable-doc 
        --disable-valgrind-tests
        --disable-gcc-warnings 
        --disable-silent-rules
        ${OPTIONS}
)
vcpkg_install_make()

