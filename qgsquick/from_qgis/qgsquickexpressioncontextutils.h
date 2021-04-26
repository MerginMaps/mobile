/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef QGSQUICKEXPRESSIONCONTEXTUTILS_H
#define QGSQUICKEXPRESSIONCONTEXTUTILS_H

#include <QAbstractListModel>
#include <QVector>

#include "qgis_quick.h"
#include <qgsexpressioncontext.h>
#include "qgsquickfeaturelayerpair.h"
#include "qgsquickpositionkit.h"

/**
 * \ingroup quick
 * \brief TODO
 *
 * \since QGIS 3.20
 */
class QgsQuickExpressionContextUtils
{

    ~QgsQuickExpressionContextUtils() = default;
  public:
    static QgsExpressionContextScope *positionScope( PositionInfo &positionInformation );
    static QgsExpressionContextScope *positionScopeTest( const QString &test );
  private:
    static void addPositionVariable( QgsExpressionContextScope *scope, const QString &name, const QVariant &value, const QVariant &defaultValue = QVariant() );
};

#endif // QGSQUICKEXPRESSIONCONTEXTUTILS_H
