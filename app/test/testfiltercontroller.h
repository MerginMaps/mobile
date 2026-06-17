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
#include <memory>

#include "filtercontroller.h"

class TestFilterController : public QObject
{
    Q_OBJECT

  private slots:
    void init();    // called before each test function
    void cleanup(); // called after each test function

    // Date range
    void testDateRangeDateTime();
    void testDateRangeDate();
    void testDateRangeDateTimeNull();              // invalid bounds fall back to min/max sentinels
    void testDateRangeDateNull();                  // invalid bounds fall back to min/max sentinels
    void testDateRangeDateTimeFeatureAtLowerBound(); // >= is inclusive: feature exactly at from-bound is counted
    void testDateRangeDateTimeMidnightLowerBound();  // from=midnight; midnight feature is at the inclusive lower bound
    void testDateRangeDateTimeZeroMsInsideRange();   // 0 ms feature inside range: range uses >= / <=, no double-expr needed

    // Single select
    void testSingleSelectDateTimeNonZeroMs();
    void testSingleSelectDateTimeZeroMs(); // edge case: 0 ms
    void testSingleSelectDateTimeNull();   // null -> NULL OR ''
    void testSingleSelectDate();

    // Multi select
    void testMultiSelectDateTimeNonZeroMs();
    void testMultiSelectDateTimeZeroMs();  // edge case: 0 ms
    void testMultiSelectDateTimeMixed();   // mix of 0 ms and non-zero ms values
    void testMultiSelectDateTimeNull();    // null -> NULL OR ''
    void testMultiSelectDateTimeEmpty();   // empty list -> no subset string
    void testMultiSelectDate();

  private:
    std::unique_ptr<FilterController> mController;
};

#endif // TESTFILTERCONTROLLER_H
