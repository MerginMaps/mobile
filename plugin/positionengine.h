#ifndef POSITIONENGINE_H
#define POSITIONENGINE_H

#include <QObject>

#include <QtPositioning>

class MapEngine;

class PositionEngine : public QObject
{
  Q_OBJECT
  Q_PROPERTY(QPointF position READ position NOTIFY positionChanged)
  Q_PROPERTY(bool hasPosition READ hasPosition NOTIFY hasPositionChanged)
  Q_PROPERTY(qreal accuracy READ accuracy NOTIFY positionChanged)
  Q_PROPERTY(qreal direction READ direction NOTIFY positionChanged)
public:
  static PositionEngine* instance();

  bool hasPosition() const { return mHasPosition; }

  QPointF position() const { return mPosition; }

  qreal accuracy() const { return mAccuracy; }

  qreal direction() const { return mDirection; }

signals:
  void positionChanged();

  void hasPositionChanged();

public slots:

private slots:
    void positionUpdated(const QGeoPositionInfo &info);
    void onUpdateTimeout();

protected:
  explicit PositionEngine(QObject *parent = 0);

protected:
  QPointF mPosition; // in WGS84 coords
  qreal mAccuracy; // horizontal accuracy in meters (-1 if not available)
  qreal mDirection; // bearing in degrees clockwise from north to direction of travel (-1 if not available)
  bool mHasPosition;

  static PositionEngine* sInstance;
};


/** simulated GPS - makes circles around a fixed position */
class SimulationPositionSource : public QGeoPositionInfoSource
{
    Q_OBJECT
public:
    SimulationPositionSource(QObject *parent = 0);

    QGeoPositionInfo lastKnownPosition(bool /*fromSatellitePositioningMethodsOnly = false*/) const { return lastPosition; }
    PositioningMethods supportedPositioningMethods() const { return AllPositioningMethods; }
    int minimumUpdateInterval() const { return 500; }
    Error error() const { return QGeoPositionInfoSource::NoError; }

public slots:
    virtual void startUpdates();
    virtual void stopUpdates();

    virtual void requestUpdate(int timeout = 5000);

private slots:
    void readNextPosition();

private:
    QTimer *timer;
    QGeoPositionInfo lastPosition;
    double angle;
};

#endif // POSITIONENGINE_H
