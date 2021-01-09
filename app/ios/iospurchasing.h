/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef IOSPURCHASING_H
#define IOSPURCHASING_H

#include <QtGlobal>
#include <QObject>
#include <QMap>
#include <QSet>

#include "purchasing.h"

class IosPurchasingBackend;
Q_FORWARD_DECLARE_OBJC_CLASS( InAppPurchaseManager );
Q_FORWARD_DECLARE_OBJC_CLASS( SKProduct );
Q_FORWARD_DECLARE_OBJC_CLASS( SKPaymentTransaction );

/**
 * Wrapper around SKProduct class
 */
class IosPurchasingPlan: public PurchasingPlan
{
    Q_OBJECT
  public:
    SKProduct *nativeProduct() const;
    IosPurchasingBackend *backend() const;
    void setNativeProduct( SKProduct *nativeProduct );

  private:
    SKProduct *mNativeProduct; //this is objective-C instance
};

/**
 * Wrapper around SKPaymentTransaction class
 */
class IosPurchasingTransaction: public PurchasingTransaction
{
    Q_OBJECT
  public:
    enum TransactionStatus
    {
      Unknown,
      PurchaseApproved,
      PurchaseFailed,
      PurchaseRestored
    };
    Q_ENUM( TransactionStatus )

    IosPurchasingTransaction( SKPaymentTransaction *transaction,
                              TransactionStatus status,
                              QSharedPointer<PurchasingPlan> plan
                            );

    IosPurchasingPlan *iosPlan() const;

    SKPaymentTransaction *nativeTransaction() const;
    TransactionStatus status() const;

    QString receipt() const override;
    MerginSubscriptionType::SubscriptionType provider() const override { return MerginSubscriptionType::AppleSubscriptionType; }

    /**
     * Localized ios error message from the native framework,
     * populated when TransactionStatus::PurchaseFailed
     */
    QString errMsg() const;

    void finalizeTransaction() override;

  private:
    TransactionType status2type( TransactionStatus status );

    TransactionStatus mStatus;
    SKPaymentTransaction *mNativeTransaction; //objective-C instance
};

/**
 * Backend encapsulating the ios purchasing methods.
 * Contains InAppPurchaseManager,
 * which is Objective-C implementation of
 * KProductsRequestDelegate and SKPaymentTransactionObserver
 */
class IosPurchasingBackend: public PurchasingBackend
{
    Q_OBJECT
  public:
    IosPurchasingBackend();
    ~IosPurchasingBackend() override;

    void init() override;
    QSharedPointer<PurchasingPlan> createPlan() override {return QSharedPointer<PurchasingPlan>( new IosPurchasingPlan() );}
    void registerPlan( QSharedPointer<PurchasingPlan> plan ) override;

    void createTransaction( QSharedPointer<PurchasingPlan> plan ) override;
    void restore() override;

    QString subscriptionManageUrl() override {return "https://apps.apple.com/account/subscriptions"; }
    QString subscriptionBillingUrl() override {return "https://apps.apple.com/account/billing"; }
    MerginSubscriptionType::SubscriptionType provider() const override { return MerginSubscriptionType::AppleSubscriptionType; }
    bool userCanMakePayments() const override;
    bool hasManageSubscriptionCapability() const override { return false; }
    QString getLocalizedPrice( const QString &planId ) const override;

    QSharedPointer<IosPurchasingPlan> findRegisteredPlan( const QString &productId ) const;
    QSharedPointer<IosPurchasingPlan> findPendingPlan( const QString &productId ) const;
    void processTransaction( QSharedPointer<IosPurchasingTransaction> transaction );

  private:
    InAppPurchaseManager *mManager; //this is objective-C instance
};

#endif // IOSPURCHASING_H
