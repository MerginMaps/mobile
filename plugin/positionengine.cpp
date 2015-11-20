#include "positionengine.h"


PositionEngine* PositionEngine::sInstance = 0;


PositionEngine::PositionEngine(QObject *parent)
  : QObject(parent)
  , mAccuracy(-1)
  , mDirection(-1)
  , mHasPosition(false)
{
#ifdef ANDROID
  // this should give us "true" position source
  // on Linux it comes from Geoclue library
  QGeoPositionInfoSource *source = QGeoPositionInfoSource::createDefaultSource(this);
#else
  QGeoPositionInfoSource *source = new SimulationPositionSource(this);
#endif
  if (source)
  {
    connect(source, SIGNAL(positionUpdated(QGeoPositionInfo)),
            this, SLOT(positionUpdated(QGeoPositionInfo)));
    connect(source, SIGNAL(updateTimeout()), this, SLOT(onUpdateTimeout()));
    source->startUpdates();
    qDebug() << "Position source: " << source->sourceName();
  }

}


PositionEngine* PositionEngine::instance()
{
  if (!sInstance)
    sInstance = new PositionEngine();
  return sInstance;
}

void PositionEngine::positionUpdated(const QGeoPositionInfo &info)
{
  //qDebug() << "Position updated:" << info;
  mPosition = QPointF(info.coordinate().longitude(), info.coordinate().latitude());
  if (info.hasAttribute(QGeoPositionInfo::HorizontalAccuracy))
    mAccuracy = info.attribute(QGeoPositionInfo::HorizontalAccuracy);
  else
    mAccuracy = -1;
  if (info.hasAttribute(QGeoPositionInfo::Direction))
    mDirection = info.attribute(QGeoPositionInfo::Direction);
  else
    mDirection = -1;
  //qDebug("acc %f dir %f", mAccuracy, mDirection);
  emit positionChanged();

  if (!mHasPosition)
  {
    mHasPosition = true;
    emit hasPositionChanged();
  }
}


void PositionEngine::onUpdateTimeout()
{
  if (mHasPosition)
  {
    mHasPosition = false;
    emit hasPositionChanged();
  }
}




//////////////////////////

double gSimulationPositionFlightRadius = 0.5; // may be changed to zero in unit tests

SimulationPositionSource::SimulationPositionSource(QObject *parent)
  : QGeoPositionInfoSource(parent)
  , timer(new QTimer(this))
  , angle(0)
{
  connect(timer, SIGNAL(timeout()), this, SLOT(readNextPosition()));
}

void SimulationPositionSource::startUpdates()
{
  int interval = updateInterval();
  if (interval < minimumUpdateInterval())
    interval = minimumUpdateInterval();

  timer->start(interval);
}

void SimulationPositionSource::stopUpdates()
{
  timer->stop();
}

void SimulationPositionSource::requestUpdate(int /*timeout*/)
{
  readNextPosition();
  //emit updateTimeout(); // if the position would not be available
}

void SimulationPositionSource::readNextPosition()
{
  double latitude = 50, longitude = 20;
  double flightRadius = gSimulationPositionFlightRadius;
  latitude += sin(angle*M_PI/180) * flightRadius;
  longitude += cos(angle*M_PI/180) * flightRadius;
  angle += 5;

  QDateTime timestamp = QDateTime::currentDateTime();
  QGeoCoordinate coordinate(latitude, longitude);
  QGeoPositionInfo info(coordinate, timestamp);
  if (info.isValid()) {
      lastPosition = info;
      info.setAttribute(QGeoPositionInfo::Direction, 360 - int(angle)%360);
      info.setAttribute(QGeoPositionInfo::HorizontalAccuracy, 10000); // 10 km
      emit positionUpdated(info);
  }
}
