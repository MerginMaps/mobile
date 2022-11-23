#include "codescanner.h"

CodeScanner::CodeScanner( QObject *parent )
  : QVideoSink( parent )
  , mCamera( nullptr )
{
  connect( &mDecoder, &QRDecoder::capturedStringChanged, this, &CodeScanner::setCapturedString );
  connect( this, &QVideoSink::videoFrameChanged, this, &CodeScanner::processFrame );

  mWorker = new QRWorker( this );
  mWorker->moveToThread( &mWorkThread );
  connect( &mWorkThread, &QThread::finished, mWorker, &QObject::deleteLater );
  connect( this, &CodeScanner::process, mWorker, &QRWorker::process );
  mWorkThread.start();

  initCamera();
}

CodeScanner::~CodeScanner()
{
  mWorkThread.quit();
  mWorkThread.wait();
  stopCamera();
}

void CodeScanner::initCamera()
{
  mCamera = new QCamera( this );
  const auto settings = mCamera->cameraDevice().videoFormats();

#ifdef Q_OS_ANDROID
  int i = mCamera->cameraDevice().videoFormats().size() - 1;
#else
  int i = 0;
#endif

  const auto s = settings.at( i );

  int w = settings.at( i ).resolution().width();
  int h = settings.at( i ).resolution().height();
  mDecoder.setResolution( w, h );

  mCamera->setFocusMode( QCamera::FocusModeAuto );
  mCamera->setExposureMode( QCamera::ExposureBarcode );
  mCamera->setCameraFormat( s );

  mCaptureSession.setCamera( mCamera );
  mCaptureSession.setVideoSink( this );

  mCamera->start();
}

void CodeScanner::stopCamera()
{
  mCamera->stop();
  disconnect( mCamera, nullptr, nullptr, nullptr );
  mCamera->setParent( nullptr );
  delete mCamera;
  mCamera = nullptr;
}

void CodeScanner::processFrame( const QVideoFrame &frame )
{
  qDebug() << "CodeScanner::processFrame";
  if ( mProcessing )
  {
    emit process( mDecoder.videoFrameToImage( frame, captureRect().toRect() ) );

    if ( mVideoSink )
    {
      mVideoSink->setVideoFrame( frame );
    }
  }
  pauseProcessing();
}

void CodeScanner::processImage( QRDecoder *decoder, const QImage &image )
{
  qDebug() << "CodeScanner::processImage";
  decoder->processImage( image );
  continueProcessing();
}

void CodeScanner::setProcessing( bool processing )
{
  mProcessing = processing;
}

void CodeScanner::pauseProcessing()
{
  disconnect( this, &QVideoSink::videoFrameChanged, this, &CodeScanner::processFrame );
}

void CodeScanner::continueProcessing()
{
  connect( this, &QVideoSink::videoFrameChanged, this, &CodeScanner::processFrame );
}

QRDecoder *CodeScanner::decoder()
{
  return &mDecoder;
}

QString CodeScanner::capturedString() const
{
  return mCapturedString;
}

void CodeScanner::setCapturedString( const QString &capturedString )
{
  qDebug() << "CodeScanner::setCapturedString" << capturedString;
  if ( mCapturedString == capturedString )
  {
    return;
  }

  mCapturedString = capturedString;
  emit capturedStringChanged( mCapturedString );
}

QRectF CodeScanner::captureRect() const
{
  return mCaptureRect;
}

void CodeScanner::setCaptureRect( const QRectF &captureRect )
{
  if ( mCaptureRect == captureRect )
  {
    return;
  }

  mCaptureRect = captureRect;
  emit captureRectChanged( mCaptureRect );
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

  mVideoSink = videoSink;
  emit videoSinkChanged();
}


QRWorker::QRWorker( CodeScanner *scanner )
  : mScanner( scanner )
{
}

void QRWorker::process( const QImage &image )
{
  mScanner->processImage( mScanner->decoder(), image );
}
