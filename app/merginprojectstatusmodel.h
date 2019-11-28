#ifndef MERGINPROJECTSTATUSMODEL_H
#define MERGINPROJECTSTATUSMODEL_H

#include <QObject>
#include <QAbstractListModel>
#include "merginapi.h"

struct ProjectStatusItem
{
  QString status;
  QString text;
  QString section;
};


class MerginProjectStatusModel : public QAbstractListModel
{
    Q_OBJECT

  public:
    enum Roles
    {
      Status = Qt::UserRole + 1, // added, removed, modified
      Text,
      Section
    };
    Q_ENUMS( Roles )

    explicit MerginProjectStatusModel( LocalProjectsManager &localProjects, QObject *parent = nullptr );

    int rowCount( const QModelIndex &parent ) const override;
    QHash<int, QByteArray> roleNames() const override;
    Q_INVOKABLE QVariant data( const QModelIndex &index, int role ) const override;

  signals:

  public slots:
    void infoProjectUpdated( const ProjectDiff &projectDiff, const QString &projectDir );

  private:
    void insertIntoItems( const QSet<QString> &files, const QString &status, const QString &projectDir );

    ProjectDiff mProjectDiff;
    QList<ProjectStatusItem> mItems;



};

#endif // MERGINPROJECTSTATUSMODEL_H
