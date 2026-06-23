/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef TESTFILTERCONTROLLER_H
#define TESTFILTERCONTROLLER_H

#include <QObject>
#include <QMap>
#include <memory>

#include "filtercontroller.h"

class TestFilterController : public QObject
{
    Q_OBJECT

  private slots:
    void initTestCase();    // load project once
    void cleanupTestCase(); // clear project
    void init();            // reset filters before each test
    void cleanup();         // reset controller after each test

    // Date range
    void testDateRangeDateTime();
    void testDateRangeDate();
    void testDateRangeDateTimeNull();
    void testDateRangeDateNull();
    void testDateRangeDateTimeLowerBoundInclusive();
    void testDateRangeDateTimeMidnightLowerBound();
    void testDateRangeDateTimeZeroMsInsideRange();

    // Single select
    void testSingleSelectText();
    void testSingleSelectDateTimeNonZeroMs();
    void testSingleSelectDateTimeZeroMs();
    void testSingleSelectDate();
    void testSingleSelectNull();

    // Multi select
    void testMultiSelectText();
    void testMultiSelectDateTimeNonZeroMs();
    void testMultiSelectDateTimeZeroMs();
    void testMultiSelectDateTimeMixed();
    void testMultiSelectDate();
    void testMultiSelectNull();
    void testMultiSelectEmpty();

    // Number range
    void testNumberRange();

    // Text filter
    void testTextFilter();

    // Checkbox filter
    void testCheckboxFilter();

    // Predefined subset string preserved with filters
    void testPredefinedSubsetString();

  private:
    std::unique_ptr<FilterController> mController;
    QMap<QString, QString> mOriginalSubsets; // layer ID -> original subset string
};

#endif // TESTFILTERCONTROLLER_H
