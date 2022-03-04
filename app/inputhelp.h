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
    Q_PROPERTY( QString inputWebLink READ inputWebLink NOTIFY linkChanged )
    Q_PROPERTY( QString merginWebLink READ merginWebLink NOTIFY merginLinkChanged )
    Q_PROPERTY( QString merginDashboardLink READ merginDashboardLink NOTIFY merginLinkChanged )
    Q_PROPERTY( QString merginSubscriptionDetailsLink READ merginSubscriptionDetailsLink NOTIFY linkChanged )
    Q_PROPERTY( QString howToEnableDigitizingLink READ howToEnableDigitizingLink NOTIFY linkChanged )
    Q_PROPERTY( QString howToEnableBrowsingDataLink READ howToEnableBrowsingDataLink NOTIFY linkChanged )
    Q_PROPERTY( QString howToSetupThemesLink READ howToSetupThemesLink NOTIFY linkChanged )
    Q_PROPERTY( QString howToCreateNewProjectLink READ howToCreateNewProjectLink NOTIFY linkChanged )
    Q_PROPERTY( QString howToDownloadProjectLink READ howToDownloadProjectLink NOTIFY linkChanged )
    Q_PROPERTY( QString howToSetupProj READ howToSetupProj NOTIFY linkChanged )
    Q_PROPERTY( QString gpsAccuracyHelpLink READ gpsAccuracyHelpLink NOTIFY linkChanged )
    Q_PROPERTY( QString howToConnectGPSLink READ howToConnectGPSLink NOTIFY linkChanged )
    Q_PROPERTY( QString merginTermsLink READ merginTermsLink NOTIFY linkChanged )

    //! When adding new link, make sure you also create unit test for it in TestLinks

    Q_PROPERTY( bool submitReportPending READ submitReportPending NOTIFY submitReportPendingChanged )

  signals:
    void linkChanged();
    void merginLinkChanged();
    void submitReportPendingChanged();

  public slots:
    void onSubmitReportReplyFinished();

  public:
    explicit InputHelp( MerginApi *merginApi, InputUtils *utils );

    QString helpRootLink() const;
    QString inputWebLink() const;
    QString merginWebLink() const;
    QString merginDashboardLink() const;
    QString privacyPolicyLink() const;
    QString merginSubscriptionDetailsLink() const;
    QString howToEnableDigitizingLink() const;
    QString howToEnableBrowsingDataLink() const;
    QString howToSetupThemesLink() const;
    QString howToCreateNewProjectLink() const;
    QString howToDownloadProjectLink() const;
    QString howToSetupProj() const;
    QString gpsAccuracyHelpLink() const;
    QString howToConnectGPSLink() const;
    QString merginTermsLink() const;

    bool submitReportPending() const;
    /**
     * Reads and returns the internal text log file content.
     *
     * The latest messages in the log come at the beginning. Only last 0.5MB are read.
     * Prepends the information about screen, device, logged user and application
     *
     * \see log()
     */
    Q_INVOKABLE QString fullLog( bool isHtml );

    /** Submit user log*/
    Q_INVOKABLE void submitReport( );

  private:
    QVector<QString> logHeader( bool isHtml );

  private:
    MerginApi *mMerginApi = nullptr;
    InputUtils *mInputUtils = nullptr;
    QNetworkAccessManager mManager;
    bool mSubmitReportPending = false;
};

#endif // INPUTHELP_H
