/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef RECORDINGMAPTOOL_H
#define RECORDINGMAPTOOL_H

#include "abstractmaptool.h"

#include <QObject>
#include <qglobal.h>

#include "qgsvertexid.h"
#include "qgsgeometry.h"
#include "qgsvectorlayer.h"
#include "position/positionkit.h"
#include "inpututils.h"
#include "streamingintervaltype.h"

class PositionKit;
class QgsVectorLayer;

class Vertex
{
    Q_GADGET

    Q_PROPERTY( QgsVertexId vertexId READ vertexId WRITE setVertexId )
    Q_PROPERTY( QgsPoint coordinates READ coordinates WRITE setCoordinates )
    Q_PROPERTY( VertexType type READ type WRITE setType )

  public:

    enum VertexType
    {
      Existing = 0, // Existing vertex in geometry
      MidPoint, // Node between two existing vertices
      HandleStart, // Node before beginning of line
      HandleEnd // Node after end of line
    };
    Q_ENUM( VertexType );

    Vertex();
    Vertex( QgsVertexId id, QgsPoint coordinates, VertexType type );

    virtual ~Vertex();

    Q_INVOKABLE bool isValid() const;

    const QgsVertexId &vertexId() const;
    void setVertexId( const QgsVertexId &newVertexId );

    QgsPoint coordinates() const;
    void setCoordinates( QgsPoint newCoordinates );

    const VertexType &type() const;
    void setType( const VertexType &newType );

    bool operator==( const Vertex &other ) const
    {
      return other.vertexId() == mVertexId && InputUtils::equals( other.coordinates(), mCoordinates ) && other.type() == mType;
    }

    bool operator!=( const Vertex &other ) const
    {
      return !( *this == other );
    }

  private:
    QgsVertexId mVertexId;
    QgsPoint mCoordinates;
    VertexType mType = VertexType::Existing;
};

class RecordingMapTool : public AbstractMapTool
{
    Q_OBJECT

    Q_PROPERTY( bool centeredToGPS READ centeredToGPS WRITE setCenteredToGPS NOTIFY centeredToGPSChanged )
    Q_PROPERTY( RecordingType recordingType READ recordingType WRITE setRecordingType NOTIFY recordingTypeChanged )
    Q_PROPERTY( int recordingInterval READ recordingInterval WRITE setRecordingInterval NOTIFY recordingIntervalChanged )
    Q_PROPERTY( StreamingIntervalType::IntervalType recordingIntervalType READ recordingIntervalType WRITE setRecordingIntervalType NOTIFY recordingIntervalTypeChanged )

    Q_PROPERTY( QgsVectorLayer *activeLayer READ activeLayer WRITE setActiveLayer NOTIFY activeLayerChanged )
    Q_PROPERTY( PositionKit *positionKit READ positionKit WRITE setPositionKit NOTIFY positionKitChanged )

    Q_PROPERTY( QgsGeometry recordedGeometry READ recordedGeometry WRITE setRecordedGeometry NOTIFY recordedGeometryChanged )
    Q_PROPERTY( QgsGeometry existingVertices READ existingVertices WRITE setExistingVertices NOTIFY existingVerticesChanged )
    Q_PROPERTY( QgsGeometry midPoints READ midPoints WRITE setMidPoints NOTIFY midPointsChanged )
    Q_PROPERTY( QgsGeometry handles READ handles WRITE setHandles NOTIFY handlesChanged )

    Q_PROPERTY( Vertex activeVertex READ activeVertex WRITE setActiveVertex NOTIFY activeVertexChanged )
    Q_PROPERTY( QgsGeometry activeVertexGeometry READ activeVertexGeometry WRITE setActiveVertexGeometry NOTIFY activeVertexGeometryChanged )
    Q_PROPERTY( InsertPolicy insertPolicy READ insertPolicy WRITE setInsertPolicy NOTIFY insertPolicyChanged )

    Q_PROPERTY( MapToolState state READ state WRITE setState NOTIFY stateChanged )

    Q_PROPERTY( QgsPoint recordPoint READ recordPoint WRITE setRecordPoint NOTIFY recordPointChanged )
    Q_PROPERTY( int activePart READ activePart NOTIFY activePartChanged )
    Q_PROPERTY( int activeRing READ activeRing NOTIFY activeRingChanged )

    Q_PROPERTY( bool canUndo READ canUndo WRITE setCanUndo NOTIFY canUndoChanged )
    Q_PROPERTY( QgsFeature activeFeature READ activeFeature WRITE setActiveFeature NOTIFY activeFeatureChanged )

  public:

    enum RecordingType
    {
      StreamMode = 0,
      Manual
    };
    Q_ENUM( RecordingType );

    enum MapToolState
    {
      Record = 0, // No point selected, show crosshair and guideline
      Grab, // Existing point selected, show crosshair and guideline
      View // No point selected, hide crosshair and guideline [start of editing]
    };
    Q_ENUM( MapToolState );

    enum InsertPolicy
    {
      End = 0, // Default, new vertices are appended to the end of geometry
      Start // Vertices will be added to the beggining of geometry (when clicked on continue line from start)
    };
    Q_ENUM( InsertPolicy );

    explicit RecordingMapTool( QObject *parent = nullptr );
    virtual ~RecordingMapTool() override;

    /**
     * Adds point to the end of the recorded geometry; updates recordedGeometry afterwards
     * Passed point needs to be in active vector layer CRS
     */
    Q_INVOKABLE void addPoint( const QgsPoint &point );

    void addPointAtPosition( Vertex vertex, const QgsPoint &point );

    /**
     *  Removes last point from recorded geometry if there is at least one point
     *  Updates recordedGeometry afterwards
     */
    Q_INVOKABLE void removePoint();

    //! Returns true if the captured geometry has enought points for the specified layer
    Q_INVOKABLE bool hasValidGeometry() const;

    /**
     * Finds vertex id which matches given screen coordinates. Search radius defined in pixels
     */
    Q_INVOKABLE void lookForVertex( const QPointF &clickedPoint, double searchRadius = 3 );

    /**
     * Updates vertex at the active vertex position and updates recordedGeometry
     * Passed point needs to be in active vector layer CRS
     */
    Q_INVOKABLE void releaseVertex( const QgsPoint &point );

    Q_INVOKABLE FeatureLayerPair getFeatureLayerPair();

    Q_INVOKABLE void discardChanges();

    /**
     * Reverts last change from the layer undo stack.
     */
    Q_INVOKABLE void undo();

    /**
     * Returns true if there are changes in the layer edit buffer
     */
    Q_INVOKABLE bool hasChanges() const;

    void updateVertex( const Vertex &vertex, const QgsPoint &point );

    /**
     * Returns coordinates of the active vertex in map CRS
     */
    Q_INVOKABLE QgsPoint vertexMapCoors( const Vertex &vertex ) const;

    Q_INVOKABLE void cancelGrab();

    // Getters / setters
    bool centeredToGPS() const;
    void setCenteredToGPS( bool newCenteredToGPS );

    const RecordingType &recordingType() const;
    void setRecordingType( const RecordingType &newRecordingType );

    int recordingInterval() const;
    void setRecordingInterval( int newRecordingInterval );

    StreamingIntervalType::IntervalType recordingIntervalType() const;
    void setRecordingIntervalType( StreamingIntervalType::IntervalType intervalType );

    PositionKit *positionKit() const;
    void setPositionKit( PositionKit *newPositionKit );

    QgsVectorLayer *activeLayer() const;
    void setActiveLayer( QgsVectorLayer *newActiveLayer );

    const QgsGeometry &recordedGeometry() const;
    void setRecordedGeometry( const QgsGeometry &newRecordedGeometry );

    const QgsGeometry &existingVertices() const;
    void setExistingVertices( const QgsGeometry &newExistingVertices );

    const QgsGeometry &midPoints() const;
    void setMidPoints( const QgsGeometry &newMidPoints );

    const QgsGeometry &handles() const;
    void setHandles( const QgsGeometry &newHandles );

    const QVector< Vertex > &collectedVertices() const;

    MapToolState state() const;
    void setState( const MapToolState &newState );

    QPointF crosshairPosition() const;
    void setCrosshairPosition( QPointF newCrosshairPosition );

    QgsPoint recordPoint() const;
    void setRecordPoint( QgsPoint newRecordPoint );

    const Vertex &activeVertex() const;
    void setActiveVertex( const Vertex &newActiveVertex );

    const QgsGeometry &activeVertexGeometry() const;
    void setActiveVertexGeometry( const QgsGeometry &newActiveVertexGeometry );

    const InsertPolicy &insertPolicy() const;
    void setInsertPolicy( const InsertPolicy &insertPolicy );

    int activePart() const;
    int activeRing() const;
    void setActivePartAndRing( int newActivePart, int newActiveRing );

    bool canUndo() const;
    void setCanUndo( bool newCanUndo );

    const QgsFeature &activeFeature() const;
    void setActiveFeature( const QgsFeature &newActiveFeature );

  signals:
    void activeLayerChanged( QgsVectorLayer *activeLayer );
    void centeredToGPSChanged( bool centeredToGPS );
    void positionKitChanged( PositionKit *positionKit );
    void recordedGeometryChanged( const QgsGeometry &recordedGeometry );
    void recordingIntervalChanged( int lineRecordingInterval );
    void recordingTypeChanged( const RecordingMapTool::RecordingType &recordingType );
    void recordingIntervalTypeChanged();

    void existingVerticesChanged( const QgsGeometry &existingVertices );
    void midPointsChanged( const QgsGeometry &midPoints );
    void handlesChanged( const QgsGeometry &handles );
    void stateChanged( const RecordingMapTool::MapToolState &state );

    void recordPointChanged( QgsPoint recordPoint );

    void activeVertexChanged( const Vertex &activeVertex );
    void activeVertexGeometryChanged( const QgsGeometry &activeVertexGeometry );

    void insertPolicyChanged( const RecordingMapTool::InsertPolicy &insertPolicy );

    void activePartChanged( int activePart );
    void activeRingChanged( int activeRing );

    void canUndoChanged( bool canUndo );
    void activeFeatureChanged( const QgsFeature &activeFeature );

    void finalEmptyGeometry();
    void finalSingleGeometry();

  public slots:
    void onPositionChanged();

  private slots:
    void prepareEditing();
    void onFeatureAdded( QgsFeatureId newFeatureId );

    /**
     * Creates nodes index. Extracts existing geometry vertices and generates virtual
     * vertices representing midpoints (for lines and polygons) and start/end points
     * (for lines).
     */
    void collectVertices();

    /**
     * Creates geometries represeinting existing nodes, midpoints (for lines and polygons),
     * start/end points and "handles" (for lines) from the nodes index.
     */
    void updateVisibleItems();

    /**
     * Updates geometry of the active vertex
     */
    void updateActiveVertexGeometry();

    /**
     * Grabs next vertex after the removal of the currently selected vertex
     */
    void grabNextVertex();

    /**
     * Ends layer editing command and puts it into layer undo stack
     */
    void completeEditOperation();

  protected:
    //! Unifies Z/M coordinate of the point with current layer - drops / adds it
    void fixZM( QgsPoint &point ) const;

  private:
    double pixelsToMapUnits( double numPixels );
    QgsPoint handlePoint( QgsPoint p1, QgsPoint p2 );

    /**
     * Check whether given point should be used for creating markers/handles
     */
    bool shouldBeVisible( QgsPoint point );

    /**
     * Check the layers intersection mode and change the feature geometry accordingly
     */
    void avoidIntersections();

    QgsGeometry mRecordedGeometry;

    bool mCenteredToGPS = false;
    RecordingType mRecordingType = Manual;
    int mRecordingInterval;  // in seconds or meters for the StreamingMode
    StreamingIntervalType::IntervalType mRecordingIntervalType = StreamingIntervalType::IntervalType::Time;

    QDateTime mLastTimeRecorded;
    QgsPoint mLastRecordedPoint;

    QgsVectorLayer *mActiveLayer = nullptr; // not owned
    PositionKit *mPositionKit = nullptr; // not owned

    QgsGeometry mExistingVertices;
    QgsGeometry mMidPoints;
    QgsGeometry mHandles;

    MapToolState mState = MapToolState::Record;
    InsertPolicy mInsertPolicy = InsertPolicy::End;

    QVector< Vertex > mVertices;

    QgsPoint mRecordPoint;

    // ActiveVertex is set only when we grab a point,
    // it is the grabbed point, contains its coordinates and index
    Vertex mActiveVertex;
    QgsGeometry mActiveVertexGeometry;

    // ActiveRing and ActivePart are set only when we record,
    // in order to know where to insert the points
    int mActiveRing = 0;
    int mActivePart = 0;

    bool mCanUndo = false;
    QgsFeature mActiveFeature;

    int mMinUndoStackIndex = 0; // We can not undo more than this index
};

#endif // RECORDINGMAPTOOL_H
