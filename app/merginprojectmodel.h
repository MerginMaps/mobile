#ifndef MERGINPROJECTMODEL_H
#define MERGINPROJECTMODEL_H

#include <QObject>
#include <QString>
#include <QAbstractListModel>
#include <memory>
#include "merginapi.h"

class MerginProjectModel: public QAbstractListModel
{
    Q_OBJECT
    Q_PROPERTY( int filterCreator READ filterCreator WRITE setFilterCreator )
    Q_PROPERTY( int filterWriter READ filterWriter WRITE setFilterWriter )

  public:
    enum Roles
    {
      Name = Qt::UserRole + 1,
      ProjectNamespace,
      Size,
      ProjectInfo,
      Status,
      Pending,
      PassesFilter,
      SyncProgress
    };
    Q_ENUMS( Roles )

    explicit MerginProjectModel( QObject *parent = nullptr );

    Q_INVOKABLE QVariant data( const QModelIndex &index, int role ) const override;
    Q_INVOKABLE void setPending( int row, bool pending );
    ProjectList projects();

    QHash<int, QByteArray> roleNames() const override;

    int rowCount( const QModelIndex &parent = QModelIndex() ) const override;

    void resetProjects( const ProjectList &merginProjects );

    int filterCreator() const;
    void setFilterCreator( int filterCreator );

    int filterWriter() const;
    void setFilterWriter( int filterWriter );

  public slots:
    void syncProjectFinished( const QString &projectFolder, const QString &projectFullName, bool successfully );
    void syncProgressUpdated( const QString &projectFullName, qreal progress );
  private:

    int findProjectIndex( const QString &projectFullName );

    ProjectList mMerginProjects;
    int mFilterCreator;
    int mFilterWriter;

};
#endif // MERGINPROJECTMODEL_H
