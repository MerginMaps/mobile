#ifndef MERGINPROJECTMODEL_H
#define MERGINPROJECTMODEL_H

#include <QObject>
#include <QAbstractListModel>
#include <QString>
#include <memory>
#include "merginapi.h"

class MerginProjectModel: public QAbstractListModel
{
    Q_OBJECT

  public:
    enum Roles
    {
      Name = Qt::UserRole + 1,
      Size,
      ProjectInfo,
      Status // just "pending" for now
    };
    Q_ENUMS( Roles )

    explicit MerginProjectModel(std::shared_ptr<MerginApi> merginApi, QObject* parent = nullptr);

    Q_INVOKABLE QVariant data( const QModelIndex& index, int role ) const override;
    Q_INVOKABLE QModelIndex index( int row ) const;
    ProjectList projects();

    QHash<int, QByteArray> roleNames() const override;

    int rowCount(const QModelIndex &parent = QModelIndex()) const override;

    void resetProjects();

signals:
    void merginProjectsChanged();

public slots:
    void downloadProjectFinished(QString projectFolder, QString projectName);
  private:
    std::shared_ptr<MerginApi> mApi;
    ProjectList mMerginProjects;

};
#endif // MERGINPROJECTMODEL_H
