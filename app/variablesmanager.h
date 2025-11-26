/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef VARIABLESMANAGER_H
#define VARIABLESMANAGER_H

#include <QObject>

#include "merginapi.h"
#include "qgsproject.h"
#include "position/positionkit.h"
#include "compass.h"

class MerginApi;

/*
 * The class sets global and project variables related to Mergin Maps.
 */
class VariablesManager : public QObject
{
    Q_OBJECT
    //! Source of QGeoPositionInfo.
    Q_PROPERTY( PositionKit *positionKit READ positionKit WRITE setPositionKit NOTIFY positionKitChanged )
    //! Source of direction
    Q_PROPERTY( Compass *compass READ compass WRITE setCompass NOTIFY compassChanged )
    /**
     * Property bound with RecordingMapTool. If true, the RecordingMapTool has used current position to create or edit geometry
     * for a feature. Therefore, current position information matches geometry for that feature.
     **/
    Q_PROPERTY( bool useGpsPoint READ useGpsPoint WRITE setUseGpsPoint NOTIFY useGpsPointChanged )

  public:
    VariablesManager( MerginApi *merginApi, QObject *parent = nullptr );
    ~VariablesManager() override;

    void removeMerginProjectVariables( QgsProject *project );
    //! Creates and registers custom expression functions to mobile app, so they can be used in default value definitions.
    void registerInputExpressionFunctions();
    QgsExpressionContextScope *positionScope();

    PositionKit *positionKit() const;
    void setPositionKit( PositionKit *positionKit );

    Compass *compass() const;
    void setCompass( Compass *compass );

    bool useGpsPoint() const;
    void setUseGpsPoint( bool useGpsPoint );

  signals:
    void positionKitChanged();
    void compassChanged();
    void useGpsPointChanged();

  public slots:
    void merginProjectChanged( QgsProject *project );

  private slots:
    void apiRootChanged();
    void setUserVariables();
    void setVersionVariable( const QString &projectFullName );

  private:
    MerginApi *mMerginApi = nullptr;
    QgsProject *mCurrentProject = nullptr;
    PositionKit *mPositionKit = nullptr; // not owned
    Compass *mCompass = nullptr; // not owned
    bool mUseGpsPoint = false;

    void setProjectVariables();
    void addPositionVariable( QgsExpressionContextScope *scope, const QString &name, const QVariant &value, const QVariant &defaultValue = QVariant() );
    QVariant getGeoPositionAttribute( double attributeValue, int precision = 2 );
};

#endif // VARIABLESMANAGER_H
