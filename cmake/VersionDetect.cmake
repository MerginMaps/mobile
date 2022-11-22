# GPLv2 Licence

# needs to be called after project() command so toolchains are already evaluated
macro (mm_detect_version)
  set(INPUT_VERSION ${MM_VERSION_MAJOR}.${MM_VERSION_MINOR}.${MM_VERSION_PATCH})
  string(
    REPLACE " "
            "\ "
            ESCAPED_VERSTR
            ${INPUT_VERSION}
  )

  set(version_desc "version name ${INPUT_VERSION}")
  if (ANDROID
      OR IOS
      OR MACOS
  )
    set(version_desc "${version_desc}; code ${INPUT_VERSION_CODE}")
  endif ()
endmacro ()
