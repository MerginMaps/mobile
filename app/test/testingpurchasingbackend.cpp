/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include "testingpurchasingbackend.h"
#include "merginapi.h"
#include "merginuserinfo.h"

#if defined (HAVE_WIDGETS)
#include <QInputDialog>
#include <QMessageBox>
#endif

TestingPurchasingTransaction::TestingPurchasingTransaction( QByteArray receipt, PurchasingTransaction::TransactionType type, QSharedPointer<PurchasingPlan> plan )
  : PurchasingTransaction( type, plan ), mReceipt( receipt ) {}

QByteArray TestingPurchasingTransaction::receipt() const {return mReceipt;}

TestingPurchasingBackend::TestingPurchasingBackend( MerginApi *api )
  : PurchasingBackend()
  , mMerginApi( api )
{
}

void TestingPurchasingBackend::setNextPurchaseResult( const TestingPurchasingBackend::NextPurchaseResult expected )
{
  mNextResult = expected;
}


void TestingPurchasingBackend::createTransaction( QSharedPointer<PurchasingPlan> plan )
{
  if ( mNextResult == Interactive )
  {
#if defined (HAVE_WIDGETS)
    QStringList items;
    if ( mMerginApi->userInfo()->ownsActiveSubscription() )
    {
      items << tr( "Buy tier recommeneded->2 | tier12" )
            << tr( "Immediately refund the subscription (got refund) | cancel" )
            << tr( "Set grace period | grace" )
            << tr( "Set unsubscribed | unsubscribe" );
    }
    else
    {
      items << tr( "Buy tier free->recommeneded | tier01" );
    }
    items << tr( "Cancel Payment | cancelPayment" )
          << tr( "Send invalid receipt | invalidreceipt" );

    bool ok;
    QString item = QInputDialog::getItem( nullptr, tr( "QInputDialog::getItem()" ),
                                          "PURCHASING TEST", items, 0, false, &ok );
    if ( ok && !item.isEmpty() )
    {
      const QStringList parts = item.split( " | " );
      Q_ASSERT( parts.size() == 2 );
      const QString key = parts[1];
      if ( key.contains( "cancelPayment" ) )
        emit transactionCreationFailed();
      else
        emit transactionCreationSucceeded( createTestingTransaction( plan, key ) );
    }
#else
    emit transactionCreationFailed();
#endif
  }
  else if ( mNextResult == NonInteractiveBuyTier01 )
  {
    emit transactionCreationSucceeded( createTestingTransaction( plan, "tier01" ) );
  }
  else if ( mNextResult == NonInteractiveBuyTier12 )
  {
    emit transactionCreationSucceeded( createTestingTransaction( plan, "tier12" ) );
  }
  else if ( mNextResult == NonInteractiveSimulateImmediatelyCancelSubscription )
  {
    emit transactionCreationSucceeded( createTestingTransaction( plan, "cancel" ) );
  }
  else if ( mNextResult == NonInteractiveSimulateGracePeriod )
  {
    emit transactionCreationSucceeded( createTestingTransaction( plan, "grace" ) );
  }
  else if ( mNextResult == NonInteractiveSimulateUnsubscribed )
  {
    emit transactionCreationSucceeded( createTestingTransaction( plan, "unsubscribe" ) );
  }
  else if ( mNextResult == NonInteractiveBadReceipt )
  {
    emit transactionCreationSucceeded( createTestingTransaction( plan, "invalidreceipt" ) );
  }
  else if ( mNextResult == NonInteractiveUserCancelled )
  {
    emit transactionCreationFailed();
  }
}

void TestingPurchasingBackend::restore()
{
  QString tier = "tier01";
  if ( mMerginApi->userInfo()->ownsActiveSubscription() )
  {
    tier = "tier12";
  }

  if ( mNextResult == Interactive )
  {
#if defined (HAVE_WIDGETS)
    QMessageBox::information( nullptr, "TEST RESTORE", "Test restore " + tier );
    emit transactionCreationSucceeded( createTestingTransaction( mPlan, tier, true ) );
    return;
#endif
  }
  emit transactionCreationSucceeded( createTestingTransaction( mPlan, tier, true ) );
}

QString TestingPurchasingBackend::subscriptionManageUrl()
{
  return mMerginApi->apiRoot();
}

QString TestingPurchasingBackend::subscriptionBillingUrl()
{
  return mMerginApi->apiRoot();
}

QSharedPointer<TestingPurchasingTransaction> TestingPurchasingBackend::createTestingTransaction( QSharedPointer<PurchasingPlan> plan, const QString &data, bool restore )
{
  QByteArray recept;
  recept.append( "TEST-1|" );
  recept.append( data );

  PurchasingTransaction::TransactionType type;
  restore ? type = PurchasingTransaction::RestoreTransaction : type = PurchasingTransaction::PuchaseTransaction;

  QSharedPointer<TestingPurchasingTransaction> transaction( new TestingPurchasingTransaction( recept, type, plan ) );
  return transaction;
}
