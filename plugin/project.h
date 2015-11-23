#ifndef PROJECT_H
#define PROJECT_H

#include <QObject>

class Project : public QObject
{
  Q_OBJECT
  Q_PROPERTY(QString projectFile READ projectFile WRITE setProjectFile NOTIFY projectFileChanged)
  Q_PROPERTY(QStringList layers READ layers NOTIFY projectFileChanged)
public:
  static Project* instance();

  void setProjectFile(const QString& filename);
  QString projectFile() const { return mFilename; }

  QStringList layers() const;

signals:
  void projectFileChanged();

public slots:

protected:
  explicit Project(QObject *parent = 0);

  static Project* sInstance;

  QString mFilename;
};

#endif // PROJECT_H
