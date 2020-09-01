#include <QtTest/QtTest>
#include <QtCore/QObject>
#include <QSignalSpy>
#include <QObject>

#include "testpurchasing.h"
#include "merginapi.h"
#include "merginapistatus.h"
#include "merginuserauth.h"
#include "merginuserinfo.h"
#include "testutils.h"
#include "test/testingpurchasingbackend.h"

static const double FREE_STORAGE =  122312.0;

static const char *TIER01_PLAN_ID = "test_mergin_tier_1_1";
static const double TIER01_STORAGE =  104857600.0;

static const char *TIER02_PLAN_ID = "test_mergin_tier_1_2";
static const double TIER02_STORAGE =  10737418240.0;

TestPurchasing::TestPurchasing( MerginApi *api, Purchasing *purchasing )
{
  mApi = api;
  Q_ASSERT( mApi );  // does not make sense to run without API

  mPurchasing = purchasing;
  Q_ASSERT( mPurchasing );

  mPurchasingBackend = qobject_cast<TestingPurchasingBackend * >( purchasing->backend() );
  Q_ASSERT( mPurchasingBackend );
}

void TestPurchasing::runPurchasingCommand( TestingPurchasingBackend::NextPurchaseResult result, const QString &planId )
{
  mPurchasingBackend->setNextPurchaseResult( result );

  QSignalSpy spy0( mApi, &MerginApi::userInfoChanged );
  mPurchasing->purchase( planId );
  QVERIFY( spy0.wait( TestUtils::LONG_REPLY ) );
  QCOMPARE( spy0.count(), 1 );
}

void TestPurchasing::initTestCase()
{
  QString apiRoot, username, password;
  TestUtils::mergin_auth( apiRoot, username, password );

  mApi->setApiRoot( apiRoot );
  QSignalSpy spy( mApi, &MerginApi::authChanged );
  mApi->authorize( username, password );
  QVERIFY( spy.wait( TestUtils::LONG_REPLY ) );
  QCOMPARE( spy.count(), 1 );

  // verify we have test or none subscription
  MerginSubscriptionType::SubscriptionType subscriptionType = mApi->userInfo()->planProvider();
  if ( subscriptionType == MerginSubscriptionType::TestSubscriptionType )
  {
    // unsubscribe to have always the same start position
    runPurchasingCommand( TestingPurchasingBackend::NonInteractiveSimulateImmediatelyCancelSubscription, mApi->userInfo()->planProductId() );
    QCOMPARE( mApi->userInfo()->ownsActiveSubscription(), false );
    QCOMPARE( mApi->userInfo()->subscriptionStatus(), MerginSubscriptionStatus::CanceledSubscription );
  }
  else
  {
    // if there is other type, it means that the test user on test.dev
    // was manually modified outside the automatic testing
    Q_ASSERT( subscriptionType == MerginSubscriptionType::NoneSubscriptionType );
  }
}

void TestPurchasing::cleanupTestCase()
{
}

void TestPurchasing::testUserBuyTier01()
{
  runPurchasingCommand( TestingPurchasingBackend::NonInteractiveBuyTier01, TIER01_PLAN_ID );
  QCOMPARE( mApi->userInfo()->planProductId(), TIER01_PLAN_ID );
  QCOMPARE( mApi->userInfo()->storageLimit(), TIER01_STORAGE );
  QCOMPARE( mApi->userInfo()->ownsActiveSubscription(), true );
  QCOMPARE( mApi->userInfo()->subscriptionStatus(), MerginSubscriptionStatus::ValidSubscription );
  QCOMPARE( mApi->userInfo()->planProvider(), MerginSubscriptionType::TestSubscriptionType );
}

void TestPurchasing::testUserBuyTier12()
{
  runPurchasingCommand( TestingPurchasingBackend::NonInteractiveBuyTier01, TIER01_PLAN_ID );
  runPurchasingCommand( TestingPurchasingBackend::NonInteractiveBuyTier12, TIER02_PLAN_ID );
  QCOMPARE( mApi->userInfo()->planProductId(), TIER02_PLAN_ID );
  QCOMPARE( mApi->userInfo()->storageLimit(), TIER02_STORAGE );
  QCOMPARE( mApi->userInfo()->ownsActiveSubscription(), true );
  QCOMPARE( mApi->userInfo()->subscriptionStatus(), MerginSubscriptionStatus::ValidSubscription );
  QCOMPARE( mApi->userInfo()->planProvider(), MerginSubscriptionType::TestSubscriptionType );
}

void TestPurchasing::testUserCancelledTransaction()
{
  int oldStatus = mApi->userInfo()->subscriptionStatus();
  runPurchasingCommand( TestingPurchasingBackend::NonInteractiveUserCancelled, TIER01_PLAN_ID );
  QCOMPARE( mApi->userInfo()->planProductId(), "" );
  QCOMPARE( mApi->userInfo()->storageLimit(), FREE_STORAGE );
  QCOMPARE( mApi->userInfo()->ownsActiveSubscription(), false );
  QCOMPARE( mApi->userInfo()->subscriptionStatus(), oldStatus );
}
void TestPurchasing::testUserUnsubscribed()
{
  runPurchasingCommand( TestingPurchasingBackend::NonInteractiveBuyTier01, TIER01_PLAN_ID );
  runPurchasingCommand( TestingPurchasingBackend::NonInteractiveSimulateUnsubscribed, TIER01_PLAN_ID );
  QCOMPARE( mApi->userInfo()->planProductId(), TIER01_PLAN_ID );
  QCOMPARE( mApi->userInfo()->storageLimit(), TIER01_STORAGE );
  QCOMPARE( mApi->userInfo()->ownsActiveSubscription(), true );
  QCOMPARE( mApi->userInfo()->subscriptionStatus(), MerginSubscriptionStatus::SubscriptionUnsubscribed );
  QCOMPARE( mApi->userInfo()->planProvider(), MerginSubscriptionType::TestSubscriptionType );
}

void TestPurchasing::testUserInGracePeriod()
{
  runPurchasingCommand( TestingPurchasingBackend::NonInteractiveBuyTier01, TIER01_PLAN_ID );
  runPurchasingCommand( TestingPurchasingBackend::NonInteractiveSimulateGracePeriod, TIER01_PLAN_ID );
  QCOMPARE( mApi->userInfo()->planProductId(), TIER01_PLAN_ID );
  QCOMPARE( mApi->userInfo()->storageLimit(), TIER01_STORAGE );
  QCOMPARE( mApi->userInfo()->ownsActiveSubscription(), true );
  QCOMPARE( mApi->userInfo()->subscriptionStatus(), MerginSubscriptionStatus::SubscriptionInGracePeriod );
  QCOMPARE( mApi->userInfo()->planProvider(), MerginSubscriptionType::TestSubscriptionType );
}

void TestPurchasing::testUserCancelledSubscription()
{
  QCOMPARE( mApi->userInfo()->planProductId(), "" );
  QCOMPARE( mApi->userInfo()->storageLimit(), FREE_STORAGE );
  QCOMPARE( mApi->userInfo()->ownsActiveSubscription(), false );
  QCOMPARE( mApi->userInfo()->subscriptionStatus(), MerginSubscriptionStatus::CanceledSubscription );
  QCOMPARE( mApi->userInfo()->planProvider(), MerginSubscriptionType::NoneSubscriptionType );
}

void TestPurchasing::testUserSendsBadReceipt()
{
  int oldStatus = mApi->userInfo()->subscriptionStatus();
  runPurchasingCommand( TestingPurchasingBackend::NonInteractiveBadReceipt, TIER01_PLAN_ID );
  QCOMPARE( mApi->userInfo()->subscriptionStatus(), oldStatus );
}

void TestPurchasing::testUserRestore()
{
  mPurchasing->restore();
  QCOMPARE( mApi->userInfo()->planProductId(), TIER01_PLAN_ID );
  QCOMPARE( mApi->userInfo()->storageLimit(), TIER01_STORAGE );
  QCOMPARE( mApi->userInfo()->subscriptionStatus(), MerginSubscriptionStatus::ValidSubscription );
  QCOMPARE( mApi->userInfo()->planProvider(), MerginSubscriptionType::TestSubscriptionType );
}
