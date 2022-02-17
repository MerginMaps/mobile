#include <QtTest/QtTest>
#include <QtCore/QObject>
#include <QSignalSpy>
#include <QObject>

#include "testpurchasing.h"
#include "merginapi.h"
#include "merginapistatus.h"
#include "merginuserauth.h"
#include "merginuserinfo.h"
#include "merginsubscriptioninfo.h"
#include "testutils.h"
#include "test/testingpurchasingbackend.h"

static const double FREE_STORAGE =  104857600.0; // 100 MB

static const char *TIER01_PLAN_ID = "test_mergin_tier_1_1";
static const double TIER01_STORAGE =  1073741824.0; // 1GB

static const char *TIER02_PLAN_ID = "test_mergin_tier_1_2";
static const double TIER02_STORAGE =  10737418240.0; // 10 GB

TestPurchasing::TestPurchasing( MerginApi *api, Purchasing *purchasing )
{
  mApi = api;
  Q_ASSERT( mApi );  // does not make sense to run without API

  mPurchasing = purchasing;
  Q_ASSERT( mPurchasing );

  mPurchasingBackend = qobject_cast<TestingPurchasingBackend * >( purchasing->backend() );
  Q_ASSERT( mPurchasingBackend );
}

void TestPurchasing::runPurchasingCommand( TestingPurchasingBackend::NextPurchaseResult result, const QString &planId, bool waitForUserInfo )
{
  mPurchasingBackend->setNextPurchaseResult( result );

  QSignalSpy spy0( mApi, &MerginApi::subscriptionInfoChanged );
  if ( waitForUserInfo )
  {
    QSignalSpy spy1( mApi, &MerginApi::userInfoChanged );
    mPurchasing->purchase( planId );
    QVERIFY( spy0.wait( TestUtils::LONG_REPLY ) );
    QCOMPARE( spy0.count(), 1 );
    QVERIFY( spy1.wait( TestUtils::LONG_REPLY ) );
  }
  else
  {
    mPurchasing->purchase( planId );
    QVERIFY( spy0.wait( TestUtils::LONG_REPLY ) );
    QCOMPARE( spy0.count(), 1 );
  }
}

void TestPurchasing::initTestCase()
{
  QString apiRoot, username, password;
  TestUtils::mergin_auth( mApi, apiRoot, username, password );

  QSignalSpy spy( mApi, &MerginApi::authChanged );
  mApi->authorize( username, password );
  QVERIFY( spy.wait( TestUtils::LONG_REPLY ) );
  QCOMPARE( spy.count(), 1 );

  QSignalSpy spy2( mApi, &MerginApi::subscriptionInfoChanged );
  mApi->getSubscriptionInfo();
  QVERIFY( spy2.wait( TestUtils::LONG_REPLY ) );
  QCOMPARE( spy2.count(), 1 );

  // verify we have test or none subscription
  MerginSubscriptionType::SubscriptionType subscriptionType = mApi->subscriptionInfo()->planProvider();
  if ( subscriptionType == MerginSubscriptionType::TestSubscriptionType )
  {
    // unsubscribe to have always the same start position
    runPurchasingCommand( TestingPurchasingBackend::NonInteractiveSimulateImmediatelyCancelSubscription, mApi->subscriptionInfo()->planProductId() );
    QCOMPARE( mApi->subscriptionInfo()->ownsActiveSubscription(), false );
    QCOMPARE( mApi->subscriptionInfo()->subscriptionStatus(), MerginSubscriptionStatus::CanceledSubscription );
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
  runPurchasingCommand( TestingPurchasingBackend::NonInteractiveBuyIndividualPlan, TIER01_PLAN_ID, true );

  QCOMPARE( mApi->subscriptionInfo()->planProductId(), TIER01_PLAN_ID );
  QCOMPARE( mApi->userInfo()->storageLimit(), TIER01_STORAGE );
  QCOMPARE( mApi->subscriptionInfo()->ownsActiveSubscription(), true );
  QCOMPARE( mApi->subscriptionInfo()->subscriptionStatus(), MerginSubscriptionStatus::ValidSubscription );
  QCOMPARE( mApi->subscriptionInfo()->planProvider(), MerginSubscriptionType::TestSubscriptionType );
}

void TestPurchasing::testUserBuyTier12()
{
  runPurchasingCommand( TestingPurchasingBackend::NonInteractiveBuyIndividualPlan, TIER01_PLAN_ID );
  runPurchasingCommand( TestingPurchasingBackend::NonInteractiveBuyProfessionalPlan, TIER02_PLAN_ID, true );

  QCOMPARE( mApi->subscriptionInfo()->planProductId(), TIER02_PLAN_ID );
  QCOMPARE( mApi->userInfo()->storageLimit(), TIER02_STORAGE );
  QCOMPARE( mApi->subscriptionInfo()->ownsActiveSubscription(), true );
  QCOMPARE( mApi->subscriptionInfo()->subscriptionStatus(), MerginSubscriptionStatus::ValidSubscription );
  QCOMPARE( mApi->subscriptionInfo()->planProvider(), MerginSubscriptionType::TestSubscriptionType );
}

void TestPurchasing::testUserUnsubscribed()
{
  runPurchasingCommand( TestingPurchasingBackend::NonInteractiveBuyIndividualPlan, TIER01_PLAN_ID );
  runPurchasingCommand( TestingPurchasingBackend::NonInteractiveSimulateUnsubscribed, TIER01_PLAN_ID );
  QCOMPARE( mApi->subscriptionInfo()->planProductId(), TIER01_PLAN_ID );
  QCOMPARE( mApi->userInfo()->storageLimit(), TIER01_STORAGE );
  QCOMPARE( mApi->subscriptionInfo()->ownsActiveSubscription(), true );
  QCOMPARE( mApi->subscriptionInfo()->subscriptionStatus(), MerginSubscriptionStatus::SubscriptionUnsubscribed );
  QCOMPARE( mApi->subscriptionInfo()->planProvider(), MerginSubscriptionType::TestSubscriptionType );
}

void TestPurchasing::testUserInGracePeriod()
{
  runPurchasingCommand( TestingPurchasingBackend::NonInteractiveBuyIndividualPlan, TIER01_PLAN_ID );
  runPurchasingCommand( TestingPurchasingBackend::NonInteractiveSimulateGracePeriod, TIER01_PLAN_ID );
  QCOMPARE( mApi->subscriptionInfo()->planProductId(), TIER01_PLAN_ID );
  QCOMPARE( mApi->userInfo()->storageLimit(), TIER01_STORAGE );
  QCOMPARE( mApi->subscriptionInfo()->ownsActiveSubscription(), true );
  QCOMPARE( mApi->subscriptionInfo()->subscriptionStatus(), MerginSubscriptionStatus::SubscriptionInGracePeriod );
  QCOMPARE( mApi->subscriptionInfo()->planProvider(), MerginSubscriptionType::TestSubscriptionType );
}

void TestPurchasing::testUserCancelledSubscription()
{
  runPurchasingCommand( TestingPurchasingBackend::NonInteractiveBuyIndividualPlan, TIER01_PLAN_ID );
  runPurchasingCommand( TestingPurchasingBackend::NonInteractiveSimulateImmediatelyCancelSubscription, TIER01_PLAN_ID, true );

  QCOMPARE( mApi->subscriptionInfo()->planProductId(), "" );
  QCOMPARE( mApi->userInfo()->storageLimit(), FREE_STORAGE );
  QCOMPARE( mApi->subscriptionInfo()->ownsActiveSubscription(), false );
  QCOMPARE( mApi->subscriptionInfo()->subscriptionStatus(), MerginSubscriptionStatus::CanceledSubscription );
  QCOMPARE( mApi->subscriptionInfo()->planProvider(), MerginSubscriptionType::NoneSubscriptionType );
}

void TestPurchasing::testUserCancelledTransaction()
{
  int oldStatus = mApi->subscriptionInfo()->subscriptionStatus();
  mPurchasingBackend->setNextPurchaseResult( TestingPurchasingBackend::NonInteractiveUserCancelled );

  QSignalSpy spy0( mPurchasingBackend, &PurchasingBackend::transactionCreationFailed );
  mPurchasing->purchase( TIER01_PLAN_ID );
  // immediate action without server for testbackend
  QCOMPARE( spy0.count(), 1 );

  QCOMPARE( mApi->subscriptionInfo()->subscriptionStatus(), oldStatus );
}

void TestPurchasing::testUserSendsBadReceipt()
{
  int oldStatus = mApi->subscriptionInfo()->subscriptionStatus();
  mPurchasingBackend->setNextPurchaseResult( TestingPurchasingBackend::NonInteractiveBadReceipt );

  QSignalSpy spy0( mApi, &MerginApi::networkErrorOccurred );
  mPurchasing->purchase( TIER01_PLAN_ID );
  QVERIFY( spy0.wait( TestUtils::LONG_REPLY ) );
  QCOMPARE( spy0.count(), 1 );

  QCOMPARE( mApi->subscriptionInfo()->subscriptionStatus(), oldStatus );
}

void TestPurchasing::testUserRestore()
{
  QSignalSpy spy0( mApi->subscriptionInfo(), &MerginSubscriptionInfo::subscriptionInfoChanged );
  QSignalSpy spy1( mApi->userInfo(), &MerginUserInfo::userInfoChanged );
  mPurchasing->restore();
  QVERIFY( spy0.wait( TestUtils::LONG_REPLY ) );
  QCOMPARE( spy0.count(), 1 );
  QVERIFY( spy1.wait( TestUtils::LONG_REPLY ) );
  QCOMPARE( spy1.count(), 1 );

  QCOMPARE( mApi->subscriptionInfo()->planProductId(), TIER01_PLAN_ID );
  QCOMPARE( mApi->userInfo()->storageLimit(), TIER01_STORAGE );
  QCOMPARE( mApi->subscriptionInfo()->subscriptionStatus(), MerginSubscriptionStatus::ValidSubscription );
  QCOMPARE( mApi->subscriptionInfo()->planProvider(), MerginSubscriptionType::TestSubscriptionType );
}
