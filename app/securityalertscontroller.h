/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef SECURITYALERTSCONTROLLER_H
#define SECURITYALERTSCONTROLLER_H

#include <QObject>
#include <QPointer>
#include <QVariantList>

class QgsProject;
class PositionKit;

/**
 * Warns field users when their GPS position is inside a security zone (e.g. a no-go area or a
 * seasonal flood area). Zones are polygon layers configured in the Mergin Maps QGIS plugin and
 * stored in the project:
 *
 *  - "Mergin" / "SecurityAlerts/Enabled": bool
 *  - "Mergin" / "SecurityAlerts/Zones": JSON list of zones
 *      [ { "layer": "<layer id>", "message": "<QGIS expression>", "level": "warning" | "danger" } ]
 *
 * The message expression is evaluated for the polygon the user is in, so it can use its attributes.
 * The zone layers do not need to be visible on the map.
 */
class SecurityAlertsController : public QObject
{
    Q_OBJECT

    Q_PROPERTY( QgsProject *qgsProject READ qgsProject WRITE setQgsProject NOTIFY qgsProjectChanged )
    Q_PROPERTY( PositionKit *positionKit READ positionKit WRITE setPositionKit NOTIFY positionKitChanged )

    Q_PROPERTY( bool enabled READ enabled NOTIFY configurationChanged )
    //! Zones the GPS position is in, most severe first: [ { message, level, layer } ]
    Q_PROPERTY( QVariantList activeAlerts READ activeAlerts NOTIFY activeAlertsChanged )

  public:
    explicit SecurityAlertsController( QObject *parent = nullptr );

    //! Re-reads the zones from the project
    Q_INVOKABLE void reload();

    QgsProject *qgsProject() const;
    void setQgsProject( QgsProject *project );
    PositionKit *positionKit() const;
    void setPositionKit( PositionKit *positionKit );

    bool enabled() const;
    QVariantList activeAlerts() const;

    static const QString CONFIG_SCOPE;

  signals:
    void qgsProjectChanged();
    void positionKitChanged();
    void configurationChanged();
    void activeAlertsChanged();

    //! Emitted once when the user enters a zone (not on every position update)
    void zoneEntered( const QString &message, const QString &level );

  private slots:
    void checkPosition();

  private:
    struct Zone
    {
      QString layerId;
      QString message;
      QString level;
    };

    QPointer<QgsProject> mProject;
    QPointer<PositionKit> mPositionKit;
    QVector<Zone> mZones;
    QVariantList mActiveAlerts;
    QStringList mActiveKeys; // layer id + feature id of the zones the user is in
};

#endif // SECURITYALERTSCONTROLLER_H
