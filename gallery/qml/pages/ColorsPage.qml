/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

import QtQuick
import QtQuick.Controls
import QtQuick.Controls.Basic

import "../../app/qml/components"
import "../"

ScrollView {
  Column {
    id: page

    padding: 20
    spacing: 5

    property int rectSize: 10

    GroupBox {
      title: "Colors - primary palette"
      background: Rectangle {
        color: "white"
        border.color: "gray"
      }
      label: Text {
        color: "black"
        text: parent.title
        padding: 5
      }


      Grid {
        columns: 3
        spacing: 20
        anchors.fill: parent
        ColorBox {
          text: "grassColor"
          color: __style.grassColor
        }
        ColorBox {
          text: "forestColor"
          color: __style.forestColor
        }
        ColorBox {
          text: "nightColor"
          color: __style.nightColor
        }
        ColorBox {
          text: "polarColor"
          color: __style.polarColor
        }
        ColorBox {
          text: "transparentColor"
          color: __style.transparentColor
        }
      }
    }

    GroupBox {
      title: "Colors - additional colors"
      background: Rectangle {
        color: "white"
        border.color: "gray"
      }
      label: Text {
        color: "black"
        text: parent.title
        padding: 5
      }

      Grid {
        columns: 3
        spacing: 20
        anchors.fill: parent
        ColorBox {
          text: "sandColor"
          color: __style.sandColor
        }
        ColorBox {
          text: "sunsetColor"
          color: __style.sunsetColor
        }
        ColorBox {
          text: "sunColor"
          color: __style.sunColor
        }
        ColorBox {
          text: "earthColor"
          color: __style.earthColor
        }
        ColorBox {
          text: "roseColor"
          color: __style.roseColor
        }
        ColorBox {
          text: "skyColor"
          color: __style.skyColor
        }
        ColorBox {
          text: "grapeColor"
          color: __style.grapeColor
        }
        ColorBox {
          text: "deepOceanColor"
          color: __style.deepOceanColor
        }
        ColorBox {
          text: "purpleColor"
          color: __style.purpleColor
        }
        ColorBox {
          text: "fieldColor"
          color: __style.fieldColor
        }
      }

    }

    GroupBox {
      title: "Colors - additional colors"
      background: Rectangle {
        color: "white"
        border.color: "gray"
      }
      label: Text {
        color: "black"
        text: parent.title
        padding: 5
      }

      Grid {
        columns: 2
        spacing: 20
        anchors.fill: parent
        ColorBox {
          text: "positiveColor"
          color: __style.positiveColor
        }
        ColorBox {
          text: "warningColor"
          color: __style.warningColor
        }
        ColorBox {
          text: "negativeColor"
          color: __style.negativeColor
        }
        ColorBox {
          text: "informativeColor"
          color: __style.informativeColor
        }
        ColorBox {
          text: "snappingColor"
          color: __style.snappingColor
        }
      }
    }

    GroupBox {
      title: "Colors - others"
      background: Rectangle {
        color: "white"
        border.color: "gray"
      }
      label: Text {
        color: "black"
        text: parent.title
        padding: 5
      }

      Grid {
        columns: 3
        spacing: 20
        anchors.fill: parent
        ColorBox {
          text: "nightAlphaColor"
          color: __style.nightAlphaColor
        }
        ColorBox {
          text: "errorBgInputColor"
          color: __style.errorBgInputColor
        }
        ColorBox {
          text: "shadowColor"
          color: __style.shadowColor
        }
        ColorBox {
          text: "lightGreenColor"
          color: __style.lightGreenColor
        }
        ColorBox {
          text: "mediumGreenColor"
          color: __style.mediumGreenColor
        }
        ColorBox {
          text: "greyColor"
          color: __style.greyColor
        }
        ColorBox {
          text: "darkGrey"
          color: __style.darkGreyColor
        }
        ColorBox {
          text: "mediumGrey"
          color: __style.mediumGreyColor
        }
      }
    }
  }
}
