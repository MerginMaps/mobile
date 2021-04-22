/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef TESTPURCHASING_H
#define TESTPURCHASING_H

#include <QObject>
#include "testingpurchasingbackend.h"

class MerginApi;
class TestingPurchasingBackend;
class Purchasing;

class TestPurchasing: public QObject
{
    Q_OBJECT
  public:
    explicit TestPurchasing( MerginApi *api, Purchasing *purchasing );
    ~TestPurchasing() = default;

  private slots:
    void initTestCase();
    void cleanupTestCase();

    void testUserBuyTier01();
    void testUserBuyTier12();
    void testUserCancelledTransaction();
    void testUserUnsubscribed();
    void testUserInGracePeriod();
    void testUserCancelledSubscription();
    void testUserSendsBadReceipt();
    void testUserRestore();

  private:
    /**
     * @brief runPurchasingCommand Test util function to invoke purchasing function and wait for the replies.
     * @param result
     * @param planId ID of the plan that is going to be purchased.
     * @param waitForUserInfo True, if current user planID will be changed after executing the command.
     *        The change will trigger an extra userInfo request to update storage.
     */
    void runPurchasingCommand( TestingPurchasingBackend::NextPurchaseResult result, const QString &planId, bool waitForUserInfo = false );

    MerginApi *mApi = nullptr;
    Purchasing *mPurchasing = nullptr;
    TestingPurchasingBackend *mPurchasingBackend = nullptr;
};

# endif // TESTPURCHASING_H
