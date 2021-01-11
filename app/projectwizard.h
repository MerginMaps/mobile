#ifndef PROJECTWIZARD_H
#define PROJECTWIZARD_H

#include <QObject>
#include "fieldsmodel.h"
#include "qgsfieldmodel.h"
#include "qgsvectorlayer.h"

class ProjectWizard : public QObject
{
    Q_OBJECT
  public:
    explicit ProjectWizard( const QString &dataDir, FieldsModel *fieldsModel, QObject *parent = nullptr );
    ~ProjectWizard() = default;

    Q_INVOKABLE void createProject( QString const &projectName );
  signals:
    void projectCreated( const QString &projectDir, const QString &projectName );
  private:
    QgsVectorLayer *createGpkgLayer( QString const &projectDir );

    QString mDataDir;
    FieldsModel *mFieldsModel;
};

#endif // PROJECTWIZARD_H
