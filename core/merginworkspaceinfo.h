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

  signals:
    void workspaceInfoChanged();

  private:
    double mDiskUsage = 0.0; // in Bytes
    double mStorageLimit = 0.0; // in Bytes
    QString mRole; // user's role in this workspace
};

#endif // MERGINWORKSPACEINFO_H
