#include "appsettings.h"

#include <QSettings>
#include <QFileInfo>

AppSettings::AppSettings(QObject* parent):QObject(parent)
{
    mDefaultLayers = QHash<QString, QString>();
    reloadDefaultLayers();

    QSettings settings;
    settings.beginGroup(mGroupName);
    QString path = settings.value("defaultProject","").toString();
    QString layer = settings.value("defaultLayer/"  + path,"").toString();
    settings.endGroup();

    setDefaultProject(path);
    setActiveProject(path);
    setDefaultLayer(layer);
}

QString AppSettings::defaultLayer() const
{
    return mDefaultLayers.value(mActiveProject);
}

void AppSettings::setDefaultLayer(const QString &value)
{
    if (defaultLayer() != value) {
        QSettings settings;
        settings.beginGroup(mGroupName);
        settings.setValue("defaultLayer/" + mActiveProject, value);
        settings.endGroup();
        mDefaultLayers.insert(mActiveProject, value);
        emit defaultLayerChanged();
    }
}

void AppSettings::reloadDefaultLayers()
{
    QSettings settings;
    settings.beginGroup(mGroupName);
    for (QString key: settings.allKeys()) {
        if (key.startsWith("defaultLayer/")) {
            QVariant value = settings.value(key);
            mDefaultLayers.insert(key.replace("defaultLayer", ""), value.toString());
        }
    }

    settings.endGroup();
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

QString AppSettings::activeProject() const
{
    return mActiveProject;
}

void AppSettings::setActiveProject(const QString &value)
{
    if (mActiveProject != value) {
        mActiveProject = value;

        emit activeProjectChanged();
        emit defaultLayerChanged();
    }
}

QString AppSettings::defaultProjectName() const
{
    if (!mDefaultProject.isEmpty()) {
        QFileInfo info(mDefaultProject);
        return info.baseName();
    }
    return QString("");
}
