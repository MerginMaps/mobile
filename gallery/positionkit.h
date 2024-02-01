#ifndef POSITIONKIT_H
#define POSITIONKIT_H

#include <QObject>
#include <QString>

class  PositionKit : public QObject
{
    Q_OBJECT

    Q_PROPERTY( double latitude READ latitude CONSTANT)
    Q_PROPERTY( double longitude READ longitude CONSTANT)

public:
    explicit PositionKit( QObject *parent = nullptr ) {};

    double latitude() const {return pKitlatitude;}
    double longitude() const {return pKitlongitude;}
    bool hasPosition() const {return true;}
    double horizontalAccuracy() const {return pKitHorizontalAccuracy;}
    double verticalAccuracy() const {return pKitVerticalAccuracy;}
    int satellitesUsed() const {return pKitSatellitesUsed;}
    int satellitesVisible() const {return pKitSatellitesVisible;}
    double speed() const {return pKitSpeed;}
    double gpsAntennaHeight() {return pKitAntennaHeight;}
    double altitude() {return pKitAltitude;}
    QString units() const {return mUnits;}

private:
    int pKitSatellitesUsed = 1;
    int pKitSatellitesVisible = 40;
    double pKitlatitude = 2391;
    double pKitlongitude = 2;
    double pKitHorizontalAccuracy = 20.00;
    double pKitVerticalAccuracy = 20.00;
    double pKitSpeed = 9.91;
    double pKitAntennaHeight = 10;
    double pKitAltitude = 199.20;
    QString pKitLastFix =  "17:19:08 CEST";
    QString mUnits = "m"; // e.g. km or m

};

#endif // POSITIONKIT_H


