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
import "../components" as GalleryComponents

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
        GalleryComponents.ColorBox {
          text: "grassColor"
          color: __style.grassColor
        }
        GalleryComponents.ColorBox {
          text: "forestColor"
          color: __style.forestColor
        }
        GalleryComponents.ColorBox {
          text: "nightColor"
          color: __style.nightColor
        }
        GalleryComponents.ColorBox {
          text: "polarColor"
          color: __style.polarColor
        }
        GalleryComponents.ColorBox {
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
        GalleryComponents.ColorBox {
          text: "sandColor"
          color: __style.sandColor
        }
        GalleryComponents.ColorBox {
          text: "sunsetColor"
          color: __style.sunsetColor
        }
        GalleryComponents.ColorBox {
          text: "sunColor"
          color: __style.sunColor
        }
        GalleryComponents.ColorBox {
          text: "earthColor"
          color: __style.earthColor
        }
        GalleryComponents.ColorBox {
          text: "roseColor"
          color: __style.roseColor
        }
        GalleryComponents.ColorBox {
          text: "skyColor"
          color: __style.skyColor
        }
        GalleryComponents.ColorBox {
          text: "grapeColor"
          color: __style.grapeColor
        }
        GalleryComponents.ColorBox {
          text: "deepOceanColor"
          color: __style.deepOceanColor
        }
        GalleryComponents.ColorBox {
          text: "purpleColor"
          color: __style.purpleColor
        }
        GalleryComponents.ColorBox {
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
        GalleryComponents.ColorBox {
          text: "positiveColor"
          color: __style.positiveColor
        }
        GalleryComponents.ColorBox {
          text: "warningColor"
          color: __style.warningColor
        }
        GalleryComponents.ColorBox {
          text: "negativeColor"
          color: __style.negativeColor
        }
        GalleryComponents.ColorBox {
          text: "informativeColor"
          color: __style.informativeColor
        }
        GalleryComponents.ColorBox {
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
        GalleryComponents.ColorBox {
          text: "nightAlphaColor"
          color: __style.nightAlphaColor
        }
        GalleryComponents.ColorBox {
          text: "errorBgInputColor"
          color: __style.errorBgInputColor
        }
        GalleryComponents.ColorBox {
          text: "shadowColor"
          color: __style.shadowColor
        }
        GalleryComponents.ColorBox {
          text: "lightGreenColor"
          color: __style.lightGreenColor
        }
        GalleryComponents.ColorBox {
          text: "mediumGreenColor"
          color: __style.mediumGreenColor
        }
        GalleryComponents.ColorBox {
          text: "darkGreenColor"
          color: __style.darkGreenColor
        }
        GalleryComponents.ColorBox {
          text: "greyColor"
          color: __style.greyColor
        }
        GalleryComponents.ColorBox {
          text: "darkGrey"
          color: __style.darkGreyColor
        }
        GalleryComponents.ColorBox {
          text: "mediumGrey"
          color: __style.mediumGreyColor
        }
      }
    }
  }
}
