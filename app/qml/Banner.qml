/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/
import QtQuick 2.7
import QtQuick.Layouts 1.3
import QtGraphicalEffects 1.0
import "."
import "components"

Rectangle {
  id: banner
  property color bgColor: InputStyle.warningBannerColor
  property color fontColor: "black"
  property string link: ""
  property color linkColor: fontColor
  property real padding: InputStyle.innerFieldMargin
  property bool showWarning: false
  property string source: InputStyle.exclamationIcon
  property string text: ""

  color: banner.bgColor
  height: childrenRect.height
  layer.enabled: true
  radius: InputStyle.cornerRadius
  state: "fade"
  x: padding
  y: padding

  anchors {
    margins: padding
  }

  //! Prevents propagating events to other components while banner is shown (e.g no map panning)
  MouseArea {
    anchors.fill: banner
    enabled: banner.showWarning
  }
  ColumnLayout {
    anchors.centerIn: banner
    spacing: 0
    width: banner.width

    TextWithIcon {
      id: content
      Layout.fillWidth: true
      bgColor: banner.bgColor
      fontColor: banner.fontColor
      iconColor: banner.fontColor
      source: banner.source
      textItem.font.bold: true
      textItem.rightPadding: InputStyle.innerFieldMargin
      textItem.text: "<style>a:link { color: " + banner.linkColor + "; text-decoration: underline; }</style>" + qsTr("%1<br><a href='%2'>Learn more</a>").arg(banner.text).arg(banner.link)

      onLinkActivated: Qt.openUrlExternally(link)
    }
  }

  layer.effect: Shadow {
    verticalOffset: 0
  }
  states: [
    State {
      name: "show"
      when: banner.showWarning

      PropertyChanges {
        opacity: 1.0
        target: banner
      }
    },
    State {
      name: "fade"
      when: !banner.showWarning

      PropertyChanges {
        opacity: 0.0
        target: banner
      }
    }
  ]
  transitions: Transition {
    NumberAnimation {
      duration: 500
      property: "opacity"
    }
  }
}
