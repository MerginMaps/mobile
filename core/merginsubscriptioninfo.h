/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef MERGINSUBSCRIPTIONINFO_H
#define MERGINSUBSCRIPTIONINFO_H

#include <QObject>
#include <QString>
#include <QJsonObject>

#include "merginsubscriptionstatus.h"
#include "merginsubscriptiontype.h"

class MerginSubscriptionInfo: public QObject
{
    Q_OBJECT

    Q_PROPERTY( QString planAlias READ planAlias NOTIFY subscriptionInfoChanged ) // see PurchasingPlan::alias()
    Q_PROPERTY( /*MerginSubscriptionType::SubscriptionType*/ int planProvider READ planProvider NOTIFY subscriptionInfoChanged ) // see PurchasingTransaction::provider()
    Q_PROPERTY( QString planProductId READ planProductId NOTIFY subscriptionInfoChanged ) // see PurchasingPlan::id()
    Q_PROPERTY( /*MerginSubscriptionStatus::SubscriptionStatus*/ int subscriptionStatus READ subscriptionStatus NOTIFY subscriptionInfoChanged )
    Q_PROPERTY( QString subscriptionTimestamp READ subscriptionTimestamp NOTIFY subscriptionInfoChanged )
    Q_PROPERTY( QString nextBillPrice READ nextBillPrice NOTIFY subscriptionInfoChanged ) // in Bytes
    Q_PROPERTY( bool ownsActiveSubscription READ ownsActiveSubscription NOTIFY subscriptionInfoChanged )
    Q_PROPERTY( bool actionRequired READ actionRequired NOTIFY subscriptionInfoChanged )

  public:
    explicit MerginSubscriptionInfo( QObject *parent = nullptr );
    ~MerginSubscriptionInfo() = default;

  public:
    void clearSubscriptionData();
    void clearPlanInfo();
    void clear();

    QString planAlias() const;
    int subscriptionId() const;
    MerginSubscriptionType::SubscriptionType planProvider() const;
    QString planProductId() const;
    QString nextBillPrice() const;
    /*MerginSubscriptionStatus::SubscriptionStatus*/ int subscriptionStatus() const;
    QString subscriptionTimestamp() const;
    bool ownsActiveSubscription() const;

    void setLocalizedPrice( const QString &price );
    void setFromJson( QJsonObject docObj );
    void setSubscriptionInfoFromJson( QJsonObject docObj );
    bool actionRequired() const;

  signals:
    void subscriptionInfoChanged();
    void planProductIdChanged();
    void planProviderChanged();
    void storageChanged( double storage );

  private:
    QString mPlanAlias;
    int mSubscriptionId = -1;
    MerginSubscriptionType::SubscriptionType mPlanProvider = MerginSubscriptionType::NoneSubscriptionType;
    QString mPlanProductId;
    bool mOwnsActiveSubscription = false;
    QString mNextBillPrice;
    MerginSubscriptionStatus::SubscriptionStatus mSubscriptionStatus = MerginSubscriptionStatus::FreeSubscription;
    QString mSubscriptionTimestamp;
    bool mActionRequired = false;
};

#endif // MERGINSUBSCRIPTIONINFO_H
