#include "appsettings.h"

#include <QSettings>

AppSettings::AppSettings(QObject* parent):QObject(parent)
{
    mDefaultLayers = QHash<QString, QString>();

    QSettings settings;
    settings.beginGroup(mGroupName);
    QString path = settings.value("defaultProject","").toString();
    QString layer = settings.value("defaultLayer/"  + path,"").toString();
    settings.endGroup();

    setDefaultProject(path);
    setDefaultLayer(layer);
}

QString AppSettings::defaultLayer() const
{
    return mDefaultLayers.value(mDefaultProject);
}

void AppSettings::setDefaultLayer(const QString &value)
{
    if (defaultLayer() != value) {
        QSettings settings;
        settings.beginGroup(mGroupName);
        settings.setValue("defaultLayer/" + mDefaultProject, value);
        settings.endGroup();
        mDefaultLayers.insert(mDefaultProject, value);
        emit defaultLayerChanged();
    }
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
