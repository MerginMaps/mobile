/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include "variablesmanager.h"

#include "qgsexpressioncontextutils.h"
#include "inputexpressionfunctions.h"

VariablesManager::VariablesManager( MerginApi *merginApi, QObject *parent )
  : QObject( parent )
  , mMerginApi( merginApi )
{
  apiRootChanged();
  authChanged();

  QObject::connect( mMerginApi, &MerginApi::apiRootChanged, this, &VariablesManager::apiRootChanged );
  QObject::connect( mMerginApi, &MerginApi::authChanged, this, &VariablesManager::authChanged );
  QObject::connect( mMerginApi, &MerginApi::projectDataChanged, this, &VariablesManager::setVersionVariable );
}

VariablesManager::~VariablesManager() = default;

void VariablesManager::removeMerginProjectVariables( QgsProject *project )
{
  QgsExpressionContextUtils::removeProjectVariable( project, QStringLiteral( "mergin_project_name" ) );
  QgsExpressionContextUtils::removeProjectVariable( project, QStringLiteral( "mergin_project_full_name" ) );
  QgsExpressionContextUtils::removeProjectVariable( project, QStringLiteral( "mergin_project_version" ) );
  QgsExpressionContextUtils::removeProjectVariable( project, QStringLiteral( "mergin_project_owner" ) );
}

void VariablesManager::registerInputExpressionFunctions()
{
  QgsExpression::registerFunction( new ReadExif() );
  QgsExpression::registerFunction( new ReadExifImgDirection() );
  QgsExpression::registerFunction( new ReadExifLongitude() );
  QgsExpression::registerFunction( new ReadExifLatitude() );
}

QgsExpressionContextScope *VariablesManager::positionScope()
{
  GeoPosition position = mPositionKit->position();
  return positionScope( position, compass()->direction(), mUseGpsPoint );
}

QgsExpressionContextScope *VariablesManager::positionScope( const GeoPosition &pos, const double direction, bool useGpsPoint )
{
  QgsExpressionContextScope *scope = new QgsExpressionContextScope( QStringLiteral( "Position" ) );
  const QgsGeometry point = QgsGeometry( new QgsPoint( pos.longitude, pos.latitude, pos.elevation ) );

  addPositionVariable( scope, QStringLiteral( "coordinate" ), QVariant::fromValue<QgsGeometry>( point ) );
  addPositionVariable( scope, QStringLiteral( "longitude" ), pos.longitude );
  addPositionVariable( scope, QStringLiteral( "latitude" ), pos.latitude );
  addPositionVariable( scope, QStringLiteral( "altitude" ), pos.elevation );
  addPositionVariable( scope, QStringLiteral( "horizontal_accuracy" ), getGeoPositionAttribute( pos.hacc ) );
  addPositionVariable( scope, QStringLiteral( "vertical_accuracy" ), getGeoPositionAttribute( pos.vacc ) );
  addPositionVariable( scope, QStringLiteral( "ground_speed" ), getGeoPositionAttribute( pos.speed ) );
  addPositionVariable( scope, QStringLiteral( "vertical_speed" ), getGeoPositionAttribute( pos.verticalSpeed ) );
  addPositionVariable( scope, QStringLiteral( "magnetic_variation" ), getGeoPositionAttribute( pos.magneticVariation ) );
  addPositionVariable( scope, QStringLiteral( "timestamp" ), pos.utcDateTime );
  addPositionVariable( scope, QStringLiteral( "direction" ), ( 360 + int( direction ) ) % 360 );
  addPositionVariable( scope, QStringLiteral( "from_gps" ), useGpsPoint );

  return scope;
}

void VariablesManager::apiRootChanged()
{
  QgsExpressionContextUtils::setGlobalVariable( QStringLiteral( "mergin_url" ),  mMerginApi->apiRoot() );
}

void VariablesManager::authChanged()
{
  QgsExpressionContextUtils::setGlobalVariable( QStringLiteral( "mergin_username" ),  mMerginApi->merginUserName() );
}

void VariablesManager::setVersionVariable( const QString &projectFullName )
{
  if ( !mCurrentProject )
    return;

  if ( mCurrentProject->customVariables().value( QStringLiteral( "mergin_project_full_name" ) ).toString() == projectFullName )
    setProjectVariables();
}

bool VariablesManager::useGpsPoint() const
{
  return mUseGpsPoint;
}

void VariablesManager::setUseGpsPoint( bool useGpsPoint )
{
  if ( mUseGpsPoint != useGpsPoint )
  {
    mUseGpsPoint = useGpsPoint;
    emit useGpsPointChanged();
  }
}

Compass *VariablesManager::compass() const
{
  return mCompass;
}

void VariablesManager::setCompass( Compass *compass )
{
  if ( mCompass != compass )
  {
    mCompass = compass;
    emit compassChanged();
  }
}

PositionKit *VariablesManager::positionKit() const
{
  return mPositionKit;
}

void VariablesManager::setPositionKit( PositionKit *positionKit )
{
  if ( mPositionKit != positionKit )
  {
    mPositionKit = positionKit;
    emit positionKitChanged();
  }
}

void VariablesManager::merginProjectChanged( QgsProject *project )
{
  mCurrentProject = project;
  setProjectVariables();
}

void VariablesManager::setProjectVariables()
{
  if ( !mCurrentProject )
    return;


  QString filePath = mCurrentProject->fileName();
  QString projectDir = mMerginApi->localProjectsManager().projectFromProjectFilePath( filePath ).projectDir;
  if ( projectDir.isEmpty() )
  {
    removeMerginProjectVariables( mCurrentProject );
    return;
  }

  MerginProjectMetadata metadata = MerginProjectMetadata::fromCachedJson( projectDir + "/" + MerginApi::sMetadataFile );
  if ( metadata.isValid() )
  {
    QgsExpressionContextUtils::setProjectVariable( mCurrentProject, QStringLiteral( "mergin_project_version" ), metadata.version );
    QgsExpressionContextUtils::setProjectVariable( mCurrentProject, QStringLiteral( "mergin_project_name" ),  metadata.name );
    QgsExpressionContextUtils::setProjectVariable( mCurrentProject, QStringLiteral( "mergin_project_full_name" ),  mMerginApi->getFullProjectName( metadata.projectNamespace,  metadata.name ) );
    QgsExpressionContextUtils::setProjectVariable( mCurrentProject, QStringLiteral( "mergin_project_owner" ),   metadata.projectNamespace );
  }
  else
  {
    removeMerginProjectVariables( mCurrentProject );
  }
}

void VariablesManager::addPositionVariable( QgsExpressionContextScope *scope, const QString &name, const QVariant &value, const QVariant &defaultValue )
{
  if ( value.isValid() )
  {
    scope->addVariable( QgsExpressionContextScope::StaticVariable( QStringLiteral( "position_%1" ).arg( name ), value, true, true ) );
  }
  else
  {
    scope->addVariable( QgsExpressionContextScope::StaticVariable( QStringLiteral( "position_%1" ).arg( name ), defaultValue, true, true ) );
  }
}

QVariant VariablesManager::getGeoPositionAttribute( double attributeValue )
{
  if ( attributeValue >= 0 )
  {
    return QString::number( attributeValue, 'f', 2 );
  }
  else
    return QVariant();
}
