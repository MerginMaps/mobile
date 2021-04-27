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
 * \brief The utility class providing an extra QgsExpressionContextScope with new variables.
 * \since QGIS 3.20
 */
class QUICK_EXPORT QgsQuickExpressionContextUtils: public QObject
{

    Q_OBJECT

    ~QgsQuickExpressionContextUtils() = default;
  public:
    /**
       * @brief Creates a new QgsExpressionContextScope with position variables that suppose to extend expression scope while creating/editing a feature.
       * @param positionInformation The source of position information.
       * @param usesGpsPosition Value is set to the "position_point_from_gps" variable. Suppose to be true when given positionInformation matches a feature
       * that a new scope will be used with.
       * @return QgsExpressionContextScope
       */
    static QgsExpressionContextScope *positionScope( PositionInfo positionInformation, bool usesGpsPosition );
  private:
    static void addPositionVariable( QgsExpressionContextScope *scope, const QString &name, const QVariant &value, const QVariant &defaultValue = QVariant() );
    static QVariant notNaNorEmpty( double value );
};

#endif // QGSQUICKEXPRESSIONCONTEXTUTILS_H
