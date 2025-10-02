/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef QRCODEDECODER_H
#define QRCODEDECODER_H

#include <QObject>
#include <QVideoSink>
#include <QPointer>
#include <QVideoFrame>
#include <QFutureWatcher>

/**
 * Converts frames from videoSink (from QML) to images and decodes QR codes from them
 * It ignores most of the frames and process only few of them per second.
 * Processing of a single frame is in new thread.
 */
class QrCodeDecoder : public QObject
{
    Q_OBJECT

    Q_PROPERTY( QVideoSink *videoSink READ videoSink WRITE setVideoSink NOTIFY videoSinkChanged )

  public:

    explicit QrCodeDecoder( QObject *parent = nullptr );
    ~QrCodeDecoder() override;

    QVideoSink *videoSink() const;
    void setVideoSink( QVideoSink *videoSink );

  signals:
    void codeScanned( const QString &codeData );
    void videoSinkChanged();

  private slots:
    void onFutureFinished();
    void ignoreTimeout();
    void processFrame( const QVideoFrame &frame );

  private:
    QString processImage( QImage capturedImage );

    QPointer<QVideoSink> mVideoSink;
    QFutureWatcher<QString> mQrCodeWatcher;

    bool mIsDecoding = false;
    bool mIgnoreFrames = false;

    const int IGNORE_TIMER_INTERVAL = 1000; // in ms
};

#endif // QRCODEDECODER_H
