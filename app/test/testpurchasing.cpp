#include <QtTest/QtTest>
#include <QtCore/QObject>
#include <QSignalSpy>
#include <QObject>

#include "testpurchasing.h"
#include "merginapi.h"
#include "testutils.h"
#include "test/testingpurchasingbackend.h"

TestPurchasing::TestPurchasing( MerginApi *api, Purchasing *purchasing )
{
  mApi = api;
  Q_ASSERT( mApi );  // does not make sense to run without API

  mPurchasing = purchasing;
  Q_ASSERT( mPurchasing );

  mPurchasingBackend = qobject_cast<TestingPurchasingBackend * >( purchasing->backend() );
  Q_ASSERT( mPurchasingBackend );
}

void TestPurchasing::initTestCase()
{
  QString apiRoot, username, password;
  TestUtils::mergin_auth( apiRoot, username, password );

  mApi->setApiRoot( apiRoot );
  QSignalSpy spy( mApi, &MerginApi::authChanged );
  mApi->authorize( username, password );
  Q_ASSERT( spy.wait( TestUtils::LONG_REPLY ) );
  QCOMPARE( spy.count(), 1 );
}

void TestPurchasing::cleanupTestCase()
{
}

void TestPurchasing::testUserCancelledTransaction()
{
  /*
  mPurchasingBackend->setNextPurchaseResult( TestingPurchasingBackend::NonInteractiveUserCancelled );

  QSignalSpy spy0( mPurchasing, &Purchasing::transactionFailed );
  mPurchasing->purchase( QString() );
  // cancel is emitted immediately
  QCOMPARE( spy0.count(), 1 );
  */
}
