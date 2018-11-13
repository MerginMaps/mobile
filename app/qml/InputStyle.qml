pragma Singleton
import QtQuick 2.0

import QgsQuick 0.1 as QgsQuick

QtObject {

    property color clrPanelMain: "white"
    property color clrPanelHighlight: "yellow"
    property color clrPanelBackground: "#7b909d"

    property real fontPixelSizeNormal: QgsQuick.Utils.dp * 30
    property real fontPixelSizeBig: QgsQuick.Utils.dp * 50
    property real fontPointSizeBig: 24
    property real fontPointSizeSmall: 12

    property color fontColor: "#0b5036"
    property color panelBackground2: "#EEF0F0"

    property real buttonSize: 96

    property real panelSpacing: QgsQuick.Utils.dp * 5
    property real panelOpacity: 0.7

    // used in scaling functions for high DPI screens
    property real deviceRatio: 1

    // Scaling function using QgsQuick screenDensity and deviceRatio
    property var scale: function scaleParam(size) {
        return size * QgsQuick.Utils.dp * deviceRatio;
    }

    // Scaling function for fonts using deviceRatio
    property var scaleFontPointSize: function scaleParam(size) {
        return size * deviceRatio;
    }
}
