#ifndef CODEFILTER_H
#define CODEFILTER_H

#include <QObject>
#include <QAbstractVideoFilter>
#include <QtConcurrent/QtConcurrent>

#include "qrdecoder.h"

/*!
 * \brief Filter used to capture QR code from camera
 */
class CodeFilter : public QAbstractVideoFilter
{
    Q_OBJECT
    Q_PROPERTY( QString capturedData READ capturedData NOTIFY capturedDataChanged )
    Q_PROPERTY( bool isDecoding READ isDecoding NOTIFY isDecodingChanged )
  public:
    CodeFilter();

    QString capturedData();
    bool isDecoding() const;
#ifdef MOBILE_OS
    QRDecoder *decoder() const;
    QFuture<void> futureThread() const;
#endif
    QVideoFilterRunnable *createFilterRunnable() override;


  signals:
    void capturedDataChanged();
    void isDecodingChanged( bool isDecoding );

  private slots:
    void setCapturedData( const QString &capturedData );

  private:
    QSharedPointer<QMutex> _mutexP;
    QString mCapturedData;
    bool mIsDecoding;
#ifdef MOBILE_OS
    QRDecoder *mDecoder;
    QFuture<void> mFutureThread;
#endif
};

#endif // CODEFILTER_H
