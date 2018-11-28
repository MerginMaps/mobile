#ifndef APPSETTINGS_H
#define APPSETTINGS_H

#include <QObject>

class AppSettings: public QObject {
    Q_OBJECT
    Q_PROPERTY(QString defaultProject READ defaultProject WRITE setDefaultProject NOTIFY defaultProjectChanged) // never changes

public:
    explicit AppSettings(QObject* parent = nullptr);

    QString defaultProject() const;
    void setDefaultProject(const QString &value);

signals:
    void defaultProjectChanged();

private:
    QString mDefaultProject;

    const QString mGroupName = QString("inputApp");
};

#endif // APPSETTINGS_H
