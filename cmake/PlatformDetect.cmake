# GPLv2 Licence

# needs to be called after project() command so toolchains are already evaluated
macro (mm_detect_platform)
  if (ANDROID_ABI OR CMAKE_SYSTEM_NAME STREQUAL "Android")
    # android can be build on any host platform
    set(ANDROID TRUE)
  else ()
    # APPLE is both iOS and macOS
    if (APPLE AND NOT IOS)
      set(MACOS TRUE)
    endif ()

    if (UNIX AND NOT APPLE)
      set(LNX TRUE)
    endif ()

    if (CMAKE_HOST_SYSTEM_NAME STREQUAL "Windows")
      set(WIN TRUE)
    endif ()
  endif ()

  # Determine message to output and make sure we have all other flags off
  if (ANDROID)
    set(platform_desc
        "android (abis: ${QT_ANDROID_ABIS}, min: ${ANDROID_PLATFORM}, host: ${CMAKE_HOST_SYSTEM_NAME})"
    )
    set(MACOS FALSE)
    set(IOS FALSE)
    set(WIN FALSE)
    set(LNX FALSE)
  elseif (MACOS)
    set(platform_desc "macOS (min: ${CMAKE_OSX_DEPLOYMENT_TARGET})")
    set(IOS FALSE)
    set(ANDROID FALSE)
    set(WIN FALSE)
    set(LNX FALSE)
  elseif (IOS)
    set(platform_desc "iOS (target: ${DEPLOYMENT_TARGET})")
    set(MACOS FALSE)
    set(ANDROID FALSE)
    set(WIN FALSE)
    set(LNX FALSE)
  elseif (WIN)
    set(platform_desc "windows")
    set(MACOS FALSE)
    set(IOS FALSE)
    set(ANDROID FALSE)
    set(LNX FALSE)
  elseif (LNX)
    set(platform_desc "linux")
    set(MACOS FALSE)
    set(IOS FALSE)
    set(ANDROID FALSE)
    set(WIN FALSE)
  else ()
    message(
      FATAL_ERROR
        "unknown platform target ${CMAKE_SYSTEM_NAME}, host ${CMAKE_HOST_SYSTEM_NAME}"
    )
  endif ()
endmacro ()
