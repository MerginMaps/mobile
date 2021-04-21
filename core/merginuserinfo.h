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
#include "merginsubscriptiontype.h"

class MerginUserInfo: public QObject
{
    Q_OBJECT
    Q_PROPERTY( QString email READ email NOTIFY userInfoChanged )
    Q_PROPERTY( double storageLimit READ storageLimit NOTIFY userInfoChanged )
    Q_PROPERTY( double diskUsage READ diskUsage NOTIFY userInfoChanged ) // in Bytes

    Q_PROPERTY( QString planAlias READ planAlias NOTIFY subscriptionChanged ) // see PurchasingPlan::alias()
    Q_PROPERTY( /*MerginSubscriptionType::SubscriptionType*/ int planProvider READ planProvider NOTIFY subscriptionChanged ) // see PurchasingTransaction::provider()
    Q_PROPERTY( QString planProductId READ planProductId NOTIFY subscriptionChanged ) // see PurchasingPlan::id()
    Q_PROPERTY( /*MerginSubscriptionStatus::SubscriptionStatus*/ int subscriptionStatus READ subscriptionStatus NOTIFY subscriptionChanged )
    Q_PROPERTY( QString subscriptionTimestamp READ subscriptionTimestamp NOTIFY subscriptionChanged )
    Q_PROPERTY( QString nextBillPrice READ nextBillPrice NOTIFY subscriptionChanged ) // in Bytes
    Q_PROPERTY( bool ownsActiveSubscription READ ownsActiveSubscription NOTIFY subscriptionChanged )

  public:
    explicit MerginUserInfo( QObject *parent = nullptr );
    ~MerginUserInfo() = default;

  public:
    void clearSubscriptionData();
    void clearPlanInfo();
    void clear();

    QString email() const;
    QString planAlias() const;
    int subscriptionId() const;
    MerginSubscriptionType::SubscriptionType planProvider() const;
    QString planProductId() const;
    QString nextBillPrice() const;
    /*MerginSubscriptionStatus::SubscriptionStatus*/ int subscriptionStatus() const;
    double diskUsage() const;
    double storageLimit() const;
    QString subscriptionTimestamp() const;
    bool ownsActiveSubscription() const;

    void setLocalizedPrice( const QString &price );
    void setFromJson( QJsonObject docObj );
    void setSubscriptionInfoFromJson( QJsonObject docObj );

  signals:
    void userInfoChanged();
    void subscriptionChanged();
    void planProductIdChanged();
    void planProviderChanged();

  private:
    QString mEmail;
    QString mPlanAlias;
    int mSubscriptionId = -1;
    MerginSubscriptionType::SubscriptionType mPlanProvider = MerginSubscriptionType::NoneSubscriptionType;
    QString mPlanProductId;
    bool mOwnsActiveSubscription = false;
    QString mNextBillPrice;
    MerginSubscriptionStatus::SubscriptionStatus mSubscriptionStatus = MerginSubscriptionStatus::FreeSubscription;
    double mDiskUsage = 0.0; // in Bytes
    double mStorageLimit = 0.0; // in Bytes
    QString mSubscriptionTimestamp;
};

#endif // MERGINUSERINFO_H
