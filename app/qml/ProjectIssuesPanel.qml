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
import QtGraphicalEffects 1.0
import lc 1.0
import "." // import InputStyle singleton
import "./components"

Item {
  id: root

  visible: false
  property real rowHeight: InputStyle.rowHeight
  property var projectIssuesModel: ListModel {}
  property string projectLoadingLog: ""
  property string headerText: ""

  function reportIssue( layerName, message ) {
    projectIssuesModel.append( { name: layerName, message: message } );
  }

  function clear() {
    projectIssuesModel.clear();
  }

  Keys.onReleased: {
    if (event.key === Qt.Key_Back || event.key === Qt.Key_Escape) {
      event.accepted = true

      if (stackview.depth > 1) {
        // hide about or log panel
        stackview.pop(null)
      } else
        root.visible = false
    }
  }

  StackView {
    id: stackview

    anchors.fill: parent
    initialItem: projectIssuesContentComponent
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

      header: PanelHeader {
        id: header
        height: root.rowHeight
        width: parent.width
        color: InputStyle.clrPanelMain
        rowHeight: InputStyle.rowHeightHeader
        titleText: qsTr("Project Issues")

        onBack: root.visible = false
      }

      ScrollView {
        id: scrollPage
        width: projectIssuesPanel.width
        height: projectIssuesPanel.height - header.height
        contentWidth: availableWidth // to only scroll vertically
        spacing: InputStyle.panelSpacing

        background: Rectangle {
          anchors.fill: parent
          color: InputStyle.panelBackgroundLight
        }

        Column {
          id: settingListContent
          anchors.fill: parent
          spacing: 1

          PanelItem {
            color: InputStyle.panelBackgroundLight
            text: headerText
            bold: true
          }

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
                width: parent.width
                height: row.height
                color: InputStyle.clrPanelMain
                Row {
                  id: row
                   width: parent.width
                   anchors.left: parent.left
                   anchors.top: parent.top
                    Text {
                      id: nameTextItem
                      padding: 5
                      font.pixelSize: InputStyle.fontPixelSizeBig
                      text:  qsTr( name )
                      wrapMode: Text.Wrap
                    }

                    Rectangle {
                        id: seperator
                        width: 3
                        height: parent.height
                        color: "gray"
                    }

                    Text {
                      id: messageTextItem
                      width: parent.width - nameTextItem.width - seperator.width
                      padding: 5
                      font.pixelSize: InputStyle.fontPixelSizeBig
                      text:  qsTr( message )
                      wrapMode: Text.Wrap
                    }
                }
                onHeightChanged: invalidLayersList.height += height;
              }

              onCountChanged: {
                if ( count == 0 )
                  invalidLayersList.height = 0;
              }
            }
          }


          // Debug/Logging
          PanelItem {
            color: InputStyle.panelBackgroundLight
            text: qsTr("QGIS log")
            bold: true
          }

          PanelItem {
            height: qgisLogTextItem.height
            width: parent.width
            Text {
              id: qgisLogTextItem
              width: parent.width
              padding: 5
              text: projectLoadingLog
              wrapMode: Text.Wrap
            }
          }
        }
      }
    }
  }
}
