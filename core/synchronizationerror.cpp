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

SynchronizationError::ErrorType SynchronizationError::errorType( const int httpErrorCode, const QString &errorMessage, const QString &serverErrorCode )
{
  //prioritise serverErrorCode parsing before httpErrorCode deduction
  if ( serverErrorCode == QStringLiteral( "ProjectsLimitHit" ) )
  {
    return ErrorType::ProjectLimitHit;
  }
  if ( serverErrorCode == QStringLiteral( "StorageLimitHit" ) )
  {
    return ErrorType::StorageLimitHit;
  }
  if ( serverErrorCode == QStringLiteral( "ProjectVersionExists" ) )
  {
    return ErrorType::VersionMismatch;
  }
  if ( serverErrorCode == QStringLiteral( "AnotherUploadRunning" ) )
  {
    return ErrorType::AnotherProcessIsRunning;
  }
  if ( serverErrorCode == QStringLiteral( "ProjectLocked" ) )
  {
    return ErrorType::ProjectLocked;
  }
  if ( serverErrorCode == QStringLiteral( "UploadError" ) )
  {
    // there is a special case during push v2 where UploadError returns with 409 instead of 422,
    // which indicates that the version already exists
    if ( httpErrorCode == 409 )
    {
      return ErrorType::VersionMismatch;
    }
    return ErrorType::UploadError;
  }
  if ( serverErrorCode == QStringLiteral( "DataSyncError" ) )
  {
    return ErrorType::DataSyncError;
  }

  if ( httpErrorCode == 400 )
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
  else if ( httpErrorCode == 403 )
  {
    // Cannot sync project
    return ErrorType::NoPermissions;
  }
  else if ( httpErrorCode == 404 )
  {
    // Project no longer exists / is on different server
    return ErrorType::ProjectNotFound;
  }
  else if ( httpErrorCode == 429 )
  {
    return ErrorType::RateLimiting;
  }
  else if ( httpErrorCode >= 500 )
  {
    // Exceptions in server code or maintenance mode
    return ErrorType::ServerError;
  }

  return ErrorType::UnknownError;
}

bool SynchronizationError::isPermanent( const ErrorType errorType )
{
  switch ( errorType )
  {
    case ErrorType::AnotherProcessIsRunning:
      [[fallthrough]];
    case ErrorType::VersionMismatch:
      [[fallthrough]];
    case ErrorType::RateLimiting:
      return false;

    default: return true;
  }
}
