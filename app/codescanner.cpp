#include "codescanner.h"

#include <QTimer>

CodeScanner::CodeScanner( QObject *parent )
  : QObject( parent )
{
}

CodeScanner::~CodeScanner() = default;

void CodeScanner::processFrame( const QVideoFrame &frame )
{
  if ( mIgnoreFrames )
  {
    return;
  }

  QImage image = frame.toImage();

  QString response = mDecoder.processImage( image );

  if ( !response.isEmpty() )
  {
    emit codeScanned( response );
  }

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
