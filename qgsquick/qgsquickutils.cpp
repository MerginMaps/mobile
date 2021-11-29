/***************************************************************************
  qgsquickutils.cpp
  --------------------------------------
  Date                 : Nov 2017
  Copyright            : (C) 2017 by Peter Petrik
  Email                : zilolv at gmail dot com
 ***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include <QGuiApplication>
#include <QScreen>
#include <QString>
#include <QWindow>

#include "qgsquickmapsettings.h"
#include "qgsquickutils.h"

QgsQuickUtils::QgsQuickUtils( QObject *parent )
  : QObject( parent )
  , mScreenDensity( calculateScreenDensity() )
{
}

qreal QgsQuickUtils::screenDensity() const
{
  return mScreenDensity;
}

qreal QgsQuickUtils::calculateScreenDensity()
{
  // Previously calculated screen pixel density, now kept here for possible future use
  return 1;
}
