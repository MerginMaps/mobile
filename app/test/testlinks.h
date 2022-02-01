/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef TESTLINKS_H
#define TESTLINKS_H

#include <QNetworkAccessManager>
#include <QtTest/QtTest>

#include "inputhelp.h"

class UrlTester: public QObject
{
    Q_OBJECT
  public:
    UrlTester( const QString url ): mUrl( url ) {}
    void processFinished();
    void open();
    int result() const { return mResult; }

  signals:
    void finished();

  private:
    QString mUrl;
    QNetworkAccessManager mManager;
    int mResult = 0;
};

class TestLinks: public QObject
{
    Q_OBJECT

  public:
    TestLinks( MerginApi *api, InputUtils *utils );

  private slots:

    void _run( const QString &url );

    // global init + cleanup functions
    void initTestCase() {}
    void cleanupTestCase() {}

    void testInputWebLink()
    {
      _run( mHelp.inputWebLink() );
    }

    void testPrivacyPolicy()
    {
      _run( mHelp.privacyPolicyLink() );
    }

    void testHowToEnableDigitizing()
    {
      _run( mHelp.howToEnableDigitizingLink() );
    }

    void testHowToEnableBrowsingData()
    {
      _run( mHelp.howToEnableBrowsingDataLink() );
    }

    void testHowToSetupThemes()
    {
      _run( mHelp.howToSetupThemesLink() );
    }

    void testHowToCreateNewProject()
    {
      _run( mHelp.howToCreateNewProjectLink() );
    }

    void testHowToDownloadProject()
    {
      _run( mHelp.howToDownloadProjectLink() );
    }

    void testHelpRootLink()
    {
      _run( mHelp.helpRootLink() );
    }

    void testMerginWebLink()
    {
      _run( mHelp.merginWebLink() );
    }

    void testMerginDashboardLink()
    {
      _run( mHelp.merginDashboardLink() );
    }

    void testMerginSubscriptionDetailsLink()
    {
      _run( mHelp.merginSubscriptionDetailsLink() );
    }

    void testHowToSetupProj()
    {
      _run( mHelp.howToSetupProj() );
    }

    void testGpsAccuracyHelpLink()
    {
      _run( mHelp.gpsAccuracyHelpLink() );
    }

    void testHowToConnectGpsLink()
    {
      _run( mHelp.howToConnectGPSLink() );
    }

    void testMerginTermsLink()
    {
      _run( mHelp.merginTermsLink() );
    }

  private:
    InputHelp mHelp;
    QNetworkAccessManager mManager;
};

#endif // TESTLINKS_H
