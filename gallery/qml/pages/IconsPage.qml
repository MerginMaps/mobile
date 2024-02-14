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

    MMCheckBox { id: checkboxBorder; text: "Show border" }
    Item { width: 1; height: 10 }
    MMCheckBox { id: checkboxWhite; text: "White?" }
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
              Repeater {
                model: ["16px", "18px", "24px", "32px", "40px"]
                delegate: Text {
                  width: 50 * __dp
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
            IconLine { name: "closeIcon"; source: __style.closeIcon; showRect: checkboxBorder.checked; showWhite: checkboxWhite.checked }
            IconLine { name: "deleteIcon"; source: __style.deleteIcon; showRect: checkboxBorder.checked; showWhite: checkboxWhite.checked }
            IconLine { name: "downloadIcon"; source: __style.downloadIcon; showRect: checkboxBorder.checked; showWhite: checkboxWhite.checked }
            IconLine { name: "editIcon"; source: __style.editIcon; showRect: checkboxBorder.checked; showWhite: checkboxWhite.checked }
            IconLine { name: "globeIcon"; source: __style.globeIcon; showRect: checkboxBorder.checked; showWhite: checkboxWhite.checked }
            IconLine { name: "globalIcon"; source: __style.globalIcon; showRect: checkboxBorder.checked; showWhite: checkboxWhite.checked }
            IconLine { name: "globalFilledIcon"; source: __style.globalFilledIcon; showRect: checkboxBorder.checked; showWhite: checkboxWhite.checked }
            IconLine { name: "gpsAntennaHeightIcon"; source: __style.gpsAntennaHeightIcon; showRect: checkboxBorder.checked; showWhite: checkboxWhite.checked }
            IconLine { name: "gpsIcon"; source: __style.gpsIcon; showRect: checkboxBorder.checked; showWhite: checkboxWhite.checked }
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
            IconLine { name: "termsIcon"; source: __style.termsIcon; showRect: checkboxBorder.checked; showWhite: checkboxWhite.checked }
            IconLine { name: "mouthIcon"; source: __style.mouthIcon; showRect: checkboxBorder.checked; showWhite: checkboxWhite.checked }
            IconLine { name: "qgisIcon"; source: __style.qgisIcon; showRect: checkboxBorder.checked; showWhite: checkboxWhite.checked }
            IconLine { name: "subscriptionsIcon"; source: __style.subscriptionsIcon; showRect: checkboxBorder.checked; showWhite: checkboxWhite.checked }
            IconLine { name: "teacherIcon"; source: __style.teacherIcon; showRect: checkboxBorder.checked; showWhite: checkboxWhite.checked }
            IconLine { name: "briefcaseIcon"; source: __style.briefcaseIcon; showRect: checkboxBorder.checked; showWhite: checkboxWhite.checked }
            IconLine { name: "socialMediaIcon"; source: __style.socialMediaIcon; showRect: checkboxBorder.checked; showWhite: checkboxWhite.checked }
            IconLine { name: "otherIcon"; source: __style.otherIcon; showRect: checkboxBorder.checked; showWhite: checkboxWhite.checked }
            IconLine { name: "youtubeIcon"; source: __style.youtubeIcon; showRect: checkboxBorder.checked; showWhite: checkboxWhite.checked }
            IconLine { name: "xTwitterIcon"; source: __style.xTwitterIcon; showRect: checkboxBorder.checked; showWhite: checkboxWhite.checked }
            IconLine { name: "facebookIcon"; source: __style.facebookIcon; showRect: checkboxBorder.checked; showWhite: checkboxWhite.checked }
            IconLine { name: "linkedinIcon"; source: __style.linkedinIcon; showRect: checkboxBorder.checked; showWhite: checkboxWhite.checked }
            IconLine { name: "mastodonIcon"; source: __style.mastodonIcon; showRect: checkboxBorder.checked; showWhite: checkboxWhite.checked }
            IconLine { name: "redditIcon"; source: __style.redditIcon; showRect: checkboxBorder.checked; showWhite: checkboxWhite.checked }
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
              Repeater {
                model: ["16px", "18px", "24px", "32px", "40px"]
                delegate: Text {
                  width: 50 * __dp
                  height: 20
                  text: modelData
                  font.bold: true
                }
              }
            }
            IconLine { name: "xMarkCircleIcon"; source: __style.xMarkCircleIcon; showRect: checkboxBorder.checked; showWhite: checkboxWhite.checked }
            IconLine { name: "checkmarkCircleIcon"; source: __style.checkmarkCircleIcon; showRect: checkboxBorder.checked; showWhite: checkboxWhite.checked }
            IconLine { name: "errorCircleIcon"; source: __style.errorCircleIcon; showRect: checkboxBorder.checked; showWhite: checkboxWhite.checked }

            // REMOVE - replace with checkmarkCircleIcon
            IconLine { name: "doneIcon"; source: __style.doneIcon; showRect: checkboxBorder.checked; showWhite: checkboxWhite.checked }
            IconLine { name: "done2Icon"; source: __style.done2Icon; showRect: checkboxBorder.checked; showWhite: checkboxWhite.checked }

            // REMOVE replace with xMarkCircleIcon?
            IconLine { name: "closeButtonIcon"; source: __style.closeButtonIcon; showRect: checkboxBorder.checked; showWhite: checkboxWhite.checked }

            // TODO we need icon!
            IconLine { name: "edit2Icon"; source: __style.edit2Icon; showRect: checkboxBorder.checked; showWhite: checkboxWhite.checked }

            // REMOVE
            IconLine { name: "gpsIconFilled"; source: __style.gpsIconFilled; showRect: checkboxBorder.checked; showWhite: checkboxWhite.checked }

            // TODO convert to button!
            IconLine { name: "projectButtonMoreIcon"; source: __style.projectButtonMoreIcon; showRect: checkboxBorder.checked; showWhite: checkboxWhite.checked }
           }
        }
      }
    }
  }
}
