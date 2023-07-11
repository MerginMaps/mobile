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

  Connections{
    target: _hotReload
    function onWatchedSourceChanged() {
      console.log(_qmlPath)
      mainLoader.active = false;
      _hotReload.clearCache();
      mainLoader.setSource("file://"+ _qmlPath+ "/InitialGalleryPage.qml")
      mainLoader.active = true;
    }
  }

  Shortcut {
    sequences: ["Esc", "Back"]
    enabled: stackView.depth > 1
    onActivated: navigateBackAction.trigger()
  }

  Action {
    id: navigateBackAction
    icon.name: stackView.depth > 1 ? "back" : "drawer"
    onTriggered: {
      if (stackView.depth > 1) {
        stackView.pop()
        listView.currentIndex = -1
      } else {
        drawer.open()
      }
    }
  }

  header: ToolBar {
    RowLayout {
      spacing: 20
      anchors.fill: parent

      ToolButton {
        action: navigateBackAction
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
          listView.currentIndex = index
          stackView.push(model.source)
          drawer.close()
        }
      }

      model: ListModel {
        ListElement { title: "ToolTip"; source: "qml/pages/ToolTipPage.qml" }
        ListElement { title: "CheckBox"; source: "qml/pages/CheckBoxPage.qml" }
      }

      ScrollIndicator.vertical: ScrollIndicator { }
    }
  }

  StackView {
    id: stackView
    anchors.fill: parent

    initialItem: Loader {
      id: mainLoader
      anchors.fill: parent
      source: "file://"+ _qmlPath+ "/InitialGalleryPage.qml"
      scale: 1.0
    }
  }
}
