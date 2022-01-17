/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include  <QDebug>
#include <QNetworkReply>
#include <QUrlQuery>

#include "purchasing.h"
#include "merginapi.h"
#include "inpututils.h"
#include "merginuserinfo.h"
#include "merginsubscriptioninfo.h"
#include "coreutils.h"

#if defined (APPLE_PURCHASING)
#include "ios/iospurchasing.h"
#endif
#if defined (INPUT_TEST)
#include "test/testingpurchasingbackend.h"
#endif

void PurchasingPlan::clear()
{
  mAlias = QString();
  mId = QString();
  mPeriod = QString();
  mPrice = QString();
  mStorage = QString();
}

QString PurchasingPlan::alias() const
{
  return mAlias;
}

void PurchasingPlan::setAlias( const QString &alias )
{
  mAlias = alias;
  emit planChanged();
}

QString PurchasingPlan::id() const
{
  return mId;
}

void PurchasingPlan::setId( const QString &billingProductId )
{
  mId = billingProductId;
  emit planChanged();
}

QString PurchasingPlan::period() const
{
  return mPeriod;
}

void PurchasingPlan::setPeriod( const QString &billingPeriod )
{
  mPeriod = billingPeriod;
  emit planChanged();
}

QString PurchasingPlan::price() const
{
  return mPrice;
}

void PurchasingPlan::setPrice( const QString &billingPrice )
{
  mPrice = billingPrice;
  emit planChanged();
}

bool PurchasingPlan::isIndividualPlan() const
{
  return !mIsProfessional;
}

bool PurchasingPlan::isProfessionalPlan() const
{
  return mIsProfessional;
}

QString PurchasingPlan::storage() const
{
  return mStorage;
}

void PurchasingPlan::setStorage( const QString &storage )
{
  mStorage = storage;
  emit planChanged();
}

Purchasing *PurchasingPlan::purchasing() const
{
  return mPurchasing;
}

void PurchasingPlan::setPurchasing( Purchasing *purchasing )
{
  mPurchasing = purchasing;
}

void PurchasingPlan::setFromJson( QJsonObject docObj )
{
  mAlias = docObj.value( QStringLiteral( "alias" ) ).toString();
  mId = docObj.value( QStringLiteral( "billing_product_id" ) ).toString();
  const QString billingPeriod = docObj.value( QStringLiteral( "billing_period" ) ).toString();
  bool isMonthly = billingPeriod.contains( "month", Qt::CaseInsensitive );
  if ( isMonthly )
  {
    mPeriod = tr( "Monthly subscription" );
  }
  else
  {
    mPeriod = tr( "Annual subscription" );
  }
  const QString billingPrice = docObj.value( QStringLiteral( "billing_price" ) ).toString();
  if ( isMonthly )
  {
    mPrice = billingPrice + "/" + tr( "month" );
  }
  else
  {
    mPrice = billingPrice + "/" + tr( "year" );
  }

  double bytes = docObj.value( QStringLiteral( "storage" ) ).toDouble();
  mStorage = InputUtils::bytesToHumanSize( bytes );

  mIsProfessional = ( bytes > 1024.0 * 1024.0 * 1024.0 * 5 ); // storage > 5GB

  emit planChanged();
}

/* ********************************************************************************************************/
/* ********************************************************************************************************/
/* ********************************************************************************************************/


PurchasingTransaction::PurchasingTransaction( PurchasingTransaction::TransactionType type, QSharedPointer<PurchasingPlan> plan )
  : mPlan( plan )
  , mType( type )
{
}

void PurchasingTransaction::verificationFinished()
{
  Q_ASSERT( mPlan );
  Q_ASSERT( mPlan->purchasing() );
  MerginApi *api = mPlan->purchasing()->merginApi();

  QNetworkReply *r = qobject_cast<QNetworkReply *>( sender() );
  Q_ASSERT( r );

  if ( r->error() == QNetworkReply::NoError )
  {
    CoreUtils::log( "purchase successful", QStringLiteral( "Payment success" ) );
    mPlan->purchasing()->onTransactionVerificationSucceeded( this );
  }
  else
  {
    QString serverMsg = api->extractServerErrorMsg( r->readAll() );
    QString message = QStringLiteral( "Network API error: %1(): %2. %3" ).arg( QStringLiteral( "purchase" ), r->errorString(), serverMsg );
    emit api->networkErrorOccurred( serverMsg, QStringLiteral( "Mergin API error: purchase" ) );
    CoreUtils::log( "purchase", QStringLiteral( "FAILED - %1" ).arg( message ) );
    mPlan->purchasing()->onTransactionVerificationFailed( this );
  }
  r->deleteLater();
}

PurchasingTransaction::TransactionType PurchasingTransaction::type() const
{
  return mType;
}

PurchasingPlan *PurchasingTransaction::plan() const
{
  return mPlan.get();
}

/* ********************************************************************************************************/
/* ********************************************************************************************************/
/* ********************************************************************************************************/

Purchasing::Purchasing( MerginApi *merginApi, QObject *parent )
  : QObject( parent )
  , mMerginApi( merginApi )
{
  setDefaultUrls();
  createBackend();

  connect( mMerginApi, &MerginApi::apiRootChanged, this, &Purchasing::onMerginServerChanged );
  connect( mMerginApi, &MerginApi::apiSupportsSubscriptionsChanged, this, &Purchasing::onMerginServerStatusChanged );
  connect( mMerginApi, &MerginApi::apiVersionStatusChanged, this, &Purchasing::onMerginServerStatusChanged );
  connect( mMerginApi->subscriptionInfo(), &MerginSubscriptionInfo::planProviderChanged, this, &Purchasing::evaluateHasInAppPurchases );
  connect( mMerginApi->subscriptionInfo(), &MerginSubscriptionInfo::planProductIdChanged, this, &Purchasing::onMerginPlanProductIdChanged );

  connect( this, &Purchasing::hasInAppPurchasesChanged, this, &Purchasing::onHasInAppPurchasesChanged );
}

void Purchasing::createBackend()
{
  mBackend.reset();
#if defined( PURCHASING )
#if defined (APPLE_PURCHASING)
  mBackend.reset( new IosPurchasingBackend );
#elif defined (INPUT_TEST)
  mBackend.reset( new TestingPurchasingBackend( mMerginApi ) );
#endif
#endif

  if ( mBackend )
  {
    mBackend->setPurchasing( this );
    mBackend->init();

    connect( mBackend.get(), &PurchasingBackend::planRegistrationSucceeded, this, &Purchasing::onPlanRegistrationSucceeded );
    connect( mBackend.get(), &PurchasingBackend::planRegistrationFailed, this, &Purchasing::onPlanRegistrationFailed );

    connect( mBackend.get(), &PurchasingBackend::transactionCreationSucceeded, this, &Purchasing::onTransactionCreationSucceeded );
    connect( mBackend.get(), &PurchasingBackend::transactionCreationFailed, this, &Purchasing::onTransactionCreationFailed );
  }
}

void Purchasing::evaluateHasInAppPurchases()
{
  bool hasInApp =
    mMerginApi->apiSupportsSubscriptions() &&
    mMerginApi->apiVersionStatus() == MerginApiStatus::OK &&
    bool( mBackend ) &&
    mBackend->userCanMakePayments();

  bool hasCompatiblePlanType = true;
  if ( mMerginApi->subscriptionInfo()->ownsActiveSubscription() )
  {
    hasCompatiblePlanType =
      bool( mBackend ) &&
      mBackend->provider() == mMerginApi->subscriptionInfo()->planProvider();
  }
  setHasInAppPurchases( hasInApp && hasCompatiblePlanType );
}

void Purchasing::onHasInAppPurchasesChanged()
{
  bool hasManageCapability = false;
  QString subscriptionManageUrl = mMerginApi->apiRoot() + "subscription";
  QString subscriptionBillingUrl = mMerginApi->apiRoot() + "billing";

  if ( hasInAppPurchases() )
  {
    hasManageCapability = mBackend->hasManageSubscriptionCapability();
    subscriptionManageUrl = mBackend->subscriptionManageUrl();
    subscriptionBillingUrl = mBackend->subscriptionBillingUrl();
  }

  setHasManageSubscriptionCapability( hasManageCapability );
  setSubscriptionManageUrl( subscriptionManageUrl );
  setSubscriptionBillingUrl( subscriptionBillingUrl );
}

bool Purchasing::hasInAppPurchases() const
{
  return mHasInAppPurchases;
}

bool Purchasing::hasManageSubscriptionCapability() const
{
  return mHasManageSubscriptionCapability;
}

QString Purchasing::subscriptionManageUrl()
{
  return mSubscriptionManageUrl;
}

QString Purchasing::subscriptionBillingUrl()
{
  return mSubscriptionBillingUrl;
}

void Purchasing::onMerginServerChanged()
{
  qDebug() << "Mergin Server Url changed reseting purchasing";
  clean();
}

void Purchasing::purchase( const QString &planId )
{
  if ( transactionPending() )
  {
    qDebug() << "unable to initiate purchase, there is a transaction pending";
    return;
  }

  if ( hasInAppPurchases() )
  {
    if ( mPlansWithPendingRegistration.contains( planId ) )
    {
      qDebug() << "unable to initiate purchase, plan " << planId << " is not yet registered";
    }

    if ( !mRegisteredPlans.contains( planId ) )
    {
      qDebug() << "unable to initiate purchase, unable to find plan " << planId;
    }

    setTransactionCreationRequested( true );
    return mBackend->createTransaction( mRegisteredPlans.value( planId ) );
  }
  else
  {
    qDebug() << "unable to initiate purchase, purchase api not ready";
  }
}

void Purchasing::restore()
{
  if ( transactionPending() )
  {
    qDebug() << "unable to initiate restore, there is a transaction pending";
    return;
  }

  if ( hasInAppPurchases() )
  {
    setTransactionCreationRequested( true );
    return mBackend->restore( );
  }
  else
  {
    qDebug() << "unable to initiate restore, purchase api not ready";
  }
}

void Purchasing::onMerginPlanProductIdChanged()
{
  if ( !mBackend )
    return;

  QString planId = mMerginApi->subscriptionInfo()->planProductId();
  if ( planId.isEmpty() )
    return;

  if ( mBackend->provider() != mMerginApi->subscriptionInfo()->planProvider() )
    return;

  QString price = mBackend->getLocalizedPrice( mMerginApi->subscriptionInfo()->planProductId() );
  mMerginApi->subscriptionInfo()->setLocalizedPrice( price );
}

void Purchasing::onMerginServerStatusChanged()
{
  qDebug() << "Mergin Server status changed, fetching purchasing plan";
  if ( mBackend && mPlansWithPendingRegistration.empty() && mRegisteredPlans.empty() )
  {
    fetchPurchasingPlans();
  }
  evaluateHasInAppPurchases();
}

void Purchasing::fetchPurchasingPlans( )
{
  if ( !mMerginApi->apiSupportsSubscriptions() ) return;
  if ( mMerginApi->apiVersionStatus() != MerginApiStatus::OK ) return;

  QUrl url( mMerginApi->apiRoot() + QStringLiteral( "/v1/plan" ) );
  QUrlQuery query;
  query.addQueryItem( "billing_service", MerginSubscriptionType::toString( mBackend->provider() ) );
  url.setQuery( query );
  QNetworkRequest request = mMerginApi->getDefaultRequest( false );
  request.setUrl( url );
  QNetworkReply *reply = mMerginApi->mManager.get( request );
  connect( reply, &QNetworkReply::finished, this, &Purchasing::onFetchPurchasingPlansFinished );
  CoreUtils::log( "request plan", QStringLiteral( "Requesting purchasing plans for provider %1" ).arg( MerginSubscriptionType::toString( mBackend->provider() ) ) );
}

void Purchasing::onFetchPurchasingPlansFinished()
{
  QNetworkReply *r = qobject_cast<QNetworkReply *>( sender() );
  Q_ASSERT( r );
  QString serverMsg;
  if ( r->error() == QNetworkReply::NoError )
  {
    CoreUtils::log( "fetch plans", QStringLiteral( "Success" ) );
    QByteArray data = r->readAll();
    const QJsonDocument doc = QJsonDocument::fromJson( data );
    if ( doc.isArray() )
    {
      const QJsonArray vArray = doc.array();
      for ( auto it = vArray.constBegin(); it != vArray.constEnd(); ++it )
      {
        const QJsonObject obj = it->toObject();
        QSharedPointer<PurchasingPlan> plan = mBackend->createPlan();
        plan->setFromJson( obj );
        plan->setPurchasing( this );
        if ( mPlansWithPendingRegistration.contains( plan->id() ) )
        {
          qDebug() << "Plan " << plan->id() << " registration already pending";
        }
        else if ( mRegisteredPlans.contains( plan->id() ) )
        {
          qDebug() << "Plan " << plan->id() << " already registered";
        }
        else
        {
          qDebug() << "Plan " << plan->id() << " requested registration";
          mPlansWithPendingRegistration.insert( plan->id(), plan );
          mBackend->registerPlan( plan );
        }
      }
    }
  }
  else
  {
    serverMsg = mMerginApi->extractServerErrorMsg( r->readAll() );
    CoreUtils::log( "fetch plans", QStringLiteral( "FAILED - %1. %2" ).arg( r->errorString(), serverMsg ) );
  }
  r->deleteLater();
}

void Purchasing::clean()
{
  createBackend();
  mRegisteredPlans.clear();
  mPlansWithPendingRegistration.clear();
  mTransactionsWithPendingVerification.clear();
  mTransactionCreationRequested = false;
  mIndividualPlanId.clear();
  mProfessionalPlanId.clear();
  setDefaultUrls();
  setHasInAppPurchases( false );

  emit individualPlanChanged();
  emit professionalPlanChanged();
  emit transactionPendingChanged();
}

void Purchasing::onPlanRegistrationFailed( const QString &id )
{
  qDebug() << "Failed to register plan " + id;
  if ( mPlansWithPendingRegistration.contains( id ) )
    mPlansWithPendingRegistration.remove( id );

  if ( mPlansWithPendingRegistration.empty() && mRegisteredPlans.empty() )
  {
    CoreUtils::log( "Plan Registration", QStringLiteral( "Failed to register any plans" ) );
  }
}

void Purchasing::onPlanRegistrationSucceeded( const QString &id )
{
  if ( mPlansWithPendingRegistration.contains( id ) )
  {
    QSharedPointer<PurchasingPlan> plan = mPlansWithPendingRegistration.take( id );
    if ( mRegisteredPlans.contains( plan->id() ) )
    {
      qDebug() << "Plan " << id << " is already in registered plans.";
    }
    else
    {
      qDebug() << "Plan " + id + " registered";
      mRegisteredPlans.insert( id, plan );
      if ( plan->isProfessionalPlan() )
      {
        qDebug() << "Plan " + id + " is professional plan";
        setProfessionalPlanId( plan->id() );
      }
      if ( plan->isIndividualPlan() )
      {
        qDebug() << "Plan " + id + " is individual plan";
        setIndividualPlanId( plan->id() );
      }
    }
  }
  else
  {
    qDebug() << "Plan " + id + " registered OK, but failed to find in pending registrations";
  }
  emit hasInAppPurchasesChanged();
}

void Purchasing::onTransactionCreationSucceeded( QSharedPointer<PurchasingTransaction> transaction )
{
  setTransactionCreationRequested( false );

  if ( !mMerginApi->validateAuthAndContinute() || mMerginApi->apiVersionStatus() != MerginApiStatus::OK )
  {
    return;
  }

  mTransactionsWithPendingVerification.push_back( transaction );

  QNetworkRequest request = mMerginApi->getDefaultRequest();
  QUrl url( mMerginApi->apiRoot() + QStringLiteral( "v1/subscription/process-transaction" ) );
  request.setUrl( url );
  request.setHeader( QNetworkRequest::ContentTypeHeader, QVariant( "application/json" ) );
  QJsonDocument jsonDoc;
  QJsonObject jsonObject;
  jsonObject.insert( QStringLiteral( "type" ), MerginSubscriptionType::toString( transaction->provider() ) );
  jsonObject.insert( QStringLiteral( "receipt-data" ), transaction->receipt() );
  jsonObject.insert( QStringLiteral( "api_key" ), mMerginApi->getApiKey( mMerginApi->apiRoot() ) );
  jsonDoc.setObject( jsonObject );
  QByteArray json = jsonDoc.toJson( QJsonDocument::Compact );
  QNetworkReply *reply = mMerginApi->mManager.post( request, json );
  connect( reply, &QNetworkReply::finished, transaction.get(), &PurchasingTransaction::verificationFinished );
  CoreUtils::log( "process transaction", QStringLiteral( "Requesting processing of in-app transaction: " ) + url.toString() );
}

void Purchasing::onTransactionCreationFailed()
{
  notify( tr( "Failed to process payment details.%1Subscription is not purchased." ).arg( "<br/>" ) );
  setTransactionCreationRequested( false );
}

void Purchasing::onTransactionVerificationSucceeded( PurchasingTransaction *transaction )
{
  Q_ASSERT( transaction );
  if ( transaction->type() == PurchasingTransaction::RestoreTransaction )
    notify( tr( "Successfully restored your subscription" ) );
  else
    notify( tr( "Successfully purchased subscription" ) );

  removePendingTransaction( transaction );
  mMerginApi->getSubscriptionInfo();
}

void Purchasing::onTransactionVerificationFailed( PurchasingTransaction *transaction )
{
  Q_ASSERT( transaction );
  if ( transaction->type() == PurchasingTransaction::RestoreTransaction )
    notify( tr( "Unable to restore your subscription" ) );
  else
    notify( tr( "Failed to purchase subscription" ) );

  removePendingTransaction( transaction );
}

void Purchasing::notify( const QString &msg )
{
  mMerginApi->notify( msg );
}


MerginApi *Purchasing::merginApi() const
{
  return mMerginApi;
}

int Purchasing::registeredPlansCount() const
{
  return mRegisteredPlans.count();
}

void Purchasing::removePendingTransaction( PurchasingTransaction *transaction )
{
  transaction->finalizeTransaction();

  int index = -1;
  for ( int i = 0; i < mTransactionsWithPendingVerification.count(); ++i )
  {
    if ( mTransactionsWithPendingVerification.at( i ).get() == transaction )
    {
      index = i;
      break;
    }
  }
  if ( index >= 0 )
  {
    mTransactionsWithPendingVerification.removeAt( index );
  }

  emit transactionPendingChanged();
}


bool Purchasing::transactionPending() const
{
  return !mTransactionsWithPendingVerification.empty() || mTransactionCreationRequested;
}

PurchasingPlan *Purchasing::individualPlan() const
{
  static PurchasingPlan sEmptyPlan;

  QSharedPointer<PurchasingPlan> plan = registeredPlan( mIndividualPlanId );
  if ( plan )
  {
    return plan.get();
  }
  else
  {
    return &sEmptyPlan;
  }
}

PurchasingPlan *Purchasing::professionalPlan() const
{
  static PurchasingPlan sEmptyPlan;

  QSharedPointer<PurchasingPlan> plan = registeredPlan( mProfessionalPlanId );
  if ( plan )
  {
    return plan.get();
  }
  else
  {
    return &sEmptyPlan;
  }
}

QSharedPointer<PurchasingPlan> Purchasing::registeredPlan( const QString &id ) const
{
  if ( id.isEmpty() )
    return nullptr;

  if ( mRegisteredPlans.contains( id ) )
    return mRegisteredPlans.value( id );

  return nullptr;
}

QSharedPointer<PurchasingPlan> Purchasing::pendingPlan( const QString &id ) const
{
  if ( id.isEmpty() )
    return nullptr;

  if ( mPlansWithPendingRegistration.contains( id ) )
    return mPlansWithPendingRegistration.value( id );

  return nullptr;
}

void Purchasing::setSubscriptionBillingUrl( const QString &subscriptionBillingUrl )
{
  if ( mSubscriptionBillingUrl != subscriptionBillingUrl )
  {
    mSubscriptionBillingUrl = subscriptionBillingUrl;
    emit subscriptionBillingUrlChanged();
  }
}

void Purchasing::setDefaultUrls()
{
  mSubscriptionManageUrl = mMerginApi->apiRoot() + "subscription";
  emit subscriptionManageUrlChanged();
  mSubscriptionBillingUrl = mMerginApi->apiRoot() + "billing";
  emit subscriptionBillingUrlChanged();
}

void Purchasing::setSubscriptionManageUrl( const QString &subscriptionManageUrl )
{
  if ( mSubscriptionManageUrl != subscriptionManageUrl )
  {
    mSubscriptionManageUrl = subscriptionManageUrl;
    emit subscriptionManageUrlChanged();
  }
}

void Purchasing::setHasManageSubscriptionCapability( bool hasManageSubscriptionCapability )
{
  if ( mHasManageSubscriptionCapability != hasManageSubscriptionCapability )
  {
    mHasManageSubscriptionCapability = hasManageSubscriptionCapability;
    emit hasManageSubscriptionCapabilityChanged();
  }
}

void Purchasing::setHasInAppPurchases( bool hasInAppPurchases )
{
  if ( mHasInAppPurchases != hasInAppPurchases )
  {
    mHasInAppPurchases = hasInAppPurchases;
    emit hasInAppPurchasesChanged();
  }
}

void Purchasing::setTransactionCreationRequested( bool transactionCreationRequested )
{
  if ( mTransactionCreationRequested != transactionCreationRequested )
  {
    mTransactionCreationRequested = transactionCreationRequested;
    emit transactionPendingChanged();
  }
}

void Purchasing::setIndividualPlanId( const QString &planId )
{
  if ( mIndividualPlanId != planId )
  {
    mIndividualPlanId = planId;
    emit individualPlanChanged();
  }
}

void Purchasing::setProfessionalPlanId( const QString &planId )
{
  if ( mProfessionalPlanId != planId )
  {
    mProfessionalPlanId = planId;
    emit professionalPlanChanged();
  }
}
