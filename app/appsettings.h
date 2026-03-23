/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef APPSETTINGS_H
#define APPSETTINGS_H

#include <QVariant>
#include <QString>
#include <QtQml/qqmlregistration.h>

#include "streamingintervaltype.h"

class AppSettings: public QObject
{
    Q_OBJECT
    QML_ELEMENT
    QML_SINGLETON

    Q_PROPERTY( QString defaultProject READ defaultProject WRITE setDefaultProject NOTIFY defaultProjectChanged )
    Q_PROPERTY( QString activeProject READ activeProject WRITE setActiveProject NOTIFY activeProjectChanged )
    Q_PROPERTY( QString defaultProjectName READ defaultProjectName NOTIFY defaultProjectChanged )
    Q_PROPERTY( QString defaultLayer READ defaultLayer WRITE setDefaultLayer NOTIFY defaultLayerChanged )
    Q_PROPERTY( int lineRecordingInterval READ lineRecordingInterval WRITE setLineRecordingInterval NOTIFY lineRecordingIntervalChanged )
    Q_PROPERTY( StreamingIntervalType::IntervalType intervalType READ intervalType WRITE setIntervalType NOTIFY intervalTypeChanged )
    Q_PROPERTY( double gpsAccuracyTolerance READ gpsAccuracyTolerance WRITE setGpsAccuracyTolerance NOTIFY gpsAccuracyToleranceChanged )
    Q_PROPERTY( bool reuseLastEnteredValues READ reuseLastEnteredValues WRITE setReuseLastEnteredValues NOTIFY reuseLastEnteredValuesChanged )
    Q_PROPERTY( QString appVersion READ appVersion WRITE setAppVersion NOTIFY appVersionChanged )
    Q_PROPERTY( QString activePositionProviderId READ activePositionProviderId WRITE setActivePositionProviderId NOTIFY activePositionProviderIdChanged )
    Q_PROPERTY( bool autosyncAllowed READ autosyncAllowed WRITE setAutosyncAllowed NOTIFY autosyncAllowedChanged )
    Q_PROPERTY( double gpsAntennaHeight READ gpsAntennaHeight WRITE setGpsAntennaHeight NOTIFY gpsAntennaHeightChanged )
    Q_PROPERTY( QString ignoreMigrateVersion READ ignoreMigrateVersion WRITE setIgnoreMigrateVersion NOTIFY ignoreMigrateVersionChanged )
    Q_PROPERTY( bool autolockPosition READ autolockPosition WRITE setAutolockPosition NOTIFY autolockPositionChanged )
    Q_PROPERTY( QList<QVariant> windowPosition READ windowPosition WRITE setWindowPosition NOTIFY windowPositionChanged )
    Q_PROPERTY( HapticsType hapticsType READ hapticsType WRITE setHapticsType NOTIFY hapticsTypeChanged )
    Q_PROPERTY( bool alwaysShowFilterButton READ alwaysShowFilterButton WRITE setAlwaysShowFilterButton NOTIFY alwaysShowFilterButtonChanged )

  public:
    // enum of haptic modes we support
    enum HapticsType
    {
#ifdef DESKTOP_OS
      HapticsOff = 0,
      HapticsSound,
#else
      HapticsOff = 0,
      HapticsSound,
      HapticsVibration,
      HapticsVibrationSound
#endif
    };
    Q_ENUM( HapticsType )

    explicit AppSettings( QObject *parent = nullptr );

    QString defaultProject() const;
    void setDefaultProject( const QString &value );

    QString defaultProjectName() const;

    QString activeProject() const;
    void setActiveProject( const QString &value );

    QString defaultLayer() const;
    void setDefaultLayer( const QString &value );

    double gpsAccuracyTolerance() const;
    void setGpsAccuracyTolerance( double gpsAccuracyTolerance );

    int lineRecordingInterval() const;
    void setLineRecordingInterval( int lineRecordingInterval );

    StreamingIntervalType::IntervalType intervalType() const;
    void setIntervalType( StreamingIntervalType::IntervalType intervalType );

    bool reuseLastEnteredValues() const;

    QString appVersion() const;
    void setAppVersion( const QString &newAppVersion );

    // SavedPositionProviders property is read only when needed ~ not at startup time.
    // It returns list of all external position providers (does not include internal/simulated position providers)
    QVariantList savedPositionProviders() const;
    void savePositionProviders( const QVariantList &providers );

    const QString &activePositionProviderId() const;
    void setActivePositionProviderId( const QString &id );

    bool autosyncAllowed() const;
    void setAutosyncAllowed( bool newAutosyncAllowed );

    double gpsAntennaHeight() const;
    void setGpsAntennaHeight( double gpsAntennaHeight );

    QString ignoreMigrateVersion() const;
    void setIgnoreMigrateVersion( const QString &version );

    static const QString POSITION_PROVIDERS_GROUP;

    bool autolockPosition() const;
    void setAutolockPosition( bool autolockPosition );

    QList<QVariant> windowPosition() const;
    void setWindowPosition( const QList<QVariant> &newWindowPosition );

    HapticsType hapticsType() const;
    void setHapticsType( HapticsType hapticsType );

    bool alwaysShowFilterButton() const;
    void setAlwaysShowFilterButton( bool alwaysShowFilterButton );

  public slots:
    void setReuseLastEnteredValues( bool reuseLastEnteredValues );

  signals:
    void defaultProjectChanged();
    void activeProjectChanged();
    void defaultLayerChanged();
    void gpsAccuracyToleranceChanged();
    void gpsAntennaHeightChanged( double antennaHeight );
    void lineRecordingIntervalChanged();
    void intervalTypeChanged();

    void reuseLastEnteredValuesChanged( bool reuseLastEnteredValues );
    void appVersionChanged( const QString &version );
    void activePositionProviderIdChanged( const QString & );

    void autosyncAllowedChanged( bool autosyncAllowed );
    void autolockPositionChanged( bool autolockPosition );
    void hapticsTypeChanged( HapticsType hapticsType );
    void alwaysShowFilterButtonChanged( bool alwaysShowFilterButton );

    void ignoreMigrateVersionChanged();

    void windowPositionChanged();

  private:
    // Projects path
    QString mDefaultProject;
    // Path to active project
    QString mActiveProject;
    // flag for following GPS position
    bool mAutoCenterMapChecked = false;
    // used in GPS signal indicator
    double mGpsAccuracyTolerance = -1;
    // Digitizing period in seconds
    int mLineRecordingInterval = 3;
    StreamingIntervalType::IntervalType mIntervalType = StreamingIntervalType::IntervalType::Time;

    // Application version, helps to differentiate between app installation, update or regular run:
    //  1. if the value is null, this run is first after installation (or after user reset application data in settings)
    //  2. if the value is different from current version, this is first run after update
    //  3. if the value is the same as current version, this is regular run
    // these check is possible to do during startup (in main.cpp)
    QString mAppVersion;

    // Projects path -> defaultLayer name
    QHash<QString, QString> mDefaultLayers;

    // used to allow remembering values of last created feature to speed up digitizing for user
    bool mReuseLastEnteredValues;

    void setValue( const QString &key, const QVariant &value );
    QVariant value( const QString &key, const QVariant &defaultValue = QVariant() ) const;
    QString mActivePositionProviderId;
    bool mAutosyncAllowed = false;
    bool mAutolockPosition = true;
    double mGpsAntennaHeight = 0;
    QString mIgnoreMigrateVersion;

    HapticsType mHapticsType;
    bool mAlwaysShowFilterButton = false;
};

#endif // APPSETTINGS_H
