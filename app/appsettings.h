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

#include <QObject>
#include <QHash>
#include <QVariant>

class AppSettings: public QObject
{
    Q_OBJECT
    Q_PROPERTY( QString defaultProject READ defaultProject WRITE setDefaultProject NOTIFY defaultProjectChanged )
    Q_PROPERTY( QString activeProject READ activeProject WRITE setActiveProject NOTIFY activeProjectChanged )
    Q_PROPERTY( QString defaultProjectName READ defaultProjectName NOTIFY defaultProjectChanged )
    Q_PROPERTY( QString defaultLayer READ defaultLayer WRITE setDefaultLayer NOTIFY defaultLayerChanged )
    Q_PROPERTY( bool autoCenterMapChecked READ autoCenterMapChecked WRITE setAutoCenterMapChecked NOTIFY autoCenterMapCheckedChanged )
    Q_PROPERTY( int lineRecordingInterval READ lineRecordingInterval WRITE setLineRecordingInterval NOTIFY lineRecordingIntervalChanged )
    Q_PROPERTY( double gpsAccuracyTolerance READ gpsAccuracyTolerance WRITE setGpsAccuracyTolerance NOTIFY gpsAccuracyToleranceChanged )
    Q_PROPERTY( bool gpsAccuracyWarning READ gpsAccuracyWarning WRITE setGpsAccuracyWarning NOTIFY gpsAccuracyWarningChanged )
    Q_PROPERTY( bool reuseLastEnteredValues READ reuseLastEnteredValues WRITE setReuseLastEnteredValues NOTIFY reuseLastEnteredValuesChanged )
    Q_PROPERTY( QString appVersion READ appVersion WRITE setAppVersion NOTIFY appVersionChanged )
    Q_PROPERTY( bool legacyFolderMigrated READ legacyFolderMigrated WRITE setLegacyFolderMigrated NOTIFY legacyFolderMigratedChanged )
    Q_PROPERTY( QString activePositionProviderId READ activePositionProviderId WRITE setActivePositionProviderId NOTIFY activePositionProviderIdChanged )

  public:
    explicit AppSettings( QObject *parent = nullptr );

    QString defaultProject() const;
    void setDefaultProject( const QString &value );

    QString defaultProjectName() const;

    QString activeProject() const;
    void setActiveProject( const QString &value );

    QString defaultLayer() const;
    void setDefaultLayer( const QString &value );

    bool autoCenterMapChecked();
    void setAutoCenterMapChecked( const bool value );

    double gpsAccuracyTolerance() const;
    void setGpsAccuracyTolerance( double gpsAccuracyTolerance );

    int lineRecordingInterval() const;
    void setLineRecordingInterval( int lineRecordingInterval );

    bool demoProjectsCopied();
    void setDemoProjectsCopied( const bool value );

    bool reuseLastEnteredValues() const;

    bool gpsAccuracyWarning() const;
    void setGpsAccuracyWarning( bool gpsAccuracyWarning );

    QString appVersion() const;
    void setAppVersion( const QString &newAppVersion );

    bool legacyFolderMigrated();
    void setLegacyFolderMigrated( bool hasBeenMigrated );

    // SavedPositionProviders property is read only when needed ~ not at startup time.
    // It returns list of all external position providers (does not include internal/simulated position providers)
    QVariantList savedPositionProviders() const;
    void savePositionProviders( const QVariantList &providers );

    const QString &activePositionProviderId() const;
    void setActivePositionProviderId( const QString &id );

    static const QString GROUP_NAME;
    static const QString POSITION_PROVIDERS_GROUP;

  public slots:
    void setReuseLastEnteredValues( bool reuseLastEnteredValues );

  signals:
    void defaultProjectChanged();
    void activeProjectChanged();
    void defaultLayerChanged();
    void autoCenterMapCheckedChanged();
    void gpsAccuracyToleranceChanged();
    void gpsAccuracyWarningChanged();
    void lineRecordingIntervalChanged();

    void reuseLastEnteredValuesChanged( bool reuseLastEnteredValues );
    void legacyFolderMigratedChanged( bool legacyFolderMigrated );
    void appVersionChanged( const QString &version );
    void activePositionProviderIdChanged( const QString & );

  private:
    // Projects path
    QString mDefaultProject;
    // Path to active project
    QString mActiveProject;
    // flag for following GPS position
    bool mAutoCenterMapChecked = false;
    // used in GPS signal indicator
    double mGpsAccuracyTolerance = -1;
    // flag for showing accuracy warning if is above tolerance
    bool mGpsAccuracyWarning = true;
    // Digitizing period in seconds
    int mLineRecordingInterval = 3;
    // Application version, helps to differentiate between app installation, update or regular run:
    //  1. if the value is null, this run is first after installation (or after user reset application data in settings)
    //  2. if the value is different from current version, this is first run after update
    //  3. if the value is the same as current version, this is regular run
    // these check is possible to do during startup (in main.cpp)
    QString mAppVersion;
    // signalizes if application has already successfully migrated legacy Android folder
    // this flag can be removed in future (prob. jan 2022), all users should have app version higher than 1.0.2 at that time
    bool mLegacyFolderMigrated;

    // Projects path -> defaultLayer name
    QHash<QString, QString> mDefaultLayers;

    // used to allow remembering values of last created feature to speed up digitizing for user
    bool mReuseLastEnteredValues;

    void setValue( const QString &key, const QVariant &value );
    QVariant value( const QString &key, const QVariant &defaultValue = QVariant() );
    QString mActivePositionProviderId;
};

#endif // APPSETTINGS_H
