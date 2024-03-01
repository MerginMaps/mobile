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

import "../components"

Drawer {
  id: root

  height: mainColumn.height + header.height + borderRectangle.height
  width: window.width
  edge: Qt.BottomEdge
  focus: true
  dim: true
  interactive: false
  dragMargin: 0
  closePolicy: Popup.CloseOnEscape | Popup.CloseOnPressOutside

  Component.onCompleted: {
    forceActiveFocus()
  }

  property var positionProvider: __positionKit.positionProvider
  property string howToConnectGPSLink: __inputHelp.howToConnectGPSLink

  property string titleText: {
    if ( rootstate.state === "working" )
    {
      return qsTr( "Connecting to" ) + " " + ( root.positionProvider ? root.positionProvider.name() : "" )
    }
    else if ( rootstate.state === "success" )
    {
      return qsTr( "Connected" )
    }
    else
    {
      // either NoConnection or WaitingToReconnect
      return qsTr( "Failed to connect to" ) + " " + ( root.positionProvider ? root.positionProvider.name() : "" )
    }
  }

  property string descriptionText: {
    if ( rootstate.state === "working" )
    {
      return qsTr( "You might be asked to pair your device during this process." )
    }
    else if ( rootstate.state === "success" )
    {
      return ""
    }
    else if ( rootstate.state === "waitingToReconnect" )
    {
      return __positionKit.positionProvider.stateMessage + "<br><br>" +
          qsTr( "You can close this message, we will try to repeatedly connect to your device.%1 If you need more help, %2click here%3" )
      .arg("<br>")
      .arg("<a style=\"text-decoration: underline; color:" + __style.forestColor + ";\" href='" + root.howToConnectGPSLink + "'>")
      .arg("</a>")
    }

    else
    {
      return qsTr( "We were not able to connect to the specified device.
        Please make sure your device is powered on and can be connected to.%1 %2Learn more here%3." )
      .arg("<br>")
      .arg("<a style=\"text-decoration: underline; color:" + __style.forestColor + ";\" href='" + root.howToConnectGPSLink + "'>")
      .arg("</a>")
    }
  }

  property var imageSource: {
    if ( rootstate.state === "fail" )
    {
      return __style.externalGpsRedImage
    }
    else {
      return __style.externalGpsGreenImage
    }
  }

  signal success()
  signal failure()

  Item {
    focus: true
    // just close the popup, keep the connection running
    Keys.onReleased: function( event ) {
      if (event.key === Qt.Key_Back || event.key === Qt.Key_Escape) {
        event.accepted = true
        close()
      }
    }
  }

  Timer {
    id: closeTimer

    interval: 1500
    repeat: false
    running: false
    onTriggered: {
      if ( rootstate.state === "success" )
      {
        root.success()
      }
      else
      {
        root.failure()
      }
      root.close()
    }
  }

  StateGroup {
    id: rootstate

    states: [
      State {
        name: "working"
        when: root.positionProvider && root.positionProvider.state === PositionProvider.Connecting
        PropertyChanges { target: loadingSpinner; opacity: 1.0 }
      },
      State {
        name: "success"
        when: root.positionProvider && root.positionProvider.state === PositionProvider.Connected
        PropertyChanges { target: loadingSpinner; opacity: 0.0 }
      },
      State {
        name: "fail"
        when: !root.positionProvider || root.positionProvider.state === PositionProvider.NoConnection
        PropertyChanges { target: loadingSpinner; opacity: 0.0 }
      },
      State {
        name: "waitingToReconnect"
        when: !root.positionProvider || root.positionProvider.state === PositionProvider.WaitingToReconnect
        PropertyChanges { target: loadingSpinner; opacity: 0.0 }
      }
    ]

    transitions: [
      Transition {
        from: "working"
        to: "success"

        SequentialAnimation {
          NumberAnimation { target:loadingSpinner; property: "opacity"; duration: 200 }
          ScriptAction {
            script: {
              closeTimer.start()
            }
          }
        }
      },
      Transition {
        from: "working"
        to: "fail"

        SequentialAnimation {
          NumberAnimation { target:loadingSpinner; property: "opacity"; duration: 200 }
        }
      },
      Transition {
        from: "*"
        to: "waitingToReconnect"

        SequentialAnimation {
          NumberAnimation { target:loadingSpinner; property: "opacity"; duration: 200 }
        }
      },
      Transition {
        from: "*"
        to: "working"

        SequentialAnimation {
          NumberAnimation { target:loadingSpinner; property: "opacity"; from: 0.0; to: 1.0; duration: 200 }
        }
      }
    ]

    state: "working"
  }


  Rectangle {
    id: borderRectangle

    color: roundedRect.color
    anchors.top: parent.top
    anchors.left: parent.left
    anchors.right: parent.right
    height: 2 * radius
    anchors.topMargin: -radius
    radius: 20 * __dp
  }

  Rectangle {
    id: roundedRect

    anchors.fill: parent
    color: __style.whiteColor

    MMPageHeader {
      id: header

      backVisible: false

      MMRoundButton {
        id: backBtn

        anchors.right: parent.right
        anchors.rightMargin: __style.pageMargins
        anchors.verticalCenter: parent.verticalCenter

        iconSource: __style.closeIcon
        iconColor: __style.forestColor

        bgndColor: __style.lightGreenColor
        bgndHoverColor: __style.mediumGreenColor

        onClicked: close()
      }
    }

    Column {
      id: mainColumn

      width: parent.width
      anchors.left: parent.left
      anchors.leftMargin: __style.pageMargins
      anchors.top: header.top
      anchors.topMargin: __style.margin40

      spacing: __style.margin12

      Item {
        id: statusIconContainer

        width: parent.width
        height: resultIcon.height

        BusyIndicator {
          id: loadingSpinner
          height: parent.height / 2
          width: parent.height / 2

          anchors.centerIn: parent
          running: rootstate.state === "working"
        }

        Image {
          id: resultIcon
          anchors.centerIn: parent
          source: root.imageSource
        }
      }

      Text {
        text: root.titleText

        width: parent.width

        horizontalAlignment: Text.AlignHCenter

        elide: Text.ElideRight
        color: __style.forestColor
        font: __style.t1
      }

      Text {
        text: root.descriptionText

        width: parent.width
        horizontalAlignment: Text.AlignHCenter

        elide: Text.ElideRight
        wrapMode: Text.WordWrap
        textFormat: Text.RichText
        color: __style.nightColor
        font: __style.p5

        onLinkActivated: function( link ) {
          Qt.openUrlExternally( link )
        }
      }

      Item {
        width: parent.width
        height: __style.margin40
      }
    }
  }
}
