/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef INPUTHELP_H
#define INPUTHELP_H

#include <QObject>
#include <QString>
#include <QNetworkAccessManager>

class MerginApi;
class InputUtils;

class InputHelp: public QObject
{
    Q_OBJECT

    Q_PROPERTY( QString privacyPolicyLink READ privacyPolicyLink NOTIFY linkChanged )
    Q_PROPERTY( QString helpRootLink READ helpRootLink )
    Q_PROPERTY( QString mmWebLink READ mmWebLink NOTIFY linkChanged )
    Q_PROPERTY( QString merginDashboardLink READ merginDashboardLink NOTIFY merginLinkChanged )
    Q_PROPERTY( QString merginSubscriptionLink READ merginSubscriptionLink NOTIFY merginLinkChanged )
    Q_PROPERTY( QString merginSubscriptionDetailsLink READ merginSubscriptionDetailsLink NOTIFY linkChanged )
    Q_PROPERTY( QString howToEnableDigitizingLink READ howToEnableDigitizingLink NOTIFY linkChanged )
    Q_PROPERTY( QString howToEnableBrowsingDataLink READ howToEnableBrowsingDataLink NOTIFY linkChanged )
    Q_PROPERTY( QString howToSetupThemesLink READ howToSetupThemesLink NOTIFY linkChanged )
    Q_PROPERTY( QString howToSetupProj READ howToSetupProj NOTIFY linkChanged )
    Q_PROPERTY( QString gpsAccuracyHelpLink READ gpsAccuracyHelpLink NOTIFY linkChanged )
    Q_PROPERTY( QString howToConnectGPSLink READ howToConnectGPSLink NOTIFY linkChanged )
    Q_PROPERTY( QString merginTermsLink READ merginTermsLink NOTIFY linkChanged )
    Q_PROPERTY( QString projectLoadingErrorHelpLink READ projectLoadingErrorHelpLink NOTIFY linkChanged )
    Q_PROPERTY( QString whatsNewPostLink READ whatsNewPostLink NOTIFY linkChanged )
    Q_PROPERTY( QString migrationGuides READ migrationGuides CONSTANT )

    //! When adding new link, make sure you also create unit test for it in TestLinks

    Q_PROPERTY( bool submitReportPending READ submitReportPending NOTIFY submitReportPendingChanged )

  signals:
    void linkChanged();
    void merginLinkChanged();
    void submitReportPendingChanged();
    void submitReportSuccessful();
    void submitReportFailed();

  public slots:
    void onSubmitReportReplyFinished();

  public:
    explicit InputHelp( MerginApi *merginApi );

    static QString helpRootLink() ;
    static QString mmWebLink() ;
    QString merginDashboardLink() const;
    QString merginSubscriptionLink() const;
    static QString privacyPolicyLink() ;
    static QString merginSubscriptionDetailsLink() ;
    static QString howToEnableDigitizingLink() ;
    static QString howToEnableBrowsingDataLink() ;
    static QString howToSetupThemesLink() ;
    static QString howToSetupProj() ;
    static QString gpsAccuracyHelpLink() ;
    static QString howToConnectGPSLink() ;
    static QString merginTermsLink() ;
    static QString projectLoadingErrorHelpLink() ;
    static QString whatsNewPostLink() ;
    static QString changelogLink();
    static QString helpdeskMail();
    static QString migrationGuides() ;

    bool submitReportPending() const;
    /**
     * Reads and returns the internal text log file content.
     *
     * The latest messages in the log come at the beginning. Only last 5MB are read.
     * Prepends the information about screen, device, logged user and application
     *
     * \see log()
     */
    Q_INVOKABLE QString fullLog( bool isHtml ) const;

    /**
     * Submit user log
     */
    Q_INVOKABLE void submitReport( );

  private:
    QVector<QString> logHeader( bool isHtml ) const;
    QString merginLinkHelper( const QString &subpath, const QString &utmTag ) const;

    MerginApi *mMerginApi = nullptr;
    QNetworkAccessManager mManager;
    bool mSubmitReportPending = false;
};

#endif // INPUTHELP_H
