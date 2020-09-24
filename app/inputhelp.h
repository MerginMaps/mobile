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

class InputHelp: public QObject
{
    Q_OBJECT

    Q_PROPERTY( QString privacyPolicyLink READ privacyPolicyLink NOTIFY linkChanged )
    Q_PROPERTY( QString merginSubscriptionDetailsLink READ merginSubscriptionDetailsLink NOTIFY linkChanged )
    Q_PROPERTY( QString privacyPolicyLink READ privacyPolicyLink NOTIFY linkChanged )
    Q_PROPERTY( QString howToEnableDigitizingLink READ howToEnableDigitizingLink NOTIFY linkChanged )
    Q_PROPERTY( QString howToEnableBrowsingDataLink READ howToEnableBrowsingDataLink NOTIFY linkChanged )
    Q_PROPERTY( QString howToSetupThemesLink READ howToSetupThemesLink NOTIFY linkChanged )
    Q_PROPERTY( QString howToCreateNewProjectLink READ howToCreateNewProjectLink NOTIFY linkChanged )
    Q_PROPERTY( QString howToDownloadProjectLink READ howToDownloadProjectLink NOTIFY linkChanged )

  signals:
    void linkChanged();

  public:
    explicit InputHelp();

    QString privacyPolicyLink() const;
    QString merginSubscriptionDetailsLink() const;
    QString howToEnableDigitizingLink() const;
    QString howToEnableBrowsingDataLink() const;
    QString howToSetupThemesLink() const;
    QString howToCreateNewProjectLink() const;
    QString howToDownloadProjectLink() const;
};

#endif // INPUTHELP_H
