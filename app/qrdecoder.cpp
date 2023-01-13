#include "qrdecoder.h"

#include <QImage>
#include <QtMultimedia/qvideoframe.h>
#include <QOpenGLContext>
#include <QOpenGLFunctions>
#include <QOffscreenSurface>
#include <iostream>

#include <ZXing/ReadBarcode.h>

/*
 * zxing-cpp interface
 */
namespace ZXing
{
  namespace Qt
  {
    using ZXing::DecodeHints;
    using ZXing::BarcodeFormat;
    using ZXing::BarcodeFormats;
    using ZXing::Binarizer;

    template <typename T, typename _ = decltype( ToString( T() ) )>
    QDebug operator << ( QDebug dbg, const T &v )
    {
      return dbg.noquote() << QString::fromStdString( ToString( v ) );
    }

    // Result of the decode operation
    class Result : private ZXing::Result
    {
      public:
        explicit Result( ZXing::Result &&r ) : ZXing::Result( std::move( r ) ) { }

        using ZXing::Result::format;
        using ZXing::Result::isValid;
        using ZXing::Result::status;

        // Return result of qt code decoding as a human-readable text
        inline QString text() const { return QString::fromWCharArray( ZXing::Result::text().c_str() ); }
    };

    // Call ZXing::ReadBarcode() method and get result as a text
    Result ReadBarcode( const QImage &img, const DecodeHints &hints = {} )
    {
      auto ImgFmtFromQImg = []( const QImage & img )
      {
        switch ( img.format() )
        {
          case QImage::Format_ARGB32:
          case QImage::Format_RGB32:
#if Q_BYTE_ORDER == Q_LITTLE_ENDIAN
            return ImageFormat::BGRX;
#else
            return ImageFormat::XRGB;
#endif
          case QImage::Format_RGB888: return ImageFormat::RGB;
          case QImage::Format_RGBX8888:
          case QImage::Format_RGBA8888: return ImageFormat::RGBX;
          case QImage::Format_Grayscale8: return ImageFormat::Lum;
          default: return ImageFormat::None;
        }
      };

      auto exec = [&]( const QImage & img )
      {
        return Result( ZXing::ReadBarcode( { img.bits(), img.width(), img.height(), ImgFmtFromQImg( img ) }, hints ) );
      };
      return ImgFmtFromQImg( img ) == ImageFormat::None ? exec( img.convertToFormat( QImage::Format_RGBX8888 ) ) : exec( img );
    }
  } // Qt namespace
} // ZXing namespace

using namespace ZXing::Qt;

std::ostream &operator << ( std::ostream &os, const std::vector< ZXing::ResultPoint > &points )
{
  for ( const auto &p : points )
  {
    os << int( p.x() + .5f ) << "x" << int( p.y() + .5f ) << " ";
  }

  return os;
}

QRDecoder::QRDecoder( QObject *parent )
  : QObject( parent )
{
}

QString QRDecoder::processImage( const QImage capturedImage )
{
  if ( mIsDecoding )
  {
    return QString();
  }

  setIsDecoding( true );

  const auto hints = DecodeHints()
                     .setFormats( BarcodeFormat::QRCode | BarcodeFormat::DataMatrix | BarcodeFormat::Codabar |
                                  BarcodeFormat::Code39 | BarcodeFormat::Code93 | BarcodeFormat::Code128 |
                                  BarcodeFormat::EAN8 | BarcodeFormat::EAN13 )
                     .setTryHarder( true );

  const auto result = ReadBarcode( capturedImage, hints );

  setIsDecoding( false );

  if ( result.isValid() )
  {
    return result.text();
  }

  return QString();
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
