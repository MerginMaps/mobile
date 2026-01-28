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

  parent: Overlay.overlay
  width: parent.width
  height: parent.height
  anchors.centerIn: parent

  background: Rectangle {
    color: __style.lightGreenColor
  }

  header: RowLayout {

    MMComponents.MMRoundButton {
      id: backButton

      Layout.alignment: Qt.AlignVCenter | Qt.AlignLeft
      Layout.topMargin: __style.pageMargins + __style.safeAreaTop
      Layout.leftMargin: __style.pageMargins + __style.safeAreaLeft

      onClicked: {
        root.close()
        root.controller.clear()
      }
    }

    MMComponents.MMRoundButton {
      iconSource: __style.undoIcon

      enabled: root.controller.canUndo

      Layout.alignment: Qt.AlignVCenter | Qt.AlignHCenter
      Layout.topMargin: __style.pageMargins + __style.safeAreaTop

      onClicked: root.controller.undo()
    }

    MMComponents.MMRoundButton {
      id: doneButton

      Layout.alignment: Qt.AlignVCenter | Qt.AlignRight
      Layout.topMargin: __style.pageMargins + __style.safeAreaTop
      Layout.rightMargin: __style.pageMargins + __style.safeAreaRight

      iconSource: __style.checkmarkIcon
      iconColor: __style.forestColor
      bgndColor: __style.grassColor

      onClicked: {
        root.close()
        root.controller.backupSketches()
      }
    }
  }

  contentItem: ColumnLayout {

    Item {
      id: container
      Layout.fillWidth: true
      Layout.fillHeight: true
      Layout.leftMargin: __style.pageMargins + __style.safeAreaLeft
      Layout.rightMargin: __style.pageMargins + __style.safeAreaRight

      MMComponents.MMPhoto {
        id: photo

        anchors.fill: parent

        photoUrl: root.photoUrl
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
        anchors.centerIn: parent
        width: photo.paintedWidth
        height: photo.paintedHeight
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

        DragHandler {
          id: dragHandler
          target: null

          onCentroidChanged: {
            // the drag handler gets notified even when drag continues outside parent
            const outsideImageBounds = centroid.position.x > parent.width || centroid.position.x < 0 || centroid.position.y > parent.height || centroid.position.y < 0
            if (!outsideImageBounds) {
              // centroid gets set to (0, 0) when drag stops
              if ( dragHandler.centroid.position === Qt.point( 0, 0 ) ) {
                root.controller.newSketch()
              }
              else {
                root.controller.addPoint( dragHandler.centroid.position )
              }
            }
          }
        }
      }
    }

    MMComponents.MMListSpacer { implicitHeight: __style.margin20 }

    MMComponents.MMColorPicker{
      colors: [__style.photoSketchingWhiteColor, __style.photoSketchingBlackColor, __style.photoSketchingBlueColor, __style.photoSketchingGreenColor, __style.photoSketchingYellowColor, __style.photoSketchingOrangeColor, __style.photoSketchingPinkColor]

      Layout.alignment: Qt.AlignHCenter
      Layout.maximumWidth: parent.width - ( 2 * __style.pageMargins + __style.safeAreaLeft + __style.safeAreaRight )
      Layout.bottomMargin: __style.pageMargins + __style.safeAreaBottom
      Layout.leftMargin: __style.pageMargins + __style.safeAreaLeft
      Layout.rightMargin: __style.pageMargins + __style.safeAreaRight

      bgndColor: __style.lightGreenColor

      onActiveColorChanged:{
        root.controller.activeColor = activeColor
      }
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
