/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/
import QtQuick
import QtQuick.Layouts
import Qt5Compat.GraphicalEffects
import "."
import "./components"

Rectangle {
  id: banner

  // TODO: replace use of this class with Notifications

  property color fontColor: "black"
  property color linkColor: fontColor
  property color bgColor: InputStyle.warningBannerColor
  property string text: ""
  property string link: ""
  property string source: InputStyle.exclamationIcon
  property real padding: InputStyle.innerFieldMargin
  property bool showBanner: false
  property bool withLink: false

  signal clicked()

  function getText()
  {
    if ( banner.withLink )
    {
      return "<style>a:link { color: " + banner.linkColor
          + "; text-decoration: underline; }</style>%1<br><a href='%2' style=\"color: %3;\">".arg(banner.text).arg(banner.link).arg(InputStyle.learnMoreLinkColor) +
          qsTr("Learn more") + "</a>"
    }
    else
    {
      return banner.text
    }
  }

  color: banner.bgColor
  radius: InputStyle.cornerRadius
  x: padding
  y: padding
  height: childrenRect.height
  anchors {
    margins: padding
  }
  state:"fade"

  states: [
    State { name: "show"; when: banner.showBanner;
      PropertyChanges { target: banner; opacity: 1.0 }
    },
    State { name: "fade"; when: !banner.showBanner;
      PropertyChanges { target: banner; opacity: 0.0 }
    }
  ]

  transitions: Transition {
    NumberAnimation { property: "opacity"; duration: 500 }
    onRunningChanged:
    {
      if ( state === "show" && !running )
        banner.visible = true;
      if ( state === "fade" && !running )
        banner.visible = false;
    }
  }

  layer.enabled: true
  layer.effect: Shadow { verticalOffset: 0 }

  //! Prevents propagating events to other components while banner is shown (e.g no map panning)
  MouseArea {
    anchors.fill: banner
    enabled: banner.showBanner

    onClicked: banner.clicked()
  }

  ColumnLayout {
    width: banner.width
    spacing: 0
    anchors.centerIn: banner

    TextWithIcon {
      id: content
      height: banner.height
      Layout.fillWidth: true
      fontColor: banner.fontColor
      iconColor: banner.fontColor
      bgColor: banner.bgColor
      source: banner.source
      textItem.font.bold: true
      textItem.rightPadding: InputStyle.innerFieldMargin
      textItem.text: banner.getText()

      onLinkActivated: function( link ) {
        Qt.openUrlExternally(link)
      }
    }
  }
}
