#ifndef MERGINPROJECTMODEL_H
#define MERGINPROJECTMODEL_H

#include <QObject>
#include <QString>
#include <memory>
#include "merginapi.h"
#include "inputsearchmodel.h"

class MerginProjectModel: public InputSearchModel
{
    Q_OBJECT

  public:
    enum Roles
    {
      Name = Qt::UserRole + 1,
      Size,
      ProjectInfo,
      Status,
      Pending,
      PassesFilter
    };
    Q_ENUMS( Roles )

    explicit MerginProjectModel( QObject *parent = nullptr );

    Q_INVOKABLE QVariant data( const QModelIndex &index, int role ) const override;
    Q_INVOKABLE void setPending( int row, bool pending );
    ProjectList projects();

    QHash<int, QByteArray> roleNames() const override;

    int rowCount( const QModelIndex &parent = QModelIndex() ) const override;

    void resetProjects( const ProjectList &merginProjects );

  public slots:
    void syncProjectFinished( const QString &projectFolder, const QString &projectName, bool successfully );
  private:
    ProjectList mMerginProjects;

};
#endif // MERGINPROJECTMODEL_H
