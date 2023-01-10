#include "codescanner.h"

#include <QTimer>

#include "imageutils.h"

CodeScanner::CodeScanner( QObject *parent )
  : QObject( parent )
{
  mWorker = new QRWorker( this );
  mWorker->moveToThread( &mWorkThread );

  connect( this, &CodeScanner::startProcessing, mWorker, &QRWorker::process );
  connect( mWorker, &QRWorker::codeScanned, this, &CodeScanner::codeScanned );
  connect( &mWorkThread, &QThread::finished, mWorker, &QObject::deleteLater );

  mWorkThread.start();
}

CodeScanner::~CodeScanner()
{
  mWorkThread.quit();
  mWorkThread.wait();
}

void CodeScanner::processFrame( const QVideoFrame &frame )
{
  if ( mIgnoreFrames )
  {
    return;
  }

  QImage image = ImageUtils::rescaledImage( frame );

  emit startProcessing( image );

  mIgnoreFrames = true;
  QTimer::singleShot( IGNORE_TIMER_INTERVAL, this, &CodeScanner::ignoreTimeout );
}

void CodeScanner::ignoreTimeout()
{
  mIgnoreFrames = false;
}

QVideoSink *CodeScanner::videoSink() const
{
  return mVideoSink.get();
}

void CodeScanner::setVideoSink( QVideoSink *videoSink )
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
    connect( mVideoSink, &QVideoSink::videoFrameChanged, this, &CodeScanner::processFrame );
  }

  emit videoSinkChanged();
}

QRWorker::QRWorker( QObject *parent ) : QObject( parent )
{
  connect( &mDecoder, &QRDecoder::codeDecoded, this, &QRWorker::codeScanned );
}

void QRWorker::process( const QImage &image )
{
  mDecoder.processImage( image );
}
