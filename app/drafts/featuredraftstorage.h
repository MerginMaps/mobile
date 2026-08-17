/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef FEATUREDRAFTSTORAGE_H
#define FEATUREDRAFTSTORAGE_H

#include <QString>
#include <QJsonObject>

// Saves/loads/clears an in-progress feature edit ("draft") per project. Stores
// a JSON blob - knows nothing about what's inside it or when it's valid.
class FeatureDraftStorage
{
  public:
    explicit FeatureDraftStorage() = default;
    ~FeatureDraftStorage() = default;

    // Persists the draft payload for the given project, replacing any previous draft for it.
    static void saveDraft( const QString &projectId, const QJsonObject &draft );

    // Returns the stored draft payload for the given project, or an empty object if none exists.
    static QJsonObject loadDraft( const QString &projectId );

    // Removes the stored draft for the given project, if any.
    static void clearDraft( const QString &projectId );

  private:
    static QString settingsKey( const QString &projectId );

    static const QString QSETTINGS_DRAFTS_GROUP_NAME;
};

#endif // FEATUREDRAFTSTORAGE_H
