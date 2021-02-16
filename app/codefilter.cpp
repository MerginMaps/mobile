#include "codefilter.h"
#include <QVideoFilterRunnable>
#include <QFuture>

#include <QtMultimedia/qvideoframe.h>
#include <QImage>
#include <QOpenGLContext>
#include <QOpenGLFunctions>
#include <QDebug>

void processImage( QRDecoder *decoder, const QImage &image )
{
  decoder->process( image );
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
  mDecoder = new QRDecoder;

  QObject::connect( mDecoder, &QRDecoder::capturedChanged, this, &CodeFilter::setCapturedData );
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

QRDecoder *CodeFilter::decoder() const
{
  return mDecoder;
}

QFuture<void> CodeFilter::futureThread() const
{
  return mFutureThread;
}
