#ifndef HOTRELOAD_H
#define HOTRELOAD_H

#include <QObject>
#include <QQmlApplicationEngine>

class QFileSystemWatcher;

class HotReload : public QObject
{
  Q_OBJECT
public:
  explicit HotReload(QQmlApplicationEngine& engine, const QString& directory, QObject *parent = nullptr);

signals:
  void watchedSourceChanged();

public slots:
  void clearCache();

private:
  QFileSystemWatcher *_watcher;
  QQmlApplicationEngine& _engine;
};

#endif // HOTRELOAD_H
