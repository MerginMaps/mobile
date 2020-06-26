#ifndef LAYERFEATURESMODEL_H
#define LAYERFEATURESMODEL_H

#include <QAbstractListModel>

class LayerFeaturesModel : public QAbstractListModel
{
    Q_OBJECT

  enum roleNames {
    id = Qt::UserRole + 1,
    displayName
  };

  public:
    explicit LayerFeaturesModel( QObject *parent = nullptr );

    // Basic functionality:
    int rowCount( const QModelIndex &parent = QModelIndex() ) const override;

    QVariant data( const QModelIndex &index, int role = Qt::DisplayRole ) const override;

    QHash<int, QByteArray> roleNames() const override;

    bool addFeature( const QPair<int, QString> &feature );

    // Editable:
    bool setData( const QModelIndex &index, const QVariant &value,
                  int role = Qt::EditRole ) override;

    Qt::ItemFlags flags( const QModelIndex &index ) const override;

  private:
    QList<QPair<int, QString>> m_features;
};

#endif // LAYERFEATURESMODEL_H
