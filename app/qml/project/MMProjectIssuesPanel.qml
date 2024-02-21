
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
import Qt5Compat.GraphicalEffects
import lc 1.0
import ".."
import "../components"

Item {
  id: root

  visible: false
  property var projectIssuesModel: ListModel {}
  property string projectLoadingLog: ""

  function reportIssue(title, message) {
    projectIssuesModel.append( { title: title, message: message } )
  }

  function clear() {
    projectIssuesModel.clear()
  }

  Keys.onReleased: function (event) {
    if (event.key === Qt.Key_Back || event.key === Qt.Key_Escape) {
      event.accepted = true

      if (stackview.depth > 1) {
        // hide about or log panel
        stackview.pop(null)
      } else
        root.visible = false
    }
  }

  Rectangle {
    id: roundedRect

    anchors.fill: parent
    color: __style.lightGreenColor

    ColumnLayout {
      anchors.fill: parent
      width: parent.width
      spacing: 40 * __dp

      MMHeader {
        id: header

        Layout.fillWidth: true
        width: parent.width
        onBackClicked: root.visible = false

        title: qsTr("Project issues")
        titleFont: __style.t3
      }

      ScrollView {
        id: scrollPage

        Layout.fillWidth: true
        Layout.fillHeight: true
        Layout.leftMargin: __style.pageMargins
        Layout.rightMargin: __style.pageMargins
        Layout.maximumWidth: __style.maxPageWidth
        Layout.alignment: Qt.AlignHCenter

        contentWidth: availableWidth
        Layout.preferredHeight: settingListContent.childrenRect.height

        Column {
          id: settingListContent
          anchors.fill: parent
          spacing: 1

          PanelItem {
            id: invalidLayersList
            color: roundedRect.color
            height: 0

            ListView {
              id: invalidLayersListView

              anchors.fill: parent
              model: projectIssuesModel
              spacing: 3

              delegate: PanelItem {
                color: __style.lightGreenColor
                anchors.margins: 5
                width: ListView.view.width
                height: row.height

                Column {
                  id: row

                  width: parent.width
                  anchors.left: parent.left
                  anchors.top: parent.top

                  Text {
                    id: nameTextItem
                    width: parent.width
                    padding: 5
                    font: __style.t1
                    text: title
                    color: __style.forestColor
                    wrapMode: Text.Wrap
                  }

                  Text {
                    id: messageTextItem
                    width: parent.width
                    padding: 10
                    text: message
                    wrapMode: Text.Wrap
                    font: __style.p5
                    color:  __style.nightColor
                  }
                }

                onHeightChanged: invalidLayersList.height += height;
              }

              onCountChanged: {
                if ( count === 0 )
                  invalidLayersList.height = 0;
              }
            }
          }

          PanelItem {
            height: qgisLogTextHeader.height
            width: parent.width
            color: roundedRect.color

            Text {
              id: qgisLogTextHeader
              width: parent.width
              padding: 5
              text: qsTr("QGIS log")
              font: __style.t1
              color:  __style.forestColor
            }
          }

          PanelItem {
            height: qgisLogTextItem.height
            width: parent.width
            color: roundedRect.color

            Text {
              id: qgisLogTextItem
              width: parent.width
              padding: 10
              text: projectLoadingLog
              wrapMode: Text.Wrap
              font: __style.p5
              color:  __style.nightColor
            }
          }
        }
      }
    }
  }
}
