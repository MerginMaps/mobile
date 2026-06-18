/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef ANALYTICSCONTROLLER_H
#define ANALYTICSCONTROLLER_H

#include <QNetworkAccessManager>
#include <QObject>
#include <QVariantMap>

/**
 * Sends anonymous feature-usage events to PostHog EU Cloud.
 *
 * PII rules (never include):
 *   - username, email, workspace ID, project name/ID, file paths
 *
 * Always included automatically on every event:
 *   - platform, app_version, "$ip": "" (suppresses IP on PostHog side)
 *
 * distinct_id is a device-scoped UUID from CoreUtils::deviceUuid() —
 * generated once, persisted in QSettings, not linked to any account.
 *
 * When enabled is false, capture() is a no-op — no network call is made.
 */
class AnalyticsController : public QObject
{
    Q_OBJECT
    Q_PROPERTY( bool enabled READ enabled WRITE setEnabled NOTIFY enabledChanged )

  public:
    explicit AnalyticsController( QObject *parent = nullptr );

    bool enabled() const;
    void setEnabled( bool enabled );

    /**
     * Fire an analytics event with optional extra properties.
     * Callable from C++ and QML: __analytics.capture("event_name", {"key": "value"})
     */
    Q_INVOKABLE void capture( const QString &event, const QVariantMap &properties = {} );

  signals:
    void enabledChanged( bool enabled );

  private:
    void sendEvent( const QString &event, QVariantMap properties );

    QNetworkAccessManager mManager;
    QString mDistinctId;
    bool mEnabled = true;

    static const QString ENDPOINT;
    static const QString API_KEY;
    static const QString SETTINGS_KEY;
};

#endif // ANALYTICSCONTROLLER_H
