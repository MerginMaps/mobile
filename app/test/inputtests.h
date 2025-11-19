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

#include <QStringList>

class MerginApi;
class InputUtils;
class VariablesManager;
class PositionKit;
class AppSettings;

class InputTests
{
  public:
    InputTests();
    ~InputTests();

    void parseArgs( int argc, char *argv[] );

    bool testingRequested() const;

    void init( MerginApi *api, InputUtils *utils, VariablesManager *varManager, PositionKit *positionKit, AppSettings *settings );
    void initTestDeclarative();
    QString initTestingDir();
    int runTest() const;

  private:
    QString mTestRequested;
    QStringList mTestArgs;
    MerginApi *mApi = nullptr;
    InputUtils *mInputUtils = nullptr;
    VariablesManager *mVariablesManager = nullptr;
    PositionKit *mPositionKit = nullptr;
    AppSettings *mAppSettings = nullptr;
};

#endif // INPUTTESTS_H
