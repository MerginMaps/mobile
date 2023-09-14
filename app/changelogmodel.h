#ifndef CHANGELOGMODEL_H
#define CHANGELOGMODEL_H

#include "QNetworkAccessManager"
#include <QAbstractListModel>
#include <QDate>

struct Changelog
{
  QString title;
  QString description;
  QString link;
  QDateTime date;
};

class ChangelogModel : public QAbstractListModel
{
    Q_OBJECT
    Q_ENUMS( MyRoles )

  public:
    enum MyRoles
    {
      TitleRole = Qt::UserRole + 1, DescriptionRole, LinkRole, DateRole
    };

    ChangelogModel( QObject *parent = nullptr );

    QHash<int, QByteArray> roleNames() const override;
    int rowCount( const QModelIndex &parent = QModelIndex() ) const override;
    QVariant data( const QModelIndex &index, int role = Qt::DisplayRole ) const override;

    Q_INVOKABLE void seeChangelogs( bool all = false );

  private slots:
    void onFinished( QNetworkReply *reply );

  signals:
    void finished( const QString &title, const QString &link );

  private:
    QList<Changelog> _logs;
    QNetworkAccessManager *_networkManager;
    QDateTime _lastSeen;
};

#endif // CHANGELOGMODEL_H
