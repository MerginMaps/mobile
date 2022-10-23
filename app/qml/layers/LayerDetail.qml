/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

import QtQuick 2.14
import QtQuick.Controls 2.14
import QtQuick.Layouts 1.14

import lc 1.0
import "../components" as MMComponents
import ".."

Page {
  id: root

  signal close()

  property var layerTreeNode: null

  LayerDetailData {
    id: layerDetailData
    layerTreeNode: root.layerTreeNode

    Component.onDestruction: {
      __layerDetailLegendImageProvider.reset()
    }
  }

  Keys.onReleased: {
    if (event.key === Qt.Key_Back || event.key === Qt.Key_Escape) {
      event.accepted = true
      root.closePage()
    }
  }

  SwipeView {
    id: content

    anchors.fill: parent

    interactive: layerDetailData.isVectorLayer
  }

  Component {
    id: layerDetailPageComponent

    Page {
      id: layerDetailPage

      header: MMComponents.PanelHeaderV2 {
        width: parent.width
        color: InputStyle.panelBackgroundLight

        headerTitle: layerDetailData.name

        onBackClicked: root.closePage()
      }

      background: Rectangle {
        color: InputStyle.panelBackgroundLight
      }

      ScrollView {
        id: scrollview

        anchors {
          left: parent.left
          leftMargin: InputStyle.panelMargin
          right: parent.right
          rightMargin: InputStyle.panelMargin
          top: parent.top
          topMargin: InputStyle.panelMarginV2
          bottom: parent.bottom
        }

        contentWidth: availableWidth // only scroll vertically

        ColumnLayout {

          width: scrollview.width
          spacing: InputStyle.bigGap

          // visibility
          Column {
            Layout.fillWidth: true
            Layout.preferredHeight: InputStyle.rowHeightListEntry

            spacing: InputStyle.panelSpacing

            Text {
              text: qsTr( "Customisation" )
              font.bold: true
              font.pixelSize: InputStyle.fontPixelSizeNormal
            }

            Rectangle {
              width: parent.width
              height: InputStyle.rowHeightMedium

              color: InputStyle.clrPanelMain
              radius: InputStyle.cornerRadius

              RowLayout {
                id: visibleSwitchContent

                anchors {
                  fill: parent
                  leftMargin: InputStyle.panelMarginV2
                  rightMargin: InputStyle.panelMarginV2
                }

                Image {
                  Layout.preferredWidth: InputStyle.iconSizeMedium
                  Layout.preferredHeight: InputStyle.iconSizeMedium

                  source: layerDetailData.isVisible ? InputStyle.eyeIconV2 : InputStyle.eyeSlashIconV2

                }

                Text {
                  Layout.fillWidth: true

                  text: qsTr( "Visible on map" )

                  elide: Text.ElideMiddle
                  font.pixelSize: InputStyle.fontPixelSizeNormal
                }

                MMComponents.Switch {
                  id: visibleSwitch

                  Layout.preferredWidth: InputStyle.switchWidth
                  Layout.preferredHeight: InputStyle.switchHeight

                  onReleased: function() {
                    __activeProject.switchLayerTreeNodeVisibility( layerDetailData.layerTreeNode )
                  }

                  checked: layerDetailData.isVisible
                }
              }

              MouseArea {
                anchors.fill: parent
                onClicked: visibleSwitch.released()
              }
            }
          }

          // legend
          Column {
            id: legendWrapper

            Layout.fillWidth: true
            Layout.preferredHeight: legend.height + symbologyTitle.height

            spacing: InputStyle.panelSpacing

            Text {
              id: symbologyTitle

              text: qsTr( "Symbology" )
              font.bold: true
              font.pixelSize: InputStyle.fontPixelSizeNormal
            }

            Rectangle {
              id: legendItem

              width: parent.width
              height: legend.height + 2 * InputStyle.panelMarginV2

              color: InputStyle.clrPanelMain
              radius: InputStyle.cornerRadius

              Item {
                height: childrenRect.height
                width: parent.width - 2 * InputStyle.panelMarginV2

                x: legendItem.x + InputStyle.panelMarginV2
                y: legendItem.y - InputStyle.panelMarginV2

                Image {
                  id: legend

                  sourceSize: Qt.size( parent.width, parent.height)

                  source: {
                    __layerDetailLegendImageProvider.setData( layerDetailData )
                    return "image://LayerDetailLegendImageProvider/0"
                  }

                  cache: false // In future - cache based on layer id
                }
              }
            }
          }
        }
      }
    }
  }

  Component {
    id: featuresListPageComponent

    FeaturesListPageV2 {
      selectedLayer: layerDetailData.vectorLayer

      onFeatureClicked: function( featureId ) {

      }

      onAddFeatureClicked: function ( toLayer ) {

      }

      onClose: function() {
        root.closePage()
      }
    }
  }

  footer: Rectangle {

    height: InputStyle.toolbarHeight
    color: InputStyle.clrPanelBackground

    RowLayout {
      anchors.fill: parent

      Item {
        id: featuresButtonParent

        Layout.fillWidth: true
        Layout.fillHeight: true

        visible: layerDetailData.isVectorLayer

        MMComponents.ToolbarButton {

          text: qsTr( "Features" )
          imageSource: InputStyle.tableV2Icon

          anchors.centerIn: parent

          onClicked: {
            if ( layerDetailData.isVectorLayer ) {
              content.setCurrentIndex( 0 )
            }
          }
        }
      }

      Item {
        id: layerInfoButtonParent

        Layout.fillWidth: true
        Layout.fillHeight: true

        visible: !layerDetailData.isVectorLayer || layerDetailData.isSpatial

        MMComponents.ToolbarButton {

          text: qsTr( "Layer info" )
          imageSource: InputStyle.fileInfoIcon

          anchors.centerIn: parent

          onClicked: {
            if ( layerDetailData.isVectorLayer ) {
              content.setCurrentIndex( 1 )
            }
          }
        }
      }
    }
  }

  Component.onCompleted: {

    //
    // LayerDetail can show (a) features list and (b) layer info
    //  - a: shown if the selected layer is vector layer (can have features)
    //  - b: shown for all types of layers except for no-geometry vector layers,
    //       we do not have anything to show for such layers for now (we only
    //       show legend and if the layer is visible on the map - both unrelated)
    //

    if ( layerDetailData.isVectorLayer ) {
      content.addItem( featuresListPageComponent.createObject( content ) )
    }

    if ( !layerDetailData.isVectorLayer || layerDetailData.isSpatial )
    {
      content.addItem( layerDetailPageComponent.createObject( content ) )
    }
  }

  function closePage() {

    //
    // LayerDetail is instantiated via StackView in layers panels. It uses its transitions (animations)
    // for creating (moving to the right) and destroying this page (moving back to the left).
    // However, as this page contains a SwipeView, moving back to the left would reveal what if on the
    // first item in the SwipeView. Thus, we remove the first item so that there is no visual noise.
    // It is a kind of a hack.
    //

    if ( content.count === 2 )
    {
      if ( content.currentIndex === 1 )
      {
        content.takeItem( 0 )
      }
    }

    root.close()
  }
}
