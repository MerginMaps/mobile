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

#include "qgsproject.h"
#include "filtercontroller.h"

class MerginApi;
class QgsVectorLayer;

namespace TestUtils
{
  const int SHORT_REPLY = 5000;
  const int LONG_REPLY = 90000;
  const int TEST_WORKSPACE_STORAGE_SIZE = 1 * 1024 * 1024 * 1024; // 1 GB
  const int TEST_WORKSPACE_PROJECT_NUMBER = 100;

  //! authorize user and select the active workspace
  void authorizeUser( MerginApi *api, const QString &username, const QString &password );

  //! select the first workspace as active workspace
  void selectFirstWorkspace( MerginApi *api, QString &workspace );

  /**
   *  Get TEST user credentials from env variables
   *  - TEST_MERGIN_URL
   *  - TEST_API_USERNAME
   *  - TEST_API_PASSWORD
   *  And if not found, generate a test run-specific user
   */
  void merginGetAuthCredentials( MerginApi *api, QString &apiRoot, QString &username, QString &password );

  //! Whether we need to auth again
  bool needsToAuthorizeAgain( MerginApi *api, const QString &username );
  void generateRandomUser( MerginApi *api, QString &username, QString &password );

  QString generateEmail();
  QString generatePassword();
  QString testDataDir();

  QgsProject *loadPlanesTestProject();

  /**
   * Creates an in-memory no-geometry lookup layer with \a count features.
   * Fields: key (integer), label (string).
   */
  QgsVectorLayer *createVRLookupLayer( int count );

  /**
   * Creates an in-memory no-geometry lookup layer for ordering tests.
   * Features are inserted in an order that is neither key-sorted nor label-sorted:
   *   key=3 label="Gamma", key=1 label="Alpha", key=4 label="Beta", key=2 label="Delta"
   */
  QgsVectorLayer *createVROrderingLayer();

  /**
   * Generates files and folders in rootPath based on json structure.
   * \param structure is a json instance, each object is considered as folder. Each folder can have a key named "files"
   * which is an array of files to be created.
   * \see TestCoreUtils::testFindUniquePath for usage
   * Returns true if files were successfully created
   */
  bool generateProjectFolder( const QString &rootPath, const QJsonDocument &structure );

  void testLayerHasGeometry();
  void testLayerVisible();
  void testIsPositionTrackingLayer();
  void testMapLayerFromName();
  void testIsValidUrl();

  bool testExifPositionMetadataExists( const QString &imageSource );

  //! Creates an in-memory layer with a single field of the given type and registers it in QgsProject::instance()
  //! fieldType is the QGIS memory-provider type string: "datetime", "date", "string", "integer", "double", "bool", etc.
  QgsVectorLayer *createFilterTestLayer( const QString &fieldName,
                                         const QString &fieldType,
                                         const QString &layerName = QStringLiteral( "FilterTestLayer" ) );

  //! Appends a single feature to layer via the data provider; value is stored in the named field. Returns false if addFeatures() fails.
  bool addFeatureToLayer( QgsVectorLayer *layer, const QString &fieldName, const QVariant &value );

  //! Writes a single-filter config into the project and loads it; returns the assigned filterId
  QString setupControllerWithFilter( FilterController *controller,
                                     FieldFilter::FilterType filterType,
                                     const QString &layerId,
                                     const QString &fieldName,
                                     const QString &sqlExpression );
}

#define COMPARENEAR(actual, expected, epsilon) \
  do {\
    if (!QTest::compare_helper((qAbs((actual) - (expected)) <= (epsilon)), \
    QString{"Compared values are not the same in respect to epsilon %1"} \
  .arg(epsilon).toLocal8Bit().constData(), \
  QTest::toString(actual), \
  QTest::toString(expected), \
  #actual, #expected, __FILE__, __LINE__)) \
    return;\
  } while (false)

#endif // TESTUTILS_H
