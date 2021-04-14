#ifndef COMPASS_H
#define COMPASS_H

#include <QObject>
#include <QTimer>
#include <QOrientationSensor>
#include <QCompass>

class Compass: public QObject
{

    Q_OBJECT
  public:
    explicit Compass( QObject *parent = nullptr );

    qreal direction() const;
    QCompassReading *reading();

  signals:
    void directionChanged();
  public slots:
    void setUserOrientation();
  private:
    QOrientationSensor *mOrientationSensor = nullptr;
    QCompass *mCompass = nullptr;
};

#endif // COMPASS_H
