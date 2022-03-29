/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include "synchronizationerror.h"

SynchronizationError::SynchronizationError()
{

}

SynchronizationError::ErrorType SynchronizationError::errorType( int errorCode, const QString &errorMessage )
{
  if ( errorCode == 400 )
  {
    // 'Another process is running. Please try later.'
    if ( errorMessage.contains( QStringLiteral( "Another process" ) ) )
    {
      return ErrorType::AnotherProcessIsRunning;
    }

    // 'Version mismatch'
    if ( errorMessage.contains( QStringLiteral( "mismatch" ) ) )
    {
      return ErrorType::VersionMismatch;
    }
  }
  else if ( errorCode == 403 )
  {
    // Cannot sync project
    return ErrorType::NoPermissions;
  }
  else if ( errorCode == 404 )
  {
    // Project no longer exists / is on different server
    return ErrorType::ProjectNotFound;
  }
  else if ( errorCode >= 500 )
  {
    // Exceptions in server code or maintenance mode
    return ErrorType::ServerError;
  }

  return ErrorType::Other;
}

bool SynchronizationError::isWorthOfRetry( ErrorType errorType )
{
  switch ( errorType )
  {
    case AnotherProcessIsRunning: return true;

    case VersionMismatch: return true;

    case ServerError: return true;

    default: return false;
  }
}
