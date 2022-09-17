#ifndef CODESCANNER_H
#define CODESCANNER_H

#include <QObject>
#include <QCamera>
#include <QThread>
#include <QImageCapture>
#include <QMediaCaptureSession>
#include <QVideoSink>
#include <QPointer>

#include "qrdecoder.h"

class QRWorker;

/**
 * Process a video output from QML Camera by using QRDecoder
 */
class CodeScanner : public QVideoSink
{
    Q_OBJECT

    Q_PROPERTY( QVideoSink* videoSink READ videoSink WRITE setVideoSink NOTIFY videoSinkChanged )
    Q_PROPERTY( QRectF captureRect READ captureRect WRITE setCaptureRect NOTIFY captureRectChanged )

  public:
    explicit CodeScanner( QObject *parent = nullptr );
    ~CodeScanner() override;

    QRDecoder *decoder() ;

    QVideoSink *videoSink() const;
    void setVideoSink( QVideoSink *videoSink );
    QRectF captureRect() const;
    void setCaptureRect( const QRectF &captureRect );

    QString capturedString() const;

  public slots:
    void pauseProcessing();
    void continueProcessing();
    void setProcessing(bool processing);

    void processImage( QRDecoder *decoder, const QImage &image );

  signals:
    void cameraChanged();
    void videoSinkChanged();
    void captureRectChanged( const QRectF &captureRect );
    void capturedStringChanged( const QString &captured );

    void process(const QImage &image);

  private slots:
    void initCamera();
    void stopCamera();

  private:
    QRDecoder mDecoder;
    QCamera *mCamera = nullptr;
    QPointer<QVideoSink> mVideoSink;
    QRectF mCaptureRect;
    QString mCapturedString = "";
    QMediaCaptureSession mCaptureSession;
    QThread mWorkThread;
    QRWorker *mWorker;
    bool mProcessing = true;

    void setCapturedString( const QString &capturedString );
    void processFrame( const QVideoFrame &frame );
};

/**
 * Runs CodeScanner processImage() in a separate thread
 */
class QRWorker : public QObject
{
    Q_OBJECT

  public:
    QRWorker( CodeScanner *scanner );

  public slots:
    void process( const QImage &image );

  private:
    CodeScanner *mScanner;
};

#endif // CODESCANNER_H
