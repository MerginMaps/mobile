/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include "abstractpositionprovider.h"

AbstractPositionProvider::AbstractPositionProvider( const QString &id, const QString &type, const QString &name, PositionTransformer &positionTransformer, QObject *object )
  : QObject( object )
  , mProviderId( id )
  , mProviderType( type )
  , mProviderName( name )
  , mPositionTransformer( &positionTransformer )
{
}

void AbstractPositionProvider::setPosition( QgsPoint )
{
}

QString AbstractPositionProvider::name() const
{
  return mProviderName;
}

QString AbstractPositionProvider::stateMessage() const
{
  return mStateMessage;
}

AbstractPositionProvider::State AbstractPositionProvider::state() const
{
  return mState;
}

QString AbstractPositionProvider::id() const
{
  return mProviderId;
}

QString AbstractPositionProvider::type() const
{
  return mProviderType;
}

void AbstractPositionProvider::setState( const QString &message )
{
  setState( message, mState );
}

void AbstractPositionProvider::setState( const QString &message, AbstractPositionProvider::State state )
{
  if ( mStateMessage != message )
  {
    mStateMessage = message;
    emit stateMessageChanged( mStateMessage );
  }

  if ( mState != state )
  {
    mState = state;
    emit stateChanged( mState );
  }
}
