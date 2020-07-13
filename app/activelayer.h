#ifndef ACTIVELAYER_H
#define ACTIVELAYER_H

#include <QObject>
#include "qgsmaplayer.h"

#include "layersproxymodel.h"
#include "appsettings.h"

class ActiveLayer : public QObject
{
    Q_OBJECT

    Q_PROPERTY( int index WRITE setActiveLayer READ index NOTIFY activeLayerChanged)
    Q_PROPERTY( QModelIndex modelIndex READ modelIndex )
    Q_PROPERTY( QgsMapLayer *layer READ layer )
    Q_PROPERTY( QString layerId READ layerId )

  public:
    ActiveLayer( LayersProxyModel &model, AppSettings &appSettings );

    QModelIndex modelIndex() const;
    QgsMapLayer *layer() const;
    QString layerId() const;
    int index() const;

    //! Stores index of a selected layer from a model
    void setActiveLayer( int index );

  public slots:
    void activeMapThemeChanged();

  signals:
    void activeLayerChanged();

  private:
    LayersProxyModel &mModel;
    AppSettings &mAppSettings;

    int mIndex;
};

#endif // ACTIVELAYER_H
