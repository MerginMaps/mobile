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

#include <QObject>
#include <QString>
#include <QDateTime>
#include <QVariant>
#include <QVector>

#include "qgsgeometry.h"
#include "qgsfeature.h"

#include "inpututils.h"

#include <qqmlintegration.h>

//! Which stage a draft was interrupted at. Namespaced so it can be shared by
//! FeatureDraft and FeatureDraftController, and registered for QML as DraftStage
namespace FeatureDraftStage
{
  Q_NAMESPACE
  QML_NAMED_ELEMENT( DraftStage )

  enum class DraftStage
  {
    GeometryCapture,
    AttributeForm
  };
  Q_ENUM_NS( DraftStage )
}

//! One attribute of a draft, typeName is used to detect a changed field type
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
  bool isEmpty() const { return layerId.isEmpty(); }

  // Whether this draft belongs to an existing feature being edited, rather than a new one being added
  bool isExistingFeature() const { return InputUtils::isFeatureIdValid( featureId ); }

  QString layerId;
  FeatureDraftStage::DraftStage stage = FeatureDraftStage::DraftStage::AttributeForm;
  QDateTime timestamp;
  QgsGeometry geometry;
  QVector<FeatureDraftAttribute> attributes;
  QgsFeatureId featureId = FID_NULL; // FID_NULL means a new (not-yet-existing) feature
};

#endif // FEATUREDRAFT_H
