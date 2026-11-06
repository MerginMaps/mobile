/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef USAGEREPORTCONTROLLER_H
#define USAGEREPORTCONTROLLER_H

#include <QObject>
#include <QElapsedTimer>
#include <QNetworkAccessManager>
#include <QTimer>

class AppSettings;
class LocalProjectsManager;
class MerginApi;

class UsageReportController : public QObject
{
    Q_OBJECT

  public:
    explicit UsageReportController( AppSettings *appSettings,
                                    LocalProjectsManager *localProjectsManager,
                                    MerginApi *merginApi,
                                    QObject *parent = nullptr );

    Q_INVOKABLE void trackFeature( const QString &key );
    void incrementCounter( const QString &key, int amount = 1 );
    void setData( const QString &key, const QVariant &value );

    //! Tracks external GNSS usage, does nothing for the internal provider
    void trackExternalProvider( const QString &providerId, const QString &providerType, const QString &providerName );

    //! Stores the role if it is higher than the highest role stored so far
    void trackProjectRole( const QString &role );

    void startLoadTimer();
    void recordLoadTime();

    void trySubmitSnapshot();
    // Temporary: bypass interval check for testing
    void forceSubmitSnapshot();

    void startPingTimer();
    void startSnapshotTimer();

    /**
     * Returns brand of the external GNSS receiver, never the provider name itself (it can be set by the user).
     * Returns "other" for unrecognised bluetooth devices and "unknown" when the brand can not be derived.
     */
    static QString externalProviderBrand( const QString &providerType, const QString &providerName );

    /**
     * Returns connection type of the provider ("mock", "bluetooth", "network", "trimble")
     * or an empty string for the internal provider.
     */
    static QString externalConnectionType( const QString &providerId, const QString &providerType );

    /**
     * Returns rank of the project role, higher means more permissions.
     */
    static int projectRoleRank( const QString &role );

  signals:
    //! Emitted after a snapshot was submitted and the sent data was reset
    void snapshotSubmitted();

  private:
    void submitSnapshot( bool force ); // Temporary: remove force param when removing forceSubmitSnapshot
    void checkConnectivity();
    bool isEnabled() const;

    AppSettings *mAppSettings = nullptr;
    LocalProjectsManager *mLocalProjectsManager = nullptr;
    MerginApi *mMerginApi = nullptr;

    QNetworkAccessManager mNam;
    QTimer *mPingTimer = nullptr;
    QTimer *mSnapshotTimer = nullptr;
    QElapsedTimer mLoadTimer;
    bool mSnapshotInFlight = false;
};

#endif // USAGEREPORTCONTROLLER_H
