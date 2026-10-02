/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef TESTFEATUREDRAFTCONTROLLER_H
#define TESTFEATUREDRAFTCONTROLLER_H

#include <QObject>

class TestFeatureDraftController : public QObject
{
    Q_OBJECT

  private slots:
    void init();
    void cleanup();

    void validNewFeatureGeometryDraftDetected();
    void validExistingFeatureAttributeDraftDetected();
    void expiredDraftIgnoredAndCleared();
    void draftWithRemovedFieldIgnored();
    void draftWithChangedFieldTypeIgnored();
    void existingFeatureDraftRequiresLiveFeature();
    void resumeDraftAppliesGeometryAndAttributesButKeepsStorage();
    void discardDraftClearsStorage();
};

#endif // TESTFEATUREDRAFTCONTROLLER_H
