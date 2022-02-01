/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef INPUTTESTS_H
#define INPUTTESTS_H

#include <QString>
#include <QStringList>

class MerginApi;
class Purchasing;
class InputUtils;
class VariablesManager;
class PositionKit;

class InputTests
{
  public:
    InputTests();
    ~InputTests();

    void parseArgs( int argc, char *argv[] );

    bool testingRequested() const;

    void init( MerginApi *api, Purchasing *purchasing, InputUtils *utils, VariablesManager *varManager, PositionKit *positionKit );
    void initTestDeclarative();
    QString initTestingDir();
    int runTest() const;

  private:
    QString mTestRequested;
    QStringList mTestArgs;
    MerginApi *mApi = nullptr;
    Purchasing *mPurchasing = nullptr;
    InputUtils *mInputUtils = nullptr;
    VariablesManager *mVariablesManager = nullptr;
    PositionKit *mPositionKit = nullptr;
};

#endif // INPUTTESTS_H
