#include "HotReload.h"
#include <QFileSystemWatcher>

HotReload::HotReload(QQmlApplicationEngine& engine, const QString &directory, QObject *parent):
  _engine(engine)
{
  _watcher = new QFileSystemWatcher(this);
  _watcher->addPath(directory);

  connect(_watcher, &QFileSystemWatcher::directoryChanged, this, [this, &engine](const QString& path){
    emit watchedSourceChanged();
  });
}

void HotReload::clearCache()
{
  _engine.clearComponentCache();
}
