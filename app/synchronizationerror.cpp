/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include "synchronizationerror.h"

SynchronizationError::SynchronizationError() = default;

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
  else if ( errorCode == 422 )
  {
    // Hit the maximun number of contributor per month
    if ( errorMessage.contains( QStringLiteral( "Maximum number of workspace contributors" ) ) )
    {
      return ErrorType::MonthlyContributorsLimitHit;
    }

    return ErrorType::UnknownError;
  }
  else if ( errorCode >= 500 )
  {
    // Exceptions in server code or maintenance mode
    return ErrorType::ServerError;
  }

  return ErrorType::UnknownError;
}

bool SynchronizationError::isPermanent( ErrorType errorType )
{
  switch ( errorType )
  {
    case AnotherProcessIsRunning:
      [[fallthrough]];
    case VersionMismatch:
      [[fallthrough]];
    case ServerError: return false;

    default: return true;
  }
}
