# GPLv2 Licence
# Find Input-SDK

if(EXISTS ${INPUT_SDK_PATH}) 
  message("Using Input  SDK: ${INPUT_SDK_PATH}") 
else() 
  message(FATAL_ERROR "${INPUT_SDK_PATH} is not valid  path with the SDK. Set valid INPUT_SDK_PATH path.") 
endif()

set(InputSDK_INCLUDE_DIR ${INPUT_SDK_PATH}/include)
set(InputSDK_GEODIFF_LIBRARY ${INPUT_SDK_PATH}/lib/libgeodiff.a)


# mark_as_advanced(CURL_INCLUDE_DIR)

find_package_handle_standard_args(
  InputSDK
  REQUIRED_VARS InputSDK_INCLUDE_DIR InputSDK_GEODIFF_LIBRARY
  HANDLE_COMPONENTS
)
                                  
if(InputSDK_FOUND)
  set(InputSDK_LIBRARIES ${InputSDK_GEODIFF_LIBRARY})
  set(InputSDK_INCLUDE_DIRS ${InputSDK_INCLUDE_DIR})
 
   if(NOT TARGET InputSDK::geodiff)
     add_library(InputSDK::geodiff STATIC IMPORTED)
     set_target_properties(InputSDK::geodiff PROPERTIES
                           INTERFACE_INCLUDE_DIRECTORIES "${InputSDK_INCLUDE_DIR}"
                           IMPORTED_LINK_INTERFACE_LANGUAGES "C"
                           IMPORTED_LOCATION "${InputSDK_GEODIFF_LIBRARY}")
   endif()
endif()