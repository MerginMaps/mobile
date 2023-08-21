#include "qrdecoder.h"

#include <QImage>
#include <ZXing/ReadBarcode.h>

static ZXing::ImageFormat formatFromQImage( const QImage &img )
{
  switch ( img.format() )
  {
    case QImage::Format_ARGB32:
    case QImage::Format_RGB32:
#if Q_BYTE_ORDER == Q_LITTLE_ENDIAN
      return ZXing::ImageFormat::BGRX;
#else
      return ImageFormat::XRGB;
#endif
    case QImage::Format_RGB888:
      return ZXing::ImageFormat::RGB;
    case QImage::Format_RGBX8888:
    case QImage::Format_RGBA8888:
      return ZXing::ImageFormat::RGBX;
    case QImage::Format_Grayscale8:
      return ZXing::ImageFormat::Lum;
    default:
      return ZXing::ImageFormat::None;
  }
}

QRDecoder::QRDecoder( QObject *parent )
  : QObject( parent )
{
}

QString QRDecoder::processImage( const QImage capturedImage )
{
  QString resultText;

  if ( mIsDecoding )
  {
    return QString();
  }

  setIsDecoding( true );
  ZXing::ImageFormat imageFormat = formatFromQImage( capturedImage );
  if ( imageFormat != ZXing::ImageFormat::None )
  {
    ZXing::ImageView imageView( capturedImage.bits(), capturedImage.width(), capturedImage.height(), imageFormat, static_cast<int>( capturedImage.bytesPerLine() ) );

    ZXing::DecodeHints hints = ZXing::DecodeHints()
                               .setFormats( ZXing::BarcodeFormat::Any )
                               .setTryRotate( true )
                               .setTryHarder( true );

    ZXing::Result result = ZXing::ReadBarcode( imageView, hints );
    if ( result.isValid() )
    {
      const std::string text = result.text();
      resultText = QString::fromStdString( text.c_str() );
    }
  }
  setIsDecoding( false );
  return resultText;
}

bool QRDecoder::isDecoding() const
{
  return mIsDecoding;
}

void QRDecoder::setIsDecoding( bool isDecoding )
{
  if ( mIsDecoding == isDecoding )
  {
    return;
  }

  mIsDecoding = isDecoding;
  emit isDecodingChanged( mIsDecoding );
}
