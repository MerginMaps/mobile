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
 * File: qgsquickmapcanvasmap.h by (C) 2014 by Matthias Kuhn
 */

#ifndef INPUTMAPCANVASMAP_H
#define INPUTMAPCANVASMAP_H

#include "inputmapsettings.h"

#include <QFutureSynchronizer>
#include <QTimer>
#include <QtQuick/QQuickItem>
#include <qgsmapsettings.h>
#include <qgspoint.h>

#include <memory>

class QgsMapRendererParallelJob;
class QgsMapRendererCache;
class QgsLabelingResults;

/**
 * \brief This class implements a visual Qt Quick Item that does map rendering
 * according to the current map settings. Client code is expected to use
 * MapCanvas item rather than using this class directly.
 *
 * InputMapCanvasMap instance internally creates InputMapSettings in
 * constructor. The QgsProject should be attached to the InputMapSettings.
 * The map settings for other Input components should be initialized from
 * InputMapCanvasMap's mapSettings
 *
 * \note QML Type: MapCanvasMap
 *
 * \sa InputMapCanvas
 */
class InputMapCanvasMap : public QQuickItem
{
    Q_OBJECT

    /**
     * The mapSettings property contains configuration for rendering of the map.
     *
     * It should be used as a primary source of map settings (and project) for
     * all other components in the application.
     *
     * This is a readonly property.
     */
    Q_PROPERTY( InputMapSettings *mapSettings READ mapSettings )

    /**
     * When freeze property is set to TRUE, the map canvas does not refresh.
     * The value temporary changes during the rendering process.
     */
    Q_PROPERTY( bool freeze READ freeze WRITE setFreeze NOTIFY freezeChanged )

    /**
     * The isRendering property is set to TRUE while a rendering job is pending for this map canvas map.
     * It can be used to show a notification icon about an ongoing rendering job.
     * This is a readonly property.
     */
    Q_PROPERTY( bool isRendering READ isRendering NOTIFY isRenderingChanged )

    /**
     * Interval in milliseconds after which the map canvas will be updated while a rendering job is ongoing.
     * This only has an effect if incrementalRendering is activated.
     * Default is 250 [ms].
     */
    Q_PROPERTY( int mapUpdateInterval READ mapUpdateInterval WRITE setMapUpdateInterval NOTIFY mapUpdateIntervalChanged )

    /**
     * When the incrementalRendering property is set to TRUE, the automatic refresh of map canvas during rendering is allowed.
     */
    Q_PROPERTY( bool incrementalRendering READ incrementalRendering WRITE setIncrementalRendering NOTIFY incrementalRenderingChanged )

    /**
     * When TRUE, the map is rendered to a square as big as the diagonal of the canvas, so the picture still covers the
     * whole canvas when it is rotated (by any angle) before the next rendering is finished.
     */
    Q_PROPERTY( bool overscan READ overscan WRITE setOverscan NOTIFY overscanChanged )

    /**
     * While the map is being rotated by rotate(), the map is rendered again only when it differs from the rendered
     * picture by at least this angle (in degrees) or when it stops rotating. In between, the rendered picture is rotated.
     * The picture is replaced only when the rendering is finished.
     */
    /**
     * TRUE when the rendered picture is big enough for the overscan (or the overscan is off). The map should not be rotated
     * before that, otherwise the corners of the canvas would be empty.
     */
    Q_PROPERTY( bool overscanReady READ overscanReady NOTIFY overscanReadyChanged )

    Q_PROPERTY( qreal rotationRenderThreshold READ rotationRenderThreshold WRITE setRotationRenderThreshold NOTIFY rotationRenderThresholdChanged )

  public:
    //! Create map canvas map
    explicit InputMapCanvasMap( QQuickItem *parent = nullptr );
    ~InputMapCanvasMap();

    QSGNode *updatePaintNode( QSGNode *oldNode, QQuickItem::UpdatePaintNodeData * ) override;

    //! \copydoc InputMapCanvasMap::mapSettings
    InputMapSettings *mapSettings() const;

    //! \copydoc InputMapCanvasMap::overscan
    bool overscan() const;
    void setOverscan( bool overscan );
    bool overscanReady() const;

    //! \copydoc InputMapCanvasMap::rotationRenderThreshold
    qreal rotationRenderThreshold() const;
    void setRotationRenderThreshold( qreal threshold );

    //! \copydoc InputMapCanvasMap::freeze
    bool freeze() const;

    //! \copydoc InputMapCanvasMap::freeze
    void setFreeze( bool freeze );

    //! \copydoc InputMapCanvasMap::isRendering
    bool isRendering() const;

    //! \copydoc InputMapCanvasMap::mapUpdateInterval
    int mapUpdateInterval() const;

    //! \copydoc InputMapCanvasMap::mapUpdateInterval
    void setMapUpdateInterval( int mapUpdateInterval );

    //! \copydoc InputMapCanvasMap::incrementalRendering
    bool incrementalRendering() const;

    //! \copydoc InputMapCanvasMap::incrementalRendering
    void setIncrementalRendering( bool incrementalRendering );

  signals:

    /**
     * Signal is emitted when a rendering is starting
     */
    void renderStarting();

    /**
     * Signal is emitted when a canvas is refreshed
     */
    void mapCanvasRefreshed();

    //! \copydoc InputMapCanvasMap::freeze
    void freezeChanged();
    void overscanChanged();
    void overscanReadyChanged();
    void rotationRenderThresholdChanged();

    //! \copydoc InputMapCanvasMap::isRendering
    void isRenderingChanged();

    //!\copydoc InputMapCanvasMap::mapUpdateInterval
    void mapUpdateIntervalChanged();

    //!\copydoc InputMapCanvasMap::incrementalRendering
    void incrementalRenderingChanged();

  protected:
#if QT_VERSION < QT_VERSION_CHECK( 6, 0, 0 )
    void geometryChanged( const QRectF &newGeometry, const QRectF &oldGeometry ) override;
#else
    void geometryChange( const QRectF &newGeometry, const QRectF &oldGeometry ) override;
#endif

  public slots:
    //! Stop map rendering
    void stopRendering();

    /**
     * Set map setting's extent (zoom the map) on the center by given scale
     */
    void zoom( QPointF center, qreal scale );

    /**
     * Rotates the map by given angle (in degrees, clockwise) around the given screen position
     */
    void rotate( QPointF center, qreal angle );

    /**
     * Set map setting's extent (pan the map) based on the difference of positions
     */
    void pan( QPointF oldPos, QPointF newPos );

    /**
     * Refresh the map canvas.
     * Does nothing when output size of map settings is not set
     */
    void refresh();

    /**
     * Clears rendering cache
     */
    void clearCache();

  private slots:
    void refreshMap();
    void renderJobUpdated();
    void renderJobFinished();
    void layerRepaintRequested( bool deferred );
    void onWindowChanged( QQuickWindow *window );
    void onScreenChanged( QScreen *screen );
    void onExtentChanged();
    void onLayersChanged();
    void onTemporalStateChanged();
    void checkRotationRender();
    void onRotationSettled();

  private:

    /**
     * Should only be called by stopRendering()!
     */
    void destroyJob( QgsMapRendererJob *job );
    QgsMapSettings prepareMapSettings() const;
    void updateTransform();
    void setImageFromJob();
    void zoomToFullExtent();
    void clearTemporalCache();
    void refreshForRotation();
    double rotationDifference() const;

    std::unique_ptr<InputMapSettings> mMapSettings;
    bool mPinching = false;
    QPoint mPinchStartPoint;
    QgsMapRendererParallelJob *mJob = nullptr;
    std::unique_ptr<QgsMapRendererCache> mCache;
    QgsLabelingResults *mLabelingResults = nullptr;
    QImage mImage;
    QgsMapSettings mImageMapSettings;
    QTimer mRefreshTimer;
    bool mDirty = false;
    bool mFreeze = false;
    QList<QMetaObject::Connection> mLayerConnections;
    QTimer mMapUpdateTimer;
    bool mIncrementalRendering = false;
    bool mSilentRefresh = false;
    bool mDeferredRefreshPending = false;
    bool mRotating = false; // inside of rotate(), see onExtentChanged
    bool mOverscan = false;
    bool mJobOverscan = false; // the last started job was rendered with the overscan
    bool mImageOverscan = false; // the current picture was rendered with the overscan
    qreal mRotationRenderThreshold = 45;
    bool mRotationRender = false; // the current rendering job was started because of the rotation
    bool mRotationRenderRequested = false; // the next rendering job will be started because of the rotation
    QTimer mRotationSettleTimer; // fires when the map stopped rotating

    QQuickWindow *mWindow = nullptr;
};

#endif // INPUTMAPCANVASMAP_H
