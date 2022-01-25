/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

import QtQuick 2.14
import QtQuick.Controls 2.14
import QtQuick.Layouts 1.14
import ".."

Dialog {
  id: root

  property string titleText: {
    if ( rootstate.state === "working")
    {
      return qsTr( "Connecting to" ) + " " + ( __positionKit.positionProvider ? __positionKit.positionProvider.name() : "" )
    }
    else if ( rootstate.state === "success" )
    {
      return qsTr( "Connected" )
    }
    else
    {
      return qsTr( "Failed to connect to" ) + " " + ( __positionKit.positionProvider ? __positionKit.positionProvider.name() : "" )
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
    else
    {
      return qsTr( "We were not able to connect to the specified device.
        Please make sure your device is powered on and can be connected to.%1 %2Learn more here%3." )
      .arg("\n")
      .arg("<a style=\"text-decoration: underline; color:" + InputStyle.fontColor + ";\" href='" + __inputHelp.howToConnectGPSLink + "'>")
      .arg("</a>")
    }
  }

  signal success()
  signal failure()

  Connections {
    target: __positionKit.positionProvider

    function onLostConnection()
    {
      rootstate.state = "fail"
    }

    function onProviderConnecting()
    {
      rootstate.state = "working"
    }

    function onProviderConnected()
    {
      rootstate.state = "success"
    }
  }

  focus: true

  // just close the popup, keep the connection running
  Keys.onReleased: {
    if (event.key === Qt.Key_Back || event.key === Qt.Key_Escape) {
      event.accepted = true
      close()
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
        PropertyChanges { target: loadingSpinner; opacity: 1.0 }
        PropertyChanges { target: resultIcon; opacity: 0.0 }
      },
      State {
        name: "success"
        PropertyChanges { target: resultIcon; source: InputStyle.yesIcon }
        PropertyChanges { target: loadingSpinner; opacity: 0.0 }
        PropertyChanges { target: resultIcon; opacity: 1.0 }
      },
      State {
        name: "fail"
        PropertyChanges { target: resultIcon; source: InputStyle.noIcon }
        PropertyChanges { target: loadingSpinner; opacity: 0.0 }
        PropertyChanges { target: resultIcon; opacity: 1.0 }
      }
    ]

    transitions: [
      Transition {
        from: "working"
        to: "success"

        SequentialAnimation {
          NumberAnimation { target:loadingSpinner; property: "opacity"; duration: 200 }
          NumberAnimation { target:resultIcon; property: "opacity"; duration: 200 }
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
          NumberAnimation { target:resultIcon; property: "opacity"; duration: 200 }
        }
      },
      Transition {
        from: "*"
        to: "working"

        SequentialAnimation {
          NumberAnimation { target:loadingSpinner; property: "opacity"; from: 0.0; to: 1.0; duration: 200 }
          NumberAnimation { target:resultIcon; property: "opacity"; from: 1.0; to: 0.0; duration: 200 }
        }
      }
    ]

    state: "working"
  }

  modal: true

  enter: Transition {
    NumberAnimation { property: "opacity"; from: 0.0; to: 1.0; duration: 200 }
  }
  exit: Transition {
    NumberAnimation { property: "opacity"; from: 1.0; to: 0.0; duration: 200 }
  }

  closePolicy: Popup.CloseOnEscape

  background: Rectangle {
    width: root.width
    height: root.height
    radius: InputStyle.cornerRadius
  }

  // content
  Rectangle {
    anchors.fill: parent

    opacity: root.opacity
    radius: InputStyle.cornerRadius

    ColumnLayout {
      anchors.fill: parent
      spacing: 5 * __dp

      Item {
        id: statusIconContainer

        Layout.fillWidth: true
        Layout.preferredHeight: parent.height * 2 / 6

        // either loading spinner when dialog is in working state or success / failure
        LoadingSpinner {
          id: loadingSpinner

          height: parent.height / 2
          width: parent.height / 2

          anchors.centerIn: parent
          iconSize: width

          speed: 1600
          running: rootstate.state === "working"
        }

        Symbol {
          id: resultIcon

          height: parent.height / 2
          width: parent.height / 2

          anchors.centerIn: parent
          iconSize: width
        }
      }

      Item {
        id: titleContainer

        Layout.fillWidth: true
        Layout.preferredHeight: parent.height * 1 / 6

        Text {
          text: root.titleText

          anchors.fill: parent

          horizontalAlignment: Text.AlignHCenter

          elide: Text.ElideRight
          color: InputStyle.fontColor
          font.pixelSize: InputStyle.fontPixelSizeNormal
        }
      }

      Item {
        id: descriptionContainer

        Layout.fillWidth: true
        Layout.preferredHeight: parent.height * 2 / 6

        Text {
          text: root.descriptionText

          anchors.fill: parent
          horizontalAlignment: Text.AlignHCenter

          elide: Text.ElideRight
          wrapMode: Text.WordWrap
          textFormat: Text.RichText
          color: InputStyle.fontColor
          font.pixelSize: InputStyle.fontPixelSizeNormal

          onLinkActivated: Qt.openUrlExternally( link )
        }
      }

      Item {
        id: closeButtonContainer

        Layout.fillWidth: true
        Layout.preferredHeight: parent.height * 1 / 6

        DelegateButton {
          width: parent.width
          height: InputStyle.rowHeight > parent.height ? parent.height : InputStyle.rowHeight
          text: qsTr( "Close" )
          visible: rootstate.state === "fail"

          onClicked: {
            root.failure()
            root.close()
          }
        }
      }
    }
  }
}
