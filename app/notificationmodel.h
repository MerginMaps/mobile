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
#include <QTimer>

class NotificationType
{
    Q_GADGET
  public:

    enum MessageType
    {
      Information,
      Success,
      Warning,
      Error
    };
    Q_ENUM( MessageType )

    enum IconType // TODO: add more icons in the future
    {
      None,
      Waiting,
      Check,
      Exclamation
    };
    Q_ENUM( IconType )

  private:
    explicit NotificationType();
};

class Notification
{
    Q_GADGET

  public:
    Notification( uint id, const QString &message, uint interval, NotificationType::MessageType type, NotificationType::IconType icon );
    uint id() const { return mId; }
    QString message() const { return mMessage; }
    NotificationType::MessageType type() const { return mType; }
    NotificationType::IconType icon() const { return mIcon; }
    bool isRemovableAfterDecrement()
    {
      if ( mInterval > 0 )
      {
        --mInterval;
      }
      return ( mInterval == 0 );
    }

  private:
    uint mId;
    QString mMessage;
    uint mInterval; // [seconds]
    NotificationType::MessageType mType;
    NotificationType::IconType mIcon;
};

class NotificationModel : public QAbstractListModel
{
    Q_OBJECT

  public:
    enum MyRoles
    {
      IdRole = Qt::UserRole + 1, MessageRole, TypeRole, IconRole
    };
    Q_ENUM( MyRoles )

    NotificationModel( QObject *parent = nullptr );
    ~NotificationModel();

    QHash<int, QByteArray> roleNames() const override;
    int rowCount( const QModelIndex &parent = QModelIndex() ) const override;
    QVariant data( const QModelIndex &index, int role = Qt::DisplayRole ) const override;

    Q_PROPERTY( int rowCount READ rowCount NOTIFY rowCountChanged );
    Q_INVOKABLE void remove( uint id );

    //! Adds a new notification with message, interval (in seconds), type and icon
    Q_INVOKABLE void add( const QString &message, uint interval, NotificationType::MessageType type, NotificationType::IconType icon );

    //! Convenient methods to save typing of some parameters
    Q_INVOKABLE void addSuccess( const QString &message );
    Q_INVOKABLE void addError( const QString &message );
    Q_INVOKABLE void addInfo( const QString &message );
    Q_INVOKABLE void addWarning( const QString &message );

    uint DEFAULT_NOTIFICATION_EXPIRATION_SECS = 3;

  private:
    uint nextId() { static uint id = 0; return id++; }
    void timerFired();

  signals:
    void rowCountChanged();

  private:
    QList<Notification> mNotifications;
    QTimer *mTimer;
};

#endif // NOTIFICATIONMODEL_H
