#ifndef RECORDINGLAYERSMODEL_H
#define RECORDINGLAYERSMODEL_H

#include <QObject>

#include "layersmodelref.h"
#include "qgsmaplayermodel.h"
#include "qgsvectorlayer.h"

class RecordingLayersModel : public LayersModelRef
{
  Q_OBJECT
public:
  RecordingLayersModel();

protected:
//  bool layersFilter( int source_row, const QModelIndex &source_parent ) const override;

};

#endif // RECORDINGLAYERSMODEL_H
