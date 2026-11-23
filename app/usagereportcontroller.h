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
#include <QPointer>
#include <QTimer>
#include <QVariantMap>
#include <QtQml/qqmlregistration.h>

class AppSettings;
class LocalProjectsManager;
class MerginApi;
class QQmlEngine;
class QJSEngine;

class UsageReportController : public QObject
{
    Q_OBJECT
    QML_ELEMENT
    QML_SINGLETON

  public:
    explicit UsageReportController( AppSettings *appSettings,
                                    LocalProjectsManager *localProjectsManager,
                                    MerginApi *merginApi,
                                    QObject *parent = nullptr );
    ~UsageReportController() override;

    /**
     * Returns the instance created in main() as the QML singleton, it is not owned by the QML engine.
     * The instance has to exist before QML accesses it.
     */
    static UsageReportController *create( QQmlEngine *qmlEngine, QJSEngine *jsEngine );

    Q_INVOKABLE void trackFeature( const QString &key );

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

  private slots:
    void onInternetConnectionChecked( bool isOnline );
    void onUsageReportSent( bool success );

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

    QTimer mPingTimer;
    QTimer mSnapshotTimer;
    QElapsedTimer mLoadTimer;
    bool mSnapshotInFlight = false;
    QVariantMap mSentData; // data of the snapshot in flight, to reset it once the snapshot is sent

    inline static UsageReportController *sInstance = nullptr;
};

#endif // USAGEREPORTCONTROLLER_H
