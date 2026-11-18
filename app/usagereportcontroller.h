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
#include <QPointer>
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

  public slots:
    void incrementCounter( const QString &key, int amount = 1 );

    //! Tracks external GNSS usage, does nothing for the internal provider
    void trackExternalProvider( const QString &providerId, const QString &providerType, const QString &providerName );

    //! Stores the role if it is higher than the highest role stored so far
    void trackProjectRole( const QString &role );

    void startLoadTimer();
    void recordLoadTime();

    // Temporary: bypass interval check for testing
    void forceSubmitSnapshot();

    void startPingTimer();
    void startSnapshotTimer();

  signals:
    //! Emitted after a snapshot was submitted and the sent data was reset
    void snapshotSubmitted();

  private:
    void setData( const QString &key, const QVariant &value );
    void trySubmitSnapshot();
    void submitSnapshot( bool force ); // Temporary: remove force param when removing forceSubmitSnapshot
    void checkConnectivity();
    bool isEnabled() const;

    //! Returns a map with every key of the snapshot set to its default value, so all keys are always present
    QVariantMap emptyUsageDataMap() const;

    QPointer<AppSettings> mAppSettings;
    QPointer<LocalProjectsManager> mLocalProjectsManager;
    QPointer<MerginApi> mMerginApi;

    QNetworkAccessManager mNam;
    QTimer mPingTimer;
    QTimer mSnapshotTimer;
    QElapsedTimer mLoadTimer;
    bool mSnapshotInFlight = false;
};

#endif // USAGEREPORTCONTROLLER_H
