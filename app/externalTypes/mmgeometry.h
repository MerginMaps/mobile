/***************************************************************************
*                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef MMGEOMETRY_H
#define MMGEOMETRY_H

#include <QtQml/qqmlregistration.h>

#include <qgsgeometry.h>

/**
 * MMGeometry is just a wrapper around QgsGeometry, which adds equals operator that is missing.
 * We expose this type to QML instead of QgsGeometry. The reason is that QML in Qt 6.9+ also checks equality, when firing
 * onChanged signal. The missing operator is breaking property bindings.
 */
class MMGeometry: public QgsGeometry
{
    Q_GADGET
    QML_VALUE_TYPE( mmGeometry );

  public:
    using QgsGeometry::QgsGeometry;

    MMGeometry() = default;
    MMGeometry( const QgsGeometry &geom ) : QgsGeometry( geom ) {}
    MMGeometry( QgsGeometry &geom ) : QgsGeometry( geom ) {}

    bool operator==( const MMGeometry &rhs ) const
    {
      return equals( rhs );
    }
};

#endif //MMGEOMETRY_H
