/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef TESTAUDIORECORDER_H
#define TESTAUDIORECORDER_H

#include <QObject>
#include <QString>

class TestAudioRecorder : public QObject
{
    Q_OBJECT

  private slots:
    void init(); // will be called before each testfunction is executed.
    void cleanup(); // will be called after every testfunction.

    void testEditorSelection();
    void testPendingAudioMovedOnSave();
    void testPendingAudioFollowsPathExpression();
    void testPendingAudioRemovedOnRollback();
    void testPendingAudioReplaced();
    void testPendingAudioRemovedWithValue();
    void testPendingAudioMoveFailure();
    void testPendingAudioKeptUntilCommit();
    void testFileRemovalOnlyOnSave();
    void testAudioNotRemembered();
    void testRecorderInitialState();
    void testRecorderBusy();
    void testRecorderApplicationState();
    void testRecorderFailedRecording();
    void testRecorderReplacesFile();
    void testRecorderWatchdog();
    void testPendingAudioOutsideProject();
    void testPendingAudioKeptOnGeometryEdit();
    void testTemporaryFilesRemoved();
    void testRecorderImport();

  private:
    //! copies the test_audio project to a temporary folder and opens it, returns the project folder
    QString openTestProject( const QString &name );
    //! copies the fixture recording to the temporary audio folder, as a finished recording would be
    QString createTemporaryRecording();

    QString mProjectDir;
};

#endif // TESTAUDIORECORDER_H
