pragma Singleton
import QtQuick 2.0

import QgsQuick 0.1 as QgsQuick

QtObject {

    property color clrPanelMain: "white"
    property color clrPanelHighlight: "yellow"
    property color clrPanelBackground: "#7b909d"

    property real fontPixelSizeNormal: QgsQuick.Utils.dp * 30
    property real fontPixelSizeBig: QgsQuick.Utils.dp * 50
    property color fontColor: "#0b5036"
    property color panelBackground2: "#EEF0F0"

    property real buttonSize: 48 * 20

    property real panelSpacing: QgsQuick.Utils.dp * 5
    property real panelOpacity: 0.7

    property real pixelDensity: 1

    property var scale: function scaleParam(size) {
        console.log("!!!!!!!pixelDensity", pixelDensity)
        return size * ( QgsQuick.Utils.dp/ pixelDensity);
    }
}
