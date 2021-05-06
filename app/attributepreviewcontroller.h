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
#include <QPair>
#include <QVector>
#include <QString>

#include "qgsquickfeaturelayerpair.h"


class QgsExpressionContextScope;
class QgsVectorLayer;
class QgsProject;

/**
 * Simple name-value model to be used in the preview panel
 * for feature attributes
 *
 * The definition is in the mapTip of the layer.
 *
 * It has 2 modes:
 *  1. mapTip is empty:
 *         It creates limitFields() fields that are
 *         different from the display field, so the
 *         fields in the model are not the same as in
 *         the preview panel title
 *
 *  2. mapTipe
 *
 * Intended use is to call resetModel() before opening the panel to
 * reset the model items.
 *
 * \note QML Type: AttributePreviewModel
 */
class AttributePreviewModel : public QAbstractListModel
{
    Q_OBJECT
  public:

    AttributePreviewModel(
      const QVector<QPair<QString, QString>> &items = QVector<QPair<QString, QString>>()
    );

    ~AttributePreviewModel() override;

    enum AttributePreviewRoles
    {
      Name = Qt::UserRole + 1,
      Value
    };

    Q_ENUM( AttributePreviewRoles )

    QHash<int, QByteArray> roleNames() const override;
    int rowCount( const QModelIndex &parent = QModelIndex() ) const override;
    QVariant data( const QModelIndex &index, int role = Qt::DisplayRole ) const override;

  private:
    QVector<QPair<QString, QString>> mItems; //!< pair of name&value
};

class AttributePreviewController: public QObject
{
    Q_OBJECT

    Q_PROPERTY( QgsQuickFeatureLayerPair featureLayerPair WRITE setFeatureLayerPair NOTIFY featureLayerPairChanged )
    Q_PROPERTY( QgsProject *project WRITE setProject NOTIFY projectChanged )

    // never nullprt
    Q_PROPERTY( AttributePreviewModel *fieldModel READ fieldModel NOTIFY fieldModelChanged )
    Q_PROPERTY( QString html READ html NOTIFY htmlChanged )
    Q_PROPERTY( QString photo READ photo NOTIFY photoChanged )
    Q_PROPERTY( PreviewType type READ type NOTIFY typeChanged )
    Q_PROPERTY( QString title READ title NOTIFY titleChanged )

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

    void setFeatureLayerPair( const QgsQuickFeatureLayerPair &pair );
    void setProject( QgsProject *project );

    AttributePreviewModel *fieldModel() const;
    QString html() const;
    QString photo() const;
    QString title() const;
    PreviewType type() const;

  signals:
    void featureLayerPairChanged();
    void htmlChanged();
    void photoChanged();
    void typeChanged();
    void fieldModelChanged();
    void titleChanged();
    void projectChanged();

  private:
    QList<QgsExpressionContextScope *> globalProjectLayerScopes( QgsMapLayer *layer );
    void recalculate();
    QString mapTipImage();
    QVector<QPair<QString, QString> > mapTipFields();
    QString mapTipHtml();
    QString featureTitle();

    QgsProject *mProject = nullptr;
    QgsQuickFeatureLayerPair mFeatureLayerPair;
    QString mHtml;
    QString mPhoto;
    QString mTitle;
    std::unique_ptr<AttributePreviewModel> mFieldModel;
    PreviewType mType = PreviewType::Empty;
    const int mLimit = 3;



};

#endif // ATTRIBUTEPREVIEWMODEL_H
