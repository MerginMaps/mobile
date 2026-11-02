/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include "termstranslator.h"

#include <algorithm>

TermsTranslator::TermsTranslator( QTranslator *baseTranslator, QObject *parent )
  : QTranslator( parent )
  , mBaseTranslator( baseTranslator )
{
}

QString TermsTranslator::translate( const char *context, const char *sourceText, const char *disambiguation, int n ) const
{
  if ( mLookup.isEmpty() || !sourceText )
    return QString();

  QString text;
  if ( mBaseTranslator )
    text = mBaseTranslator->translate( context, sourceText, disambiguation, n );
  if ( text.isEmpty() )
    text = QString::fromUtf8( sourceText );

  const QString replaced = replaceTerms( text );

  // an empty result lets the other translators answer as usual
  return replaced == text ? QString() : replaced;
}

bool TermsTranslator::isEmpty() const
{
  return mLookup.isEmpty();
}

void TermsTranslator::setTerms( const QVariantMap &terms )
{
  mTerms = terms;
  mLookup.clear();

  QStringList keys;
  for ( auto it = terms.constBegin(); it != terms.constEnd(); ++it )
  {
    const QString term = it.key().trimmed();
    const QString replacement = it.value().toString();
    if ( term.isEmpty() || replacement.isEmpty() )
      continue;
    mLookup.insert( term.toLower(), replacement );
    keys << QRegularExpression::escape( term );
  }

  // longest first, so a phrase wins over a word it contains
  std::sort( keys.begin(), keys.end(), []( const QString & a, const QString & b ) { return a.size() > b.size(); } );

  // whole words only, and not inside links or file names (e.g. "https://merginmaps.com/projects")
  mPattern = QRegularExpression( QStringLiteral( "(?<![\\w/.-])(%1)(?![\\w/-]|\\.\\w)" ).arg( keys.join( QLatin1Char( '|' ) ) ),
                                 QRegularExpression::CaseInsensitiveOption | QRegularExpression::UseUnicodePropertiesOption );
}

QVariantMap TermsTranslator::terms() const
{
  return mTerms;
}

QString TermsTranslator::replaceTerms( const QString &text ) const
{
  if ( mLookup.isEmpty() || !mPattern.isValid() )
    return text;

  QString result;
  qsizetype last = 0;
  QRegularExpressionMatchIterator it = mPattern.globalMatch( text );
  while ( it.hasNext() )
  {
    const QRegularExpressionMatch match = it.next();
    const QString found = match.captured( 1 );
    QString replacement = mLookup.value( found.toLower() );

    // keep the capital letter of the app text: "Projects" -> "Missions", "projects" -> "missions"
    if ( !replacement.isEmpty() && found.at( 0 ).isUpper() )
      replacement[0] = replacement.at( 0 ).toUpper();
    else if ( !replacement.isEmpty() && found.at( 0 ).isLower() && !( replacement.size() > 1 && replacement.at( 1 ).isUpper() ) )
      replacement[0] = replacement.at( 0 ).toLower(); // but not acronyms such as "MSF"

    result += text.mid( last, match.capturedStart( 1 ) - last ) + replacement;
    last = match.capturedEnd( 1 );
  }
  result += text.mid( last );
  return result;
}
