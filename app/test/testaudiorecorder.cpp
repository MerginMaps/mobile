/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include "testaudiorecorder.h"
#include "testutils.h"

#include <QtTest/QtTest>
#include <QSignalSpy>
#include <QDir>
#include <QDirIterator>
#include <QFile>
#include <QUuid>

#include "attributecontroller.h"
#include "audiorecordercontroller.h"
#include "featurelayerpair.h"
#include "inpututils.h"
#include "rememberattributescontroller.h"

#include "qgsproject.h"
#include "qgsvectorlayer.h"

namespace
{
  // fields of the Survey layer in test_audio
  constexpr int NOTES_FIELD_INDEX = 2;
  constexpr int AUDIO_FIELD_INDEX = 3; // ExternalResource with the Audio document viewer
  constexpr int PHOTO_FIELD_INDEX = 4; // ExternalResource with the Image document viewer

  QgsVectorLayer *surveyLayer()
  {
    const QList<QgsMapLayer *> layers = QgsProject::instance()->mapLayersByName( QStringLiteral( "Survey" ) );
    return layers.isEmpty() ? nullptr : qobject_cast<QgsVectorLayer *>( layers.at( 0 ) );
  }

  QStringList audioFilesIn( const QString &dir )
  {
    QStringList files;
    QDirIterator it( dir, { QStringLiteral( "*.m4a" ) }, QDir::Files, QDirIterator::Subdirectories );
    while ( it.hasNext() )
    {
      files << it.next();
    }
    return files;
  }
}

void TestAudioRecorder::init()
{
  QDir( AudioRecorderController::temporaryAudioDir() ).removeRecursively();
}

void TestAudioRecorder::cleanup()
{
  QgsProject::instance()->clear();
  QDir( AudioRecorderController::temporaryAudioDir() ).removeRecursively();

  if ( !mProjectDir.isEmpty() )
  {
    QDir( mProjectDir ).removeRecursively();
    mProjectDir.clear();
  }
}

QString TestAudioRecorder::openTestProject( const QString &name )
{
  mProjectDir = QDir::tempPath() + QStringLiteral( "/MM_test_projects/" ) + name;
  QDir( mProjectDir ).removeRecursively();

  if ( !InputUtils::cpDir( TestUtils::testDataDir() + QStringLiteral( "/test_audio" ), mProjectDir ) )
    return QString();

  if ( !QgsProject::instance()->read( mProjectDir + QStringLiteral( "/test_audio.qgz" ) ) )
    return QString();

  return mProjectDir;
}

QString TestAudioRecorder::createTemporaryRecording()
{
  QDir().mkpath( AudioRecorderController::temporaryAudioDir() );
  const QString path = QDir( AudioRecorderController::temporaryAudioDir() ).filePath( QUuid::createUuid().toString( QUuid::WithoutBraces ) + QStringLiteral( ".m4a" ) );

  if ( !QFile::copy( TestUtils::testDataDir() + QStringLiteral( "/test_audio/recording.m4a" ), path ) )
    return QString();

  return path;
}

void TestAudioRecorder::testEditorSelection()
{
  const QString audioEditor = QStringLiteral( "../form/editors/MMFormAudioEditor.qml" );
  const QString photoEditor = QStringLiteral( "../form/editors/MMFormPhotoEditor.qml" );
  const QString notAvailable = QStringLiteral( "../form/editors/MMFormNotAvailable.qml" );

  const QVariantMap audioConfig { { QStringLiteral( "DocumentViewer" ), 3 } };

  QVERIFY( InputUtils::isAudioExternalResource( audioConfig ) );
  QCOMPARE( InputUtils::getFormEditorType( QStringLiteral( "ExternalResource" ), audioConfig ).toString(), audioEditor );

  // every other document viewer keeps the photo editor
  for ( int viewer : { 0, 1, 2, 4 } )
  {
    const QVariantMap config { { QStringLiteral( "DocumentViewer" ), viewer } };
    QVERIFY( !InputUtils::isAudioExternalResource( config ) );
    QCOMPARE( InputUtils::getFormEditorType( QStringLiteral( "ExternalResource" ), config ).toString(), photoEditor );
  }
  QCOMPARE( InputUtils::getFormEditorType( QStringLiteral( "ExternalResource" ), QVariantMap() ).toString(), photoEditor );

  // multi-edit is not supported for attachments
  QCOMPARE( InputUtils::getFormEditorType( QStringLiteral( "ExternalResource" ), audioConfig, QgsField(), QgsRelation(), QString(), true ).toString(), notAvailable );

  // the fixture project is set up in QGIS with the Audio viewer
  QVERIFY( !openTestProject( QStringLiteral( "testAudioEditorSelection" ) ).isEmpty() );
  QgsVectorLayer *layer = surveyLayer();
  QVERIFY( layer && layer->isValid() );
  QVERIFY( InputUtils::isAudioExternalResource( layer->editorWidgetSetup( AUDIO_FIELD_INDEX ).config() ) );
  QVERIFY( !InputUtils::isAudioExternalResource( layer->editorWidgetSetup( PHOTO_FIELD_INDEX ).config() ) );
}

void TestAudioRecorder::testPendingAudioMovedOnSave()
{
  const QString projectDir = openTestProject( QStringLiteral( "testPendingAudioMovedOnSave" ) );
  QVERIFY( !projectDir.isEmpty() );

  QgsVectorLayer *layer = surveyLayer();
  QVERIFY( layer && layer->isValid() );

  AttributeController controller;
  controller.setFeatureLayerPair( FeatureLayerPair( QgsFeature( layer->fields() ), layer ) );

  const QString audioId = controller.tabItem( 0 )->formItems().at( AUDIO_FIELD_INDEX ).toString();
  const QString tempPath = createTemporaryRecording();
  QVERIFY( !tempPath.isEmpty() );

  QVERIFY( controller.addPendingAudio( audioId, tempPath ) );

  // the value is the final path relative to the project, the file stays in the temporary folder until save
  const QString value = controller.featureLayerPair().feature().attribute( AUDIO_FIELD_INDEX ).toString();
  QVERIFY( value.startsWith( QStringLiteral( "audio_" ) ) );
  QVERIFY( value.endsWith( QStringLiteral( ".m4a" ) ) );
  QVERIFY( !QFile::exists( projectDir + "/" + value ) );
  QCOMPARE( controller.pendingAudioPath( audioId ), QFileInfo( tempPath ).absoluteFilePath() );
  QVERIFY( controller.hasAnyChanges() );

  QVERIFY( controller.save() );

  const QString savedValue = controller.featureLayerPair().feature().attribute( AUDIO_FIELD_INDEX ).toString();
  QCOMPARE( savedValue, value );
  QVERIFY( QFile::exists( projectDir + "/" + savedValue ) );
  QVERIFY( !QFile::exists( tempPath ) );
  QVERIFY( controller.pendingAudioPath( audioId ).isEmpty() );
}

void TestAudioRecorder::testPendingAudioFollowsPathExpression()
{
  const QString projectDir = openTestProject( QStringLiteral( "testPendingAudioFollowsPathExpression" ) );
  QVERIFY( !projectDir.isEmpty() );

  QgsVectorLayer *layer = surveyLayer();
  QVERIFY( layer && layer->isValid() );

  // audio is stored in a subfolder named after the "notes" field, which is filled in after recording
  QVariantMap config = layer->editorWidgetSetup( AUDIO_FIELD_INDEX ).config();
  config[ QStringLiteral( "RelativeStorage" ) ] = 1;
  config[ QStringLiteral( "PropertyCollection" ) ] = QVariantMap
  {
    {
      QStringLiteral( "properties" ), QVariantMap
      {
        { QStringLiteral( "propertyRootPath" ), QVariantMap { { QStringLiteral( "expression" ), QStringLiteral( "@project_folder + '/audio/' + \"notes\"" ) } } }
      }
    }
  };
  layer->setEditorWidgetSetup( AUDIO_FIELD_INDEX, QgsEditorWidgetSetup( QStringLiteral( "ExternalResource" ), config ) );

  AttributeController controller;
  controller.setFeatureLayerPair( FeatureLayerPair( QgsFeature( layer->fields() ), layer ) );

  const QVector<QUuid> items = controller.tabItem( 0 )->formItems();
  const QString tempPath = createTemporaryRecording();
  QVERIFY( controller.addPendingAudio( items.at( AUDIO_FIELD_INDEX ).toString(), tempPath ) );

  controller.setFormValue( items.at( NOTES_FIELD_INDEX ), QStringLiteral( "Open" ) );
  QVERIFY( controller.save() );

  const QString savedValue = controller.featureLayerPair().feature().attribute( AUDIO_FIELD_INDEX ).toString();
  QVERIFY( savedValue.startsWith( QStringLiteral( "audio/Open/audio_" ) ) );
  QVERIFY( QFile::exists( projectDir + "/" + savedValue ) );
  QVERIFY( !QFile::exists( tempPath ) );
}

void TestAudioRecorder::testPendingAudioRemovedOnRollback()
{
  const QString projectDir = openTestProject( QStringLiteral( "testPendingAudioRemovedOnRollback" ) );
  QVERIFY( !projectDir.isEmpty() );

  QgsVectorLayer *layer = surveyLayer();
  QVERIFY( layer && layer->isValid() );

  const QStringList filesBefore = audioFilesIn( projectDir );

  AttributeController controller;
  controller.setFeatureLayerPair( FeatureLayerPair( QgsFeature( layer->fields() ), layer ) );

  const QString audioId = controller.tabItem( 0 )->formItems().at( AUDIO_FIELD_INDEX ).toString();
  const QString tempPath = createTemporaryRecording();
  QVERIFY( controller.addPendingAudio( audioId, tempPath ) );

  controller.rollback();

  // no orphan files, neither in the temporary folder nor in the project
  QVERIFY( !QFile::exists( tempPath ) );
  QCOMPARE( audioFilesIn( projectDir ), filesBefore );
  QVERIFY( controller.pendingAudioPath( audioId ).isEmpty() );
}

void TestAudioRecorder::testPendingAudioReplaced()
{
  const QString projectDir = openTestProject( QStringLiteral( "testPendingAudioReplaced" ) );
  QVERIFY( !projectDir.isEmpty() );

  QgsVectorLayer *layer = surveyLayer();
  QVERIFY( layer && layer->isValid() );

  AttributeController controller;
  controller.setFeatureLayerPair( FeatureLayerPair( QgsFeature( layer->fields() ), layer ) );

  const QString audioId = controller.tabItem( 0 )->formItems().at( AUDIO_FIELD_INDEX ).toString();

  // re-recording replaces the first recording, which was never saved
  const QString firstRecording = createTemporaryRecording();
  QVERIFY( controller.addPendingAudio( audioId, firstRecording ) );

  const QString secondRecording = createTemporaryRecording();
  QVERIFY( controller.addPendingAudio( audioId, secondRecording, QStringLiteral( "second take.m4a" ) ) );

  QVERIFY( !QFile::exists( firstRecording ) );
  QCOMPARE( controller.pendingAudioPath( audioId ), QFileInfo( secondRecording ).absoluteFilePath() );
  QCOMPARE( controller.featureLayerPair().feature().attribute( AUDIO_FIELD_INDEX ).toString(), QStringLiteral( "second take.m4a" ) );

  QVERIFY( controller.save() );

  QVERIFY( QFile::exists( projectDir + QStringLiteral( "/second take.m4a" ) ) );
  QVERIFY( !QFile::exists( secondRecording ) );
}

void TestAudioRecorder::testPendingAudioRemovedWithValue()
{
  const QString projectDir = openTestProject( QStringLiteral( "testPendingAudioRemovedWithValue" ) );
  QVERIFY( !projectDir.isEmpty() );

  QgsVectorLayer *layer = surveyLayer();
  QVERIFY( layer && layer->isValid() );

  const QStringList filesBefore = audioFilesIn( projectDir );

  AttributeController controller;
  controller.setFeatureLayerPair( FeatureLayerPair( QgsFeature( layer->fields() ), layer ) );

  const QVector<QUuid> items = controller.tabItem( 0 )->formItems();
  const QString audioId = items.at( AUDIO_FIELD_INDEX ).toString();
  const QString tempPath = createTemporaryRecording();
  QVERIFY( controller.addPendingAudio( audioId, tempPath ) );

  // the user unlinks the recording before saving, e.g. the editor emits an empty value
  controller.setFormValue( items.at( AUDIO_FIELD_INDEX ), QVariant() );
  QVERIFY( controller.pendingAudioPath( audioId ).isEmpty() );

  QVERIFY( controller.save() );

  QVERIFY( !QFile::exists( tempPath ) );
  QCOMPARE( audioFilesIn( projectDir ), filesBefore );
}

void TestAudioRecorder::testPendingAudioMoveFailure()
{
  const QString projectDir = openTestProject( QStringLiteral( "testPendingAudioMoveFailure" ) );
  QVERIFY( !projectDir.isEmpty() );

  QgsVectorLayer *layer = surveyLayer();
  QVERIFY( layer && layer->isValid() );

  // audio is stored in a subfolder that cannot be created, a file of the same name is in the way
  QVariantMap config = layer->editorWidgetSetup( AUDIO_FIELD_INDEX ).config();
  config[ QStringLiteral( "RelativeStorage" ) ] = 1;
  config[ QStringLiteral( "DefaultRoot" ) ] = QStringLiteral( "blocked" );
  config.remove( QStringLiteral( "PropertyCollection" ) );
  layer->setEditorWidgetSetup( AUDIO_FIELD_INDEX, QgsEditorWidgetSetup( QStringLiteral( "ExternalResource" ), config ) );

  const QString blocker = projectDir + QStringLiteral( "/blocked" );
  QFile blockerFile( blocker );
  QVERIFY( blockerFile.open( QIODevice::WriteOnly ) );
  blockerFile.close();

  AttributeController controller;
  controller.setFeatureLayerPair( FeatureLayerPair( QgsFeature( layer->fields() ), layer ) );

  const QString audioId = controller.tabItem( 0 )->formItems().at( AUDIO_FIELD_INDEX ).toString();
  const QString tempPath = createTemporaryRecording();
  QVERIFY( controller.addPendingAudio( audioId, tempPath ) );

  const QString value = controller.featureLayerPair().feature().attribute( AUDIO_FIELD_INDEX ).toString();
  QVERIFY( value.startsWith( QStringLiteral( "blocked/audio_" ) ) );

  QSignalSpy commitFailedSpy( &controller, &AttributeController::commitFailed );
  QSignalSpy changesCommitedSpy( &controller, &AttributeController::changesCommited );
  const long long featureCount = layer->featureCount();

  // the feature is not saved with a path to a file that is not there
  QVERIFY( !controller.save() );
  QCOMPARE( commitFailedSpy.count(), 1 );
  QCOMPARE( changesCommitedSpy.count(), 0 );
  QCOMPARE( layer->featureCount(), featureCount );

  // the recording is kept, so the save can be repeated
  QVERIFY( QFile::exists( tempPath ) );
  QCOMPARE( controller.pendingAudioPath( audioId ), QFileInfo( tempPath ).absoluteFilePath() );

  QVERIFY( QFile::remove( blocker ) );
  QVERIFY( controller.save() );

  QCOMPARE( controller.featureLayerPair().feature().attribute( AUDIO_FIELD_INDEX ).toString(), value );
  QVERIFY( QFile::exists( projectDir + "/" + value ) );
  QVERIFY( !QFile::exists( tempPath ) );
  QCOMPARE( layer->featureCount(), featureCount + 1 );
}

void TestAudioRecorder::testPendingAudioKeptUntilCommit()
{
  const QString projectDir = openTestProject( QStringLiteral( "testPendingAudioKeptUntilCommit" ) );
  QVERIFY( !projectDir.isEmpty() );

  QgsVectorLayer *layer = surveyLayer();
  QVERIFY( layer && layer->isValid() );

  const QStringList filesBefore = audioFilesIn( projectDir );

  // a save that fails after the file was moved to the project, followed by a rollback, leaves no orphan file
  {
    AttributeController controller;
    controller.setFeatureLayerPair( FeatureLayerPair( QgsFeature( layer->fields() ), layer ) );

    const QString audioId = controller.tabItem( 0 )->formItems().at( AUDIO_FIELD_INDEX ).toString();
    QVERIFY( controller.addPendingAudio( audioId, createTemporaryRecording() ) );

    layer->setReadOnly( true ); // editing cannot start
    QVERIFY( !controller.save() );
    layer->setReadOnly( false );

    QCOMPARE( audioFilesIn( projectDir ).count(), filesBefore.count() + 1 );
    QVERIFY( !controller.pendingAudioPath( audioId ).isEmpty() );

    controller.rollback();
    QCOMPARE( audioFilesIn( projectDir ), filesBefore );
  }

  // a repeated save keeps the file that is already in place, it is not moved or renamed again
  AttributeController controller;
  controller.setFeatureLayerPair( FeatureLayerPair( QgsFeature( layer->fields() ), layer ) );

  const QString audioId = controller.tabItem( 0 )->formItems().at( AUDIO_FIELD_INDEX ).toString();
  QVERIFY( controller.addPendingAudio( audioId, createTemporaryRecording() ) );

  const QString value = controller.featureLayerPair().feature().attribute( AUDIO_FIELD_INDEX ).toString();
  const QString finalPath = projectDir + "/" + value;

  layer->setReadOnly( true );
  QVERIFY( !controller.save() );
  layer->setReadOnly( false );

  QVERIFY( QFile::exists( finalPath ) );
  QCOMPARE( QFileInfo( controller.pendingAudioPath( audioId ) ).canonicalFilePath(), QFileInfo( finalPath ).canonicalFilePath() );

  QVERIFY( controller.save() );

  QCOMPARE( controller.featureLayerPair().feature().attribute( AUDIO_FIELD_INDEX ).toString(), value );
  QVERIFY( QFile::exists( finalPath ) );
  QCOMPARE( audioFilesIn( projectDir ).count(), filesBefore.count() + 1 );
  QVERIFY( controller.pendingAudioPath( audioId ).isEmpty() );
  QVERIFY( audioFilesIn( AudioRecorderController::temporaryAudioDir() ).isEmpty() );
}

void TestAudioRecorder::testFileRemovalOnlyOnSave()
{
  const QString projectDir = openTestProject( QStringLiteral( "testFileRemovalOnlyOnSave" ) );
  QVERIFY( !projectDir.isEmpty() );

  QgsVectorLayer *layer = surveyLayer();
  QVERIFY( layer && layer->isValid() );

  const QString existingFile = projectDir + QStringLiteral( "/recording.m4a" );
  QVERIFY( QFile::exists( existingFile ) );

  // canceled form keeps the file
  {
    AttributeController controller;
    controller.setFeatureLayerPair( FeatureLayerPair( QgsFeature( layer->fields() ), layer ) );
    controller.scheduleFileRemoval( existingFile );
    controller.rollback();
  }
  QVERIFY( QFile::exists( existingFile ) );

  // saved form removes it
  AttributeController controller;
  controller.setFeatureLayerPair( FeatureLayerPair( QgsFeature( layer->fields() ), layer ) );
  controller.setFormValue( controller.tabItem( 0 )->formItems().at( NOTES_FIELD_INDEX ), QStringLiteral( "deleted audio" ) );
  controller.scheduleFileRemoval( existingFile );

  QVERIFY( QFile::exists( existingFile ) );
  QVERIFY( controller.save() );
  QVERIFY( !QFile::exists( existingFile ) );
}

void TestAudioRecorder::testAudioNotRemembered()
{
  const QString projectDir = openTestProject( QStringLiteral( "testAudioNotRemembered" ) );
  QVERIFY( !projectDir.isEmpty() );

  QgsVectorLayer *layer = surveyLayer();
  QVERIFY( layer && layer->isValid() );

  RememberAttributesController remController;
  remController.reset();
  remController.setRememberValuesAllowed( true );

  AttributeController controller1;
  controller1.setRememberAttributesController( &remController );
  controller1.setFeatureLayerPair( FeatureLayerPair( QgsFeature( layer->fields() ), layer ) );

  remController.setShouldRememberValue( layer, NOTES_FIELD_INDEX, true );
  remController.setShouldRememberValue( layer, AUDIO_FIELD_INDEX, true );

  const QVector<QUuid> items1 = controller1.tabItem( 0 )->formItems();
  controller1.setFormValue( items1.at( NOTES_FIELD_INDEX ), QStringLiteral( "first" ) );
  controller1.setFormValue( items1.at( AUDIO_FIELD_INDEX ), QStringLiteral( "recording.m4a" ) );
  QVERIFY( controller1.save() );

  const QStringList filesAfterFirst = audioFilesIn( projectDir );

  AttributeController controller2;
  controller2.setRememberAttributesController( &remController );
  controller2.setFeatureLayerPair( FeatureLayerPair( QgsFeature( layer->fields() ), layer ) );

  // other remembered values are reused, the audio field is left empty and no copy is made
  QCOMPARE( controller2.featureLayerPair().feature().attribute( NOTES_FIELD_INDEX ).toString(), QStringLiteral( "first" ) );
  QVERIFY( controller2.featureLayerPair().feature().attribute( AUDIO_FIELD_INDEX ).toString().isEmpty() );
  QCOMPARE( audioFilesIn( projectDir ), filesAfterFirst );
}

void TestAudioRecorder::testRecorderInitialState()
{
  AudioRecorderController recorder;

  QVERIFY( !recorder.isRecording() );
  QCOMPARE( recorder.duration(), qint64( 0 ) );
  QVERIFY( recorder.activeFieldItemId().isEmpty() );
  QVERIFY( recorder.outputPath().isEmpty() );
  QCOMPARE( recorder.maximumDuration(), qint64( 30 * 60 * 1000 ) );

  // stopping or discarding without a recording does nothing
  QSignalSpy finishedSpy( &recorder, &AudioRecorderController::recordingFinished );
  QSignalSpy discardedSpy( &recorder, &AudioRecorderController::recordingDiscarded );
  recorder.stopRecording();
  recorder.discardRecording();
  QCOMPARE( finishedSpy.count(), 0 );
  QCOMPARE( discardedSpy.count(), 0 );

  // a recording cannot be assigned anywhere without the form's controller
  QSignalSpy failedSpy( &recorder, &AudioRecorderController::recordingFailed );
  recorder.startRecording( QUuid::createUuid().toString() );
  QCOMPARE( failedSpy.count(), 1 );
  QVERIFY( !recorder.isRecording() );

  QVERIFY( AudioRecorderController::isSupportedAudioFile( QStringLiteral( "/data/Voice Note.M4A" ) ) );
  QVERIFY( AudioRecorderController::isSupportedAudioFile( QStringLiteral( "call.mp3" ) ) );
  QVERIFY( !AudioRecorderController::isSupportedAudioFile( QStringLiteral( "photo.jpg" ) ) );
  QVERIFY( AudioRecorderController::audioNameFilters().contains( QStringLiteral( "*.m4a" ) ) );
}

void TestAudioRecorder::testRecorderBusy()
{
  AttributeController controller;
  AudioRecorderController recorder;
  recorder.setAttributeController( &controller );

  // pretend another field is recording, a real recording needs a microphone
  const QString recordingField = QUuid::createUuid().toString();
  recorder.mState = AudioRecorderController::State::Recording;
  recorder.mActiveFieldItemId = recordingField;
  recorder.mActiveFieldTitle = QStringLiteral( "Voice note" );

  QSignalSpy busySpy( &recorder, &AudioRecorderController::recordingBusy );
  QSignalSpy startedSpy( &recorder, &AudioRecorderController::recordingStarted );
  QSignalSpy failedSpy( &recorder, &AudioRecorderController::recordingFailed );

  // another field is told the recorder is busy and nothing starts
  const QString otherField = QUuid::createUuid().toString();
  recorder.startRecording( otherField, QStringLiteral( "Bird call" ) );
  QCOMPARE( busySpy.count(), 1 );
  QCOMPARE( busySpy.at( 0 ).at( 0 ).toString(), otherField );
  QCOMPARE( recorder.activeFieldItemId(), recordingField );
  QCOMPARE( recorder.activeFieldTitle(), QStringLiteral( "Voice note" ) );

  // the recording field itself is ignored
  recorder.startRecording( recordingField, QStringLiteral( "Voice note" ) );
  QCOMPARE( busySpy.count(), 1 );
  QCOMPARE( startedSpy.count(), 0 );
  QCOMPARE( failedSpy.count(), 0 );

  recorder.mState = AudioRecorderController::State::Idle;
}

void TestAudioRecorder::testRecorderApplicationState()
{
  AudioRecorderController recorder;

  // pretend a field is recording, a real recording needs a microphone
  recorder.mState = AudioRecorderController::State::Recording;
  recorder.mActiveFieldItemId = QUuid::createUuid().toString();

  QSignalSpy interruptedSpy( &recorder, &AudioRecorderController::recordingInterrupted );

  // a system alert (e.g. the microphone permission dialog closing) makes the app inactive for a moment
  recorder.onApplicationStateChanged( Qt::ApplicationInactive );
  recorder.onApplicationStateChanged( Qt::ApplicationActive );
  QVERIFY( recorder.isRecording() );
  QVERIFY( recorder.mInterruptionMessage.isEmpty() );
  QCOMPARE( interruptedSpy.count(), 0 );

  recorder.mState = AudioRecorderController::State::Idle;
  recorder.mActiveFieldItemId.clear();

  // a recording granted while the permission dialog covers the app is dropped when the app goes to the background
  recorder.mPendingFieldItemId = QUuid::createUuid().toString();
  recorder.mPendingFieldTitle = QStringLiteral( "Voice note" );
  recorder.mPendingFileToReplace = QStringLiteral( "/project/previous.m4a" );
  recorder.onApplicationStateChanged( Qt::ApplicationInactive );
  QVERIFY( !recorder.mPendingFieldItemId.isEmpty() );
  recorder.onApplicationStateChanged( Qt::ApplicationSuspended );
  QVERIFY( recorder.mPendingFieldItemId.isEmpty() );
  QVERIFY( recorder.mPendingFieldTitle.isEmpty() );
  QVERIFY( recorder.mPendingFileToReplace.isEmpty() );
  QVERIFY( !recorder.isRecording() );
}

void TestAudioRecorder::testRecorderFailedRecording()
{
  const QString projectDir = openTestProject( QStringLiteral( "testRecorderFailedRecording" ) );
  QVERIFY( !projectDir.isEmpty() );

  QgsVectorLayer *layer = surveyLayer();
  QVERIFY( layer && layer->isValid() );

  AttributeController controller;
  controller.setFeatureLayerPair( FeatureLayerPair( QgsFeature( layer->fields() ), layer ) );

  AudioRecorderController recorder;
  recorder.setAttributeController( &controller );

  const QString audioId = controller.tabItem( 0 )->formItems().at( AUDIO_FIELD_INDEX ).toString();

  // pretend the field was recording and the recorder produced no file, a real recording needs a microphone
  recorder.mState = AudioRecorderController::State::Recording;
  recorder.mActiveFieldItemId = audioId;
  recorder.mOutputPath = QDir( AudioRecorderController::temporaryAudioDir() ).filePath( QStringLiteral( "missing.m4a" ) );

  QSignalSpy finishedSpy( &recorder, &AudioRecorderController::recordingFinished );
  QSignalSpy discardedSpy( &recorder, &AudioRecorderController::recordingDiscarded );
  QSignalSpy failedSpy( &recorder, &AudioRecorderController::recordingFailed );

  // the form saves or closes from the result signals, the recorder is idle by then
  bool idleWhenFailed = false;
  connect( &recorder, &AudioRecorderController::recordingFailed, &recorder, [&recorder, &idleWhenFailed]()
  {
    idleWhenFailed = !recorder.isRecording();
  } );

  recorder.finishRecording();

  // a failed recording is never reported as finished, so a save waiting for it does not go ahead
  QCOMPARE( failedSpy.count(), 1 );
  QCOMPARE( failedSpy.at( 0 ).at( 0 ).toString(), audioId );
  QCOMPARE( finishedSpy.count(), 0 );
  QCOMPARE( discardedSpy.count(), 0 );
  QVERIFY( idleWhenFailed );
  QVERIFY( !recorder.isRecording() );

  QVERIFY( controller.featureLayerPair().feature().attribute( AUDIO_FIELD_INDEX ).toString().isEmpty() );
  QVERIFY( controller.pendingAudioPath( audioId ).isEmpty() );
}

void TestAudioRecorder::testRecorderReplacesFile()
{
  const QString projectDir = openTestProject( QStringLiteral( "testRecorderReplacesFile" ) );
  QVERIFY( !projectDir.isEmpty() );

  QgsVectorLayer *layer = surveyLayer();
  QVERIFY( layer && layer->isValid() );

  const QString existingFile = projectDir + QStringLiteral( "/recording.m4a" );
  QVERIFY( QFile::exists( existingFile ) );

  // a discarded re-recording keeps the current file, even when the feature is saved
  {
    AttributeController controller;
    controller.setFeatureLayerPair( FeatureLayerPair( QgsFeature( layer->fields() ), layer ) );

    const QVector<QUuid> items = controller.tabItem( 0 )->formItems();
    controller.setFormValue( items.at( AUDIO_FIELD_INDEX ), QStringLiteral( "recording.m4a" ) );

    AudioRecorderController recorder;
    recorder.setAttributeController( &controller );

    // pretend the field was re-recording, a real recording needs a microphone
    recorder.mState = AudioRecorderController::State::Recording;
    recorder.mActiveFieldItemId = items.at( AUDIO_FIELD_INDEX ).toString();
    recorder.mOutputPath = createTemporaryRecording();
    recorder.mFileToReplace = existingFile;
    recorder.mDiscardRequested = true;

    QSignalSpy discardedSpy( &recorder, &AudioRecorderController::recordingDiscarded );
    recorder.finishRecording();
    QCOMPARE( discardedSpy.count(), 1 );
    QVERIFY( recorder.mFileToReplace.isEmpty() );

    QVERIFY( controller.save() );
    QVERIFY( QFile::exists( existingFile ) );
    QCOMPARE( controller.featureLayerPair().feature().attribute( AUDIO_FIELD_INDEX ).toString(), QStringLiteral( "recording.m4a" ) );
  }

  // a kept re-recording replaces it once the feature is saved
  AttributeController controller;
  controller.setFeatureLayerPair( FeatureLayerPair( QgsFeature( layer->fields() ), layer ) );

  const QVector<QUuid> items = controller.tabItem( 0 )->formItems();
  controller.setFormValue( items.at( AUDIO_FIELD_INDEX ), QStringLiteral( "recording.m4a" ) );

  AudioRecorderController recorder;
  recorder.setAttributeController( &controller );

  recorder.mState = AudioRecorderController::State::Recording;
  recorder.mActiveFieldItemId = items.at( AUDIO_FIELD_INDEX ).toString();
  recorder.mOutputPath = createTemporaryRecording();
  recorder.mFileToReplace = existingFile;

  // "Save while recording": the form saves right from recordingFinished(), no field editor is involved
  bool existedWhenFinished = false;
  bool saved = false;
  connect( &recorder, &AudioRecorderController::recordingFinished, &controller, [&]()
  {
    existedWhenFinished = QFile::exists( existingFile );
    saved = controller.save();
  } );

  recorder.finishRecording();

  QVERIFY( existedWhenFinished );
  QVERIFY( saved );
  QVERIFY( !QFile::exists( existingFile ) );

  const QString savedValue = controller.featureLayerPair().feature().attribute( AUDIO_FIELD_INDEX ).toString();
  QVERIFY( savedValue.startsWith( QStringLiteral( "audio_" ) ) );
  QVERIFY( QFile::exists( projectDir + "/" + savedValue ) );
}

void TestAudioRecorder::testRecorderWatchdog()
{
  const QString projectDir = openTestProject( QStringLiteral( "testRecorderWatchdog" ) );
  QVERIFY( !projectDir.isEmpty() );

  QgsVectorLayer *layer = surveyLayer();
  QVERIFY( layer && layer->isValid() );

  AttributeController controller;
  controller.setFeatureLayerPair( FeatureLayerPair( QgsFeature( layer->fields() ), layer ) );

  AudioRecorderController recorder;
  recorder.setAttributeController( &controller );

  const QString audioId = controller.tabItem( 0 )->formItems().at( AUDIO_FIELD_INDEX ).toString();

  QSignalSpy interruptedSpy( &recorder, &AudioRecorderController::recordingInterrupted );
  QSignalSpy finishedSpy( &recorder, &AudioRecorderController::recordingFinished );

  // a recording that gets no sound anymore (e.g. a phone call took the microphone) is stopped and kept
  recorder.mState = AudioRecorderController::State::Recording;
  recorder.mActiveFieldItemId = audioId;
  recorder.mOutputPath = createTemporaryRecording();

  recorder.onWatchdogTimeout();

  QCOMPARE( interruptedSpy.count(), 1 );
  QCOMPARE( finishedSpy.count(), 1 );
  QVERIFY( !recorder.isRecording() );
  QVERIFY( !controller.pendingAudioPath( audioId ).isEmpty() );

  // a recorder that never reports it stopped does not leave the form stuck
  recorder.mState = AudioRecorderController::State::Stopping;
  recorder.mActiveFieldItemId = audioId;
  recorder.mOutputPath = createTemporaryRecording();

  recorder.onWatchdogTimeout();

  QCOMPARE( interruptedSpy.count(), 1 );
  QCOMPARE( finishedSpy.count(), 2 );
  QVERIFY( !recorder.isRecording() );

  // nothing to do without a recording
  recorder.onWatchdogTimeout();
  QCOMPARE( finishedSpy.count(), 2 );

  // leaving the form stops with a message, other activities are only blocked on Android
  recorder.mState = AudioRecorderController::State::Recording;
  recorder.mActiveFieldItemId = audioId;
  recorder.mOutputPath = createTemporaryRecording();
#ifndef Q_OS_ANDROID
  QVERIFY( !recorder.blockExternalActivity() );
#endif
  recorder.interruptRecording( QStringLiteral( "left the form" ) );
  QCOMPARE( interruptedSpy.count(), 2 );
  QCOMPARE( interruptedSpy.at( 1 ).at( 1 ).toString(), QStringLiteral( "left the form" ) );
  QCOMPARE( finishedSpy.count(), 3 );
}

void TestAudioRecorder::testPendingAudioOutsideProject()
{
  const QString projectDir = openTestProject( QStringLiteral( "testPendingAudioOutsideProject" ) );
  QVERIFY( !projectDir.isEmpty() );

  QgsVectorLayer *layer = surveyLayer();
  QVERIFY( layer && layer->isValid() );

  // paths are relative to the project, but the default path is outside of it, so there is no valid value
  QVariantMap config = layer->editorWidgetSetup( AUDIO_FIELD_INDEX ).config();
  config[ QStringLiteral( "RelativeStorage" ) ] = 1;
  config[ QStringLiteral( "DefaultRoot" ) ] = QDir::tempPath() + QStringLiteral( "/MM_test_outside_project" );
  config.remove( QStringLiteral( "PropertyCollection" ) );
  layer->setEditorWidgetSetup( AUDIO_FIELD_INDEX, QgsEditorWidgetSetup( QStringLiteral( "ExternalResource" ), config ) );

  AttributeController controller;
  controller.setFeatureLayerPair( FeatureLayerPair( QgsFeature( layer->fields() ), layer ) );

  const QString audioId = controller.tabItem( 0 )->formItems().at( AUDIO_FIELD_INDEX ).toString();
  const QString tempPath = createTemporaryRecording();

  QVERIFY( !controller.addPendingAudio( audioId, tempPath ) );
  QVERIFY( controller.featureLayerPair().feature().attribute( AUDIO_FIELD_INDEX ).toString().isEmpty() );
  QVERIFY( controller.pendingAudioPath( audioId ).isEmpty() );

  // the recorder reports it as a failed recording and removes the file
  AudioRecorderController recorder;
  recorder.setAttributeController( &controller );
  recorder.mState = AudioRecorderController::State::Recording;
  recorder.mActiveFieldItemId = audioId;
  recorder.mOutputPath = tempPath;

  QSignalSpy failedSpy( &recorder, &AudioRecorderController::recordingFailed );
  recorder.finishRecording();
  QCOMPARE( failedSpy.count(), 1 );
  QVERIFY( !QFile::exists( tempPath ) );
}

void TestAudioRecorder::testPendingAudioKeptOnGeometryEdit()
{
  const QString projectDir = openTestProject( QStringLiteral( "testPendingAudioKeptOnGeometryEdit" ) );
  QVERIFY( !projectDir.isEmpty() );

  QgsVectorLayer *layer = surveyLayer();
  QVERIFY( layer && layer->isValid() );

  AttributeController controller;
  controller.setFeatureLayerPair( FeatureLayerPair( QgsFeature( layer->fields() ), layer ) );

  const QVector<QUuid> items = controller.tabItem( 0 )->formItems();
  const QString audioId = items.at( AUDIO_FIELD_INDEX ).toString();
  const QString tempPath = createTemporaryRecording();

  controller.setFormValue( items.at( NOTES_FIELD_INDEX ), QStringLiteral( "before editing" ) );
  QVERIFY( controller.addPendingAudio( audioId, tempPath ) );
  const QString value = controller.featureLayerPair().feature().attribute( AUDIO_FIELD_INDEX ).toString();

  // editing the geometry takes the feature of the form and gives it back with another geometry
  QgsFeature edited = controller.featureLayerPair().feature();
  edited.setGeometry( QgsGeometry::fromWkt( QStringLiteral( "POINT( 1 2 )" ) ) );
  controller.setFeatureLayerPair( FeatureLayerPair( edited, layer ) );

  // the recording and the other values are still there
  QCOMPARE( controller.featureLayerPair().feature().attribute( NOTES_FIELD_INDEX ).toString(), QStringLiteral( "before editing" ) );
  QCOMPARE( controller.featureLayerPair().feature().attribute( AUDIO_FIELD_INDEX ).toString(), value );
  QCOMPARE( controller.pendingAudioPath( audioId ), QFileInfo( tempPath ).absoluteFilePath() );

  QVERIFY( controller.save() );
  QVERIFY( QFile::exists( projectDir + "/" + value ) );
  QVERIFY( !QFile::exists( tempPath ) );
}

void TestAudioRecorder::testTemporaryFilesRemoved()
{
  // files left by a previous run of the app are removed on start
  const QString leftover = createTemporaryRecording();
  QVERIFY( QFile::exists( leftover ) );

  AudioRecorderController::removeTemporaryFiles();
  QVERIFY( !QFile::exists( leftover ) );
}

void TestAudioRecorder::testRecorderImport()
{
  const QString projectDir = openTestProject( QStringLiteral( "testRecorderImport" ) );
  QVERIFY( !projectDir.isEmpty() );

  QgsVectorLayer *layer = surveyLayer();
  QVERIFY( layer && layer->isValid() );

  AttributeController controller;
  controller.setFeatureLayerPair( FeatureLayerPair( QgsFeature( layer->fields() ), layer ) );

  AudioRecorderController recorder;
  recorder.setAttributeController( &controller );

  const QString audioId = controller.tabItem( 0 )->formItems().at( AUDIO_FIELD_INDEX ).toString();

  // a file chosen from the device somewhere outside the project
  const QString deviceDir = QDir::tempPath() + QStringLiteral( "/MM_test_device_files" );
  QDir( deviceDir ).removeRecursively();
  QVERIFY( QDir().mkpath( deviceDir ) );
  const QString deviceFile = deviceDir + QStringLiteral( "/Bird call.m4a" );
  QVERIFY( QFile::copy( TestUtils::testDataDir() + QStringLiteral( "/test_audio/recording.m4a" ), deviceFile ) );

  QVERIFY( !recorder.importAudio( audioId, QUrl::fromLocalFile( deviceDir + QStringLiteral( "/notes.txt" ) ) ) );
  QVERIFY( recorder.importAudio( audioId, QUrl::fromLocalFile( deviceFile ) ) );

  // the original is untouched, the field plays the temporary copy until save
  QVERIFY( QFile::exists( deviceFile ) );
  QCOMPARE( controller.featureLayerPair().feature().attribute( AUDIO_FIELD_INDEX ).toString(), QStringLiteral( "Bird call.m4a" ) );

  const QString finalPath = projectDir + QStringLiteral( "/Bird call.m4a" );
  const QString playbackPath = recorder.playbackPath( audioId, finalPath );
  QVERIFY( playbackPath.startsWith( AudioRecorderController::temporaryAudioDir() ) );
  QVERIFY( QFile::exists( playbackPath ) );

  QVERIFY( controller.save() );

  QVERIFY( QFile::exists( finalPath ) );
  QVERIFY( !QFile::exists( playbackPath ) );
  QCOMPARE( recorder.playbackPath( audioId, finalPath ), finalPath );

  QDir( deviceDir ).removeRecursively();
}
