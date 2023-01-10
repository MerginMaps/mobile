#ifndef CODESCANNER_H
#define CODESCANNER_H

#include <QObject>
#include <QThread>
#include <QVideoSink>
#include <QPointer>

#include "inputconfig.h"

#include "qrdecoder.h"

class QRWorker;

/**
 * Converts frames from videoSink (from QML) to images, rescales them and sends to QRDecoder.
 * It ignores most of the frames and process only few of them per second.
 */
class CodeScanner : public QObject
{
    Q_OBJECT

    Q_PROPERTY( QVideoSink *videoSink READ videoSink WRITE setVideoSink NOTIFY videoSinkChanged )

  public:

    explicit CodeScanner( QObject *parent = nullptr );
    ~CodeScanner() override;

    QVideoSink *videoSink() const;
    void setVideoSink( QVideoSink *videoSink );

  public slots:
    void processFrame( const QVideoFrame &frame );
    void ignoreTimeout();

  signals:
    void startProcessing( const QImage &image );

    void codeScanned( const QString &codeData );
    void videoSinkChanged();

  private:
    QPointer<QVideoSink> mVideoSink;

    QRWorker *mWorker = nullptr; // owned
    QThread mWorkThread;

    bool mIgnoreFrames = false;

    const int IGNORE_TIMER_INTERVAL = 500; // in ms
};

/**
 * Runs decoding in a separate thread
 */
class QRWorker : public QObject
{
    Q_OBJECT

  public:
    QRWorker( QObject *parent = nullptr );

  public slots:
    void process( const QImage &image );

  signals:
    void codeScanned( const QString &scannedValue );

  private:
    QRDecoder mDecoder;
};

#endif // CODESCANNER_H
