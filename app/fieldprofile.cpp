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
#include <QFontDatabase>
#include <QGuiApplication>
#include <QHash>
#include <QJsonDocument>
#include <QJsonObject>
#include <QSettings>
#include <QUrl>

#include "coreutils.h"
#include "mmstyle.h"
#include "termstranslator.h"

#include "qgsproject.h"

const QString FieldProfile::FILE_NAME = QStringLiteral( "field-profile.json" );

static const QString BRANDING_COLORS_KEY = QStringLiteral( "fieldProfile/brandingColors" );
static const QString BRANDING_LOGO_KEY = QStringLiteral( "fieldProfile/brandingLogo" );
static const QString BRANDING_FONT_KEY = QStringLiteral( "fieldProfile/brandingFont" );
static const QString TERMS_KEY = QStringLiteral( "fieldProfile/terms" );

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

  const QDir projectDir = QFileInfo( filePath( project ) ).absoluteDir();
  // one file or a list of files, e.g. the regular and bold font
  auto brandingFiles = [&branding, &projectDir]( const QString & key )
  {
    QStringList paths;
    const QStringList values = branding.value( key ).toStringList();
    for ( const QString &value : values )
    {
      const QString path = projectDir.absoluteFilePath( value );
      if ( QFileInfo::exists( path ) )
        paths << path;
      else
        CoreUtils::log( QStringLiteral( "Field profile" ), QStringLiteral( "Branding %1 %2 not found" ).arg( key, path ) );
    }
    return paths;
  };

  const QString logoPath = brandingFiles( QStringLiteral( "logo" ) ).value( 0 );
  const QStringList fontPaths = brandingFiles( QStringLiteral( "font" ) );

  setBranding( style, colors, logoPath, fontPaths );

  QSettings settings;
  settings.setValue( BRANDING_COLORS_KEY, colors );
  settings.setValue( BRANDING_LOGO_KEY, logoPath );
  settings.setValue( BRANDING_FONT_KEY, fontPaths );
}

void FieldProfile::restoreBranding( MMStyle *style )
{
  QSettings settings;
  const QVariantMap colors = settings.value( BRANDING_COLORS_KEY ).toMap();
  QString logoPath = settings.value( BRANDING_LOGO_KEY ).toString();
  QStringList fontPaths = settings.value( BRANDING_FONT_KEY ).toStringList();

  // the project may have been removed from the device since
  if ( !logoPath.isEmpty() && !QFileInfo::exists( logoPath ) )
    logoPath.clear();
  fontPaths.removeIf( []( const QString & path ) { return !QFileInfo::exists( path ); } );

  setBranding( style, colors, logoPath, fontPaths );
}

bool FieldProfile::applyTerms( TermsTranslator *translator, const QgsProject *project )
{
  if ( !translator )
    return false;

  const QVariantMap terms = read( project ).value( QStringLiteral( "terms" ) ).toMap();
  QSettings().setValue( TERMS_KEY, terms );

  if ( terms == translator->terms() )
    return false;

  translator->setTerms( terms );
  return true;
}

void FieldProfile::restoreTerms( TermsTranslator *translator )
{
  if ( translator )
    translator->setTerms( QSettings().value( TERMS_KEY ).toMap() );
}

QString FieldProfile::fontFamily( const QStringList &fontPaths )
{
  static QHash<QString, QString> sFamilies; // font file -> family, a font file is loaded only once

  for ( const QString &fontPath : fontPaths )
  {
    if ( sFamilies.contains( fontPath ) )
      continue;

    const int id = QFontDatabase::addApplicationFont( fontPath );
    const QStringList families = QFontDatabase::applicationFontFamilies( id );
    if ( id == -1 || families.isEmpty() )
      CoreUtils::log( QStringLiteral( "Field profile" ), QStringLiteral( "Could not load font %1" ).arg( fontPath ) );
    sFamilies.insert( fontPath, families.value( 0 ) );
  }

  // the family of the first font, the other files are its weights and styles
  return fontPaths.isEmpty() ? QString() : sFamilies.value( fontPaths.first() );
}

void FieldProfile::setBranding( MMStyle *style, const QVariantMap &colors, const QString &logoPath, const QStringList &fontPaths )
{
  if ( !style )
    return;

  // texts not using the style fonts follow the application font
  static const QFont sDefaultFont = QGuiApplication::font();
  const QString family = fontFamily( fontPaths );
  QFont appFont = sDefaultFont;
  if ( !family.isEmpty() )
    appFont.setFamily( family );
  QGuiApplication::setFont( appFont );

  QHash<QString, QColor> brandColors;
  for ( auto it = colors.constBegin(); it != colors.constEnd(); ++it )
  {
    const QColor color = QColor::fromString( it.value().toString() );
    if ( color.isValid() )
      brandColors.insert( it.key(), color );
    else
      CoreUtils::log( QStringLiteral( "Field profile" ), QStringLiteral( "Invalid colour %1 for %2" ).arg( it.value().toString(), it.key() ) );
  }

  style->setBranding( brandColors, logoPath.isEmpty() ? QUrl() : QUrl::fromLocalFile( logoPath ), family );
}
