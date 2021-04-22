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
#include "merginsubscriptioninfo.h"
#include "inpututils.h"

#if defined (HAVE_WIDGETS)
#include <QInputDialog>
#include <QMessageBox>
#endif

TestingPurchasingTransaction::TestingPurchasingTransaction( QString receipt, PurchasingTransaction::TransactionType type, QSharedPointer<PurchasingPlan> plan )
  : PurchasingTransaction( type, plan ), mReceipt( receipt ) {}

QString TestingPurchasingTransaction::receipt() const {return mReceipt;}

TestingPurchasingBackend::TestingPurchasingBackend( MerginApi *api )
  : PurchasingBackend()
  , mMerginApi( api )
{
}

void TestingPurchasingBackend::setNextPurchaseResult( const TestingPurchasingBackend::NextPurchaseResult expected )
{
  mNextResult = expected;
}

void TestingPurchasingBackend::registerPlan( QSharedPointer<PurchasingPlan> plan )
{
  if ( plan->isIndividualPlan() )
    mIndividualPlan = plan;

  emit planRegistrationSucceeded( plan->id() );
}


void TestingPurchasingBackend::createTransaction( QSharedPointer<PurchasingPlan> plan )
{
  if ( mNextResult == Interactive )
  {
#if defined (HAVE_WIDGETS)
    QStringList items;
    if ( plan->isIndividualPlan() )
    {
      items << "Buy individual plan | tier01";
    }
    else
    {
      items << "Buy professional plan | tier12";
    }

    if ( mMerginApi->subscriptionInfo()->ownsActiveSubscription() )
    {
      items << "Immediately refund the subscription (got refund) | cancel"
            << "Set grace period | grace"
            << "Set unsubscribed | unsubscribe";
    }
    items << "Cancel Payment | cancelPayment"
          << "Send invalid receipt | invalidreceipt";

    bool ok;
    QString item = QInputDialog::getItem( nullptr, "QInputDialog::getItem()",
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
  else if ( mNextResult == NonInteractiveBuyIndividualPlan )
  {
    emit transactionCreationSucceeded( createTestingTransaction( plan, "tier01" ) );
  }
  else if ( mNextResult == NonInteractiveBuyProfessionalPlan )
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
  if ( mMerginApi->subscriptionInfo()->ownsActiveSubscription() )
  {
    // we can try to "restore" only recommended plan in test backend
    return;
  }

  if ( mNextResult == Interactive )
  {
#if defined (HAVE_WIDGETS)
    QMessageBox::information( nullptr, "TEST RESTORE", "Test restore individual plan" );
    emit transactionCreationSucceeded( createTestingTransaction( mIndividualPlan, "tier01", true ) );
    return;
#endif
  }
  emit transactionCreationSucceeded( createTestingTransaction( mIndividualPlan, "tier01", true ) );
}

QString TestingPurchasingBackend::subscriptionManageUrl()
{
  return mMerginApi->apiRoot() + "subscription";
}

QString TestingPurchasingBackend::subscriptionBillingUrl()
{
  return mMerginApi->apiRoot() + "billing";
}

QSharedPointer<TestingPurchasingTransaction> TestingPurchasingBackend::createTestingTransaction( QSharedPointer<PurchasingPlan> plan, const QString &data, bool restore )
{
  QString planMerginId;
  const int id = mMerginApi->subscriptionInfo()->subscriptionId();
  if ( id > 0 )
  {
    // this is an existing subscription
    planMerginId = QString::number( mMerginApi->subscriptionInfo()->subscriptionId() );
  }

  QString recept = planMerginId + "|" + data;

  PurchasingTransaction::TransactionType type;
  restore ? type = PurchasingTransaction::RestoreTransaction : type = PurchasingTransaction::PuchaseTransaction;

  QSharedPointer<TestingPurchasingTransaction> transaction( new TestingPurchasingTransaction( recept, type, plan ) );
  return transaction;
}
