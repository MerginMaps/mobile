/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/


#ifndef MERGINWORKSPACEINFO_H
#define MERGINWORKSPACEINFO_H

#include <QObject>
#include <QString>
#include <QJsonObject>

class MerginWorkspaceInfo: public QObject
{
    Q_OBJECT
    Q_PROPERTY( double storageLimit READ storageLimit NOTIFY workspaceInfoChanged )
    Q_PROPERTY( double diskUsage READ diskUsage NOTIFY workspaceInfoChanged ) // in Bytes
    Q_PROPERTY( QString role READ role NOTIFY workspaceInfoChanged )

  public:
    explicit MerginWorkspaceInfo( QObject *parent = nullptr );
    ~MerginWorkspaceInfo() = default;

    void clear();
    void setFromJson( QJsonObject docObj );

    double diskUsage() const;
    double storageLimit() const;

    QString role() const;

    //! Sets plan and industry from the reply of the workspace service endpoint, an empty object clears them
    void setServiceDetailsFromJson( const QJsonObject &docObj );

    //! Plan and industry of the workspace, empty when not available
    QString planName() const;
    QString planAlias() const;
    QString industry() const;

  signals:
    void workspaceInfoChanged();

  private:
    double mDiskUsage = 0.0; // in Bytes
    double mStorageLimit = 0.0; // in Bytes
    QString mRole; // user's role in this workspace
    QString mPlanName;
    QString mPlanAlias;
    QString mIndustry;
};

#endif // MERGINWORKSPACEINFO_H
