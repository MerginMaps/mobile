#ifndef QRDECODER_H
#define QRDECODER_H

#include <QObject>
#include <QVideoFrame>

#define DEFAULT_RES_W 1080
#define DEFAULT_RES_H 1920

/*
 * \brief Class used to convert video frame into image and scan QR code from it.
 */
class QRDecoder : public QObject
{
    Q_OBJECT

  public:
    explicit QRDecoder( QObject *parent = nullptr );

    bool isDecoding() const;

    QString capturedString() const;

    // Returns image from video frame
    static QImage videoFrameToImage( const QVideoFrame &videoFrame, const QRect &captureRect );

    // Set camera resolution
    void setResolution( const int &width, const int &height );

  public slots:
    // read qr code from the image
    void processImage( const QImage capturedImage );

  signals:
    void isDecodingChanged( bool isDecoding );
    void capturedStringChanged( const QString &capturedString );

  private:
    void setCapturedString( const QString &capturedString );
    void setIsDecoding( bool isDecoding );

    bool mIsDecoding = false;
    QString mCapturedString = "";

};

#endif // QRDECODER_H
