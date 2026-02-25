/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef HOTRELOAD_H
#define HOTRELOAD_H

#include <QObject>
#include <QQmlApplicationEngine>
#include <QTimer>

class QFileSystemWatcher;

class HotReload : public QObject
{
    Q_OBJECT
  public:
    explicit HotReload( QQmlApplicationEngine &engine, QObject *parent = nullptr );

  signals:
    void watchedSourceChanged();

  public slots:
    void clearCache();
    void startHotReload();

  private:
    QString syncScript() const;

  private:
    QFileSystemWatcher *_watcher;
    QQmlApplicationEngine &_engine;
    QTimer* _debounceTimer = nullptr;
};

#endif // HOTRELOAD_H
