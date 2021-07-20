/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef RELATIONREFERENCEFEATURESMODEL_H
#define RELATIONREFERENCEFEATURESMODEL_H

#include <QObject>

#include "qgsproject.h"
#include "featureslistmodel.h"

class RelationReferenceFeaturesModel : public FeaturesListModel
{
  Q_OBJECT

  Q_PROPERTY( QVariantMap config WRITE setConfig NOTIFY configChanged )
  Q_PROPERTY( QgsProject *project WRITE setProject NOTIFY projectChanged )
  Q_PROPERTY( bool allowNull READ allowNull NOTIFY allowNullChanged )

public:

  explicit RelationReferenceFeaturesModel( QObject *parent = nullptr );
  virtual ~RelationReferenceFeaturesModel() {};

  bool allowNull() const;

  void setConfig( QVariantMap config );
  void setProject( QgsProject *project );

  void setup();

signals:
  void configChanged( QVariantMap config );
  void projectChanged( QgsProject *project );
  void allowNullChanged( bool allowNull );
  void populated();

private:

  QString mFeatureTitle;
  QVariantMap mConfig;
  QgsProject *mProject = nullptr;
  bool mAllowNull;
};

#endif // RELATIONREFERENCEFEATURESMODEL_H
