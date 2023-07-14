/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef APPSTATE_H
#define APPSTATE_H

#include <qglobal.h>
#include <QObject>

class AppState : public QObject
{
    Q_OBJECT

    Q_PROPERTY( AppState::State state READ state WRITE setState NOTIFY stateChanged )

  public:
    explicit AppState( QObject *parent = nullptr );

    enum State
    {
      Active = 0, // app is opened, visible and focused
      Inactive, // app is opened, but not focused
      Suspended // app is minimised
    };
    Q_ENUM( State );

    AppState::State state() const;
    void setState( const AppState::State &newState );

  signals:
    void stateChanged( AppState::State state );

  private:
    AppState::State mState = Active;
};

#endif // APPSTATE_H
