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

import lc 1.0
import "../components" as MMComponents
import ".."

Page {
  id: root

  signal close()
  signal featureClicked( var featurePair )
  signal addFeatureClicked( var targetLayer )

  property var layerTreeNode: null

  LayerDetailData {
    id: layerDetailData
    layerTreeNode: root.layerTreeNode

    Component.onDestruction: {
      __layerDetailLegendImageProvider.reset()
    }
  }

  Keys.onReleased: function( event ) {
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
        color: InputStyle.clrPanelMain

        headerTitle: layerDetailData.name

        onBackClicked: root.closePage()
      }

      background: Rectangle {
        color: InputStyle.clrPanelMain
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
              text: qsTr( "Settings" )
              font.bold: true
              font.pixelSize: InputStyle.fontPixelSizeNormal
            }

            Rectangle {
              width: parent.width
              height: InputStyle.rowHeightMedium

              color: InputStyle.panelBackgroundLight
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
            Layout.preferredHeight: legend.height + symbologyTitle.height + 2 * InputStyle.panelMarginV2

            spacing: InputStyle.panelSpacing

            Text {
              id: symbologyTitle

              text: qsTr( "Legend" )
              font.bold: true
              font.pixelSize: InputStyle.fontPixelSizeNormal
            }

            Rectangle {
              id: legendItem

              width: parent.width
              height: parent.height - symbologyTitle.height

              color: InputStyle.panelBackgroundLight
              radius: InputStyle.cornerRadius

              Item {
                height: parent.height - 2 * InputStyle.panelMarginV2
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

          Column {
            id: layerAttribution

            visible: __inputUtils.layerAttribution(layerDetailData.mapLayer) != ""

            Layout.fillWidth: true
            Layout.preferredHeight: attributionText.height + attributionTitle.height + 2 * InputStyle.panelMarginV2

            spacing: InputStyle.panelSpacing

            Text {
              id: attributionTitle

              text: qsTr( "Attribution" )
              font.bold: true
              font.pixelSize: InputStyle.fontPixelSizeNormal
            }

            Rectangle {
              id: attributionItem

              width: parent.width
              height: parent.height - attributionTitle.height

              color: InputStyle.panelBackgroundLight
              radius: InputStyle.cornerRadius

              Flickable {
                id: flickableItem
                clip: true

                width: parent.width - InputStyle.panelMargin
                height: parent.height
                contentHeight: attributionText.height
                contentWidth: width
                maximumFlickVelocity: __androidUtils.isAndroid ? InputStyle.scrollVelocityAndroid : maximumFlickVelocity

                Text {
                  id: attributionText

                  width: parent.width
                  font.pixelSize: InputStyle.fontPixelSizeNormal
                  wrapMode: Text.WordWrap
                  text: __inputUtils.layerAttribution(layerDetailData.mapLayer)
                }

                ScrollBar.vertical: ScrollBar {}
              }
/*
              Text {
                id: attributionText

                width: parent.width
                textFormat: Text.RichText
                font.pixelSize: InputStyle.fontPixelSizeNormal
                wrapMode: Text.WordWrap
                text: __inputUtils.layerAttribution(layerDetailData.mapLayer)
              }
*/
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

      onFeatureClicked: function( featurePair ) {
        root.featureClicked( featurePair )
      }

      onAddFeatureClicked: function ( toLayer ) {
        root.addFeatureClicked( toLayer )
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

        property bool isSelected: content.currentIndex === 0

        Layout.fillWidth: true
        Layout.fillHeight: true

        visible: layerDetailData.isVectorLayer

        MMComponents.ToolbarButton {

          text: qsTr( "Features" )

          fontColor: featuresButtonParent.isSelected ? InputStyle.fontColorWhite : InputStyle.secondaryFontColor
          imageSource: featuresButtonParent.isSelected ? InputStyle.tableV2Icon : InputStyle.tableInactiveV2Icon

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

        property bool isSelected: {
          if ( featuresButtonParent.visible ) {
            if ( content.currentIndex === 0 ) {
              return false
            }
          }
          return true
        }

        Layout.fillWidth: true
        Layout.fillHeight: true

        visible: !layerDetailData.isVectorLayer || layerDetailData.isSpatial

        MMComponents.ToolbarButton {

          text: qsTr( "Layer info" )

          fontColor: layerInfoButtonParent.isSelected ? InputStyle.fontColorWhite : InputStyle.secondaryFontColor
          imageSource: layerInfoButtonParent.isSelected ? InputStyle.fileInfoIcon : InputStyle.fileInfoInactiveIcon

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
