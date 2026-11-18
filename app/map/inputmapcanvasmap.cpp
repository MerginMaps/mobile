/**************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

/*
 * The source code forked from https://github.com/qgis/QGIS on 25th Nov 2022
 * File: qgsquickmapcanvasmap.cpp by (C) 2014 by Matthias Kuhn
 */

#include <QQuickWindow>
#include <QSGSimpleTextureNode>
#include <QScreen>
#include <algorithm>
#include <cmath>

#include "qgis.h"
#include "qgsexpressioncontextutils.h"
#include "qgsmaplayertemporalproperties.h"
#include "qgsmaprenderercache.h"
#include "qgsmaprendererparalleljob.h"
#include "qgsmessagelog.h"
#include "qgspallabeling.h"
#include "qgsproject.h"
#include "qgsannotationlayer.h"
#include "qgsvectorlayer.h"
#include "qgslabelingresults.h"

#include "inputmapcanvasmap.h"
#include "inputmapsettings.h"

// the picture is not rendered again when the map is rotated less than this angle (in degrees) from it
static constexpr double ROTATION_TOLERANCE = 0.5;
// the biggest side (in pixels) of the picture rendered for the overscan, keeps it within texture size limits of the GPUs
static constexpr int MAX_OVERSCAN_SIDE = 4096;
// longest pause between two renderings started by the rotation, see rotationRenderAllowed()
static constexpr qint64 MAX_ROTATION_RENDER_REST_MS = 2000;
// a rendering whose start blocks the UI at least this long causes a visible hiccup, so the pause after it is longer
static constexpr qint64 SLOW_RENDER_START_MS = 30;
static constexpr qint64 MIN_REST_AFTER_SLOW_START_MS = 1000;
// the map stopped rotating after this time without rotation
static constexpr int ROTATION_SETTLE_TIME_MS = 400;
// when the map stops this far from the picture, it is rendered right away, without the pause (tilted labels look wrong)
static constexpr double ROTATION_SETTLE_RENDER_NOW_DEG = 10;
// tolerance for rounding errors when checking that the picture covers the canvas
static constexpr double PICTURE_EDGE_TOLERANCE_PX = 1;

InputMapCanvasMap::InputMapCanvasMap( QQuickItem *parent )
  : QQuickItem( parent )
  , mMapSettings( std::make_unique<InputMapSettings>() )
  , mCache( std::make_unique<QgsMapRendererCache>() )
{
  connect( this, &QQuickItem::windowChanged, this, &InputMapCanvasMap::onWindowChanged );
  connect( &mRefreshTimer, &QTimer::timeout, this, [ = ] { refreshMap(); } );
  connect( &mMapUpdateTimer, &QTimer::timeout, this, &InputMapCanvasMap::renderJobUpdated );

  connect( mMapSettings.get(), &InputMapSettings::extentChanged, this, &InputMapCanvasMap::onExtentChanged );
  connect( mMapSettings.get(), &InputMapSettings::rotationChanged, this, [this]
  {
    // rotate() announces the extent change right after, handled once there
    if ( !mRotating )
      onExtentChanged();
  } );
  connect( mMapSettings.get(), &InputMapSettings::layersChanged, this, &InputMapCanvasMap::onLayersChanged );
  connect( mMapSettings.get(), &InputMapSettings::temporalStateChanged, this, &InputMapCanvasMap::onTemporalStateChanged );

  connect( this, &InputMapCanvasMap::renderStarting, this, &InputMapCanvasMap::isRenderingChanged );
  connect( this, &InputMapCanvasMap::mapCanvasRefreshed, this, &InputMapCanvasMap::isRenderingChanged );

  mMapUpdateTimer.setSingleShot( false );
  mMapUpdateTimer.setInterval( 250 );
  mRefreshTimer.setSingleShot( true );
  mRotationSettleTimer.setSingleShot( true );
  // onRotationSettled() changes the interval to wait for the pause, so always start it with an explicit interval
  mRotationSettleTimer.setInterval( ROTATION_SETTLE_TIME_MS );
  connect( &mRotationSettleTimer, &QTimer::timeout, this, &InputMapCanvasMap::onRotationSettled );
  setTransformOrigin( QQuickItem::TopLeft );
  setFlags( QQuickItem::ItemHasContents );
}

InputMapCanvasMap::~InputMapCanvasMap()
{
  stopRendering();
}

InputMapSettings *InputMapCanvasMap::mapSettings() const
{
  return mMapSettings.get();
}

void InputMapCanvasMap::zoom( QPointF center, qreal scale )
{
  QgsRectangle extent = mMapSettings->extent();
  QgsPoint oldCenter( extent.center() );
  QgsPoint mousePos( mMapSettings->screenToCoordinate( center ) );

  QgsPointXY newCenter( mousePos.x() + ( ( oldCenter.x() - mousePos.x() ) * scale ),
                        mousePos.y() + ( ( oldCenter.y() - mousePos.y() ) * scale ) );

  // same as zoomWithCenter (no coordinate transformations are needed)
  extent.scale( scale, &newCenter );
  mMapSettings->setExtent( extent );
}

void InputMapCanvasMap::rotate( QPointF center, qreal angle )
{
  mRotating = true;
  mMapSettings->rotate( center, angle );
  mRotating = false;
}

void InputMapCanvasMap::pan( QPointF oldPos, QPointF newPos )
{
  QgsPoint start = mMapSettings->screenToCoordinate( oldPos.toPoint() );
  QgsPoint end = mMapSettings->screenToCoordinate( newPos.toPoint() );

  double dx = end.x() - start.x();
  double dy = end.y() - start.y();

  // modify the extent
  QgsRectangle extent = mMapSettings->extent();

  extent.setXMinimum( extent.xMinimum() + dx );
  extent.setXMaximum( extent.xMaximum() + dx );
  extent.setYMaximum( extent.yMaximum() + dy );
  extent.setYMinimum( extent.yMinimum() + dy );

  mMapSettings->setExtent( extent );
}

void InputMapCanvasMap::refreshMap()
{
  // the start of the rendering blocks the UI thread, see rotationRenderRest()
  QElapsedTimer startDuration;
  startDuration.start();

  stopRendering(); // if any...

  QgsMapSettings mapSettings = mMapSettings->mapSettings();
  if ( !mapSettings.hasValidSettings() )
    return;

  //build the expression context
  QgsExpressionContext expressionContext;
  expressionContext << QgsExpressionContextUtils::globalScope()
                    << QgsExpressionContextUtils::mapSettingsScope( mapSettings );

  QgsProject *project = mMapSettings->project();
  if ( project )
  {
    expressionContext << QgsExpressionContextUtils::projectScope( project );

    mapSettings.setLabelingEngineSettings( project->labelingEngineSettings() );

    // render main annotation layer above all other layers
    QList<QgsMapLayer *> allLayers = mapSettings.layers();
    allLayers.insert( 0, project->mainAnnotationLayer() );
    mapSettings.setLayers( allLayers );
  }

  mapSettings.setExpressionContext( expressionContext );

  mJobOverscan = mOverscan;

  if ( mOverscan )
  {
    // render a square as big as the diagonal of the canvas (same scale, same center), so the picture covers
    // the canvas also when it is rotated by any angle
    const QSize size = mapSettings.outputSize();
    const int side = std::min( MAX_OVERSCAN_SIDE, static_cast<int>( std::ceil( std::hypot( size.width(), size.height() ) ) ) );
    const QgsPointXY center = mapSettings.extent().center();
    const double halfSize = side * mapSettings.mapUnitsPerPixel() / 2;

    mapSettings.setOutputSize( QSize( side, side ) );
    mapSettings.setExtent( QgsRectangle( center.x() - halfSize, center.y() - halfSize, center.x() + halfSize, center.y() + halfSize ) );
  }

  // enables on-the-fly simplification of geometries to spend less time rendering
  mapSettings.setFlag( Qgis::MapSettingsFlag::UseRenderingOptimization );
  // with incremental rendering - enables updates of partially rendered layers (good for WMTS, XYZ layers)
  // (the picture is not replaced by partial results when the rendering was started by the rotation)
  mRotationRender = mRotationRenderRequested;
  mRotationRenderRequested = false;
  const bool partialOutput = mIncrementalRendering && !mRotationRender;
  mapSettings.setFlag( Qgis::MapSettingsFlag::RenderPartialOutput, partialOutput );

  // create the renderer job
  Q_ASSERT( !mJob );
  mJob = new QgsMapRendererParallelJob( mapSettings );

  if ( partialOutput )
    mMapUpdateTimer.start();

  connect( mJob, &QgsMapRendererJob::renderingLayersFinished, this, &InputMapCanvasMap::renderJobUpdated );
  connect( mJob, &QgsMapRendererJob::finished, this, &InputMapCanvasMap::renderJobFinished );
  mJob->setCache( mCache.get() );

  mJob->start();
  mRenderTimer.start();
  mLastRenderStartDuration = startDuration.elapsed();

  if ( !mSilentRefresh )
  {
    emit renderStarting();
  }
}

void InputMapCanvasMap::setImageFromJob()
{
  mImage = mJob->renderedImage();
  mImageMapSettings = mJob->mapSettings();

  if ( mImageOverscan != mJobOverscan )
  {
    mImageOverscan = mJobOverscan;
    emit overscanReadyChanged();
  }
}

void InputMapCanvasMap::renderJobUpdated()
{
  if ( !mJob || mRotationRender )
    return;

  setImageFromJob();
  mDirty = true;
  // Temporarily freeze the canvas, we only need to reset the geometry but not trigger a repaint
  bool freeze = mFreeze;
  mFreeze = true;
  updateTransform();
  mFreeze = freeze;

  update();
}

void InputMapCanvasMap::renderJobFinished()
{
  if ( !mJob )
    return;

  const QgsMapRendererJob::Errors errors = mJob->errors();
  for ( const QgsMapRendererJob::Error &error : errors )
  {
    QgsMessageLog::logMessage( QStringLiteral( "%1 :: %2" ).arg( error.layerID, error.message ), QStringLiteral( "Rendering" ) );
  }

  // take labeling results before emitting renderComplete, so labeling map tools
  // connected to signal work with correct results
  delete mLabelingResults;
  mLabelingResults = mJob->takeLabelingResults();

  setImageFromJob();

  mLastRenderDuration = mRenderTimer.elapsed();
  mRenderRestTimer.start();

  // now we are in a slot called from mJob - do not delete it immediately
  // so the class is still valid when the execution returns to the class
  mJob->deleteLater();
  mJob = nullptr;
  mDirty = true;
  mMapUpdateTimer.stop();

  // Temporarily freeze the canvas, we only need to reset the geometry but not trigger a repaint
  bool freeze = mFreeze;
  mFreeze = true;
  updateTransform();
  mFreeze = freeze;

  update();
  if ( !mSilentRefresh )
  {
    emit mapCanvasRefreshed();
  }
  else
  {
    mSilentRefresh = false;
  }

  if ( mDeferredRefreshPending )
  {
    mDeferredRefreshPending = false;
    mSilentRefresh = true;
    refresh();
  }
  else if ( mRotationRender )
  {
    // the map could have been rotated more while it was rendered
    checkRotationRender();
  }
}

void InputMapCanvasMap::layerRepaintRequested( bool deferred )
{
  if ( mMapSettings->outputSize().isNull() )
    return; // the map image size has not been set yet

  if ( !mFreeze )
  {
    if ( deferred )
    {
      if ( !mJob )
      {
        mSilentRefresh = true;
        refresh();
      }
      else
      {
        mDeferredRefreshPending = true;
      }
    }
    else
    {
      refresh();
    }
  }
}

void InputMapCanvasMap::onWindowChanged( QQuickWindow *window )
{
  if ( mWindow == window )
    return;

  if ( mWindow )
    disconnect( mWindow, &QQuickWindow::screenChanged, this, &InputMapCanvasMap::onScreenChanged );

  if ( window )
  {
    connect( window, &QQuickWindow::screenChanged, this, &InputMapCanvasMap::onScreenChanged );
    onScreenChanged( window->screen() );
  }

  mWindow = window;
}

void InputMapCanvasMap::onScreenChanged( QScreen *screen )
{
  if ( screen )
  {
    if ( screen->devicePixelRatio() > 0 )
    {
      mMapSettings->setDevicePixelRatio( screen->devicePixelRatio() );
    }
    mMapSettings->setOutputDpi( screen->physicalDotsPerInch() );
    // Re-apply logical size so setOutputSize re-scales it with the new DPR
    mMapSettings->setOutputSize( boundingRect().size().toSize() );
    refresh();
  }
}

void InputMapCanvasMap::onExtentChanged()
{
  updateTransform();

  if ( mRotating )
  {
    // The map is being rotated continuously (e.g. animation). The rendered picture is rotated, and the map
    // is rendered again only when needed, otherwise the running rendering would be restarted on every step.
    if ( !mFreeze )
      checkRotationRender();
    return;
  }

  // And trigger a new rendering job
  refresh();
}

void InputMapCanvasMap::checkRotationRender()
{
  if ( mFreeze || mMapSettings->outputSize().isNull() )
    return;

  const double diff = rotationDifference();
  if ( diff < ROTATION_TOLERANCE )
    return;

  if ( mJob )
  {
    // do not cancel the running rendering, wait for it. A rendering started by the rotation checks again when it is
    // finished, for the others we check when the map stops rotating
    if ( !mRotationRender )
      mRotationSettleTimer.start( ROTATION_SETTLE_TIME_MS );
    return;
  }

  if ( diff >= mRotationRenderThreshold && rotationRenderAllowed() )
    refreshForRotation();
  else
    mRotationSettleTimer.start( ROTATION_SETTLE_TIME_MS ); // restarted on every step, so it fires when the map stops rotating
}

void InputMapCanvasMap::onRotationSettled()
{
  if ( mFreeze || mMapSettings->outputSize().isNull() )
    return;

  if ( mJob )
  {
    // try again when the running rendering is (probably) finished
    mRotationSettleTimer.start( ROTATION_SETTLE_TIME_MS );
    return;
  }

  const double diff = rotationDifference();
  if ( diff < ROTATION_TOLERANCE )
    return;

  // The map stopped: a visibly tilted picture is replaced right away, small differences (e.g. the device
  // wobbling in follow-direction) wait for the pause, otherwise the map would render all the time.
  if ( diff >= ROTATION_SETTLE_RENDER_NOW_DEG || rotationRenderAllowed() )
    refreshForRotation();
  else
    mRotationSettleTimer.start( static_cast<int>( std::max<qint64>( rotationRenderRest(), 1 ) ) ); // render when the pause ends
}

double InputMapCanvasMap::rotationDifference() const
{
  double diff = std::fmod( mMapSettings->rotation() - mImageMapSettings.rotation(), 360.0 );
  if ( diff > 180 )
    diff -= 360;
  else if ( diff < -180 )
    diff += 360;
  return std::abs( diff );
}

bool InputMapCanvasMap::pictureCoversCanvas() const
{
  const QSize imageSize = mImageMapSettings.outputSize();
  if ( mImage.isNull() || imageSize.isEmpty() )
    return false;

  const QSizeF canvasSize = boundingRect().size();
  const QPointF corners[] = { QPointF( 0, 0 ), QPointF( canvasSize.width(), 0 ), QPointF( 0, canvasSize.height() ), QPointF( canvasSize.width(), canvasSize.height() ) };

  for ( const QPointF &corner : corners )
  {
    // canvas corner in picture pixels
    const QgsPointXY pt = mImageMapSettings.mapToPixel().transform( QgsPointXY( mMapSettings->screenToCoordinate( corner ) ) );
    if ( pt.x() < -PICTURE_EDGE_TOLERANCE_PX || pt.y() < -PICTURE_EDGE_TOLERANCE_PX || pt.x() > imageSize.width() + PICTURE_EDGE_TOLERANCE_PX || pt.y() > imageSize.height() + PICTURE_EDGE_TOLERANCE_PX )
      return false;
  }

  return true;
}

bool InputMapCanvasMap::rotationRenderAllowed() const
{
  // While the map keeps rotating, it pauses after each rendering for as long as the rendering took and rotates the old
  // picture meanwhile, so heavy projects do not render nonstop. Projects whose rendering start blocks the UI pause
  // longer, see rotationRenderRest().
  if ( !mRenderRestTimer.isValid() )
    return true;

  // empty parts of the canvas are worse than rendering too often
  if ( !pictureCoversCanvas() )
    return true;

  return rotationRenderRest() == 0;
}

qint64 InputMapCanvasMap::rotationRenderRest() const
{
  if ( !mRenderRestTimer.isValid() )
    return 0;

  return rotationRenderRest( mLastRenderDuration, mLastRenderStartDuration, mRenderRestTimer.elapsed() );
}

qint64 InputMapCanvasMap::rotationRenderRest( qint64 lastRenderDuration, qint64 lastRenderStartDuration, qint64 sinceRenderFinished )
{
  qint64 rest = std::min( lastRenderDuration, MAX_ROTATION_RENDER_REST_MS );
  if ( lastRenderStartDuration >= SLOW_RENDER_START_MS )
    rest = std::max( rest, MIN_REST_AFTER_SLOW_START_MS );

  return std::max<qint64>( 0, rest - sinceRenderFinished );
}

void InputMapCanvasMap::refreshForRotation()
{
  mRotationSettleTimer.stop();
  mRotationRenderRequested = true;
  mRefreshTimer.start( 1 );
}


void InputMapCanvasMap::onTemporalStateChanged()
{
  clearTemporalCache();

  // And trigger a new rendering job
  refresh();
}

void InputMapCanvasMap::updateTransform()
{
  setScale( mImageMapSettings.mapUnitsPerPixel() / mMapSettings->mapSettings().mapUnitsPerPixel() );

  // the rendered image may have been rendered with a different rotation, so we place its top left corner
  // and rotate it (around that corner, see transformOrigin) to match the current view
  const QgsPointXY imageOrigin = mImageMapSettings.mapToPixel().toMapCoordinates( 0, 0 );
  const QgsPointXY pixelPt = mMapSettings->coordinateToScreen( QgsPoint( imageOrigin ) );
  setRotation( mMapSettings->rotation() - mImageMapSettings.rotation() );
  setX( pixelPt.x() );
  setY( pixelPt.y() );
}

int InputMapCanvasMap::mapUpdateInterval() const
{
  return mMapUpdateTimer.interval();
}

void InputMapCanvasMap::setMapUpdateInterval( int mapUpdateInterval )
{
  if ( mMapUpdateTimer.interval() == mapUpdateInterval )
    return;

  mMapUpdateTimer.setInterval( mapUpdateInterval );

  emit mapUpdateIntervalChanged();
}

bool InputMapCanvasMap::incrementalRendering() const
{
  return mIncrementalRendering;
}

void InputMapCanvasMap::setIncrementalRendering( bool incrementalRendering )
{
  if ( incrementalRendering == mIncrementalRendering )
    return;

  mIncrementalRendering = incrementalRendering;
  emit incrementalRenderingChanged();
}

bool InputMapCanvasMap::overscan() const
{
  return mOverscan;
}

void InputMapCanvasMap::setOverscan( bool overscan )
{
  if ( mOverscan == overscan )
    return;

  mOverscan = overscan;

  // render the picture with the new size, it replaces the old one when it is finished
  refreshForRotation();

  emit overscanChanged();
  emit overscanReadyChanged();
}

bool InputMapCanvasMap::overscanReady() const
{
  return !mOverscan || mImageOverscan || mMapSettings->outputSize().isNull();
}

qreal InputMapCanvasMap::rotationRenderThreshold() const
{
  return mRotationRenderThreshold;
}

void InputMapCanvasMap::setRotationRenderThreshold( qreal threshold )
{
  if ( qgsDoubleNear( mRotationRenderThreshold, threshold ) )
    return;

  mRotationRenderThreshold = threshold;
  emit rotationRenderThresholdChanged();
}

bool InputMapCanvasMap::freeze() const
{
  return mFreeze;
}

void InputMapCanvasMap::setFreeze( bool freeze )
{
  if ( freeze == mFreeze )
    return;

  mFreeze = freeze;

  if ( mFreeze )
    stopRendering();
  else
    refresh();

  emit freezeChanged();
}

bool InputMapCanvasMap::isRendering() const
{
  return mJob;
}

QSGNode *InputMapCanvasMap::updatePaintNode( QSGNode *oldNode, QQuickItem::UpdatePaintNodeData * )
{
  if ( mDirty )
  {
    delete oldNode;
    oldNode = nullptr;
    mDirty = false;
  }

  QSGSimpleTextureNode *node = static_cast<QSGSimpleTextureNode *>( oldNode );
  if ( !node )
  {
    node = new QSGSimpleTextureNode();
    QSGTexture *texture = window()->createTextureFromImage( mImage );
    node->setTexture( texture );
    node->setOwnsTexture( true );
  }

  // the picture can have a different size than the canvas (overscan, resize), its top left corner is placed by updateTransform()
  QSizeF size = mImage.size();
  if ( !size.isEmpty() )
    size /= mMapSettings->devicePixelRatio();

  const QRectF rect( QPointF( 0, 0 ), size );

  node->setRect( rect );

  return node;
}

#if QT_VERSION < QT_VERSION_CHECK( 6, 0, 0 )
void InputMapCanvasMap::geometryChanged( const QRectF &newGeometry, const QRectF &oldGeometry )
{
  QQuickItem::geometryChanged( newGeometry, oldGeometry );
#else
void InputMapCanvasMap::geometryChange( const QRectF &newGeometry, const QRectF &oldGeometry )
{
  QQuickItem::geometryChange( newGeometry, oldGeometry );
#endif
  if ( newGeometry.size() != oldGeometry.size() )
  {
    mMapSettings->setOutputSize( newGeometry.size().toSize() );

    if ( !mMapSettings->outputSize().isNull() )
    {
      // keep the old image at the right place until the new one is rendered
      updateTransform();
      refresh();
    }
  }
}

void InputMapCanvasMap::onLayersChanged()
{
  if ( mMapSettings->extent().isEmpty() )
    zoomToFullExtent();

  for ( const QMetaObject::Connection &conn : std::as_const( mLayerConnections ) )
  {
    disconnect( conn );
  }
  mLayerConnections.clear();

  const QList<QgsMapLayer *> layers = mMapSettings->layers();
  for ( QgsMapLayer *layer : layers )
  {
    mLayerConnections << connect( layer, &QgsMapLayer::repaintRequested, this, &InputMapCanvasMap::layerRepaintRequested );
  }

  refresh();
}

void InputMapCanvasMap::destroyJob( QgsMapRendererJob *job )
{
  job->cancel();
  job->deleteLater();
}

void InputMapCanvasMap::stopRendering()
{
  if ( mJob )
  {
    mMapUpdateTimer.stop();

    disconnect( mJob, &QgsMapRendererJob::renderingLayersFinished, this, &InputMapCanvasMap::renderJobUpdated );
    disconnect( mJob, &QgsMapRendererJob::finished, this, &InputMapCanvasMap::renderJobFinished );

    if ( !mJob->isActive() )
      mJob->deleteLater();
    else
      connect( mJob, &QgsMapRendererJob::finished, mJob, &QObject::deleteLater );

    mJob->cancelWithoutBlocking();
    mJob = nullptr;
  }
}

void InputMapCanvasMap::zoomToFullExtent()
{
  QgsRectangle extent;
  const QList<QgsMapLayer *> layers = mMapSettings->layers();
  for ( QgsMapLayer *layer : layers )
  {
    if ( mMapSettings->destinationCrs() != layer->crs() )
    {
      QgsCoordinateTransform transform( layer->crs(), mMapSettings->destinationCrs(), mMapSettings->transformContext() );
      try
      {
        extent.combineExtentWith( transform.transformBoundingBox( layer->extent() ) );
      }
      catch ( const QgsCsException &exp )
      {
        // Ignore extent if it can't be transformed
      }
    }
    else
    {
      extent.combineExtentWith( layer->extent() );
    }
  }
  mMapSettings->setExtent( extent );

  refresh();
}

void InputMapCanvasMap::refresh()
{
  if ( mMapSettings->outputSize().isNull() )
    return; // the map image size has not been set yet

  if ( !mFreeze )
  {
    mRotationRenderRequested = false;
    mRefreshTimer.start( 1 );
  }
}

void InputMapCanvasMap::clearCache()
{
  if ( mCache )
    mCache->clear();
}

void InputMapCanvasMap::clearTemporalCache()
{
  if ( mCache )
  {
    bool invalidateLabels = false;
    const QList<QgsMapLayer *> layerList = mMapSettings->mapSettings().layers();
    for ( QgsMapLayer *layer : layerList )
    {
      if ( layer->temporalProperties() && layer->temporalProperties()->isActive() )
      {
        if ( QgsVectorLayer *vl = qobject_cast<QgsVectorLayer *>( layer ) )
        {
          if ( vl->labelsEnabled() || vl->diagramsEnabled() )
            invalidateLabels = true;
        }

        if ( layer->temporalProperties()->flags() & QgsTemporalProperty::FlagDontInvalidateCachedRendersWhenRangeChanges )
          continue;

        mCache->invalidateCacheForLayer( layer );
      }
    }

    if ( invalidateLabels )
    {
      mCache->clearCacheImage( QStringLiteral( "_labels_" ) );
      mCache->clearCacheImage( QStringLiteral( "_preview_labels_" ) );
    }
  }
}

