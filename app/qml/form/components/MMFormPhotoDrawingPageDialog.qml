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

  parent: Overlay.overlay
  width: parent.width
  height: parent.height
  anchors.centerIn: parent

  function save() {
    console.log( "Changes saved" )
  }

  function draw(){
    // centroid gets set to (0, 0) when drag stops
    if ( dragHandler.centroid.position === Qt.point( 0, 0 ) ) {
      annotationsController.newDrawing()
    }
    else {
      annotationsController.addPoint( dragHandler.centroid.position, shape.photoPaddingWidth, shape.photoPaddingHeight )
    }
  }

  background: Rectangle {
    color: __style.lightGreenColor
  }

  header: RowLayout {

    MMComponents.MMButton {
      type: MMButton.Types.Tertiary
      text: qsTr( "Undo" )
      iconSourceLeft: __style.undoIcon
      size: MMButton.Sizes.Small
      bgndColor: __style.polarColor
      bgndColorDisabled: __style.polarColor
      enabled: annotationsController.canUndo
      Layout.topMargin: __style.pageMargins + __style.safeAreaTop
      Layout.leftMargin: __style.pageMargins + __style.safeAreaLeft

      onClicked: {
        annotationsController.undo()
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
        closeDialog.open()
      }
    }
  }

  footer: RowLayout {
    MMComponents.MMButton {
      type: MMButton.Types.Primary
      text: qsTr( "Save Changes" )
      bgndColor: __style.grassColor
      size: MMButton.Sizes.Small
      Layout.fillWidth: true
      Layout.bottomMargin: __style.pageMargins + __style.safeAreaBottom
      Layout.leftMargin: __style.pageMargins + __style.safeAreaLeft
      Layout.rightMargin: __style.pageMargins + __style.safeAreaRight

      onClicked: {
        root.save()
      }
    }
  }

  contentItem: ColumnLayout {

    Item {
      id: container
      Layout.fillWidth: true
      Layout.fillHeight: true

      Shape {
        id: shape
        anchors.centerIn: parent
        anchors.fill: parent
        property real photoPaddingWidth: ( width - photo.paintedWidth ) / 2
        property real photoPaddingHeight: ( height - photo.paintedHeight ) / 2
        property var shapePaths: []

        MMComponents.MMPhoto {
          id: photo

          anchors.fill: parent
          z: -1

          photoUrl: root.photoUrl
          isLocalFile: root.photoUrl.startsWith("file://")

          fillMode: Image.PreserveAspectFit

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
                  root.draw()
                }
              }
            }
          }
        }

        Connections {
          target: annotationsController.annotations

          function onRowsInserted( _, __, last) {
            console.log( "Shape children count before: " + shape.data.length)
            console.log( "Shape children before: " + shape.data )
            const modelData = annotationsController.annotations.getPath( last );
            const newObject = shapePathComponent.createObject( shape, {
              modelData: modelData,
            });
            if ( newObject ) {
              shape.data.push( newObject )
              shape.shapePaths.push( newObject )
              console.log("ShapePath successfully created")
              console.log("Current ShapePath offset - X: " + shape.photoPaddingWidth + ", Y: " + shape.photoPaddingHeight )
            }
            console.log( "Shape children count after: " + shape.data.length)
            console.log( "Shape children after: " + shape.data )
          }

          function onRowsRemoved( _, __, ___) {
            console.log( "Shape children count before removal: " + shape.data.length)
            console.log( "Shape children before removal: " + shape.data )
            const stringArray = shape.data.toString().split(",")
            stringArray.reverse()
            console.log( "String array: " + stringArray )
            const itemReverseIndex = stringArray.findIndex( element => element.toString().includes("QQuickShapePath") );
            const itemIndex = shape.data.length - itemReverseIndex - 1
            console.log( "Shape path to remove: " + shape.data[itemIndex] )
            const removedItem = shape.data.splice( itemIndex, 1, ...shape.data.slice( itemIndex + 1 ) )
            console.log("Actual removed item: " + removedItem )
            console.log( "ShapePaths: " + shape.shapePaths)
            const shapePathsIndex = shape.shapePaths.findIndex( element => element.toString() === removedItem.toString() )
            console.log("ShapePaths index: " + shapePathsIndex )
            console.log("Actual shapePaths item: " + shape.shapePaths[shapePathsIndex] )
            shape.shapePaths[shapePathsIndex].destroy()
            shape.shapePaths.splice( shapePathsIndex, 1 )
            console.log( "Shape children count after removal: " + shape.data.length)
            console.log( "Shape children after removal: " + shape.data )
            // const item = shape.data.pop();
            // const stringArray = shape.data.toString().split(",")
            // const itemIndex = stringArray.findLastIndex( element => element.toString().includes("QQuickShapePath") );
            // if ( itemIndex > -1 ) {
            //   // console.log( "Modified data array: " + shape.data.toSpliced( itemIndex, 1 ) )
            //   console.log("Removed item: " + shape.data[itemIndex] )
            //   const removedItem = shape.data.splice( itemIndex, 1 )
            //   console.log("Actual removed item: " + removedItem )
            //   removedItem[0].destroy()
            //   console.log( "Shape children count after removal: " + shape.data.length)
            //   console.log( "Shape children after removal: " + shape.data )
            // }
          }

          function onModelReset() {
            // we delete all the paths in Shape (skipping the first child which should be the photo)
            // while (shape.data.length > 4) {
            //   const shapePath = shape.data.pop()
            //   shapePath.destroy()
            // }
            console.log( "Shape children count before clearing: " + shape.data.length)
            console.log( "Shape children before clearing: " + shape.data )
            // const stringArray = shape.data.toString().split(",")
            // stringArray.reverse()
            // console.log( "String array: " + stringArray )
            while ( shape.data.findIndex( element => element.toString().includes("QQuickShapePath") ) > -1 )
            {
              const itemIndex = shape.data.findIndex( element => element.toString().includes("QQuickShapePath") )
              console.log("Shape path to remove: " + shape.data[itemIndex])
              const removedItem = shape.data.splice(itemIndex, 1, ...shape.data.slice(itemIndex + 1))
              console.log("Actual removed item: " + removedItem)
              console.log( "ShapePaths: " + shape.shapePaths)
              const shapePathsIndex = shape.shapePaths.findIndex( element => element.toString() === removedItem.toString() )
              console.log("ShapePaths index: " + shapePathsIndex )
              console.log("Actual shapePaths item: " + shape.shapePaths[shapePathsIndex] )
              shape.shapePaths[shapePathsIndex].destroy()
              shape.shapePaths.splice( shapePathsIndex, 1 )
            }
            console.log( "Shape children count after removal: " + shape.data.length)
            console.log( "Shape children after removal: " + shape.data )
          }
        }
      }
    }

    MMComponents.MMListSpacer { height: __style.margin20 }

    RowLayout {
      Layout.fillWidth: true
      Layout.bottomMargin: closeButton.implicitWidth

      Repeater {
        // we use more vibrant versions of our product colors
        model: ["#FFFFFF", "#12181F", "#5E9EE4", "#57B46F", "#FDCB2A", "#FF9C40", "#FF8F93"]

        Rectangle {
          required property color modelData
          Layout.alignment: Qt.AlignVCenter | Qt.AlignHCenter
          Layout.fillWidth: true

          MMComponents.MMRoundButton {
            contentItem: Rectangle {
              color: modelData
              radius: __style.radius20
            }
            background: Rectangle {
              radius: implicitWidth / 2
              implicitWidth: parent.implicitWidth + 5
              implicitHeight: parent.implicitHeight + 5
              color: __style.transparentColor
              border.width: 2
              border.color: modelData === annotationsController.activeColor ? __style.grassColor : __style.transparentColor
            }

            onClicked: {
              annotationsController.setActiveColor( modelData )
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
      strokeWidth: 2
      fillColor: __style.transparentColor
      startX: shape.photoPaddingWidth + modelData.points[0].x
      startY: shape.photoPaddingHeight + modelData.points[0].y

      PathPolyline {
        path: modelData.points
      }

      Component.onCompleted: {
        console.log("Created ShapePath with", modelData.points.length, "points and color", modelData.color)
      }

    }
  }

  MM.PhotoDrawingController {
    id: annotationsController

    photoSource: root.photoUrl
  }

  MMComponents.MMDrawerDialog {
    id: closeDialog

    imageSource: __style.neutralMMSymbolImage
    title: qsTr( "Do you wish to exit drawing?" )
    description: qsTr( "Your sketches will be lost." )
    primaryButton.text: qsTr( "No" )
    primaryButton.type: MMComponents.MMButton.Types.Secondary
    secondaryButton.type: MMComponents.MMButton.Types.Primary
    secondaryButton.text: qsTr( "Yes" )
    secondaryButton.fontColor: __style.nightColor
    secondaryButton.fontColorHover: __style.nightColor
    secondaryButton.bgndColor: __style.negativeColor
    secondaryButton.bgndColorHover: __style.negativeColor
    horizontalbuttons: true

    onPrimaryButtonClicked: {
      close()
    }

    onSecondaryButtonClicked: {
      close()
      root.close()
      annotationsController.clear()
    }
  }
}
