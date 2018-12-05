import QtQuick 2.0
import QtGraphicalEffects 1.0

DropShadow {
    id: shadow
    horizontalOffset: 0
    verticalOffset: InputStyle.shadowVerticalOffset
    radius: InputStyle.shadowRadius
    samples: InputStyle.shadowSamples
    color: "grey"
    transparentBorder: true

}
