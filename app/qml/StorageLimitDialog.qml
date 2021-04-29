import QtQuick 2.13
import QtQuick.Controls 2.13
import QtQuick.Dialogs 1.1
import QtQuick.Layouts 1.12

import "./components"

Dialog {
  property var uploadSize
  property string titleText
  property int diskUsage: __merginApi.userInfo.diskUsage
  property int storageLimit: __merginApi.userInfo.storageLimit
  property string planAlias: __merginApi.subscriptionInfo.planAlias
  property real fieldHeight: InputStyle.rowHeight

  signal openSubscriptionPlans

  id: root
  visible: false
  modal: true
  closePolicy: Popup.CloseOnEscape | Popup.CloseOnPressOutside
  onAccepted: root.close()
  anchors.centerIn: parent
  margins: InputStyle.outerFieldMargin

  background: Rectangle {
    anchors.fill: parent
    color: "white"
  }

  contentItem: ColumnLayout {
    anchors.margins: InputStyle.outerFieldMargin

    Text {
      id: title
      anchors.topMargin: InputStyle.panelMargin
      height: root.fieldHeight
      text: root.titleText
      color: InputStyle.fontColor
      font.pixelSize: InputStyle.fontPixelSizeTitle
      font.bold: true
      verticalAlignment: Text.AlignVCenter
      horizontalAlignment: Text.AlignHCenter
    }

    TextWithIcon {
      width: parent.width
      height: root.fieldHeight
      source: 'sync.svg'
      text: qsTr("Data to sync: %1").arg(__inputUtils.bytesToHumanSize(
                                           uploadSize))
    }

    Row {
      width: parent.width
      height: root.fieldHeight
      Item {
        width: root.fieldHeight
        height: root.fieldHeight

        CircularProgressBar {
          id: storageIcon
          width: parent.width * 0.6
          anchors.centerIn: parent
          height: width
          value: root.diskUsage / root.storageLimit
        }
      }

      Text {
        id: textItem
        height: root.fieldHeight
        verticalAlignment: Text.AlignVCenter
        font.pixelSize: InputStyle.fontPixelSizeNormal
        color: InputStyle.fontColor
        text: qsTr("Using %1 / %2").arg(__inputUtils.bytesToHumanSize(
                                          diskUsage)).arg(
                __inputUtils.bytesToHumanSize(storageLimit))
      }
    }

    TextWithIcon {
      width: parent.width
      height: root.fieldHeight
      source: 'edit.svg'
      text: qsTr("Plan: %1").arg(planAlias)
      visible: __merginApi.apiSupportsSubscriptions
    }

    Text {
      id: planLink
      height: root.fieldHeight
      Layout.fillWidth: true
      horizontalAlignment: Qt.AlignHCenter
      verticalAlignment: Qt.AlignVCenter
      font.pixelSize: InputStyle.fontPixelSizeNormal
      font.underline: true
      color: InputStyle.fontColor
      text: qsTr("Find a plan that fits you better!")
      visible: __merginApi.apiSupportsSubscriptions

      MouseArea {
        anchors.fill: parent
        onClicked: root.openSubscriptionPlans()
      }
    }

    Item {
      id: spacer
      height: InputStyle.fontPixelSizeTitle
      width: parent.width
    }

    DelegateButton {
      text: "OK"
      onClicked: root.close()
      height: root.fieldHeight
      width: parent.width
      Layout.fillWidth: true
    }
  }
}
