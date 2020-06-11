/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef TESTUTILS_H
#define TESTUTILS_H

#include <QString>

namespace TestUtils
{
  const int SHORT_REPLY = 3000;
  const int LONG_REPLY = 5000;

  void mergin_auth( QString &apiRoot, QString &username, QString &password );
}

#endif // TESTUTILS_H
