#ifndef LAYERSMODELREF_H
#define LAYERSMODELREF_H

#include <QObject>

#include "qgsmaplayerproxymodel.h"
#include "qgsmaplayermodel.h"
#include "qgsmaplayer.h"

class LayersModelRef : public QgsMapLayerProxyModel
{
  Q_OBJECT

public:
  LayersModelRef();

//  QVariant data( const QModelIndex &index, int role = Qt::DisplayRole ) const override;

  bool filterAcceptsRow( int source_row, const QModelIndex &source_parent ) const override;

protected:
  virtual bool layersFilter( int source_row, const QModelIndex &source_parent ) const;
};

#endif // LAYERSMODELREF_H
