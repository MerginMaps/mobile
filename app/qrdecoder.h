#ifndef QRDECODER_H
#define QRDECODER_H

#include "inputconfig.h"

#include <QObject>
#include <QImage>

/*
 * \brief Class used to convert video frame into image and scan QR code from it.
 */
class QRDecoder : public QObject
{
    Q_OBJECT

  public:
    explicit QRDecoder( QObject *parent = nullptr );

    bool isDecoding() const;

  public slots:
    // read qr code from the image
    QString processImage( const QImage capturedImage );

  signals:
    void isDecodingChanged( bool isDecoding );
    void codeDecoded( const QString &data );

  private:
    void setIsDecoding( bool isDecoding );

    bool mIsDecoding = false;
};

#endif // QRDECODER_H
