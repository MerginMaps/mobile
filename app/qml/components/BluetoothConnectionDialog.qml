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

  property string titleText: "Connecting to device A"
  property string descriptionText: "Await pairing shortly"

  // TODO: assign AbstractPositionProvider and listen to its different states! :)

  StateGroup {
    id: rootstate

    states: [
      State {
        name: "working"
//        PropertyChanges { target: loadingSpinner; visible: true }
//        PropertyChanges { target: resultIcon; visible: false }
        PropertyChanges { target: loadingSpinner; opacity: 1.0 }
        PropertyChanges { target: resultIcon; opacity: 0.0 }
      },
      State {
        name: "success"
//        PropertyChanges { target: loadingSpinner; visible: false }
        PropertyChanges { target: resultIcon; source: InputStyle.yesIcon }
//        PropertyChanges { target: resultIcon; visible: true }
        PropertyChanges { target: loadingSpinner; opacity: 0.0 }
        PropertyChanges { target: resultIcon; opacity: 1.0 }
      },
      State {
        name: "fail"
//        PropertyChanges { target: loadingSpinner; visible: false }
        PropertyChanges { target: resultIcon; source: InputStyle.noIcon }
//        PropertyChanges { target: resultIcon; visible: true }
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
  onAccepted: {
    // either "ok" or "retry"
    if ( rootstate.state === "success" )
    {
      console.log( "Ok after success" )
      rootstate.state = "fail"
    }
    else if ( rootstate.state === "fail" )
    {
      console.log( "Retry after failure" )
      rootstate.state = "working"
      root.open()
    }
  }

  onRejected: {
    // "cancel" button
    if ( rootstate.state === "working" )
    {
      // cancel connection process
      console.log( "Cancel connection process" )
      rootstate.state = "success"
    }
    else if ( rootstate.state === "fail" )
    {
      // not interested in trying again
      console.log( "Cancel after failure" )
      root.close()
    }
  }

  modal: true

  Timer {
    property bool turn: false

    running: true
    repeat: true
    interval: 4000

    onTriggered: {
      if ( rootstate.state === "working" )
      {
        if ( turn )
          rootstate.state = "success"
        else
          rootstate.state = "fail"
        turn = !turn
      }
      else
        rootstate.state = "working"
    }
  }

  enter: Transition {
    NumberAnimation { property: "opacity"; from: 0.0; to: 1.0; duration: 200 }
  }
  exit: Transition {
    NumberAnimation { property: "opacity"; from: 1.0; to: 0.0; duration: 200 }
  }

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

          running: rootstate.state === "working"
//          visible: rootstate.state === "working"
        }

        Symbol {
          id: resultIcon

          height: parent.height / 2
          width: parent.height / 2

          anchors.centerIn: parent
//          visible: rootstate.state !== "working"

//          source: rootstate.state === "success" ? InputStyle.yesIcon : InputStyle.noIcon
          iconSize: width
        }
      }

      Item {
        id: titleContainer

        Layout.fillWidth: true
        Layout.preferredHeight: parent.height * 1 / 6

        Text {
          text: root.titleText

          anchors.centerIn: parent

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

          anchors.centerIn: parent

          elide: Text.ElideRight
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
