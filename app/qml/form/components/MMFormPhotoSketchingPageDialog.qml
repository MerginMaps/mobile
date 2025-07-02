/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import QtQuick.Shapes

import mm 1.0 as MM
import MMInput

import "../../components" as MMComponents
import "../../dialogs" as MMDialogs
import "./photo" as MMPhotoComponents

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

    MMComponents.MMButton {
      type: MMButton.Types.Primary
      text: qsTr( "Undo" )
      iconSourceLeft: __style.undoIcon
      size: MMButton.Sizes.Small
      bgndColor: __style.polarColor
      bgndColorDisabled: __style.polarColor
      bgndColorHover: __style.mediumGreenColor
      fontColorHover: __style.forestColor
      iconColorHover: __style.forestColor
      enabled: controller.canUndo
      Layout.alignment: Qt.AlignVCenter | Qt.AlignLeft
      Layout.topMargin: __style.pageMargins + __style.safeAreaTop
      Layout.leftMargin: __style.pageMargins + __style.safeAreaLeft

      onClicked: {
        controller.undo()
      }
    }

    MMComponents.MMRoundButton {
      id: closeButton

      Layout.alignment: Qt.AlignVCenter | Qt.AlignRight
      Layout.topMargin: __style.pageMargins + __style.safeAreaTop
      Layout.rightMargin: __style.pageMargins + __style.safeAreaRight

      iconSource: __style.closeIcon
      iconColor: __style.forestColor
      bgndColor: __style.polarColor

      onClicked: {
        root.close()
        controller.clear()
      }
    }
  }

  footer: RowLayout {
    MMComponents.MMButton {
      type: MMButton.Types.Primary
      text: qsTr( "Done" )
      bgndColor: __style.grassColor
      size: MMButton.Sizes.Small
      enabled: controller.canUndo
      Layout.fillWidth: true
      Layout.bottomMargin: __style.pageMargins + __style.safeAreaBottom
      Layout.leftMargin: __style.pageMargins + __style.safeAreaLeft
      Layout.rightMargin: __style.pageMargins + __style.safeAreaRight

      onClicked: {
        root.close()
      }
    }
  }

  contentItem: ColumnLayout {

    Item {
      id: container
      Layout.fillWidth: true
      Layout.fillHeight: true
      Layout.leftMargin: __style.pageMargins
      Layout.rightMargin: __style.pageMargins

      Shape {
        id: shape
        anchors.fill: parent
        property real photoPaddingWidth: ( width - photo.paintedWidth ) / 2
        property real photoPaddingHeight: ( height - photo.paintedHeight ) / 2
        property var shapePaths: []

        onPhotoPaddingWidthChanged: {
          if ( shape.data.findIndex(element => element.toString().includes("QQuickShapePath")) > -1 ) {
            controller.setAnnotationsOffset( photoPaddingWidth, photoPaddingHeight )
          }
        }

        onPhotoPaddingHeightChanged: {
          if ( shape.data.findIndex(element => element.toString().includes("QQuickShapePath")) > -1 ) {
            controller.setAnnotationsOffset( photoPaddingWidth, photoPaddingHeight )
          }
        }

        MMComponents.MMPhoto {
          id: photo

          anchors.fill: parent
          z: -1

          photoUrl: root.photoUrl

          fillMode: Image.PreserveAspectFit

          onPaintedWidthChanged: updateScaleRatio()
          onPaintedHeightChanged: updateScaleRatio()
          onStatusChanged: {
            if (status === Image.Ready) updateScaleRatio()
          }

          function updateScaleRatio() {
            if ( status === Image.Ready && sourceSize.width > 0 && sourceSize.height > 0 ) {
              controller.setPhotoScaleRatio( sourceSize.width / paintedWidth )
            }
          }

          Item {
            width: parent.paintedWidth
            height: parent.paintedHeight
            anchors.centerIn: parent

            DragHandler {
              id: dragHandler
              target: null

              onCentroidChanged: {
                // somehow the drag handler gets notified even when drag continues outside parent on some sides
                const outsideImageBounds = centroid.position.x > parent.width || centroid.position.y > parent.height
                if (!outsideImageBounds) {
                  // centroid gets set to (0, 0) when drag stops
                  if ( dragHandler.centroid.position === Qt.point( 0, 0 ) ) {
                    controller.newDrawing()
                  }
                  else {
                    controller.addPoint( dragHandler.centroid.position )
                  }
                }
              }
            }
          }
        }

        Component.onCompleted: {
          controller.redrawPaths()
        }

        Connections {
          target: controller

          function onPathUpdated( indexArray ) {
            for ( const index of indexArray ) {
              const modelData = controller.getPath(index)
              const stringArray = shape.data.toString().split(",")
              const firstIndex = stringArray.findIndex(element => element.toString().includes("QQuickShapePath"));
              const itemIndex =  firstIndex + index
              shape.data[itemIndex].modelData = modelData
            }
          }

          function onNewPathAdded( pathIndex ) {
            const modelData = controller.getPath( pathIndex );
            const newObject = shapePathComponent.createObject( shape, {
              modelData: modelData,
            });
            if ( newObject ) {
              shape.data.push( newObject )
              shape.shapePaths.push( newObject )
            }
          }

          function onLastPathRemoved() {
            // we don't remove the last ShapePath but the second last, as that is the last painted line on screen
            const stringArray = shape.data.toString().split(",")
            stringArray.reverse()
            let itemReverseIndex = stringArray.findIndex( element => element.toString().includes("QQuickShapePath") )
            stringArray.splice( itemReverseIndex, 1 )
            itemReverseIndex = stringArray.findIndex( element => element.toString().includes("QQuickShapePath") ) + 1
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
    }

    MMComponents.MMListSpacer { height: __style.margin20 }

    RowLayout {
      Layout.fillWidth: true
      Layout.bottomMargin: closeButton.implicitWidth
      Layout.leftMargin: __style.pageMargins + __style.safeAreaLeft
      Layout.rightMargin: __style.pageMargins + __style.safeAreaRight

      Repeater {
        // we use more vibrant versions of our product colors
        model: ["#FFFFFF", "#12181F", "#5E9EE4", "#57B46F", "#FDCB2A", "#FF9C40", "#FF8F93"]

        Rectangle {
          required property color modelData
          Layout.alignment: Qt.AlignVCenter | Qt.AlignHCenter
          Layout.fillWidth: true

          MMComponents.MMRoundButton {
            anchors.centerIn: parent

            contentItem: Rectangle {
              color: modelData
              radius: width / 2
              anchors.fill: parent
            }
            background: Rectangle {
              anchors.verticalCenter: parent.verticalCenter
              anchors.horizontalCenter: parent.horizontalCenter
              radius: width / 2
              width: __style.margin48
              height: __style.margin48
              color: modelData === controller.activeColor ? __style.transparentColor : __style.lightGreenColor
              border.width: 2
              border.color: modelData === controller.activeColor ? __style.grassColor : __style.transparentColor
            }

            onClicked: {
              controller.setActiveColor( modelData )
            }
          }
        }
      }
    }

  }

  Component {
    id: shapePathComponent

    ShapePath {
      required property var modelData

      strokeColor: modelData.color
      // if you are adjusting width here don't forget to adjust it also in PhotoSketchingController saveDrawings()
      strokeWidth: 4
      fillColor: __style.transparentColor
      startX: modelData.points[0]?.x ?? 0
      startY: modelData.points[0]?.y ?? 0

      PathPolyline {
        path: modelData.points
      }
    }
  }
}
