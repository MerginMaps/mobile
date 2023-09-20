#include "ChangelogModel.h"
#include <QNetworkReply>
#include <QXmlStreamReader>
#include <QDebug>

ChangelogModel::ChangelogModel( QObject *parent ) : QAbstractListModel{parent}
{
  _networkManager = new QNetworkAccessManager( this );
  connect( _networkManager, &QNetworkAccessManager::finished, this, &ChangelogModel::onFinished );
}

void ChangelogModel::onFinished( QNetworkReply *reply )
{
  if ( reply->error() == QNetworkReply::NoError )
  {
    QXmlStreamReader xml( reply );
    QString title, description, link, pubDate;
    while ( !xml.atEnd() )
    {
      xml.readNext();
      if ( xml.isStartElement() )
      {
        if ( xml.name().toString() == "item" )
        {
          title.clear();
          description.clear();
          link.clear();
          pubDate.clear();
        }
        else if ( xml.name().toString() == "title" )
        {
          title = xml.readElementText();
        }
        else if ( xml.name().toString() == "description" )
        {
          description = xml.readElementText();
        }
        else if ( xml.name().toString() == "link" )
        {
          link = xml.readElementText();
        }
        else if ( xml.name().toString() == "pubDate" )
        {
          pubDate = xml.readElementText();
        }
      }
      else if ( xml.isEndElement() )
      {
        if ( xml.name().toString() == "item" )
        {
          QDateTime dt = QDateTime::fromString( pubDate, "ddd, dd MMM yyyy hh:mm:ss t" );
          if ( dt > _lastSeen )
          {
            beginInsertRows( QModelIndex(), rowCount(), rowCount() );
            _logs << Changelog{ title, description, link, dt };
            endInsertRows();
          }
        }
      }
    }
    if ( xml.hasError() )
    {
      qWarning() << "XML error:" << xml.errorString();
    }
  }
  else
  {
    qWarning() << "Network error:" << reply->errorString();
  }
  reply->deleteLater();

  if ( !_logs.isEmpty() )
  {
    emit dataChanged( createIndex( 0, 0 ), createIndex( rowCount(), 0 ) );
  }
}

QHash<int, QByteArray> ChangelogModel::roleNames() const
{
  return
  {
    { TitleRole, "title" },
    { DescriptionRole, "description" },
    { LinkRole, "link" },
    { DateRole, "date" }
  };
}

int ChangelogModel::rowCount( const QModelIndex &parent ) const
{
  Q_UNUSED( parent )
  return _logs.size();
}

QVariant ChangelogModel::data( const QModelIndex &index, int role ) const
{
  if ( !hasIndex( index.row(), index.column(), index.parent() ) )
    return {};

  Changelog log = _logs.at( index.row() );
  if ( role == TitleRole ) return log.title;
  if ( role == DescriptionRole ) return log.description;
  if ( role == LinkRole ) return log.link;
  if ( role == DateRole ) return log.date.toString( "ddd, dd MMMM yyyy" );

  return {};
}

// TODO: if all=false, get the date of last seen
void ChangelogModel::seeChangelogs( bool all )
{
  beginResetModel();
  _logs.clear();
  endResetModel();

  // get all the changes
  _lastSeen = QDateTime::fromMSecsSinceEpoch( 0 );
  _networkManager->get( QNetworkRequest( QUrl( "https://wishlist.merginmaps.com/rss/changelog.xml" ) ) ); // TODO get URL from somewhere
}


