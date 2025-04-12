set(QT_MODULES
    Qt6
    Qt6Designer
    Qt63DAnimation
    Qt63DCore
    Qt63DExtras
    Qt63DInput
    Qt63DLogic
    Qt63DQuick
    Qt63DQuickAnimation
    Qt63DQuickExtras
    Qt63DQuickInput
    Qt63DQuickRender
    Qt63DQuickScene2D
    Qt63DRender
    Qt6AccessibilitySupport
    Qt6AttributionsScannerTools
    Qt6Bluetooth
    Qt6Bootstrap
    Qt6BundledPcre2
    Qt6BundledLibpng
    Qt6BundledHarfbuzz
    Qt6BundledFreetype
    Qt6BuildInternals
    Qt6Bundled_Clip2Tri
    Qt6BundledFreetype
    Qt6BundledGlslang_Glslang
    Qt6BundledGlslang_Oglcompiler
    Qt6BundledGlslang_Osdependent
    Qt6BundledGlslang_Spirv
    Qt6BundledHarfbuzz
    Qt6BundledLibjpeg
    Qt6BundledLibpng
    Qt6BundledPcre2
    Qt6BundledResonanceAudio
    Qt6BundledSpirv_Cross
    Qt6Bundled_Clip2Tri
    Qt6Bundled_Clipper
    Qt6Bundled_Poly2Tri
    Qt6BundledPcre2
    Qt6HostInfo
    Qt6Linguist
    Qt6QmlDomPrivate
    Qt6QmlIntegration
    Qt6Tools
    Qt6UiPlugin
    Qt6Bluetooth
    Qt6BundledLibjpeg
    Qt6BundledLibpng
    Qt6Bundled_Clipper
    Qt6Charts
    Qt6ChartsQml
    Qt6ClipboardSupport
    Qt6Concurrent
    Qt6Core
    Qt6Core5Compat
    Qt6DeviceDiscoverySupport
    Qt6DocTools
    Qt6EntryPointPrivate
    Qt6EdidSupport
    Qt6EglSupport
    Qt6EntryPointPrivate
    Qt6EventDispatcherSupport
    Qt6FbSupport
    Qt6FFmpegMediaPluginImplPrivate
    Qt6FontDatabaseSupport
    Qt6Gamepad
    Qt6Gui
    Qt6GraphicsSupport
    Qt6Help
    Qt6HostInfo
    Qt6InputSupport
    Qt6LabsAnimation
    Qt6LabsFolderListModel
    Qt6LabsQmlModels
    Qt6LabsSettings
    Qt6LabsSharedImage
    Qt6LabsWavefrontMesh
    Qt6Location
    Qt6MacExtras
    Qt6Multimedia
    Qt6MultimediaQuick
    Qt6MultimediaQuickPrivate
    Qt6MultimediaWidgets
    Qt6Network
    Qt6Nfc
    Qt6OpenGL
    Qt6OpenGLExtensions
    Qt6OpenGLWidgets
    Qt6PacketProtocol
    Qt6PacketProtocolPrivate
    Qt6PlatformCompositorSupport
    Qt6PngPrivate
    Qt6Positioning
    Qt6PositioningQuick
    Qt6PrintSupport
    Qt6Qml
    Qt6QmlCompilerPrivate
    Qt6QmlCore
    Qt6QmlDebugPrivate
    Qt6QmlDomPrivate
    Qt6QmlIntegration
    Qt6QmlLocalStorage
    Qt6QmlMeta
    Qt6QmlModels
    Qt6QmlWorkerScript
    Qt6QmlXmlListModel
    Qt6Quick
    Qt6Quick3DUtils
    Qt6QuickControls2
    Qt6QuickControls2Impl
    Qt6QuickControls2ImplPrivate
    Qt6QuickDialogs2
    Qt6QuickDialogs2QuickImpl
    Qt6QuickDialogs2Utils
    Qt6QuickEffectsPrivate
    Qt6QuickLayouts
    Qt6QuickParticlesPrivate
    Qt6QuickShapesPrivate
    Qt6QuickTemplates2
    Qt6QuickTest
    Qt6QuickWidgets
    Qt6RemoteObjects
    Qt6RepParser
    Qt6Scxml
    Qt6Sensors
    Qt6SensorsQuick
    Qt6SerialPort
    Qt6ServiceSupport
    Qt6Sql
    Qt6Svg
    Qt6SvgWidgets
    Qt6Test
    Qt6TextToSpeech
    Qt6ThemeSupport
    Qt6UiPlugin
    Qt6UiTools
    Qt6VulkanSupport
    Qt6WebChannel
    Qt6WebSockets
    Qt6WebView
    Qt6WebViewQuick
    Qt6Widgets
    Qt6Xml
    Qt6XmlPatterns
    Qt6Zlib
    Qt6BundledPcre2
)

set(Qt6_ROOT_DIR $ENV{Qt6_DIR}/lib/cmake)

if(EXISTS ${Qt6_ROOT_DIR})
    MESSAGE(STATUS "Using Qt6 CMAKE dir: ${Qt6_ROOT_DIR}")
else()
    MESSAGE(FATAL_ERROR "Qt6 installation not found: ${Qt6_ROOT_DIR}; Do you have Qt6_DIR environment variable set?")
endif()

foreach(MOD ${QT_MODULES})
  if(EXISTS ${Qt6_ROOT_DIR}/${MOD})
      set(${MOD}_DIR ${Qt6_ROOT_DIR}/${MOD})
      MESSAGE(STATUS "Qt6 ${MOD} found: ${Qt6_ROOT_DIR}/${MOD}")
  else()
      # Not all modules are on all platforms
      MESSAGE(STATUS "Skipped -- Qt6 ${MOD}: ${Qt6_ROOT_DIR}/${MOD}")
  endif()
endforeach()

### HOST
# These modules are not in iOS/Android Qt, but they are needed for Qt
set(QT_HOST_MODULES
    Qt6CoreTools 
    Qt6LinguistTools
    Qt6WidgetsTools
    Qt6GuiTools
    Qt6QmlTools
    Qt6QmlImportScanner
)

set(Qt6_ROOT_HOST_DIR $ENV{QT_HOST_PATH}/lib/cmake)
if(EXISTS ${Qt6_ROOT_HOST_DIR})
  MESSAGE(STATUS "Using Qt6 host CMAKE dir from env. variable QT_HOST_PATH ${Qt6_ROOT_HOST_DIR}")
else()
  MESSAGE(STATUS "Using Qt6 host CMAKE dir from env. variable Qt6_DIR; for iOS or Android you need to set QT_HOST_PATH environment variable instead.")
  set(Qt6_ROOT_HOST_DIR ${Qt6_ROOT_DIR})
endif()

foreach(MOD ${QT_HOST_MODULES})
  if(EXISTS ${Qt6_ROOT_HOST_DIR}/${MOD})
      set(${MOD}_DIR ${Qt6_ROOT_HOST_DIR}/${MOD})
      MESSAGE(STATUS "Qt6 HOST ${MOD} found: ${Qt6_ROOT_HOST_DIR}/${MOD}")
  else()
      MESSAGE(STATUS "Skipped -- Qt6 HOST ${MOD} not found: ${Qt6_ROOT_HOST_DIR}/${MOD}")
  endif()
endforeach()

_find_package(${ARGS})