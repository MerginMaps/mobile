/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/


#ifndef MERGINSECRETS_H
#define MERGINSECRETS_H

#include <QString>

#if !defined(USE_MERGIN_DUMMY_API_KEY)
QString __getSecretApiKey( const QString &serverName );
#endif

QString getSecretApiKey( const QString &serverName )
{
#if defined(USE_MERGIN_DUMMY_API_KEY)
  Q_UNUSED( serverName );
#else
  QString secretKey = __getSecretApiKey( serverName );
  if ( !secretKey.isEmpty() )
    return secretKey;
#endif
  return "not-secret-key";
}

#endif // MERGINSECRETS_H
