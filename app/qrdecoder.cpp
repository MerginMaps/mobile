#include "qrdecoder.h"

#include <QDebug>
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
namespace ZXing {
  namespace Qt {
    using ZXing::DecodeHints;
    using ZXing::BarcodeFormat;
    using ZXing::BarcodeFormats;
    using ZXing::Binarizer;

    template <typename T, typename _ = decltype(ToString(T()))>
    QDebug operator << (QDebug dbg, const T& v)
    {
      return dbg.noquote() << QString::fromStdString(ToString(v));
    }

    // Result of the decode operation
    class Result : private ZXing::Result
    {
      public:
        explicit Result(ZXing::Result&& r) : ZXing::Result(std::move(r)){ }

        using ZXing::Result::format;
        using ZXing::Result::isValid;
        using ZXing::Result::status;

        inline QString text() const { return QString::fromWCharArray( ZXing::Result::text().c_str() ); }
    };

    Result ReadBarcode( const QImage& img, const DecodeHints& hints = {} )
    {
      auto ImgFmtFromQImg = []( const QImage& img ) {
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

      auto exec = [&]( const QImage& img ){
        return Result( ZXing::ReadBarcode( { img.bits(), img.width(), img.height(), ImgFmtFromQImg( img ) }, hints ) );
      };
      return ImgFmtFromQImg(img) == ImageFormat::None ? exec( img.convertToFormat( QImage::Format_RGBX8888 ) ) : exec( img );
    }
  } // Qt namespace
} // ZXing namespace

using namespace ZXing::Qt;

std::ostream& operator << ( std::ostream& os, const std::vector< ZXing::ResultPoint >& points )
{
  for ( const auto& p : points )
  {
    os << int( p.x() + .5f ) << "x" << int( p.y() + .5f ) << " ";
  }

  return os;
}

static int mResolutionWidth = DEFAULT_RES_W;
static int mResolutionHeight = DEFAULT_RES_H;

QRDecoder::QRDecoder( QObject *parent )
  : QObject(parent)
{
}

void QRDecoder::processImage( const QImage capturedImage )
{
  setIsDecoding( true );

  const auto hints = DecodeHints()
                     .setFormats( BarcodeFormat::QRCode | BarcodeFormat::DataMatrix | BarcodeFormat::Codabar |
                                  BarcodeFormat::Code39 | BarcodeFormat::Code93 | BarcodeFormat::Code128 |
                                  BarcodeFormat::EAN8 | BarcodeFormat::EAN13 )
                     .setTryHarder( true );

  const auto result = ReadBarcode( capturedImage, hints );

  if ( result.isValid() )
  {
    setCapturedString( result.text() );
  }

  setIsDecoding( false );
}

QImage QRDecoder::videoFrameToImage( const QVideoFrame &videoFrame, const QRect &captureRect )
{
  auto handleType = videoFrame.handleType();

  if (handleType == QVideoFrame::NoHandle)
  {
    QImage image = videoFrame.toImage();

    if ( image.isNull() )
    {
      return QImage();
    }

    if ( image.format() != QImage::Format_ARGB32 )
    {
      image = image.convertToFormat( QImage::Format_ARGB32 );
    }

    // QML videooutput has no mapNormalizedRectToItem method
#ifdef Q_OS_ANDROID
    return image.copy( mResolutionHeight / 4, mResolutionWidth / 4, mResolutionHeight / 2, mResolutionWidth / 2 );
#else
    return image.copy( captureRect );
#endif
  }

  return QImage();
}

QString QRDecoder::capturedString() const
{
  return mCapturedString;
}

void QRDecoder::setCapturedString( const QString &capturedString )
{
  if ( mCapturedString == capturedString )
  {
    return;
  }

  mCapturedString = capturedString;
  emit capturedStringChanged( mCapturedString );
}

bool QRDecoder::isDecoding() const
{
  return mIsDecoding;
}

void QRDecoder::setIsDecoding( bool isDecoding )
{
  if ( mIsDecoding == isDecoding)
  {
    return;
  }

  mIsDecoding = isDecoding;
  emit isDecodingChanged( mIsDecoding );
}

void QRDecoder::setResolution(const int &width, const int &height)
{
  mResolutionWidth = width;
  mResolutionHeight = height;
}
