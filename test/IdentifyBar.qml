import QtQuick 2.0
import QtGraphicalEffects 1.0

Item {
    id: bar

    height: ir.height

    property real shadowSize: 4
    property alias content: contentItem

    state: "off"
    states: [
        State {
            name: "on"
            PropertyChanges { target: bar; y: parent.height - bar.height }
        },
        State {
            name: "off"
            PropertyChanges { target: bar; y: parent.height }
        }
    ]

    transitions: Transition {
        NumberAnimation { properties: "y"; easing.type: Easing.InOutQuad; duration: 200 }
    }

    // identify results
    Item {
        id: ir
        height: contentItem.height + shadowSize

        function identify(x, y) {
            res = canvas.engine.identify(x,y) // res is QVariant-based model
        }

        anchors.left: parent.left
        anchors.right: parent.right
        anchors.bottom: parent.bottom

        IdentifyContentItem {
            id: contentItem
            y: shadowSize
            width: ir.width
        }

    }

    DropShadow {
        source: ir
        anchors.fill: source
        verticalOffset: -shadowSize
        radius: shadowSize
        samples: 16
        color: "#50000000"
    }

}

