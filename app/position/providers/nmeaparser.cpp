/***************************************************************************
*                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include "nmeaparser.h"

#include <QBuffer>

NmeaParser::NmeaParser() : QgsNmeaConnection( new QBuffer() )
{
}

QgsGpsInformation NmeaParser::parseNmeaString( const QString &nmeaString )
{
  mStringBuffer = nmeaString;
  processStringBuffer();
  return mLastGPSInformation;
}