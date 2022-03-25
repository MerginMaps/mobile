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

SynchronizationError::ErrorType SynchronizationError::errorType( int errorCode, const QString &errorMessage, const QString &topic, QNetworkReply::NetworkError networkError )
{
  if ( errorCode == 400 )
  {
    //'You have reached a data limit'
    if ( errorMessage.contains( QStringLiteral( "data limit" ) ) )
    {
      return ErrorType::DataLimitReached;
    }

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

    // "Please don't start project name with . and use only alphanumeric or these -._~()'!*:@,; characters in project name."
    if ( errorMessage.contains( QStringLiteral( "start project name" ) ) )
    {
      return ErrorType::WrongProjectName;
    }
  }
  else if ( errorCode == 403 )
  {
    // "Failed to push changes. Ask the project owner to log in to their Mergin dashboard for more information.")
    if ( errorMessage.contains( QStringLiteral( "Ask the project owner to log in" ) ) )
    {
      return ErrorType::ServiceAbusement;
    }

    // "You do not have permissions for this project"
    if ( errorMessage.contains( QStringLiteral( "not have permissions" ) ) )
    {
      return ErrorType::NoPermissions;
    }
  }
  else if ( errorCode == 404 )
  {
    // "The requested URL was not found on the server. If you entered the URL manually please check your spelling and try again."
    if ( errorMessage.contains( QStringLiteral( "The requested URL was not found" ) ) )
    {
      return ErrorType::ProjectNotFound;
    }
  }
  else if ( errorCode == 409 )
  {
    // "Project {} already exists!"
    if ( errorMessage.contains( QStringLiteral( "already exists" ) ) )
    {
      return ErrorType::ProjectAlreadyExists;
    }
  }
  else if ( errorCode == 503 )
  {
    // "Service unavailable due to maintenance, please try later"
    if ( errorMessage.contains( QStringLiteral( "maintenance" ) ) )
    {
      return ErrorType::ServerMaintenance;
    }
  }
  else if ( errorCode >= 500 )
  {
    // Exceptions in server code
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
