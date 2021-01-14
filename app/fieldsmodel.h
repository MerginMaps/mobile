#ifndef FIELDSMODEL_H
#define FIELDSMODEL_H

#include <QObject>
#include <QAbstractListModel>
#include <QVector>
#include <QMap>

#include "qgsfield.h"
#include "qgsfields.h"

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
      FieldType,
      WidgetType,
    };

    //! Creates a new fields model
    explicit FieldsModel( QObject *parent = nullptr );

    Q_INVOKABLE bool addField( const QString &name, const QString &type, const QString &widgetType = QString( "TextEdit" ) );
    Q_INVOKABLE bool removeField( int rowIndex );
    Q_INVOKABLE QVariantMap supportedTypes();
    Q_INVOKABLE QgsFields fields();


    QHash<int, QByteArray> roleNames() const override;
    int rowCount( const QModelIndex &parent ) const override;
    QVariant data( const QModelIndex &index, int role ) const override;
    bool setData( const QModelIndex &index, const QVariant &value, int role = Qt::EditRole ) override;

    QString findWidgetTypeByFieldName( const QString name ) const;

  signals:
    void widgetListChanged();
    void notify(const QString &message );

  private:
    QgsFields mFields;

    //! Inits model with default fields
    void initModel();
    QVariant::Type parseType( const QString &type );
    QgsField createField( const QString &name, const QString &type, const QString &widgetType );
};

#endif // FIELDSMODEL_H
