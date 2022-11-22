# GPLv2 Licence

set(QT_BIN_DIR ${Qt6_DIR}/../../../bin)
if (LNX)
  set(ASSETS_DIR "share/input")
  set(executable_path "\${QT_DEPLOY_BIN_DIR}/$<TARGET_FILE_NAME:Input>")
elseif (MACOS)
  set(ASSETS_DIR "Mergin Maps.app/Contents/Resources/INPUT")
  set(executable_path "Input.app")
elseif (IOS)
  set(ASSETS_DIR "Mergin Maps.app/Contents/Resources/INPUT")
  set(executable_path "Input.app")
elseif (ANDROID)
  set(ASSETS_DIR "data")
  set(executable_path "\${QT_DEPLOY_BIN_DIR}/$<TARGET_FILE_NAME:Input>")
elseif (WIN)
  set(ASSETS_DIR "data")
  set(executable_path "\${QT_DEPLOY_BIN_DIR}/$<TARGET_FILE_NAME:Input>")
else ()
  message(FATAL_ERROR "unknown deployment platform")
endif ()

get_property(isMultiConfig GLOBAL PROPERTY GENERATOR_IS_MULTI_CONFIG)
if (isMultiConfig)
  set(deploy_script "${CMAKE_CURRENT_BINARY_DIR}/deploy_input_$<CONFIG>.cmake")
else ()
  set(deploy_script "${CMAKE_CURRENT_BINARY_DIR}/deploy_input_${CMAKE_BUILD_TYPE}.cmake")
endif ()

if (WIN)
  # TODO migrate scripts/package_win.cmd to here
  return()
else ()
  # Binary
  install(
    TARGETS Input
    LIBRARY DESTINATION lib/
    BUNDLE DESTINATION .
  )
  # Assets
  install(DIRECTORY app/android/assets/demo-projects DESTINATION ${ASSETS_DIR})
  install(DIRECTORY app/android/assets/qgis-data DESTINATION ${ASSETS_DIR})

  # Add its runtime dependencies
  if (NOT LNX)
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
