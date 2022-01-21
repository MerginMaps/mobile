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

  property string titleText: ""
  property string descriptionText: rootstate.state === "working" ? qsTr( "You might be asked to pair your device during this process." ) : ""

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
        to: "*"

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

  // Handle dialog buttons
//  onAccepted: {
//    // either "ok" or "retry"
//    if ( rootstate.state === "success" )
//    {
//      console.log( "Ok after success" )
//      rootstate.state = "fail"
//    }
//    else if ( rootstate.state === "fail" )
//    {
//      console.log( "Retry after failure" )
//      rootstate.state = "working"
//      root.open()
//    }
//  }

//  onRejected: {
//    // "cancel" button
//    if ( rootstate.state === "working" )
//    {
//      // cancel connection process
//      console.log( "Cancel connection process" )
//      rootstate.state = "success"
//    }
//    else if ( rootstate.state === "fail" )
//    {
//      // not interested in trying again
//      console.log( "Cancel after failure" )
//      root.close()
//    }
//  }

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

      Item {
        id: statusIconContainer

        Layout.fillWidth: true
        Layout.preferredHeight: parent.height * 3 / 6

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
        Layout.preferredHeight: parent.height * 1 / 6

        Text {
          text: root.descriptionText

          anchors.fill: parent

          horizontalAlignment: Text.AlignHCenter

          elide: Text.ElideRight
          wrapMode: Text.WordWrap
          color: InputStyle.fontColor
          font.pixelSize: InputStyle.fontPixelSizeNormal
        }
      }

      Item {
        id: buttonsContainer
        Layout.fillWidth: true
        Layout.preferredHeight: parent.height * 1 / 6
      }
    }
  }
}
