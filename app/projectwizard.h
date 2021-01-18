#ifndef PROJECTWIZARD_H
#define PROJECTWIZARD_H

#include <QObject>
#include "fieldsmodel.h"
#include "qgsfieldmodel.h"
#include "qgsvectorlayer.h"

/**
 * Controller for creating new Input project.
 */
class ProjectWizard : public QObject
{
    Q_OBJECT
  public:
    explicit ProjectWizard( const QString &dataDir, FieldsModel *fieldsModel, QObject *parent = nullptr );
    ~ProjectWizard() = default;

    /**
     * Creates a new project in unique directory named accoridng project name.
     * \param projectName  Project's name for newly created project.
     */
    Q_INVOKABLE void createProject( QString const &projectName );
  signals:
    /**
      * Emitted after a project has been craeted.
     */
    void projectCreated( const QString &projectDir, const QString &projectName );
    void notify( const QString &message );
  private:
    QgsVectorLayer *createGpkgLayer( QString const &projectDir );
    QgsEditorWidgetSetup getEditorWidget( QgsField const &field, const QString &widgetType );
    QgsFields createFields( const QList<FieldConfiguration> fieldsConfig ) const;
    QVariant::Type parseType( const QString &type ) const;
    QString widgetToType( const QString &widgetType ) const;

    QString mDataDir;
    FieldsModel *mFieldsModel;
};

#endif // PROJECTWIZARD_H
