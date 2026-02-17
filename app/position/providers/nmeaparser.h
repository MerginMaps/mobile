/***************************************************************************
*                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef NMEAPARSER_H
#define NMEAPARSER_H

#include <qgsnmeaconnection.h>

/**
 * NmeaParser is a big hack how to reuse QGIS NmeaConnection function in order to (a) keep ownership of bluetooth
 * socket, (b) do not have multiple unique_ptrs holding the same pointer and to avoid some possible crashes.
 *
 * Note: This way of reusing makes the parser highly dependent on QgsNmeaConnection class and any change inside the class
 * can lead to misbehavior's. See implementation of QgsNmeaConnection and QgsGpsConnection for more details.
 */
class NmeaParser : public QgsNmeaConnection
{
  public:
    NmeaParser();

// Takes nmea string and returns gps position
    QgsGpsInformation parseNmeaString( const QString &nmeaString );
};


#endif //NMEAPARSER_H