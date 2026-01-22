/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include "positiontrackingmanager.h"

#include "positionkit.h"

// backend implementations
#ifdef ANDROID
#include "androidtrackingbackend.h"
#elif defined( Q_OS_IOS )
#include "iostrackingbackend.h"
#else
#include "internaltrackingbackend.h"
#endif

#include "qgsproject.h"
#include "qgslinestring.h"

#include "coreutils.h"
#include "inpututils.h"
#include "variablesmanager.h"

PositionTrackingManager::PositionTrackingManager( QObject *parent )
  : QObject( parent )
{
  connect( &mElapsedTimeTextTimer, &QTimer::timeout, this, &PositionTrackingManager::elapsedTimeTextChanged );
}

void PositionTrackingManager::addPoint( const QgsPoint &position )
{
  if ( position.isEmpty() )
    return;

  int pointsCount = mTrackedGeometry.constGet()->vertexCount();

  mTrackedGeometry.get()->insertVertex( QgsVertexId( 0, 0, pointsCount ), position );

  emit trackedGeometryChanged( mTrackedGeometry );
}

void PositionTrackingManager::addPoints( const QList<QgsPoint> &positions )
{
  // get points from the previous geometry
  QgsLineString *oldLine = nullptr;
  oldLine = qgsgeometry_cast<QgsLineString *>( mTrackedGeometry.constGet() );

  if ( !oldLine )
  {
    qDebug() << "Error, tracked geometry is not a line";
    return;
  }

  QList<double> xValues = oldLine->xVector();
  QList<double> yValues = oldLine->yVector();
  QList<double> zValues = oldLine->zVector();
  QList<double> mValues = oldLine->mVector();

  QListIterator<QgsPoint> positionsIterator( positions );
  while ( positionsIterator.hasNext() )
  {
    const QgsPoint &p = positionsIterator.next();
    xValues += p.x();
    yValues += p.y();
    zValues += p.z();
    mValues += p.m();
  }

  QgsLineString *newLine = new QgsLineString( xValues, yValues, zValues, mValues );
  mTrackedGeometry.set( newLine );

  emit trackedGeometryChanged( mTrackedGeometry );
}

void PositionTrackingManager::commitTrackedPath()
{
  if ( !mQgsProject )
  {
    CoreUtils::log( QStringLiteral( "Position tracking" ), QStringLiteral( "Can not save tracking feature, missing required properties" ) );
    return;
  }

  QgsVectorLayer *trackingLayer = mQgsProject->mapLayer<QgsVectorLayer *>( mLayerId );

  if ( !trackingLayer || !trackingLayer->isValid() )
  {
    CoreUtils::log( QStringLiteral( "Position tracking" ), QStringLiteral( "Can not save tracking feature, layer is missing or invalid" ) );
    return;
  }

  // convert captured geometry to the destination layer's CRS
  QgsGeometry geometryInLayerCRS = InputUtils::transformGeometry( mTrackedGeometry, PositionKit::positionCrs3D(), trackingLayer );

  // create feature - add tracking variables to scope
  QgsExpressionContextScope *scope = new QgsExpressionContextScope( QStringLiteral( "MM_Tracking" ) );
  // ownership of the scope will be moved to QgsExpressionContext when the feature is created
  scope->addVariable( QgsExpressionContextScope::StaticVariable( QStringLiteral( "tracking_start_time" ), mTrackingStartTime, true, true ) );
  scope->addVariable( QgsExpressionContextScope::StaticVariable( QStringLiteral( "tracking_end_time" ), QDateTime::currentDateTime(), true, true ) );

  FeatureLayerPair trackedFeature;

  if ( mVariablesManager )
  {
    trackedFeature = InputUtils::createFeatureLayerPair( trackingLayer, geometryInLayerCRS, mVariablesManager, scope );
  }
  else
  {
    trackedFeature = InputUtils::createFeatureLayerPair( trackingLayer, geometryInLayerCRS, nullptr, scope );
  }

  trackingLayer->startEditing();
  trackingLayer->addFeature( trackedFeature.featureRef() );

  if ( !trackingLayer->commitChanges() )
  {
    CoreUtils::log( QStringLiteral( "CommitChanges" ),
                    QStringLiteral( "Failed to commit changes:\n%1" )
                    .arg( trackingLayer->commitErrors().join( QLatin1Char( '\n' ) ) ) );

    trackingLayer->rollBack();
  }
  else
  {
    trackingLayer->triggerRepaint();

    CoreUtils::log(
      QStringLiteral( "Position tracking" ),
      QStringLiteral( "Feature %1 commited" ).arg( trackedFeature.featureRef().id() )
    );
  }
}

void PositionTrackingManager::setup()
{
  if ( !mTrackingBackend || !mQgsProject )
  {
    return;
  }

  //
  // find the tracking layer
  //

  QString trackingLayerId = mQgsProject->readEntry( QStringLiteral( "Mergin" ), QStringLiteral( "PositionTracking/TrackingLayer" ), QStringLiteral() );

  if ( trackingLayerId.isEmpty() )
  {
    emit trackingErrorOccured( tr( "Could not find tracking layer for the project" ) );
    CoreUtils::log( QStringLiteral( "Position tracking" ), QStringLiteral( "Could not find tracking layer for the project" ) );
    return;
  }

  QgsVectorLayer *trackingLayer = mQgsProject->mapLayer<QgsVectorLayer *>( trackingLayerId );

  if ( !trackingLayer || !trackingLayer->isValid() )
  {
    emit trackingErrorOccured( tr( "Tracking layer not found or invalid" ) );
    CoreUtils::log( QStringLiteral( "Position tracking" ), QStringLiteral( "Tracking layer not found or invalid" ) );
    return;
  }

  setLayerId( trackingLayerId );

  mTrackingStartTime = QDateTime::currentDateTime();
  emit startTimeChanged( mTrackingStartTime );

  //
  // build the track line
  //  - in case of direct calls from the tracking backend we start with clear line
  //  - in case of updates through file we want to read the file first
  //

  if ( mTrackingBackend->trackingMethod() == AbstractTrackingBackend::TrackingMethod::UpdatesThroughDirectCall )
  {
    QgsLineString *line = new QgsLineString();
    mTrackedGeometry.set( line );
  }
  else if ( mTrackingBackend->trackingMethod() == AbstractTrackingBackend::TrackingMethod::UpdatesThroughFile )
  {
    QList<QgsPoint> positions = mTrackingBackend->getAllUpdates();
    QgsLineString *line = new QgsLineString( positions );
    mTrackedGeometry.set( line );
  }

  if ( mTrackingBackend->signalSlotSupport() == AbstractTrackingBackend::SignalSlotSupport::Supported )
  {
    connect( mTrackingBackend.get(), &AbstractTrackingBackend::positionChanged, this, &PositionTrackingManager::addPoint );
    connect( mTrackingBackend.get(), &AbstractTrackingBackend::multiplePositionChanges, this, &PositionTrackingManager::addPoints );
  }
  else
  {
    mTrackingBackend->setNotifyFunction( [ = ]( const QgsPoint & p ) { this->addPoint( p ); } );
  }

  connect( mTrackingBackend.get(), &AbstractTrackingBackend::errorOccured, this, &PositionTrackingManager::trackingErrorOccured );
  connect( mTrackingBackend.get(), &AbstractTrackingBackend::abort, this, &PositionTrackingManager::abort );

  mIsTrackingPosition = true;
  emit isTrackingPositionChanged( true );

  // update highlight if the geometry is not empty
  if ( mTrackedGeometry.constGet() && mTrackedGeometry.constGet()->vertexCount() )
  {
    emit trackedGeometryChanged( mTrackedGeometry );
  }

  mElapsedTimeTextTimer.start( 1000 );
}

AbstractTrackingBackend *PositionTrackingManager::constructTrackingBackend( QgsProject *project, PositionKit *positionKit )
{
  AbstractTrackingBackend *positionBackend = nullptr;
  AbstractTrackingBackend::UpdateFrequency frequency = AbstractTrackingBackend::Normal;

  if ( project )
  {
    int readFrequency = project->readNumEntry( QStringLiteral( "Mergin" ), QStringLiteral( "PositionTracking/UpdateFrequency" ), 0 );

    if ( readFrequency == 0 )
    {
      frequency = AbstractTrackingBackend::Often;
    }
    else if ( readFrequency == 1 )
    {
      frequency = AbstractTrackingBackend::Normal;
    }
    else
    {
      frequency = AbstractTrackingBackend::Occasional;
    }
  }

#ifdef ANDROID
  positionBackend = new AndroidTrackingBackend( frequency );
#elif defined( Q_OS_IOS )
  positionBackend = new IOSTrackingBackend( frequency );
#else
  // desktop
  if ( positionKit )
  {
    positionBackend = new InternalTrackingBackend( positionKit, frequency );
  }
  else
  {
    CoreUtils::log(
      QStringLiteral( "Position tracking" ),
      QStringLiteral( "Requested tracking on desktop, but no position kit provided" )
    );
  }
#endif

  QQmlEngine::setObjectOwnership( positionBackend, QQmlEngine::CppOwnership );

  return positionBackend;
}

QString PositionTrackingManager::layerId() const
{
  return mLayerId;
}

void PositionTrackingManager::setLayerId( QString newLayerId )
{
  if ( mLayerId == newLayerId )
    return;
  mLayerId = newLayerId;
  emit layerIdChanged( mLayerId );
}

QgsGeometry PositionTrackingManager::trackedGeometry() const
{
  return mTrackedGeometry;
}

AbstractTrackingBackend *PositionTrackingManager::trackingBackend() const
{
  return mTrackingBackend.get();
}

void PositionTrackingManager::setTrackingBackend( AbstractTrackingBackend *newTrackingBackend )
{
  if ( mTrackingBackend.get() == newTrackingBackend )
  {
    return;
  }

  if ( mTrackingBackend )
  {
    disconnect( mTrackingBackend.get() );
  }

  mTrackingBackend.reset( newTrackingBackend );

  emit trackingBackendChanged( mTrackingBackend.get() );

  setup();
}

QgsProject *PositionTrackingManager::qgsProject() const
{
  return mQgsProject;
}

void PositionTrackingManager::setQgsProject( QgsProject *newQgsProject )
{
  if ( mQgsProject == newQgsProject )
    return;
  mQgsProject = newQgsProject;
  emit qgsProjectChanged( mQgsProject );

  setup();
}

VariablesManager *PositionTrackingManager::variablesManager() const
{
  return mVariablesManager;
}

void PositionTrackingManager::setVariablesManager( VariablesManager *newVariablesManager )
{
  if ( mVariablesManager == newVariablesManager )
    return;
  mVariablesManager = newVariablesManager;
  emit variablesManagerChanged( mVariablesManager );
}

QgsCoordinateReferenceSystem PositionTrackingManager::crs() const
{
  return PositionKit::positionCrs3D();
}

void PositionTrackingManager::tryAgain()
{
  if ( !mTrackingBackend )
  {
    emit trackingErrorOccured( tr( "Failed to start tracking, please contact support" ) );
    CoreUtils::log( QStringLiteral( "Position tracking" ), QStringLiteral( "Try again failed, no position provider" ) );
  }

  setup();
}

QDateTime PositionTrackingManager::startTime() const
{
  return mTrackingStartTime;
}

QString PositionTrackingManager::elapsedTimeText() const
{
  if ( !mIsTrackingPosition )
    return QString();

  qint64 totalSecs = mTrackingStartTime.secsTo( QDateTime::currentDateTime() );
  int hours = totalSecs / 3600;
  totalSecs = totalSecs % 3600;
  int minutes = totalSecs / 60;
  int seconds = totalSecs % 60;

  return QString( "%1:%2:%3" ).arg( hours, 2, 10, QChar( '0' ) ).arg( minutes, 2, 10, QChar( '0' ) ).arg( seconds, 2, 10, QChar( '0' ) );
}

bool PositionTrackingManager::isTrackingPosition() const
{
  return mIsTrackingPosition;
}
