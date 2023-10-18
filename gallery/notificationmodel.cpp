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

Notification::Notification(uint id, const QString &message, uint interval, NotificationType::MessageType type = NotificationType::Information)
{
  mId = id;
  mMessage = message;
  mInterval = interval;
  mType = type;
}

NotificationModel::NotificationModel(QObject *parent) : QAbstractListModel{parent}
{
  qmlRegisterUncreatableType<NotificationType>("notificationType", 1, 0, "NotificationType", "Not creatable as it is an enum type");

  mTimer = new QTimer(this);
  connect(mTimer, &QTimer::timeout, this, &NotificationModel::timerFired);
  mTimer->start(1000);

  // Initial data
  mNotifications << Notification{ nextId(), "Ahoj", 10, NotificationType::Information };
  mNotifications << Notification{ nextId(), "Hello all", 5, NotificationType::Information };
}

NotificationModel::~NotificationModel()
{
  mTimer->stop();
  delete mTimer;
}

QHash<int, QByteArray> NotificationModel::roleNames() const
{
  return {
      { IdRole, "id" },
      { MessageRole, "message" },
      { TypeRole, "type" }
  };
}

int NotificationModel::rowCount(const QModelIndex &parent) const
{
  Q_UNUSED(parent)
  return mNotifications.size();
}

QVariant NotificationModel::data(const QModelIndex &index, int role) const
{
  if (!hasIndex(index.row(), index.column(), index.parent()))
    return {};

  Notification notification = mNotifications.at(index.row());
  if (role == IdRole) return notification.id();
  if (role == MessageRole) return notification.message();
  if (role == TypeRole) return notification.type();

  return {};
}

// remove item by message
void NotificationModel::remove(uint id)
{
  for(int i=0; i<mNotifications.count(); i++) {
    if(mNotifications[i].id() == id) {
      beginRemoveRows(QModelIndex(), i, i);
      mNotifications.removeAt(i);
      endRemoveRows();

      emit dataChanged(createIndex(0, 0), createIndex(rowCount(), 0)); // refresh whole model
      emit rowCountChanged();
      return;
    }
  }
}

// add new unique message with interval
void NotificationModel::add(const QString &message, uint interval, NotificationType::MessageType type = NotificationType::Information)
{
  for(Notification &notification : mNotifications) {
    if(notification.message() == message)
      return;
  }

  beginInsertRows(QModelIndex(), rowCount(), rowCount());
  mNotifications << Notification{ nextId(), message, interval, type };
  endInsertRows();

  emit rowCountChanged();
}

// check for auto removing notification
void NotificationModel::timerFired()
{
  for(Notification &notification : mNotifications) {
    if(notification.canBeRemoved())
      remove(notification.id());
  }
}


