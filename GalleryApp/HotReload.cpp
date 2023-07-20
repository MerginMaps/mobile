#include "HotReload.h"
#include <QFileSystemWatcher>
#include <QDir>
#include <QGuiApplication>
#include <QProcess>

#warning Tested on Mac
QString HotReload::syncScript() const
{
  return "#!/bin/sh \
    echo running hot reload sync directories ... \
    while true; do \
      rsync -rav ../../../../GalleryApp/qml/pages/ qml/ \
      sleep 1 \
    done";
}

HotReload::HotReload(QQmlApplicationEngine& engine, const QString &directory, QObject *parent):
  _engine(engine)
{
  if(!QDir(directory).exists())
    QDir().mkdir(QGuiApplication::applicationDirPath() + "/" + directory);

  _watcher = new QFileSystemWatcher(this);
  _watcher->addPath(directory);

  connect(_watcher, &QFileSystemWatcher::directoryChanged, this, [this, &engine](const QString& path){
    emit watchedSourceChanged();
  });

  // create runnable sync script near the app
  QString scriptFilename = QGuiApplication::applicationDirPath() + "/syncGallery.sh";
  qInfo() << "Sync script location: " << scriptFilename;
  if(!QFileInfo::exists(scriptFilename)) {
    QFile file(QFileInfo(scriptFilename).absoluteFilePath());
    const QString script = syncScript();
    if (!file.open(QIODevice::WriteOnly)) {
      qInfo() << "Cannot create script file";
      exit(1);
    }
    QTextStream out(&file);
    out << script;
    file.close();
    QProcess::execute("chmod", QStringList() << "+x" << file.fileName());
  }

  // run sync script
  QProcess::startDetached("./syncGallery.sh");
}

void HotReload::clearCache()
{
  _engine.clearComponentCache();
}
