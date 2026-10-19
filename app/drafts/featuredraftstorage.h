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
    static void saveDraft( const QString &projectId, const FeatureDraft &draft );

    // Returns the stored draft for the given project, or a default (empty) FeatureDraft if none exists.
    static FeatureDraft loadDraft( const QString &projectId );

    // Removes the stored draft for the given project, if any.
    static void clearDraft( const QString &projectId );

  private:
    static QJsonObject toJson( const FeatureDraft &draft );
    static FeatureDraft fromJson( const QJsonObject &json );

    static QString settingsKey( const QString &projectId );
};

#endif // FEATUREDRAFTSTORAGE_H
