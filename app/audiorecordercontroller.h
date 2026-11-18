/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef AUDIORECORDERCONTROLLER_H
#define AUDIORECORDERCONTROLLER_H

#include <QObject>
#include <QPointer>
#include <QUrl>
#include <QMediaRecorder>
#include <QTimer>
#include <QtQml/qqmlregistration.h>

#include <memory>

#include "attributecontroller.h"

class QAudioInput;
class QMediaCaptureSession;

/**
 * Records audio for attachment fields of a feature form.
 *
 * It lives in the form page, not in the field editor, because form editors are list delegates
 * that get destroyed when scrolled out of view, while the recording must keep running.
 * The field editor and the form header bind to its state, so a re-created editor picks it back up.
 *
 * The recording is written to a temporary file. When it is finished, the file is handed to the
 * AttributeController, which moves it to the field's target folder when the feature is saved
 * and removes it when the changes are rolled back.
 *
 * Only one recording runs at a time and only while the app is in the foreground.
 */
class AudioRecorderController : public QObject
{
    Q_OBJECT
    QML_ELEMENT

    Q_PROPERTY( AttributeController *attributeController READ attributeController WRITE setAttributeController NOTIFY attributeControllerChanged )
    Q_PROPERTY( bool isRecording READ isRecording NOTIFY isRecordingChanged )
    Q_PROPERTY( qint64 duration READ duration NOTIFY durationChanged ) // milliseconds
    Q_PROPERTY( QString activeFieldItemId READ activeFieldItemId NOTIFY activeFieldItemIdChanged )
    Q_PROPERTY( QString activeFieldTitle READ activeFieldTitle NOTIFY activeFieldItemIdChanged ) // title of the field being recorded, for messages
    Q_PROPERTY( QString outputPath READ outputPath NOTIFY outputPathChanged )
    Q_PROPERTY( qint64 maximumDuration READ maximumDuration CONSTANT )

  public:
    static constexpr qint64 MAXIMUM_DURATION_MS = 30 * 60 * 1000;
    static constexpr qint64 DURATION_WARNING_MS = 29 * 60 * 1000;

    explicit AudioRecorderController( QObject *parent = nullptr );
    ~AudioRecorderController() override;

    AttributeController *attributeController() const;
    void setAttributeController( AttributeController *controller );

    bool isRecording() const;
    qint64 duration() const;
    QString activeFieldItemId() const;
    QString activeFieldTitle() const;
    QString outputPath() const;
    qint64 maximumDuration() const;

    /**
     * Starts recording for the field, asks for the microphone permission first if needed.
     * Only one field records at a time: while another field records, emits recordingBusy() instead.
     * \param fileToReplace absolute path of the field's current file when re-recording, it is removed
     *        when the feature is saved, but only if the new recording is kept
     */
    Q_INVOKABLE void startRecording( const QString &fieldItemId, const QString &fieldTitle = QString(), const QString &fileToReplace = QString() );

    // stops the recording and keeps the file
    Q_INVOKABLE void stopRecording();

    // stops the recording and removes the file
    Q_INVOKABLE void discardRecording();

    // stops the recording, keeps the file and tells the user why with recordingInterrupted()
    Q_INVOKABLE void interruptRecording( const QString &message );

    /**
     * To be called before another activity is opened over the app (camera, gallery, file picker).
     * On Android that suspends the app, which would stop the recording. Returns true and emits recordingBusy()
     * if a recording is running there, the caller should not open the activity then.
     */
    Q_INVOKABLE bool blockExternalActivity();

    // copies an audio file chosen from the device to a temporary file and assigns it to the field
    Q_INVOKABLE bool importAudio( const QString &fieldItemId, const QUrl &source );

    // returns the file to play for the field: the temporary file of an unsaved recording, otherwise the given path
    Q_INVOKABLE QString playbackPath( const QString &fieldItemId, const QString &absolutePath ) const;

    // returns true if the device has an audio input
    Q_INVOKABLE static bool hasMicrophone();

    // returns true if the file has one of the supported audio suffixes
    Q_INVOKABLE static bool isSupportedAudioFile( const QString &path );

    // name filters for the file picker, e.g. "*.m4a"
    Q_INVOKABLE static QStringList audioNameFilters();

    //! folder for recordings and imported files until the feature is saved
    static QString temporaryAudioDir();

    //! removes files left in the temporary folder by a previous run of the app, to be called on start
    static void removeTemporaryFiles();

  signals:
    void attributeControllerChanged();
    void isRecordingChanged();
    void durationChanged();
    void activeFieldItemIdChanged();
    void outputPathChanged();

    void recordingStarted( const QString &fieldItemId );
    //! recording was requested for a field while another field records
    void recordingBusy( const QString &fieldItemId );

    // Each recording ends with exactly one of recordingFinished(), recordingDiscarded() or recordingFailed().
    // They are emitted after isRecording turned false, so the form can save or close from them.

    //! the file is kept and assigned to the field
    void recordingFinished( const QString &fieldItemId );
    void recordingDiscarded( const QString &fieldItemId );
    void recordingFailed( const QString &fieldItemId, const QString &message );
    //! the app stopped the recording (background, interruption, length limit), the file recorded so far is kept
    void recordingInterrupted( const QString &fieldItemId, const QString &message );
    void maximumDurationWarning();
    void microphonePermissionDenied();

  private slots:
    void onRecorderStateChanged( QMediaRecorder::RecorderState state );
    void onRecorderError( QMediaRecorder::Error error, const QString &errorString );
    void onRecorderDurationChanged( qint64 duration );
    void onApplicationStateChanged( Qt::ApplicationState state );
    void onWatchdogTimeout();

  private:
    // a recording that gets no sound for this long was interrupted (phone call, another app took the microphone)
    static constexpr int STALLED_TIMEOUT_MS = 8000;
    // the recorder should report it stopped long before this
    static constexpr int STOP_TIMEOUT_MS = 10000;

    enum class State
    {
      Idle,
      Recording,
      Stopping
    };

    void setupRecorder();
    void record( const QString &fieldItemId, const QString &fieldTitle, const QString &fileToReplace );
    void finishRecording();
    void setState( State state );
    void setActiveField( const QString &fieldItemId, const QString &fieldTitle );
    void setOutputPath( const QString &path );
    void setDuration( qint64 duration );

    QPointer<AttributeController> mAttributeController; // not owned

    // created on the first recording, so the audio device is not touched by forms without audio fields
    std::unique_ptr<QAudioInput> mAudioInput;
    std::unique_ptr<QMediaRecorder> mRecorder;
    std::unique_ptr<QMediaCaptureSession> mCaptureSession;

    State mState = State::Idle;
    QString mActiveFieldItemId;
    QString mActiveFieldTitle;
    QString mOutputPath;
    qint64 mDuration = 0;
    bool mDiscardRequested = false;
    bool mDurationWarningEmitted = false;
    QString mInterruptionMessage;
    // kept here and not in the field editor, which can be scrolled out of view while recording
    QString mFileToReplace;

    // recording waiting for the app to come back to the foreground after the permission was granted
    QString mPendingFieldItemId;
    QString mPendingFieldTitle;
    QString mPendingFileToReplace;
    bool mPermissionRequestPending = false;

    // stops a recording that got stuck: no sound while recording or no answer from the recorder while stopping
    QTimer mWatchdog;

    friend class TestAudioRecorder;
};

#endif // AUDIORECORDERCONTROLLER_H
