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

    property int rectSize: 10

    GroupBox {
      title: "Icons"
      background: Rectangle {
        color: "white"
        border.color: "gray"
      }
      label: Text {
        color: "black"
        text: parent.title
        padding: 5
      }

      Grid {
        columns: 3
        spacing: 20
        anchors.fill: parent
        IconBox {
          text: "arrowDownIcon"
          source: __style.arrowDownIcon
        }
        IconBox {
          text: "arrowLinkRightIcon"
          source: __style.arrowLinkRightIcon
          colorise: true
        }
        IconBox {
          text: "arrowUpIcon"
          source: __style.arrowUpIcon
        }
        IconBox {
          text: "backIcon"
          source: __style.backIcon
        }
        IconBox {
          text: "calendarIcon"
          source: __style.calendarIcon
        }
        IconBox {
          text: "checkmarkIcon"
          source: __style.checkmarkIcon
        }
        IconBox {
          text: "closeButtonIcon"
          source: __style.closeButtonIcon
        }
        IconBox {
          text: "closeIcon"
          source: __style.closeIcon
          colorise: true
        }
        IconBox {
          text: "comboBoxCheckIcon"
          source: __style.comboBoxCheckIcon
        }
        IconBox {
          text: "deleteIcon"
          source: __style.deleteIcon
          colorise: true
        }
        IconBox {
          text: "directionIcon"
          source: __style.directionIcon
        }
        IconBox {
          text: "doneIcon"
          source: __style.doneIcon
        }
        IconBox {
          text: "downloadIcon"
          source: __style.downloadIcon
        }
        IconBox {
          text: "editIcon"
          source: __style.editIcon
          colorise: true
        }
        IconBox {
          text: "errorIcon"
          source: __style.errorIcon
        }
        IconBox {
          text: "globeIcon"
          source: __style.globeIcon
        }
        IconBox {
          text: "globalIcon"
          source: __style.globalIcon
          colorise: true
        }
        IconBox {
          text: "globalFilledIcon"
          source: __style.globalFilledIcon
          colorise: true
        }
        IconBox {
          text: "hideIcon"
          source: __style.hideIcon
        }
        IconBox {
          text: "homeIcon"
          source: __style.homeIcon
          colorise: true
        }
        IconBox {
          text: "homeFilledIcon"
          source: __style.homeFilledIcon
          colorise: true
        }
        IconBox {
          text: "infoIcon"
          source: __style.infoIcon
        }
        IconBox {
          text: "minusIcon"
          source: __style.minusIcon
        }
        IconBox {
          text: "moreIcon"
          source: __style.moreIcon
          colorise: true
        }
        IconBox {
          text: "morePhotosIcon"
          source: __style.morePhotosIcon
          colorise: true
        }
        IconBox {
          text: "nextIcon"
          source: __style.nextIcon
        }
        IconBox {
          text: "plusIcon"
          source: __style.plusIcon
        }
        IconBox {
          text: "projectButtonMoreIcon"
          source: __style.projectButtonMoreIcon
        }
        IconBox {
          text: "previousIcon"
          source: __style.previousIcon
        }
        IconBox {
          text: "projectsIcon"
          source: __style.projectsIcon
          colorise: true
        }
        IconBox {
          text: "projectsFilledIcon"
          source: __style.projectsFilledIcon
          colorise: true
        }
        IconBox {
          text: "qrCodeIcon"
          source: __style.qrCodeIcon
        }
        IconBox {
          text: "searchIcon"
          source: __style.searchIcon
        }
        IconBox {
          text: "showIcon"
          source: __style.showIcon
        }
        IconBox {
          text: "stopIcon"
          source: __style.stopIcon
          colorise: true
        }
        IconBox {
          text: "syncIcon"
          source: __style.syncIcon
          colorise: true
        }
        IconBox {
          text: "trackingDirectionIcon"
          source: __style.trackingDirectionIcon
        }
        IconBox {
          text: "waitingIcon"
          source: __style.waitingIcon
        }
        IconBox {
          text: "xMarkIcon"
          source: __style.xMarkIcon
        }
        IconBox {
          text: "Antenna"
          source: __style.gpsAntennaHeightIcon
        }
        IconBox {
          text: "GPS"
          source: __style.gpsIcon
        }
        IconBox {
          text: "GPS Filled"
          source: __style.gpsIconFilled
        }
        IconBox {
          text: "Layers"
          source: __style.layersIcon
        }
        IconBox {
          text: "Map pin"
          source: __style.mapPinIcon
        }
        IconBox {
          text: "Tracking"
          source: __style.positionTrackingIcon
        }
        IconBox {
          text: "Satellite"
          source: __style.satelliteIcon
        }
        IconBox {
          text: "Undo"
          source: __style.undoIcon
        }
      }
    }

    GroupBox {
      title: "Icons - How you found us"
      background: Rectangle {
        color: "white"
        border.color: "gray"
      }
      label: Text {
        color: "black"
        text: parent.title
        padding: 5
      }

      Grid {
        columns: 3
        spacing: 20
        anchors.fill: parent
        IconBox {
          text: "searchIcon"
          source: __style.searchIcon
        }
        IconBox {
          text: "termsIcon"
          source: __style.termsIcon
        }
        IconBox {
          text: "mouthIcon"
          source: __style.mouthIcon
        }
        IconBox {
          text: "qgisIcon"
          source: __style.qgisIcon
        }
        IconBox {
          text: "subscriptionsIcon"
          source: __style.subscriptionsIcon
        }
        IconBox {
          text: "teacherIcon"
          source: __style.teacherIcon
        }
        IconBox {
          text: "briefcaseIcon"
          source: __style.briefcaseIcon
        }
        IconBox {
          text: "socialMediaIcon"
          source: __style.socialMediaIcon
        }
        IconBox {
          text: "otherIcon"
          source: __style.otherIcon
        }
      }
    }

    GroupBox {
      title: "Icons - Social"
      background: Rectangle {
        color: "white"
        border.color: "gray"
      }
      label: Text {
        color: "black"
        text: parent.title
        padding: 5
      }

      Grid {
        columns: 3
        spacing: 20
        anchors.fill: parent
        IconBox {
          text: "youtubeIcon"
          source: __style.youtubeIcon
        }
        IconBox {
          text: "xTwitterIcon"
          source: __style.xTwitterIcon
        }
        IconBox {
          text: "facebookIcon"
          source: __style.facebookIcon
        }
        IconBox {
          text: "linkedinIcon"
          source: __style.linkedinIcon
        }
        IconBox {
          text: "mastodonIcon"
          source: __style.mastodonIcon
        }
        IconBox {
          text: "redditIcon"
          source: __style.redditIcon
        }
      }
    }

    GroupBox {
      title: "Icons - Industries"
      background: Rectangle {
        color: "white"
        border.color: "gray"
      }
      label: Text {
        color: "black"
        text: parent.title
        padding: 5
      }

      Grid {
        columns: 2
        spacing: 20
        anchors.fill: parent
        IconBox {
          text: "tractorIcon"
          source: __style.tractorIcon
        }
        IconBox {
          text: "archaeologyIcon"
          source: __style.archaeologyIcon
        }
        IconBox {
          text: "engineeringIcon"
          source: __style.engineeringIcon
        }
        IconBox {
          text: "electricityIcon"
          source: __style.electricityIcon
        }
        IconBox {
          text: "environmentalIcon"
          source: __style.environmentalIcon
        }
        IconBox {
          text: "stateAndLocalIcon"
          source: __style.stateAndLocalIcon
        }
        IconBox {
          text: "naturalResourcesIcon"
          source: __style.naturalResourcesIcon
        }
        IconBox {
          text: "telecommunicationIcon"
          source: __style.telecommunicationIcon
        }
        IconBox {
          text: "transportationIcon"
          source: __style.transportationIcon
        }
        IconBox {
          text: "waterResourcesIcon"
          source: __style.waterResourcesIcon
        }
        IconBox {
          text: "othersIcon"
          source: __style.othersIcon
        }
      }
    }
  }
}
