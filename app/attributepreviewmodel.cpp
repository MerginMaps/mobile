/***************************************************************************
  attributepreviewmodels.cpp
  --------------------------------------
  Date                 : 5.5.2021
  Copyright            : (C) 2021 by Peter Petrik
  Email                : zilolv@gmail.com
 ***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include "attributepreviewmodel.h"
#include "qgsquickfeaturelayerpair.h"
#include "qgsfield.h"
#include "qgsvectorlayer.h"

AttributePreviewModel::AttributePreviewModel( QObject *parent )
  : QAbstractListModel( parent )
{
}

QHash<int, QByteArray> AttributePreviewModel::roleNames() const
{
  QHash<int, QByteArray> roles = QAbstractItemModel::roleNames();
  roles[AttributePreviewModel::Name]  = QByteArray( "Name" );
  roles[AttributePreviewModel::Value] = QByteArray( "Value" );
  return roles;
}

AttributePreviewModel::~AttributePreviewModel() = default;

int AttributePreviewModel::rowCount( const QModelIndex &parent ) const
{
  Q_UNUSED( parent )
  return mItems.size();
}

QVariant AttributePreviewModel::data( const QModelIndex &index, int role ) const
{
  if ( !index.isValid() )
    return QVariant();

  const int row = index.row();
  if ( row < 0 || row >= mItems.size() )
    return QVariant();

  switch ( role )
  {
    case AttributePreviewModel::Name:
      return mItems.at( row ).first;
    case AttributePreviewModel::Value:
      return mItems.at( row ).second;
    default:
      return QVariant();
  }
}

static QVector<QPair<QString, QString>> _getItems( const QgsQuickFeatureLayerPair &pair )
{
  if ( !pair.layer() || !pair.feature().isValid() )
    return QVector<QPair<QString, QString>>();

  QString mapTip = pair.layer()->mapTipTemplate();
  QVector<QPair<QString, QString>> lst;
  const QgsFields fields = pair.layer()->fields();
  const int LIMIT = 3;  // max. 3 fields can fit in the preview

  if ( mapTip.isEmpty() )
  {
    // user has not provided any map tip - let's use first two fields to show
    // at least something.
    QString featureTitleExpression = pair.layer()->displayExpression();
    for ( QgsField field : fields )
    {
      if ( featureTitleExpression != field.name() )
      {
        const QPair<QString, QString> item = qMakePair(
                                               field.displayName(),
                                               pair.feature().attribute( field.name() ).toString()
                                             );

        lst.append( item );
      }

      if ( lst.count() == LIMIT )
        break;
    }
  }
  else
  {
    // user has specified "# fields" on the first line and then each next line is a field name
    QStringList lines = mapTip.split( '\n' );
    for ( int i = 1; i < lines.count(); ++i ) // starting from index to avoid first line with "# fields"
    {
      int index = fields.indexFromName( lines[i] );
      if ( index >= 0 )
      {
        const QPair<QString, QString> item = qMakePair(
                                               fields[index].displayName(),
                                               pair.feature().attribute( fields[index].name() ).toString()
                                             );

        lst.append( item );
      }
      if ( lst.count() == LIMIT )
        break;
    }
  }
  return lst;
}

void AttributePreviewModel::resetModel( const QgsQuickFeatureLayerPair &pair )
{
  const QVector<QPair<QString, QString>> items = _getItems( pair );
  if ( mItems != items )
  {
    beginResetModel();
    mItems = items;
    endResetModel();
  }
}
