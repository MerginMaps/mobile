/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include "testlocalprojectsmanager.h"
#include "localprojectsmanager.h"

#include <QtTest/QtTest>
#include <QDir>
#include <QFile>

namespace
{
  //! Creates a fake local project directory containing a dummy .qgz file, so
  //! LocalProjectsManager::findQgisProjectFile() has something to discover.
  QString createFakeProject( const QString &dataDir, const QString &name )
  {
    QString projectDir = dataDir + "/" + name;
    QDir().mkpath( projectDir );

    QFile qgzFile( projectDir + "/" + name + ".qgz" );
    qgzFile.open( QIODevice::WriteOnly );
    qgzFile.close();

    return projectDir;
  }
}

void TestLocalProjectsManager::init()
{
  mDataDir = QDir::tempPath() + QStringLiteral( "/testLocalProjectsManager" );

  QDir dir( mDataDir );
  if ( dir.exists() )
    dir.removeRecursively();

  QDir().mkpath( mDataDir );
}

void TestLocalProjectsManager::cleanup()
{
  QDir( mDataDir ).removeRecursively();
}

void TestLocalProjectsManager::testRenameSuccess()
{
  createFakeProject( mDataDir, "OriginalName" );

  LocalProjectsManager manager( mDataDir );
  QCOMPARE( manager.projects().size(), 1 );

  QString projectId = manager.projects().first().id();
  QCOMPARE( projectId, QStringLiteral( "OriginalName" ) );

  QSignalSpy renamedSpy( &manager, &LocalProjectsManager::localProjectRenamed );
  QSignalSpy finishedSpy( &manager, &LocalProjectsManager::renameLocalProjectFinished );

  manager.renameLocalProject( projectId, QStringLiteral( "NewName" ) );

  QCOMPARE( finishedSpy.count(), 1 );
  QCOMPARE( finishedSpy.at( 0 ).at( 0 ).toBool(), true );
  QCOMPARE( renamedSpy.count(), 1 );
  QCOMPARE( renamedSpy.at( 0 ).at( 0 ).toString(), projectId );

  QVERIFY( !QDir( mDataDir + "/OriginalName" ).exists() );
  QVERIFY( QDir( mDataDir + "/NewName" ).exists() );
  QVERIFY( QFile::exists( mDataDir + "/NewName/NewName.qgz" ) );

  LocalProject updated = manager.projectFromProjectId( QStringLiteral( "NewName" ) );
  QVERIFY( updated.isValid() );
  QCOMPARE( updated.projectName, QStringLiteral( "NewName" ) );
  QCOMPARE( updated.projectDir, mDataDir + "/NewName" );
  QCOMPARE( updated.qgisProjectFilePath, mDataDir + "/NewName/NewName.qgz" );
}

void TestLocalProjectsManager::testRenameEmptyName()
{
  createFakeProject( mDataDir, "OriginalName" );

  LocalProjectsManager manager( mDataDir );
  QString projectId = manager.projects().first().id();

  QSignalSpy renamedSpy( &manager, &LocalProjectsManager::localProjectRenamed );
  QSignalSpy finishedSpy( &manager, &LocalProjectsManager::renameLocalProjectFinished );

  manager.renameLocalProject( projectId, QStringLiteral( "   " ) );

  QCOMPARE( finishedSpy.count(), 1 );
  QCOMPARE( finishedSpy.at( 0 ).at( 0 ).toBool(), false );
  QCOMPARE( renamedSpy.count(), 0 );
  QVERIFY( QDir( mDataDir + "/OriginalName" ).exists() );
}

void TestLocalProjectsManager::testRenameInvalidCharacters()
{
  createFakeProject( mDataDir, "OriginalName" );

  LocalProjectsManager manager( mDataDir );
  QString projectId = manager.projects().first().id();

  QSignalSpy renamedSpy( &manager, &LocalProjectsManager::localProjectRenamed );
  QSignalSpy finishedSpy( &manager, &LocalProjectsManager::renameLocalProjectFinished );

  manager.renameLocalProject( projectId, QStringLiteral( "Bad/Name" ) );

  QCOMPARE( finishedSpy.count(), 1 );
  QCOMPARE( finishedSpy.at( 0 ).at( 0 ).toBool(), false );
  QCOMPARE( renamedSpy.count(), 0 );
  QVERIFY( QDir( mDataDir + "/OriginalName" ).exists() );
}

void TestLocalProjectsManager::testRenameNameAlreadyTaken()
{
  createFakeProject( mDataDir, "ProjectA" );
  createFakeProject( mDataDir, "ProjectB" );

  LocalProjectsManager manager( mDataDir );
  QCOMPARE( manager.projects().size(), 2 );

  QSignalSpy renamedSpy( &manager, &LocalProjectsManager::localProjectRenamed );
  QSignalSpy finishedSpy( &manager, &LocalProjectsManager::renameLocalProjectFinished );

  manager.renameLocalProject( QStringLiteral( "ProjectA" ), QStringLiteral( "ProjectB" ) );

  QCOMPARE( finishedSpy.count(), 1 );
  QCOMPARE( finishedSpy.at( 0 ).at( 0 ).toBool(), false );
  QCOMPARE( renamedSpy.count(), 0 );
  QVERIFY( QDir( mDataDir + "/ProjectA" ).exists() );
  QVERIFY( QDir( mDataDir + "/ProjectB" ).exists() );
}

void TestLocalProjectsManager::testRenameSameName()
{
  createFakeProject( mDataDir, "OriginalName" );

  LocalProjectsManager manager( mDataDir );
  QString projectId = manager.projects().first().id();

  QSignalSpy renamedSpy( &manager, &LocalProjectsManager::localProjectRenamed );
  QSignalSpy finishedSpy( &manager, &LocalProjectsManager::renameLocalProjectFinished );

  manager.renameLocalProject( projectId, QStringLiteral( "OriginalName" ) );

  // a no-op rename still reports success, but nothing on disk is touched
  QCOMPARE( finishedSpy.count(), 1 );
  QCOMPARE( finishedSpy.at( 0 ).at( 0 ).toBool(), true );
  QCOMPARE( renamedSpy.count(), 1 );
  QVERIFY( QDir( mDataDir + "/OriginalName" ).exists() );
}

void TestLocalProjectsManager::testRenameUnknownProject()
{
  LocalProjectsManager manager( mDataDir );
  QCOMPARE( manager.projects().size(), 0 );

  QSignalSpy renamedSpy( &manager, &LocalProjectsManager::localProjectRenamed );
  QSignalSpy finishedSpy( &manager, &LocalProjectsManager::renameLocalProjectFinished );

  manager.renameLocalProject( QStringLiteral( "does-not-exist" ), QStringLiteral( "NewName" ) );

  QCOMPARE( finishedSpy.count(), 1 );
  QCOMPARE( finishedSpy.at( 0 ).at( 0 ).toBool(), false );
  QCOMPARE( renamedSpy.count(), 0 );
}

void TestLocalProjectsManager::testRenameDirectoryCollision()
{
  createFakeProject( mDataDir, "OriginalName" );

  LocalProjectsManager manager( mDataDir );
  QString projectId = manager.projects().first().id();

  // A folder that already exists on disk at the rename target path, but that the
  // manager does not know about (created after it last scanned mDataDir).
  QDir().mkpath( mDataDir + "/NewName" );

  QSignalSpy renamedSpy( &manager, &LocalProjectsManager::localProjectRenamed );
  QSignalSpy finishedSpy( &manager, &LocalProjectsManager::renameLocalProjectFinished );

  manager.renameLocalProject( projectId, QStringLiteral( "NewName" ) );

  QCOMPARE( finishedSpy.count(), 1 );
  QCOMPARE( finishedSpy.at( 0 ).at( 0 ).toBool(), true );
  QCOMPARE( renamedSpy.count(), 1 );
  QVERIFY( QDir( mDataDir + "/NewName (1)" ).exists() );

  LocalProject updated = manager.projectFromProjectId( QStringLiteral( "NewName (1)" ) );
  QVERIFY( updated.isValid() );
  QCOMPARE( updated.projectDir, mDataDir + "/NewName (1)" );
}

void TestLocalProjectsManager::testRenameTrimsWhitespace()
{
  createFakeProject( mDataDir, "OriginalName" );

  LocalProjectsManager manager( mDataDir );
  QString projectId = manager.projects().first().id();

  QSignalSpy renamedSpy( &manager, &LocalProjectsManager::localProjectRenamed );
  QSignalSpy finishedSpy( &manager, &LocalProjectsManager::renameLocalProjectFinished );

  manager.renameLocalProject( projectId, QStringLiteral( "  NewName  " ) );

  QCOMPARE( finishedSpy.count(), 1 );
  QCOMPARE( finishedSpy.at( 0 ).at( 0 ).toBool(), true );
  QCOMPARE( renamedSpy.count(), 1 );
  QVERIFY( QDir( mDataDir + "/NewName" ).exists() );

  LocalProject updated = manager.projectFromProjectId( QStringLiteral( "NewName" ) );
  QVERIFY( updated.isValid() );
  QCOMPARE( updated.projectName, QStringLiteral( "NewName" ) );
}

void TestLocalProjectsManager::testCanRenameProjectAccepts()
{
  createFakeProject( mDataDir, "OriginalName" );

  LocalProjectsManager manager( mDataDir );
  QString projectId = manager.projects().first().id();

  QSignalSpy renamedSpy( &manager, &LocalProjectsManager::localProjectRenamed );
  QSignalSpy finishedSpy( &manager, &LocalProjectsManager::renameLocalProjectFinished );

  QCOMPARE( manager.canRenameProject( projectId, QStringLiteral( "NewName" ) ), QString() );
  QCOMPARE( manager.canRenameProject( projectId, QStringLiteral( "OriginalName" ) ), QString() ); // unchanged name is fine too

  // canRenameProject() must be side-effect-free
  QVERIFY( QDir( mDataDir + "/OriginalName" ).exists() );
  QCOMPARE( manager.projectFromProjectId( projectId ).projectName, QStringLiteral( "OriginalName" ) );
  QCOMPARE( renamedSpy.count(), 0 );
  QCOMPARE( finishedSpy.count(), 0 );
}

void TestLocalProjectsManager::testCanRenameProjectRejects()
{
  createFakeProject( mDataDir, "ProjectA" );
  createFakeProject( mDataDir, "ProjectB" );

  LocalProjectsManager manager( mDataDir );
  QString projectId = manager.projectFromDirectory( mDataDir + "/ProjectA" ).id();

  QVERIFY( !manager.canRenameProject( projectId, QStringLiteral( "   " ) ).isEmpty() );
  QVERIFY( !manager.canRenameProject( projectId, QStringLiteral( "Bad/Name" ) ).isEmpty() );
  QVERIFY( !manager.canRenameProject( projectId, QStringLiteral( "ProjectB" ) ).isEmpty() );
  QVERIFY( !manager.canRenameProject( QStringLiteral( "does-not-exist" ), QStringLiteral( "NewName" ) ).isEmpty() );
}
