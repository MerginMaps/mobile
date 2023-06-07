/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef TESTUTILS_H
#define TESTUTILS_H

#include <QString>
#include <qtestcase.h>

#include "inputconfig.h"
#include "testingpurchasingbackend.h"
#include "qgsproject.h"

class MerginApi;
class Purchasing;
class TestingPurchasingBackend;

namespace TestUtils
{
  const int SHORT_REPLY = 5000;
  const int LONG_REPLY = 90000;

  const double FREE_STORAGE =  104857600.0; // 100 MB

  const char *const TIER01_PLAN_ID = "test_mergin_tier_1_1";
  const double TIER01_STORAGE =  1073741824.0; // 1GB

  const char *const TIER02_PLAN_ID = "test_mergin_tier_1_2";
  const double TIER02_STORAGE =  10737418240.0; // 10 GB

  //! Use credentials from env variables if they are set, otherwise register new user and set its credentials to env var
  void mergin_setup_auth( MerginApi *api, QString &apiRoot, QString &username, QString &password );

  //! Setup professional plan for active workspace
  void mergin_setup_pro_subscription( MerginApi *api, TestingPurchasingBackend *purchasingBackend );

  QString generateUsername();
  QString generateEmail();
  QString generatePassword();

  QString testDataDir();

  QgsProject *loadPlanesTestProject();

  /**
   * Generates files and folders in rootPath based on json structure.
   * \param structure is a json instance, each object is considered as folder. Each folder can have a key named "files"
   * which is an array of files to be created.
   * \see TestCoreUtils::testFindUniquePath for usage
   * Returns true if files were successfully created
   */
  bool generateProjectFolder( const QString &rootPath, const QJsonDocument &structure );

  //! Test util function to invoke purchasing function and wait for the replies.
  void runPurchasingCommand( MerginApi *api, TestingPurchasingBackend *purchasingBackend, TestingPurchasingBackend::NextPurchaseResult result, const QString &planId, bool waitForWorkspaceInfoChanged = true );
}

#define COMPARENEAR(actual, expected, epsilon) \
  do {\
    if (!QTest::compare_helper((qAbs(actual - expected) <= epsilon), \
    QString{"Compared values are not the same in respect to epsilon %1"} \
  .arg(epsilon).toLocal8Bit().constData(), \
  QTest::toString(actual), \
  QTest::toString(expected), \
  #actual, #expected, __FILE__, __LINE__)) \
    return;\
  } while (false)

#endif // TESTUTILS_H
