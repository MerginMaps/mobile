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
#include "testutils.h"

#include <QtTest/QtTest>
#include <QDir>
#include <QFile>

void TestLocalProjectsManager::init()
{
  mDataDir = QDir::tempPath() + QStringLiteral( "/testLocalProjectsManager" );

  QDir dir( mDataDir );
  if ( dir.exists() )
    dir.removeRecursively();

  QDir().mkpath( mDataDir );

  TestUtils::createFakeLocalProject( mDataDir, QStringLiteral( "OriginalName" ) );
}

void TestLocalProjectsManager::cleanup()
{
  QDir( mDataDir ).removeRecursively();
}

void TestLocalProjectsManager::testRenameSuccess()
{
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
  LocalProjectsManager manager( mDataDir );
  QString projectId = manager.projects().first().id();

  QSignalSpy renamedSpy( &manager, &LocalProjectsManager::localProjectRenamed );
  QSignalSpy finishedSpy( &manager, &LocalProjectsManager::renameLocalProjectFinished );

  manager.renameLocalProject( projectId, QString() );

  QCOMPARE( finishedSpy.count(), 1 );
  QCOMPARE( finishedSpy.at( 0 ).at( 0 ).toBool(), false );
  QCOMPARE( renamedSpy.count(), 0 );
  QVERIFY( QDir( mDataDir + "/OriginalName" ).exists() );

  manager.renameLocalProject( projectId, QStringLiteral( "   " ) );

  QCOMPARE( finishedSpy.count(), 2 );
  QCOMPARE( finishedSpy.at( 1 ).at( 0 ).toBool(), false );
  QCOMPARE( renamedSpy.count(), 0 );
  QVERIFY( QDir( mDataDir + "/OriginalName" ).exists() );
}

void TestLocalProjectsManager::testRenameInvalidCharacters()
{
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
  TestUtils::createFakeLocalProject( mDataDir, QStringLiteral( "OtherProject" ) );

  LocalProjectsManager manager( mDataDir );
  QCOMPARE( manager.projects().size(), 2 );

  QSignalSpy renamedSpy( &manager, &LocalProjectsManager::localProjectRenamed );
  QSignalSpy finishedSpy( &manager, &LocalProjectsManager::renameLocalProjectFinished );

  manager.renameLocalProject( QStringLiteral( "OriginalName" ), QStringLiteral( "OtherProject" ) );

  QCOMPARE( finishedSpy.count(), 1 );
  QCOMPARE( finishedSpy.at( 0 ).at( 0 ).toBool(), false );
  QCOMPARE( renamedSpy.count(), 0 );
  QVERIFY( QDir( mDataDir + "/OriginalName" ).exists() );
  QVERIFY( QDir( mDataDir + "/OtherProject" ).exists() );
}

void TestLocalProjectsManager::testRenameSameName()
{
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

  QSignalSpy renamedSpy( &manager, &LocalProjectsManager::localProjectRenamed );
  QSignalSpy finishedSpy( &manager, &LocalProjectsManager::renameLocalProjectFinished );

  manager.renameLocalProject( QStringLiteral( "does-not-exist" ), QStringLiteral( "NewName" ) );

  QCOMPARE( finishedSpy.count(), 1 );
  QCOMPARE( finishedSpy.at( 0 ).at( 0 ).toBool(), false );
  QCOMPARE( renamedSpy.count(), 0 );
  QVERIFY( QDir( mDataDir + "/OriginalName" ).exists() );
  QVERIFY( !QDir( mDataDir + "/NewName" ).exists() );
}

void TestLocalProjectsManager::testRenameDirectoryCollision()
{
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

void TestLocalProjectsManager::testValidateRenameAccepts()
{
  LocalProjectsManager manager( mDataDir );
  QString projectId = manager.projects().first().id();

  QSignalSpy renamedSpy( &manager, &LocalProjectsManager::localProjectRenamed );
  QSignalSpy finishedSpy( &manager, &LocalProjectsManager::renameLocalProjectFinished );

  QCOMPARE( manager.validateRename( projectId, QStringLiteral( "NewName" ) ), QString() );
  QCOMPARE( manager.validateRename( projectId, QStringLiteral( "OriginalName" ) ), QString() ); // unchanged name is fine too

  // validateRename() must be side-effect-free
  QVERIFY( QDir( mDataDir + "/OriginalName" ).exists() );
  QVERIFY( QFile::exists( mDataDir + "/OriginalName/OriginalName.qgz" ) );
  QVERIFY( !QDir( mDataDir + "/NewName" ).exists() );

  const LocalProject project = manager.projectFromProjectId( projectId );
  QVERIFY( project.isValid() );
  QCOMPARE( project.projectName, QStringLiteral( "OriginalName" ) );
  QCOMPARE( project.projectDir, mDataDir + "/OriginalName" );
  QCOMPARE( project.qgisProjectFilePath, mDataDir + "/OriginalName/OriginalName.qgz" );

  QCOMPARE( renamedSpy.count(), 0 );
  QCOMPARE( finishedSpy.count(), 0 );
}

void TestLocalProjectsManager::testValidateRenameRejects()
{
  TestUtils::createFakeLocalProject( mDataDir, QStringLiteral( "OtherProject" ) );

  LocalProjectsManager manager( mDataDir );
  QString projectId = manager.projectFromDirectory( mDataDir + "/OriginalName" ).id();

  QVERIFY( !manager.validateRename( projectId, QString() ).isEmpty() );
  QVERIFY( !manager.validateRename( projectId, QStringLiteral( "   " ) ).isEmpty() );
  QVERIFY( !manager.validateRename( projectId, QStringLiteral( "Bad/Name" ) ).isEmpty() );
  QVERIFY( !manager.validateRename( projectId, QStringLiteral( "OtherProject" ) ).isEmpty() );
  QVERIFY( !manager.validateRename( QStringLiteral( "does-not-exist" ), QStringLiteral( "NewName" ) ).isEmpty() );
}
