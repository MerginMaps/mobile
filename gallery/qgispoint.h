#ifndef QGISPOINT_H
#define QGISPOINT_H

class QgisPoint {
public:
    QgisPoint() :  qLongitude(12.23), qLatitude(22.32), qElevation(0) {}

    double latitude() const { return qLatitude; }
    double longitude() const { return qLongitude; }

private:
    double qLatitude;
    double qLongitude;
    double qElevation;
};

#endif // QGISPOINT_H
