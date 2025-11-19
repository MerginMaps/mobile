/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef TESTUTILSFUNCTIONS_H
#define TESTUTILSFUNCTIONS_H

#include <QObject>
#include "inpututils.h"

class TestUtilsFunctions: public QObject
{
    Q_OBJECT
  public:
    TestUtilsFunctions( InputUtils *utils );
    ~TestUtilsFunctions() = default;

  private slots:
    void testFormatDuration();
    void dump_screen_info();
    void screenUnitsToMeters();
    void transformedPointXY();
    void transformedPoint();
    void formatPoint();
    void testHtmlLink();
    void formatDistance();
    void loadIcon();
    void fileExists();
    void loadQmlComponent();
    void getRelativePath();
    void resolvePhotoPath();
    void resolveTargetDir();
    void testExtractPointFromFeature();
    void testStakeoutPathExtent();
    void testDistanceBetweenGpsAndFeature();
    void testAngleBetweenGpsAndFeature();
    void testIsPointLayerFeature();
    void testMapPointToGps();
    void testEquals();
    void testGeometryIcons();
    void testCreateGeometryForLayer();
    void testInvalidGeometryWarning();
    void testAttribution();
    void testParsePositionUpdates();
    void testFormatDistanceInProjectUnit();
    void testFormatAreaInProjectUnit();
    void testRelevantGeometryCenterToScreenCoordinates();
    void testIsValidEmail();
    void testSanitizeFileName();

  private:
    void testFormatDuration( const QDateTime &t0, qint64 diffSecs, const QString &expectedResult );

    InputUtils *mUtils;
};

#endif // TESTUTILSFUNCTIONS_H
