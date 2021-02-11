#ifndef QR_DECODER_H
#define QR_DECODER_H

#include <QObject>
#ifdef MOBILE_OS
#include <QVideoFrame>
#endif
#include <QOpenGLContext>

/*!
 * \brief Class used to convert video frame into image and scan QR code from it.
 */

class QRDecoder : public QObject
{
    Q_OBJECT
#ifdef MOBILE_OS
    Q_PROPERTY( QString captured READ captured WRITE setCaptured NOTIFY capturedChanged )
    Q_PROPERTY( bool isDecoding READ isDecoding WRITE setIsDecoding NOTIFY isDecodingChanged )
#endif
  public:
    explicit QRDecoder( QObject *parent = nullptr );
#ifdef MOBILE_OS
    QString captured() const;
    bool isDecoding() const;
    QVideoFrame videoFrame() const;

    void setCtx( QOpenGLContext *ctx );
    void setVideoFrame( const QVideoFrame &videoFrame );

    static QImage videoFrameToImage( const QVideoFrame &videoFrame );

  public slots:
    void process( const QImage capturedImage );

  signals:
    void capturedChanged( QString captured );
    void isDecodingChanged( bool isDecoding );

  private:
    QString _captured = "";
    QOpenGLContext *_ctx;
    bool _isDecoding = false;
    QVideoFrame _videoFrame;

    void setCaptured( QString captured );
    void setIsDecoding( bool isDecoding );
#endif
};

#endif // QR_DECODER_H
