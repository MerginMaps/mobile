# GPLv2 Licence

if (WIN)
  set(Qt6_base_dir ${Qt6_DIR}/../../..)
  # Binary
  install(
    TARGETS Input
    LIBRARY DESTINATION lib/
    RUNTIME DESTINATION .
    BUNDLE DESTINATION .
  )

  set(regex_no_db_libs "[(A-Z)|(a-z)|(0-9)|_][^d]\.dll")
  # Libraries
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

  # Qt
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
    install(
      DIRECTORY ${Qt6_base_dir}/qml/${qml_dir}
      DESTINATION qml
      PATTERN "*d.dll" EXCLUDE
    )
  endforeach ()

  set(plugins_dirs
      position
      sqldrivers
      imageformats
      platforms
      multimedia
  )
  foreach (plugins_dir ${plugins_dirs})
    install(
      DIRECTORY ${Qt6_base_dir}/plugins/${plugins_dir}
      DESTINATION .
      PATTERN "*d.dll" EXCLUDE
    )
  endforeach ()

  set(qt_libs
      Qt6Bluetooth
      Qt6Concurrent
      Qt6Core
      Qt6Core5Compat
      Qt6DBus
      Qt6Designer
      Qt6DesignerComponents
      Qt6Gui
      Qt6Help
      Qt6LabsAnimation
      Qt6LabsFolderListModel
      Qt6LabsQmlModels
      Qt6LabsSettings
      Qt6LabsSharedImage
      Qt6LabsWavefrontMesh
      Qt6Multimedia
      Qt6MultimediaQuick
      Qt6MultimediaWidgets
      Qt6Network
      Qt6Nfc
      Qt6OpenGL
      Qt6OpenGLWidgets
      Qt6Positioning
      Qt6PositioningQuick
      Qt6PrintSupport
      Qt6Qml
      Qt6QmlCompiler
      Qt6QmlCore
      Qt6QmlLocalStorage
      Qt6QmlModels
      Qt6QmlWorkerScript
      Qt6QmlXmlListModel
      Qt6Quick
      Qt6Quick3DSpatialAudio
      Qt6QuickControls2
      Qt6QuickControls2Impl
      Qt6QuickDialogs2
      Qt6QuickDialogs2QuickImpl
      Qt6QuickDialogs2Utils
      Qt6QuickLayouts
      Qt6QuickParticles
      Qt6QuickShapes
      Qt6QuickTemplates2
      Qt6QuickTest
      Qt6QuickWidgets
      Qt6Sensors
      Qt6SensorsQuick
      Qt6ShaderTools
      Qt6SpatialAudio
      Qt6Sql
      Qt6Svg
      Qt6SvgWidgets
      Qt6Test
      Qt6UiTools
      Qt6Widgets
      Qt6Xml
      d3dcompiler_47
      opengl32sw
  )

  foreach (qt_lib ${qt_libs})
    install(FILES ${Qt6_base_dir}/bin/${qt_lib}.dll DESTINATION .)
  endforeach ()

  # Translations
  install(
    DIRECTORY app/i18n/
    DESTINATION translations
    FILES_MATCHING
    PATTERN "*.qm"
  )

  # Assets
  install(FILES app/win/AppIcon.ico DESTINATION images/)
  install(FILES app/win/license.txt DESTINATION .)
  install(DIRECTORY app/android/assets/demo-projects DESTINATION .)
  install(DIRECTORY app/android/assets/qgis-data DESTINATION .)

else ()
  set(QT_BIN_DIR ${Qt6_DIR}/../../../bin)
  if (LNX)
    set(assets_dir "share/INPUT")
    set(executable_path "\${QT_DEPLOY_BIN_DIR}/$<TARGET_FILE_NAME:Input>")
  elseif (MACOS)
    set(assets_dir "Input.app/Contents/Resources/INPUT")
    set(executable_path "Input.app")
  endif ()

  # Binary
  install(
    TARGETS Input
    LIBRARY DESTINATION lib/
    BUNDLE DESTINATION .
  )

  # Assets
  install(DIRECTORY app/android/assets/demo-projects DESTINATION ${assets_dir})
  install(DIRECTORY app/android/assets/qgis-data DESTINATION ${assets_dir})

  # Use macdeployqt when possible
  if (MACOS)
    get_property(isMultiConfig GLOBAL PROPERTY GENERATOR_IS_MULTI_CONFIG)
    if (isMultiConfig)
      set(deploy_script "${CMAKE_CURRENT_BINARY_DIR}/deploy_input_$<CONFIG>.cmake")
    else ()
      set(deploy_script
          "${CMAKE_CURRENT_BINARY_DIR}/deploy_input_${CMAKE_BUILD_TYPE}.cmake"
      )
    endif ()

    # https://doc-snapshots.qt.io/qt6-dev/qt-deploy-runtime-dependencies.html Replace with
    # qt_generate_deploy_script from QT 6.5.x The following script must only be executed
    # at install time Note: This command is in technology preview and may change in future
    # releases. (QT 6.4.x)
    file(
      GENERATE
      OUTPUT ${deploy_script}
      CONTENT
        "
			# Including the file pointed to by QT_DEPLOY_SUPPORT ensures the generated
			# deployment script has access to qt_deploy_runtime_dependencies()
			include(\"${QT_DEPLOY_SUPPORT}\")
			qt_deploy_runtime_dependencies(
				EXECUTABLE \"${executable_path}\"
				GENERATE_QT_CONF
				VERBOSE
			)"
    )

    install(SCRIPT ${deploy_script})
  endif ()

endif ()
