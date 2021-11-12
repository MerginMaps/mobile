import QtQuick 2.0
import ".."
import "../components"

Rectangle {
  color: '#ff4f4f'
  height: InputStyle.rowHeight
  visible: __merginApi.subscriptionInfo ? __merginApi.subscriptionInfo.actionRequired : false

  TextWithIcon {
    width: parent.width
    fontColor: 'white'
    bgColor: '#ff4f4f'
    iconColor: 'white'
    linkColor: 'white'
    source: InputStyle.exclamationTriangleIcon
    text: qsTr("Your attention is required. Please visit the %1Mergin dashboard%2.")
               .arg("<a href='" + __inputHelp.merginDashboardLink + "'>")
               .arg("</a>")
  }

  MouseArea {
    anchors.fill: parent
    onClicked: Qt.openUrlExternally( __inputHelp.merginDashboardLink )
  }
}
