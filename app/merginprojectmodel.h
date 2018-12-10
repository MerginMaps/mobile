#ifndef MERGINPROJECTMODEL_H
#define MERGINPROJECTMODEL_H

#include <QObject>
#include <QAbstractListModel>
#include <QString>

struct MerginProject {
    QString name;
    QStringList tags;
    //QDate created;
};
typedef QList<std::shared_ptr<MerginProject>> ProjectList;

class MerginProjectModel: public QAbstractListModel
{
    Q_OBJECT

  public:
    enum Roles
    {
      Name = Qt::UserRole + 1,
      Size
    };
    Q_ENUMS( Roles )

    explicit MerginProjectModel(QObject* parent = nullptr);

    Q_INVOKABLE QVariant data( const QModelIndex& index, int role ) const override;
    Q_INVOKABLE QModelIndex index( int row ) const;

    QHash<int, QByteArray> roleNames() const override;

    int rowCount(const QModelIndex &parent = QModelIndex()) const override;

    void resetProjects(ProjectList projects);

signals:
    void merginProjectsChanged();

  private:
    ProjectList mMerginProjects;

};
#endif // MERGINPROJECTMODEL_H
