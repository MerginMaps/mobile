/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef CODEFILTER_H
#define CODEFILTER_H

#include <QObject>
#include <QAbstractVideoFilter>
#include <QtConcurrent/QtConcurrent>

#include "qrdecoder.h"

/*!
 * Used to process a video output from QML Camera by using QRDecoder.
 * A part of the filter is QVideoFilterRunnable that is resposible for actual processing. The result of proceseed image is parsed QR Code content.
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
    std::shared_ptr<QRDecoder> decoder() const;
    QFuture<void> futureThread() const;
    /**
     * Factory function to create a new instance of a QVideoFilterRunnable subclass corresponding to this filter.
     * This function is called on the thread on which the Qt Quick scene graph performs rendering, with the OpenGL context bound.
     * Ownership of the returned instance is transferred: the returned instance will live on the render thread and will be destroyed
     * automatically when necessary.
     * @return a new QVideoFilterRunnable instance associated with CodeFilter instance.
     */
    QVideoFilterRunnable *createFilterRunnable() override;

  signals:
    void capturedDataChanged();
    void isDecodingChanged( bool isDecoding );

  private slots:
    void setCapturedData( const QString &capturedData );

  private:
    QString mCapturedData;
    bool mIsDecoding;
    std::shared_ptr<QRDecoder> mDecoder = nullptr;
    QFuture<void> mFutureThread;
};

#endif // CODEFILTER_H
