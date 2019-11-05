/***************************************************************************
  iosutils.h
  --------------------------------------
  Date                 : Sept 2019
  Copyright            : (C) 2019 by Peter Petrik
  Email                : zilolv at gmail dot com
 ***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef IOSUTILS_H
#define IOSUTILS_H

#include <QObject>

class IosUtils: public QObject
{
    Q_OBJECT
    Q_PROPERTY( bool isIos READ isIos CONSTANT )

  public:
    explicit IosUtils( QObject *parent = nullptr );
    bool isIos() const;
};

#endif // IOSUTILS_H
