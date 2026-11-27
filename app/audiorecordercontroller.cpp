/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include "audiorecordercontroller.h"

#include <QAudioDevice>
#include <QAudioInput>
#include <QCoreApplication>
#include <QDir>
#include <QFile>
#include <QFileInfo>
#include <QGuiApplication>
#include <QMediaCaptureSession>
#include <QMediaDevices>
#include <QMediaFormat>
#include <QPermissions>
#include <QUuid>

#include <utility>

#include "coreutils.h"

namespace
{
  // speech quality, about 0.5 MB per minute
  constexpr int AUDIO_SAMPLE_RATE = 44100;
  constexpr int AUDIO_BIT_RATE = 64000;
  constexpr int AUDIO_CHANNEL_COUNT = 1;

  const QStringList &supportedAudioSuffixes()
  {
    static const QStringList suffixes =
    {
      QStringLiteral( "m4a" ),
      QStringLiteral( "aac" ),
      QStringLiteral( "mp3" ),
      QStringLiteral( "wav" ),
      QStringLiteral( "ogg" ),
      QStringLiteral( "opus" ),
      QStringLiteral( "flac" )
    };
    return suffixes;
  }
}

AudioRecorderController::AudioRecorderController( QObject *parent )
  : QObject( parent )
{
  if ( QGuiApplication *app = qobject_cast<QGuiApplication *>( QCoreApplication::instance() ) )
  {
    connect( app, &QGuiApplication::applicationStateChanged, this, &AudioRecorderController::onApplicationStateChanged );
  }

  mWatchdog.setSingleShot( true );
  connect( &mWatchdog, &QTimer::timeout, this, &AudioRecorderController::onWatchdogTimeout );
}

AudioRecorderController::~AudioRecorderController()
{
  // the form is closing, an unfinished recording is never assigned to a field
  if ( mRecorder && mState != State::Idle )
  {
    mRecorder->disconnect( this );
    mRecorder->stop();
    QFile::remove( mOutputPath );

    // the recorder can adjust the file name
    const QString actualPath = mRecorder->actualLocation().toLocalFile();
    if ( !actualPath.isEmpty() && actualPath != mOutputPath )
    {
      QFile::remove( actualPath );
    }
  }
}

AttributeController *AudioRecorderController::attributeController() const
{
  return mAttributeController;
}

void AudioRecorderController::setAttributeController( AttributeController *controller )
{
  if ( mAttributeController == controller )
    return;

  mAttributeController = controller;
  emit attributeControllerChanged();
}

bool AudioRecorderController::isRecording() const
{
  return mState != State::Idle;
}

qint64 AudioRecorderController::duration() const
{
  return mDuration;
}

QString AudioRecorderController::activeFieldItemId() const
{
  return mActiveFieldItemId;
}

QString AudioRecorderController::activeFieldTitle() const
{
  return mActiveFieldTitle;
}

QString AudioRecorderController::outputPath() const
{
  return mOutputPath;
}

qint64 AudioRecorderController::maximumDuration() const
{
  return MAXIMUM_DURATION_MS;
}

void AudioRecorderController::startRecording( const QString &fieldItemId, const QString &fieldTitle, const QString &fileToReplace )
{
  if ( fieldItemId.isEmpty() )
    return;

  if ( !mAttributeController )
  {
    CoreUtils::log( QStringLiteral( "Audio recorder" ), QStringLiteral( "Cannot record without an attribute controller" ) );
    emit recordingFailed( fieldItemId, tr( "Could not start the recording" ) );
    return;
  }

  if ( mState != State::Idle )
  {
    if ( mActiveFieldItemId != fieldItemId )
    {
      emit recordingBusy( fieldItemId );
    }
    return;
  }

  // the system dialog is on its way, a second tap must not ask again
  if ( mPermissionRequestPending )
    return;

  const QMicrophonePermission permission;
  switch ( qApp->checkPermission( permission ) )
  {
    case Qt::PermissionStatus::Undetermined:
      mPermissionRequestPending = true;
      qApp->requestPermission( permission, this, [this, fieldItemId, fieldTitle, fileToReplace]( const QPermission & result )
      {
        mPermissionRequestPending = false;

        if ( result.status() != Qt::PermissionStatus::Granted )
        {
          emit microphonePermissionDenied();
        }
        else if ( QGuiApplication::applicationState() == Qt::ApplicationActive )
        {
          record( fieldItemId, fieldTitle, fileToReplace );
        }
        else
        {
          // Android delivers the result while the permission dialog still covers the app,
          // start once the app is in the foreground again
          mPendingFieldItemId = fieldItemId;
          mPendingFieldTitle = fieldTitle;
          mPendingFileToReplace = fileToReplace;
        }
      } );
      return;
    case Qt::PermissionStatus::Denied:
      emit microphonePermissionDenied();
      return;
    case Qt::PermissionStatus::Granted:
      break;
  }

  record( fieldItemId, fieldTitle, fileToReplace );
}

void AudioRecorderController::stopRecording()
{
  if ( mState != State::Recording )
    return;

  setState( State::Stopping );

  if ( mRecorder )
  {
    mRecorder->stop();
  }

  // some backends report the stopped state synchronously, some never do when nothing was recorded
  if ( ( !mRecorder || mRecorder->recorderState() == QMediaRecorder::StoppedState ) && mState == State::Stopping )
  {
    finishRecording();
  }
}

void AudioRecorderController::interruptRecording( const QString &message )
{
  if ( mState != State::Recording )
    return;

  if ( mInterruptionMessage.isEmpty() )
  {
    mInterruptionMessage = message;
  }
  stopRecording();
}

bool AudioRecorderController::blockExternalActivity()
{
#ifdef Q_OS_ANDROID
  // another activity suspends the app and there is no background recording
  if ( mState != State::Idle )
  {
    emit recordingBusy( QString() );
    return true;
  }
#endif
  return false;
}

void AudioRecorderController::discardRecording()
{
  if ( mState == State::Idle )
    return;

  mDiscardRequested = true;

  if ( mState == State::Recording )
  {
    stopRecording();
  }
}

bool AudioRecorderController::importAudio( const QString &fieldItemId, const QUrl &source )
{
  if ( !mAttributeController || fieldItemId.isEmpty() )
    return false;

  // content:// URIs on Android are opened by QFile directly
  const QString sourcePath = source.isLocalFile() ? source.toLocalFile() : source.toString();
  const QString fileName = QFileInfo( sourcePath ).fileName();

  if ( !isSupportedAudioFile( fileName ) )
  {
    CoreUtils::log( QStringLiteral( "Audio recorder" ), QStringLiteral( "Unsupported audio file %1" ).arg( sourcePath ) );
    return false;
  }

  if ( !QDir().mkpath( temporaryAudioDir() ) )
  {
    CoreUtils::log( QStringLiteral( "Audio recorder" ), QStringLiteral( "Could not create directory %1" ).arg( temporaryAudioDir() ) );
    return false;
  }

  const QString tempPath = QDir( temporaryAudioDir() ).filePath( QStringLiteral( "%1.%2" ).arg( QUuid::createUuid().toString( QUuid::WithoutBraces ), QFileInfo( fileName ).suffix().toLower() ) );

  if ( !QFile::copy( sourcePath, tempPath ) )
  {
    CoreUtils::log( QStringLiteral( "Audio recorder" ), QStringLiteral( "Could not copy audio file from %1 to %2" ).arg( sourcePath, tempPath ) );
    return false;
  }

  if ( !mAttributeController->addPendingAudio( fieldItemId, tempPath, fileName ) )
  {
    QFile::remove( tempPath );
    return false;
  }

  return true;
}

QString AudioRecorderController::playbackPath( const QString &fieldItemId, const QString &absolutePath ) const
{
  if ( mAttributeController )
  {
    const QString pending = mAttributeController->pendingAudioPath( fieldItemId );
    if ( !pending.isEmpty() )
      return pending;
  }

  return absolutePath;
}

bool AudioRecorderController::hasMicrophone()
{
  return !QMediaDevices::audioInputs().isEmpty();
}

bool AudioRecorderController::isSupportedAudioFile( const QString &path )
{
  return supportedAudioSuffixes().contains( QFileInfo( path ).suffix().toLower() );
}

QStringList AudioRecorderController::audioNameFilters()
{
  QStringList filters;
  for ( const QString &suffix : supportedAudioSuffixes() )
  {
    filters << QStringLiteral( "*.%1" ).arg( suffix );
  }
  return filters;
}

QString AudioRecorderController::temporaryAudioDir()
{
  return QDir::temp().absoluteFilePath( QStringLiteral( "mergin-audio" ) );
}

void AudioRecorderController::removeTemporaryFiles()
{
  // nothing in there belongs to a form anymore, e.g. after the app was closed with a form open
  QDir( temporaryAudioDir() ).removeRecursively();
}

void AudioRecorderController::onWatchdogTimeout()
{
  if ( mState == State::Recording )
  {
    CoreUtils::log( QStringLiteral( "Audio recorder" ), QStringLiteral( "No sound recorded for %1 ms, stopping" ).arg( STALLED_TIMEOUT_MS ) );
    interruptRecording( tr( "Recording stopped because the microphone was interrupted" ) );
  }
  else if ( mState == State::Stopping )
  {
    // keep what was written instead of leaving the form stuck
    CoreUtils::log( QStringLiteral( "Audio recorder" ), QStringLiteral( "The recorder did not report it stopped, finishing anyway" ) );
    finishRecording();
  }
}

void AudioRecorderController::onRecorderStateChanged( QMediaRecorder::RecorderState state )
{
  if ( state == QMediaRecorder::StoppedState && mState != State::Idle )
  {
    finishRecording();
  }
}

void AudioRecorderController::onRecorderError( QMediaRecorder::Error error, const QString &errorString )
{
  if ( error == QMediaRecorder::NoError )
    return;

  CoreUtils::log( QStringLiteral( "Audio recorder" ), QStringLiteral( "Recording error %1: %2" ).arg( static_cast<int>( error ) ).arg( errorString ) );

  if ( mState == State::Idle )
    return;

  // e.g. another app took the microphone, keep what was recorded so far
  if ( mInterruptionMessage.isEmpty() )
  {
    mInterruptionMessage = tr( "Recording stopped: %1" ).arg( errorString );
  }

  if ( mState == State::Recording )
  {
    stopRecording();
  }
  else if ( mRecorder->recorderState() == QMediaRecorder::StoppedState )
  {
    finishRecording();
  }
}

void AudioRecorderController::onRecorderDurationChanged( qint64 duration )
{
  if ( mState != State::Recording )
    return;

  // sound is coming in
  if ( duration != mDuration )
  {
    mWatchdog.start( STALLED_TIMEOUT_MS );
  }

  setDuration( duration );

  if ( duration >= DURATION_WARNING_MS && !mDurationWarningEmitted )
  {
    mDurationWarningEmitted = true;
    emit maximumDurationWarning();
  }

  if ( duration >= MAXIMUM_DURATION_MS )
  {
    mInterruptionMessage = tr( "Recording stopped after reaching the maximum length of %1 minutes" ).arg( MAXIMUM_DURATION_MS / 60000 );
    stopRecording();
  }
}

void AudioRecorderController::onApplicationStateChanged( Qt::ApplicationState state )
{
  if ( state == Qt::ApplicationActive )
  {
    // timers do not run while the app is suspended, give the recorder its full time again
    if ( mWatchdog.isActive() )
    {
      mWatchdog.start();
    }

    if ( !mPendingFieldItemId.isEmpty() )
    {
      const QString fieldItemId = std::exchange( mPendingFieldItemId, QString() );
      const QString fieldTitle = std::exchange( mPendingFieldTitle, QString() );
      const QString fileToReplace = std::exchange( mPendingFileToReplace, QString() );
      record( fieldItemId, fieldTitle, fileToReplace );
    }
    return;
  }

  // Inactive is only a short interruption that keeps the app on screen (system alerts such as
  // the permission dialog, Control Center, notification banners), the recording continues
  if ( state == Qt::ApplicationInactive )
    return;

  // the user left before the permission dialog flow finished
  mPendingFieldItemId.clear();
  mPendingFieldTitle.clear();
  mPendingFileToReplace.clear();

  // no background recording in v1, stop and keep the file
  interruptRecording( tr( "Recording stopped because the app went to the background" ) );
}

void AudioRecorderController::setupRecorder()
{
  mAudioInput = std::make_unique<QAudioInput>();
  mRecorder = std::make_unique<QMediaRecorder>();
  mCaptureSession = std::make_unique<QMediaCaptureSession>();

  mCaptureSession->setAudioInput( mAudioInput.get() );
  mCaptureSession->setRecorder( mRecorder.get() );

  // AAC in .m4a plays natively on Android, iOS, in QGIS and in browsers
  QMediaFormat format( QMediaFormat::MPEG4 );
  format.setAudioCodec( QMediaFormat::AudioCodec::AAC );
  mRecorder->setMediaFormat( format );
  mRecorder->setAudioChannelCount( AUDIO_CHANNEL_COUNT );
  mRecorder->setAudioSampleRate( AUDIO_SAMPLE_RATE );
  mRecorder->setAudioBitRate( AUDIO_BIT_RATE );
  mRecorder->setEncodingMode( QMediaRecorder::ConstantBitRateEncoding );

  connect( mRecorder.get(), &QMediaRecorder::recorderStateChanged, this, &AudioRecorderController::onRecorderStateChanged );
  connect( mRecorder.get(), &QMediaRecorder::errorOccurred, this, &AudioRecorderController::onRecorderError );
  connect( mRecorder.get(), &QMediaRecorder::durationChanged, this, &AudioRecorderController::onRecorderDurationChanged );
}

void AudioRecorderController::record( const QString &fieldItemId, const QString &fieldTitle, const QString &fileToReplace )
{
  // the permission callback can arrive after another recording started
  if ( mState != State::Idle )
    return;

  if ( !hasMicrophone() )
  {
    CoreUtils::log( QStringLiteral( "Audio recorder" ), QStringLiteral( "No audio input available" ) );
    emit recordingFailed( fieldItemId, tr( "No microphone found" ) );
    return;
  }

  if ( !QDir().mkpath( temporaryAudioDir() ) )
  {
    CoreUtils::log( QStringLiteral( "Audio recorder" ), QStringLiteral( "Could not create directory %1" ).arg( temporaryAudioDir() ) );
    emit recordingFailed( fieldItemId, tr( "Could not start the recording" ) );
    return;
  }

  if ( !mRecorder )
  {
    setupRecorder();
  }

  const QString path = QDir( temporaryAudioDir() ).filePath( QStringLiteral( "%1.m4a" ).arg( QUuid::createUuid().toString( QUuid::WithoutBraces ) ) );

  mDiscardRequested = false;
  mDurationWarningEmitted = false;
  mInterruptionMessage.clear();
  mFileToReplace = fileToReplace;
  setDuration( 0 );
  setOutputPath( path );
  setActiveField( fieldItemId, fieldTitle );
  // the state changes before the recorder starts, so the players of the form release the audio session first
  setState( State::Recording );
  emit recordingStarted( fieldItemId );

  // errors can be reported synchronously from record(), they end up in finishRecording()
  mRecorder->setOutputLocation( QUrl::fromLocalFile( path ) );
  mRecorder->record();
}

void AudioRecorderController::finishRecording()
{
  const QString fieldItemId = mActiveFieldItemId;
  const QString interruptionMessage = mInterruptionMessage;
  const QString fileToReplace = std::exchange( mFileToReplace, QString() );

  QString path = mRecorder ? mRecorder->actualLocation().toLocalFile() : QString();
  if ( path.isEmpty() )
  {
    path = mOutputPath;
  }

  enum class Result { Finished, Discarded, Failed };
  Result result = Result::Finished;

  if ( mDiscardRequested )
  {
    QFile::remove( path );
    result = Result::Discarded;
  }
  else if ( QFileInfo( path ).size() <= 0 )
  {
    CoreUtils::log( QStringLiteral( "Audio recorder" ), QStringLiteral( "Recording produced no file at %1" ).arg( path ) );
    QFile::remove( path );
    result = Result::Failed;
  }
  else if ( !mAttributeController || !mAttributeController->addPendingAudio( fieldItemId, path ) )
  {
    QFile::remove( path );
    result = Result::Failed;
  }
  else if ( !fileToReplace.isEmpty() )
  {
    // only a kept recording replaces the previous file, which stays in the project until the feature is saved
    mAttributeController->scheduleFileRemoval( fileToReplace );
  }

  // the value is assigned and the replaced file is scheduled before the signals below,
  // so the form can save right from recordingFinished()
  mDiscardRequested = false;
  mInterruptionMessage.clear();
  setActiveField( QString(), QString() );
  setOutputPath( QString() );
  setState( State::Idle );

  switch ( result )
  {
    case Result::Finished:
      if ( !interruptionMessage.isEmpty() )
      {
        emit recordingInterrupted( fieldItemId, interruptionMessage );
      }
      emit recordingFinished( fieldItemId );
      break;
    case Result::Discarded:
      emit recordingDiscarded( fieldItemId );
      break;
    case Result::Failed:
      emit recordingFailed( fieldItemId, tr( "Recording failed" ) );
      break;
  }
}

void AudioRecorderController::setState( State state )
{
  const bool wasRecording = isRecording();
  mState = state;

  switch ( state )
  {
    case State::Recording:
      mWatchdog.start( STALLED_TIMEOUT_MS );
      break;
    case State::Stopping:
      mWatchdog.start( STOP_TIMEOUT_MS );
      break;
    case State::Idle:
      mWatchdog.stop();
      break;
  }

  if ( wasRecording != isRecording() )
  {
    emit isRecordingChanged();
  }
}

void AudioRecorderController::setActiveField( const QString &fieldItemId, const QString &fieldTitle )
{
  if ( mActiveFieldItemId == fieldItemId && mActiveFieldTitle == fieldTitle )
    return;

  mActiveFieldItemId = fieldItemId;
  mActiveFieldTitle = fieldTitle;
  emit activeFieldItemIdChanged();
}

void AudioRecorderController::setOutputPath( const QString &path )
{
  if ( mOutputPath == path )
    return;

  mOutputPath = path;
  emit outputPathChanged();
}

void AudioRecorderController::setDuration( qint64 duration )
{
  if ( mDuration == duration )
    return;

  mDuration = duration;
  emit durationChanged();
}
