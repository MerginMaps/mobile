/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include "notificationmodel.h"

Notification::Notification(uint id, const QString &message, uint interval, MessageType type = Information)
{
  mId = id;
  mMessage = message;
  mInterval = interval;
  mType = type;
}

NotificationModel::NotificationModel(QObject *parent) : QAbstractListModel{parent}
{
  // Initial data
  _notifications << Notification{ nextId(), "Ahoj", 10 };
  _notifications << Notification{ nextId(), "Hello all", 5 };
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
  return _notifications.size();
}

QVariant NotificationModel::data(const QModelIndex &index, int role) const
{
  if (!hasIndex(index.row(), index.column(), index.parent()))
    return {};

  Notification notification = _notifications.at(index.row());
  if (role == IdRole) return notification.id();
  if (role == MessageRole) return notification.message();
  if (role == TypeRole) return notification.type();

  return {};
}

// remove item by message
void NotificationModel::remove(uint id)
{
  for(int i=0; i<_notifications.count(); i++) {
    if(_notifications[i].id() == id) {
      beginRemoveRows(QModelIndex(), i, i);
      _notifications.removeAt(i);
      endRemoveRows();

      emit dataChanged(createIndex(0, 0), createIndex(rowCount(), 0)); // refresh whole model
      emit rowCountChanged();
      return;
    }
  }
}

// add new unique message with interval
void NotificationModel::add(const QString &message, uint interval, Notification::MessageType type = Notification::Information)
{
  for(Notification &notification : _notifications) {
    if(notification.message() == message)
      return;
  }

  beginInsertRows(QModelIndex(), rowCount(), rowCount());
  _notifications << Notification{ nextId(), message, interval, type };
  endInsertRows();

  emit rowCountChanged();
}


