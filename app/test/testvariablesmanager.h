/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/
#include <QObject>
#include <QtTest>

#include "inputconfig.h"
#include "variablesmanager.h"
#include "qgeopositioninfo.h"
#include "positionkit.h"
#include "appsettings.h"
#include "compass.h"

#ifndef TESTVARIABLESMANAGER_H
#define TESTVARIABLESMANAGER_H

class MerginApi;

class TestVariablesManager: public QObject
{
    Q_OBJECT
  public:
    TestVariablesManager( MerginApi *api, VariablesManager *vm, PositionKit *pk, AppSettings *as );
  private slots:
    void init(); // will be called before each testfunction is executed.
    void cleanup(); // will be called after every testfunction.

    void positionVariables();
    void userVariables();

  private:
    MerginApi *mApi;
    VariablesManager *mVariablesManager;
    PositionKit *mPositionKit;
    AppSettings *mAppSettings;
    void evaluateExpression( const QString &expStr, const QString &expectedValue, const QgsExpressionContext *context );
    GeoPosition testGeoPosition();
};

#endif // TESTVARIABLESMANAGER_H
