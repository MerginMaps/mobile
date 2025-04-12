# see https://github.com/OSGeo/gdal/blob/master/frmts/jpeg/libjpeg/CMakeLists.txt
cmake_minimum_required(VERSION 3.6)
project(JPEG)

set(CMAKE_WINDOWS_EXPORT_ALL_SYMBOLS ON)

add_library(
  jpeg ${JPEG_BUILD_TYPE}
  jcapimin.c
  jchuff.h
  jconfig.h
  jdapimin.c
  jdct.h
  jdmarker.c
  jdtrans.c
  jidctflt.c
  jmemmgr.c
  jquant2.c
  jcapistd.c
  jcinit.c
  jcparam.c
  jdapistd.c
  jddctmgr.c
  jdmaster.c
  jerror.c
  jidctfst.c
  jmemsys.h
  jutils.c
  jccoefct.c
  jcmainct.c
  jcphuff.c
  jdatadst.c
  jdhuff.c
  jdmerge.c
  jerror.h
  jidctint.c
  jmorecfg.h
  jversion.h
  jccolor.c
  jcmarker.c
  jcprepct.c
  jdatasrc.c
  jdhuff.h
  jdphuff.c
  jfdctflt.c
  jidctred.c
  jpegint.h
  jcdctmgr.c
  jcmaster.c
  jcsample.c
  jdcoefct.c
  jdinput.c
  jdpostct.c
  jfdctfst.c
  jinclude.h
  jpeglib.h
  jchuff.c
  jcomapi.c
  jctrans.c
  jdcolor.c
  jdmainct.c
  jdsample.c
  jfdctint.c
  jmemnobs.c
  jquant1.c)
  
target_include_directories(jpeg PRIVATE $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}> $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>/..)
# target_compile_options(libjpeg PRIVATE ${GDAL_C_WARNING_FLAGS} ${GDAL_SOFTWARNFLAGS})

target_compile_definitions(jpeg PRIVATE -DDEFAULT_MAX_MEM=500000000L)
target_compile_definitions(jpeg PUBLIC -DRENAME_INTERNAL_LIBJPEG_SYMBOLS)

# produce position independent code, default is on when building a shared library
# option(GDAL_OBJECT_LIBRARIES_POSITION_INDEPENDENT_CODE "Set ON to produce -fPIC code" ${BUILD_SHARED_LIBS})
# set_property(TARGET jpeg PROPERTY POSITION_INDEPENDENT_CODE ${GDAL_OBJECT_LIBRARIES_POSITION_INDEPENDENT_CODE})
# target_sources(${GDAL_LIB_TARGET_NAME} PRIVATE $<TARGET_OBJECTS:libjpeg>)

configure_file(libjpeg.pc.in libjpeg.pc @ONLY)
install(FILES ${CMAKE_CURRENT_BINARY_DIR}/libjpeg.pc DESTINATION ${CMAKE_INSTALL_PREFIX}/lib/pkgconfig)

INSTALL(TARGETS jpeg)
INSTALL(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/ DESTINATION include FILES_MATCHING PATTERN "*.h")