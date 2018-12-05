#ifndef APPSETTINGS_H
#define APPSETTINGS_H

#include <QObject>
#include <QHash>

class AppSettings: public QObject {
    Q_OBJECT
    Q_PROPERTY(QString defaultProject READ defaultProject WRITE setDefaultProject NOTIFY defaultProjectChanged)
    Q_PROPERTY(QString activeProject READ activeProject WRITE setActiveProject NOTIFY activeProjectChanged)
    Q_PROPERTY(QString defaultProjectName READ defaultProjectName NOTIFY defaultProjectChanged)
    Q_PROPERTY(QString defaultLayer READ defaultLayer WRITE setDefaultLayer NOTIFY defaultLayerChanged)
    Q_PROPERTY(bool autoCenterMapChecked READ autoCenterMapChecked WRITE setAutoCenterMapChecked NOTIFY autoCenterMapCheckedChanged)
    Q_PROPERTY(int gpsAccuracyTolerance READ gpsAccuracyTolerance WRITE setGpsAccuracyTolerance NOTIFY gpsAccuracyToleranceChanged)

public:
    explicit AppSettings(QObject* parent = nullptr);

    QString defaultProject() const;
    void setDefaultProject(const QString &value);

    QString activeProject() const;
    void setActiveProject(const QString &value);

    QString defaultLayer() const;
    void setDefaultLayer(const QString &value);

    bool autoCenterMapChecked();
    void setAutoCenterMapChecked(const bool value);

    QString defaultProjectName() const;

    int gpsAccuracyTolerance() const;
    void setGpsAccuracyTolerance(int gpsAccuracyTolerance);

signals:
    void defaultProjectChanged();
    void activeProjectChanged();
    void defaultLayerChanged();
    void autoCenterMapCheckedChanged();
    void gpsAccuracyToleranceChanged();

private:
    // Projects path
    QString mDefaultProject;
    // Path to active project
    QString mActiveProject;
    // flag for following GPS position
    bool mAutoCenterMapChecked = false;
    // used in GPS signal indicator
    int mGpsAccuracyTolerance = 0;

    // Projects path -> defaultLayer name
    QHash<QString, QString> mDefaultLayers;

    const QString mGroupName = QString("inputApp");

    void reloadDefaultLayers();
};

#endif // APPSETTINGS_H
