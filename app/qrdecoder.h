#ifndef QR_DECODER_H
#define QR_DECODER_H

#include <QObject>
#include <QVideoFrame>
#include <QOpenGLContext>

/*!
 * \brief Class used to convert video frame into image and scan QR code from it.
 */

class QRDecoder : public QObject
{
    Q_OBJECT
    Q_PROPERTY( bool isDecoding READ isDecoding WRITE setIsDecoding NOTIFY isDecodingChanged )

  public:
    explicit QRDecoder( QObject *parent = nullptr );
    QString captured() const;
    bool isDecoding() const;
    QVideoFrame videoFrame() const;

    void setCtx( QOpenGLContext *ctx );
    void setVideoFrame( const QVideoFrame &videoFrame );

    static QImage videoFrameToImage( const QVideoFrame &videoFrame );

  public slots:
    void process( const QImage capturedImage );

  signals:
    void capturedText( QString capturedText );
    void isDecodingChanged( bool isDecoding );

  private:
    QOpenGLContext *_ctx;
    bool _isDecoding = false;
    QVideoFrame _videoFrame;

    void setIsDecoding( bool isDecoding );
};

#endif // QR_DECODER_H
