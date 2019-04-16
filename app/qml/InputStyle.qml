pragma Singleton
import QtQuick 2.0

import QgsQuick 0.1 as QgsQuick

QtObject {

    // Primary colors
    property color fontColor: "#006146"
    property color fontColorBright: "#679D70"
    property color panelBackground2: "#C6CCC7"

    // Secondary colors
    property color clrPanelMain: "white"
    property color clrPanelBackground: fontColor
    property color panelBackgroundDark: "#B3B3B3"
    property color panelBackgroundLight: "#E6E6E6"

    property color highlightColor: "#FD9626"
    property color softRed: "#FC9FB1"
    property color softOrange: "#FDD7B1"
    property color softGreen: "#32AA3A"

    property real rado2Lutra: 0.75 // TODO delete this, but later!

    property real fontPixelSizeSmall: QgsQuick.Utils.dp * 20 * rado2Lutra
    property real fontPixelSizeNormal: QgsQuick.Utils.dp * 28 * rado2Lutra
    property real fontPixelSizeTitle: QgsQuick.Utils.dp * 32 * rado2Lutra

    property int panelMargin: scale(30)
    property real rowHeight: scale(64)
    property real rowHeightHeader: scale(64)
    property real scaleBarHeight: fontPixelSizeSmall * 3 //according scaleBar text

    property real panelSpacing: QgsQuick.Utils.dp * 5
    property real shadowVerticalOffset: -2 * QgsQuick.Utils.dp
    property real shadowRadius: 8 * QgsQuick.Utils.dp
    property real shadowSamples: 12
    property real panelOpacity: 1
    property real lowHighlightOpacity: 0.4
    property real highHighlightOpacity: 0.8

    property real refWidth: 640
    property real refHeight: 1136
    property real realWidth
    property real realHeight

    // used in scaling functions for high DPI screens
    property real deviceRatio: 1

    // icons
    property var cameraIcon: "qrc:/add_photo.svg"
    property var removeIcon: "qrc:/trash.svg"

    // Scaling function using QgsQuick screenDensity and deviceRatio
    property var scale: function scale(size) {
        return size * QgsQuick.Utils.dp
    }

    // Scaling function for fonts using deviceRatio
    property var scaleFontPointSize: function scaleParam(size) {
        return size * deviceRatio;
    }
}
