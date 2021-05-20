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
  QgsExpressionContextScope *scope = new QgsExpressionContextScope( QObject::tr( "Position" ) );
  QGeoPositionInfo geoInfo = mPositionKit->lastKnownPosition();
  const QgsGeometry point = QgsGeometry( new QgsPoint( geoInfo.coordinate().longitude(), geoInfo.coordinate().latitude(), geoInfo.coordinate().altitude() ) );

  addPositionVariable( scope, QStringLiteral( "coordinate" ), QVariant::fromValue<QgsGeometry>( point ) );
  addPositionVariable( scope, QStringLiteral( "longitude" ), geoInfo.coordinate().longitude() );
  addPositionVariable( scope, QStringLiteral( "latitude" ), geoInfo.coordinate().latitude() );
  addPositionVariable( scope, QStringLiteral( "altitude" ), geoInfo.coordinate().altitude() );
  addPositionVariable( scope, QStringLiteral( "horizontal_accuracy" ), getGeoPositionAttribute( geoInfo, QGeoPositionInfo::HorizontalAccuracy ) );
  addPositionVariable( scope, QStringLiteral( "vertical_accuracy" ), getGeoPositionAttribute( geoInfo, QGeoPositionInfo::VerticalAccuracy ) );
  addPositionVariable( scope, QStringLiteral( "vertical_speed" ), getGeoPositionAttribute( geoInfo, QGeoPositionInfo::GroundSpeed ) );
  addPositionVariable( scope, QStringLiteral( "ground_speed" ), getGeoPositionAttribute( geoInfo, QGeoPositionInfo::VerticalSpeed ) );
  addPositionVariable( scope, QStringLiteral( "magnetic_variation" ), getGeoPositionAttribute( geoInfo, QGeoPositionInfo::MagneticVariation ) );
  addPositionVariable( scope, QStringLiteral( "timestamp" ), geoInfo.timestamp() );
  addPositionVariable( scope, QStringLiteral( "direction" ), ( 360 + int( mCompass->direction() ) ) % 360 );
  addPositionVariable( scope, QStringLiteral( "from_gps" ), mUseGpsPoint );

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
  mUseGpsPoint = useGpsPoint;
  emit useGpsPointChanged();
}

Compass *VariablesManager::compass() const
{
  return mCompass;
}

void VariablesManager::setCompass( Compass *compass )
{
  mCompass = compass;
  emit compassChanged();
}

PositionKit *VariablesManager::positionKit() const
{
  return mPositionKit;
}

void VariablesManager::setPositionKit( PositionKit *positionKit )
{
  mPositionKit = positionKit;
  emit positionKitChanged();
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

QVariant VariablesManager::getGeoPositionAttribute( const QGeoPositionInfo &info, QGeoPositionInfo::Attribute attribute )
{
  double value = std::numeric_limits< double >::quiet_NaN();
  if ( info.hasAttribute( attribute ) )
  {
    value = info.attribute( attribute );
    return QString::number( value, 'f', 2 );
  }
  else
    return QVariant();
}
