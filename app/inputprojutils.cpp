/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include "inputprojutils.h"

#include <QDebug>
#include <QDir>
#include <QFileInfo>

#include "inpututils.h"
#include "proj.h"
#include "qgsprojutils.h"
#include "inputhelp.h"

InputProjUtils::InputProjUtils( QObject *parent )
  : QObject( parent )
{
  initCoordinateOperationHandlers();
}

void InputProjUtils::initProjLib( const QString &pkgPath )
{
#ifdef MOBILE_OS
#ifdef ANDROID
  // win and ios resources are already in the bundle
  InputUtils::cpDir( "assets:/qgis-data", pkgPath );
  QString prefixPath = pkgPath + "/proj";
  QString projFilePath = prefixPath + "/proj.db";
#endif

#ifdef Q_OS_IOS
  QString prefixPath = pkgPath + "/proj";
  QString projFilePath = prefixPath + "/proj.db";
#endif

#ifdef Q_OS_WIN32
  QString prefixPath = pkgPath + "\\proj";
  QString projFilePath = prefixPath + "\\proj.db";
#endif
  QFile projdb( projFilePath );
  if ( projdb.exists() )
  {
    qputenv( "PROJ_LIB", prefixPath.toUtf8().constData() );
    qDebug() << "PROJ_LIB environment variable" << prefixPath;
  }
  else
  {
    InputUtils::log( QStringLiteral( "PROJ6 error" ), QStringLiteral( "The Input has failed to load PROJ6 database." ) );
  }

#else
  // proj share lib is set from the proj installation on the desktop,
  // so it should work without any modifications.
  // to test check QgsProjUtils.searchPaths() in QGIS Python Console
  Q_UNUSED( pkgPath )
#endif
}

void InputProjUtils::setDefaultProjPaths()
{
  sDefaultProjPaths = QgsProjUtils::searchPaths();
  qDebug() << "PROJ DEFAULT search paths" << sDefaultProjPaths;
}

void InputProjUtils::warnUser( const QString &message )
{
  if ( !mPopUpShown )
  {
    emit projError( message );
  }
}

void InputProjUtils::initCoordinateOperationHandlers()
{
  QgsCoordinateTransform::setCustomMissingRequiredGridHandler( [ = ]( const QgsCoordinateReferenceSystem & sourceCrs,
      const QgsCoordinateReferenceSystem & destinationCrs,
      const QgsDatumTransform::GridDetails & grid )
  {
    Q_UNUSED( destinationCrs )
    Q_UNUSED( sourceCrs )

    if ( !mMissingRequiredGridReported )
      InputUtils::log( "InputProj", QStringLiteral( "missingRequiredGrid %1" ).arg( grid.shortName ) );
    warnUser( tr( "Missing required PROJ datum shift grid: %1" ).arg( grid.shortName ) );
  } );

  QgsCoordinateTransform::setCustomMissingPreferredGridHandler( [ = ]( const QgsCoordinateReferenceSystem & sourceCrs,
      const QgsCoordinateReferenceSystem & destinationCrs,
      const QgsDatumTransform::TransformDetails & preferredOperation,
      const QgsDatumTransform::TransformDetails & availableOperation )
  {
    Q_UNUSED( destinationCrs )
    Q_UNUSED( sourceCrs )
    Q_UNUSED( preferredOperation )
    Q_UNUSED( availableOperation )

    if ( !mMissingPreferredGridReported )
      InputUtils::log( "InputProj", QStringLiteral( "missingPrefferedGrid" ) );
  } );

  QgsCoordinateTransform::setCustomCoordinateOperationCreationErrorHandler( [ = ]( const QgsCoordinateReferenceSystem & sourceCrs,
      const QgsCoordinateReferenceSystem & destinationCrs,
      const QString & error )
  {
    Q_UNUSED( destinationCrs )
    Q_UNUSED( sourceCrs )

    if ( !mCoordinateOperationCreationErrorReported )
      InputUtils::log( "InputProj", QStringLiteral( "coordinateOperationCreationError: %1" ).arg( error ) );
  } );

  QgsCoordinateTransform::setCustomMissingGridUsedByContextHandler( [ = ]( const QgsCoordinateReferenceSystem & sourceCrs,
      const QgsCoordinateReferenceSystem & destinationCrs,
      const QgsDatumTransform::TransformDetails & desired )
  {
    Q_UNUSED( destinationCrs )
    Q_UNUSED( sourceCrs )
    Q_UNUSED( desired )

    if ( !mMissingGridUsedByContextHandlerReported )
      InputUtils::log( "InputProj", QStringLiteral( "customMissingGridUsedByContextHandler %1" ) );
    warnUser( tr( "Missing required PROJ datum shift grid" ) );

  } );

  QgsCoordinateTransform::setFallbackOperationOccurredHandler( [ = ]( const QgsCoordinateReferenceSystem & sourceCrs,
      const QgsCoordinateReferenceSystem & destinationCrs,
      const QString & desired )
  {
    Q_UNUSED( destinationCrs )
    Q_UNUSED( sourceCrs )
    Q_UNUSED( desired )

    if ( !mFallbackOperationOccurredReported )
      InputUtils::log( "InputProj", QStringLiteral( "fallbackOperationOccurredReported" ) );
  } );
}

static void _updateProj( const QStringList &searchPaths )
{
  char **newPaths = new char *[searchPaths.count()];
  for ( int i = 0; i < searchPaths.count(); ++i )
  {
    newPaths[i] = strdup( searchPaths.at( i ).toUtf8().constData() );
  }
  proj_context_set_search_paths( nullptr, searchPaths.count(), newPaths );
  for ( int i = 0; i < searchPaths.count(); ++i )
  {
    free( newPaths[i] );
  }
  delete [] newPaths;
}

void InputProjUtils::modifyProjPath( const QString &projectFile )
{
  mPopUpShown = false;
  mMissingRequiredGridReported = false;
  mMissingPreferredGridReported = false;
  mCoordinateOperationCreationErrorReported = false;
  mMissingGridUsedByContextHandlerReported = false;
  mFallbackOperationOccurredReported = false;

  if ( projectFile.isEmpty() )
  {
    _updateProj( sDefaultProjPaths );
    mCurrentCustomProjDir = QString();
  }
  else
  {
    QFileInfo fi( projectFile );
    QDir projDir( fi.absoluteDir().absolutePath() + "/proj" );
    mCurrentCustomProjDir = projDir.absolutePath();
    if ( projDir.isReadable() && !projDir.isEmpty() && !sDefaultProjPaths.contains( projectFile ) )
    {
      qDebug() << "PROJ6 using custom resource folder: " << projDir.absolutePath();
    }
    else
    {
      qDebug() << "PROJ6 missing custom resource folder: " << projDir.absolutePath();
    }
    QStringList searchPaths( sDefaultProjPaths );
    searchPaths.append( mCurrentCustomProjDir );
    _updateProj( searchPaths );
  }
}
