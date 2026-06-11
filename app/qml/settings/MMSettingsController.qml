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

import mm 1.0 as MM
import MMInput

import "../components"
import "../gps"

Item {
  id: root

  property string defaultLayer: AppSettings.defaultLayer

  signal closed()
  signal opened()

  enum Pages { Normal, GPSConnection }

  function open( subpage = MMSettingsController.Pages.Normal )
  {
    // opens settings panel and if subpage is provided, opens directly that
    if ( subpage === MMSettingsController.Pages.GPSConnection )
    {
      stackview.push( positionProviderComponent )
    }
    else {
      stackview.push( settingsPageComponent )
    }

    stackview.forceActiveFocus()
    root.opened()
  }

  function back()
  {
    // close the last page; if there is only one, close the controller

    if (stackview.depth > 1) {
      stackview.pop( null )
    }
    else {
      stackview.clear()
      root.closed()
    }
  }


  MMStackView {
    id: stackview

    width: ApplicationWindow.window?.width ?? 0
    height: ApplicationWindow.window?.height ?? 0
  }

  Component {
    id: settingsPageComponent

    MMSettingsPage {
      onBackClicked: root.back()
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

    MMAboutPage {
      onBackClicked: root.back()
      onVisitWebsiteClicked: Qt.openUrlExternally( __inputHelp.mmWebLink )
    }
  }

  Component {
    id: changelogPanelComponent

    MMChangelogPage {
      id: changelogPanel

      onBackClicked: root.back()

      model: MM.ChangelogModel {
        onLoadingFailure: __notificationModel.addError( qsTr("Changelog could not be loaded") )
      }
    }
  }

  Component {
    id: logPanelComponent

    MMLogPage {
      onBackClicked: root.back()
      onSubmitReport: __inputHelp.submitReport()
    }
  }

  Component {
    id: positionProviderComponent

    MMPositionProviderPage {
      onBackClicked: root.back()
    }
  }
}
