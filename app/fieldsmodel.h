#ifndef FIELDSMODEL_H
#define FIELDSMODEL_H

#include <QObject>
#include <QAbstractListModel>
#include <QVector>

#include "qgsfield.h"

class FieldsModel: public QAbstractListModel
{
    Q_OBJECT
  public:
    /**
     * Feature roles enum.
     */
    Q_ENUMS( FeatureRoles )

    //! Feature roles
    enum FeatureRoles
    {
      AttributeName = Qt::UserRole + 1,  //!< Attribute's display name (the original field name or a custom alias)
      WidgetType,
      Field,                             //!< Field definition (QgsField)
    };

    //! Creates a new fields model
    explicit FieldsModel( QObject *parent = nullptr );

    Q_INVOKABLE bool addField( const QString &name, const QString &type );
    Q_INVOKABLE QStringList supportedTypes();

    QHash<int, QByteArray> roleNames() const override;
    int rowCount( const QModelIndex &parent ) const override;
    QVariant data( const QModelIndex &index, int role ) const override;
    bool setData( const QModelIndex &index, const QVariant &value, int role = Qt::EditRole ) override;

signals:
    void widgetListChanged();

private:
    QList<QgsField> mFields;
    //QStringList widgetList;
    //! Inits model with default fields
    void initModel();
    QVariant parseType(const QString &type);
};

#endif // FIELDSMODEL_H
