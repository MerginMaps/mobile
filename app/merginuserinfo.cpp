/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include "merginuserinfo.h"
#include "inpututils.h"

MerginUserInfo::MerginUserInfo( QObject *parent )
  : QObject( parent )
{
  clear();
}

void MerginUserInfo::clear()
{
  mEmail = "";
  mPlanAlias = "";
  mOriginalTransactionId = "";
  mPlanProvider = "";
  mPlanProductId = "";
  mNextBillPrice = "";
  mSubscriptionStatus = MerginSubscriptionStatus::FreeSubscription;
  mDiskUsage = 0;
  mStorageLimit = 0;

  emit planProductIdChanged();
  emit userInfoChanged();
}

void MerginUserInfo::setFromJson( QJsonObject docObj )
{
  mEmail = docObj.value( QStringLiteral( "email" ) ).toString();
  mNextBillPrice = docObj.value( QStringLiteral( "next_bill_price" ) ).toString();
  QString timestamp = docObj.value( QStringLiteral( "next_payment_date" ) ).toString();
  mSubscriptionTimestamp = InputUtils::localizedDateFromUTFString( timestamp );
  mDiskUsage = docObj.value( QStringLiteral( "disk_usage" ) ).toDouble();
  mStorageLimit = docObj.value( QStringLiteral( "storage" ) ).toDouble();
  mOwnsActiveSubscription = docObj.value( QStringLiteral( "is_paid_plan" ) ).toBool();

  QString status = docObj.value( QStringLiteral( "status" ) ).toString();
  if ( status == "active" )
  {
    if ( mOwnsActiveSubscription )
    {
      mSubscriptionStatus = MerginSubscriptionStatus::ValidSubscription;
    }
    else
    {
      mSubscriptionStatus = MerginSubscriptionStatus::FreeSubscription;
    }
  }
  else if ( status == "unsubscribed" )
  {
    mSubscriptionStatus = MerginSubscriptionStatus::SubscriptionUnsubscribed;
  }
  else if ( status == "past_due" )
  {
    mSubscriptionStatus = MerginSubscriptionStatus::SubscriptionInGracePeriod;
  }
  else
  {
    // internal error some new mergin api? what to do?
    mSubscriptionStatus = MerginSubscriptionStatus::FreeSubscription;
  }

  QJsonObject metaObj = docObj.value( QStringLiteral( "meta" ) ).toObject();
  mOriginalTransactionId = metaObj.value( QStringLiteral( "original_transaction_id" ) ).toString();

  QJsonObject planObj = docObj.value( QStringLiteral( "plan" ) ).toObject();
  mPlanAlias = planObj.value( QStringLiteral( "alias" ) ).toString();
  mPlanProvider = planObj.value( QStringLiteral( "type" ) ).toString();
  QString planProductId = planObj.value( QStringLiteral( "product_id" ) ).toString();
  if ( planProductId !=  mPlanProductId )
  {
    mPlanProductId = planProductId;
    emit planProductIdChanged();
  }
  emit userInfoChanged();
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
    emit userInfoChanged();
  }
}


QString MerginUserInfo::originalTransactionId() const
{
  return mOriginalTransactionId;
}


QString MerginUserInfo::planProvider() const
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
