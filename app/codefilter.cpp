#include "codefilter.h"
#include <QVideoFilterRunnable>
#include <QFuture>

#include <QtMultimedia/qvideoframe.h>
#include <QImage>
#include <QOpenGLContext>
#include <QOpenGLFunctions>
#include <QDebug>

#ifdef MOBILE_OS
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
#endif

CodeFilter::CodeFilter()
{
#ifdef MOBILE_OS
  mDecoder = new QRDecoder;

  QObject::connect( mDecoder, &QRDecoder::capturedChanged, this, &CodeFilter::setCapturedData );
#endif
}

QVideoFilterRunnable *CodeFilter::createFilterRunnable()
{
#ifdef MOBILE_OS
  return new QRRunnable( this );
#else
  return nullptr;
#endif

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

#ifdef MOBILE_OS

QRDecoder *CodeFilter::decoder() const
{
  return mDecoder;
}

QFuture<void> CodeFilter::futureThread() const
{
  return mFutureThread;
}
#endif
