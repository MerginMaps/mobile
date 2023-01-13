#ifndef CODESCANNER_H
#define CODESCANNER_H

#include <QObject>
#include <QVideoSink>
#include <QPointer>
#include <QVideoFrame>

#include "inputconfig.h"

#include "qrdecoder.h"

class QRWorker;

/**
 * Converts frames from videoSink (from QML) to images and sends to QRDecoder.
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
    void startProcessing( const QVideoFrame &frame );

    void codeScanned( const QString &codeData );
    void videoSinkChanged();

  private:
    QPointer<QVideoSink> mVideoSink;

    QRDecoder mDecoder;
    bool mIgnoreFrames = false;

    const int IGNORE_TIMER_INTERVAL = 1000; // in ms
};

#endif // CODESCANNER_H
