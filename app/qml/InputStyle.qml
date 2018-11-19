pragma Singleton
import QtQuick 2.0

import QgsQuick 0.1 as QgsQuick

QtObject {

    property color clrPanelMain: "white"
    property color clrPanelHighlight: "yellow"
    property color clrPanelBackground: fontColor
    property color clrPanelBackground2: "#EAEAEA"

    property color highlightColor: "#FB831F"
    property color softRed: "#FC9FB1"
    property color softOrange: "#FDD7B1"
    property color softGreen: "#32AA3A"

    property real fontPixelSizeSmaller: QgsQuick.Utils.dp * 15
    property real fontPixelSizeSmall: QgsQuick.Utils.dp * 20
    property real fontPixelSizeNormal: QgsQuick.Utils.dp * 28 / 2
    property real fontPixelSizeTitle: QgsQuick.Utils.dp * 32 / 2
    property real fontPixelSizeBig: QgsQuick.Utils.dp * 50

    property int panelMargin: scale(16)

    property color fontColor: "#0b5036"
    property color panelBackground2: "#EEF0F0"

    property real rowHeight: scale(64)
    property real rowHeightHeader: scale(64)

    property real panelSpacing: QgsQuick.Utils.dp * 5
    property real panelOpacity: 1

    property real refWidth: 640
    property real refHeight: 1136
    property real realWidth
    property real realHeight

    // used in scaling functions for high DPI screens
    property real deviceRatio: 1

    // Scaling function using QgsQuick screenDensity and deviceRatio
    property var scale: function scale(size) {
        return size * QgsQuick.Utils.dp
    }

    // Scaling function for fonts using deviceRatio
    property var scaleFontPointSize: function scaleParam(size) {
        return size * deviceRatio;
    }
}
