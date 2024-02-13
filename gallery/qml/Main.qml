

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/
import QtCore
import QtQuick
import QtQuick.Layouts
import QtQuick.Controls

ApplicationWindow {
  id: window
  width: 400
  height: 700
  visible: true
  title: "Gallery"

  property string currentPageSource: "InitialGalleryPage.qml"

  Connections {
    target: __isMobile ? null : _hotReload
    enabled: !__isMobile
    function onWatchedSourceChanged() {
      mainLoader.active = false
      _hotReload.clearCache()
      mainLoader.setSource("file://" + _qmlWrapperPath + currentPageSource)
      mainLoader.active = true
      console.log( new Date().toLocaleTimeString().split(' ')[0] + " ------ App reloaded 🔥 ------ ")
    }
  }

  Settings {
    // start window where it was closed last time
    property alias x: window.x
    property alias y: window.y
    property alias width: window.width
    property alias height: window.height
  }

  Shortcut {
    sequences: ["Esc", "Back"]
    enabled: stackView.depth > 1
    onActivated: navigateBackAction.trigger()
  }

  Action {
    id: navigateBackAction
    onTriggered: drawer.open()
  }

  header: ToolBar {
    RowLayout {
      spacing: 20
      anchors.fill: parent

      ToolButton {
        action: navigateBackAction
        text: "☰"
      }

      Label {
        id: titleLabel
        text: listView.currentItem ? listView.currentItem.text : "Gallery"
        font.pixelSize: 20
        elide: Label.ElideRight
        horizontalAlignment: Qt.AlignHCenter
        verticalAlignment: Qt.AlignVCenter
        Layout.fillWidth: true
      }

      ToolButton {
        onClicked: window.close()
        text: "⨯"
      }
    }
  }

  Drawer {
    id: drawer
    width: Math.min(window.width, window.height) / 3 * 2
    height: window.height
    interactive: stackView.depth === 1

    ListView {
      id: listView

      focus: true
      currentIndex: -1
      anchors.fill: parent

      delegate: ItemDelegate {
        width: listView.width
        text: model.title
        highlighted: ListView.isCurrentItem
        onClicked: {
          window.currentPageSource = model.source
          listView.currentIndex = index
          if (__isMobile)
            stackView.push("qrc:/qml/pages/" + model.source)
          else
            stackView.push("file://" + _qmlWrapperPath + model.source)
          stackView.pop()
          drawer.close()
        }
      }

      model: ListModel {
        ListElement {
          title: "Initial"
          source: "InitialGalleryPage.qml"
        }
        ListElement {
          title: "Style"
          source: "StylePage.qml"
        }
        ListElement {
          title: "Icons"
          source: "IconsPage.qml"
        }
        ListElement {
          title: "Images"
          source: "ImagesPage.qml"
        }
        ListElement {
          title: "Buttons"
          source: "ButtonsPage.qml"
        }
        ListElement {
          title: "Editors"
          source: "EditorsPage.qml"
        }
        ListElement {
          title: "Inputs"
          source: "InputsPage.qml"
        }
        ListElement {
          title: "Calendar"
          source: "CalendarPage.qml"
        }
        ListElement {
          title: "Checks"
          source: "ChecksPage.qml"
        }
        ListElement {
          title: "Notifications"
          source: "NotificationPage.qml"
        }
        ListElement {
          title: "Drawers"
          source: "DrawerPage.qml"
        }
        ListElement {
          title: "Pages"
          source: "PagesPage.qml"
        }
        ListElement {
          title: "Photos"
          source: "PhotosPage.qml"
        }
        ListElement {
          title: "Map"
          source: "MapPage.qml"
        }
        ListElement {
          title: "Toolbars"
          source: "ToolbarPage.qml"
        }
        ListElement {
          title: "Project items"
          source: "ProjectItemsPage.qml"
        }
        ListElement {
          title: "Misc"
          source: "MiscPage.qml"
        }
        ListElement {
          title: "Onboarding"
          source: "OnboardingPage.qml"
        }
        ListElement {
          title: "Feature form"
          source: "FormPage.qml"
        }
      }

      ScrollIndicator.vertical: ScrollIndicator {}
    }
  }

  StackView {
    id: stackView
    anchors.fill: parent

    initialItem: Loader {
      id: mainLoader
      source: (__isMobile ? "qrc:/qml/pages/" : ("file://" + _qmlWrapperPath)) + currentPageSource
      scale: 1.0
    }
  }
}
