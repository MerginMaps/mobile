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
    void testDateRange_dateTime();
    void testDateRange_date();
    void testDateRange_dateTime_null();              // invalid bounds fall back to min/max sentinels
    void testDateRange_date_null();                  // invalid bounds fall back to min/max sentinels
    void testDateRange_dateTime_featureAtLowerBound(); // >= is inclusive: feature exactly at from-bound is counted
    void testDateRange_dateTime_midnightLowerBound();  // from=midnight; midnight feature is at the inclusive lower bound
    void testDateRange_dateTime_zeroMsInsideRange();   // 0 ms feature inside range: range uses >= / <=, no double-expr needed

    // Single select
    void testSingleSelect_dateTime_nonZeroMs();
    void testSingleSelect_dateTime_zeroMs(); // edge case: 0 ms
    void testSingleSelect_dateTime_null();   // null -> NULL OR ''
    void testSingleSelect_date();

    // Multi select
    void testMultiSelect_dateTime_nonZeroMs();
    void testMultiSelect_dateTime_zeroMs();  // edge case: 0 ms
    void testMultiSelect_dateTime_mixed();   // mix of 0 ms and non-zero ms values
    void testMultiSelect_dateTime_null();    // null -> NULL OR ''
    void testMultiSelect_dateTime_empty();   // empty list -> no subset string
    void testMultiSelect_date();

  private:
    std::unique_ptr<FilterController> mController;
};

#endif // TESTFILTERCONTROLLER_H
