if (NOT WIN)
  message(FATAL_ERROR "Windows libraries are not available on this platform")
endif ()

set(WIN_LIBRARIES
    avcodec
    avdevice
    avformat
    avutil
    swresample
    swscale
    openh264
)

foreach (library ${WIN_LIBRARIES})
  find_library(WIN_${library}_LIBRARY NAMES ${library})
  set(WIN_REQUIRED_VARS ${WIN_REQUIRED_VARS} WIN_${library}_LIBRARY)
endforeach ()

find_package_handle_standard_args(WinLibraries REQUIRED_VARS ${WIN_REQUIRED_VARS})

if (WinLibraries_FOUND)
  foreach (library ${WIN_LIBRARIES})
    if (NOT TARGET WinLibraries::${library})
      add_library(WinLibraries::${library} INTERFACE IMPORTED)
      set_target_properties(
        WinLibraries::${library} PROPERTIES IMPORTED_LOCATION ${library}
      )
    endif ()
  endforeach ()
endif ()

foreach (library ${WIN_LIBRARIES})
  mark_as_advanced(${WIN_${library}_LIBRARY})
endforeach ()
