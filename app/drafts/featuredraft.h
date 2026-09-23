/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef FEATUREDRAFT_H
#define FEATUREDRAFT_H

#include <QString>
#include <QDateTime>
#include <QVariant>
#include <QVector>

#include "qgsgeometry.h"
#include "qgsfeature.h"

// One touched attribute captured for a draft - the type name lets isDraftValid()
// detect a field that has since changed shape.
struct FeatureDraftAttribute
{
  QString name;
  QString typeName;
  QVariant value;
};

// In-memory shape of an in-progress feature edit. FeatureDraftStorage is the only
// class that knows how this maps to the on-disk (QSettings/JSON) format.
struct FeatureDraft
{
  enum Stage
  {
    GeometryCapture,
    AttributeForm
  };

  bool isEmpty() const { return layerId.isEmpty(); }

  // Whether this draft belongs to an existing feature being edited, rather than a new one being added
  bool isExistingFeature() const { return !FID_IS_NULL( featureId ) && !FID_IS_NEW( featureId ); }

  QString layerId;
  Stage stage = AttributeForm;
  QDateTime timestamp;
  QgsGeometry geometry;
  QVector<FeatureDraftAttribute> attributes;
  QgsFeatureId featureId = FID_NULL; // FID_NULL means a new (not-yet-existing) feature
};

#endif // FEATUREDRAFT_H
