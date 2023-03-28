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
#include <QString>

#include "inputconfig.h"
#include "streamingintervaltype.h"

class AppSettings: public QObject
{
    Q_OBJECT
    Q_PROPERTY( QString defaultProject READ defaultProject WRITE setDefaultProject NOTIFY defaultProjectChanged )
    Q_PROPERTY( QString activeProject READ activeProject WRITE setActiveProject NOTIFY activeProjectChanged )
    Q_PROPERTY( QString defaultProjectName READ defaultProjectName NOTIFY defaultProjectChanged )
    Q_PROPERTY( QString defaultLayer READ defaultLayer WRITE setDefaultLayer NOTIFY defaultLayerChanged )
    Q_PROPERTY( bool autoCenterMapChecked READ autoCenterMapChecked WRITE setAutoCenterMapChecked NOTIFY autoCenterMapCheckedChanged )
    Q_PROPERTY( int lineRecordingInterval READ lineRecordingInterval WRITE setLineRecordingInterval NOTIFY lineRecordingIntervalChanged )
    Q_PROPERTY( StreamingIntervalType::IntervalType intervalType READ intervalType WRITE setIntervalType NOTIFY intervalTypeChanged )
    Q_PROPERTY( double gpsAccuracyTolerance READ gpsAccuracyTolerance WRITE setGpsAccuracyTolerance NOTIFY gpsAccuracyToleranceChanged )
    Q_PROPERTY( bool gpsAccuracyWarning READ gpsAccuracyWarning WRITE setGpsAccuracyWarning NOTIFY gpsAccuracyWarningChanged )
    Q_PROPERTY( bool reuseLastEnteredValues READ reuseLastEnteredValues WRITE setReuseLastEnteredValues NOTIFY reuseLastEnteredValuesChanged )
    Q_PROPERTY( QString appVersion READ appVersion WRITE setAppVersion NOTIFY appVersionChanged )
    Q_PROPERTY( QString activePositionProviderId READ activePositionProviderId WRITE setActivePositionProviderId NOTIFY activePositionProviderIdChanged )
    Q_PROPERTY( bool autosyncAllowed READ autosyncAllowed WRITE setAutosyncAllowed NOTIFY autosyncAllowedChanged )
    Q_PROPERTY( bool ignoreWhatsNew READ ignoreWhatsNew WRITE setIgnoreWhatsNew NOTIFY ignoreWhatsNewChanged )
    Q_PROPERTY( bool ignoreWsTooltip READ ignoreWsTooltip NOTIFY ignoreWsTooltipChanged )
    Q_PROPERTY( double gpsAntennaHeight READ gpsAntennaHeight WRITE setGpsAntennaHeight NOTIFY gpsAntennaHeightChanged )

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

    StreamingIntervalType::IntervalType intervalType() const;
    void setIntervalType( StreamingIntervalType::IntervalType intervalType );

    bool demoProjectsCopied();
    void setDemoProjectsCopied( const bool value );

    bool reuseLastEnteredValues() const;

    bool gpsAccuracyWarning() const;
    void setGpsAccuracyWarning( bool gpsAccuracyWarning );

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

    bool ignoreWhatsNew() const;
    void setIgnoreWhatsNew( bool newIgnoreWhatsNew );

    double gpsAntennaHeight() const;
    void setGpsAntennaHeight( double gpsAntennaHeight );

    Q_INVOKABLE void wsTooltipShown();
    bool ignoreWsTooltip() const; // ---> can be removed after migration to ws

    static const QString INPUTAPP_GROUP_NAME;
    static const QString POSITION_PROVIDERS_GROUP;
    static const int WS_TOOLTIP_MAX_NUM_OF_OCCURENCIES; // ---> can be removed after migration to ws

  public slots:
    void setReuseLastEnteredValues( bool reuseLastEnteredValues );

  signals:
    void defaultProjectChanged();
    void activeProjectChanged();
    void defaultLayerChanged();
    void autoCenterMapCheckedChanged();
    void gpsAccuracyToleranceChanged();
    void gpsAccuracyWarningChanged();
    void gpsAntennaHeightChanged( double antennaHeight );
    void lineRecordingIntervalChanged();
    void intervalTypeChanged();

    void reuseLastEnteredValuesChanged( bool reuseLastEnteredValues );
    void appVersionChanged( const QString &version );
    void activePositionProviderIdChanged( const QString & );

    void autosyncAllowedChanged( bool autosyncAllowed );
    void ignoreWhatsNewChanged();

    void ignoreWsTooltipChanged(); // ---> can be removed after migration to ws

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
    QVariant value( const QString &key, const QVariant &defaultValue = QVariant() );
    QString mActivePositionProviderId;
    bool mAutosyncAllowed = false;
    bool mIgnoreWhatsNew = false;
    int mWsTooltipShownCounter = 0; // ---> can be removed after migration to ws
    double mGpsAntennaHeight = 0;
};

#endif // APPSETTINGS_H
