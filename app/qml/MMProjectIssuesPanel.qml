
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
import "." // import InputStyle singleton
import "./components"

Item {
  id: root

  visible: false
  property real rowHeight: InputStyle.rowHeight
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

  // StackView {
  //   id: stackview

  //   anchors.fill: parent
  //   initialItem: projectIssuesContentComponent
  // }

  Rectangle {
    id: roundedRect

    anchors.fill: parent
    color: __style.whiteColor

    ColumnLayout {
      anchors.fill: parent

      MMHeader {
        id: header

        Layout.fillWidth: true
        height: root.rowHeight
        width: parent.width
        onBackClicked: root.visible = false

        title: qsTr("Project issues")
        titleFont: __style.t3
      }

      ScrollView {
        id: scrollPage

        Layout.fillWidth: true
        Layout.fillHeight: true

        Column {
          id: settingListContent
          anchors.fill: parent
          spacing: 1

          PanelItem {
            id: invalidLayersList
            height: 0

            ListView {
              id: invalidLayersListView
              anchors.fill: parent

              model: projectIssuesModel
              spacing: 3
              delegate: PanelItem {
                anchors.margins: 5
                width: ListView.view.width
                height: row.height
                color: InputStyle.clrPanelMain
                Column {
                  id: row
                  width: parent.width
                  anchors.left: parent.left
                  anchors.top: parent.top
                  Text {
                    id: nameTextItem
                    width: parent.width
                    padding: 5
                    font.pixelSize: __style.t1
                    text: title
                    color: __style.forestColor
                    wrapMode: Text.Wrap
                  }

                  Text {
                    id: messageTextItem
                    width: parent.width
                    padding: 10
                    //font.pixelSize: __style.t3
                    text: message
                    wrapMode: Text.Wrap
                    font.pixelSize: __style.p5
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

          // Debug/Logging
          PanelItem {
            height: qgisLogTextHeader.height
            width: parent.width
            Text {
              id: qgisLogTextHeader
              width: parent.width
              padding: 5
              text: qsTr("QGIS log")
              font.pixelSize: __style.t1
              color:  __style.forestColor
            }
          }

          PanelItem {
            height: qgisLogTextItem.height
            width: parent.width
            Text {
              id: qgisLogTextItem
              width: parent.width
              padding: 10
              text: projectLoadingLog
              wrapMode: Text.Wrap
              font.pixelSize: __style.p5
              color:  __style.nightColor
            }
          }
        }
      }

    }
  }

  Component {
    id: projectIssuesContentComponent

    Page {
      id: projectIssuesPanel
      padding: 0

      background: Rectangle {
        anchors.fill: parent
        color: InputStyle.clrPanelMain
      }

      MMHeader {
        id: header

        height: root.rowHeight
        width: parent.width
        rightMarginShift: 0
        backVisible: false

        title: qsTr("Project issues")
        titleFont: __style.t2
      }

      // header: PanelHeader {
      //   id: header
      //   height: root.rowHeight
      //   width: parent.width
      //   color: InputStyle.clrPanelMain
      //   rowHeight: InputStyle.rowHeightHeader
      //   titleText: qsTr("Project Issues")

      //   onBack: root.visible = false
      // }

      ScrollView {
        id: scrollPage
        width: projectIssuesPanel.width
        height: projectIssuesPanel.height - header.height
        contentWidth: availableWidth // to only scroll vertically
        spacing: InputStyle.panelSpacing

        Column {
          id: settingListContent
          anchors.fill: parent
          spacing: 1

          PanelItem {
            id: invalidLayersList
            height: 0

            ListView {
              id: invalidLayersListView
              anchors.fill: parent

              model: projectIssuesModel
              spacing: 3
              delegate: PanelItem {
                anchors.margins: 5
                width: ListView.view.width
                height: row.height
                color: InputStyle.clrPanelMain
                Column {
                  id: row
                  width: parent.width
                  anchors.left: parent.left
                  anchors.top: parent.top
                  Text {
                    id: nameTextItem
                    width: parent.width
                    padding: 5
                    font.pixelSize: InputStyle.fontPixelSizeBig
                    text: title
                    color: InputStyle.fontColor
                    wrapMode: Text.Wrap
                  }

                  Text {
                    id: messageTextItem
                    width: parent.width
                    padding: 10
                    font.pixelSize: InputStyle.fontPixelSizeNormal
                    text: message
                    wrapMode: Text.Wrap
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

          // Debug/Logging
          PanelItem {
            height: qgisLogTextHeader.height
            width: parent.width
            Text {
              id: qgisLogTextHeader
              width: parent.width
              padding: 5
              text: qsTr("QGIS log")
              font.pixelSize: InputStyle.fontPixelSizeBig
              color: InputStyle.fontColor
            }
          }

          PanelItem {
            height: qgisLogTextItem.height
            width: parent.width
            Text {
              id: qgisLogTextItem
              width: parent.width
              padding: 10
              text: projectLoadingLog
              wrapMode: Text.Wrap
            }
          }
        }
      }//
    }
  }
}
