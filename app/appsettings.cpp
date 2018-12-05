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
    bool autoCenter = settings.value("autoCenter", false).toBool();
    int gpsTolerance = settings.value("gpsTolerance", 0).toInt();
    settings.endGroup();

    setDefaultProject(path);
    setActiveProject(path);
    setDefaultLayer(layer);
    setAutoCenterMapChecked(autoCenter);
    setGpsAccuracyTolerance(gpsTolerance);
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

bool AppSettings::autoCenterMapChecked() {
    return mAutoCenterMapChecked;
}


void AppSettings::setAutoCenterMapChecked(bool value)
{

    if (mAutoCenterMapChecked != value) {
        mAutoCenterMapChecked = value;
        QSettings settings;
        settings.beginGroup(mGroupName);
        settings.setValue("autoCenter", value);
        settings.endGroup();

        emit autoCenterMapCheckedChanged();
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

int AppSettings::gpsAccuracyTolerance() const
{
    return mGpsAccuracyTolerance;
}

void AppSettings::setGpsAccuracyTolerance(int value)
{
    if (mGpsAccuracyTolerance != value) {
        mGpsAccuracyTolerance = value;
        QSettings settings;
        settings.beginGroup(mGroupName);
        settings.setValue("gpsTolerance", value);
        settings.endGroup();

        emit gpsAccuracyToleranceChanged();
    }

}
