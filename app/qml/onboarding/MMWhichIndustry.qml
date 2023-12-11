/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/
import QtQuick
import QtQuick.Layouts
import QtQuick.Controls

import "../components"

Page {
  id: root

  signal backClicked
  signal continueClicked

  function uncheckAll(except, isChecked) {
    if (!isChecked)
      return;

    var items = [
      agricultureSource,
      archaeologySource,
      engineeringSource,
      electricitySource,
      environmentalSource,
      stateAndLocalSource,
      naturalResourcesSource,
      telecommunicationSource,
      transportationSource,
      waterResourcesSource,
      othersSource
    ]

    items.forEach( function(item) {
      if (item !== except) {
        item.checked = false
      }
    } )
  }

  Column {
    MMHeader {
      headerTitle: qsTr("Which industry")
      step: 3

      onBackClicked: root.backClicked()
    }

    Grid {
        columns: 2
        spacing: 10

        MMIconCheckBoxVertical {
          id: agricultureSource
          sourceIcon: __style.tractorIcon
          text: qsTr("Agriculture")
          onToggled: uncheckAll(agricultureSource, checked)
          bgColorIcon: __style.sunColor
        }
        MMIconCheckBoxVertical {
          id: archaeologySource
          sourceIcon: __style.archaeologyIcon
          text: qsTr("Archaeology")
          onToggled: uncheckAll(archaeologySource, checked)
          bgColorIcon: __style.sandColor
        }
        MMIconCheckBoxVertical {
          id: engineeringSource
          sourceIcon: __style.engineeringIcon
          text: qsTr("Construction and engineering")
          onToggled: uncheckAll(engineeringSource, checked)
          bgColorIcon: __style.roseColor
        }
        MMIconCheckBoxVertical {
          id: electricitySource
          sourceIcon: __style.electricityIcon
          text: qsTr("Electric utilities")
          onToggled: uncheckAll(electricitySource, checked)
          bgColorIcon: __style.nightColor
        }
        MMIconCheckBoxVertical {
          id: environmentalSource
          sourceIcon: __style.environmentalIcon
          text: qsTr("Environmental protection")
          onToggled: uncheckAll(environmentalSource, checked)
          bgColorIcon: __style.fieldColor
        }
        MMIconCheckBoxVertical {
          id: stateAndLocalSource
          sourceIcon: __style.stateAndLocalIcon
          text: qsTr("Local governments")
          onToggled: uncheckAll(stateAndLocalSource, checked)
          bgColorIcon: __style.purpleColor
        }
        MMIconCheckBoxVertical {
          id: naturalResourcesSource
          sourceIcon: __style.naturalResourcesIcon
          text: qsTr("Natural resources")
          onToggled: uncheckAll(naturalResourcesSource, checked)
          bgColorIcon: __style.earthColor
        }
        MMIconCheckBoxVertical {
          id: telecommunicationSource
          sourceIcon: __style.telecommunicationIcon
          text: qsTr("Telecom")
          onToggled: uncheckAll(telecommunicationSource, checked)
          bgColorIcon: __style.deepOceanColor
        }
        MMIconCheckBoxVertical {
          id: transportationSource
          sourceIcon: __style.transportationIcon
          text: qsTr("Transportation")
          onToggled: uncheckAll(transportationSource, checked)
          bgColorIcon: __style.skyColor
        }
        MMIconCheckBoxVertical {
          id: waterResourcesSource
          sourceIcon: __style.waterResourcesIcon
          text: qsTr("Water utilities")
          onToggled: uncheckAll(waterResourcesSource, checked)
          bgColorIcon: __style.lightGreenColor
        }

        MMIconCheckBoxVertical {
          id: othersSource
          sourceIcon: __style.othersIcon
          text: qsTr("Others")
          onToggled: uncheckAll(othersSource, checked)
          bgColorIcon: __style.sunsetColor
        }
    }

    MMInput {
      id: othersSourceDescription
      visible: othersSource.checked
      title: qsTr("Industry")
      placeholderText: qsTr("Please specify the industry")
    }

    MMButton {
      onClicked: root.continueClicked()
      text: qsTr("Continue")
    }
  }
}
