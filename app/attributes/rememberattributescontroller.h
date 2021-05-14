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

  public:
    RememberAttributesController( QObject *parent = nullptr );
    ~RememberAttributesController() override;

    //! Restore clean/initial state: no layers, no features!
    Q_INVOKABLE void reset();

    bool rememberValuesAllowed() const;
    void setRememberValuesAllowed( bool rememberValuesAllowed );

    void storeLayerFields( const QgsVectorLayer *layer );
    void storeFeature( const FeatureLayerPair &pair );

    // Returns false if the value is not remembered
    bool rememberedValue( const QgsVectorLayer *layer, int fieldIndex, QVariant &value ) const;

    // Returns false if the value should not be remembered
    bool shouldRememberValue( const QgsVectorLayer *layer, int fieldIndex ) const;

    // Returns whether value was changed
    bool setShouldRememberValue( const QgsVectorLayer *layer, int fieldIndex, bool shouldRemember );

  signals:
    void rememberValuesAllowedChanged();

  private:
    //! Remembered values struct contains last created feature instance and a boolean vector masking attributes that should be remembered
    struct RememberedValues
    {
      QgsFeature feature;
      QVector<bool> attributeFilter;
    };

    bool mRememberValuesAllowed = false;
    //! Remembered last created feature for each layer (key)
    QHash<QString, RememberedValues> mRememberedValues;

};

#endif // REMEMBERATTRIBUTESCONTROLLER_H

