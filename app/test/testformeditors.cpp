/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include "testformeditors.h"

#include <QtTest/QtTest>
#include <memory>

void TestFormEditors::init()
{
}

void TestFormEditors::cleanup()
{
}

void TestFormEditors::testDoublesValidation()
{
//  std::unique_ptr<InputNumberValidator> validator = std::unique_ptr<InputNumberValidator>( new InputNumberValidator() );

//  // let's test with en_US locale, we will test others later
//  // spec: https://lh.2xlibre.net/locale/en_US/
//  QLocale enLocale = QLocale( "en_US" );
//  validator->setLocale( enLocale );

//  double bottom, top, decimals;

//  // let's start with simple range <-100;100>, 2 decimal places
//  bottom = -100;
//  top = 100;
//  decimals = 2;

//  validator->setRange( bottom, top, decimals );

//  // combinations of input
//  QList<QPair<QString, QValidator::State>> combinations =
//  {
//    {"0", QValidator::Acceptable},
//    {"0.01", QValidator::Acceptable},
//    {"0.5", QValidator::Acceptable},
//    {"10", QValidator::Acceptable},
//    {"50.53", QValidator::Acceptable},
//    {"100", QValidator::Acceptable},
//    {"100.", QValidator::Acceptable},
//    {"100.0", QValidator::Acceptable},
//    {"-10", QValidator::Acceptable},
//    {"-50.", QValidator::Acceptable},
//    {"-100", QValidator::Acceptable},
//    {"-100.", QValidator::Acceptable},
//    {"-100.0", QValidator::Acceptable},
//    {"-50.53", QValidator::Acceptable},

//    {"0.001", QValidator::Invalid},
//    {"-101", QValidator::Invalid},
//    {"-1000000", QValidator::Invalid},
//    {"101", QValidator::Invalid},
//    {"10000000", QValidator::Invalid},
//    {"50.523", QValidator::Invalid},
//    {"-50.523", QValidator::Invalid},

//    {"", QValidator::Intermediate},
//    {"-", QValidator::Intermediate},
//  };

//  int npos = 0; // start, not used during validation

//  for ( auto &c : combinations )
//  {
//    qDebug() << c.first;
//    QCOMPARE( validator->validate( c.first, npos ), c.second );
//  }

//  // only positive numbers, starting at zero: <0; 10.50>, 2 decimal places
//  bottom = 0;
//  top = 10.5;
//  decimals = 2;

//  validator->setRange( bottom, top, decimals );

//  combinations =
//  {
//    {"0", QValidator::Acceptable},
//    {"0.00", QValidator::Acceptable},
//    {"0.01", QValidator::Acceptable},
//    {"5", QValidator::Acceptable},
//    {"10", QValidator::Acceptable},
//    {"10.35", QValidator::Acceptable},
//    {"10.50", QValidator::Acceptable},

//    {"0.001", QValidator::Invalid},
//    {"5.001", QValidator::Invalid},
//    {"10.51", QValidator::Invalid},
//    {"11", QValidator::Invalid},
//    {"10000", QValidator::Invalid},
//    {"-", QValidator::Invalid},
//    {"-0.01", QValidator::Invalid},
//    {"-5", QValidator::Invalid},
//    {"-11", QValidator::Invalid},
//    {"-10000", QValidator::Invalid}

//  };

//  for ( auto &c : combinations )
//  {
//    QCOMPARE( validator->validate( c.first, npos ), c.second );
//  }

//  // only positive numbers, not starting at zero: <100.050; 1000>, 3 decimal places
//  bottom = 100.050;
//  top = 1000;
//  decimals = 3;

//  validator->setRange( bottom, top, decimals );

//  combinations =
//  {
//    {"100.050", QValidator::Acceptable},
//    {"100.05", QValidator::Acceptable},
//    {"100.1", QValidator::Acceptable},
//    {"101", QValidator::Acceptable},
//    {"530.321", QValidator::Acceptable},
//    {"452.2", QValidator::Acceptable},
//    {"1000", QValidator::Acceptable},
//    {"500.", QValidator::Acceptable},
//    {"1000.000", QValidator::Acceptable},

//    {"0.000", QValidator::Invalid},
//    {"0.01", QValidator::Invalid},
//    {"0.5", QValidator::Invalid},
//    {"50.", QValidator::Invalid},
//    {"50.1", QValidator::Invalid},
//    {"50.53", QValidator::Invalid},
//    {"50.531", QValidator::Invalid},
//    {"100.049", QValidator::Invalid},
//    {"100.0501", QValidator::Invalid},
//    {"500.5314", QValidator::Invalid},
//    {"1000.001", QValidator::Invalid},
//    {"1001", QValidator::Invalid},
//    {"10000000", QValidator::Invalid},
//    {"-", QValidator::Invalid},
//    {"-101", QValidator::Invalid},
//    {"-50.523", QValidator::Invalid},
//    {"-520", QValidator::Invalid},
//    {"-1000000", QValidator::Invalid},

//    {"0", QValidator::Intermediate},
//    {"10", QValidator::Intermediate},
//    {"50", QValidator::Intermediate},
//    {"100.", QValidator::Intermediate},
//    {"100.0", QValidator::Intermediate}
//  };

//  for ( auto &c : combinations )
//  {
//    QCOMPARE( validator->validate( c.first, npos ), c.second );
//  }

//  // only negative numbers, not starting at zero: <-200; -100>, 4 decimal places
//  bottom = -200;
//  top = -100;
//  decimals = 4;

//  validator->setRange( bottom, top, decimals );

//  combinations =
//  {
//    {"-200.0000", QValidator::Acceptable},
//    {"-200.00", QValidator::Acceptable},
//    {"-200", QValidator::Acceptable},
//    {"-199", QValidator::Acceptable},
//    {"-150.", QValidator::Acceptable},
//    {"-150.1", QValidator::Acceptable},
//    {"-100.0001", QValidator::Acceptable},
//    {"-100.0", QValidator::Acceptable},
//    {"-100", QValidator::Acceptable},

//    {"-12.", QValidator::Invalid},
//    {"-99.9999", QValidator::Invalid},
//    {"-200.001", QValidator::Invalid},
//    {"-150.00010", QValidator::Invalid},
//    {"0", QValidator::Invalid},
//    {"0.", QValidator::Invalid},
//    {"5", QValidator::Invalid},
//    {"150.1", QValidator::Invalid},

//    {"-", QValidator::Intermediate},
//    {"-1", QValidator::Intermediate},
//    {"-50", QValidator::Intermediate},
//    {"-99", QValidator::Intermediate},
//    {"-99.9", QValidator::Intermediate}, // this should be invalid, there is no way to get to the 100, but it needs more hacks
//  };

//  for ( auto &c : combinations )
//  {
//    QCOMPARE( validator->validate( c.first, npos ), c.second );
//  }
}

void TestFormEditors::testIntValidation()
{
//  std::unique_ptr<InputNumberValidator> validator = std::unique_ptr<InputNumberValidator>( new InputNumberValidator() );

//  QLocale enLocale = QLocale( "en_US" );
//  validator->setLocale( enLocale );

//  double bottom, top, decimals;

//  // <-100;100>
//  bottom = -100;
//  top = 100;
//  decimals = 0; // will stay zero for the rest of the test, this is integer test

//  validator->setRange( bottom, top, decimals );

//  // combinations of input
//  QList<QPair<QString, QValidator::State>> combinations =
//  {
//    {"0", QValidator::Acceptable},
//    {"5", QValidator::Acceptable},
//    {"10", QValidator::Acceptable},
//    {"100", QValidator::Acceptable},
//    {"-10", QValidator::Acceptable},
//    {"-100", QValidator::Acceptable},

//    {".", QValidator::Invalid},
//    {"0.", QValidator::Invalid},
//    {"0.01", QValidator::Invalid},
//    {"50.523", QValidator::Invalid},
//    {"101", QValidator::Invalid},
//    {"10000000", QValidator::Invalid},
//    {"-101", QValidator::Invalid},
//    {"-1000000", QValidator::Invalid},
//    {"-50.523", QValidator::Invalid},

//    {"", QValidator::Intermediate},
//    {"-", QValidator::Intermediate}
//  };

//  int npos = 0; // start, not used during validation

//  for ( auto &c : combinations )
//  {
//    QCOMPARE( validator->validate( c.first, npos ), c.second );
//  }

//  // range without zero <10; 200>, to test intermediate values
//  bottom = 10;
//  top = 200;

//  validator->setRange( bottom, top, decimals );

//  // combinations of input
//  combinations =
//  {
//    {"10", QValidator::Acceptable},
//    {"100", QValidator::Acceptable},
//    {"200", QValidator::Acceptable},

//    {"-", QValidator::Invalid},
//    {".", QValidator::Invalid},
//    {"0.", QValidator::Invalid},
//    {"0.01", QValidator::Invalid},
//    {"5.", QValidator::Invalid},
//    {"10.", QValidator::Invalid},
//    {"50.5", QValidator::Invalid},
//    {"10000000", QValidator::Invalid},
//    {"-50.523", QValidator::Invalid},

//    {"1", QValidator::Intermediate},
//    {"5", QValidator::Intermediate},
//    {"9", QValidator::Intermediate}
//  };

//  for ( auto &c : combinations )
//  {
//    QCOMPARE( validator->validate( c.first, npos ), c.second );
//  }
}

void TestFormEditors::testDifferentLocalesForNumberValidation()
{
//  struct combination
//  {
//    QString value;
//    QString localeString;
//    QValidator::State expectedState;
//  };

//  std::unique_ptr<InputNumberValidator> validator = std::unique_ptr<InputNumberValidator>( new InputNumberValidator() );

//  double bottom, top, decimals;

//  // we will test more complicated range <100.050; 10,000.500> with 3 decimal places
//  bottom = 100.050;
//  top = 10000.500;
//  decimals = 3;

//  validator->setRange( bottom, top, decimals );

//  // combinations of input values and locales in form : {{<input>, <locale>} <output state>}
//  // we are testing 4 different locales: de_DE, sk_SK, en_US and de_CH
//  // all have different rule for decimal point and thousand separator characters
//  QList<combination> combinations =
//  {
//    {"1,000.50", "en_US", QValidator::Acceptable},
//    {"1.000,50", "de_DE", QValidator::Acceptable},
//    {"1000,50", "sk_SK", QValidator::Acceptable},

//    // Swiss are using ’ in combination with . for money values. Qt, however, seems not to use it
//    // spec: https://lh.2xlibre.net/locale/de_CH/
//    // {"1’000.50", "de_CH", QValidator::Acceptable},
//    // simple dot is used instead
//    {"1000.50", "de_CH", QValidator::Acceptable},
//    {"200,", "de_DE", QValidator::Acceptable},
//    {"200,", "sk_SK", QValidator::Acceptable},
//    {"2.000,", "de_DE", QValidator::Acceptable},

//    // QGIS accepts both comma and dot as decimal point char no matter the locale, but not both of them
//    {"1.00,", "en_US", QValidator::Invalid},
//    {"1,000.50", "de_DE", QValidator::Invalid},
//    {"1000,5001", "de_CH", QValidator::Invalid},

//    {"100.", "en_US", QValidator::Intermediate},
//    {"100,", "de_DE", QValidator::Intermediate},
//    {"100,", "sk_SK", QValidator::Intermediate},
//    {"100.", "de_CH", QValidator::Intermediate},
//  };

//  int npos = 0; // start, not used during validation

//  for ( auto &c : combinations )
//  {
//    validator->setLocale( c.localeString );
//    QCOMPARE( validator->validate( c.value, npos ), c.expectedState );
//  }
}
