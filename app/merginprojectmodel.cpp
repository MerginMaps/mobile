#include "merginprojectmodel.h"

#include <QAbstractListModel>
#include <QString>

MerginProjectModel::MerginProjectModel(QObject* parent)
    : QAbstractListModel( parent )
{
}

QVariant MerginProjectModel::data( const QModelIndex& index, int role ) const
{
    int row = index.row();
    if (row < 0 || row >= mMerginProjects.count())
        return QVariant("");

    const MerginProject* project = mMerginProjects.at(row).get();

    switch ( role )
    {
    case Name: return QVariant(project->name);
    }

    return QVariant();
}

QHash<int, QByteArray> MerginProjectModel::roleNames() const
{
    QHash<int, QByteArray> roleNames = QAbstractListModel::roleNames();
    roleNames[Name] = "name";
    return roleNames;
}

QModelIndex MerginProjectModel::index( int row ) const {
    return createIndex(row, 0, nullptr);
}

int MerginProjectModel::rowCount(const QModelIndex &parent) const {
    Q_UNUSED(parent);
    return mMerginProjects.count();
}

void MerginProjectModel::resetProjects(ProjectList projects)
{
    mMerginProjects.clear();
    beginResetModel();
    mMerginProjects = projects;
    endResetModel();

    emit merginProjectsChanged();
}
