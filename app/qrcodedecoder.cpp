/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include "qrcodedecoder.h"

#include <QTimer>
#include <QtConcurrent>
#include <ZXing/ReadBarcode.h>

QrCodeDecoder::QrCodeDecoder( QObject *parent )
  : QObject( parent )
{
  connect( &mQrCodeWatcher, &QFutureWatcher<QString>::finished, this, &QrCodeDecoder::onFutureFinished );
}

QrCodeDecoder::~QrCodeDecoder() = default;

void QrCodeDecoder::processFrame( const QVideoFrame &frame )
{
  if ( mIsDecoding || mIgnoreFrames )
  {
    return;
  }
  mIgnoreFrames = true;
  mIsDecoding = true;

  mQrCodeWatcher.setFuture( QtConcurrent::run( &QrCodeDecoder::processImage, this, frame.toImage() ) );

  QTimer::singleShot( IGNORE_TIMER_INTERVAL, this, &QrCodeDecoder::ignoreTimeout );
}

void QrCodeDecoder::ignoreTimeout()
{
  mIgnoreFrames = false;
}

void QrCodeDecoder::onFutureFinished()
{
  QFutureWatcher<QString> *watcher = static_cast< QFutureWatcher<QString> *>( sender() );
  const QString codeData = watcher->future().result();

  if ( !codeData.isEmpty() )
  {
    emit codeScanned( codeData );
  }
  mIsDecoding = false;
}

QVideoSink *QrCodeDecoder::videoSink() const
{
  return mVideoSink.get();
}

void QrCodeDecoder::setVideoSink( QVideoSink *videoSink )
{
  if ( mVideoSink == videoSink )
  {
    return;
  }

  if ( mVideoSink )
  {
    disconnect( mVideoSink );
  }

  mVideoSink = videoSink;

  if ( mVideoSink )
  {
    connect( mVideoSink, &QVideoSink::videoFrameChanged, this, &QrCodeDecoder::processFrame );
  }

  emit videoSinkChanged();
}

static ZXing::ImageFormat formatFromQImage( const QImage &img )
{
  switch ( img.format() )
  {
    case QImage::Format_ARGB32:
    case QImage::Format_ARGB32_Premultiplied:
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
    case QImage::Format_RGBA8888_Premultiplied:
      return ZXing::ImageFormat::RGBX;
    case QImage::Format_Grayscale8:
      return ZXing::ImageFormat::Lum;
    default:
      return ZXing::ImageFormat::None;
  }
}


QString QrCodeDecoder::processImage( QImage capturedImage )
{
  QString resultText;

  if ( capturedImage.isNull() )
  {
    return QString();
  }

  QImage img = capturedImage;
  ZXing::ImageFormat imageFormat = formatFromQImage( img );
  if ( imageFormat != ZXing::ImageFormat::None )
  {
    img = capturedImage.convertToFormat( QImage::Format_RGB888 );
    imageFormat = formatFromQImage( img );
  }

  if ( imageFormat != ZXing::ImageFormat::None )
  {
    const ZXing::ImageFormat imageFormat = ZXing::ImageFormat::RGBX;
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

  return resultText;
}
