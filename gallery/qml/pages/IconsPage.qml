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
import QtQuick.Controls.Basic

import "../../app/qml/components"
import "../components" as GalleryComponents

ScrollView {

  Column {
    padding: 20

    MMCheckBox { id: checkboxBorder; text: "Show border" }
    Item { width: 1; height: 10 }
    MMCheckBox { id: checkboxColor; text: "Invert colors" }
    Item { width: 1; height: 10 }

    Row {
      Column {
        spacing: 5

        GroupBox {
          title: "MMIcon"
          background: Rectangle {
            color: "white"
            border.color: "gray"
          }
          label: Text {
            color: "black"
            text: parent.title
            padding: 5
          }

          Column {
            spacing: 5
            Row {
              anchors.horizontalCenter: parent.horizontalCenter
              Repeater {
                model: ["16px", "24px", "32px"]
                delegate: Text {
                  width: 50 * __dp
                  height: 20
                  text: modelData
                  font.bold: true
                }
              }
            }
            GalleryComponents.IconLine { name: "linkIcon"; source: __style.linkIcon; showRect: checkboxBorder.checked; invertColors: checkboxColor.checked }
            GalleryComponents.IconLine { name: "splitGeometryIcon"; source: __style.splitGeometryIcon; showRect: checkboxBorder.checked; invertColors: checkboxColor.checked }
            GalleryComponents.IconLine { name: "streamingIcon"; source: __style.streamingIcon; showRect: checkboxBorder.checked; invertColors: checkboxColor.checked }
            GalleryComponents.IconLine { name: "workspacesIcon"; source: __style.workspacesIcon; showRect: checkboxBorder.checked; invertColors: checkboxColor.checked }
            GalleryComponents.IconLine { name: "settingsIcon"; source: __style.settingsIcon; showRect: checkboxBorder.checked; invertColors: checkboxColor.checked }
            GalleryComponents.IconLine { name: "addImageIcon"; source: __style.addImageIcon; showRect: checkboxBorder.checked; invertColors: checkboxColor.checked }
            GalleryComponents.IconLine { name: "calendarIcon"; source: __style.calendarIcon; showRect: checkboxBorder.checked; invertColors: checkboxColor.checked }
            GalleryComponents.IconLine { name: "checkmarkIcon"; source: __style.checkmarkIcon; showRect: checkboxBorder.checked; invertColors: checkboxColor.checked }
            GalleryComponents.IconLine { name: "deleteIcon"; source: __style.deleteIcon; showRect: checkboxBorder.checked; invertColors: checkboxColor.checked }
            GalleryComponents.IconLine { name: "editIcon"; source: __style.editIcon; showRect: checkboxBorder.checked; invertColors: checkboxColor.checked }
            GalleryComponents.IconLine { name: "redrawGeometryIcon"; source: __style.redrawGeometryIcon; showRect: checkboxBorder.checked; invertColors: checkboxColor.checked }
            GalleryComponents.IconLine { name: "globeIcon"; source: __style.globeIcon; showRect: checkboxBorder.checked; invertColors: checkboxColor.checked }
            GalleryComponents.IconLine { name: "gpsAntennaHeightIcon"; source: __style.gpsAntennaHeightIcon; showRect: checkboxBorder.checked; invertColors: checkboxColor.checked }
            GalleryComponents.IconLine { name: "gpsIcon"; source: __style.gpsIcon; showRect: checkboxBorder.checked; invertColors: checkboxColor.checked }
            GalleryComponents.IconLine { name: "hideIcon"; source: __style.hideIcon; showRect: checkboxBorder.checked; invertColors: checkboxColor.checked }
            GalleryComponents.IconLine { name: "layersIcon"; source: __style.layersIcon; showRect: checkboxBorder.checked; invertColors: checkboxColor.checked }
            GalleryComponents.IconLine { name: "minusIcon"; source: __style.minusIcon; showRect: checkboxBorder.checked; invertColors: checkboxColor.checked }
            GalleryComponents.IconLine { name: "moreIcon"; source: __style.moreIcon; showRect: checkboxBorder.checked; invertColors: checkboxColor.checked }
            GalleryComponents.IconLine { name: "moreVerticalIcon"; source: __style.moreVerticalIcon; showRect: checkboxBorder.checked; invertColors: checkboxColor.checked }
            GalleryComponents.IconLine { name: "morePhotosIcon"; source: __style.morePhotosIcon; showRect: checkboxBorder.checked; invertColors: checkboxColor.checked }
            GalleryComponents.IconLine { name: "plusIcon"; source: __style.plusIcon; showRect: checkboxBorder.checked; invertColors: checkboxColor.checked }
            GalleryComponents.IconLine { name: "positionTrackingIcon"; source: __style.positionTrackingIcon; showRect: checkboxBorder.checked; invertColors: checkboxColor.checked }
            GalleryComponents.IconLine { name: "measurementToolIcon"; source: __style.measurementToolIcon; showRect: checkboxBorder.checked; invertColors: checkboxColor.checked }
            GalleryComponents.IconLine { name: "closeShapeIcon"; source: __style.closeShapeIcon; showRect: checkboxBorder.checked; invertColors: checkboxColor.checked }
            GalleryComponents.IconLine { name: "qrCodeIcon"; source: __style.qrCodeIcon; showRect: checkboxBorder.checked; invertColors: checkboxColor.checked }
            GalleryComponents.IconLine { name: "satelliteIcon"; source: __style.satelliteIcon; showRect: checkboxBorder.checked; invertColors: checkboxColor.checked }
            GalleryComponents.IconLine { name: "searchIcon"; source: __style.searchIcon; showRect: checkboxBorder.checked; invertColors: checkboxColor.checked }
            GalleryComponents.IconLine { name: "showIcon"; source: __style.showIcon; showRect: checkboxBorder.checked; invertColors: checkboxColor.checked }
            GalleryComponents.IconLine { name: "stopIcon"; source: __style.stopIcon; showRect: checkboxBorder.checked; invertColors: checkboxColor.checked }
            GalleryComponents.IconLine { name: "syncIcon"; source: __style.syncIcon; showRect: checkboxBorder.checked; invertColors: checkboxColor.checked }
            GalleryComponents.IconLine { name: "waitingIcon"; source: __style.waitingIcon; showRect: checkboxBorder.checked; invertColors: checkboxColor.checked }
            GalleryComponents.IconLine { name: "closeAccountIcon"; source: __style.closeAccountIcon; showRect: checkboxBorder.checked; invertColors: checkboxColor.checked }
            GalleryComponents.IconLine { name: "closeIcon"; source: __style.closeIcon; showRect: checkboxBorder.checked; invertColors: checkboxColor.checked }
            GalleryComponents.IconLine { name: "signOutIcon"; source: __style.signOutIcon; showRect: checkboxBorder.checked; invertColors: checkboxColor.checked }
            GalleryComponents.IconLine { name: "downloadIcon"; source: __style.downloadIcon; showRect: checkboxBorder.checked; invertColors: checkboxColor.checked }
            GalleryComponents.IconLine { name: "uploadIcon"; source: __style.uploadIcon; showRect: checkboxBorder.checked; invertColors: checkboxColor.checked }
            GalleryComponents.IconLine { name: "undoIcon"; source: __style.undoIcon; showRect: checkboxBorder.checked; invertColors: checkboxColor.checked }
            GalleryComponents.IconLine { name: "backIcon"; source: __style.backIcon; showRect: checkboxBorder.checked; invertColors: checkboxColor.checked }
            GalleryComponents.IconLine { name: "nextIcon"; source: __style.nextIcon; showRect: checkboxBorder.checked; invertColors: checkboxColor.checked }
            GalleryComponents.IconLine { name: "arrowDownIcon"; source: __style.arrowDownIcon; showRect: checkboxBorder.checked; invertColors: checkboxColor.checked }
            GalleryComponents.IconLine { name: "arrowLinkRightIcon"; source: __style.arrowLinkRightIcon; showRect: checkboxBorder.checked; invertColors: checkboxColor.checked }
            GalleryComponents.IconLine { name: "arrowUpIcon"; source: __style.arrowUpIcon; showRect: checkboxBorder.checked; invertColors: checkboxColor.checked }
            GalleryComponents.IconLine { name: "stakeOutIcon"; source: __style.stakeOutIcon; showRect: checkboxBorder.checked; invertColors: checkboxColor.checked }
          }
        }
      }

      Column {
        spacing: 5

        GroupBox {
          title: "MMIcon - toolbar"
          background: Rectangle {
            color: "white"
            border.color: "gray"
          }
          label: Text {
            color: "black"
            text: parent.title
            padding: 5
          }

          Column {
            spacing: 5
            Row {
              anchors.horizontalCenter: parent.horizontalCenter
              Repeater {
                model: ["16px", "24px", "32px"]
                delegate: Text {
                  width: 50 * __dp
                  height: 20
                  text: modelData
                  font.bold: true
                }
              }
            }
            GalleryComponents.IconLine { name: "globalIcon"; source: __style.globalIcon; showRect: checkboxBorder.checked; invertColors: checkboxColor.checked }
            GalleryComponents.IconLine { name: "globalFilledIcon"; source: __style.globalFilledIcon; showRect: checkboxBorder.checked; invertColors: checkboxColor.checked }
            GalleryComponents.IconLine { name: "homeIcon"; source: __style.homeIcon; showRect: checkboxBorder.checked; invertColors: checkboxColor.checked }
            GalleryComponents.IconLine { name: "homeFilledIcon"; source: __style.homeFilledIcon; showRect: checkboxBorder.checked; invertColors: checkboxColor.checked }
            GalleryComponents.IconLine { name: "projectsIcon"; source: __style.projectsIcon; showRect: checkboxBorder.checked; invertColors: checkboxColor.checked }
            GalleryComponents.IconLine { name: "projectsFilledIcon"; source: __style.projectsFilledIcon; showRect: checkboxBorder.checked; invertColors: checkboxColor.checked }
            GalleryComponents.IconLine { name: "featuresIcon"; source: __style.featuresIcon; showRect: checkboxBorder.checked; invertColors: checkboxColor.checked }
            GalleryComponents.IconLine { name: "featuresFilledIcon"; source: __style.featuresFilledIcon; showRect: checkboxBorder.checked; invertColors: checkboxColor.checked }
            GalleryComponents.IconLine { name: "infoIcon"; source: __style.infoIcon; showRect: checkboxBorder.checked; invertColors: checkboxColor.checked }
            GalleryComponents.IconLine { name: "infoFilledIcon"; source: __style.infoFilledIcon; showRect: checkboxBorder.checked; invertColors: checkboxColor.checked }
          }
        }
      }

      Column {
        spacing: 5

        GroupBox {
          title: "MMIcon - With Circle"
          background: Rectangle {
            color: "white"
            border.color: "gray"
          }
          label: Text {
            color: "black"
            text: parent.title
            padding: 5
          }

          Column {
            spacing: 5
            Row {
              anchors.horizontalCenter: parent.horizontalCenter
              Repeater {
                model: ["16px", "24px", "32px"]
                delegate: Text {
                  width: 50 * __dp
                  height: 20
                  text: modelData
                  font.bold: true
                }
              }
            }
            GalleryComponents.IconLine { name: "xMarkCircleIcon"; source: __style.xMarkCircleIcon; showRect: checkboxBorder.checked; invertColors: checkboxColor.checked; isCircle: true }
            GalleryComponents.IconLine { name: "doneCircleIcon"; source: __style.doneCircleIcon; showRect: checkboxBorder.checked; invertColors: checkboxColor.checked; isCircle: true }
            GalleryComponents.IconLine { name: "errorCircleIcon"; source: __style.errorCircleIcon; showRect: checkboxBorder.checked; invertColors: checkboxColor.checked; isCircle: true }
            GalleryComponents.IconLine { name: "warningCircleIcon"; source: __style.warningCircleIcon; showRect: checkboxBorder.checked; invertColors: checkboxColor.checked; isCircle: true }
            GalleryComponents.IconLine { name: "editCircleIcon"; source: __style.editCircleIcon; showRect: checkboxBorder.checked; invertColors: checkboxColor.checked; isCircle: true }
            }
        }
      }

      Column {
        spacing: 5

        GroupBox {
          title: "MMIcon - Social and how you heard about us"
          background: Rectangle {
            color: "white"
            border.color: "gray"
          }
          label: Text {
            color: "black"
            text: parent.title
            padding: 5
          }

          Column {
            spacing: 5
            Row {
              anchors.horizontalCenter: parent.horizontalCenter
              Repeater {
                model: ["16px", "24px", "32px"]
                delegate: Text {
                  width: 50 * __dp
                  height: 20
                  text: modelData
                  font.bold: true
                }
              }
            }
            GalleryComponents.IconLine { name: "termsIcon"; source: __style.termsIcon; showRect: checkboxBorder.checked; invertColors: checkboxColor.checked }
            GalleryComponents.IconLine { name: "mouthIcon"; source: __style.mouthIcon; showRect: checkboxBorder.checked; invertColors: checkboxColor.checked }
            GalleryComponents.IconLine { name: "qgisIcon"; source: __style.qgisIcon; showRect: checkboxBorder.checked; invertColors: checkboxColor.checked }
            GalleryComponents.IconLine { name: "subscriptionsIcon"; source: __style.subscriptionsIcon; showRect: checkboxBorder.checked; invertColors: checkboxColor.checked }
            GalleryComponents.IconLine { name: "teacherIcon"; source: __style.teacherIcon; showRect: checkboxBorder.checked; invertColors: checkboxColor.checked }
            GalleryComponents.IconLine { name: "briefcaseIcon"; source: __style.briefcaseIcon; showRect: checkboxBorder.checked; invertColors: checkboxColor.checked }
            GalleryComponents.IconLine { name: "socialMediaIcon"; source: __style.socialMediaIcon; showRect: checkboxBorder.checked; invertColors: checkboxColor.checked }
            GalleryComponents.IconLine { name: "otherIcon"; source: __style.otherIcon; showRect: checkboxBorder.checked; invertColors: checkboxColor.checked }
            GalleryComponents.IconLine { name: "youtubeIcon"; source: __style.youtubeIcon; showRect: checkboxBorder.checked; invertColors: checkboxColor.checked }
            GalleryComponents.IconLine { name: "xTwitterIcon"; source: __style.xTwitterIcon; showRect: checkboxBorder.checked; invertColors: checkboxColor.checked }
            GalleryComponents.IconLine { name: "facebookIcon"; source: __style.facebookIcon; showRect: checkboxBorder.checked; invertColors: checkboxColor.checked }
            GalleryComponents.IconLine { name: "linkedinIcon"; source: __style.linkedinIcon; showRect: checkboxBorder.checked; invertColors: checkboxColor.checked }
            GalleryComponents.IconLine { name: "mastodonIcon"; source: __style.mastodonIcon; showRect: checkboxBorder.checked; invertColors: checkboxColor.checked }
            GalleryComponents.IconLine { name: "redditIcon"; source: __style.redditIcon; showRect: checkboxBorder.checked; invertColors: checkboxColor.checked }
          }
        }
      }

      Column {
        spacing: 5

        GroupBox {
          title: "MMIcon - do not color overlay"
          background: Rectangle {
            color: "white"
            border.color: "gray"
          }
          label: Text {
            color: "black"
            text: parent.title
            padding: 5
          }

          Column {
            spacing: 5
            Row {
              anchors.horizontalCenter: parent.horizontalCenter
              Repeater {
                model: ["16px", "24px", "32px"]
                delegate: Text {
                  width: 50 * __dp
                  height: 20
                  text: modelData
                  font.bold: true
                }
              }
            }
            GalleryComponents.IconLine { name: "lineLayerNoColorOverlayIcon"; source: __style.lineLayerNoColorOverlayIcon; showRect: checkboxBorder.checked; invertColors: checkboxColor.checked }
            GalleryComponents.IconLine { name: "pointLayerNoColorOverlayIcon"; source: __style.pointLayerNoColorOverlayIcon; showRect: checkboxBorder.checked; invertColors: checkboxColor.checked }
            GalleryComponents.IconLine { name: "polygonLayerNoColorOverlayIcon"; source: __style.polygonLayerNoColorOverlayIcon; showRect: checkboxBorder.checked; invertColors: checkboxColor.checked }
            GalleryComponents.IconLine { name: "rasterLayerNoColorOverlayIcon"; source: __style.rasterLayerNoColorOverlayIcon; showRect: checkboxBorder.checked; invertColors: checkboxColor.checked }
            GalleryComponents.IconLine { name: "tableLayerNoColorOverlayIcon"; source: __style.tableLayerNoColorOverlayIcon; showRect: checkboxBorder.checked; invertColors: checkboxColor.checked }
            GalleryComponents.IconLine { name: "followGPSNoColorOverlayIcon"; source: __style.followGPSNoColorOverlayIcon; showRect: checkboxBorder.checked; invertColors: checkboxColor.checked }
          }
        }
      }
    }
  }
}
