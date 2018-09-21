pragma Singleton
import QtQuick 2.0

import QgsQuick 0.1 as QgsQuick

QtObject {

    property color clrPanelMain: "white"
    property color clrPanelHighlight: "yellow"
    property color clrPanelBackground: "#7b909d"

    property real fontPixelSizeNormal: QgsQuick.Utils.dp * 30
    property real fontPixelSizeBig: QgsQuick.Utils.dp * 50

    property real panelSpacing: QgsQuick.Utils.dp * 5
    property real panelOpacity: 0.7
}
