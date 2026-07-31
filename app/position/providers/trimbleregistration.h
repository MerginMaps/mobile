/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef TRIMBLEREGISTRATION_H
#define TRIMBLEREGISTRATION_H

#include <QObject>
#include <QString>
#include <memory>

#ifdef ANDROID
class TmmResultReceiver; // defined in trimbleregistrationandroid.cpp
#endif

/**
 * Platform-agnostic async contract for registering with Trimble Mobile Manager.
 *
 * Call requestRegistration() once; listen for registered() or failed().
 * Concrete implementations are in trimbleregistrationandroid.cpp (Android)
 * and trimbleregistrationios.mm (iOS).
 *
 * TODO: move to native utils when refactoring native utils
 */
class TrimbleRegistration : public QObject
{
    Q_OBJECT

  public:
    explicit TrimbleRegistration( QObject *parent = nullptr );
    ~TrimbleRegistration() override = default;

    void requestRegistration( const QString &appId );

#ifdef Q_OS_IOS
  slots:
    void handleCallback( const QUrl &url );
#endif

  signals:
    void registered( int locationV2Port );
    void failed( const QString &reason );

  private:
#ifdef ANDROID
    std::unique_ptr<TmmResultReceiver> mResultReceiver;
#endif
};

#endif // TRIMBLEREGISTRATION_H
