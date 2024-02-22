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
import QtQuick.Layouts

import lc 1.0

import "../components"
import "../misc"
import "../gps"

Item {
  id: root

  visible: false

  property string defaultLayer: __appSettings.defaultLayer


  function open( subsetting="" )
  {
    // opens settings panel and if subsetting is provided, opens directly that
    root.visible = true

    if ( subsetting === "gps" )
    {
      stackview.push( positionProviderComponent )
    }
  }

  function close()
  {
    if (stackview.depth > 1) {
      // hide about or log panel
      stackview.pop(null)
    } else
      root.visible = false
  }

  Keys.onReleased: function( event ) {
    if (event.key === Qt.Key_Back || event.key === Qt.Key_Escape) {
      event.accepted = true
      root.close()
    }
  }

  StackView {
    id: stackview

    anchors.fill: parent
    initialItem: MMSettingsPanel {
      id: settingsPanel

      onClose: root.close()
      onManageGpsClicked: stackview.push( positionProviderComponent )
      onAboutClicked: stackview.push(aboutPanelComponent)
      onChangelogClicked: stackview.push(changelogPanelComponent)
      onHelpClicked: Qt.openUrlExternally(__inputHelp.helpRootLink)
      onPrivacyPolicyClicked: Qt.openUrlExternally(__inputHelp.privacyPolicyLink)
      onTermsOfServiceClicked: Qt.openUrlExternally(__inputHelp.merginTermsLink)
      onDiagnosticLogClicked: stackview.push(logPanelComponent, { "text": __inputHelp.fullLog( true ) } )
    }
  }

  Component {
    id: aboutPanelComponent

    MMAboutPanel {
      onClose: stackview.pop(null)
      onVisitWebsiteClicked: Qt.openUrlExternally( __inputHelp.inputWebLink )
      Component.onCompleted: forceActiveFocus()
    }
  }

  Component {
    id: changelogPanelComponent

    MMChangelogPanel {
      id: changelogPanel
      onClose: stackview.pop(null)
      Component.onCompleted: forceActiveFocus()
      model: ChangelogModel {
        onErrorMsgChanged: function(msg) {
          changelogPanel.errorDialog.text = msg
          changelogPanel.errorDialog.open()
        }
      }
    }
  }

  Component {
    id: logPanelComponent

    MMLogPanel {
      onClose: stackview.pop(null)
      onSubmitReport: __inputHelp.submitReport()
      Component.onCompleted: forceActiveFocus()
    }
  }

  Component {
    id: positionProviderComponent

    MMPositionProviderPage {
      onClose: stackview.pop(null)
      Component.onCompleted: forceActiveFocus()
    }
  }
}
