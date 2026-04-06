/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

import QtQuick
import QtCore
import QtQuick.Controls
import QtMultimedia
import QtQml.Models
import QtPositioning // para GPS
import QtQuick.Dialogs
import QtQuick.Layouts

import QtQuick.Window

import mm 1.0 as MM
import MMInput

import "./map" // Componentes relacionados con el mapa (controlador, herramientas).
import "./dialogs" // Diálogos personalizados (errores, advertencias).
import "./layers"// Paneles para gestionar las capas del mapa
import "./components" // Botones, listas, etc., reutilizables.
import "./project"// Gestión de proyectos (carga, sincronización).
import "./settings" // Pantalla de configuración.
import "./gps" // Paneles para datos y seguimiento GPS.
import "./form" // La lógica para mostrar y editar los formularios de atributos de las entidades.

/*
(ApplicationWindow):
Es el contenedor principal de toda la aplicación.
Aquí se define el título de la app ("MDM Móvil 2026") y se configuran sus dimensiones, visibilidad y comportamiento
dependiendo de si el sistema operativo es iOS, Android o Windows
*/

ApplicationWindow {
  id: window

  visible: true
  x:  __appwindowx
  y:  __appwindowy
  width:  __appwindowwidth
  height: __appwindowheight
  visibility: __appwindowvisibility
  /*
    Banderas (flags):
    Configura el comportamiento de la ventana según el sistema operativo.
    En iOS, usa una bandera especial para maximizar la pantalla completa.
    En escritorio (Windows/Linux/macOS), agrega los botones típicos de título, minimizar, maximizar y cerrar.
  */
  flags: {
    if ( Qt.platform.os === "ios" ) {
      return Qt.Window | Qt.MaximizeUsingFullscreenGeometryHint
    }
    else if ( Qt.platform.os !== "ios" && Qt.platform.os !== "android" ) {
      return Qt.Window | Qt.WindowTitleHint | Qt.WindowSystemMenuHint |
          Qt.WindowMinMaxButtonsHint | Qt.WindowCloseButtonHint
    }

    return Qt.Window
  }

  title: "MDM Móvil 2026" // Do not translate

  /*
   Orientación (isPortraitOrientation):
   Detecta si la pantalla está en orientación vertical.
   Al cambiar, llama a recalculateSafeArea(),
   que ajusta los márgenes de la interfaz para evitar áreas no seguras (como la "muesca" o "isla dinámica" en móviles).
  */

  readonly property bool isPortraitOrientation: ( Screen.primaryOrientation === Qt.PortraitOrientation
                                                 || Screen.primaryOrientation === Qt.InvertedPortraitOrientation )

  onIsPortraitOrientationChanged: recalculateSafeArea()

  /*
   Guardar posición: Los manejadores onXChanged, onYChanged, etc.,
   disparan un timer (storeWindowPositionTimer) que guarda la geometría de la ventana en las configuraciones de la app
   después de 1 segundo de inactividad en el redimensionamiento.
  */

  // start window where it was closed last time
  onXChanged: storeWindowPosition()
  onYChanged: storeWindowPosition()
  onWidthChanged: storeWindowPosition()
  onHeightChanged: storeWindowPosition()

/*
 Gestor de Estados (stateManager):
 Es un componente que controla qué pantalla principal está viendo el usuario en un momento dado.
 Se divide en tres estados:
 "map" (la vista del mapa y herramientas de edición),
 "projects" (la lista de proyectos) y
 "misc" (para configuraciones y el panel del GPS).
*/

  Item {
    id: stateManager
    state: "map"

    states: [
      State {
        name: "map" // Working with map in an opened project - view, record, stakeout, form, ...
                    // La vista principal donde se ve el mapa y se realizan las operaciones de edición, medición, etc.
      },
      State {
        name: "projects" // Listing projects
                         // La lista de proyectos, donde el usuario puede abrir, crear o gestionar proyectos.
      },
      State {
        name: "misc" // Settings, GPS panel, ..
                     // Una categoría "miscelánea" que actualmente se usa para mostrar
                     // los paneles de configuración (settings) y el panel de capas (layers).
      }
    ]
// aquí carga el mapa 2026 < ---
    onStateChanged: {
      // Al entrar en "map", se asegura de que el estado interno del mapa sea "view" y
      // verifica si debe iniciar la animación de sincronización.
      if ( stateManager.state === "map" ) {
        map.state = "view"

        // Stop/Start sync animation when user goes to map
        syncButton.iconRotateAnimationRunning = ( __syncManager.hasPendingSync( __activeProject.projectFullName() ) )
      }
      // Al entrar en "projects", llama a projectController.openPanel() para mostrar la lista de proyectos.
      else if ( stateManager.state === "projects" ) {
        projectController.openPanel()
      }
      // Si se sale de "map", se establece el estado interno del mapa a "inactive", posiblemente para ahorrar recursos.
      if ( stateManager.state !== "map" ) {
        map.state = "inactive";
      }
    }
  }

  function showProjError(message) {
    projDialog.detailedDescription = message
    projDialog.open()
  }

  function identifyFeature( pair ) {
    let hasNullGeometry = pair.feature.geometry.isNull

    if ( hasNullGeometry ) {
      formsStackManager.openForm( pair, "readOnly", "form" )
    }
    else if ( pair.valid ) {
      map.highlightPair( pair )
      formsStackManager.openForm( pair, "readOnly", "preview")
    }
  }

/*  Component.onCompleted: {

    // load default project
    if ( AppSettings.defaultProject ) {
      let path = AppSettings.defaultProject

      if ( __localProjectsManager.projectIsValid( path ) && __activeProject.load( path ) ) {
        AppSettings.activeProject = path
      }
      else {
        // if default project load failed, delete default setting
        AppSettings.defaultProject = ""
        stateManager.state = "projects"
      }
    }
    else {
      stateManager.state = "projects"
    }

    // Catch back button click (if no other component catched it so far)
    // to prevent QT from quitting the APP immediately
    contentItem.Keys.released.connect( function( event ) {
      if ( event.key === Qt.Key_Back ) {
        event.accepted = true
        window.backButtonPressed()
      }
    } )

    console.log("Application initialized!")
  }*/

  /*Component.onCompleted: {
      // 1. Definir la ruta local a tu proyecto
      var rutaLocal = "E:/INEGI/Aguascalientes.qgz";
      console.log("Iniciando app personalizada. Intentando cargar: " + rutaLocal);

      // 2. Usar el objeto C++ interno de MerginMaps para cargar el proyecto
      if ( __activeProject.load( rutaLocal ) ) {
        // Si el proyecto carga correctamente:
        console.log("Proyecto cargado con éxito.");
        AppSettings.defaultProject = rutaLocal;
        AppSettings.activeProject = rutaLocal;

        // 3. Forzar a la interfaz a mostrar la pantalla del mapa
        stateManager.state = "map";
      } else {
        // Si la ruta está mal o el archivo no existe, no se quedará en blanco
        console.log("Error: No se pudo cargar el proyecto en la ruta especificada.");
        stateManager.state = "projects";
      }

      // 4. Capturar el botón físico de "Atrás" (Vital para Android)
      contentItem.Keys.released.connect( function( event ) {
        if ( event.key === Qt.Key_Back ) {
          event.accepted = true
          window.backButtonPressed()
      }
    } )
  }*/

/*
 Inicialización (Component.onCompleted):
 Es el código que se ejecuta automáticamente cuando la aplicación termina de abrirse.
 En el bloque actualmente activo, se fuerza a la app a entrar en el estado "map",
 se abre de inmediato un cuadro de diálogo para seleccionar un archivo (projectFileDialog.open())
 y se configura el comportamiento del botón físico de "Atrás" en los dispositivos móviles para evitar
 que la aplicación se cierre por accidente
*/

  Component.onCompleted: {
      // Primero definimos el estado para que la interfaz esté lista
      stateManager.state = "map"

      // Luego abrimos el selector de archivos
      //projectFileDialog.open()

      // El resto de tu lógica de botones...
      contentItem.Keys.released.connect( function( event ) {
        if ( event.key === Qt.Key_Back ) {
          event.accepted = true
          window.backButtonPressed()
        }
      } )
    }
/*
 Controlador del Mapa (MMMapController):
 Es el núcleo de la aplicación donde se visualiza la cartografía.
 Este componente contiene las instrucciones para reaccionar a las acciones del usuario,
 tales como:
 tocar un elemento (onFeatureIdentified),
 iniciar la captura de geometrías (onEditingGeometryStarted),
 medir distancias (onMeasureStarted) o
 encender el rastreo de ubicación (onOpenTrackingPanel).
*/
/*
 Este es el componente más importante.
 Hereda de un tipo personalizado (MMMapController) que seguramente envuelve la lógica de QGIS y el renderizado del mapa.
*/

  // Diálogo para crear nuevas tablas // 2026
  CreateTableDialog {
      id: createTableDialog
      parent: window

      // Ahora usa el objeto global registrado en QML
      dbManager: __dbManager
      currentDatabaseName: __dbManager.databaseName
      currentDatabasePath: __dbManager.databasePath

      onOpened: {
        stateManager.state = "misc"
        console.log("Se abre ventana de crear tabla");
      // window.currentDatabaseName = __dbManager.databaseName
       //   console.log("DB en " + Qt.platform.os + ": " + window.currentDatabaseName);
       // window.currentDatabasePath = __dbManager.databasePath
       //   console.log("DB en " + Qt.platform.os + ": " + window.currentDatabasePath);

      }

      onClosed: {
        stateManager.state = "map"
        console.log("Se cierra ventana de crear tabla");
      }
  }

  MMMapController {
    id: map
    // Su altura es window.height - mapToolbar.height, dejando espacio para la barra de herramientas inferior.
    height: window.height - mapToolbar.height
    width: window.width

    // Es una propiedad clave para la usabilidad.
    // Calcula cuánto espacio vertical está ocupando un panel deslizable (como el de "replanteo" o "medición")
    // que se abre desde la parte inferior.
    // El mapa usa este valor para centrar correctamente una entidad o ajustar la vista para que no quede oculta detrás del panel
    mapExtentOffset: {
      // offset depends on what panels are visible.
      // we need to subtract mapToolbar's height from any visible panel
      // because panels start at the bottom of the screen, but map canvas's height is lowered
      // by mapToolbar's height.
      if ( stakeoutPanelLoader.active )
      {
        // if stakeout panel is opened
        return stakeoutPanelLoader.item.panelHeight - mapToolbar.height
      }
      else if ( measurePanelLoader.active )
      {
        return measurePanelLoader.item.panelHeight - mapToolbar.height
      }
      else if ( multiSelectPanelLoader.active )
      {
        return multiSelectPanelLoader.item.panelHeight - mapToolbar.height
      }
      else if ( formsStackManager.takenVerticalSpace > 0 )
      {
        // if feature preview panel is opened
        return formsStackManager.takenVerticalSpace - mapToolbar.height
      }

      return 0
    }
    // Manejadores de Señales (onFeatureIdentified, onRecordingFinished, etc.):
    // Conectan las acciones del usuario en el mapa (tocar una entidad, terminar de dibujar un polígono)
    // con las acciones correspondientes de la interfaz, como abrir un formulario (formsStackManager.openForm),
    // iniciar un replanteo, o mostrar un panel de selección múltiple.
    onFeatureIdentified: function( pair ) {
      formsStackManager.openForm( pair, "readOnly", "preview" );
    }

    onFeaturesIdentified: function( pairs ) {
      formsStackManager.closeDrawer()
      featurePairSelection.showPairs( pairs );
    }

    onNothingIdentified: {
      formsStackManager.closeDrawer()
    }

    onRecordingFinished: function( pair ) {
      formsStackManager.openForm( pair, "add", "form" )
      map.highlightPair( pair )
    }

    onEditingGeometryStarted: 
    {
      mapPanelsStackView.hideMapStackIfNeeded()
      formsStackManager.geometryEditingStarted()
    }
    onEditingGeometryFinished: function( pair ) {
      mapPanelsStackView.showMapStack()
      formsStackManager.geometryEditingFinished( pair )
    }
    onEditingGeometryCanceled: {
      mapPanelsStackView.showMapStack()
      formsStackManager.geometryEditingFinished( null, false )
    }

    onRecordInLayerFeatureStarted: 
    {
      mapPanelsStackView.hideMapStackIfNeeded()
      formsStackManager.geometryEditingStarted()
    }
    onRecordInLayerFeatureFinished: function( pair ) {
      mapPanelsStackView.showMapStack()
      formsStackManager.recordInLayerFinished( pair )
    }
    onRecordInLayerFeatureCanceled: {
      mapPanelsStackView.showMapStack()
      formsStackManager.recordInLayerFinished( null, false )
    }

    onSplittingStarted: formsStackManager.hideAll()
    onSplittingFinished: {
      formsStackManager.closeAll()
    }
    onSplittingCanceled: {
      formsStackManager.reopenAll()
    }
    onAccuracyButtonClicked: {
      gpsDataDrawerLoader.active = true
      gpsDataDrawerLoader.focus = true
    }

    onStakeoutStarted: function( pair ) {
      stakeoutPanelLoader.active = true
      stakeoutPanelLoader.focus = true
      stakeoutPanelLoader.item.targetPair = pair
    }

    onMeasureStarted: function( pair ) {
      measurePanelLoader.active = true
      measurePanelLoader.focus = true
    }

    onMultiSelectStarted: {
      multiSelectPanelLoader.active = true
      multiSelectPanelLoader.focus = true
    }

    onDrawStarted: {
      sketchesPanelLoader.active = true
      sketchesPanelLoader.focus = true
    }

    onLocalChangesPanelRequested: {
      stateManager.state = "projects"
      projectController.openChangesPanel( __activeProject.projectFullName(), true )
    }

    onOpenTrackingPanel: {
      trackingPanelLoader.active = true
    }

    onOpenStreamingPanel: {
      streamingModeDialog.open()
    }

    Component.onCompleted: {
      __activeProject.mapSettings = map.mapSettings
      __iosUtils.positionKit = PositionKit
      __iosUtils.compass = map.compass
      __variablesManager.compass = map.compass
      __variablesManager.positionKit = PositionKit
    }
  }
  // Solicita y gestiona los permisos de ubicación precisa del dispositivo.
  LocationPermission {
    id: locationPermission
    accuracy: LocationPermission.Precise
  }

  // prueba 2026
  /*MMToolbar {
    id :mapToolBarRight
    anchors.bottom: parent.right
    visible: map.state === "view"
    model: ObjectModel {

      MMToolbarButton {
        id: syncButton2

        text: qsTr("Sync")
        iconSource: __style.syncIcon
        onClicked: {
          __activeProject.requestSync()
        }
      }


    }
  }*/

/*
 Barra de Herramientas Inferior (MMToolbar):
 Es la barra de botones que aparece en la parte inferior de la pantalla cuando el usuario está en la vista del mapa.
 Contiene un botón personalizado llamado "Abrir Archivo" que lanza el explorador de documentos del dispositivo.
 Incluye otros botones operativos como Agregar información ("Add"),
 ver las Capas ("Layers"), medir ("Measure") y Configuraciones ("Settings").
 Se puede observar que algunos botones predeterminados como "Sync" (Sincronizar),
 "Projects" (Proyectos) y "Local changes" (Cambios locales) fueron ocultados específicamente para esta versión (visible: false)
*/

  MMToolbar {
    id: mapToolbar

    anchors.bottom: parent.bottom
    visible: map.state === "view"

    model: ObjectModel {

      MMToolbarButton {
            text: qsTr("Base de Datos")
            iconSource: __style.addTableIcon
            visible: __activeProject.projectRole !== "reader"
            onClicked: {
              createDatabaseDialog.open()
            }
          }

      MMToolbarButton {
            text: qsTr("Tablas")
            iconSource: __style.addTableIcon
            visible: __activeProject.projectRole !== "reader"
            onClicked: {
              createTableDialog.open()
            }
          }


      MMToolbarButton {
        id: syncButton

        text: qsTr("Sync")
        iconSource: __style.syncIcon
        visible: false // <-- para que no sea visible 2026
        onClicked: {
          __activeProject.requestSync()
        }
      }

      MMToolbarButton {
              id: addTable
              text: qsTr("Abrir Archivo") // Cambié el texto // traducir
              iconSource: __style.homeIcon
              onClicked: {
                projectFileDialog.open() // Ahora este botón abre el explorador
              }
            }

      MMToolbarButton {
        id: addButton

        text: qsTr("Add")
        iconSource: __style.addIcon
        visible: __activeProject.projectRole !== "reader"
        onClicked: {
          if ( __activeProject.projectHasRecordingLayers() ) {
            stateManager.state = "map"
            map.record()
          }
          else {
            __notificationModel.addInfo( qsTr( "No editable layers found." ) )
          }
        }
      }

      MMToolbarButton {
        text: qsTr("Layers")
        iconSource: __style.layersIcon
        onClicked: {
          stateManager.state = "misc"
          let layerspanel = mapPanelsStackView.push( layersPanelComponent, {}, StackView.PushTransition )
        }
      }

      MMToolbarButton {
        text: qsTr("Projects")
        iconSource: __style.homeIcon
        visible: false // <-- para que no sea visible 2026
        onClicked: {
          stateManager.state = "projects"
        }
      }

      MMToolbarButton {
        text: qsTr("Zoom to project")
        iconSource: __style.zoomToProjectIcon
        onClicked: {
          map.centeredToGPS = false
          __inputUtils.zoomToProject( __activeProject.qgsProject, map.mapSettings )
        }
      }

      MMToolbarButton {
        text: qsTr("Map themes")
        iconSource: __style.mapThemesIcon
        onClicked: {
          mapThemesPanel.visible = true
          stateManager.state = "misc"
        }
      }

      MMToolbarButton {
        id: positionTrackingButton

        text: qsTr("Position tracking")
        iconSource: __style.positionTrackingIcon
        active: map.isTrackingPosition
        visible: __activeProject.positionTrackingSupported

        onClicked: {
          trackingPanelLoader.active = true
        }
      }

      MMToolbarButton {
        text: qsTr("Measure")
        iconSource: __style.measurementToolIcon
        onClicked: map.measure()
      }

      MMToolbarButton {
        text: qsTr("Local changes")
        iconSource: __style.localChangesIcon
        visible: false // <-- para que no sea visible 2026
        onClicked: {
          stateManager.state = "projects"
          projectController.openChangesPanel( __activeProject.projectFullName(), true )
        }
      }

      MMToolbarButton {
        text: qsTr("Settings")
        iconSource: __style.settingsIcon
        onClicked: {
          settingsController.open()
        }
      }
    }
  }

  MMSettingsController {
    id: settingsController

    onOpened: {
      stateManager.state = "misc"
    }

    onClosed: {
      stateManager.state = "map"
    }
  }

  MMProjectController {
    id: projectController

    height: window.height
    width: window.width

    activeProjectId: __activeProject.localProject.id() ?? ""

    onVisibleChanged: {
      if ( projectController.visible ) {
        projectController.forceActiveFocus()
      }
    }

    onOpenProjectRequested: function( projectPath ) {
      __activeProject.load( projectPath )
    }

    onClosed: stateManager.state = "map"
  }

/*
  Gestión de Vistas Dinámicas (StackView y Loader): *
  Usa un StackView para deslizar paneles sobre el mapa (por ejemplo, el menú de capas) con animaciones fluidas de entrada y salida.
  Usa elementos Loader para cargar paneles pesados (como la interfaz del GPS, paneles de selección múltiple o dibujo)
  en la memoria RAM únicamente cuando el usuario solicita abrirlos, lo cual optimiza el rendimiento.
*/

  StackView {
    id: mapPanelsStackView

    //
    // View that can show panels on top of the map,
    // like layers panel, settings and similar
    //

    anchors.fill: parent

    pushEnter: Transition {
      YAnimator {
        to: 0
        from: mapPanelsStackView.height
        duration: 400
        easing.type: Easing.OutCubic
      }
    }

    pushExit: Transition {}

    popEnter: Transition {}

    popExit: Transition {
      YAnimator {
        to: mapPanelsStackView.height
        from: 0
        duration: 400
        easing.type: Easing.OutCubic
      }
    }

    function hideMapStackIfNeeded() {
      // if present in the stack, hide the other layers when editing the geometry
      if(mapPanelsStackView.depth > 0){
        mapPanelsStackView.visible = false
      }
    }

    function showMapStack(){
        mapPanelsStackView.visible = true
    }
  }

  Component {
    id: layersPanelComponent

    MMLayersController {

      onClose: function() {
        mapPanelsStackView.clear( StackView.PopTransition )
        stateManager.state = "map"
      }

      onSelectFeature: function( featurePair ) {
        // close layers panel if the feature has geometry
        if ( __inputUtils.isSpatialLayer( featurePair.layer ) )
        {
          close()
        }

        window.identifyFeature( featurePair )
      }

      onAddFeature: function( targetLayer ) {
        let newPair = __inputUtils.createFeatureLayerPair( targetLayer, __inputUtils.emptyGeometry(), __variablesManager )
        formsStackManager.openForm( newPair, "add", "form" )

        // If we start supporting addition of spatial features from the layer's list,
        // make sure to change the root state here to "map"
      }
    }
  }

  Component {
    id: gpsDataDrawerComponent

    MMGpsDataDrawer {
      id: gpsDataDrawer

      mapSettings: map.mapSettings

      // disable the receivers button when staking out
      showReceiversButton: !stakeoutPanelLoader.active

      onManageReceiversClicked: {
        gpsDataDrawer.close()
        settingsController.open( MMSettingsController.Pages.GPSConnection )
      }

      onClosed: {
        gpsDataDrawerLoader.active = false
      }
    }
  }

  Loader {
    id: gpsDataDrawerLoader

    asynchronous: true
    active: false
    focus: true

    sourceComponent: gpsDataDrawerComponent

    onActiveChanged: {
      if ( active )
      {
        gpsDataDrawerLoader.item?.open()
      }
    }
  }

  MMMapThemeDrawer {
    id: mapThemesPanel

    width: window.width
    edge: Qt.BottomEdge

    onClosed: stateManager.state = "map"
  }

  MMStreamingModeDialog {
    id: streamingModeDialog

    streamingActive: map.isStreaming

    onStreamingBtnClicked: {
      map.toggleStreaming()
    }
  }

  Loader {
    id: trackingPanelLoader

    focus: true
    active: false
    asynchronous: true

    sourceComponent: Component {

      MMPositionTrackingDialog {

        width: window.width

        trackingActive: map.isTrackingPosition

        distanceTraveled: trackingPrivate.getDistance()
        trackingStartedAt: trackingPrivate.getStartingTime()

        onTrackingBtnClicked: map.setTracking( !trackingActive )

        onClosed: {
          trackingPanelLoader.active = false
        }

        QtObject {
          id: trackingPrivate

          function getDistance() {
            if ( map.isTrackingPosition ) {
              return __inputUtils.geometryLengthAsString( map.trackingManager?.trackedGeometry )
            }
            return qsTr( "not tracking" )
          }

          function getStartingTime() {
            if ( map.isTrackingPosition )
            {
              return map.trackingManager?.elapsedTimeText
            }
            return qsTr( "not tracking" )
          }
        }
      }
    }

    onActiveChanged: {
      if ( active )
      {
        trackingPanelLoader.item?.open()
      }
    }
  }

  MMProjectIssuesPage {
    id: projectIssuesPage

    height: window.height
    width: window.width
    visible: false

    onVisibleChanged: {
      if (projectIssuesPage.visible)
        projectIssuesPage.focus = true; // get focus
    }
  }

  Loader {
    id: stakeoutPanelLoader

    focus: true
    active: false
    asynchronous: true

    sourceComponent: stakeoutPanelComponent
  }

  Component {
    id: stakeoutPanelComponent

    MMStakeoutDrawer {
      id: stakeoutPanel

      width: window.width

      mapCanvas: map

      onStakeoutFinished: {
        map.stopStakeout()
        formsStackManager.openForm( targetPair, "readOnly", "preview" )
        stakeoutPanelLoader.active = false
      }

      onPanelHeightUpdated: map.updatePosition()
    }
  }

  Loader {
    id: multiSelectPanelLoader

    focus: true
    active: false
    asynchronous: true

    sourceComponent: multiSelectPanelComponent
  }

  Component {
    id: multiSelectPanelComponent

    MMSelectionDrawer {
      id: multiSelectPanel

      model: map.multiEditManager?.model
      layer: map.multiEditManager?.layer
      width: window.width

      onEditSelected: {
        let pair = map.multiEditManager.editableFeature()
        formsStackManager.openForm( pair, selectedCount === 1 ? "edit" : "multiEdit", "form" );
        multiSelectPanel.formOpened = true
      }

      onDeleteSelected: {
        deleteDialog.countToDelete = selectedCount
        deleteDialog.open()
      }

      onSelectionFinished: {
        multiSelectPanelLoader.active = false
        map.finishMultiSelect()
      }
    }
  }

  MMFormDeleteFeatureDialog {
    id: deleteDialog

    property int countToDelete: 0

    title: qsTr( "Delete %n feature(s)", "", countToDelete )
    description: qsTr( "Delete %n selected feature(s)?", "", countToDelete )

    primaryButton.text: qsTr( "Yes, I want to delete" )
    secondaryButton.text: qsTr( "No, thanks" )

    onDeleteFeature: {
      map.multiEditManager.deleteSelectedFeatures()
      if (multiSelectPanelLoader.item)
      {
        multiSelectPanelLoader.item.close()
      }
    }
  }

  Loader {
    id: sketchesPanelLoader

    focus: true
    active: false
    asynchronous: true

    sourceComponent: sketchesPanelComponent
  }

  Component {
    id: sketchesPanelComponent

    MMMapSketchesDrawer {
      id: sketchesPanel

      sketchingController: map.sketchingController

      width: window.width

      onClosed: {
        sketchesPanelLoader.active = false
        map.state = "view"
      }
    }
  }

  Loader {
    id: measurePanelLoader

    focus: true
    active: false
    asynchronous: true

    sourceComponent: measurePanelComponent
  }

  Component {
    id: measurePanelComponent

    MMMeasureDrawer {
      id: measurePanel

      width: window.width
      mapTool: map.mapToolComponent

      onMeasureFinished: {
        measurePanelLoader.active = false
        map.finishMeasure()
      }
    }
  }

/*
 Controlador de Formularios (MMFormStackController):
 Es el encargado de abrir y gestionar las pantallas de formularios donde el usuario puede ver,
 editar o rellenar la base de datos (atributos) de un punto, línea o polígono del mapa.
*/

  MMFormStackController {
    id: formsStackManager

    height: window.height
    width: window.width

    project: __activeProject.qgsProject

    /*
      Se llama cuando, desde un formulario, se quiere crear una nueva entidad relacionada.
      Si la capa destino no tiene geometría, crea la entidad directamente.
      Si la tiene, cambia el estado a "map" y activa la herramienta de grabación de geometría.
    */

    onCreateLinkedFeatureRequested: function( targetLayer, parentPair )  {
      if ( __inputUtils.isNoGeometryLayer( targetLayer) ) {
        let newPair = __inputUtils.createFeatureLayerPair( targetLayer, __inputUtils.emptyGeometry(), __variablesManager )
        recordInLayerFinished( newPair, true )
      }
      else { // we will record geometry
        stateManager.state = "map"
        map.recordInLayer( targetLayer, parentPair )
      }
    }

    // Permite al usuario editar la geometría de una entidad existente.
    // Cambia el estado a "map" y activa la herramienta de edición.

    onEditGeometryRequested: function( pair ) {
      stateManager.state = "map"
      map.edit( pair )
    }

    // onClosed:
    // Es un manejador crucial que se ejecuta cuando se cierra el último formulario.
    // Restaura el estado de la aplicación a "map" (o "misc" si el panel de capas estaba abierto),
    // oculta el resaltado del mapa y limpia cualquier modo de selección múltiple.

    onClosed: {
      if ( mapPanelsStackView.depth ) {
        // this must be layers panel as it is the only thing on the stackview currently
        const item = mapPanelsStackView.get( 0 )
        item.forceActiveFocus()
        stateManager.state = "misc"
      }
      else if ( gpsDataDrawerLoader.active )
      {
        stateManager.state = "misc"
        // do nothing, gps page already has focus
      }
      else {
        stateManager.state = "map"
      }

      map.hideHighlight()

      if ( multiSelectPanelLoader.active && multiSelectPanelLoader.item.formOpened )
      {
        multiSelectPanelLoader.active = false
        map.finishMultiSelect()
      }
    }

    onMultiSelectFeature: function( feature ) {
      closeDrawer()
      map.startMultiSelect( feature )
    }

    // Si la entidad es un punto y la ubicación GPS está disponible, inicia la herramienta de replanteo.

    onStakeoutFeature: function( feature ) {
      if ( !__inputUtils.isPointLayerFeature( feature ) )
        return;
      if ( !PositionKit.hasPosition )
      {
        __notificationModel.addWarning( qsTr( "Stake out is disabled because location is unavailable!" ) );
        return;
      }

      map.stakeout( feature )
      closeDrawer()
    }

    onPreviewPanelChanged: function( panelHeight ) {
      map.jumpToHighlighted( panelHeight - mapToolbar.height )
    }
  }

  MMProjectLoadingPage {
    id: projectLoadingPage

    anchors.fill: parent
    visible: false
  }

/*
  Alertas y Diálogos (Dialogs y FileDialog):
  Ventanas emergentes que advierten al usuario sobre diferentes situaciones,
  como límites de almacenamiento (MMStorageLimitDialog),
  errores al cargar el proyecto (MMProjectLoadErrorDialog) o
  problemas de permisos de inicio de sesión (MMMissingAuthDialog).
  En la parte final, se define el projectFileDialog,
  que es el explorador de archivos configurado específicamente para filtrar y buscar extensiones de QGIS (.qgz, .qgs).
*/

  // MMStorageLimitDialog: Alerta cuando se alcanza el límite de almacenamiento en la nube de Mergin.

  MMStorageLimitDialog {
    id: storageLimitDialog

    dataUsing: "%1 / %2".arg(__inputUtils.bytesToHumanSize(__merginApi.workspaceInfo.diskUsage)).arg(__inputUtils.bytesToHumanSize(__merginApi.workspaceInfo.storageLimit))
    usedData: __merginApi.workspaceInfo.storageLimit > 0 ? __merginApi.workspaceInfo.diskUsage / __merginApi.workspaceInfo.storageLimit : 0
    apiSupportsSubscription: __merginApi.apiSupportsSubscriptions

    onManageAccountClicked: Qt.openUrlExternally(__inputHelp.merginSubscriptionLink)
  }

  //MMProjectLimitDialog: Alerta sobre el límite de número de proyectos.

  MMProjectLimitDialog {
    id: projectLimitDialog

    apiSupportsSubscription: __merginApi.apiSupportsSubscriptions
    onManageAccountClicked: Qt.openUrlExternally(__inputHelp.merginSubscriptionLink)
  }

  MMProjErrorDialog {
    id: projDialog
  }

  MMOutOfDateCustomServerDialog{
    id: migrationDialog

    property string version

    onIgnoreClicked: {
      AppSettings.ignoreMigrateVersion = version
    }
  }

  //MMProjectLoadErrorDialog: Muestra errores al cargar un proyecto.

  MMProjectLoadErrorDialog {
    id: projectErrorDialog

    onClosed: {
      projectLoadingPage.visible = false
      projectController.openPanel()
    }
  }

  MMMigrateToMerginDialog {
    id: migrateToMerginDialog

    onMigrationRequested: __syncManager.migrateProjectToMergin( __activeProject.projectFullName() )
  }

  MMNoPermissionsDialog {
    id: noPermissionsDialog
  }

  //MMSyncFailedDialog: Informa de un fallo en la sincronización.

  MMSyncFailedDialog {
    id: syncFailedDialog
  }

  //MMMissingAuthDialog: Pide al usuario que inicie sesión si falta autenticación.

  MMMissingAuthDialog {
    id: missingAuthDialog

    onSingInRequested: {
      stateManager.state = "projects"
      projectController.showLogin()
    }
  }

  MMSsoExpiredTokenDialog {
    id: ssoExpiredTokenDialog

    onSingInRequested: {
      stateManager.state = "projects"
      projectController.showLogin()
    }
  }

  // MMNotificationView: Un área para mostrar mensajes de notificación no intrusivos (tipo "toast").

  MMNotificationView {}

  MMListDrawer {
    id: featurePairSelection

    drawerHeader.title: qsTr( "Select feature" )
    list.model: MM.StaticFeaturesModel {}

    list. delegate: MMListDelegate {
      text: model.FeatureTitle
      secondaryText: model.LayerName
      leftContent: MMIcon { source: model.LayerIcon }
      onClicked: {
        let pair = model.FeaturePair
        featurePairSelection.close()
        map.highlightPair( pair )
        formsStackManager.openForm( pair, "readOnly", "preview" );
      }
    }

    function showPairs( pairs ) {
      if ( pairs.length > 0 )
      {
        list.model.populate( pairs )
        open()
      }
    }
  }

  MMWelcomeToNewDesignDialog {
    id: welcomeToNewDesignDialog

    Component.onCompleted: {
      if ( __showWelcomeToNewDesignDialog )
        open()
    }
  }
/*
 Conexiones con el Sistema Central (Connections):
 Son bloques lógicos que "escuchan" en segundo plano lo que sucede en el núcleo de la app (C++) para actualizar la interfaz.
 Por ejemplo, reciben señales si la sincronización del proyecto falla (onSyncError) ,
 si se pierde la conexión de red (onNetworkErrorOccurred) o
 si se conceden/deniegan los permisos de ubicación GPS del celular al momento de abrir el proyecto (onLoadingFinished)
*/

  Connections {
    target: __syncManager
    enabled: stateManager.state === "map"

    // onSyncStarted / onSyncFinished:
    // Actualizan la animación del ícono de sincronización y muestran mensajes de éxito.

    function onSyncStarted( projectFullName )
    {
      if ( projectFullName === __activeProject.projectFullName() )
      {
        syncButton.iconRotateAnimationRunning = true
      }
    }

    function onSyncFinished( projectFullName, success )
    {
      if ( projectFullName === __activeProject.projectFullName() )
      {
        syncButton.iconRotateAnimationRunning = false

        if ( success )
        {
          __notificationModel.addSuccess( qsTr( "Successfully synchronised" ) )

          // refresh canvas
          map.refreshMap()
        }
      }
    }

    function onSyncCancelled( projectFullName )
    {
      if ( projectFullName === __activeProject.projectFullName() )
      {
        syncButton.iconRotateAnimationRunning = false
      }
    }

    function onSyncError( projectFullName, errorType, willRetry, errorMessage )
    {
      if ( projectFullName === __activeProject.projectFullName() )
      {
        if ( errorType === MM.SyncError.NotAMerginProject )
        {
          migrateToMerginDialog.open()
        }
        else if ( errorType === MM.SyncError.NoPermissions )
        {
          noPermissionsDialog.open()
        }
        else if ( errorType === MM.SyncError.AnotherProcessIsRunning && willRetry )
        {
          // just banner that we will try again
          __notificationModel.addInfo( qsTr( "Somebody else is syncing, we will try again later" ) )
        }
        else
        {
          syncFailedDialog.detailedText = errorMessage
          if ( willRetry )
          {
            __notificationModel.addError( qsTr( "There was an issue during synchronisation, we will try again. Click to learn more" ),
            MM.NotificationType.ShowSyncFailedDialog )
          }
          else
          {
            syncFailedDialog.open()
          }
        }
      }
    }

    function onProjectAlreadyOnLatestVersion( projectFullName )
    {
      if ( projectFullName === __activeProject.projectFullName() )
      {
        __notificationModel.addSuccess( qsTr( "Up to date" ) )
      }
    }
  }

  Connections {
    target: __merginApi

    // onNetworkErrorOccurred: Muestra un error de red en la pantalla de proyectos.

    function onNetworkErrorOccurred( message, topic, httpCode, projectFullName ) {
      if ( stateManager.state === "projects" )
      {
        var msg = message ? message : qsTr( "Failed to communicate with server. Try improving your network connection." )
        __notificationModel.addError( msg )
      }
    }

    // onStorageLimitReached: Abre el diálogo de límite de almacenamiento.

    function onStorageLimitReached( uploadSize ) {
      __merginApi.getUserInfo()
      if (__merginApi.apiSupportsSubscriptions) {
        __merginApi.getWorkspaceInfo()
      }
      storageLimitDialog.dataToSync = __inputUtils.bytesToHumanSize(uploadSize)
      storageLimitDialog.open()
    }

    function onProjectLimitReached( maxProjects, errorMsg ) {
      __merginApi.getUserInfo()
      if (__merginApi.apiSupportsSubscriptions) {
        __merginApi.getWorkspaceInfo()
      }
      projectLimitDialog.maxProjectNumber = maxProjects
      projectLimitDialog.open()

      syncButton.iconRotateAnimationRunning = false
    }

    function onProjectDataChanged( projectFullName ) {
      //! if current project has been updated, refresh canvas
      if ( projectFullName === projectController.activeProjectId ) {
        map.mapSettings.extentChanged()
      }
    }

    function onMigrationRequested( version ) {
      if( AppSettings.ignoreMigrateVersion !== version ) {
        migrationDialog.version = version
        migrationDialog.open()
      }
    }

    function onMissingAuthorizationError( projectFullName )
    {
      if ( projectFullName === __activeProject.projectFullName() && !__merginApi.userAuth.isUsingSso() )
      {
        missingAuthDialog.open()
      }
    }

    function onProjectCreationFailed()
    {
      syncButton.iconRotateAnimationRunning = false
    }

    function onSsoLoginExpired()
    {
      ssoExpiredTokenDialog.open()
    }
  }

  Connections {
    target: __inputProjUtils
    function onProjError( message ) {
      showProjError(message)
    }
  }

  Connections {
    target: __notificationModel
    function onShowProjectIssuesActionClicked() {
      projectIssuesPage.projectLoadingLog = __activeProject.projectLoadingLog();
      projectIssuesPage.visible = true;
    }
    function onShowSwitchWorkspaceActionClicked() {
      stateManager.state = "projects"
      projectController.showSelectWorkspacePage()
    }
    function onShowSyncFailedDialogClicked() {
      syncFailedDialog.open()
    }
  }

  Connections {
    target: __activeProject

    // onLoadingStarted / onLoadingFinished:
    // Muestran/ocultan una pantalla de carga al abrir un proyecto.
    // Al terminar la carga, se solicita el permiso de ubicación si aún no se ha concedido.

    function onLoadingStarted() {
      projectLoadingPage.visible = true;
      projectIssuesPage.clear();
    }

    function onLoadingFinished() {
      projectLoadingPage.visible = false

      if ( __activeProject.isProjectLoaded() )
      {
        projectController.hidePanel()
      }

      // check location permission
      if ( locationPermission.status === Qt.Undetermined ) {
        // This is the place where we actually request permissions.
        // When the system's request permissions dialog get closed,
        // we get a notification that our application is active again,
        // and PositionKit::appStateChanged() will try to start updates.
        locationPermission.request();
      }
      else if ( locationPermission.status === Qt.Denied ) {
        __inputUtils.log("Permissions", "Location permission is denied")
      }
    }

    // onLoadingErrorFound:
    // Notifica al usuario que hubo problemas al cargar el proyecto (como capas faltantes) y
    // le ofrece un enlace para ver los detalles.

    function onLoadingErrorFound() {
      __notificationModel.addWarning(
        __inputUtils.htmlLink(qsTr( "There were issues loading the project. %1View details%2" ), __style.forestColor),
        MM.NotificationType.ShowProjectIssuesAction
      )
    }

    function onReportIssue( title, message ) {
      projectIssuesPage.reportIssue( title, message )
    }

    function onProjectReloaded( project ) {
      map.clear()

      AppSettings.defaultProject = __activeProject.localProject.qgisProjectFilePath ?? ""
      AppSettings.activeProject = __activeProject.localProject.qgisProjectFilePath ?? ""
    }

    function onProjectWillBeReloaded() {
      formsStackManager.reload()
    }

    function onProjectReadingFailed( message ) {
      projectErrorDialog.informativeText = qsTr( "Could not read the project file:" ) + "\n" + message
      projectErrorDialog.open()
    }

    function onPositionTrackingSupportedChanged() {
      positionTrackingButton.visible = __activeProject.positionTrackingSupported
      mapToolbar.recalculate()
    }
  }

  Timer {
    id: closeAppTimer

    interval: 3000
    running: false
    repeat: false
  }

  Timer {
    id: storeWindowPositionTimer

    interval: 1000

    onTriggered: AppSettings.windowPosition = [window.x, window.y, window.width, window.height]
  }

  function backButtonPressed() {

    if ( closeAppTimer.running ) {
      __inputUtils.quitApp()
    }
    else {
      closeAppTimer.start()
      __notificationModel.addInfo( qsTr( "Press back again to quit the app" ) )
    }
  }

  function recalculateSafeArea() {
    let safeArea = []

    // Should be merged in future with the same code in main.cpp
    if ( Qt.platform.os === "ios" ) {
      safeArea = Array.from( __iosUtils.getSafeArea() )
    }
    else if ( Qt.platform.os === "android" ) {
      safeArea = Array.from( __androidUtils.getSafeArea() )

      // Values from Android API must be divided by dpr
      safeArea[0] = safeArea[0] / Screen.devicePixelRatio
      safeArea[1] = safeArea[1] / Screen.devicePixelRatio
      safeArea[2] = safeArea[2] / Screen.devicePixelRatio
      safeArea[3] = safeArea[3] / Screen.devicePixelRatio
    }

    if ( safeArea.length === 4 ) {
      __style.safeAreaTop = safeArea[0]
      __style.safeAreaRight = safeArea[1]
      __style.safeAreaBottom = safeArea[2]
      __style.safeAreaLeft = safeArea[3]
    }
  }

  function storeWindowPosition() {
    if ( Qt.platform.os !== "ios" && Qt.platform.os !== "android")
    {
      storeWindowPositionTimer.restart()
    }
  }

  /*Component.onCompleted: {
      // 1. Definir la ruta local a tu proyecto
      var rutaLocal = "E:/INEGI/Aguascalientes.qgz";
      console.log("Iniciando app personalizada. Intentando cargar: " + rutaLocal);

      // 2. Usar el objeto C++ interno de MerginMaps para cargar el proyecto
      if ( __activeProject.load( rutaLocal ) ) {
        // Si el proyecto carga correctamente:
        console.log("Proyecto cargado con éxito.");
        AppSettings.defaultProject = rutaLocal;
        AppSettings.activeProject = rutaLocal;

        // 3. Forzar a la interfaz a mostrar la pantalla del mapa
        stateManager.state = "map";
      } else {
        // Si la ruta está mal o el archivo no existe, no se quedará en blanco
        console.log("Error: No se pudo cargar el proyecto en la ruta especificada.");
        stateManager.state = "projects";
      }

      // 4. Capturar el botón físico de "Atrás" (Vital para Android)
      contentItem.Keys.released.connect( function( event ) {
        if ( event.key === Qt.Key_Back ) {
          event.accepted = true
          window.backButtonPressed()
      }
    } )
  }*/
  FileDialog {
      id: projectFileDialog
      title: "Seleccionar Proyecto QGIS"
      // Usamos el nombre de propiedad correcto según tu versión de Qt
//      currentFolder: StandardPaths.writableLocation(StandardPaths.DocumentsLocation)
      nameFilters: ["Archivos de proyecto QGIS (*.qgz *.qgs)"]


      /*onAccepted: {
        // 1. Convertimos el objeto URL a texto
        var rutaSucia = selectedFile.toString();

        // 2. Limpiamos el prefijo 'file:///'
        // Esto funciona tanto para Windows (E:/...) como para otros sistemas
        var rutaLimpia = rutaSucia.replace("file:///", "");

        // 3. En Windows, a veces queda una barra inicial extra (/E:/...), la quitamos:
        if (rutaLimpia.charAt(0) === '/' && rutaLimpia.charAt(2) === ':') {
            rutaLimpia = rutaLimpia.substring(1);
        }

        console.log("Ruta procesada para QGIS: " + rutaLimpia);

        // 4. Intentamos cargar
        if ( __activeProject.load( rutaLimpia ) ) {
          stateManager.state = "map"
          AppSettings.activeProject = rutaLimpia
          AppSettings.defaultProject = rutaLimpia
        } else {
          console.log("Error al cargar el proyecto en: " + rutaLimpia)
          stateManager.state = "projects"
        }
      }*/

      onAccepted: {
          var rutaSucia = selectedFile.toString();
          var rutaLimpia = "";

          if (Qt.platform.os === "android") {
              // En Android, el FileDialog suele devolver algo como "content://..." o "file:///storage/..."
              // Intentamos limpiar el prefijo file://
              rutaLimpia = rutaSucia.replace("file://", "");
              // Si el FileDialog de Android te da un "content://",
              // MerginMaps suele tener una función interna para eso:
              if (rutaLimpia.startsWith("content://")) {
                  rutaLimpia = __inputUtils.urlToLocalFile(selectedFile);
              }
          } else {
              // Lógica de Windows que ya tenías
              rutaLimpia = rutaSucia.replace("file:///", "");
              if (rutaLimpia.charAt(0) === '/' && rutaLimpia.charAt(2) === ':') {
                  rutaLimpia = rutaLimpia.substring(1);
              }
          }

          console.log("Ruta en " + Qt.platform.os + ": " + rutaLimpia);

          if ( __activeProject.load( rutaLimpia ) ) {
              stateManager.state = "map"
          }
      }



      onRejected: {
        stateManager.state = "map"
      }
    }

  // =====================================================================
  // DIÁLOGO: CREAR BD
  // =====================================================================
  Dialog {
    id: createDatabaseDialog
    title: "Crear nueva base de datos"
    width: 300
    height: 220
    modal: true
    footer: DialogButtonBox {
            Button {
                text: "Aceptar"
                DialogButtonBox.buttonRole: DialogButtonBox.AcceptRole
                implicitWidth: 5 // Tamaño fijo
                implicitHeight: 20
                font.pointSize: 8
            }
            Button {
                text: "Cancelar"
                DialogButtonBox.buttonRole: DialogButtonBox.DestructiveRole
                implicitWidth: 5
                implicitHeight: 20
                font.pointSize: 8
            }
        }

    onAccepted: {
      if (dbNameInput.text.trim() === "") {
        errorMessage.text = "El nombre no puede estar vacío"
        return
      }

      if (__dbManager.initializeDatabase(dbNameInput.text.trim(), dbPathInput.text.trim())) {
        window.currentDatabaseName = dbNameInput.text.trim()
          console.log("DB en " + Qt.platform.os + ": " + window.currentDatabaseName);
        window.currentDatabasePath = __dbManager.databasePath
          console.log("DB en " + Qt.platform.os + ": " + window.currentDatabasePath);
        successMessage.text = "✓ Base de datos creada en:\n" + __dbManager.databasePath
        dbNameInput.text = ""
        dbPathInput.text = ""
        errorMessage.text = ""
        successTimer.start()
      } else {
        errorMessage.text = "Error: " + __dbManager.getLastError()
        successMessage.text = ""
      }
    }

    ColumnLayout {
      anchors.fill: parent
      anchors.margins: 15
      spacing: 12

      ColumnLayout {
        Layout.fillWidth: true
        spacing: 5

        Text {
          text: "Nombre de la Base de Datos:"
          font.bold: true
          font.pointSize: 8
        }
        Rectangle {
            width: 150
            height: 20
            color: "lightgrey"

           TextField {
             id: dbNameInput

             anchors.fill: parent
             width: parent.width * 0.8 // 80% del ancho del padre
             font.pixelSize: 8

             placeholderText: "Ej: miproyecto"
             Layout.fillWidth: true
            }
          }
      }


      ColumnLayout {
        Layout.fillWidth: true
        spacing: 5

        Text {
          text: "Ubicación (opcional, Enter para predeterminada):"
          font.bold: true
          font.pointSize: 8
        }

        Rectangle {
            width: 150
            height: 20
            color: "lightgrey"
        TextField {
          id: dbPathInput
          anchors.fill: parent
          width: parent.width * 0.8 // 80% del ancho del padre
          font.pixelSize: 8
         // font.pointSize: 8
          placeholderText: "Ej: E:/Mis Documentos/"
          Layout.fillWidth: true
        }
        }
      }

      Text {
        id: successMessage
        text: ""
        color: "#4CAF50"
        visible: text !== ""
        wrapMode: Text.Wrap
      }

      Text {
        id: errorMessage
        text: ""
        color: "#d32f2f"
        visible: text !== ""
        wrapMode: Text.Wrap
      }

      Item { Layout.fillHeight: true }
    }

    Timer {
      id: successTimer
      interval: 2000
      onTriggered: {
        createDatabaseDialog.close()
      }
    }
  }

}




