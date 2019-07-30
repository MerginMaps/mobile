import QtQuick 2.7
import QtQuick.Controls 2.2
import QtGraphicalEffects 1.0

ProgressBar {
    id: loadingIndicator
    anchors.top: parent.top
    value: 0
    height: 5
    width: parent.width
    clip: true

    property int easingType: Easing.InOutQuad

    function toggle(value) {
      loadingIndicatorAnimation.running = value
      visible = value
    }

    background: Rectangle {
        implicitWidth: loadingIndicator.width
        implicitHeight: loadingIndicator.height
        border.color: InputStyle.panelBackgroundDarker
        radius: 0
    }
    contentItem: Item {
        implicitWidth: loadingIndicator.width
        implicitHeight: loadingIndicator.height

        Rectangle {
            id: bar
            width: loadingIndicator.visualPosition * parent.width
            height: parent.height
            radius: 0
        }

        LinearGradient {
            anchors.fill: bar
            start: Qt.point(0, 0)
            end: Qt.point(bar.width, 0)
            source: bar
            gradient: Gradient {
                GradientStop { position: 0.0; color: InputStyle.fontColor }
                GradientStop { id: grad; position: 0.5; color: Qt.lighter(InputStyle.fontColor, 2) }
                GradientStop { position: 1.0; color: InputStyle.fontColor }
            }
            PropertyAnimation {
                target: grad
                property: "position"
                from: 0.0
                to: 1.0
                duration: 1000
                running: true
                loops: Animation.Infinite
                easing.type: easingType
            }
        }
        LinearGradient {
            anchors.fill: bar
            start: Qt.point(0, 0)
            end: Qt.point(0, bar.height)
            source: bar
            gradient: Gradient {
                GradientStop { position: 0.0; color: Qt.rgba(0,0,0,0) }
                GradientStop { position: 0.5; color: Qt.rgba(1,1,1,0.3) }
                GradientStop { position: 1.0; color: Qt.rgba(0,0,0,0.05) }
            }
        }
    }
    PropertyAnimation {
        id: loadingIndicatorAnimation
        target: loadingIndicator
        property: "value"
        from: 1
        to: 1
        running: true
        loops: Animation.Infinite
        easing.type: easingType
    }
}
