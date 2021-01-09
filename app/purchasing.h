/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef PURCHASING_H
#define PURCHASING_H

#include <QObject>
#include <QVector>
#include <QString>
#include <QMap>
#include <QList>
#include <QByteArray>
#include <QSharedPointer>
#include <QJsonObject>
#include <memory>

#include "merginsubscriptiontype.h"

class MerginApi;
class Purchasing;

/**
 * Purchasing Plan is a plan for subscription
 *
 * Each user can have assigned one subscription plan on Mergin.
 * Plan defines the maximum storage, billing price, period and so.
 */
class PurchasingPlan: public QObject
{
    Q_OBJECT
    Q_PROPERTY( QString alias READ alias NOTIFY planChanged )
    Q_PROPERTY( QString id READ id NOTIFY planChanged )
    Q_PROPERTY( QString period READ period NOTIFY planChanged )
    Q_PROPERTY( QString price READ price NOTIFY planChanged )
    Q_PROPERTY( QString storage READ storage NOTIFY planChanged )

  public:
    void clear();

    QString alias() const;
    void setAlias( const QString &alias );

    QString id() const;
    void setId( const QString &id );

    QString period() const;
    void setPeriod( const QString &period );

    QString price() const;
    void setPrice( const QString &price );

    bool isIndividualPlan() const;
    bool isProfessionalPlan() const;

    void setFromJson( QJsonObject docObj );

    QString storage() const;

    void setStorage( const QString &storage );

    Purchasing *purchasing() const;
    void setPurchasing( Purchasing *purchasing );

  signals:
    void planChanged();

  private:
    QString mAlias;
    QString mId;
    QString mPeriod;
    QString mPrice;
    QString mStorage;

    Purchasing *mPurchasing = nullptr;
    bool mIsProfessional = false;
};

/**
 * The Purchasing Transaction represent one payment transaction
 *
 * The puchasing transaction is created when user pays the plan
 * and is verified on the Mergin server.
 */
class PurchasingTransaction : public QObject
{
    Q_OBJECT
  public:
    enum TransactionType
    {
      RestoreTransaction,
      PuchaseTransaction
    };
    Q_ENUM( TransactionType )

    PurchasingTransaction( TransactionType type, QSharedPointer<PurchasingPlan> plan );

    void virtual finalizeTransaction() = 0;

    //! Transaction receipt, e.g. apple base64 receipt
    virtual QString receipt() const = 0;

    //! Transaction provider, either apple, stripe or test
    virtual MerginSubscriptionType::SubscriptionType provider() const = 0;

    PurchasingPlan *plan() const;
    TransactionType type() const;

  public slots:
    void verificationFinished();

  private:
    QSharedPointer<PurchasingPlan> mPlan;
    const TransactionType mType;
};


class PurchasingBackend: public QObject
{
    Q_OBJECT
  public:
    /**
     * Initializes the service
     * Called once on very startup of the QApplication
     */
    virtual void init() = 0;

    /**
     * Creates an instance of the purchasing plan from JSON.
     * Registration is done later in registerPlan
     */
    virtual QSharedPointer<PurchasingPlan> createPlan( ) = 0;

    /**
     * Register the products in the native SDK.
     */
    virtual void registerPlan( QSharedPointer<PurchasingPlan> plan ) = 0;

    /**
     * Buys the selected product.
     */
    virtual void createTransaction( QSharedPointer<PurchasingPlan> plan ) = 0;

    /**
     * Restore purchases with recipes stored on this device
     */
    virtual void restore() = 0;

    /**
     * Returns whether the backend can manage (upgrade/downgrade/...)
     * the subscriptions or that has to be done on external
     * website (mergin/apple/...)
     */
    virtual bool hasManageSubscriptionCapability() const = 0;

    /**
     * URL to show user when he wants so upgrade/downgrade
     * existing (paid) subscriptions
     *
     * non-empty URL only when hasManageSubscriptionCapability() == false
     */
    virtual QString subscriptionManageUrl() = 0;

    /**
     * URL to show user when he wants so
     * change billing details (e.g. credit card number)
     *
     * non-empty URL only when hasManageSubscriptionCapability() == false
     */
    virtual QString subscriptionBillingUrl() = 0;

    /**
     * Name of the billing service for Mergin API
     * (e.g. stripe, apple, google, ...)
     */
    virtual MerginSubscriptionType::SubscriptionType provider() const = 0;

    /**
     * Whether user can make purchases on the device
     */
    virtual bool userCanMakePayments() const = 0;

    void setPurchasing( Purchasing *purchasing ) {mPurchasing = purchasing;}
    Purchasing *purchasing() const {return mPurchasing;}

    //! Returns localised prize of plan, empty string if cannot be fetched
    virtual QString getLocalizedPrice( const QString &planId ) const = 0;

  signals:
    void transactionCreationFailed( );
    void transactionCreationSucceeded( QSharedPointer<PurchasingTransaction> transaction );

    void planRegistrationFailed( const QString &id );
    void planRegistrationSucceeded( const QString &id );

  private:
    Purchasing *mPurchasing = nullptr;
};

/**
 * The Purchasing class, responsible for in-app purchases
 *
 * The purchasing can be disabled or enabled based on these factors
 * 1. Mergin Server can be deployed with the flag that subscriptions are disabled
 * 2. Device (e.g. iPhone) can be in mode where purchases are not allowed
 * 3. The plans in the 3rd party store (e.g. AppStore) does not match the plans in Mergin Server
 * 4. The InputApp does not have backed to manage user plan on Mergin Server (e.g. stripe plans)
 *
 * When purchasing is enabled the workflow is as follows
 * 1. Fetches the plan details from Mergin to check which plans are available for user. One of the plan
 *    is reccomended and only this one is shown in the GUI
 * 2. The plans are then registered in the backend (e.g. with Apple's StoreKit)
 * 3. If the plans are correctly registered, the purchasing API is now in state to do payments/transactions
 *    Therefore user has 2 options:
 * 3.A) restore subscriptions (in case for example he paid the subscription but
 *      network error caused that it was not correctly processed on Mergin)
 * 3.B) buy new subscription (the reccomeneded one)
 * If there are some pending transactions on the device, these can create transactions even without the user
 * action.
 * 4. When user pays the plan, the backend creates the transaction. The transaction receipt is extracted
 *    and send to the Mergin. Mergin verifies the receipt and based on the data changes the subscription on
 *    the server.
 * 5. User is notified about successfull payment and the storage is increased.
 *
 * For managing existing subscriptions, user needs to use the official URLs (e.g. Apple iTunes subscription manager)
 *
 * For testing purposes and the development purposes on the Linux, we have also created a fake backend,
 * that can be used to simulate various subscription statuses/workflows on the Mergin. The testing backend can
 * only be used on development Mergin servers.
 *
 * For testing the in-app purchases in the 3rd party stores, check the documentation of the PurchasingBackend implementations.
 */
class Purchasing : public QObject
{
    Q_OBJECT

    Q_PROPERTY( PurchasingPlan *individualPlan READ individualPlan NOTIFY individualPlanChanged )
    Q_PROPERTY( PurchasingPlan *professionalPlan READ professionalPlan NOTIFY professionalPlanChanged )
    Q_PROPERTY( bool transactionPending READ transactionPending NOTIFY transactionPendingChanged )
    Q_PROPERTY( bool hasInAppPurchases READ hasInAppPurchases NOTIFY hasInAppPurchasesChanged )
    Q_PROPERTY( bool hasManageSubscriptionCapability READ hasManageSubscriptionCapability NOTIFY hasManageSubscriptionCapabilityChanged )
    Q_PROPERTY( QString subscriptionManageUrl READ subscriptionManageUrl NOTIFY subscriptionManageUrlChanged )
    Q_PROPERTY( QString subscriptionBillingUrl READ subscriptionBillingUrl NOTIFY subscriptionBillingUrlChanged )

  public:
    explicit Purchasing( MerginApi *merginApi, QObject *parent = nullptr );

    Q_INVOKABLE void purchase( const QString &planId );
    Q_INVOKABLE void restore();

    bool hasManageSubscriptionCapability() const;
    bool transactionPending() const;
    bool hasInAppPurchases() const;
    QString subscriptionManageUrl();
    QString subscriptionBillingUrl();
    PurchasingPlan *individualPlan() const;
    PurchasingPlan *professionalPlan() const;

    // Public function required only internally within the purchasing classes
  public:
    PurchasingBackend *backend() {return mBackend.get();}
    QSharedPointer<PurchasingPlan> registeredPlan( const QString &id ) const;
    QSharedPointer<PurchasingPlan> pendingPlan( const QString &id ) const;
    MerginApi *merginApi() const;
    int registeredPlansCount() const;

  signals:
    void transactionPendingChanged();
    void individualPlanChanged();
    void professionalPlanChanged();
    void hasInAppPurchasesChanged();
    void hasManageSubscriptionCapabilityChanged();
    void subscriptionManageUrlChanged();
    void subscriptionBillingUrlChanged();

  private slots:
    void onFetchPurchasingPlansFinished();

    void onPlanRegistrationFailed( const QString &id );
    void onPlanRegistrationSucceeded( const QString &id );

    void onTransactionCreationSucceeded( QSharedPointer<PurchasingTransaction> transaction );
    void onTransactionCreationFailed( );

    void onTransactionVerificationSucceeded( PurchasingTransaction *transaction );
    void onTransactionVerificationFailed( PurchasingTransaction *transaction );

    void onMerginServerStatusChanged();
    void onMerginServerChanged();
    void onMerginPlanProductIdChanged();
    void evaluateHasInAppPurchases();
    void onHasInAppPurchasesChanged();

  private:
    void createBackend();

    void clean();
    void fetchPurchasingPlans();
    void setTransactionCreationRequested( bool transactionCreationRequested );
    void setIndividualPlanId( const QString &PlanId );
    void setProfessionalPlanId( const QString &PlanId );
    void removePendingTransaction( PurchasingTransaction *transaction );

    void setHasInAppPurchases( bool hasInAppPurchases );
    void setHasManageSubscriptionCapability( bool hasManageSubscriptionCapability );
    void setSubscriptionManageUrl( const QString &subscriptionManageUrl );
    void setSubscriptionBillingUrl( const QString &subscriptionBillingUrl );

    void setDefaultUrls();

    void notify( const QString &msg );

    QMap<QString, QSharedPointer<PurchasingPlan> > mPlansWithPendingRegistration;
    QMap<QString, QSharedPointer<PurchasingPlan> > mRegisteredPlans;
    QString mIndividualPlanId;
    QString mProfessionalPlanId;

    bool mTransactionCreationRequested = false;
    QList<QSharedPointer<PurchasingTransaction>> mTransactionsWithPendingVerification;

    std::unique_ptr<PurchasingBackend> mBackend = nullptr;
    MerginApi *mMerginApi = nullptr;

    bool mHasInAppPurchases = false;
    bool mHasManageSubscriptionCapability;
    QString mSubscriptionManageUrl;
    QString mSubscriptionBillingUrl;

    friend class PurchasingTransaction;
};

#endif // PURCHASING_H
