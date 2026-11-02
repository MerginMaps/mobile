/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef PROJECTWIZARD_H
#define PROJECTWIZARD_H

#include "fieldsmodel.h"
#include "qgsfieldmodel.h"
#include "qgsvectorlayer.h"
#include "qgsmapsettings.h"

class QgsVectorTileLayer;

/**
 * Controller for creating new Input project.
 */
class ProjectWizard : public QObject
{
    Q_OBJECT
  public:
    explicit ProjectWizard( const QString &dataDir, QObject *parent = nullptr );
    ~ProjectWizard() override = default;

    /**
     * Creates a new project in unique directory named according to project name.
     * \param projectName Project's name for newly created project.
     * \param fieldsModel Fields configuration model for a new project
     */
    Q_INVOKABLE void createProject( QString const &projectName, const FieldsModel *fieldsModel );

    /**
     * Creates a ready-to-use project for ad-hoc field collection: an Observations point layer with common
     * types (e.g. inaccessible road, health warning), photo, audio and video fields and automatic surveyor,
     * time and GPS accuracy; quick capture for that layer; map sketching; position tracking; OSM base map.
     */
    Q_INVOKABLE void createFieldObservationProject( QString const &projectName );

  public slots:
    //! To append "mapcanvas" property to project file required to correctly show a project.
    void writeMapCanvasSetting( QDomDocument &doc );

  signals:
    /**
      * Emitted after a project has been created.
     */
    void projectCreationFailed( const QString &message );
    void projectCreated( const QString &projectDir, const QString &projectName );
    void notifySuccess( const QString &message );
  private:
    QgsVectorLayer *createGpkgLayer( QString const &projectDir, QList<FieldConfiguration> const &fieldsConfig );
    QgsVectorLayer *createTrackingLayer( const QString &trackingGpkgPath );
    QgsVectorLayer *createObservationsLayer( const QString &projectDir );
    QgsVectorLayer *createMapSketchesLayer( const QString &projectDir );
    QgsVectorTileLayer *createBackgroundLayer();
    bool prepareProjectDir( const QString &projectNameRaw, QString &projectDir, QString &projectName );
    void writeProject( QgsProject &project, const QString &projectDir, const QString &projectName, QgsMapLayer *extentLayer, const QList<QgsMapLayer *> &layers );
    QgsFields createFields( const QList<FieldConfiguration> &fieldsConfig ) const;
    QgsSingleSymbolRenderer *surveyLayerRenderer();
    QMetaType::Type parseType( const QString &type ) const;
    QString widgetToType( const QString &widgetType ) const;
    QString findWidgetTypeByFieldName( const QString &name, const QList<FieldConfiguration> &fieldsConfig ) const;
    //! Returns editor widget setup for the wizard widget type, "ExternalResourceAudio"/"ExternalResourceVideo" map to attachment widgets
    QgsEditorWidgetSetup editorWidgetSetup( const QgsField &field, const QString &widgetType ) const;

    QString mDataDir;
    std::unique_ptr<QgsMapSettings> mSettings = nullptr;

    const QString PROJECT_CRS_ID = QStringLiteral( "EPSG:3857" );
    const QString LAYER_CRS_ID = QStringLiteral( "EPSG:4326" );
};

#endif // PROJECTWIZARD_H
