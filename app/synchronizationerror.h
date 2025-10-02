/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef SYNCHRONIZATIONERROR_H
#define SYNCHRONIZATIONERROR_H

#include <QObject>
#include <QNetworkReply>

class SynchronizationError
{
    Q_GADGET

  public:
    explicit SynchronizationError();

    enum ErrorType
    {
      NoError = 0,
      AnotherProcessIsRunning,
      NoPermissions,
      NotAMerginProject,
      ProjectNotFound,
      VersionMismatch,
      ServerError,
      UnknownError
    };
    Q_ENUMS( ErrorType );

    //! Parses error from server based on error code and message and returns one
    //! of SyncError types. "Other" is returned for unknown errors.
    static ErrorType errorType( int errorCode, const QString &errorMessage );

    //! Returns true if there is no point to try to sync again for the errorType
    static bool isPermanent( ErrorType errorType );
};

#endif // SYNCHRONIZATIONERROR_H
