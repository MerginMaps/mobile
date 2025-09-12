# GPLv2 Licence

# needs to be called after project() command so toolchains are already evaluated
macro (mm_detect_version)
  set(MM_VERSION ${MM_VERSION_MAJOR}.${MM_VERSION_MINOR}.${MM_VERSION_PATCH})
  set(VERSTR ${MM_VERSION})

  string(
    REPLACE " "
            "\ "
            ESCAPED_VERSTR
            ${MM_VERSION}
  )

  set(version_desc "version name ${MM_VERSION}; code ${MM_VERSION_CODE}")
endmacro ()
