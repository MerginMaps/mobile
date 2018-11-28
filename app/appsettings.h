#ifndef APPSETTINGS_H
#define APPSETTINGS_H

#include <QObject>
#include <QHash>

class AppSettings: public QObject {
    Q_OBJECT
    Q_PROPERTY(QString defaultProject READ defaultProject WRITE setDefaultProject NOTIFY defaultProjectChanged)
    Q_PROPERTY(QString defaultLayer READ defaultLayer WRITE setDefaultLayer NOTIFY defaultLayerChanged)

public:
    explicit AppSettings(QObject* parent = nullptr);

    QString defaultProject() const;
    void setDefaultProject(const QString &value);

    QString defaultLayer() const;
    void setDefaultLayer(const QString &value);

signals:
    void defaultProjectChanged();
    void defaultLayerChanged();

private:
    QString mDefaultProject;
    QHash<QString, QString> mDefaultLayers;

    const QString mGroupName = QString("inputApp");
};

#endif // APPSETTINGS_H
