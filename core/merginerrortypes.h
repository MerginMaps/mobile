/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef MERGINERRORTYPES_H
#define MERGINERRORTYPES_H

#include <QObject>

class RegistrationError
{
    Q_GADGET
  public:
    enum RegistrationErrorType
    {
      OTHER = 0,
      EMAIL,
      PASSWORD,
      TOC
    };
    Q_ENUM( RegistrationErrorType )
};

#endif
