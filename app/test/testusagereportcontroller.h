/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef TESTUSAGEREPORTCONTROLLER_H
#define TESTUSAGEREPORTCONTROLLER_H

#include <QObject>
#include <QVariantMap>

class AppSettings;
class UsageReportController;

class TestUsageReportController : public QObject
{
    Q_OBJECT

  public:
    TestUsageReportController( UsageReportController *controller, AppSettings *appSettings );

  private slots:
    void initTestCase(); // back up the usage report settings of this device
    void cleanupTestCase(); // restore them
    void init(); // start every test opted in, with no data

    void testSnapshotHasOnlyExpectedKeys();
    void testSnapshotHasNoPersonalData();
    void testSnapshotValues();
    void testSentDataResetAfterSuccess();
    void testSentDataKeptAfterFailure();
    void testOptOutClearsData();

  private:
    UsageReportController *mController = nullptr;
    AppSettings *mAppSettings = nullptr;

    bool mOriginalEnabled = false;
    QVariantMap mOriginalSettings;
};

#endif // TESTUSAGEREPORTCONTROLLER_H
