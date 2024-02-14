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
        MMCheckBox { id: checkboxBorder; text: "Show border" }
        Item { width: 1; height: 10 }
        MMCheckBox { id: checkboxWhite; text: "White?" }
        Item { width: 1; height: 10 }
        Row {
          Repeater {
            model: ["16px", "18px", "24px", "32px", "40px"]
            delegate: Text {
              width:  60 * __dp
              height: 20
              text: modelData
              font.bold: true
            }
          }
        }
        IconLine { name: "addImageIcon"; source: __style.addImageIcon; showRect: checkboxBorder.checked; showWhite: checkboxWhite.checked }
        IconLine { name: "arrowDownIcon"; source: __style.arrowDownIcon; showRect: checkboxBorder.checked; showWhite: checkboxWhite.checked }
        IconLine { name: "arrowLinkRightIcon"; source: __style.arrowLinkRightIcon; showRect: checkboxBorder.checked; showWhite: checkboxWhite.checked }
        IconLine { name: "arrowUpIcon"; source: __style.arrowUpIcon; showRect: checkboxBorder.checked; showWhite: checkboxWhite.checked }
        IconLine { name: "backIcon"; source: __style.backIcon; showRect: checkboxBorder.checked; showWhite: checkboxWhite.checked }
        IconLine { name: "calendarIcon"; source: __style.calendarIcon; showRect: checkboxBorder.checked; showWhite: checkboxWhite.checked }
        IconLine { name: "checkmarkIcon"; source: __style.checkmarkIcon; showRect: checkboxBorder.checked; showWhite: checkboxWhite.checked }
        IconLine { name: "closeButtonIcon"; source: __style.closeButtonIcon; showRect: checkboxBorder.checked; showWhite: checkboxWhite.checked }
        IconLine { name: "closeIcon"; source: __style.closeIcon; showRect: checkboxBorder.checked; showWhite: checkboxWhite.checked }
        IconLine { name: "comboBoxCheckIcon"; source: __style.comboBoxCheckIcon; showRect: checkboxBorder.checked; showWhite: checkboxWhite.checked }
        IconLine { name: "deleteIcon"; source: __style.deleteIcon; showRect: checkboxBorder.checked; showWhite: checkboxWhite.checked }
        IconLine { name: "doneIcon"; source: __style.doneIcon; showRect: checkboxBorder.checked; showWhite: checkboxWhite.checked }
        IconLine { name: "done2Icon"; source: __style.done2Icon; showRect: checkboxBorder.checked; showWhite: checkboxWhite.checked }
        IconLine { name: "downloadIcon"; source: __style.downloadIcon; showRect: checkboxBorder.checked; showWhite: checkboxWhite.checked }
        IconLine { name: "editIcon"; source: __style.editIcon; showRect: checkboxBorder.checked; showWhite: checkboxWhite.checked }
        IconLine { name: "edit2Icon"; source: __style.edit2Icon; showRect: checkboxBorder.checked; showWhite: checkboxWhite.checked }
        IconLine { name: "errorIcon"; source: __style.errorIcon; showRect: checkboxBorder.checked; showWhite: checkboxWhite.checked }
        IconLine { name: "globeIcon"; source: __style.globeIcon; showRect: checkboxBorder.checked; showWhite: checkboxWhite.checked }
        IconLine { name: "globalIcon"; source: __style.globalIcon; showRect: checkboxBorder.checked; showWhite: checkboxWhite.checked }
        IconLine { name: "globalFilledIcon"; source: __style.globalFilledIcon; showRect: checkboxBorder.checked; showWhite: checkboxWhite.checked }
        IconLine { name: "gpsAntennaHeightIcon"; source: __style.gpsAntennaHeightIcon; showRect: checkboxBorder.checked; showWhite: checkboxWhite.checked }
        IconLine { name: "gpsIcon"; source: __style.gpsIcon; showRect: checkboxBorder.checked; showWhite: checkboxWhite.checked }
        IconLine { name: "gpsIconFilled"; source: __style.gpsIconFilled; showRect: checkboxBorder.checked; showWhite: checkboxWhite.checked }
        IconLine { name: "hideIcon"; source: __style.hideIcon; showRect: checkboxBorder.checked; showWhite: checkboxWhite.checked }
        IconLine { name: "homeIcon"; source: __style.homeIcon; showRect: checkboxBorder.checked; showWhite: checkboxWhite.checked }
        IconLine { name: "homeFilledIcon"; source: __style.homeFilledIcon; showRect: checkboxBorder.checked; showWhite: checkboxWhite.checked }
        IconLine { name: "infoIcon"; source: __style.infoIcon; showRect: checkboxBorder.checked; showWhite: checkboxWhite.checked }
        IconLine { name: "layersIcon"; source: __style.layersIcon; showRect: checkboxBorder.checked; showWhite: checkboxWhite.checked }
        IconLine { name: "minusIcon"; source: __style.minusIcon; showRect: checkboxBorder.checked; showWhite: checkboxWhite.checked }
        IconLine { name: "moreIcon"; source: __style.moreIcon; showRect: checkboxBorder.checked; showWhite: checkboxWhite.checked }
        IconLine { name: "morePhotosIcon"; source: __style.morePhotosIcon; showRect: checkboxBorder.checked; showWhite: checkboxWhite.checked }
        IconLine { name: "nextIcon"; source: __style.nextIcon; showRect: checkboxBorder.checked; showWhite: checkboxWhite.checked }
        IconLine { name: "plusIcon"; source: __style.plusIcon; showRect: checkboxBorder.checked; showWhite: checkboxWhite.checked }
        IconLine { name: "positionTrackingIcon"; source: __style.positionTrackingIcon; showRect: checkboxBorder.checked; showWhite: checkboxWhite.checked }
        IconLine { name: "projectButtonMoreIcon"; source: __style.projectButtonMoreIcon; showRect: checkboxBorder.checked; showWhite: checkboxWhite.checked }
        IconLine { name: "previousIcon"; source: __style.previousIcon; showRect: checkboxBorder.checked; showWhite: checkboxWhite.checked }
        IconLine { name: "projectsIcon"; source: __style.projectsIcon; showRect: checkboxBorder.checked; showWhite: checkboxWhite.checked }
        IconLine { name: "projectsFilledIcon"; source: __style.projectsFilledIcon; showRect: checkboxBorder.checked; showWhite: checkboxWhite.checked }
        IconLine { name: "qrCodeIcon"; source: __style.qrCodeIcon; showRect: checkboxBorder.checked; showWhite: checkboxWhite.checked }
        IconLine { name: "satelliteIcon"; source: __style.satelliteIcon; showRect: checkboxBorder.checked; showWhite: checkboxWhite.checked }
        IconLine { name: "searchIcon"; source: __style.searchIcon; showRect: checkboxBorder.checked; showWhite: checkboxWhite.checked }
        IconLine { name: "showIcon"; source: __style.showIcon; showRect: checkboxBorder.checked; showWhite: checkboxWhite.checked }
        IconLine { name: "stopIcon"; source: __style.stopIcon; showRect: checkboxBorder.checked; showWhite: checkboxWhite.checked }
        IconLine { name: "syncIcon"; source: __style.syncIcon; showRect: checkboxBorder.checked; showWhite: checkboxWhite.checked }
        IconLine { name: "sync2Icon"; source: __style.sync2Icon; showRect: checkboxBorder.checked; showWhite: checkboxWhite.checked }
        IconLine { name: "undoIcon"; source: __style.undoIcon; showRect: checkboxBorder.checked; showWhite: checkboxWhite.checked }
        IconLine { name: "waitingIcon"; source: __style.waitingIcon; showRect: checkboxBorder.checked; showWhite: checkboxWhite.checked }
        IconLine { name: "xIcon"; source: __style.xIcon; showRect: checkboxBorder.checked; showWhite: checkboxWhite.checked }
        IconLine { name: "xMarkIcon"; source: __style.xMarkIcon; showRect: checkboxBorder.checked; showWhite: checkboxWhite.checked }
        Item { width: 1; height: 30 }

        Text { text: "Icons - in Circle"; font.underline: true }
        Row { MMCircleIcon { size: __style.icon24; type: MMCircleIcon.Type.Checkmark; color: __style.forestColor; iconColor: __style.grassColor } Text { text: "Type.Checkmark" } }
        Row { MMCircleIcon { size: __style.icon24; iconSize: __style.icon16; type: MMCircleIcon.Type.Edit; color: __style.forestColor; iconColor: __style.grassColor } Text { text: "Type.Edit" } }
        // Remove this one - not icon
        Row { MMCircleIcon { size: __style.icon40; type: MMCircleIcon.Type.Delete; color: __style.negativeColor; iconColor: __style.grapeColor } Text { text: "Type.Delete" } }
        Item { width: 1; height: 30 }

        // REMOVE alebo presun do ImagesPage + images (qrc aj nazov aj vsetko)
        Text { text: "Icons - Map"; font.underline: true }
        Row { Image { source: __style.directionIcon } Text { text: "directionIcon" } }
        Row { Image { source: __style.mapPinIcon } Text { text: "mapPinIcon" } }
        Row { Image { source: __style.trackingDirectionIcon } Text { text: "trackingDirectionIcon" } }
        Item { width: 1; height: 30 }

        // use IconLine
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

         // use IconLine
        Text { text: "Icons - Social"; font.underline: true }
        Row { Image { source: __style.youtubeIcon } Text { text: "youtubeIcon" } }
        Row { Image { source: __style.xTwitterIcon } Text { text: "xTwitterIcon" } }
        Row { Image { source: __style.facebookIcon } Text { text: "facebookIcon" } }
        Row { Image { source: __style.linkedinIcon } Text { text: "linkedinIcon" } }
        Row { Image { source: __style.mastodonIcon } Text { text: "mastodonIcon" } }
        Row { Image { source: __style.redditIcon } Text { text: "redditIcon" } }
        Item { width: 1; height: 30 }

        // REMOVE - mame to aj tak v onboarding page
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
