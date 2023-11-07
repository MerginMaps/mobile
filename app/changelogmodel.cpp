/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include "changelogmodel.h"
#include <QNetworkReply>
#include <QXmlStreamReader>
#include <QDebug>
#include "coreutils.h"
#include "inputhelp.h"

ChangelogModel::ChangelogModel( QObject *parent ) : QAbstractListModel{parent}
{
  mNetworkManager = new QNetworkAccessManager( this );
  connect( mNetworkManager, &QNetworkAccessManager::finished, this, &ChangelogModel::onFinished );
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
          if ( dt > mLastSeen )
          {
            beginInsertRows( QModelIndex(), rowCount(), rowCount() );
            mLogs << Changelog{ title, description, link, dt };
            endInsertRows();
          }
        }
      }
    }
    if ( xml.hasError() )
    {
      CoreUtils::log( QStringLiteral( "Changelog" ), QStringLiteral( "Failed to parse changelog. Xml parse error: " ).arg( xml.errorString() ) );
    }
  }
  else
  {
    CoreUtils::log( QStringLiteral( "Changelog" ), QStringLiteral( "Failed to get changelog. Server Error: %1" ).arg( reply->errorString() ) );
    emit errorMsgChanged( reply->errorString() );
  }
  reply->deleteLater();

  if ( !mLogs.isEmpty() )
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
  return mLogs.size();
}

QVariant ChangelogModel::data( const QModelIndex &index, int role ) const
{
  if ( !hasIndex( index.row(), index.column(), index.parent() ) )
    return {};

  Changelog log = mLogs.at( index.row() );
  if ( role == TitleRole ) return log.title;
  if ( role == DescriptionRole ) return log.description;
  if ( role == LinkRole ) return log.link;
  if ( role == DateRole ) return log.date;

  return {};
}

// fill the dialog
void ChangelogModel::seeChangelogs()
{
  beginResetModel();
  mLogs.clear();
  endResetModel();
  mNetworkManager->get( QNetworkRequest( QUrl( InputHelp::changelogLink() ) ) );
}


