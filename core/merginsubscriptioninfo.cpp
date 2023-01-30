/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include "merginsubscriptioninfo.h"
#include "coreutils.h"

MerginSubscriptionInfo::MerginSubscriptionInfo( QObject *parent )
  : QObject( parent )
{
  clear();
}

void MerginSubscriptionInfo::clearSubscriptionData()
{
  mSubscriptionStatus = MerginSubscriptionStatus::FreeSubscription;
  mSubscriptionTimestamp = "";
  mNextBillPrice = "";
  mSubscriptionId = -1;
  mOwnsActiveSubscription = false;
  mCanAccessSubscription = false;
  mActionRequired = false;
}

void MerginSubscriptionInfo::clearPlanInfo()
{
  mPlanAlias = "";
  mPlanProvider = MerginSubscriptionType::NoneSubscriptionType;

  // plan product Id might change from several sources, we need to emit its signal only when it has really changed
  if ( !mPlanProductId.isEmpty() )
  {
    mPlanProductId = "";
    emit planProductIdChanged();
  }

  emit planProviderChanged();
}

void MerginSubscriptionInfo::clear()
{
  clearSubscriptionData();
  clearPlanInfo();

  emit subscriptionInfoChanged();
}

void MerginSubscriptionInfo::setFromJson( QJsonObject docObj )
{
  // parse service.subscription data
  QJsonObject subscriptionObj = docObj.value( QStringLiteral( "subscription" ) ).toObject();
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

  if ( docObj.contains( QStringLiteral( "plan" ) ) )
  {
    // this user can access subscription information, because they received a response from /service API
    mCanAccessSubscription = true;

    // parse service.plan data
    QJsonObject planObj = docObj.value( QStringLiteral( "plan" ) ).toObject();
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
  }

  // check if some user action is required
  bool isActionRequired = docObj.value( QStringLiteral( "action_required" ) ).toBool();
  if ( isActionRequired != mActionRequired )
  {
    mActionRequired = isActionRequired;
  }

  emit subscriptionInfoChanged();
}

bool MerginSubscriptionInfo::ownsActiveSubscription() const
{
  return mOwnsActiveSubscription;
}

void MerginSubscriptionInfo::setLocalizedPrice( const QString &price )
{
  if ( price.isEmpty() )
    return;

  if ( price != mNextBillPrice )
  {
    mNextBillPrice = price;
    emit subscriptionInfoChanged();
  }
}


int MerginSubscriptionInfo::subscriptionId() const
{
  return mSubscriptionId;
}


MerginSubscriptionType::SubscriptionType MerginSubscriptionInfo::planProvider() const
{
  return mPlanProvider;
}

QString MerginSubscriptionInfo::planProductId() const
{
  return mPlanProductId;
}

QString MerginSubscriptionInfo::planAlias() const
{
  return mPlanAlias;
}

QString MerginSubscriptionInfo::nextBillPrice() const
{
  return mNextBillPrice;
}

int MerginSubscriptionInfo::subscriptionStatus() const
{
  return mSubscriptionStatus;
}

QString MerginSubscriptionInfo::subscriptionTimestamp() const
{
  return mSubscriptionTimestamp;
}

bool MerginSubscriptionInfo::actionRequired() const
{
  return mActionRequired;
}

bool MerginSubscriptionInfo::canAccessSubscription() const
{
  return mCanAccessSubscription;
}
