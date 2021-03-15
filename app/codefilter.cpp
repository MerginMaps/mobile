/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include "codefilter.h"
#include <QVideoFilterRunnable>
#include <QFuture>

#include <QtMultimedia/qvideoframe.h>
#include <QImage>
#include <QOpenGLContext>
#include <QOpenGLFunctions>
#include <QDebug>

void processImage( std::shared_ptr<QRDecoder> &decoder, const QImage &image )
{
  if ( decoder != nullptr )
  {
    decoder->process( image );
  }
};

class QRRunnable : public QVideoFilterRunnable
{

  public:

    QRRunnable( CodeFilter *filter )
    {
      mFilter = filter;
    }

    QVideoFrame run( QVideoFrame *input,
                     const QVideoSurfaceFormat &surfaceFormat,
                     QVideoFilterRunnable::RunFlags flags ) override
    {
      Q_UNUSED( surfaceFormat );
      Q_UNUSED( flags );

      if ( mFilter == nullptr )
      {
        return *input;
      }

      if ( mFilter->decoder()->isDecoding() )
      {
        return *input;
      }

      if ( mFilter->futureThread().isRunning() )
      {
        return *input;
      }

      mFilter->decoder()->setVideoFrame( *input );

      QImage captured = QRDecoder::videoFrameToImage( *input );
      mFilter->futureThread() = QtConcurrent::run( processImage, mFilter->decoder(), captured );

      return *input;
    }

  private:
    CodeFilter *mFilter;
};

CodeFilter::CodeFilter()
{
  mDecoder = std::shared_ptr<QRDecoder>( new QRDecoder );

  QObject::connect( mDecoder.get(), &QRDecoder::capturedChanged, this, &CodeFilter::setCapturedData );
}

QVideoFilterRunnable *CodeFilter::createFilterRunnable()
{
  return new QRRunnable( this );
}

QString CodeFilter::capturedData()
{
  return mCapturedData;
}

void CodeFilter::setCapturedData( const QString &newValue )
{
  mCapturedData = newValue;
  emit capturedDataChanged();
}

bool CodeFilter::isDecoding() const
{
  return mIsDecoding;
}

std::shared_ptr<QRDecoder> CodeFilter::decoder() const
{
  return mDecoder;
}

QFuture<void> CodeFilter::futureThread() const
{
  return mFutureThread;
}
