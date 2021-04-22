/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/


#ifndef MERGINUSERINFO_H
#define MERGINUSERINFO_H

#include <QObject>
#include <QString>
#include <QJsonObject>

#include "merginsubscriptionstatus.h"
#include "merginsubscriptiontype.h"

class MerginUserInfo: public QObject
{
    Q_OBJECT
    Q_PROPERTY( QString email READ email NOTIFY userInfoChanged )
    Q_PROPERTY( double storageLimit READ storageLimit NOTIFY userInfoChanged )
    Q_PROPERTY( double diskUsage READ diskUsage NOTIFY userInfoChanged ) // in Bytes

  public:
    explicit MerginUserInfo( QObject *parent = nullptr );
    ~MerginUserInfo() = default;

  public:
    void clear();
    void setFromJson( QJsonObject docObj );

    QString email() const;
    double diskUsage() const;
    double storageLimit() const;

  signals:
    void userInfoChanged();
  public slots:
    void onStorageChanged( double storage );

  private:
    QString mEmail;
    double mDiskUsage = 0.0; // in Bytes
    double mStorageLimit = 0.0; // in Bytes
};

#endif // MERGINUSERINFO_H
