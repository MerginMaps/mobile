/***************************************************************************
*                                                                         *
  *   This program is free software; you can redistribute it and/or modify  *
  *   it under the terms of the GNU General Public License as published by  *
  *   the Free Software Foundation; either version 2 of the License, or     *
  *   (at your option) any later version.                                   *
  *                                                                         *
  ***************************************************************************/

#include "testsketching.h"

#include <QSignalSpy>

#include "testutils.h"

void TestSketching::initTestCase()
{
  mController = std::make_unique<PhotoSketchingController>();
  QVERIFY( mTempDir.isValid() );
  QFile::copy( TestUtils::testDataDir() + QStringLiteral( "/photo.jpg" ), mTempDir.path() + QStringLiteral( "/photo.jpg" ) );
  mController->mPhotoSource = QUrl::fromLocalFile( mTempDir.path() + QStringLiteral( "/photo.jpg" ) ).toString();
  mController->mProjectName = mTempDir.path();
  mController->prepareController();
}

void TestSketching::cleanupTestCase()
{
  mController.reset();
}

void TestSketching::cleanup()
{
  mController->clear();
}

void TestSketching::testNewSketch()
{
  const QSignalSpy spy( mController.get(), &PhotoSketchingController::newPathAdded );
  const QSignalSpy spy2( mController.get(), &PhotoSketchingController::pathUpdated );
  QVERIFY( spy.isValid() );
  QVERIFY( spy2.isValid() );

  mController->newSketch();
  QVERIFY( mController->mCurrentLine.mPoints.isEmpty() );
  QCOMPARE( mController->mCurrentLine.mColor, QColor( Qt::white ) );

  mController->addPoint( {500, 500} );
  mController->addPoint( {800, 500} );
  mController->addPoint( {800, 800} );
  mController->addPoint( {500, 800} );
  QCOMPARE( mController->mActivePaths.size(), 1 );
  QCOMPARE( mController->mPaths.size(), 1 );
  QCOMPARE( mController->mActivePaths.last().mPoints.count(), 4 );
  QCOMPARE( mController->mPaths.last().mPoints.count(), 4 );
  QCOMPARE( spy.count(), 1 );
  QCOMPARE( spy2.count(), 3 );

  mController->newSketch();
  mController->addPoint( {700, 700} );
  mController->addPoint( {900, 700} );
  QCOMPARE( mController->mActivePaths.size(), 2 );
  QCOMPARE( mController->mPaths.size(), 2 );
  QCOMPARE( mController->mActivePaths.last().mPoints.count(), 2 );
  QCOMPARE( mController->mPaths.last().mPoints.count(), 2 );
  QCOMPARE( spy.count(), 2 );
  QCOMPARE( spy2.count(), 4 );
}

void TestSketching::testNewSketchRemovesShadowSketch()
{
  // a single point sketch, e.g. a stylus shadow sketch
  mController->newSketch();
  mController->addPoint( {500, 500} );
  QCOMPARE( mController->mActivePaths.size(), 1 );
  QCOMPARE( mController->mPaths.size(), 1 );

  const QSignalSpy spy( mController.get(), &PhotoSketchingController::lastPathRemoved );
  const QSignalSpy spy2( mController.get(), &PhotoSketchingController::canUndoChanged );
  QVERIFY( spy.isValid() );
  QVERIFY( spy2.isValid() );

  // the shadow sketch is dropped and QML is notified
  mController->newSketch();
  QCOMPARE( mController->mActivePaths.size(), 0 );
  QCOMPARE( mController->mPaths.size(), 0 );
  QCOMPARE( spy.count(), 1 );
  QCOMPARE( spy2.count(), 1 );
}

void TestSketching::testUndoSketch()
{
  // add sketches
  mController->newSketch();
  mController->addPoint( {500, 500} );
  mController->addPoint( {800, 500} );
  mController->addPoint( {800, 800} );
  mController->addPoint( {500, 800} );

  mController->newSketch();
  mController->addPoint( {700, 700} );
  mController->addPoint( {900, 700} );

  mController->newSketch();
  mController->addPoint( {200, 200} );
  mController->addPoint( {200, 900} );
  QCOMPARE( mController->mActivePaths.size(), 3 );
  QCOMPARE( mController->mPaths.size(), 3 );

  // undo all sketches
  const QSignalSpy spy( mController.get(), &PhotoSketchingController::lastPathRemoved );
  const QSignalSpy spy2( mController.get(), &PhotoSketchingController::canUndoChanged );
  QVERIFY( spy.isValid() );
  QVERIFY( spy2.isValid() );

  mController->undo();
  QCOMPARE( mController->mActivePaths.size(), 2 );
  QCOMPARE( mController->mPaths.size(), 2 );
  QCOMPARE( spy.count(), 1 );
  QCOMPARE( spy2.count(), 0 );
  mController->undo();
  QCOMPARE( mController->mActivePaths.size(), 1 );
  QCOMPARE( mController->mPaths.size(), 1 );
  QCOMPARE( spy.count(), 2 );
  QCOMPARE( spy2.count(), 0 );
  mController->undo();
  QCOMPARE( mController->mActivePaths.size(), 0 );
  QCOMPARE( mController->mPaths.size(), 0 );
  QCOMPARE( spy.count(), 3 );
  QCOMPARE( spy2.count(), 1 );
}

void TestSketching::testUndoDoesNotRestoreSketch()
{
  mController->newSketch();
  mController->addPoint( {500, 500} );
  mController->addPoint( {800, 500} );
  mController->addPoint( {800, 800} );

  mController->undo();
  QCOMPARE( mController->mPaths.size(), 0 );
  QVERIFY( !mController->mCanUndo );

  // the next point starts a new line, even without newSketch()
  mController->addPoint( {200, 200} );
  QCOMPARE( mController->mPaths.size(), 1 );
  QCOMPARE( mController->mPaths.last().mPoints.count(), 1 );

  mController->newSketch();
  mController->addPoint( {300, 300} );
  mController->addPoint( {400, 400} );
  mController->undo();
  QCOMPARE( mController->mPaths.size(), 0 );
  QVERIFY( !mController->mCanUndo );
}

void TestSketching::testSignatureDots()
{
  mController->mSignatureMode = true;

  // a tap, then a regular line
  mController->newSketch();
  mController->addPoint( {500, 500} );
  mController->newSketch();
  mController->addPoint( {600, 600} );
  mController->addPoint( {700, 700} );

  QCOMPARE( mController->mPaths.size(), 2 );
  QCOMPARE( mController->mPaths.first().mPoints.count(), 1 );

  // the dot is drawn as a tiny line
  QCOMPARE( mController->getPath( 0 ).mPoints.count(), 2 );
  QCOMPARE( mController->getPath( 1 ).mPoints.count(), 2 );

  mController->undo();
  mController->undo();
  QCOMPARE( mController->mPaths.size(), 0 );
  QVERIFY( !mController->mCanUndo );

  mController->mSignatureMode = false;
}

void TestSketching::testSketchBounds()
{
  const double photoScale = mController->mPhotoScale;
  mController->setPhotoScale( 1.0 );

  QVERIFY( mController->sketchBounds().isNull() );

  mController->newSketch();
  mController->addPoint( {500, 300} );
  QCOMPARE( mController->sketchBounds(), QRectF( 500, 300, 0, 0 ) );

  mController->addPoint( {800, 500} );
  mController->newSketch();
  mController->addPoint( {200, 400} );
  QCOMPARE( mController->sketchBounds(), QRectF( QPointF( 200, 300 ), QPointF( 800, 500 ) ) );

  mController->setPhotoScale( photoScale );
}

void TestSketching::testSignatureBackup()
{
  const QString photoSource = mController->mPhotoSource;
  const double photoScale = mController->mPhotoScale;
  mController->mSignatureMode = true;
  mController->mPhotoSource = QUrl::fromLocalFile( mTempDir.filePath( QStringLiteral( "signature.png" ) ) ).toString();
  mController->setPhotoScale( 1.0 );
  mController->setActiveColor( QColor( Qt::black ) );

  mController->newSketch();
  mController->addPoint( { -100, 50 } );
  mController->addPoint( {100, 150} );

  const QSignalSpy spy( mController.get(), &PhotoSketchingController::tempPhotoSourceChanged );
  mController->backupSketches();

  mController->mSignatureMode = false;
  mController->mPhotoSource = photoSource;
  mController->setPhotoScale( photoScale );

  QCOMPARE( spy.count(), 1 );

  // the image is as large as the strokes plus padding, negative coordinates included
  const QString signaturePath = spy.at( 0 ).at( 0 ).toString();
  const QImage image( signaturePath );
  QFile::remove( signaturePath );
  QCOMPARE( image.size(), QSize( 224, 124 ) );
  QCOMPARE( image.pixelColor( 112, 62 ), QColor( Qt::black ) );
}

void TestSketching::testChangeColor()
{
  const QSignalSpy spy( mController.get(), &PhotoSketchingController::activeColorChanged );
  QVERIFY( spy.isValid() );

  mController->setActiveColor( QColor( Qt::black ) );
  QCOMPARE( mController->mPenColor, QColor( Qt::black ) );
  QCOMPARE( mController->mCurrentLine.mColor, QColor( Qt::black ) );
  QCOMPARE( spy.count(), 1 );
}

void TestSketching::testChangePhotoSize()
{
  mController->setPhotoScale( 1 );
  mController->newSketch();
  mController->addPoint( {200, 200} );
  mController->addPoint( {500, 500} );

  const QSignalSpy spy( mController.get(), &PhotoSketchingController::pathUpdated );
  QVERIFY( spy.isValid() );
  mController->setPhotoScale( 0.5 );
  QCOMPARE( spy.count(), 1 );
}

void TestSketching::testSaveSketches()
{
  mController->newSketch();
  mController->addPoint( {500, 500} );
  mController->addPoint( {800, 500} );
  mController->addPoint( {800, 800} );
  mController->addPoint( {500, 800} );

  const QSignalSpy spy( mController.get(), &PhotoSketchingController::tempPhotoSourceChanged );
  QVERIFY( spy.isValid() );
  mController->backupSketches();
  QCOMPARE( mController->mActivePaths.count(), 0 );
  QCOMPARE( mController->mPaths.count(), 1 );
  QVERIFY( QFile( QStringLiteral( "%1/%2/photo.jpg" ).arg( QDir::tempPath(), mController->mProjectName ) ).exists() );
  QCOMPARE( spy.count(), 1 );
  const auto &signalArgs = spy.last();
  QCOMPARE( signalArgs.first().toString(), QStringLiteral( "%1/%2/photo.jpg" ).arg( QDir::tempPath(), mController->mProjectName ) );
}

void TestSketching::testLoadBackupSketch()
{
  const QString path = QStringLiteral( "file:///this/is/long/path/to/image/test_sketching/MM_test_image.jpg" );
  const QString path2 = QStringLiteral( "file:///this/is/long/path/to/image/test_sketching/MM_invalid_test_image.jpg" );
  QDir::temp().mkdir( QStringLiteral( "test_sketching" ) );
  QFile tempFile( QDir::tempPath() + QStringLiteral( "/test_sketching/MM_test_image.jpg" ) );
  tempFile.open( QIODevice::WriteOnly );
  tempFile.write( QByteArray( "Nice picture." ) );
  tempFile.close();
  QVERIFY( QDir( QDir::tempPath() + QStringLiteral( "/test_sketching" ) ).exists( "MM_test_image.jpg" ) );

  // check for existing backup
  PhotoSketchingController sketchingController;
  QSignalSpy spy( &sketchingController, &PhotoSketchingController::tempPhotoSourceChanged );
  QVERIFY( spy.isValid() );
  sketchingController.mPhotoSource = path;
  sketchingController.mProjectName = QStringLiteral( "/this/is/long/path/to/image/test_sketching" );
  sketchingController.prepareController();
  const QString localPath = QDir::tempPath() + QStringLiteral( "/test_sketching/MM_test_image.jpg" );
  const QString expectedUrl = QUrl::fromLocalFile( localPath ).toString();
  QCOMPARE( sketchingController.mPhotoSource, expectedUrl );
  QCOMPARE( sketchingController.mOriginalPhotoSource, QUrl( path ).toLocalFile() );
  QCOMPARE( spy.count(), 1 );
  auto signalArgs = spy.takeLast();
  QCOMPARE( signalArgs.first().toString(), QDir::tempPath() + QStringLiteral( "/test_sketching" ) + QStringLiteral( "/MM_test_image.jpg" ) );

  // check for nonexisting backup
  PhotoSketchingController sketchingController2;
  const QSignalSpy spy2( &sketchingController2, &PhotoSketchingController::tempPhotoSourceChanged );
  QVERIFY( spy2.isValid() );
  sketchingController2.mPhotoSource = path2;
  sketchingController2.prepareController();
  QCOMPARE( sketchingController2.mPhotoSource, path2 );
  QCOMPARE( sketchingController2.mOriginalPhotoSource, QUrl( path2 ).toLocalFile() );
  QCOMPARE( spy2.count(), 0 );
}
