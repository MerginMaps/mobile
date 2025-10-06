/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef SYNCHRONIZATIONOPTIONS_H
#define SYNCHRONIZATIONOPTIONS_H

#include <QObject>
#include <QNetworkReply>

class SyncOptions
{
    Q_GADGET

  public:
    explicit SyncOptions() = default;

    enum Strategy
    {
      Singleshot = 0, //! try to sync once, finishes on first failure
      Retry //! retries up to 2 times to repeat sync on failure
    };
    Q_ENUMS( Strategy );

    enum Authorization
    {
      Authorized = 0, //! Use authorization for synchronization
      AuthOptional //! Use authorization only when we have it, otherwise continue sync even signed out
    };
    Q_ENUMS( Authorization );
};

#endif // SYNCHRONIZATIONOPTIONS_H
