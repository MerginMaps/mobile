#ifndef SCALEBARKIT_H
#define SCALEBARKIT_H

#include <QObject>
#include <QString>

class  ScaleBarKit : public QObject
{
  Q_OBJECT

  Q_PROPERTY( QString units READ units NOTIFY scaleBarChanged )
  Q_PROPERTY( int distance READ distance NOTIFY scaleBarChanged )
  Q_PROPERTY( int width READ width NOTIFY scaleBarChanged )

  public:
  explicit ScaleBarKit( QObject *parent = nullptr ) {};

  int width() const {return mWidth;}
  int distance() const {return mDistance;}
  QString units() const {return mUnits;}

  signals:
  void scaleBarChanged();

  private:
  int mPreferredWidth = 300; // pixels
  int mWidth = 80; // pixels
  int mDistance = 100; // in meters or kilometers, rounded
  QString mUnits = "m"; // e.g. km or m

};

#endif // SCALEBARKIT_H
