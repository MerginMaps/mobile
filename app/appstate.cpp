/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include "appstate.h"

AppState::AppState( QObject *parent ) : QObject( parent )
{

}

AppState::State AppState::state() const
{
  return mState;
}

void AppState::setState( const AppState::State &newState )
{
  if ( mState == newState )
    return;

  mState = newState;
  emit stateChanged( mState );
}
