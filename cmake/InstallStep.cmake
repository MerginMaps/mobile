# GPLv2 Licence

set(QT_BIN_DIR ${Qt6_DIR}/../../../bin)
if (LINUX)
  find_program(QTDEPLOY_EXECUTABLE linuxdeploy linuxdeploy-x86_64.AppImage)
  set(ASSETS_DIR "data")
  set(executable_path "\${QT_DEPLOY_BIN_DIR}/$<TARGET_FILE_NAME:merginmaps>")
elseif (MACOS)
  find_program(
    QTDEPLOY_EXECUTABLE macdeployqt
    HINTS "${QT_BIN_DIR}"
    NO_DEFAULT_PATH
  )
  set(ASSETS_DIR "Mergin Maps.app/Contents/Resources/INPUT")
  set(executable_path "Mergin Maps.app")
elseif (IOS)
  find_program(
    QTDEPLOY_EXECUTABLE macdeployqt
    HINTS "${QT_BIN_DIR}"
    NO_DEFAULT_PATH
  )
  set(ASSETS_DIR "Mergin Maps.app/Contents/Resources/INPUT")
  set(executable_path "Mergin Maps.app")
elseif (ANDROID)
  find_program(
    QTDEPLOY_EXECUTABLE androiddeployqt
    HINTS "${QT_BIN_DIR}"
    NO_DEFAULT_PATH
  )
  set(ASSETS_DIR "data")
  set(executable_path "\${QT_DEPLOY_BIN_DIR}/$<TARGET_FILE_NAME:merginmaps>")
elseif (WIN)
  find_program(
    QTDEPLOY_EXECUTABLE windeployqt
    HINTS "${QT_BIN_DIR}"
    NO_DEFAULT_PATH
  )
  set(ASSETS_DIR "data")
  set(executable_path "\${QT_DEPLOY_BIN_DIR}/$<TARGET_FILE_NAME:merginmaps>")
else ()
  message(FATAL_ERROR "unknown deployment platform")
endif ()

# Binary
install(TARGETS merginmaps BUNDLE DESTINATION .)

# Assets
install(DIRECTORY app/android/assets/demo-projects DESTINATION ${ASSETS_DIR})
install(DIRECTORY app/android/assets/qgis-data DESTINATION ${ASSETS_DIR})

# Add its runtime dependencies
if (NOT LINUX)
  # https://doc-snapshots.qt.io/qt6-dev/qt-deploy-runtime-dependencies.html Replace with
  # qt_generate_deploy_script from QT 6.5.x The following script must only be executed at
  # install time Note: This command is in technology preview and may change in future
  # releases. (QT 6.4.x)
  set(deploy_script "${CMAKE_CURRENT_BINARY_DIR}/deploy_merginmaps.cmake")

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
