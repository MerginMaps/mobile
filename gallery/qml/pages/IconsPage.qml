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
import "../"

ScrollView {
  Column {
    padding: 20
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
        MMCheckBox { id: checkbox; text: "Show border" }
        Item { width: 1; height: 10 }
        Row {
          Repeater {
            model: ["Icon", "16px", "24px", "32px", "40px"]
            delegate: Text {
              width: model.index ? 50 * __dp : 100 * __dp
              height: 20
              text: modelData
              font.bold: true
            }
          }
        }
        IconLine { name: "addImageIcon"; source: __style.addImageIcon; showRect: checkbox.checked }
        IconLine { name: "arrowDownIcon"; source: __style.arrowDownIcon; showRect: checkbox.checked }
        IconLine { name: "arrowLinkRightIcon"; source: __style.arrowLinkRightIcon; showRect: checkbox.checked }
        IconLine { name: "arrowUpIcon"; source: __style.arrowUpIcon; showRect: checkbox.checked }
        IconLine { name: "backIcon"; source: __style.backIcon; showRect: checkbox.checked }
        IconLine { name: "calendarIcon"; source: __style.calendarIcon; showRect: checkbox.checked }
        IconLine { name: "checkmarkIcon"; source: __style.checkmarkIcon; showRect: checkbox.checked }
        IconLine { name: "closeButtonIcon"; source: __style.closeButtonIcon; showRect: checkbox.checked }
        IconLine { name: "closeIcon"; source: __style.closeIcon; showRect: checkbox.checked }
        IconLine { name: "comboBoxCheckIcon"; source: __style.comboBoxCheckIcon; showRect: checkbox.checked }
        IconLine { name: "deleteIcon"; source: __style.deleteIcon; showRect: checkbox.checked }
        IconLine { name: "doneIcon"; source: __style.doneIcon; showRect: checkbox.checked }
        IconLine { name: "done2Icon"; source: __style.done2Icon; showRect: checkbox.checked }
        IconLine { name: "downloadIcon"; source: __style.downloadIcon; showRect: checkbox.checked }
        IconLine { name: "editIcon"; source: __style.editIcon; showRect: checkbox.checked }
        IconLine { name: "edit2Icon"; source: __style.edit2Icon; showRect: checkbox.checked }
        IconLine { name: "errorIcon"; source: __style.errorIcon; showRect: checkbox.checked }
        IconLine { name: "globeIcon"; source: __style.globeIcon; showRect: checkbox.checked }
        IconLine { name: "globalIcon"; source: __style.globalIcon; showRect: checkbox.checked }
        IconLine { name: "globalFilledIcon"; source: __style.globalFilledIcon; showRect: checkbox.checked }
        IconLine { name: "gpsAntennaHeightIcon"; source: __style.gpsAntennaHeightIcon; showRect: checkbox.checked }
        IconLine { name: "gpsIcon"; source: __style.gpsIcon; showRect: checkbox.checked }
        IconLine { name: "gpsIconFilled"; source: __style.gpsIconFilled; showRect: checkbox.checked }
        IconLine { name: "hideIcon"; source: __style.hideIcon; showRect: checkbox.checked }
        IconLine { name: "homeIcon"; source: __style.homeIcon; showRect: checkbox.checked }
        IconLine { name: "homeFilledIcon"; source: __style.homeFilledIcon; showRect: checkbox.checked }
        IconLine { name: "infoIcon"; source: __style.infoIcon; showRect: checkbox.checked }
        IconLine { name: "layersIcon"; source: __style.layersIcon; showRect: checkbox.checked }
        IconLine { name: "minusIcon"; source: __style.minusIcon; showRect: checkbox.checked }
        IconLine { name: "moreIcon"; source: __style.moreIcon; showRect: checkbox.checked }
        IconLine { name: "morePhotosIcon"; source: __style.morePhotosIcon; showRect: checkbox.checked }
        IconLine { name: "nextIcon"; source: __style.nextIcon; showRect: checkbox.checked }
        IconLine { name: "plusIcon"; source: __style.plusIcon; showRect: checkbox.checked }
        IconLine { name: "positionTrackingIcon"; source: __style.positionTrackingIcon; showRect: checkbox.checked }
        IconLine { name: "projectButtonMoreIcon"; source: __style.projectButtonMoreIcon; showRect: checkbox.checked }
        IconLine { name: "previousIcon"; source: __style.previousIcon; showRect: checkbox.checked }
        IconLine { name: "projectsIcon"; source: __style.projectsIcon; showRect: checkbox.checked }
        IconLine { name: "projectsFilledIcon"; source: __style.projectsFilledIcon; showRect: checkbox.checked }
        IconLine { name: "qrCodeIcon"; source: __style.qrCodeIcon; showRect: checkbox.checked }
        IconLine { name: "satelliteIcon"; source: __style.satelliteIcon; showRect: checkbox.checked }
        IconLine { name: "searchIcon"; source: __style.searchIcon; showRect: checkbox.checked }
        IconLine { name: "showIcon"; source: __style.showIcon; showRect: checkbox.checked }
        IconLine { name: "stopIcon"; source: __style.stopIcon; showRect: checkbox.checked }
        IconLine { name: "syncIcon"; source: __style.syncIcon; showRect: checkbox.checked }
        IconLine { name: "sync2Icon"; source: __style.sync2Icon; showRect: checkbox.checked }
        IconLine { name: "undoIcon"; source: __style.undoIcon; showRect: checkbox.checked }
        IconLine { name: "waitingIcon"; source: __style.waitingIcon; showRect: checkbox.checked }
        IconLine { name: "xIcon"; source: __style.xIcon; showRect: checkbox.checked }
        IconLine { name: "xMarkIcon"; source: __style.xMarkIcon; showRect: checkbox.checked }
        Item { width: 1; height: 30 }

        Text { text: "Icons - in Circle"; font.underline: true }
        Row { MMCircleIcon { size: __style.icon24; type: MMCircleIcon.Type.Checkmark; color: __style.forestColor; iconColor: __style.grassColor } Text { text: "Type.Checkmark" } }
        Row { MMCircleIcon { size: __style.icon24; type: MMCircleIcon.Type.Edit; color: __style.forestColor; iconColor: __style.grassColor } Text { text: "Type.Edit" } }
        Row { MMCircleIcon { size: __style.icon40; type: MMCircleIcon.Type.Delete; color: __style.negativeColor; iconColor: __style.grapeColor } Text { text: "Type.Delete" } }
        Item { width: 1; height: 30 }

        Text { text: "Icons - Map"; font.underline: true }
        Row { Image { source: __style.directionIcon } Text { text: "directionIcon" } }
        Row { Image { source: __style.mapPinIcon } Text { text: "mapPinIcon" } }
        Row { Image { source: __style.trackingDirectionIcon } Text { text: "trackingDirectionIcon" } }
        Item { width: 1; height: 30 }

        Text { text: "Icons - How you found us"; font.underline: true }
        Row { Image { source: __style.termsIcon } Text { text: "termsIcon" } }
        Row { Image { source: __style.mouthIcon } Text { text: "mouthIcon" } }
        Row { Image { source: __style.qgisIcon } Text { text: "qgisIcon" } }
        Row { Image { source: __style.subscriptionsIcon } Text { text: "subscriptionsIcon" } }
        Row { Image { source: __style.teacherIcon } Text { text: "teacherIcon" } }
        Row { Image { source: __style.briefcaseIcon } Text { text: "briefcaseIcon" } }
        Row { Image { source: __style.socialMediaIcon } Text { text: "socialMediaIcon" } }
        Row { Image { source: __style.otherIcon } Text { text: "otherIcon" } }
        Item { width: 1; height: 30 }

        Text { text: "Icons - Social"; font.underline: true }
        Row { Image { source: __style.youtubeIcon } Text { text: "youtubeIcon" } }
        Row { Image { source: __style.xTwitterIcon } Text { text: "xTwitterIcon" } }
        Row { Image { source: __style.facebookIcon } Text { text: "facebookIcon" } }
        Row { Image { source: __style.linkedinIcon } Text { text: "linkedinIcon" } }
        Row { Image { source: __style.mastodonIcon } Text { text: "mastodonIcon" } }
        Row { Image { source: __style.redditIcon } Text { text: "redditIcon" } }
        Item { width: 1; height: 30 }

        Text { text: "Icons - Social"; font.underline: true }
        Row { Image { source: __style.tractorIcon } Text { text: "tractorIcon" } }
        Row { Image { source: __style.archaeologyIcon } Text { text: "archaeologyIcon" } }
        Row { Image { source: __style.engineeringIcon } Text { text: "engineeringIcon" } }
        Row { Image { source: __style.electricityIcon } Text { text: "electricityIcon" } }
        Row { Image { source: __style.environmentalIcon } Text { text: "environmentalIcon" } }
        Row { Image { source: __style.stateAndLocalIcon } Text { text: "stateAndLocalIcon" } }
        Row { Image { source: __style.naturalResourcesIcon } Text { text: "naturalResourcesIcon" } }
        Row { Image { source: __style.telecommunicationIcon } Text { text: "telecommunicationIcon" } }
        Row { Image { source: __style.transportationIcon } Text { text: "transportationIcon" } }
        Row { Image { source: __style.waterResourcesIcon } Text { text: "waterResourcesIcon" } }
        Row { Image { source: __style.othersIcon } Text { text: "othersIcon" } }
      }
    }
  }
}
