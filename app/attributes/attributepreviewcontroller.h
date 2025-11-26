/***************************************************************************
  attributepreviewcontroller.h
  --------------------------------------
  Date                 : 5.5.2021
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
#ifndef ATTRIBUTEPREVIEWCONTROLLER_H
#define ATTRIBUTEPREVIEWCONTROLLER_H

#include <QAbstractListModel>
#include <QString>

#include "qgsproject.h"
#include "featurelayerpair.h"

class QgsExpressionContextScope;
class QgsVectorLayer;

/**
 * Simple name-value model to be used in the preview panel
 * for feature attributes in "fields" mode
 *
 * \note QML Type: AttributePreviewModel
 */
class AttributePreviewModel : public QAbstractListModel
{
    Q_OBJECT
  public:

    AttributePreviewModel(
      const QVector<QPair<QString, QString>> &items = QVector<QPair<QString, QString >> ()
    );

    ~AttributePreviewModel() override;

    enum AttributePreviewRoles
    {
      Name = Qt::UserRole + 1,
      Value
    };

    Q_ENUM( AttributePreviewRoles )

    QHash<int, QByteArray> roleNames() const override;
    Q_INVOKABLE int rowCount( const QModelIndex &parent = QModelIndex() ) const override;
    QVariant data( const QModelIndex &index, int role = Qt::DisplayRole ) const override;

  private:
    QVector<QPair<QString, QString>> mItems; //!< pair of name&value
};

/*
* The definition is in the mapTip of the layer.
*
* mapTip is set in QGIS>LayerProperties>Display>MapTip
*
* It has 5 modes:
*  1. qgis' mapTip is empty:
*         It creates limitFields() fields that are
*         different from the display field, so the
*         fields in the model are not the same as in
*         the preview panel title
*      => PreviewType.Fields
*      => not supported by QGIS
*
*  2. qgis' mapTip contains "# fields", following by one
*         "display name" per line. Only first mLimit
*         fields are shown.
*      => PreviewType.Fields
*      => not supported by QGIS
*
*  3. qgis' mapTip contains "# image", following by relative
*         path to the image
*     => PreviewType.Image
*     => not supported by QGIS
*
*  4. qgis' mapTip contains some (html) text
*     => PreviewType.Html
*     => supported by QGIS
*
*  5. we are unable to parse the mapTip or there are no fields
*         in the layer
*    => PreviewType.Empty
*    => supported by QGIS
*/
class AttributePreviewController: public QObject
{
    Q_OBJECT

    Q_PROPERTY( FeatureLayerPair featureLayerPair READ featureLayerPair WRITE setFeatureLayerPair NOTIFY featureLayerPairChanged )
    Q_PROPERTY( QgsProject *project READ project WRITE setProject NOTIFY projectChanged )

    // never nullptr
    Q_PROPERTY( AttributePreviewModel *fieldModel READ fieldModel NOTIFY featureLayerPairChanged )
    Q_PROPERTY( QString html READ html NOTIFY featureLayerPairChanged )
    Q_PROPERTY( QString photo READ photo NOTIFY featureLayerPairChanged )
    Q_PROPERTY( PreviewType type READ type NOTIFY featureLayerPairChanged )
    Q_PROPERTY( QString title READ title NOTIFY featureLayerPairChanged )

  public:
    enum PreviewType
    {
      Empty = 1,
      Photo,
      HTML,
      Fields
    };
    Q_ENUMS( PreviewType )

    AttributePreviewController( QObject *parent = nullptr );
    ~AttributePreviewController() override;

    //! Restore clean/initial state: no layer, no feature!
    Q_INVOKABLE void reset();

    void setFeatureLayerPair( const FeatureLayerPair &pair );
    FeatureLayerPair featureLayerPair() const;
    void setProject( QgsProject *project );
    QgsProject *project() const;
    AttributePreviewModel *fieldModel() const;
    QString html() const;
    QString photo() const;
    QString title() const;
    PreviewType type() const;

  signals:
    void featureLayerPairChanged();
    void projectChanged();

  private:
    QList<QgsExpressionContextScope *> globalProjectLayerScopes( const QgsMapLayer *layer );
    void recalculate();
    QString mapTipImage();
    QVector<QPair<QString, QString>> mapTipFields();
    QString mapTipHtml();
    QString featureTitle();
    QString formatDateForPreview( const QgsField &field, const QVariant &value, const QVariantMap &fieldCfg ) const;

    QgsProject *mProject = nullptr;
    FeatureLayerPair mFeatureLayerPair;
    QString mHtml;
    QString mPhoto;
    QString mTitle;
    std::unique_ptr<AttributePreviewModel> mFieldModel;
    PreviewType mType = PreviewType::Empty;
    const int mLimit = 3;
};

#endif // ATTRIBUTEPREVIEWCONTROLLER_H
