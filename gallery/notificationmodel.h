/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef NOTIFICATIONMODEL_H
#define NOTIFICATIONMODEL_H

#include "qqmlintegration.h"
#include <QAbstractListModel>

class Notification
{
  Q_GADGET

  public:
  enum MessageType {
    Information,
    Warning,
    Error
  };
  Q_ENUM(MessageType)

  Notification(uint id, const QString &message, uint interval, MessageType type);
  uint id() { return mId; }
  QString message() { return mMessage; }
  MessageType type() { return mType; }

  private:
  uint mId;
  QString mMessage;
  uint mInterval; // [seconds]
  MessageType mType = Information;
};

class NotificationModel : public QAbstractListModel
{
  Q_OBJECT

  public:
  enum MyRoles {
    IdRole = Qt::UserRole + 1, MessageRole, TypeRole
  };
  Q_ENUM(MyRoles)

  NotificationModel(QObject *parent = nullptr);

  QHash<int,QByteArray> roleNames() const override;
  int rowCount(const QModelIndex & parent = QModelIndex()) const override;
  QVariant data(const QModelIndex & index, int role = Qt::DisplayRole) const override;

  Q_PROPERTY(int rowCount READ rowCount NOTIFY rowCountChanged);
  Q_INVOKABLE void remove(uint id);
  Q_INVOKABLE void add(const QString &message, uint interval, Notification::MessageType type);

  private:
  uint nextId() { static uint id = 0; return id++; }

  signals:
  void rowCountChanged();

  private:
  QList<Notification> _notifications;
};

#endif // NOTIFICATIONMODEL_H
