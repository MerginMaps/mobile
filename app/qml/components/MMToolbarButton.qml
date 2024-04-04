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

/**
  MMToolbarButton is button to use MMToolbar.
  Buttons can be
   - selectable (when using for switching tabs); to select the button, use MMToolbar.index property
   - enabled/disabled (button not clickable and shaded)
   - active/not active (text "active" on right of the more menu)
   - visible/invisible
*/
QtObject {
  id: root

  signal clicked

  /* required */ property var iconSource // The icon to be used in main toolbar for button
  /* required */ property string text // Text of the button in main toolbar and drawer menu

  /* optional */ property color bgColor: __style.grassColor // Background color - only used in long button, short button is transparent
  /* optional */ property color iconColor: __style.polarColor // Color of icon/text in enabled state
  /* optional */ property color iconColorDisabled: __style.mediumGreenColor // Color of icon/text when button is disabled
  /* optional */ property var iconSourceSelected: root.iconSource // The icon to be used in main toolbar when button is selected
  /* optional */ property bool visible: true // Set when you want to show/hide button based on caller state or condition
  /* optional */ property bool active: false // Set when the button is active (e.g. position tracking)
  /* optional */ property bool enabled: true
  /* optional */ property bool iconRotateAnimationRunning: false // When true, rotate the icon indifinetely
}
