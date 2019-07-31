#ifndef MERGINPROJECTMODEL_H
#define MERGINPROJECTMODEL_H

#include <QObject>
#include <QString>
#include <QAbstractListModel>
#include <memory>
#include "merginapi.h"


/**
 * Basic information about a remote mergin project from the received list of projects.
 *
 * We add some local information for project is also available locally:
 * - general local info: project dir, downloaded version number, project status
 * - sync status: whether sync is active, progress indicator
 */
struct MerginProject
{
  QString projectName;
  QString projectNamespace;
  QString projectDir;  // full path to the project directory
  QDateTime clientUpdated; // client's version of project files
  QDateTime serverUpdated; // available latest version of project files on server
  bool pending = false; // if there is a pending request for downlaod/update a project
  ProjectStatus status = NoVersion;
  qreal progress = 0;  // progress in case of pending download/upload (values [0..1])
  int creator; // server-side user ID of the project owner (creator)
  QList<int> writers; // server-side user IDs of users having write access to the project
};

typedef QList<std::shared_ptr<MerginProject>> ProjectList;


class MerginProjectModel: public QAbstractListModel
{
    Q_OBJECT
    Q_PROPERTY( int filterCreator READ filterCreator WRITE setFilterCreator )
    Q_PROPERTY( int filterWriter READ filterWriter WRITE setFilterWriter )

  public:
    enum Roles
    {
      ProjectName = Qt::UserRole + 1,
      ProjectNamespace,
      Size,
      ProjectInfo,
      Status,
      Pending,
      PassesFilter,
      SyncProgress
    };
    Q_ENUMS( Roles )

    explicit MerginProjectModel( LocalProjectsManager &localProjects, QObject *parent = nullptr );

    Q_INVOKABLE QVariant data( const QModelIndex &index, int role ) const override;

    ProjectList projects();

    QHash<int, QByteArray> roleNames() const override;

    int rowCount( const QModelIndex &parent = QModelIndex() ) const override;

    void resetProjects( const MerginProjectList &merginProjects );

    int filterCreator() const;
    void setFilterCreator( int filterCreator );

    int filterWriter() const;
    void setFilterWriter( int filterWriter );

  public slots:
    void syncProjectStatusChanged( const QString &projectFullName, qreal progress );

  private slots:

    void projectMetadataChanged( const QString &projectDir );
    void onLocalProjectAdded( const QString &projectDir );
    void onLocalProjectRemoved( const QString &projectDir );

  private:

    int findProjectIndex( const QString &projectFullName );

    ProjectList mMerginProjects;
    LocalProjectsManager &mLocalProjects;
    int mFilterCreator;
    int mFilterWriter;

};
#endif // MERGINPROJECTMODEL_H
