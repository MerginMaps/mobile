/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef FORMFEATURESMODEL_H
#define FORMFEATURESMODEL_H

#include <QAbstractListModel>

#include <QObject>
#include <QString>
#include <QList>

/**
 * Mockup of RelationFeaturesModel and RelationReferenceFeaturesModel class for gallery
 */
class FormFeaturesModel : public QAbstractListModel
{
    Q_OBJECT

    Q_PROPERTY( /*QgsRelation*/ QString relation READ relation WRITE setRelation NOTIFY relationChanged )
    Q_PROPERTY( QString homePath READ homePath WRITE setHomePath NOTIFY homePathChanged )
    Q_PROPERTY( /*FeatureLayerPair*/ QString parentFeatureLayerPair READ parentFeatureLayerPair WRITE setParentFeatureLayerPair NOTIFY parentFeatureLayerPairChanged )
    Q_PROPERTY( /*QgsProject */ QString project READ project WRITE setProject NOTIFY projectChanged )
    Q_PROPERTY( QString config READ config WRITE setConfig NOTIFY configChanged )

  public:

    enum relationModelRoles
    {
      FeatureTitle = Qt::UserRole + 10,
      FeatureId,
      Feature,
      FeaturePair,
      Description, // secondary text in list view
      SearchResult,
      PhotoPath = Qt::UserRole + 100,
    };
    Q_ENUM( relationModelRoles );

    explicit FormFeaturesModel( QObject *parent = nullptr )
    {
      for ( int i = 0; i <= mRows; ++i )
      {
        QString it = QString::number( i );
        mData << FeatureItem( i,
                              "title" + it,
                              "pair" + it,
                              "desc" + it, "search" + it, "feat" + it, "photo" + it );
      }
    }

    ~FormFeaturesModel() {};

    QVariant data( const QModelIndex &index, int role ) const override
    {
      int row = index.row();
      if ( row < 0 || row >= rowCount() )
        return QVariant();

      if ( !index.isValid() )
        return QVariant();

      const FeatureItem &item = mData.at( row );

      if ( role == PhotoPath )
      {
        return item.photoPath;
      }
      else if ( role == FeatureTitle )
      {
        return item.featureTitle;
      }
      else if ( role == FeatureId )
      {
        return item.featureId;
      }
      else if ( role == Feature )
      {
        return item.feature;
      }
      else if ( role == FeaturePair )
      {
        return item.featurePair;
      }
      else if ( role == Description )
      {
        return item.description;
      }
      else if ( role == SearchResult )
      {
        return item.searchResult;
      }
      else
        return QVariant();
    }
    QHash<int, QByteArray> roleNames() const override
    {
      QHash<int, QByteArray> roleNames = QAbstractListModel::roleNames();
      roleNames[PhotoPath] = QStringLiteral( "PhotoPath" ).toLatin1();
      roleNames[FeatureTitle] = QStringLiteral( "FeatureTitle" ).toLatin1();
      roleNames[FeatureId] = QStringLiteral( "FeatureId" ).toLatin1();
      roleNames[Feature] = QStringLiteral( "Feature" ).toLatin1();
      roleNames[FeaturePair] = QStringLiteral( "FeaturePair" ).toLatin1();
      roleNames[Description] = QStringLiteral( "Description" ).toLatin1();
      roleNames[SearchResult] = QStringLiteral( "SearchResult" ).toLatin1();
      return roleNames;
    }

    int rowCount( const QModelIndex &parent = QModelIndex() ) const override {return mRows;}

    // void setup();
    // void setupFeatureRequest( QgsFeatureRequest &request ) override;

    void setParentFeatureLayerPair( QString pair ) { mParentFeatureLayerPair = pair; emit parentFeatureLayerPairChanged( pair ); }
    void setRelation( QString relation ) {mRelation = relation; emit relationChanged( mRelation );}

    QString parentFeatureLayerPair() const {return mParentFeatureLayerPair;}
    QString relation() const {return mRelation;}

    QString homePath() const {return mHomePath;}
    void setHomePath( const QString &homePath ) {mHomePath = homePath; emit homePathChanged();}

    QString project() const {return mProject;}
    void setProject( const QString &project ) {mProject = project; emit projectChanged();}

    QString config() const {return mConfig;}
    void setConfig( const QString &project ) {mConfig = project; emit configChanged();}

  signals:
    void parentFeatureLayerPairChanged( QString pair );
    void relationChanged( QString relation );
    void homePathChanged();
    void projectChanged();
    void configChanged();

  private:
    /**
     * Searches and returns first index of photo field if the field is type of 'ExternalResource'.
     * @param layer Referencing layer of the relation.
     * @return Index of photo field, otherwise -1 if not found any.
     */
    int photoFieldIndex( ) const {return mPhotoIndex;}

    struct FeatureItem
    {
      FeatureItem( int id, QString title, QString pair, QString desc, QString search, QString feat, QString photo )
        : featureId( id )
        , featureTitle( title )
        , featurePair( pair )
        , description( desc )
        , searchResult( search )
        , feature( feat ),
          photoPath( photo )
      {
      }

      int featureId;
      QString featureTitle;
      QString featurePair;
      QString description;
      QString searchResult;
      QString feature;
      QString photoPath;
    };

    QList<FeatureItem> mData;

    QString mRelation; // associated relation
    QString mParentFeatureLayerPair; // parent feature (with relation widget in form)
    QString mHomePath;
    QString mProject;
    QString mConfig;

    int mPhotoIndex = -1;
    int mRows = 10;

};

#endif // FORMFEATURESMODEL_H
