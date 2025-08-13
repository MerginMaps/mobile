# GPLv2 Licence

if (IOS OR ANDROID)
  message(FATAL_ERROR "Install step cannot be called on IOS and Android")
endif ()

set(Qt6_base_dir ${VCPKG_INSTALLED_DIR}/${VCPKG_TARGET_TRIPLET}/Qt6)

# ########################################################################################
# Binary
# ########################################################################################

if (WIN)
  install(
    TARGETS Input
    LIBRARY DESTINATION lib64/
    RUNTIME DESTINATION .
  )
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
endif ()

# ########################################################################################
# SDK Shared Libraries
# ########################################################################################
if (WIN)
  install(
    DIRECTORY ${VCPKG_INSTALLED_DIR}/${VCPKG_TARGET_TRIPLET}/bin/
    DESTINATION .
    FILES_MATCHING
    PATTERN "*.dll"
    PATTERN "Qca" EXCLUDE
  )
  install(
    DIRECTORY ${VCPKG_INSTALLED_DIR}/${VCPKG_TARGET_TRIPLET}/bin/Qca/crypto/
    DESTINATION .
    FILES_MATCHING
    PATTERN "*.dll"
  )
  install(
    DIRECTORY ${VCPKG_INSTALLED_DIR}/${VCPKG_TARGET_TRIPLET}/tools/qgis/plugins/
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
      DESTINATION ${CMAKE_INSTALL_BINDIR}
      PATTERN "*d.so" EXCLUDE
    )
  endif ()
endforeach ()

# tls has names like *backend[d].dll so excluding *d.dll doesn't work
if (WIN)
  install(
    DIRECTORY ${Qt6_base_dir}/plugins/tls
    DESTINATION .
    PATTERN "*dd.dll" EXCLUDE
  )
elseif (LNX)
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
  if (WIN)
    install(FILES ${VCPKG_INSTALLED_DIR}/${VCPKG_TARGET_TRIPLET}/bin/Qt6${qt_lib}.dll DESTINATION .)
  elseif (LNX)
    install(FILES ${VCPKG_INSTALLED_DIR}/${VCPKG_TARGET_TRIPLET}/lib/libQt6${qt_lib}.so DESTINATION lib64)
  endif ()
endforeach ()

if (WIN)
  install(FILES ${VCPKG_INSTALLED_DIR}/${VCPKG_TARGET_TRIPLET}/bin/d3dcompiler_47.dll ${VCPKG_INSTALLED_DIR}/${VCPKG_TARGET_TRIPLET}/bin/opengl32sw.dll
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

install(DIRECTORY app/android/assets/qgis-data DESTINATION ${assets_dir})

# ########################################################################################
# Platform
# ########################################################################################
if (WIN)
  install(FILES app/win/AppIcon.ico DESTINATION images/)
  install(FILES app/win/license.txt DESTINATION .)
endif ()
