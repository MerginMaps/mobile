import QtQuick

MouseArea {
  id: mouseArea
  property alias clicked: onClicked
  signal singleClicked()

  onClicked: {
    if ( !mouseArea.enabled ) return;
    mouseArea.enabled = false;
    singleClicked()

    timer.start()
  }

  Timer {
    id: timer
    interval: 500
    repeat: false
    onTriggered: mouseArea.enabled = true
  }
}
