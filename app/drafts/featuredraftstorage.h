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

#include <QHash>
#include <QJsonObject>

#include "featuredraft.h"

// Saves/loads/clears an in-progress feature edit ("draft") per project. The only
// class that knows a draft is stored as a QSettings-held JSON blob - everyone
// else works with the plain FeatureDraft struct.
class FeatureDraftStorage
{
  public:
    explicit FeatureDraftStorage() = default;
    ~FeatureDraftStorage() = default;

    // Persists the draft for the given project, replacing any previous draft for it.
    static void saveDraft( const QString &projectDir, const FeatureDraft &draft );

    // Returns the stored draft for the given project, or a default (empty) FeatureDraft if none exists.
    static FeatureDraft loadDraft( const QString &projectDir );

    // Removes the stored draft for the given project, if any.
    static void clearDraft( const QString &projectDir );

    // Forgets cached projectDir -> storage key lookups. Call whenever the active project changes.
    static void clearCache();

  private:
    static QJsonObject toJson( const FeatureDraft &draft );
    static FeatureDraft fromJson( const QJsonObject &json );

    // Mergin project id when the project is server-linked, otherwise projectDir itself.
    // Resolving this reads a file from disk, so results are cached by projectDir.
    static QString projectKey( const QString &projectDir );

    static QString settingsKey( const QString &projectDir );

    static QHash<QString, QString> sProjectKeyCache;
};

#endif // FEATUREDRAFTSTORAGE_H
