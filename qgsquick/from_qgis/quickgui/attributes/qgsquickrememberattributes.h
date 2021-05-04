/***************************************************************************
  qgsquickrememberattributes.h
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

#ifndef QGSQUICKREMEMBERATTRIBUTES_H
#define QGSQUICKREMEMBERATTRIBUTES_H

#include "qgis_quick.h"
#include "qgsfeature.h"
#include "qgsvectorlayer.h"

#include <QHash>

/**
 * \ingroup quick
 *
 * \note QML Type: RememberAttributes
 *
 * \since QGIS 3.20
 */
class QUICK_EXPORT QgsQuickRememberAttributes : public QObject
{
    Q_OBJECT

    //! Returns TRUE if remembering values is allowed
    Q_PROPERTY( bool rememberValuesAllowed READ rememberValuesAllowed WRITE setRememberValuesAllowed NOTIFY rememberValuesAllowedChanged )

  public:
    QgsQuickRememberAttributes( QObject *parent = nullptr );
    ~QgsQuickRememberAttributes() override;

    bool rememberValuesAllowed() const;
    void setRememberValuesAllowed( bool rememberValuesAllowed );

    void storeLayerFields( const QgsVectorLayer *layer );

    // Returns false if the value is not remembered
    bool rememberedValue( const QgsVectorLayer *layer, int fieldIndex, QVariant &value ) const;
    // Returns false if the value should not be remembered
    bool shouldRememberValue( const QgsVectorLayer *layer, int fieldIndex ) const;

    // Returns whether value was changed
    bool setShouldRememberValue( const QgsVectorLayer *layer, int fieldIndex, bool shouldRemember );

  signals:
    void rememberValuesAllowedChanged();

  public slots:

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

#endif // QGSQUICKREMEMBERATTRIBUTES_H

