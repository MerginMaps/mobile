/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include "notificationmodel.h"
#include <QQmlEngine>

Notification::Notification( uint id, const QString &message, uint interval, NotificationType::MessageType type = NotificationType::Information, NotificationType::IconType icon = NotificationType::NoneIcon, NotificationType::ActionType action = NotificationType::NoAction )
{
  mId = id;
  mMessage = message;
  mInterval = interval;
  mType = type;
  mIcon = icon;
  mAction = action;
}

NotificationModel::NotificationModel( QObject *parent ) : QAbstractListModel{parent}
{
  mTimer = new QTimer( this );
  connect( mTimer, &QTimer::timeout, this, &NotificationModel::timerFired );
  mTimer->start( 1000 );
}

NotificationModel::~NotificationModel()
{
  mTimer->stop();
  delete mTimer;
}

QHash<int, QByteArray> NotificationModel::roleNames() const
{
  return
  {
    { IdRole, "id" },
    { MessageRole, "message" },
    { TypeRole, "type" },
    { IconRole, "icon" }
  };
}

int NotificationModel::rowCount( const QModelIndex &parent ) const
{
  Q_UNUSED( parent )
  return mNotifications.size();
}

QVariant NotificationModel::data( const QModelIndex &index, int role ) const
{
  if ( !hasIndex( index.row(), index.column(), index.parent() ) )
    return {};

  Notification notification = mNotifications.at( index.row() );
  if ( role == IdRole ) return notification.id();
  if ( role == MessageRole ) return notification.message();
  if ( role == TypeRole ) return notification.type();
  if ( role == IconRole ) return notification.icon();

  return {};
}

// remove item by message
void NotificationModel::remove( uint id )
{
  for ( int i = 0; i < mNotifications.count(); i++ )
  {
    if ( mNotifications[i].id() == id )
    {
      beginRemoveRows( QModelIndex(), i, i );
      mNotifications.removeAt( i );
      endRemoveRows();

      emit dataChanged( createIndex( 0, 0 ), createIndex( rowCount(), 0 ) ); // refresh whole model
      emit rowCountChanged();
      return;
    }
  }
}

// add new unique message with interval
void NotificationModel::add( const QString &message, uint interval, NotificationType::MessageType type = NotificationType::Information, NotificationType::IconType icon = NotificationType::NoneIcon, NotificationType::ActionType action = NotificationType::ActionType::NoAction )
{
  for ( Notification &notification : mNotifications )
  {
    if ( notification.message() == message )
      return;
  }
  beginInsertRows( QModelIndex(), rowCount(), rowCount() );
  mNotifications << Notification{ nextId(), message, interval, type, icon, action };
  endInsertRows();

  emit rowCountChanged();
}

void NotificationModel::addSuccess( const QString &message, NotificationType::ActionType action )
{
  add( message, NotificationModel::DEFAULT_NOTIFICATION_EXPIRATION_SECS, NotificationType::Success, NotificationType::CheckIcon, action );
}

void NotificationModel::addError( const QString &message, NotificationType::ActionType action )
{
  add( message, NotificationModel::DEFAULT_NOTIFICATION_EXPIRATION_SECS, NotificationType::Error, NotificationType::ExclamationIcon, action );
}

void NotificationModel::addInfo( const QString &message, NotificationType::ActionType action )
{
  add( message, NotificationModel::DEFAULT_NOTIFICATION_EXPIRATION_SECS, NotificationType::Information, NotificationType::WaitingIcon, action );
}

void NotificationModel::addWarning( const QString &message, NotificationType::ActionType action )
{
  add( message, NotificationModel::DEFAULT_NOTIFICATION_EXPIRATION_SECS, NotificationType::Warning, NotificationType::WaitingIcon, action );
}

// check for auto removing notification
void NotificationModel::timerFired()
{
  for ( Notification &notification : mNotifications )
  {
    if ( notification.isRemovableAfterDecrement() )
      remove( notification.id() );
  }
}

// do action, added when Notification was created
void NotificationModel::doAction(uint id)
{
  for ( int i = 0; i < mNotifications.count(); i++ )
  {
    if ( mNotifications[i].id() == id &&
        mNotifications[i].action() == NotificationType::ActionType::ShowProjectIssuesAction )
    {
      emit showProjectIssuesActionClicked();
    }
  }
}
