/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include "merginuserinfo.h"
#include "coreutils.h"

MerginUserInfo::MerginUserInfo( QObject *parent )
  : QObject( parent )
{
  clear();
}

void MerginUserInfo::clearSubscriptionData()
{
  mSubscriptionStatus = MerginSubscriptionStatus::FreeSubscription;
  mSubscriptionTimestamp = "";
  mNextBillPrice = "";
  mSubscriptionId = -1;
  mOwnsActiveSubscription = false;
}

void MerginUserInfo::clearPlanInfo()
{
  mPlanAlias = "";
  mPlanProvider = MerginSubscriptionType::NoneSubscriptionType;
  mPlanProductId = "";
  emit planProviderChanged();
  emit planProductIdChanged();
}

void MerginUserInfo::clear()
{
  clearSubscriptionData();
  mEmail = "";
  mDiskUsage = 0;
  mStorageLimit = 0;
  clearPlanInfo();

  emit subscriptionChanged();
  emit userInfoChanged();
}

void MerginUserInfo::setFromJson( QJsonObject docObj )
{
  // parse profile data
  mEmail = docObj.value( QStringLiteral( "email" ) ).toString();
  mDiskUsage = docObj.value( QStringLiteral( "disk_usage" ) ).toDouble();
  mStorageLimit = docObj.value( QStringLiteral( "storage" ) ).toDouble();

  emit userInfoChanged();
}

void MerginUserInfo::setSubscriptionInfoFromJson( QJsonObject docObj )
{

  QJsonObject profileObj = docObj.value( QStringLiteral( "profile" ) ).toObject();
  mStorageLimit = profileObj.value( QStringLiteral( "storage" ) ).toDouble();

  // parse service data
  QJsonObject serviceObj = docObj.value( QStringLiteral( "service" ) ).toObject();

  // parse service.subscription data
  QJsonObject subscriptionObj = serviceObj.value( QStringLiteral( "subscription" ) ).toObject();
  if ( subscriptionObj.isEmpty() )
  {
    // only free plan is assigned, subscription data is not present in JSON
    clearSubscriptionData();
  }
  else
  {
    // user has subscription

    mNextBillPrice = subscriptionObj.value( QStringLiteral( "next_bill_price" ) ).toString();
    QString nextPaymentDate = subscriptionObj.value( QStringLiteral( "next_payment" ) ).toString();
    QString validUntil = subscriptionObj.value( QStringLiteral( "valid_until" ) ).toString();
    if ( nextPaymentDate.isEmpty() )
    {
      mSubscriptionTimestamp = CoreUtils::localizedDateFromUTFString( validUntil );
    }
    else
    {
      mSubscriptionTimestamp = CoreUtils::localizedDateFromUTFString( nextPaymentDate );
    }

    mSubscriptionId = subscriptionObj.value( QStringLiteral( "id" ) ).toInt();
    QString status = subscriptionObj.value( QStringLiteral( "status" ) ).toString();

    if ( status == "active" )
    {
      if ( nextPaymentDate.isEmpty() )
      {
        mSubscriptionStatus = MerginSubscriptionStatus::SubscriptionUnsubscribed;
      }
      else
      {
        mSubscriptionStatus = MerginSubscriptionStatus::ValidSubscription;
      }
    }

    else if ( status == "past_due" )
    {
      mSubscriptionStatus = MerginSubscriptionStatus::SubscriptionInGracePeriod;
    }
    else // cancelled
    {
      mSubscriptionStatus = MerginSubscriptionStatus::CanceledSubscription;
    }
  }

  // parse service.plan data
  QJsonObject planObj = serviceObj.value( QStringLiteral( "plan" ) ).toObject();
  mOwnsActiveSubscription = planObj.value( QStringLiteral( "is_paid_plan" ) ).toBool();
  mPlanAlias = planObj.value( QStringLiteral( "alias" ) ).toString();
  MerginSubscriptionType::SubscriptionType planProvider = MerginSubscriptionType::fromString( planObj.value( QStringLiteral( "type" ) ).toString() );
  if ( planProvider != mPlanProvider )
  {
    mPlanProvider = planProvider;
    emit planProviderChanged();
  }
  QString planProductId = planObj.value( QStringLiteral( "product_id" ) ).toString();
  if ( planProductId !=  mPlanProductId )
  {
    mPlanProductId = planProductId;
    emit planProductIdChanged();
  }

  emit subscriptionChanged();
}

bool MerginUserInfo::ownsActiveSubscription() const
{
  return mOwnsActiveSubscription;
}

void MerginUserInfo::setLocalizedPrice( const QString &price )
{
  if ( price.isEmpty() )
    return;

  if ( price != mNextBillPrice )
  {
    mNextBillPrice = price;
    emit subscriptionChanged();
  }
}


int MerginUserInfo::subscriptionId() const
{
  return mSubscriptionId;
}


MerginSubscriptionType::SubscriptionType MerginUserInfo::planProvider() const
{
  return mPlanProvider;
}

QString MerginUserInfo::planProductId() const
{
  return mPlanProductId;
}


QString MerginUserInfo::email() const
{
  return mEmail;
}

QString MerginUserInfo::planAlias() const
{
  return mPlanAlias;
}

QString MerginUserInfo::nextBillPrice() const
{
  return mNextBillPrice;
}

int MerginUserInfo::subscriptionStatus() const
{
  return mSubscriptionStatus;
}

double MerginUserInfo::diskUsage() const
{
  return mDiskUsage;
}

double MerginUserInfo::storageLimit() const
{
  return mStorageLimit;
}

QString MerginUserInfo::subscriptionTimestamp() const
{
  return mSubscriptionTimestamp;
}
