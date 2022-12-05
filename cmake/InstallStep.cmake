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
# Use <plat>deployqt when possible
# ########################################################################################
if (MACOS)
  get_property(isMultiConfig GLOBAL PROPERTY GENERATOR_IS_MULTI_CONFIG)
  if (isMultiConfig)
    set(deploy_script "${CMAKE_CURRENT_BINARY_DIR}/deploy_input_$<CONFIG>.cmake")
  else ()
    set(deploy_script
        "${CMAKE_CURRENT_BINARY_DIR}/deploy_input_${CMAKE_BUILD_TYPE}.cmake"
    )
  endif ()

  set(executable_path "Input.app/Contents/MacOS/INPUT")

  # https://doc-snapshots.qt.io/qt6-dev/qt-deploy-runtime-dependencies.html Replace with
  # qt_generate_deploy_script from QT 6.5.x The following script must only be executed at
  # install time Note: This command is in technology preview and may change in future
  # releases. (QT 6.4.x)
  file(
    GENERATE
    OUTPUT ${deploy_script}
    CONTENT
      "
		include(\"${QT_DEPLOY_SUPPORT}\")

        qt_deploy_qml_imports(TARGET Input)

    	qt_deploy_runtime_dependencies(
            QML_DIR \"${qml_path}\"
			EXECUTABLE \"${executable_path}\"
			GENERATE_QT_CONF
		)"
  )

  install(SCRIPT ${deploy_script})
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
