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

    enum IconType
    {
      NoneIcon,
      WaitingIcon,
      InfoIcon,
      CheckIcon,
      ExclamationIcon
    };
    Q_ENUM( IconType )

    enum ActionType
    {
      NoAction,
      ShowProjectIssuesAction,
      ShowSwitchWorkspaceAction,
      ShowSyncFailedDialog
    };
    Q_ENUM( ActionType )

  private:
    explicit NotificationType();
};

class Notification
{
    Q_GADGET

  public:
    Notification( uint id, const QString &message, uint interval, NotificationType::MessageType type = NotificationType::Information, NotificationType::IconType icon = NotificationType::NoneIcon, NotificationType::ActionType action = NotificationType::NoAction );
    uint id() const { return mId; }
    QString message() const { return mMessage; }
    NotificationType::MessageType type() const { return mType; }
    NotificationType::IconType icon() const { return mIcon; }
    NotificationType::ActionType action() const { return mAction; }
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
    NotificationType::ActionType mAction;
};

class NotificationModel : public QAbstractListModel
{
    Q_OBJECT

    Q_PROPERTY( int rowCount READ rowCount NOTIFY rowCountChanged );

  public:
    enum NotificationModelRoles
    {
      IdRole = Qt::UserRole + 1, MessageRole, TypeRole, IconRole
    };
    Q_ENUM( NotificationModelRoles )

    NotificationModel( QObject *parent = nullptr );
    ~NotificationModel();

    Q_INVOKABLE void addSuccess( const QString &message, NotificationType::ActionType action = NotificationType::ActionType::NoAction, uint interval = DEFAULT_NOTIFICATION_EXPIRATION_SECS );
    Q_INVOKABLE void addError( const QString &message, NotificationType::ActionType action = NotificationType::ActionType::NoAction, uint interval = DEFAULT_NOTIFICATION_EXPIRATION_SECS );
    Q_INVOKABLE void addInfo( const QString &message, NotificationType::ActionType action = NotificationType::ActionType::NoAction, uint interval = DEFAULT_NOTIFICATION_EXPIRATION_SECS );
    Q_INVOKABLE void addWarning( const QString &message, NotificationType::ActionType action = NotificationType::ActionType::NoAction, uint interval = DEFAULT_NOTIFICATION_EXPIRATION_SECS );
    Q_INVOKABLE void remove( uint id );
    Q_INVOKABLE void onNotificationClicked( uint id );

    QHash<int, QByteArray> roleNames() const override;
    int rowCount( const QModelIndex &parent = QModelIndex() ) const override;
    QVariant data( const QModelIndex &index, int role = Qt::DisplayRole ) const override;

  signals:
    void rowCountChanged();
    void showProjectIssuesActionClicked();
    void showSwitchWorkspaceActionClicked();
    void showSyncFailedDialogClicked();

  private:
    void add( const QString &message, uint interval, NotificationType::MessageType type = NotificationType::Information, NotificationType::IconType icon = NotificationType::NoneIcon, NotificationType::ActionType action = NotificationType::ActionType::NoAction );
    uint nextId() { static uint id = 0; return id++; }
    void timerFired();

    static constexpr uint DEFAULT_NOTIFICATION_EXPIRATION_SECS = 5;
    QList<Notification> mNotifications;
    QTimer *mTimer;
};

#endif // NOTIFICATIONMODEL_H
