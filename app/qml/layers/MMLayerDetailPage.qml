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

import "../components" as MMComponents
import "../inputs"

// Do not use MMPage here
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

    MMComponents.MMPage {
      id: layerDetailPage

      pageHeader.title: layerDetailData.name
      onBackClicked: root.closePage()

      pageContent: MMComponents.MMScrollView {
        id: scrollview

        width: parent.width
        height: parent.height

        Column {
          width: scrollview.width
          spacing: 0

          MMComponents.MMListSpacer { height: __style.spacing20 }

          // visibility
          MMSwitchInput {
            width: parent.width

            title: qsTr( "Settings" )
            text: qsTr( "Visible on map" )
            checked: layerDetailData.isVisible

            onToggled: {
              __activeProject.switchLayerTreeNodeVisibility( layerDetailData.layerTreeNode )
            }
          }

          MMComponents.MMListSpacer { height: __style.spacing20 }

          // legend
          Column {
            id: legendWrapper

            property real border: __style.margin12

            width: parent.width
            height: legend.height + symbologyTitle.height + 2 * legendWrapper.border + legendWrapper.spacing

            spacing: __style.margin4

            MMComponents.MMText {
              id: symbologyTitle

              text: qsTr( "Legend" )
              font: __style.p6
            }

            Rectangle {
              id: legendItem

              width: parent.width
              height: parent.height - symbologyTitle.height - legendWrapper.spacing

              color: __style.polarColor
              radius: __style.radius12

              Item {
                height: parent.height - 2 * legendWrapper.border
                width: parent.width - 2 * legendWrapper.border

                x: legendWrapper.border
                y: legendWrapper.border

                Image {
                  id: legend

                  sourceSize: Qt.size( parent.width, parent.height )

                  source: {
                    __layerDetailLegendImageProvider.setData( layerDetailData )
                    return "image://LayerDetailLegendImageProvider/0"
                  }

                  cache: false // In future - cache based on layer id
                }
              }
            }
          }

          MMComponents.MMListSpacer { height: __style.spacing20 }

          Column {
            id: layerAttribution

            width: parent.width
            height: 1.5 * __style.row63 + attributionTitle.height + 2 * layerAttribution.border

            visible: __inputUtils.layerAttribution( layerDetailData.mapLayer ) !== ""

            property real border: __style.margin12

            spacing: __style.margin4

            MMComponents.MMText {
              id: attributionTitle

              text: qsTr( "Attribution" )
              font: __style.p6
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

      onBackClicked: function() {
        root.closePage()
      }
    }
  }

  footer: MMComponents.MMToolbar {
    id: selectableToolbar

    visible: internal.withToolbar

    Component.onCompleted: {
      selectableToolbar.index = content.index ?? 0
    }

    model: ObjectModel {
      MMComponents.MMToolbarButton {
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

      MMComponents.MMToolbarButton {
        id: layerInfoButton

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
    //  - a: shown if the selected layer is vector layer (can have features) *
    //  - b: shown for all types of layers except for no-geometry vector layers,
    //       we do not have anything to show for such layers for now (we only
    //       show legend and if the layer is visible on the map - both unrelated)
    //
    // * there is an exception, we do not want to show features of the map sketches layer
    //

    const isSketchingLayer = layerDetailData.layerId === __activeProject.mapSketchesLayerId()

    if ( layerDetailData.isVectorLayer && !isSketchingLayer ) {
      content.addItem( featuresListPageComponent.createObject( content, { hasToolbar: internal.withToolbar } ) )
    }

    if ( !layerDetailData.isVectorLayer || layerDetailData.isSpatial )
    {
      content.addItem( layerDetailPageComponent.createObject( content ) )
    }
  }

  QtObject {
    id: internal

    property bool withToolbar: content.count > 1
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
