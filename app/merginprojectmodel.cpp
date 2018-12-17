#include "merginprojectmodel.h"

#include <QAbstractListModel>
#include <QString>

MerginProjectModel::MerginProjectModel(MerginApi *merginApi, QObject* parent)
    : QAbstractListModel( parent )
    ,mApi(merginApi)
{
}

QVariant MerginProjectModel::data( const QModelIndex& index, int role ) const
{
    int row = index.row();
    if (row < 0 || row >= mMerginProjects.count())
        return QVariant();

    const MerginProject* project = mMerginProjects.at(row).get();

    switch ( role )
    {
    case Name: return QVariant(project->name);
    case ProjectInfo: return QVariant(project->info);
    case Status: return QVariant(project->pending);
    }

    return QVariant();
}

QHash<int, QByteArray> MerginProjectModel::roleNames() const
{
    QHash<int, QByteArray> roleNames = QAbstractListModel::roleNames();
    roleNames[Name] = "name";
    roleNames[ProjectInfo] = "projectInfo";
    roleNames[Status] = "pending";
    return roleNames;
}

QModelIndex MerginProjectModel::index( int row ) const {
    return createIndex(row, 0, nullptr);
}

ProjectList MerginProjectModel::projects()
{
    return mMerginProjects;
}

int MerginProjectModel::rowCount(const QModelIndex &parent) const {
    Q_UNUSED(parent);
    return mMerginProjects.count();
}

void MerginProjectModel::resetProjects()
{
    mMerginProjects.clear();
    beginResetModel();
    mMerginProjects = mApi->projects();
    endResetModel();

    emit merginProjectsChanged();
}

// TODO emit signal merginProjectChanged
void MerginProjectModel::downloadProjectFinished(QString projectFolder, QString projectName)
{
    Q_UNUSED(projectFolder);
    for (std::shared_ptr<MerginProject> project: mMerginProjects) {
        if (project->name == projectName) {
            beginResetModel();
            project->pending = false;
            endResetModel();
        }
    }
}
