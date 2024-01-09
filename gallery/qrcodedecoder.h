#ifndef QRCODEDECODER_H
#define QRCODEDECODER_H

#include <QObject>
#include <QPointer>

class QrCodeDecoder : public QObject
{
    Q_OBJECT

    Q_PROPERTY( QObject *videoSink READ videoSink WRITE setVideoSink NOTIFY videoSinkChanged );

  public:

    explicit QrCodeDecoder( QObject *parent = nullptr ) {};

    QObject *videoSink() {};
    void setVideoSink( QObject *videoSink ) {};

  signals:
    void codeScanned( const QString &codeData );
    void videoSinkChanged();

  private:
    QPointer<QObject> mVideoSink;
};

#endif // QRCODEDECODER_H
