#ifndef LAYERFEATURESMODEL_H
#define LAYERFEATURESMODEL_H

#include <QAbstractListModel>
#include "qgsfeaturemockup.h"

class LayerFeaturesModel : public QAbstractListModel
{
    Q_OBJECT

  enum roleNames {
    id = Qt::UserRole + 1,
    displayName
  };

  public:
    explicit LayerFeaturesModel( QObject *parent = nullptr );

    void setDataStorage( QgsFeatureMockup &dataGenerator ) { m_dataStorage = &dataGenerator; }

    // Basic functionality:
    int rowCount( const QModelIndex &parent = QModelIndex() ) const override;

    QVariant data( const QModelIndex &index, int role = Qt::DisplayRole ) const override;

    QHash<int, QByteArray> roleNames() const override;

    bool addFeature( const QgsFeatureMock &feature );

    // Editable:
    bool setData( const QModelIndex &index, const QVariant &value,
                  int role = Qt::EditRole ) override;

    Qt::ItemFlags flags( const QModelIndex &index ) const override;

public slots:
    void reloadDataFromLayer( const QString &layerName );

  private:
    QList<QgsFeatureMock> m_features;
    QgsFeatureMockup *m_dataStorage;
};

#endif // LAYERFEATURESMODEL_H
