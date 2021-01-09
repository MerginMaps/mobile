/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef TESTINGPURCHASINGBACKEND_H
#define TESTINGPURCHASINGBACKEND_H

#include "purchasing.h"

#include <QObject>
#include <QByteArray>

#if defined (HAVE_WIDGETS)
#include <QMessageBox>
#endif

class MerginApi;

/**
 * The receipt send  to Mergin can trigger either some action to active
 * subscription or change the subscription.
 *
 * Receipt is created in this form:
 * "TEST<plan_id>|<keyword>",
 * - where plan_id can be set to -1 (means server please find out my plan_id)
 * - and keyword is one of
 *    tier01       (free plan active, buy recommended plan)
 *    tier12       (recommended active, buy upgrade)
 *    cancel       (got refund from provider)
 *    grace        (grace period entered)
 *    unsubscribed (user chosen not to extend subscription when this one ends)
 */
class TestingPurchasingTransaction: public PurchasingTransaction
{
  public:
    TestingPurchasingTransaction( QString receipt, TransactionType type, QSharedPointer<PurchasingPlan> plan );
    QString receipt() const override;
    MerginSubscriptionType::SubscriptionType provider() const override {return MerginSubscriptionType::TestSubscriptionType; }

    void finalizeTransaction() override {}

  private:
    QString mReceipt;
};

/**
 * Backend usefull for
 *    GUI testing (select NextPurchaseResult::Interactive)
 *    Automated tests (select NextPurchaseResult to simulate what next createTransaction() will simulate)
 *
 * See TestingPurchasingTransaction for description of various transactions.
 *
 * For the moment, the testing backend does not append plan id to receipt and let the server
 * decode
 */
class TestingPurchasingBackend: public PurchasingBackend
{
    Q_OBJECT
  public:
    TestingPurchasingBackend( MerginApi *api );

    enum NextPurchaseResult
    {
      Interactive,
      NonInteractiveBuyIndividualPlan,
      NonInteractiveBuyProfessionalPlan,
      NonInteractiveSimulateImmediatelyCancelSubscription,
      NonInteractiveSimulateGracePeriod,
      NonInteractiveSimulateUnsubscribed,
      NonInteractiveUserCancelled,
      NonInteractiveBadReceipt
    };
    void setNextPurchaseResult( const NextPurchaseResult expected );

    void init() override {}
    QSharedPointer<PurchasingPlan> createPlan( ) override {return QSharedPointer<PurchasingPlan>( new PurchasingPlan ); }
    void registerPlan( QSharedPointer<PurchasingPlan> plan ) override;
    void createTransaction( QSharedPointer<PurchasingPlan> plan ) override;
    void restore() override;
    QString subscriptionManageUrl() override;
    QString subscriptionBillingUrl() override;
    MerginSubscriptionType::SubscriptionType provider() const override { return MerginSubscriptionType::TestSubscriptionType; }
    bool userCanMakePayments() const override { return true; }
    bool hasManageSubscriptionCapability() const override { return true; }
    QString getLocalizedPrice( const QString & ) const override { return ""; }

    void setMerginApi( const QString &url );

  private:
    QSharedPointer<TestingPurchasingTransaction> createTestingTransaction( QSharedPointer<PurchasingPlan> plan, const QString &data, bool restore = false );
    NextPurchaseResult mNextResult = NextPurchaseResult::Interactive;

    MerginApi *mMerginApi = nullptr;
    QSharedPointer<PurchasingPlan> mIndividualPlan;
};

#endif // TESTINGPURCHASINGBACKEND_H
