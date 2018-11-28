#include "appsettings.h"

#include <QSettings>

AppSettings::AppSettings(QObject* parent):QObject(parent)
{
    QSettings settings;
    settings.beginGroup(mGroupName);
    QString path = settings.value("defaultProject","").toString();
    settings.endGroup();

    setDefaultProject(path);
}

QString AppSettings::defaultProject() const
{
    return mDefaultProject;
}

void AppSettings::setDefaultProject(const QString &value)
{
    if (mDefaultProject != value) {
        mDefaultProject = value;
        QSettings settings;
        settings.beginGroup(mGroupName);
        settings.setValue("defaultProject", value);
        settings.endGroup();

        emit defaultProjectChanged();
    }
}
