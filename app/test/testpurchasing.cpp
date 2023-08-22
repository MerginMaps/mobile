#include <QtTest/QtTest>
#include <QtCore/QObject>
#include <QSignalSpy>
#include <QObject>

#include "testpurchasing.h"
#include "merginapi.h"
#include "merginapistatus.h"
#include "merginuserauth.h"
#include "merginuserinfo.h"
#include "merginworkspaceinfo.h"
#include "merginsubscriptioninfo.h"
#include "testutils.h"
#include "test/testingpurchasingbackend.h"

TestPurchasing::TestPurchasing( MerginApi *api, Purchasing *purchasing )
{
  mApi = api;
  Q_ASSERT( mApi );  // does not make sense to run without API

  Q_ASSERT( purchasing );
  mPurchasing = purchasing;
}

void TestPurchasing::initTestCase()
{
  QString apiRoot, username, password;
  TestUtils::mergin_setup_auth( mApi, apiRoot, username, password );
  TestUtils::mergin_setup_pro_subscription( mApi, mPurchasing );
}

void TestPurchasing::cleanupTestCase()
{
}

void TestPurchasing::testUserBuyTier01()
{
  TestUtils::runPurchasingCommand( mApi, mPurchasing, TestingPurchasingBackend::NonInteractiveSimulateImmediatelyCancelSubscription, mApi->subscriptionInfo()->planProductId() );
  QCOMPARE( mApi->subscriptionInfo()->subscriptionStatus(), MerginSubscriptionStatus::CanceledSubscription );

  TestUtils::runPurchasingCommand( mApi, mPurchasing, TestingPurchasingBackend::NonInteractiveBuyIndividualPlan, TestUtils::TIER01_PLAN_ID );
  QCOMPARE( mApi->subscriptionInfo()->planProductId(), TestUtils::TIER01_PLAN_ID );
  QCOMPARE( mApi->workspaceInfo()->storageLimit(), TestUtils::TIER01_STORAGE );
  QCOMPARE( mApi->subscriptionInfo()->ownsActiveSubscription(), true );
  QCOMPARE( mApi->subscriptionInfo()->subscriptionStatus(), MerginSubscriptionStatus::ValidSubscription );
  QCOMPARE( mApi->subscriptionInfo()->planProvider(), MerginSubscriptionType::TestSubscriptionType );
}

void TestPurchasing::testUserBuyTier12()
{
  TestUtils::runPurchasingCommand( mApi, mPurchasing, TestingPurchasingBackend::NonInteractiveSimulateImmediatelyCancelSubscription, mApi->subscriptionInfo()->planProductId() );
  QCOMPARE( mApi->subscriptionInfo()->subscriptionStatus(), MerginSubscriptionStatus::CanceledSubscription );

  TestUtils::runPurchasingCommand( mApi, mPurchasing, TestingPurchasingBackend::NonInteractiveBuyIndividualPlan, TestUtils::TIER01_PLAN_ID );
  TestUtils::runPurchasingCommand( mApi, mPurchasing, TestingPurchasingBackend::NonInteractiveBuyProfessionalPlan, TestUtils::TIER02_PLAN_ID );

  QCOMPARE( mApi->subscriptionInfo()->planProductId(), TestUtils::TIER02_PLAN_ID );
  QCOMPARE( mApi->workspaceInfo()->storageLimit(), TestUtils::TIER02_STORAGE );
  QCOMPARE( mApi->subscriptionInfo()->ownsActiveSubscription(), true );
  QCOMPARE( mApi->subscriptionInfo()->subscriptionStatus(), MerginSubscriptionStatus::ValidSubscription );
  QCOMPARE( mApi->subscriptionInfo()->planProvider(), MerginSubscriptionType::TestSubscriptionType );
}

void TestPurchasing::testUserUnsubscribed()
{
  QSKIP( "Must be revisited when working with workspaces!" );

  TestUtils::runPurchasingCommand( mApi, mPurchasing, TestingPurchasingBackend::NonInteractiveSimulateImmediatelyCancelSubscription, mApi->subscriptionInfo()->planProductId() );
  QCOMPARE( mApi->subscriptionInfo()->subscriptionStatus(), MerginSubscriptionStatus::CanceledSubscription );

  TestUtils::runPurchasingCommand( mApi, mPurchasing, TestingPurchasingBackend::NonInteractiveBuyIndividualPlan, TestUtils::TIER01_PLAN_ID );

  TestUtils::runPurchasingCommand( mApi, mPurchasing, TestingPurchasingBackend::NonInteractiveSimulateUnsubscribed, TestUtils::TIER01_PLAN_ID, false );
  QCOMPARE( mApi->subscriptionInfo()->planProductId(), TestUtils::TIER01_PLAN_ID );
  QCOMPARE( mApi->workspaceInfo()->storageLimit(), TestUtils::TIER01_STORAGE );
  QCOMPARE( mApi->subscriptionInfo()->ownsActiveSubscription(), true );
  QCOMPARE( mApi->subscriptionInfo()->subscriptionStatus(), MerginSubscriptionStatus::SubscriptionUnsubscribed );
  QCOMPARE( mApi->subscriptionInfo()->planProvider(), MerginSubscriptionType::TestSubscriptionType );
}

void TestPurchasing::testUserInGracePeriod()
{
  QSKIP( "Must be revisited when working with workspaces!" );

  TestUtils::runPurchasingCommand( mApi, mPurchasing, TestingPurchasingBackend::NonInteractiveSimulateImmediatelyCancelSubscription, mApi->subscriptionInfo()->planProductId() );
  QCOMPARE( mApi->subscriptionInfo()->subscriptionStatus(), MerginSubscriptionStatus::CanceledSubscription );

  TestUtils::runPurchasingCommand( mApi, mPurchasing, TestingPurchasingBackend::NonInteractiveBuyIndividualPlan, TestUtils::TIER01_PLAN_ID );
  TestUtils::runPurchasingCommand( mApi, mPurchasing, TestingPurchasingBackend::NonInteractiveSimulateGracePeriod, TestUtils::TIER01_PLAN_ID );
  QCOMPARE( mApi->subscriptionInfo()->planProductId(), TestUtils::TIER01_PLAN_ID );
  QCOMPARE( mApi->workspaceInfo()->storageLimit(), TestUtils::TIER01_STORAGE );
  QCOMPARE( mApi->subscriptionInfo()->ownsActiveSubscription(), true );
  QCOMPARE( mApi->subscriptionInfo()->subscriptionStatus(), MerginSubscriptionStatus::SubscriptionInGracePeriod );
  QCOMPARE( mApi->subscriptionInfo()->planProvider(), MerginSubscriptionType::TestSubscriptionType );
}

void TestPurchasing::testUserCancelledSubscription()
{
  QSKIP( "Must be revisited when working with workspaces!" );

  TestUtils::runPurchasingCommand( mApi, mPurchasing, TestingPurchasingBackend::NonInteractiveSimulateImmediatelyCancelSubscription, mApi->subscriptionInfo()->planProductId() );

  QCOMPARE( mApi->subscriptionInfo()->planProductId(), "" );
  QCOMPARE( mApi->workspaceInfo()->storageLimit(), TestUtils::FREE_STORAGE );
  QCOMPARE( mApi->subscriptionInfo()->ownsActiveSubscription(), false );
  QCOMPARE( mApi->subscriptionInfo()->subscriptionStatus(), MerginSubscriptionStatus::CanceledSubscription );
  QCOMPARE( mApi->subscriptionInfo()->planProvider(), MerginSubscriptionType::NoneSubscriptionType );
}

void TestPurchasing::testUserCancelledTransaction()
{
  QSKIP( "Must be revisited when working with workspaces!" );

  Q_ASSERT( mPurchasing );
  TestingPurchasingBackend *purchasingBackend = qobject_cast<TestingPurchasingBackend * >( mPurchasing->backend() );
  Q_ASSERT( purchasingBackend );

  TestUtils::runPurchasingCommand( mApi, mPurchasing, TestingPurchasingBackend::NonInteractiveSimulateImmediatelyCancelSubscription, mApi->subscriptionInfo()->planProductId() );
  QCOMPARE( mApi->subscriptionInfo()->subscriptionStatus(), MerginSubscriptionStatus::CanceledSubscription );

  int oldStatus = mApi->subscriptionInfo()->subscriptionStatus();
  purchasingBackend->setNextPurchaseResult( TestingPurchasingBackend::NonInteractiveUserCancelled );

  QSignalSpy spy0( purchasingBackend, &PurchasingBackend::transactionCreationFailed );
  mPurchasing->purchase( TestUtils::TIER01_PLAN_ID );
  // immediate action without server for testbackend
  QCOMPARE( spy0.count(), 1 );

  QCOMPARE( mApi->subscriptionInfo()->subscriptionStatus(), oldStatus );
}

void TestPurchasing::testUserSendsBadReceipt()
{
  QSKIP( "Must be revisited when working with workspaces!" );

  Q_ASSERT( mPurchasing );
  TestingPurchasingBackend *purchasingBackend = qobject_cast<TestingPurchasingBackend * >( mPurchasing->backend() );
  Q_ASSERT( purchasingBackend );

  TestUtils::runPurchasingCommand( mApi, mPurchasing, TestingPurchasingBackend::NonInteractiveSimulateImmediatelyCancelSubscription, mApi->subscriptionInfo()->planProductId() );
  QCOMPARE( mApi->subscriptionInfo()->subscriptionStatus(), MerginSubscriptionStatus::CanceledSubscription );

  int oldStatus = mApi->subscriptionInfo()->subscriptionStatus();
  purchasingBackend->setNextPurchaseResult( TestingPurchasingBackend::NonInteractiveBadReceipt );

  QSignalSpy spy0( mApi, &MerginApi::networkErrorOccurred );
  mPurchasing->purchase( TestUtils::TIER01_PLAN_ID );
  QVERIFY( spy0.wait( TestUtils::LONG_REPLY ) );
  QCOMPARE( spy0.count(), 1 );

  QCOMPARE( mApi->subscriptionInfo()->subscriptionStatus(), oldStatus );
}

void TestPurchasing::testUserRestore()
{
  QSKIP( "Must be revisited when working with workspaces!" );

  TestUtils::runPurchasingCommand( mApi, mPurchasing, TestingPurchasingBackend::NonInteractiveSimulateImmediatelyCancelSubscription, mApi->subscriptionInfo()->planProductId() );
  QCOMPARE( mApi->subscriptionInfo()->subscriptionStatus(), MerginSubscriptionStatus::CanceledSubscription );

  QSignalSpy spy0( mApi->subscriptionInfo(), &MerginSubscriptionInfo::subscriptionInfoChanged );
  QSignalSpy spy1( mApi->userInfo(), &MerginUserInfo::userInfoChanged );
  mPurchasing->restore();
  QVERIFY( spy0.wait( TestUtils::LONG_REPLY ) );
  QCOMPARE( spy0.count(), 1 );
  QVERIFY( spy1.wait( TestUtils::LONG_REPLY ) );
  QCOMPARE( spy1.count(), 1 );

  QCOMPARE( mApi->subscriptionInfo()->planProductId(), TestUtils::TIER01_PLAN_ID );
  QCOMPARE( mApi->workspaceInfo()->storageLimit(), TestUtils::TIER01_STORAGE );
  QCOMPARE( mApi->subscriptionInfo()->subscriptionStatus(), MerginSubscriptionStatus::ValidSubscription );
  QCOMPARE( mApi->subscriptionInfo()->planProvider(), MerginSubscriptionType::TestSubscriptionType );
}
