#ifndef INPUTUTILS_H
#define INPUTUTILS_H

#include <QObject>
#include <QDebug>

#include "streamingintervaltype.h"

class InputUtils: public QObject
{
    Q_OBJECT

  public:
    explicit InputUtils( QObject *parent = nullptr ) {}

    Q_INVOKABLE bool acquireCameraPermission() { return true; }
    Q_INVOKABLE static QString fieldType( const QObject &field ) { return "QDate"; }
    Q_INVOKABLE static QString dateTimeFieldFormat( const QString &fieldFormat ) { return "QDateTime"; }
    Q_INVOKABLE bool fileExists( const QString &path ) { return false; }
    Q_INVOKABLE static QString resolveTargetDir( const QString &homePath, const QVariantMap &config, const QString &pair, QString activeProject ) { return ""; }
    Q_INVOKABLE static QString resolvePrefixForRelativePath( int relativeStorageMode, const QString &homePath, const QString &targetDir ) { return ""; }
    Q_INVOKABLE static QString imageGalleryLocation() { return ""; }
    Q_INVOKABLE static QString getAbsolutePath( const QString &path, const QString &prefixPath ) { return ""; }

    Q_PROPERTY( bool isIos READ isIos CONSTANT )
    Q_PROPERTY( bool isAndroid READ isAndroid CONSTANT )
    Q_PROPERTY( double gpsAccuracyTolerance READ gpsAccuracyTolerance WRITE setGpsAccuracyTolerance NOTIFY gpsAccuracyToleranceChanged )
    Q_PROPERTY( double gpsAntennaHeight READ gpsAntennaHeight WRITE setGpsAntennaHeight NOTIFY gpsAntennaHeightChanged )
    Q_PROPERTY( int lineRecordingInterval READ lineRecordingInterval WRITE setLineRecordingInterval NOTIFY lineRecordingIntervalChanged )
    Q_PROPERTY( StreamingIntervalType::IntervalType intervalType READ intervalType WRITE setIntervalType NOTIFY intervalTypeChanged )

    bool isIos() { return false; }
    bool isAndroid() { return true; }
    double gpsAccuracyTolerance() { return mGpsAccuracy; }
    void setGpsAccuracyTolerance( double gpsAccuracyTolerance ) { mGpsAccuracy = gpsAccuracyTolerance; emit gpsAccuracyToleranceChanged(); }
    double gpsAntennaHeight() { return mGpsAntennaHeightcy; }
    void setGpsAntennaHeight( double gpsAntennaHeight ) { mGpsAntennaHeightcy = gpsAntennaHeight; emit gpsAntennaHeightChanged(); }
    int lineRecordingInterval() { return mLineRecordingInterval; }
    void setLineRecordingInterval( int lineRecordingInterval ) { mLineRecordingInterval = lineRecordingInterval; emit lineRecordingIntervalChanged(); }
    StreamingIntervalType::IntervalType intervalType() { return mIntervalType; }
    void setIntervalType( StreamingIntervalType::IntervalType intervalType ) { mIntervalType = intervalType; emit intervalTypeChanged(); }

  signals:
    void imageSelected( QString imagePath, QString code );
    void gpsAccuracyToleranceChanged();
    void gpsAntennaHeightChanged();
    void lineRecordingIntervalChanged();
    void intervalTypeChanged();

  private:
    double mGpsAccuracy = 10;
    double mGpsAntennaHeightcy = 23;
    int mLineRecordingInterval = 3;
    StreamingIntervalType::IntervalType mIntervalType = StreamingIntervalType::IntervalType::Distance;
};

#endif // INPUTUTILS_H
