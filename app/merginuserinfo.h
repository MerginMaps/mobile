/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/


#ifndef MERGINUSERINFO_H
#define MERGINUSERINFO_H

#include <QObject>
#include <QString>
#include <QJsonObject>

#include "merginsubscriptionstatus.h"

class MerginUserInfo: public QObject
{
    Q_OBJECT
    Q_PROPERTY( QString email READ email NOTIFY userInfoChanged )

    Q_PROPERTY( QString planAlias READ planAlias NOTIFY userInfoChanged ) // see PurchasingPlan::alias()
    Q_PROPERTY( QString originalTransactionId READ originalTransactionId NOTIFY userInfoChanged ) // unique identifier of the subscription in the mergin
    Q_PROPERTY( QString planProvider READ planProvider NOTIFY userInfoChanged ) // see PurchasingTransaction::provider()
    Q_PROPERTY( QString planProductId READ planProductId NOTIFY userInfoChanged ) // see PurchasingPlan::id()

    Q_PROPERTY( /*MerginSubscriptionStatus::SubscriptionStatus*/ int subscriptionStatus READ subscriptionStatus NOTIFY userInfoChanged )
    Q_PROPERTY( double storageLimit READ storageLimit NOTIFY userInfoChanged )
    Q_PROPERTY( QString subscriptionTimestamp READ subscriptionTimestamp NOTIFY userInfoChanged )
    Q_PROPERTY( QString nextBillPrice READ nextBillPrice NOTIFY userInfoChanged ) // in Bytes
    Q_PROPERTY( double diskUsage READ diskUsage NOTIFY userInfoChanged ) // in Bytes
    Q_PROPERTY( bool ownsActiveSubscription READ ownsActiveSubscription NOTIFY userInfoChanged ) // TODO  rename ownsActiveSubscription

  public:
    explicit MerginUserInfo( QObject *parent = nullptr );
    ~MerginUserInfo() = default;

  public:
    void clear();

    QString email() const;
    QString planAlias() const;
    QString originalTransactionId() const;
    QString planProvider() const;
    QString planProductId() const;
    QString nextBillPrice() const;
    /*MerginSubscriptionStatus::SubscriptionStatus*/ int subscriptionStatus() const;
    double diskUsage() const;
    double storageLimit() const;
    QString subscriptionTimestamp() const;
    bool ownsActiveSubscription() const;

    void setLocalizedPrice( const QString &price );
    void setFromJson( QJsonObject docObj );

  signals:
    void userInfoChanged();
    void planProductIdChanged();

  private:
    QString mEmail;
    QString mPlanAlias;
    QString mOriginalTransactionId;
    QString mPlanProvider;
    QString mPlanProductId;
    bool mOwnsActiveSubscription;
    QString mNextBillPrice;
    MerginSubscriptionStatus::SubscriptionStatus mSubscriptionStatus = MerginSubscriptionStatus::FreeSubscription;
    double mDiskUsage = 0.0; // in Bytes
    double mStorageLimit = 0.0; // in Bytes
    QString mSubscriptionTimestamp;
};

#endif // MERGINUSERINFO_H
