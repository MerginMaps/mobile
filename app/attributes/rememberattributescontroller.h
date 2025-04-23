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


#include <QObject>
#include <QString>
#include <QSettings>

class FeatureLayerPair;
class QgsVectorLayer;

/**
 * \note QML Type: RememberAttributes
 */
class  RememberAttributesController : public QObject
{
    Q_OBJECT

    //! Returns TRUE if remembering values is allowed
    Q_PROPERTY( bool rememberValuesAllowed MEMBER mRememberValuesAllowed )

    //! Provides context for creating project-specific settings keys
    Q_PROPERTY( QString activeProjectId MEMBER mActiveProjectId )

  public:
    RememberAttributesController( QObject *parent = nullptr );
    ~RememberAttributesController() override;

    void storeFeature( const FeatureLayerPair &pair );

    // Returns false if the value is not remembered
    bool rememberedValue( const QgsVectorLayer *layer, int fieldIndex, QVariant &value ) const;

    // Returns false if the value should not be remembered
    bool shouldRememberValue( const QgsVectorLayer *layer, int fieldIndex ) const;

    // Returns whether value was changed
    bool setShouldRememberValue( const QgsVectorLayer *layer, int fieldIndex, bool shouldRemember );

  signals:
    void rememberValuesAllowedChanged();
    void activeProjectChanged();

  private:
    // Helper method to retrieve settings value keys;
    QString keyForField( const QgsVectorLayer *layer, int fieldIndex ) const;

    // Returns if remember values is enabled in QGIS form config
    bool enabledInQgis( const QgsVectorLayer *layer, int fieldIndex ) const;

    QString mActiveProjectId;
    bool mRememberValuesAllowed = false;
    QSettings mSettings;

    friend class TestRememberAttributesController;
};

#endif // REMEMBERATTRIBUTESCONTROLLER_H

