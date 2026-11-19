/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

pragma ComponentBehavior: Bound

import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import QtQuick.Shapes

import mm 1.0 as MM
import MMInput

import "../../components" as MMComponents

/*
 * Photo drawing editor for feature photos. Opens a new page for drawing on photos.
 * Uses similar approach as MMFeaturesListPageDrawer, we use Dialog instead of Page as we would need to use stackManager,
 * for adding this page.
 */

Dialog {
  id: root

  property string photoUrl: ""
  property PhotoSketchingController controller

  // drawing a signature: full width canvas, guide line and hint instead of the color picker, done needs a drawing
  property bool signatureMode: false

  parent: Overlay.overlay
  width: parent.width
  height: parent.height
  anchors.centerIn: parent

  // undefined keeps the style's default padding
  leftPadding: signatureMode ? 0 : undefined
  rightPadding: signatureMode ? 0 : undefined

  background: Rectangle {
    color: __style.lightGreenColor
  }

  // also covers closing with Android back or Escape
  onRejected: root.controller.clear()

  header: RowLayout {

    MMComponents.MMRoundButton {
      id: backButton

      Layout.alignment: Qt.AlignVCenter
      Layout.topMargin: __style.pageMargins + __style.safeAreaTop
      Layout.leftMargin: __style.pageMargins + __style.safeAreaLeft

      onClicked: root.reject()
    }

    // keeps the undo button centered
    Item { Layout.fillWidth: true }

    MMComponents.MMRoundButton {
      iconSource: __style.undoIcon

      enabled: root.controller.canUndo

      Layout.alignment: Qt.AlignVCenter
      Layout.topMargin: __style.pageMargins + __style.safeAreaTop

      onClicked: root.controller.undo()
    }

    Item { Layout.fillWidth: true }

    MMComponents.MMRoundButton {
      id: doneButton

      Layout.alignment: Qt.AlignVCenter
      Layout.topMargin: __style.pageMargins + __style.safeAreaTop
      Layout.rightMargin: __style.pageMargins + __style.safeAreaRight

      iconSource: __style.checkmarkIcon
      iconColor: __style.forestColor
      bgndColor: __style.grassColor

      enabled: !root.signatureMode || root.controller.canUndo

      onClicked: {
        root.accept()
        root.controller.backupSketches()
      }
    }
  }

  contentItem: ColumnLayout {

    // centers the signature canvas vertically, together with the spacer below the hint text
    Item {
      visible: root.signatureMode
      Layout.fillHeight: true
    }

    Item {
      id: container

      // smaller margin in landscape leaves more height to the canvas
      readonly property real canvasTopMargin: {
        if ( !root.signatureMode )
          return 0

        if ( root.height > root.width )
          return __style.margin20

        return __style.margin8
      }
      readonly property real guideLineRatio: 0.85

      // position of the signature canvas origin in the viewport, the guide line is at the origin
      property real canvasX
      property real canvasY

      Layout.fillWidth: true
      Layout.fillHeight: true
      Layout.preferredHeight: root.signatureMode ? canvasTopMargin + Math.min( root.width, root.height ) * 1.25 : -1
      Layout.maximumHeight: root.signatureMode ? canvasTopMargin + Math.min( root.width, root.height ) * 1.25 : Number.POSITIVE_INFINITY
      Layout.leftMargin: root.signatureMode ? 0 : __style.pageMargins + __style.safeAreaLeft
      Layout.rightMargin: root.signatureMode ? 0 : __style.pageMargins + __style.safeAreaRight

      /**
       * Centers the signature canvas on the drawn signature together with the guide line, or on the guide line
       * if nothing is drawn yet. Zooms out only if the signature does not fit. Called when the size changes, never while drawing.
       */
      function updateSignatureView() {
        if ( !root.signatureMode || viewport.width <= 0 || viewport.height <= 0 )
          return

        // signature points are in pixels of the saved image, at zoom 1 one pixel of the screen
        const pixelRatio = container.Screen.devicePixelRatio
        let zoom = 1
        let focus = Qt.point( 0, 0 )
        let target = Qt.point( viewport.width / 2, viewport.height * guideLineRatio )

        if ( root.controller.canUndo ) {
          const bounds = root.controller.sketchBounds()
          const top = Math.min( bounds.y, 0 )
          const bottom = Math.max( bounds.y + bounds.height, 0 )

          focus = Qt.point( bounds.x + bounds.width / 2, ( top + bottom ) / 2 )
          target = Qt.point( viewport.width / 2, viewport.height / 2 )
          zoom = Math.min( 1, 0.9 * viewport.width * pixelRatio / bounds.width, 0.9 * viewport.height * pixelRatio / ( bottom - top ) )
        }

        const photoScale = pixelRatio / zoom
        root.controller.setPhotoScale( photoScale )
        canvasX = target.x - focus.x / photoScale
        canvasY = target.y - focus.y / photoScale
      }

      Item {
        id: viewport

        anchors.fill: parent
        anchors.topMargin: container.canvasTopMargin

        clip: root.signatureMode

        onWidthChanged: Qt.callLater( container.updateSignatureView )
        onHeightChanged: Qt.callLater( container.updateSignatureView )

        Rectangle {
          // the signature canvas, strokes can be drawn anywhere on it
          visible: root.signatureMode
          anchors.fill: parent
          color: __style.polarColor
        }

        MMComponents.MMPhoto {
          id: photo

          anchors.fill: parent
          visible: !root.signatureMode

          photoUrl: root.signatureMode ? "" : root.photoUrl
          fillMode: Image.PreserveAspectFit

          onPaintedWidthChanged: updateScaleRatio()
          onPaintedHeightChanged: updateScaleRatio()
          onStatusChanged: {
            if (status === Image.Ready) updateScaleRatio()
          }

          function updateScaleRatio() {
            if ( status === Image.Ready && sourceSize.width > 0 && sourceSize.height > 0 ) {
              root.controller.setPhotoScale( sourceSize.width / paintedWidth )
            }
          }
        }

        Shape {
          id: shape

          anchors.centerIn: root.signatureMode ? undefined : photo
          x: container.canvasX
          y: container.canvasY
          width: root.signatureMode ? viewport.width : photo.paintedWidth
          height: root.signatureMode ? viewport.height : photo.paintedHeight
          property var shapePaths: []

          Component.onCompleted: {
            root.controller.redrawPaths()
          }

          Connections {
            target: root.controller

            function onPathUpdated( indexArray ) {
              for ( const index of indexArray ) {
                const modelData = root.controller.getPath(index)
                const stringArray = shape.data.toString().split(",")
                const firstIndex = stringArray.findIndex(element => element.toString().includes("QQuickShapePath"));
                const itemIndex =  firstIndex + index
                shape.data[itemIndex].modelData = modelData
              }
            }

            function onNewPathAdded( pathIndex ) {
              const modelData = root.controller.getPath( pathIndex );
              const newObject = shapePathComponent.createObject( shape, {
                modelData: modelData,
              });
              if ( newObject ) {
                shape.data.push( newObject )
                shape.shapePaths.push( newObject )
              }
            }

            function onLastPathRemoved() {
              const stringArray = shape.data.toString().split(",")
              stringArray.reverse()
              let itemReverseIndex = stringArray.findIndex( element => element.toString().includes("QQuickShapePath") )
              const itemIndex = shape.data.length - itemReverseIndex - 1
              removeItemByIndex( itemIndex )
            }

            function onPathsReset() {
              // we delete all the paths in Shape
              while ( shape.data.findIndex( element => element.toString().includes("QQuickShapePath") ) > -1 )
              {
                const itemIndex = shape.data.findIndex( element => element.toString().includes("QQuickShapePath") )
                removeItemByIndex( itemIndex )
              }
            }

            function removeItemByIndex( index ) {
              const removedItem = shape.data.splice( index, 1, ...shape.data.slice( index + 1 ) )
              const shapePathsIndex = shape.shapePaths.findIndex( element => element.toString() === removedItem.toString() )
              shape.shapePaths[shapePathsIndex].destroy()
              shape.shapePaths.splice( shapePathsIndex, 1 )
            }
          }
        }

        PointHandler {
          id: pointHandler

          // a single handler, so newSketch() is always called before the first point of a line
          onActiveChanged: {
            if ( pointHandler.active ) {
              root.controller.newSketch()
            }
          }

          onPointChanged: {
            if ( !pointHandler.active ) {
              return
            }

            // the handler gets notified even when drag continues outside the viewport
            const position = pointHandler.point.position
            const outsideViewport = position.x < 0 || position.x > viewport.width || position.y < 0 || position.y > viewport.height

            const shapePosition = Qt.point( position.x - shape.x, position.y - shape.y )
            const outsidePhoto = !root.signatureMode && ( shapePosition.x < 0 || shapePosition.x > shape.width || shapePosition.y < 0 || shapePosition.y > shape.height )

            if ( outsideViewport || outsidePhoto ) {
              return
            }

            root.controller.addPoint( shapePosition )
          }
        }

        Rectangle {
          // signature guide line, not part of the saved image
          visible: root.signatureMode

          anchors.horizontalCenter: parent.horizontalCenter
          y: shape.y

          width: parent.width * 0.8
          height: 2

          color: __style.mediumGreyColor
        }
      }
    }

    MMComponents.MMText {
      visible: root.signatureMode
      opacity: root.controller.canUndo ? 0 : 1
      text: qsTr( "Draw your signature and confirm" )

      font: __style.p5
      color: __style.mediumGreyColor
      horizontalAlignment: Text.AlignHCenter

      Layout.fillWidth: true
      Layout.topMargin: __style.margin10
      Layout.bottomMargin: __style.margin10 + __style.safeAreaBottom
      Layout.leftMargin: __style.pageMargins + __style.safeAreaLeft
      Layout.rightMargin: __style.pageMargins + __style.safeAreaRight
    }

    Item {
      visible: root.signatureMode
      Layout.fillHeight: true
    }

    MMComponents.MMListSpacer {
      visible: !root.signatureMode
      implicitHeight: __style.margin20
    }

    MMComponents.MMColorPicker {
      visible: !root.signatureMode

      colors: [__style.photoSketchingWhiteColor, __style.photoSketchingBlackColor, __style.photoSketchingBlueColor, __style.photoSketchingGreenColor, __style.photoSketchingYellowColor, __style.photoSketchingOrangeColor, __style.photoSketchingPinkColor]
      activeColor: root.controller.activeColor

      Layout.alignment: Qt.AlignHCenter
      Layout.maximumWidth: parent.width - ( 2 * __style.pageMargins + __style.safeAreaLeft + __style.safeAreaRight )
      Layout.bottomMargin: __style.pageMargins + __style.safeAreaBottom
      Layout.leftMargin: __style.pageMargins + __style.safeAreaLeft
      Layout.rightMargin: __style.pageMargins + __style.safeAreaRight

      onActiveColorChangeRequested: newColor => root.controller.activeColor = newColor
    }
  }

  Component {
    id: shapePathComponent

    ShapePath {
      id: shapePathElement
      required property var modelData

      strokeColor: modelData.color
      strokeWidth: root.controller.sketchWidth
      fillColor: __style.transparentColor
      startX: modelData.points[0]?.x ?? 0
      startY: modelData.points[0]?.y ?? 0

      PathPolyline {
        path: shapePathElement.modelData.points
      }
    }
  }
}
