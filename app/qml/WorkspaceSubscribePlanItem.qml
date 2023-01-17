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

import "."  // import InputStyle singleton
import "./components" as MMComponents

Item {
  id: root

  signal subscribeClicked

  property var plan
  property bool hasPlan: __merginApi.subscriptionInfo.ownsActiveSubscription || !root.plan

  height: childrenRect.height
  // set width from parent

  Column {
    width: parent.width
    spacing: InputStyle.panelSpacing

    MMComponents.TextWithIcon {
      width: parent.width
      height: InputStyle.rowHeight
      source: InputStyle.infoIcon
      text: qsTr("Commercial use")
    }

    MMComponents.TextWithIcon {
      width: parent.width
      height: InputStyle.rowHeight
      source: InputStyle.todayIcon
      text: hasPlan ? "Custom billing period" : root.plan.period /* Do not translate, only used for test subscriptions */
    }

    MMComponents.TextWithIcon {
      width: parent.width
      height: InputStyle.rowHeight
      source: InputStyle.databaseIcon
      text: hasPlan ? "Custom storage" : root.plan.storage /* Do not translate, only used for test subscriptions */
    }

    MMComponents.TextWithIcon {
      width: parent.width
      height: InputStyle.rowHeight
      source: InputStyle.accountMultiIcon
      text: qsTr("Unlimited seats")
    }

    MMComponents.TextWithIcon {
      width: parent.width
      height: InputStyle.rowHeight
      source: InputStyle.projectIcon
      text: qsTr("Unlimited projects")
    }

    MMComponents.TextWithIcon {
      width: parent.width
      height: InputStyle.rowHeight
      source: InputStyle.envelopeIcon
      text: qsTr("Email support")
    }

    MMComponents.DelegateButton {

      width: parent.width
      btnWidth: width
      height: InputStyle.rowHeightMedium

      text: root.hasPlan ? "Manage" : root.plan.price /* Do not translate, only used for test subscriptions */
      enabled: text !== ''

      onClicked: root.subscribeClicked()
    }
  }
}
