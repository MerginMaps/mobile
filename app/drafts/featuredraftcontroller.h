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
#include <QPointer>

#include "featurelayerpair.h"
#include "featuredraft.h"

class QgsVectorLayer;
class QgsProject;

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

    //! The layer the pending draft belongs to (only meaningful when hasDraft is true)
    Q_PROPERTY( QgsVectorLayer *draftLayer READ draftLayer NOTIFY hasDraftChanged )

    //! Which stage the draft was interrupted at
    Q_PROPERTY( DraftStage draftStage READ draftStage NOTIFY hasDraftChanged )

    //! Whether the draft belongs to an existing feature being edited, rather than a new one being added
    Q_PROPERTY( bool draftIsExistingFeature READ draftIsExistingFeature NOTIFY hasDraftChanged )

    //! Display title of the draft's feature (via the layer's display expression), empty for a new (add-mode) draft
    Q_PROPERTY( QString draftFeatureTitle READ draftFeatureTitle NOTIFY hasDraftChanged )

  public:
    //! Shared with FeatureDraft; registered for QML as DraftStage via FeatureDraftStage
    using DraftStage = FeatureDraftStage::DraftStage;

    explicit FeatureDraftController( QObject *parent = nullptr );
    ~FeatureDraftController() override = default;

    bool hasDraft() const;
    QgsVectorLayer *draftLayer() const;
    DraftStage draftStage() const;
    bool draftIsExistingFeature() const;
    QString draftFeatureTitle() const;

    //! Rebuilds the pending draft as a FeatureLayerPair, geometry/attributes overlaid.
    //! Only the pending state (the prompt) is cleared here, the draft itself stays in storage
    Q_INVOKABLE FeatureLayerPair applyDraft();

    //! Permanently discards the pending draft for the currently active project
    Q_INVOKABLE void discardDraft();

    //! Stores the draft for the active project, replacing the previous one
    void saveDraft( const FeatureDraft &draft ) const;

    //! Removes the stored draft of the active project and the in-memory cache/pending state
    void clearDraft();

  signals:
    void hasDraftChanged();

  public slots:
    //! Sets projectId as the active project and checks it for a draft, resolving layers against project
    void checkForDraft( const QString &projectId, QgsProject *project );

  private:
    //! Resolves the layer the given draft belongs to, or nullptr if it no longer exists
    static QgsVectorLayer *resolveDraftLayer( const FeatureDraft &draft, QgsProject *project );

    // Guards: not older than 10 days, referenced fields still match the layer's
    // schema, and (for an edit-mode draft) the feature still exists.
    static bool isDraftValid( const FeatureDraft &draft, QgsVectorLayer *layer );

    void setDraft( bool hasDraft, QgsVectorLayer *layer = nullptr, DraftStage stage = DraftStage::AttributeForm, const QString &featureTitle = QString() );

    QString mProjectId;
    QPointer<QgsProject> mProject; // not owned
    bool mHasDraft = false;
    QPointer<QgsVectorLayer> mDraftLayer; // not owned
    DraftStage mDraftStage = DraftStage::AttributeForm;
    QString mDraftFeatureTitle;

    FeatureDraft mCachedDraft; // loaded in checkForDraft(), reused by applyDraft()/draftIsExistingFeature()
};

#endif // FEATUREDRAFTCONTROLLER_H
