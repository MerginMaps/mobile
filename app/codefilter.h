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
    QRDecoder *decoder() const;
    QFuture<void> futureThread() const;
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
    QRDecoder *mDecoder;
    QFuture<void> mFutureThread;
};

#endif // CODEFILTER_H
