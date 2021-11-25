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
import QtQuick.Controls 2.7
import "."
import "./components"

Rectangle {
  property color fontColor: "black"
  property color linkColor: fontColor
  property color bgColor: InputStyle.warningBannerColor
  property string source: InputStyle.exclamationIcon
  property real padding: InputStyle.innerFieldMargin
  property bool showNotification: true
  property string text: "hello"

  signal notificationClosed()
  signal detailsClicked()

  id: notificationBanner
  color: notificationBanner.bgColor
  radius: InputStyle.cornerRadius
  x: padding
  y: padding
  height: childrenRect.height
  anchors {
    margins: padding
  }

  state:"show"

  states: [
    State { name: "show"; when: notificationBanner.showNotification;
      PropertyChanges { target: notificationBanner; opacity: 1.0 }
    },
    State { name: "fade"; when: !notificationBanner.showNotification;
      PropertyChanges { target: notificationBanner; opacity: 0.0 }
    }
  ]

  transitions: Transition {
    NumberAnimation { property: "opacity"; duration: 500 }
    onRunningChanged:
    {
      if ( state == "show" && !running )
        notificationBanner.visible = true;
      if ( state == "fade" && !running )
      {
        notificationBanner.visible = false;
        notificationClosed();
      }
    }
  }
  layer.enabled: true
  layer.effect: Shadow { verticalOffset: 0 }

  //! Prevents propagating events to other components while notificationBanner is shown (e.g no map panning)
  MouseArea {
    anchors.fill: notificationBanner
    enabled: notificationBanner.showNotification
  }

  Button {
      id: closeBtn
      width: 50
      height: 50
      anchors.right: parent.right
      anchors.margins: 10
      icon.width: width
      icon.height: height
      icon.name: "close-icon"
      icon.source: "qrc:/ic_clear_black.svg"
      palette.button: "#00FFFFFF"
      onClicked: notificationBanner.state = "fade"
  }

  ColumnLayout {
    width: notificationBanner.width
    anchors.centerIn: notificationBanner
    anchors.fill: parent

    TextWithIcon {
      id: content
      Layout.fillWidth: true
      Layout.fillHeight: true
      fontColor: notificationBanner.fontColor
      iconColor: notificationBanner.fontColor
      bgColor: notificationBanner.bgColor
      source: notificationBanner.source
      textItem.font.bold: true
      textItem.rightPadding: InputStyle.innerFieldMargin
      textItem.text: notificationBanner.text
    }
  }

  Button {
      anchors.right: notificationBanner.right
      anchors.bottom: notificationBanner.bottom
      anchors.margins: 20
      palette.button: "green"
      text: "Details"
      onClicked: detailsClicked()
  }
}
