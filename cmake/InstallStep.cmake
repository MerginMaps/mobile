# GPLv2 Licence

set(Qt6_base_dir ${Qt6_DIR}/../../..)

# ########################################################################################
# Binary
# ########################################################################################

if (WIN)
  install(
    TARGETS Input
    LIBRARY DESTINATION lib/
    RUNTIME DESTINATION .
  )
else ()
  install(
    TARGETS Input
    LIBRARY DESTINATION lib/
    BUNDLE DESTINATION .
  )
endif ()

# ########################################################################################
# SDK Shared Libraries
# ########################################################################################
if (WIN)
  install(
    DIRECTORY ${INPUT_SDK_PATH_MULTI}/bin/
    DESTINATION .
    FILES_MATCHING
    PATTERN "*.dll"
    PATTERN "Qca" EXCLUDE
  )
  install(
    DIRECTORY ${INPUT_SDK_PATH_MULTI}/bin/Qca/crypto/
    DESTINATION .
    FILES_MATCHING
    PATTERN "*.dll"
  )
  install(
    DIRECTORY ${INPUT_SDK_PATH_MULTI}/tools/qgis/plugins/
    DESTINATION .
    FILES_MATCHING
    PATTERN "*.dll"
  )
elseif (LNX)
  install(
    DIRECTORY ${INPUT_SDK_PATH_MULTI}/lib/
    DESTINATION lib/
    FILES_MATCHING
    PATTERN "*.so"
    PATTERN "qca-qt6" EXCLUDE
  )
  install(
    DIRECTORY ${INPUT_SDK_PATH_MULTI}/lib/qca-qt6/crypto/
    DESTINATION lib/
    FILES_MATCHING
    PATTERN "*.so"
  )
  # elseif(MACOS) no shared libs otherwise the destination would be:
  # Input.app/Contents/MacOS/lib/
endif ()

# ########################################################################################
# QT Libraries
# ########################################################################################
set(qml_dirs
    Qt
    Qt5Compat
    QtMultimedia
    QtQml
    QtQuick
    QtPositioning
    QtCore
    QtSensors
)

foreach (qml_dir ${qml_dirs})

  if (WIN)
    install(
      DIRECTORY ${Qt6_base_dir}/qml/${qml_dir}
      DESTINATION qml
      PATTERN "*d.dll" EXCLUDE
    )
  elseif (LNX)
    install(
      DIRECTORY ${Qt6_base_dir}/qml/${qml_dir}
      DESTINATION qml
      PATTERN "*d.so" EXCLUDE
    )
  elseif (MACOS)
    install(
      DIRECTORY ${Qt6_base_dir}/qml/${qml_dir}
      DESTINATION Input.app/Contents/MacOS/qml
      PATTERN "*dylib.dSYM" EXCLUDE
    )
  endif ()

endforeach ()

set(plugins_dirs
    position
    sqldrivers
    imageformats
    platforms
    multimedia
)

foreach (plugins_dir ${plugins_dirs})

  if (WIN)
    install(
      DIRECTORY ${Qt6_base_dir}/plugins/${plugins_dir}
      DESTINATION .
      PATTERN "*d.dll" EXCLUDE
    )
  elseif (LNX)
    install(
      DIRECTORY ${Qt6_base_dir}/plugins/${plugins_dir}
      DESTINATION plugins/
      PATTERN "*d.so" EXCLUDE
    )
  elseif (MACOS)
    install(
      DIRECTORY ${Qt6_base_dir}/plugins/${plugins_dir}
      DESTINATION Input.app/Contents/PlugIns/
      PATTERN "*dylib.dSYM" EXCLUDE
    )
  endif ()

endforeach ()

set(qt_libs
    Bluetooth
    Concurrent
    Core
    Core5Compat
    DBus
    Designer
    DesignerComponents
    Gui
    Help
    LabsAnimation
    LabsFolderListModel
    LabsQmlModels
    LabsSettings
    LabsSharedImage
    LabsWavefrontMesh
    Multimedia
    MultimediaQuick
    MultimediaWidgets
    Network
    Nfc
    OpenGL
    OpenGLWidgets
    Positioning
    PositioningQuick
    PrintSupport
    Qml
    QmlCompiler
    QmlCore
    QmlLocalStorage
    QmlModels
    QmlWorkerScript
    QmlXmlListModel
    Quick
    Quick3DSpatialAudio
    QuickControls2
    QuickControls2Impl
    QuickDialogs2
    QuickDialogs2QuickImpl
    QuickDialogs2Utils
    QuickLayouts
    QuickParticles
    QuickShapes
    QuickTemplates2
    QuickTest
    QuickWidgets
    Sensors
    SensorsQuick
    ShaderTools
    SpatialAudio
    Sql
    Svg
    SvgWidgets
    Test
    UiTools
    Widgets
    Xml
)

foreach (qt_lib ${qt_libs})
  if (WIN)
    install(FILES ${Qt6_base_dir}/bin/Qt6${qt_lib}.dll DESTINATION .)
  elseif (LNX)
    install(FILES ${Qt6_base_dir}/lib/Qt6${qt_lib}.so DESTINATION .)
  elseif (MACOS)
    install(DIRECTORY ${Qt6_base_dir}/lib/Qt${qt_lib}.framework
            DESTINATION Input.app/Contents/Frameworks/
    )
  endif ()
endforeach ()

if (WIN)
  install(FILES ${Qt6_base_dir}/bin/d3dcompiler_47.dll ${Qt6_base_dir}/bin/opengl32sw.dll
          DESTINATION .
  )
endif ()

# ########################################################################################
# Translations
# ########################################################################################
if (WIN)
  set(translations_dir "translations")
elseif (LNX)
  set(translations_dir "share/translations")
elseif (MACOS)
  set(translations_dir "Input.app/Contents/Resources/i18n")
endif ()

install(
  DIRECTORY app/i18n/
  DESTINATION ${translations_dir}
  FILES_MATCHING
  PATTERN "*.qm"
)

# ########################################################################################
# Assets
# ########################################################################################
if (WIN)
  set(assets_dir ".")
elseif (LNX)
  set(assets_dir "share/input")
elseif (MACOS)
  set(assets_dir "Input.app/Contents/Resources/INPUT")
endif ()

install(DIRECTORY app/android/assets/demo-projects DESTINATION ${assets_dir})
install(DIRECTORY app/android/assets/qgis-data DESTINATION ${assets_dir})

# ########################################################################################
# Platform
# ########################################################################################
if (WIN)
  install(FILES app/win/AppIcon.ico DESTINATION images/)
  install(FILES app/win/license.txt DESTINATION .)
endif ()
