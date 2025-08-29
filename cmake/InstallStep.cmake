# GPLv2 Licence

if (IOS OR ANDROID)
  message(FATAL_ERROR "Install step cannot be called on IOS and Android")
endif ()

set(Qt6_base_dir ${VCPKG_INSTALLED_DIR}/${VCPKG_TARGET_TRIPLET}/Qt6)

# ########################################################################################
# Binary
# ########################################################################################

if (WIN)
  install(TARGETS Input RUNTIME DESTINATION .)
else ()
  install(
    TARGETS Input
    LIBRARY DESTINATION lib64/
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

  qt_generate_deploy_script(
    TARGET
    Input
    OUTPUT_SCRIPT
    deploy_script
    CONTENT
    "
      qt_deploy_runtime_dependencies(
        EXECUTABLE \"${executable_path}\"
        GENERATE_QT_CONF
  )"
  )

  install(SCRIPT ${deploy_script} COMPONENT Runtime)
  include(InstallRequiredSystemLibraries)

elseif (WIN)
  qt_generate_deploy_qml_app_script(
    TARGET
    Input
    OUTPUT_SCRIPT
    deploy_script
    NO_TRANSLATIONS
    DEPLOY_TOOL_OPTIONS
    "--libdir . --plugindir . --force-openssl"
  )
  install(SCRIPT ${deploy_script} COMPONENT Runtime)
endif ()

# ########################################################################################
# SDK Shared Libraries
# ########################################################################################
if (WIN)
  install(
    DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/${CMAKE_BUILD_TYPE}/"
    DESTINATION .
    FILES_MATCHING
    PATTERN "*.dll"
    PATTERN "Qca" EXCLUDE
  )
  install(
    DIRECTORY "${VCPKG_INSTALLED_DIR}/${VCPKG_TARGET_TRIPLET}/bin/Qca/crypto/"
    DESTINATION .
    FILES_MATCHING
    PATTERN "*.dll"
  )
elseif (LNX)
  install(
    DIRECTORY ${VCPKG_INSTALLED_DIR}/${VCPKG_TARGET_TRIPLET}/lib/
    DESTINATION lib64
    FILES_MATCHING
    PATTERN "*.so*"
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
  if (LNX)
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
  if (LNX)
    install(
      DIRECTORY ${Qt6_base_dir}/plugins/${plugins_dir}
      DESTINATION ${CMAKE_INSTALL_BINDIR}
      PATTERN "*d.so" EXCLUDE
    )
  endif ()
endforeach ()

if (LNX)
  install(
    DIRECTORY ${Qt6_base_dir}/plugins/tls
    DESTINATION ${CMAKE_INSTALL_BINDIR}
    PATTERN "*dd.so" EXCLUDE
  )
endif ()

set(qt_libs
    Bluetooth
    Concurrent
    Core
    Core5Compat
    DBus
    Designer
    DesignerComponents
    Gui
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
    QmlMeta
    QmlModels
    QmlWorkerScript
    QmlXmlListModel
    Quick
    QuickControls2
    QuickControls2Basic
    QuickControls2BasicStyleImpl
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
  if (LNX)
    install(FILES ${VCPKG_INSTALLED_DIR}/${VCPKG_TARGET_TRIPLET}/lib/libQt6${qt_lib}.so
            DESTINATION lib64
    )
  endif ()
endforeach ()

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

install(DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/app/android/assets/qgis-data
        DESTINATION ${assets_dir}
)

# ########################################################################################
# Platform
# ########################################################################################
if (WIN)
  install(FILES app/win/AppIcon.ico DESTINATION images/)
  install(FILES app/win/license.txt DESTINATION .)
endif ()
