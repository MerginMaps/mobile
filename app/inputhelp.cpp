/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include "inputhelp.h"

const QString inputHelpRoot = "https://help.inputapp.io";
// const QString merginHelpRoot = "https://help.cloudmergin.com";

InputHelp::InputHelp()
{
  emit linkChanged();
}

QString InputHelp::privacyPolicyLink() const
{
  return inputHelpRoot + "/privacy";
}

QString InputHelp::howToEnableDigitizingLink() const
{
  return inputHelpRoot + "/howto/enable_digitizing";
}

QString InputHelp::howToEnableBrowsingDataLink() const
{
  return inputHelpRoot + "/howto/enable_browsing";
}

QString InputHelp::howToSetupThemesLink() const
{
  return inputHelpRoot + "/howto/setup_themes";
}

QString InputHelp::howToCreateNewProjectLink() const
{
  return inputHelpRoot + "/howto/create_project";
}

QString InputHelp::howToDownloadProjectLink() const
{
  return inputHelpRoot + "/howto/data_sync";
}
