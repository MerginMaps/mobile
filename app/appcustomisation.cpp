/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include "appcustomisation.h"

AppCustomisation::AppCustomisation( QObject *parent ): QObject( parent )
{
    // This file will have only empty contructor in main production MM app;
    // in whitelabelled apps it will have the overriden properties like AC_ORG_NAME
    mValues[AC_ORG_NAME] = "blabllba";
}
