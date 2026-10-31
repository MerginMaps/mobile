/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include "fieldprofile.h"

#include <QColor>
#include <QDir>
#include <QFile>
#include <QFileInfo>
#include <QHash>
#include <QJsonDocument>
#include <QJsonObject>
#include <QSettings>
#include <QUrl>

#include "coreutils.h"
#include "mmstyle.h"

#include "qgsproject.h"

const QString FieldProfile::FILE_NAME = QStringLiteral( "field-profile.json" );

static const QString BRANDING_COLORS_KEY = QStringLiteral( "fieldProfile/brandingColors" );
static const QString BRANDING_LOGO_KEY = QStringLiteral( "fieldProfile/brandingLogo" );

QString FieldProfile::filePath( const QgsProject *project )
{
  if ( !project || project->fileName().isEmpty() )
    return QString();

  return QFileInfo( project->fileName() ).absoluteDir().absoluteFilePath( FILE_NAME );
}

QVariantMap FieldProfile::read( const QgsProject *project )
{
  QFile file( filePath( project ) );
  if ( file.fileName().isEmpty() || !file.exists() )
    return QVariantMap();

  if ( !file.open( QIODevice::ReadOnly ) )
  {
    CoreUtils::log( QStringLiteral( "Field profile" ), QStringLiteral( "Could not read %1" ).arg( file.fileName() ) );
    return QVariantMap();
  }

  QJsonParseError parseError;
  const QJsonDocument doc = QJsonDocument::fromJson( file.readAll(), &parseError );
  if ( !doc.isObject() )
  {
    CoreUtils::log( QStringLiteral( "Field profile" ), QStringLiteral( "Invalid field profile: %1" ).arg( parseError.errorString() ) );
    return QVariantMap();
  }

  return doc.object().toVariantMap();
}

void FieldProfile::applyBranding( MMStyle *style, const QgsProject *project )
{
  const QVariantMap branding = read( project ).value( QStringLiteral( "branding" ) ).toMap();
  const QVariantMap colors = branding.value( QStringLiteral( "colors" ) ).toMap();

  QString logoPath;
  const QString logo = branding.value( QStringLiteral( "logo" ) ).toString();
  if ( !logo.isEmpty() )
  {
    logoPath = QFileInfo( filePath( project ) ).absoluteDir().absoluteFilePath( logo );
    if ( !QFileInfo::exists( logoPath ) )
    {
      CoreUtils::log( QStringLiteral( "Field profile" ), QStringLiteral( "Logo %1 not found" ).arg( logoPath ) );
      logoPath.clear();
    }
  }

  setBranding( style, colors, logoPath );

  QSettings settings;
  settings.setValue( BRANDING_COLORS_KEY, colors );
  settings.setValue( BRANDING_LOGO_KEY, logoPath );
}

void FieldProfile::restoreBranding( MMStyle *style )
{
  QSettings settings;
  const QVariantMap colors = settings.value( BRANDING_COLORS_KEY ).toMap();
  QString logoPath = settings.value( BRANDING_LOGO_KEY ).toString();

  // the project may have been removed from the device since
  if ( !logoPath.isEmpty() && !QFileInfo::exists( logoPath ) )
    logoPath.clear();

  setBranding( style, colors, logoPath );
}

void FieldProfile::setBranding( MMStyle *style, const QVariantMap &colors, const QString &logoPath )
{
  if ( !style )
    return;

  QHash<QString, QColor> brandColors;
  for ( auto it = colors.constBegin(); it != colors.constEnd(); ++it )
  {
    const QColor color = QColor::fromString( it.value().toString() );
    if ( color.isValid() )
      brandColors.insert( it.key(), color );
    else
      CoreUtils::log( QStringLiteral( "Field profile" ), QStringLiteral( "Invalid colour %1 for %2" ).arg( it.value().toString(), it.key() ) );
  }

  style->setBranding( brandColors, logoPath.isEmpty() ? QUrl() : QUrl::fromLocalFile( logoPath ) );
}
