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

import mm 1.0 as MM

import "../components"
import "../inputs"

Page {
  id: root

  signal close()
  signal featureClicked( var featurePair )
  signal addFeatureClicked( var targetLayer )

  property var layerTreeNode: null

  MM.LayerDetailData {
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

      header: MMPageHeader {
        width: parent.width
        color: __style.lightGreenColor

        title: layerDetailData.name

        onBackClicked: root.closePage()
      }

      background: Rectangle {
        color: __style.lightGreenColor
      }

      ScrollView {
        id: scrollview

        anchors {
          left: parent.left
          leftMargin: __style.pageMargins
          right: parent.right
          rightMargin: __style.pageMargins
          top: parent.top
          topMargin: __style.margin40
          bottom: parent.bottom
        }

        contentWidth: availableWidth // only scroll vertically

        ColumnLayout {

          width: scrollview.width
          spacing: __style.margin20

          // visibility
          Column {
            Layout.fillWidth: true
            Layout.preferredHeight: __style.row63

            spacing: __style.margin12

            Text {
              text: qsTr( "Settings" )
              font: __style.p6
              color: __style.nightColor
            }

            Rectangle {
              width: parent.width
              height: __style.row49

              color: __style.polarColor
              radius: __style.radius12

              RowLayout {
                id: visibleSwitchContent

                anchors {
                  fill: parent
                  leftMargin: __style.margin4
                  rightMargin: __style.margin4
                }

                Text {
                  Layout.fillWidth: true

                  text: qsTr( "Visible on map" )

                  elide: Text.ElideMiddle
                  font: __style.p5
                  color: __style.nightColor
                }

                MMSwitch {
                  id: visibleSwitch

                  uncheckedBgColor: __style.lightGreenColor

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

            property real border: __style.margin12

            Layout.fillWidth: true
            Layout.preferredHeight: legend.height + symbologyTitle.height + 2 * legendWrapper.border

            spacing: __style.margin8

            Text {
              id: symbologyTitle

              text: qsTr( "Legend" )
              font: __style.p6
              color: __style.nightColor
            }

            Rectangle {
              id: legendItem

              width: parent.width
              height: parent.height - symbologyTitle.height

              color: __style.polarColor
              radius: __style.radius12

              Item {
                height: parent.height - 2 * legendWrapper.border
                width: parent.width - 2 * legendWrapper.border

                x: legendItem.x + legendWrapper.border
                y: legendItem.y - legendWrapper.border

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

            visible: __inputUtils.layerAttribution(layerDetailData.mapLayer) !== ""

            property real border: __style.margin12

            Layout.fillWidth: true
            Layout.preferredHeight: 1.5 * __style.row63 + attributionTitle.height + 2 * layerAttribution.border

            spacing: __style.margin8

            Text {
              id: attributionTitle

              text: qsTr( "Attribution" )
              font: __style.p6
              color: __style.nightColor
            }

            Rectangle {
              id: attributionItem

              width: parent.width
              height: 1.5 * __style.row63

              color: __style.polarColor
              radius: __style.radius12

              Item {
                height: parent.height - 2 * layerAttribution.border
                width: parent.width - 2 * layerAttribution.border

                x: legendItem.x + layerAttribution.border
                y: legendItem.y - layerAttribution.border


                Flickable {
                  id: flickableItem
                  clip: true

                  width: parent.width
                  height: parent.height
                  contentHeight: attributionText.height
                  contentWidth: width
                  maximumFlickVelocity: __androidUtils.isAndroid ? __style.scrollVelocityAndroid : maximumFlickVelocity

                  Text {
                    id: attributionText

                    width: parent.width
                    font: __style.p5
                    color: __style.nightColor
                    wrapMode: Text.WordWrap
                    text: __inputUtils.layerAttribution(layerDetailData.mapLayer)
                  }

                  ScrollBar.vertical: ScrollBar {}
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

    MMFeaturesListPage {
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

  footer:  MMToolbar {
    id: selectableToolbar

    visible: featureButton.visible && layerInfoButton.visible

    Component.onCompleted: {
      selectableToolbar.index = content.index ?? 0
    }

    model: ObjectModel {
      MMToolbarButton {
        id: featureButton

        visible: layerDetailData.isVectorLayer

        text: qsTr( "Features" )
        iconSource: __style.featuresIcon
        iconSourceSelected: __style.featuresFilledIcon
        onClicked: {
          if ( layerDetailData.isVectorLayer ) {
            selectableToolbar.index = 0
            content.setCurrentIndex( 0 )
          }
        }
      }
      MMToolbarButton {
        id: layerInfoButton

        visible: !layerDetailData.isVectorLayer || layerDetailData.isSpatial

        text: qsTr( "Layer info" )
        iconSource: __style.infoIcon
        iconSourceSelected: __style.infoFilledIcon
        onClicked: {
          if ( layerDetailData.isVectorLayer ) {
            selectableToolbar.index = 1
            content.setCurrentIndex( 1 )
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
