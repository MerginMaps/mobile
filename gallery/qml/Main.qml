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

  property int sizeIndex: 0
  property var mobileSizes: [ [375,667], [360,800] ]
  property var tabletSizes: [ [768,1024], [601,962] ]

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

  Action {
    id: showGridAction
    onTriggered: grid.toggle()
  }

  Action {
    id: switchMobileAction
    onTriggered: {
     sizeIndex++
     let id = sizeIndex % mobileSizes.length
     let size = mobileSizes[id]
     window.width = size[0]
     window.height = size[1]
    }
  }

  Action {
    id: switchTabletAction
    onTriggered: {
     sizeIndex++
     let id = sizeIndex % tabletSizes.length
     let size = tabletSizes[id]
     window.width = size[0]
     window.height = size[1]
    }
  }

  Action {
    id: switchOrientationAction
    onTriggered: {
     let s = window.width
     window.width = window.height
     window.height = s
    }
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
        horizontalAlignment: Qt.AlignLeft
        verticalAlignment: Qt.AlignVCenter
        Layout.fillWidth: true
      }

      ToolButton {
        action: switchOrientationAction
        text: "O"
      }

      ToolButton {
        action: switchMobileAction
        text: "M"
      }

      ToolButton {
        action: switchTabletAction
        text: "T"
      }

      ToolButton {
        action: showGridAction
        text: "Grid"
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
          title: "Home"
          source: "InitialGalleryPage.qml"
        }
        ListElement {
          title: "Components"
          source: "ComponentsPage.qml"
        }
        ListElement {
          title: "List delegate"
          source: "ListDelegatePage.qml"
        }
        ListElement {
          title: "Colors"
          source: "ColorsPage.qml"
        }
        ListElement {
          title: "Texts (Fonts)"
          source: "TextsPage.qml"
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
          title: "Photo cards page"
          source: "PhotoCardPage.qml"
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
          title: "Project delegate"
          source: "ProjectItemsPage.qml"
        }
        ListElement {
          title: "Misc"
          source: "MiscPage.qml"
        }
        ListElement {
          title: "Account and Onboarding"
          source: "OnboardingPage.qml"
        }
        ListElement {
          title: "Feature form"
          source: "FormPage.qml"
        }
        ListElement {
          title: "Sandbox"
          source: "SandboxPage.qml"
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

  Item {
    id: grid

    function toggle() {
      if (!safeArea.visible)
      {
        safeArea.visible = true
      } else {
        if ( gridMargins.visible ) {
          safeArea.visible = false
          gridMargins.visible = false
        } else {
          gridMargins.visible = true
        }
      }
    }

    anchors.fill: parent
    property real bw: 2
    property real step: 20

    Rectangle {
      id: safeArea

      visible: false
      color: "transparent"
      border.width: grid.bw
      border.color: "lightblue"
      width: parent.width - __style.safeAreaLeft - __style.safeAreaRight + 2 * grid.bw
      height: parent.height - __style.safeAreaTop - __style.safeAreaBottom + 2 * grid.bw
      x: __style.safeAreaLeft - grid.bw
      y: __style.safeAreaTop - grid.bw
    }


    Item {
      id: gridMargins
      visible: false
      anchors.fill: parent

      Repeater {
        model: 9

        Rectangle {
          id: gridRectangle

          required property int index
          property int i: index + 1
          property color fgColor: {
            if (! (index % 4) ) {
              return "lightpink"
            } else if (index % 2) {
              return "navajowhite"
            } else {
              return "lightgreen"
            }
          }

          color: "transparent"
          border.width: grid.bw
          border.color: fgColor
          width: parent.width - __style.safeAreaLeft - __style.safeAreaRight - i * 2 * grid.step * __dp + 2 * grid.bw
          height: parent.height - __style.safeAreaTop - __style.safeAreaBottom - i *  2 * grid.step * __dp + 2 * grid.bw
          x: __style.safeAreaLeft + i * grid.step * __dp - grid.bw
          y: __style.safeAreaTop + i * grid.step * __dp - grid.bw

          Text {
            text: (i * grid.step).toFixed(0)
            color: gridRectangle.fgColor
            x: 5
            y: -10
            style: Text.Outline
            styleColor: "white"
          }
        }
      }
    }
  }
}
