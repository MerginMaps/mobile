#include "HotReload.h"
#include <QFileSystemWatcher>
#include <QDir>
#include <QGuiApplication>
#include <QProcess>
#include <QTimer>

// TODO: not needed to sync dirs every second, just when a file was changed
QString HotReload::syncScript() const
{
  return "#!/bin/sh \n\
echo running hot reload sync directories ... \n\
while true; do \n\
  rsync -ra ../../../../GalleryApp/qml/ HotReload/GalleryApp/qml/ \n\
  rsync -ra ../../../../app/qmls/ HotReload/app/qmls/ \n\
  sleep 1 \n\
done";
}

HotReload::HotReload(QQmlApplicationEngine& engine, QObject *parent):
  _engine(engine)
{
  // create dirs for sync (near the app)
  if(!QDir("HotReload/GalleryApp/qml/").exists())
    QDir().mkpath(QGuiApplication::applicationDirPath() + "/HotReload/GalleryApp/qml/");
  if(!QDir("HotReload/app/qmls/").exists())
    QDir().mkpath(QGuiApplication::applicationDirPath() + "/HotReload/app/qmls/");

  // create runnable sync script (near the app)
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

  // start watching the changes in synced dirs
  QTimer::singleShot(2000, this, &HotReload::startHotReload);
}

void HotReload::clearCache()
{
  _engine.clearComponentCache();
}

void HotReload::startHotReload()
{
  _watcher = new QFileSystemWatcher(this);
  _watcher->addPath("HotReload/GalleryApp/qml/Pages");
  _watcher->addPath("HotReload/app/qmls");
  _watcher->addPath("HotReload/app/qmls/component");

  // send signal for hot reloading
  connect(_watcher, &QFileSystemWatcher::directoryChanged, this, [this](const QString& path){
    emit watchedSourceChanged();
  });
}
