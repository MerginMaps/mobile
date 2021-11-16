import QtQuick 2.0
import QtQuick.Templates 2.0 as T
import QgsQuick 0.1 as QgsQuick

T.Switch {
  id: control
  property color bgndColorActive: "green"
  property color bgndColorInactive: "grey"
  property color handleColor: "white"
  property bool isReadOnly: false

  implicitHeight: 60 * QgsQuick.Utils.dp
  implicitWidth: 2 * height

  signal switchChecked(bool isChecked)

  onCheckedChanged: control.switchChecked(checked)

  background: Rectangle {
    color: control.isReadOnly || !control.checked ? control.bgndColorInactive : control.bgndColorActive
    radius: 20 * QgsQuick.Utils.dp
  }
  indicator: Rectangle {
    color: control.handleColor
    height: parent.height * 0.66
    radius: 20 * QgsQuick.Utils.dp
    width: height
    x: {
      let actualPosition = control.visualPosition * (control.width - width);
      if (control.checked)
        // limit maximum position
        return Math.min(actualPosition, control.width * 0.55);
      else
        // limit minimum position
        return Math.max(actualPosition, control.width * 0.13);
    }
    y: (control.height - height) / 2

    Behavior on x  {
      enabled: !control.pressed

      SmoothedAnimation {
        velocity: 200
      }
    }
  }
}
