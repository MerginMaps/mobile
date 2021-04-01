/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/
#include <QDebug>
#include "inpututils.h"
#include "coreutils.h"

#import "iospurchasing.h"
#import <StoreKit/StoreKit.h>

/* ********************************************************************************************************************************************/
/* ********************************************************************************************************************************************/
/* ********************************************************************************************************************************************/

SKProduct *IosPurchasingPlan::nativeProduct() const
{
  return mNativeProduct;
}

IosPurchasingBackend *IosPurchasingPlan::backend() const
{

  return static_cast<IosPurchasingBackend *>( purchasing()->backend() );
}

void IosPurchasingPlan::setNativeProduct( SKProduct *nativeProduct )
{
  mNativeProduct = nativeProduct;
}

/* ********************************************************************************************************************************************/
/* ********************************************************************************************************************************************/
/* ********************************************************************************************************************************************/

IosPurchasingTransaction::IosPurchasingTransaction( SKPaymentTransaction *transaction, TransactionStatus status,
    QSharedPointer<PurchasingPlan> plan
                                                  )
  : PurchasingTransaction( status2type( status ), plan )
  , mStatus( status )
  , mNativeTransaction( transaction )
{
}

IosPurchasingPlan *IosPurchasingTransaction::iosPlan() const
{
  return static_cast<IosPurchasingPlan *>( plan() );
}

SKPaymentTransaction *IosPurchasingTransaction::nativeTransaction() const
{
  return mNativeTransaction;
}

IosPurchasingTransaction::TransactionStatus IosPurchasingTransaction::status() const
{
  return mStatus;
}

QString IosPurchasingTransaction::receipt() const
{
  NSData *dataReceipt = [NSData dataWithContentsOfURL:[[NSBundle mainBundle] appStoreReceiptURL]];
  NSString *receipt = [dataReceipt base64EncodedStringWithOptions:0];
  return QString::fromNSString( receipt );
}

QString IosPurchasingTransaction::errMsg() const
{
  NSString *nativeStr = [ mNativeTransaction.error localizedDescription ];
  QString err = QString::fromNSString( nativeStr );
  return err;
}

void IosPurchasingTransaction::finalizeTransaction()
{
  [[SKPaymentQueue defaultQueue] finishTransaction:mNativeTransaction];
}

PurchasingTransaction::TransactionType IosPurchasingTransaction::status2type( IosPurchasingTransaction::TransactionStatus status )
{
  if ( status == TransactionStatus::PurchaseRestored )
    return PurchasingTransaction::RestoreTransaction;
  else
  {
    return PurchasingTransaction::PuchaseTransaction;
  }
}

/* ********************************************************************************************************************************************/
/* ********************************************************************************************************************************************/
/* ********************************************************************************************************************************************/

@interface InAppPurchaseManager : NSObject <SKProductsRequestDelegate, SKPaymentTransactionObserver>
{
  IosPurchasingBackend *backend;
  NSMutableArray<SKPaymentTransaction *> *pendingTransactions;
}

-( void )requestProductData:( NSString * )identifier;
-( void )processPendingTransactions;

@end

@implementation InAppPurchaseManager

-( id )initWithBackend:( IosPurchasingBackend * )iapBackend
{
  if ( ( self = [super init] ) )
  {
    backend = iapBackend;
    pendingTransactions = [[NSMutableArray<SKPaymentTransaction *> alloc] init];
    [[SKPaymentQueue defaultQueue] addTransactionObserver:self];
  }
  return self;
}

-( void )dealloc
{
  [[SKPaymentQueue defaultQueue] removeTransactionObserver:self];
  [pendingTransactions release];
  [super dealloc];
}

-( void )requestProductData:( NSString * )identifier
{
  NSSet<NSString *> *productId = [NSSet<NSString *> setWithObject:identifier];
  SKProductsRequest *productsRequest = [[SKProductsRequest alloc] initWithProductIdentifiers:productId];
  productsRequest.delegate = self;
  [productsRequest start];
}

-( void )productsRequest:( SKProductsRequest * )request didReceiveResponse:( SKProductsResponse * )response
{
  NSArray<SKProduct *> *products = response.products;
  SKProduct *product = [products count] == 1 ? [[products firstObject] retain] : nil;

  if ( product == nil )
  {
    // failed to fetch product id from store
    NSString *invalidId = [response.invalidProductIdentifiers firstObject];
    QMetaObject::invokeMethod( backend, "planRegistrationFailed", Qt::AutoConnection, Q_ARG( QString, QString::fromNSString( invalidId ) ) );
  }
  else
  {
    QSharedPointer<IosPurchasingPlan> plan = backend->findPendingPlan( QString::fromNSString( [product productIdentifier] ) );
    if ( plan )
    {
      plan->setNativeProduct( product );

      NSNumberFormatter *numberFormatter = [[NSNumberFormatter alloc] init];
      [numberFormatter setFormatterBehavior:NSNumberFormatterBehavior10_4];
      [numberFormatter setNumberStyle:NSNumberFormatterCurrencyStyle];
      [numberFormatter setLocale:product.priceLocale];
      NSString *formattedPrice = [numberFormatter stringFromNumber:product.price];
      plan->setPrice( QString::fromNSString( formattedPrice ) );

      QMetaObject::invokeMethod( backend, "planRegistrationSucceeded", Qt::AutoConnection, Q_ARG( QString, plan->id() ) );
    }
    else
    {
      QMetaObject::invokeMethod( backend, "planRegistrationFailed", Qt::AutoConnection, Q_ARG( QString, QString::fromNSString( [product productIdentifier] ) ) );
    }
  }

  [request release];
}

+( IosPurchasingTransaction::TransactionStatus )statusFromTransaction:( SKPaymentTransaction * )transaction
{
  IosPurchasingTransaction::TransactionStatus status;
  switch ( transaction.transactionState )
  {
    case SKPaymentTransactionStatePurchasing:
      //Ignore the purchasing state as it's not really a transaction
      //And its important that it doesn't need to be finalized as
      //Calling finishTransaction: on a transaction that is
      //in the SKPaymentTransactionStatePurchasing state throws an exception
      status = IosPurchasingTransaction::Unknown;
      break;
    case SKPaymentTransactionStatePurchased:
      status = IosPurchasingTransaction::PurchaseApproved;
      break;
    case SKPaymentTransactionStateFailed:
      status = IosPurchasingTransaction::PurchaseFailed;
      break;
    case SKPaymentTransactionStateRestored:
      status = IosPurchasingTransaction::PurchaseRestored;
      break;
    default:
      status = IosPurchasingTransaction::Unknown;
      break;
  }
  return status;
}

-( void )processPendingTransactions
{
  NSMutableArray<SKPaymentTransaction *> *registeredTransactions = [NSMutableArray<SKPaymentTransaction *> array];

  for ( SKPaymentTransaction * transaction in pendingTransactions )
  {
    IosPurchasingTransaction::TransactionStatus status = [InAppPurchaseManager statusFromTransaction:transaction];

    QSharedPointer<IosPurchasingPlan> plan = backend->findRegisteredPlan( QString::fromNSString( transaction.payment.productIdentifier ) );
    if ( plan )
    {
      //It is possible that the product doesn't exist yet (because of previous restores).
      QSharedPointer<IosPurchasingTransaction> qtTransaction( new IosPurchasingTransaction( transaction, status, plan ) );
      if ( qtTransaction->thread() != backend->thread() )
      {
        qtTransaction->moveToThread( backend->thread() );
      }
      [registeredTransactions addObject:transaction];
      backend->processTransaction( qtTransaction );
    }
  }

  //Remove registeredTransactions from pendingTransactions
  [pendingTransactions removeObjectsInArray:registeredTransactions];
}

//SKPaymentTransactionObserver
- ( void )paymentQueue:( SKPaymentQueue * )queue updatedTransactions:( NSArray<SKPaymentTransaction *> * )transactions
{
  Q_UNUSED( queue )
  for ( SKPaymentTransaction * transaction in transactions )
  {
    //Create IosTransaction
    IosPurchasingTransaction::TransactionStatus status = [InAppPurchaseManager statusFromTransaction:transaction];

    if ( status == IosPurchasingTransaction::Unknown )
      continue;

    QSharedPointer<IosPurchasingPlan> plan = backend->findRegisteredPlan( QString::fromNSString( transaction.payment.productIdentifier ) );
    if ( plan )
    {
      QSharedPointer<IosPurchasingTransaction> qtTransaction( new IosPurchasingTransaction( transaction, status, plan ) );
      if ( qtTransaction->thread() != backend->thread() )
      {
        qtTransaction->moveToThread( backend->thread() );
      }
      backend->processTransaction( qtTransaction );
    }
    else
    {
      // Add the transaction to the pending transactions list, since product may not be yet registered
      [pendingTransactions addObject:transaction];
    }
  }
}

@end

/* ********************************************************************************************************************************************/
/* ********************************************************************************************************************************************/
/* ********************************************************************************************************************************************/


IosPurchasingBackend::IosPurchasingBackend()
  :  mManager( 0 )
{
}

IosPurchasingBackend::~IosPurchasingBackend()
{
  [mManager release];
}

void IosPurchasingBackend::init()
{
  mManager = [[InAppPurchaseManager alloc] initWithBackend:this];
}

void IosPurchasingBackend::registerPlan( QSharedPointer<PurchasingPlan> plan )
{
  [mManager requestProductData:( plan->id().toNSString() )];
}

void IosPurchasingBackend::createTransaction( QSharedPointer<PurchasingPlan> plan )
{
  QSharedPointer<IosPurchasingPlan> iosPlan = qSharedPointerObjectCast<IosPurchasingPlan> ( plan );
  SKPayment *payment = [SKPayment paymentWithProduct:iosPlan->nativeProduct()];
  [[SKPaymentQueue defaultQueue] addPayment:payment];
}

void IosPurchasingBackend::restore()
{
  [[SKPaymentQueue defaultQueue] restoreCompletedTransactions];
}

void IosPurchasingBackend::processTransaction( QSharedPointer<IosPurchasingTransaction> transaction )
{
  if ( transaction->status() == IosPurchasingTransaction::PurchaseApproved )
  {
    emit transactionCreationSucceeded( transaction );
  }
  else if ( transaction->status() == IosPurchasingTransaction::PurchaseRestored )
  {
    emit transactionCreationSucceeded( transaction );
  }
  else if ( transaction->status() == IosPurchasingTransaction::PurchaseFailed )
  {
    CoreUtils::log( "transaction creation", QStringLiteral( "Failed: " ) + transaction->errMsg() );
    emit transactionCreationFailed();
    transaction->finalizeTransaction();
  }

  qDebug() << "Transaction for product " << transaction->plan()->id() << " processed with status " << transaction->status();
}

bool IosPurchasingBackend::userCanMakePayments() const
{
  return [SKPaymentQueue canMakePayments];
}

QString IosPurchasingBackend::getLocalizedPrice( const QString &planId ) const
{
  QSharedPointer<IosPurchasingPlan> plan = findRegisteredPlan( planId );
  if ( plan )
  {
    return plan->price();
  }
  return QString();
}

QSharedPointer<IosPurchasingPlan> IosPurchasingBackend::findRegisteredPlan( const QString &productId ) const
{
  Purchasing *p = purchasing();
  Q_ASSERT( p );

  QSharedPointer<PurchasingPlan> plan = p->registeredPlan( productId );
  if ( !plan )
    return nullptr;

  return qSharedPointerObjectCast<IosPurchasingPlan>( plan );
}

QSharedPointer<IosPurchasingPlan> IosPurchasingBackend::findPendingPlan( const QString &productId ) const
{
  Purchasing *p = purchasing();
  Q_ASSERT( p );

  QSharedPointer<PurchasingPlan> plan = p->pendingPlan( productId );
  if ( !plan )
    return QSharedPointer<IosPurchasingPlan>();

  return qSharedPointerObjectCast<IosPurchasingPlan>( plan );
}

#include "moc_iospurchasing.cpp"
