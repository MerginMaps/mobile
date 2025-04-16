/***************************************************************************
  rememberattributescontroller.h
  --------------------------------------
  Date                 : 4.5.2021
  Copyright            : (C) 2021 by Peter Petrik
  Email                : zilolv@gmail.com
 ***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef REMEMBERATTRIBUTESCONTROLLER_H
#define REMEMBERATTRIBUTESCONTROLLER_H


#include "qgsfeature.h"
#include "qgsvectorlayer.h"
#include "inputconfig.h"
#include "activeproject.h"
#include <QHash>

class FeatureLayerPair;

/**
 * \note QML Type: RememberAttributes
 */
class  RememberAttributesController : public QObject
{
    Q_OBJECT

    //! Returns TRUE if remembering values is allowed
    Q_PROPERTY( bool rememberValuesAllowed READ rememberValuesAllowed WRITE setRememberValuesAllowed NOTIFY rememberValuesAllowedChanged )

    //! Provides context for creating project-specific settings keys
    Q_PROPERTY( ActiveProject *activeProject READ activeProject WRITE setActiveProject NOTIFY activeProjectChanged )

  public:
    RememberAttributesController( QObject *parent = nullptr );
    ~RememberAttributesController() override;

    static const QString CACHED_ATTRIBUTES_GROUP;

    bool rememberValuesAllowed() const;
    void setRememberValuesAllowed( bool rememberValuesAllowed );

    void storeFeature( const FeatureLayerPair &pair );

    // Returns false if the value is not remembered
    bool rememberedValue( const QgsVectorLayer *layer, int fieldIndex, QVariant &value ) const;

    // Returns false if the value should not be remembered
    bool shouldRememberValue( const QgsVectorLayer *layer, int fieldIndex ) const;

    // Returns whether value was changed
    bool setShouldRememberValue( const QgsVectorLayer *layer, int fieldIndex, bool shouldRemember );

    // Helper method to retrieve enabled and value keys;
    QString keyForField( const QString &suffix, const QgsVectorLayer *layer, int fieldIndex ) const;

    ActiveProject *activeProject() const;
    void setActiveProject( ActiveProject *newActiveProject );

  signals:
    void rememberValuesAllowedChanged();
    void activeProjectChanged();

  private:
    ActiveProject *mActiveProject = nullptr;
};

#endif // REMEMBERATTRIBUTESCONTROLLER_H

