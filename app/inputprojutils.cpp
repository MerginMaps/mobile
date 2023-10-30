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

#include "proj.h"
#include "inpututils.h"
#include "coreutils.h"
#include "qgsprojutils.h"
#include "inputhelp.h"

InputProjUtils::InputProjUtils( QObject *parent )
  : QObject( parent )
{
  initCoordinateOperationHandlers();
}

void InputProjUtils::warnUser( const QString &message )
{
  if ( !mPopUpShown )
  {
    mPopUpShown = true;
    emit projError( message );
  }
}

void InputProjUtils::logUser( const QString &message, bool &variable )
{
  if ( !variable )
  {
    CoreUtils::log( "InputPROJ", message );
    variable = true;
  }
}

void InputProjUtils::cleanCustomDir()
{
  QDir dir( mCurrentCustomProjDir );
  if ( !dir.isEmpty() )
  {
    qDebug() << "InputPROJ: cleaning custom proj dir " << mCurrentCustomProjDir;
    dir.removeRecursively();
  }
}

static QStringList detailsToStr( const QgsDatumTransform::TransformDetails &details )
{
  QStringList messages;
  for ( const QgsDatumTransform::GridDetails &grid : details.grids )
  {
    if ( !grid.isAvailable )
    {
      messages.append( grid.shortName );
    }
  }
  return messages;
}

void InputProjUtils::initCoordinateOperationHandlers()
{
  QgsCoordinateTransform::setCustomMissingRequiredGridHandler( [ = ]( const QgsCoordinateReferenceSystem & sourceCrs,
      const QgsCoordinateReferenceSystem & destinationCrs,
      const QgsDatumTransform::GridDetails & grid )
  {
    Q_UNUSED( destinationCrs )
    Q_UNUSED( sourceCrs )
    logUser( QStringLiteral( "missing required grid: %1" ).arg( grid.shortName ), mMissingRequiredGridReported );
    warnUser( tr( "Missing required PROJ datum shift grid: %1. For newly downloaded project please restart Input." ).arg( grid.shortName ) );
  } );

  QgsCoordinateTransform::setCustomMissingPreferredGridHandler( [ = ]( const QgsCoordinateReferenceSystem & sourceCrs,
      const QgsCoordinateReferenceSystem & destinationCrs,
      const QgsDatumTransform::TransformDetails & preferredOperation,
      const QgsDatumTransform::TransformDetails & availableOperation )
  {
    Q_UNUSED( destinationCrs )
    Q_UNUSED( sourceCrs )
    Q_UNUSED( availableOperation )
    logUser( QStringLiteral( "missing preffered grid: %1" ).arg( detailsToStr( preferredOperation ).join( ";" ) ), mMissingPreferredGridReported );
  } );

  QgsCoordinateTransform::setCustomCoordinateOperationCreationErrorHandler( [ = ]( const QgsCoordinateReferenceSystem & sourceCrs,
      const QgsCoordinateReferenceSystem & destinationCrs,
      const QString & error )
  {
    Q_UNUSED( destinationCrs )
    Q_UNUSED( sourceCrs )
    logUser( QStringLiteral( "coordinate operation creation error: %1" ).arg( error ), mCoordinateOperationCreationErrorReported );
    warnUser( tr( "Error creating custom PROJ operation. For newly downloaded project please restart Input." ) );
  } );

  QgsCoordinateTransform::setCustomMissingGridUsedByContextHandler( [ = ]( const QgsCoordinateReferenceSystem & sourceCrs,
      const QgsCoordinateReferenceSystem & destinationCrs,
      const QgsDatumTransform::TransformDetails & desired )
  {
    Q_UNUSED( destinationCrs )
    Q_UNUSED( sourceCrs )
    logUser( QStringLiteral( "custom missing grid used by context handler %1" ).arg( detailsToStr( desired ).join( ";" ) ), mMissingGridUsedByContextHandlerReported );
    warnUser( tr( "Missing required PROJ datum shift grids: %1. For newly downloaded project please restart Input." ).arg( detailsToStr( desired ).join( "<br>" ) ) );
  } );

  QgsCoordinateTransform::setFallbackOperationOccurredHandler( [ = ]( const QgsCoordinateReferenceSystem & sourceCrs,
      const QgsCoordinateReferenceSystem & destinationCrs,
      const QString & desired )
  {
    Q_UNUSED( destinationCrs )
    Q_UNUSED( sourceCrs )
    logUser( QStringLiteral( "fallbackOperationOccurredReported: %1" ).arg( desired ), mFallbackOperationOccurredReported );
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

void InputProjUtils::setProjDir( const QString &appBundleDir )
{
#ifdef ANDROID
  // win and ios resources are already in the bundle
  InputUtils::cpDir( "assets:/qgis-data", appBundleDir );
#endif

#ifdef Q_OS_WIN32
  mProjDir = appBundleDir + "\\proj";
  QString projFilePath = mProjDir + "\\proj.db";
#else
  mProjDir = appBundleDir + "/proj";
  QString projFilePath = mProjDir + "/proj.db";
#endif

  QFile projdb( projFilePath );
  if ( !projdb.exists() )
  {
    CoreUtils::log( QStringLiteral( "PROJ6 error" ), QStringLiteral( "The Input has failed to load PROJ6 database." ) + projFilePath );
  }
}

void InputProjUtils::setCurrentCustomProjDir( const QString &dataDir )
{
#ifdef Q_OS_IOS
  // custom proj path has to be in data dir, not in the bundle
  mCurrentCustomProjDir = dataDir + "/qgis_data/proj_custom";
#else
  Q_UNUSED( dataDir )
  mCurrentCustomProjDir = mProjDir + "_custom";
#endif
}

void InputProjUtils::initProjLib( const QString &appBundleDir, const QString &dataDir, const QString &projectsPath )
{
  setProjDir( appBundleDir );
  setCurrentCustomProjDir( dataDir );

  QStringList paths = {mProjDir};
  qDebug() << "InputPROJ: Input Search Paths" << paths;
  qDebug() << "InputPROJ: Custom Search Path" << mCurrentCustomProjDir;

  cleanCustomDir();
  copyCustomProj( projectsPath );

  paths.append( mCurrentCustomProjDir );
  _updateProj( paths );
}

void InputProjUtils::resetHandlers()
{
  mPopUpShown = false;
  mMissingRequiredGridReported = false;
  mMissingPreferredGridReported = false;
  mCoordinateOperationCreationErrorReported = false;
  mMissingGridUsedByContextHandlerReported = false;
  mFallbackOperationOccurredReported = false;
}

void InputProjUtils::copyCustomProj( const QString &projectsPath )
{
  int nProjects = 0;

  QDirIterator it( projectsPath, QStringList() << QStringLiteral( "proj" ), QDir::Dirs, QDirIterator::Subdirectories );
  while ( it.hasNext() )
  {
    QDir projDir = it.next();
    if ( projDir.isReadable() && !projDir.isEmpty() )
    {
      bool success = InputUtils::cpDir( projDir.absolutePath(), mCurrentCustomProjDir );
      if ( success )
        qDebug() << "InputPROJ: copied custom projections from" << projDir.absolutePath();
      else
        qDebug() << "InputPROJ: failed to copy custom proj dir from" << projDir.absolutePath();

      ++nProjects;
    }
  }
  qDebug() << "InputPROJ: found" << nProjects << "projects with custom projections";
}
