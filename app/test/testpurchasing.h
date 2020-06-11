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

    void testUserCancelledTransaction();

  private:
    MerginApi *mApi = nullptr;
    Purchasing *mPurchasing = nullptr;
    TestingPurchasingBackend *mPurchasingBackend = nullptr;
};

# endif // TESTPURCHASING_H
