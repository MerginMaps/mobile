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
    Q_PROPERTY( QString planMerginId READ planMerginId NOTIFY userInfoChanged ) // unique identifier of the subscription in the mergin
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
    QString planMerginId() const;
    QString planProvider() const;
    QString planProductId() const;
    QString nextBillPrice() const;
    /*MerginSubscriptionStatus::SubscriptionStatus*/ int subscriptionStatus() const;
    double diskUsage() const;
    double storageLimit() const;
    QString subscriptionTimestamp() const;
    bool ownsActiveSubscription() const;

    void setPaidPlan( bool ownsActiveSubscription );
    void setPlanMerginId( const QString &planMerginId );
    void setPlanProvider( const QString &planProvider );
    void setPlanProductId( const QString &planProductId );
    void setEmail( const QString &email );
    void setPlanAlias( const QString &planAlias );
    void setNextBillPrice( const QString &nextBillPrice );
    void setSubscriptionStatus( const MerginSubscriptionStatus::SubscriptionStatus &subscriptionStatus );
    void setDiskUsage( double diskUsage );
    void setStorageLimit( double storageLimit );
    void setSubscriptionTimestamp( const QString &subscriptionTimestamp );

    void setFromJson( QJsonObject docObj );

  signals:
    void userInfoChanged();

  private:
    QString mEmail;
    QString mPlanAlias;
    QString mPlanMerginId;
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
