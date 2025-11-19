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

#include <QObject>

#include "fieldsmodel.h"
#include "qgsfieldmodel.h"
#include "qgsvectorlayer.h"
#include "qgsmapsettings.h"
#include <qgssinglesymbolrenderer.h>

/**
 * Controller for creating new Input project.
 */
class ProjectWizard : public QObject
{
    Q_OBJECT
  public:
    explicit ProjectWizard( const QString &dataDir, QObject *parent = nullptr );
    ~ProjectWizard() = default;

    /**
     * Creates a new project in unique directory named accoridng project name.
     * \param projectName Project's name for newly created project.
     * \param fieldsModel Fields configuration model for a new project
     */
    Q_INVOKABLE void createProject( QString const &projectName, FieldsModel *fieldsModel );

  public slots:
    //! To append "mapcanvas" property to project file required to correctly show a project.
    void writeMapCanvasSetting( QDomDocument &doc );

  signals:
    /**
      * Emitted after a project has been craeted.
     */
    void projectCreationFailed( const QString &message );
    void projectCreated( const QString &projectDir, const QString &projectName );
    void notifySuccess( const QString &message );
  private:
    QgsVectorLayer *createGpkgLayer( QString const &projectDir, QList<FieldConfiguration> const &fieldsConfig );
    QgsFields createFields( const QList<FieldConfiguration> fieldsConfig ) const;
    QgsSingleSymbolRenderer *surveyLayerRenderer();
    QVariant::Type parseType( const QString &type ) const;
    QString widgetToType( const QString &widgetType ) const;
    QString findWidgetTypeByFieldName( const QString name, const QList<FieldConfiguration> fieldsConfig ) const;

    QString mDataDir;
    std::unique_ptr<QgsMapSettings> mSettings = nullptr;

    const QString PROJECT_CRS_ID = QStringLiteral( "EPSG:3857" );
    const QString LAYER_CRS_ID = QStringLiteral( "EPSG:4326" );
};

#endif // PROJECTWIZARD_H
