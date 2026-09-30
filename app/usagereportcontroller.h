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

    void trackFeature( const QString &key );
    void incrementCounter( const QString &key, int amount = 1 );
    void setData( const QString &key, const QVariant &value );

    void startLoadTimer();
    void recordLoadTime();

    void trySubmitSnapshot();
    // Temporary: bypass interval check for testing
    void forceSubmitSnapshot();

    void startPingTimer();

  private:
    void submitSnapshot( bool force ); // Temporary: remove force param when removing forceSubmitSnapshot
    bool isEnabled() const;

    AppSettings *mAppSettings = nullptr;
    LocalProjectsManager *mLocalProjectsManager = nullptr;
    MerginApi *mMerginApi = nullptr;

    QNetworkAccessManager mNam;
    QTimer *mPingTimer = nullptr;
    QElapsedTimer mLoadTimer;
};

#endif // USAGEREPORTCONTROLLER_H
