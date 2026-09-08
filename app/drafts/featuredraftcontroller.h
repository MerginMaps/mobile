/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef FEATUREDRAFTCONTROLLER_H
#define FEATUREDRAFTCONTROLLER_H

#include <QObject>
#include <QString>
#include <QJsonObject>

#include "featurelayerpair.h"
#include "qgsgeometry.h"

class QgsVectorLayer;

/**
 * Checks whether the currently active project has a recoverable feature edit
 * draft and exposes it to QML so a notification can be shown.
 *
 * The actual notification UI is handled elsewhere - this class only detects a draft
 * and performs the resume/discard action once the user has decided.
 */
class FeatureDraftController : public QObject
{
    Q_OBJECT

    //! Whether there is a recoverable draft for the currently active project
    Q_PROPERTY( bool hasDraft READ hasDraft NOTIFY hasDraftChanged )

    //! Name of the layer the pending draft belongs to (only meaningful when hasDraft is true)
    Q_PROPERTY( QString draftLayerName READ draftLayerName NOTIFY hasDraftChanged )

    //! The layer the pending draft belongs to (only meaningful when hasDraft is true)
    Q_PROPERTY( QgsVectorLayer *draftLayer READ draftLayer NOTIFY hasDraftChanged )

    //! Which stage the draft was interrupted at: "geometryCapture" or "attributeForm"
    Q_PROPERTY( QString draftStage READ draftStage NOTIFY hasDraftChanged )

    //! Whether the draft belongs to an existing feature being edited, rather than a new one being added
    Q_PROPERTY( bool draftIsEdit READ draftIsEdit NOTIFY hasDraftChanged )

    //! Display title of the draft's feature (via the layer's display expression), empty for a new (add-mode) draft
    Q_PROPERTY( QString draftFeatureTitle READ draftFeatureTitle NOTIFY hasDraftChanged )

  public:
    explicit FeatureDraftController( QObject *parent = nullptr );
    ~FeatureDraftController() override = default;

    bool hasDraft() const;
    QString draftLayerName() const;
    QgsVectorLayer *draftLayer() const;
    QString draftStage() const;
    bool draftIsEdit() const;
    QString draftFeatureTitle() const;

    // Rebuilds the draft as a FeatureLayerPair, geometry/attributes overlaid. Draft
    // stays in storage - the resumed form clears or updates it as usual.
    Q_INVOKABLE FeatureLayerPair resumeDraft();

    // For a new-feature geometry-capture draft: returns just the geometry, to feed
    // into RecordingMapTool::resumeCapture() instead of opening the form.
    Q_INVOKABLE QgsGeometry resumeGeometryDraft();

    //! Permanently discards the pending draft for the currently active project
    Q_INVOKABLE void discardDraft();

  signals:
    void hasDraftChanged();

  public slots:
    //! Checks the active project (QgsProject::instance()) for a pending draft
    void checkForDraft();

  private:
    //! Resolves the layer the given draft belongs to, or nullptr if it no longer exists
    QgsVectorLayer *resolveDraftLayer( const QJsonObject &draft ) const;

    // Guards: not older than 10 days, referenced fields still match the layer's
    // schema, and (for an edit-mode draft) the feature still exists.
    bool isDraftValid( const QJsonObject &draft, QgsVectorLayer *layer ) const;

    void setDraft( bool hasDraft, QgsVectorLayer *layer = nullptr, const QString &stage = QString(), bool isEdit = false, const QString &featureTitle = QString() );

    bool mHasDraft = false;
    QString mDraftLayerName;
    QgsVectorLayer *mDraftLayer = nullptr; // not owned
    QString mDraftStage;
    bool mDraftIsEdit = false;
    QString mDraftFeatureTitle;
};

#endif // FEATUREDRAFTCONTROLLER_H
