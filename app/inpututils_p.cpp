/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include "inpututils_p.h"

static QString htmlATag( const QColor &color, const QString &url, bool underline, bool bold )
{
  QString atag;
  // a tag start
  atag += "<a style='";

  // underline
  if ( underline )
  {
    atag += "text-decoration:underline;";
  }
  else
  {
    atag += "text-decoration:none;";
  }

  // bold
  if ( bold )
  {
    atag += " font-weight: 600;"; // semi-bold
  }

  // color
  atag += " color:" + color.name() + ";'";

  // href
  atag += " href='" + url + "'>";

  return atag;
}

QString InputUtilsPrivate::htmlLink(
  const QString &text,
  const QColor &color,
  const QString &url,
  const QString &url2,
  bool underline,
  bool bold )
{
  QString ret( text );

  if ( url2.isEmpty() )
  {
    if ( !ret.contains( "%1" ) )
      ret = "%1" + ret + "%2";

    Q_ASSERT( ret.contains( "%1" ) && ret.contains( "%2" ) );
    ret = ret.arg( htmlATag( color, url, underline, bold ) ).arg( "</a>" );
  }
  else
  {
    Q_ASSERT( ret.contains( "%1" ) && ret.contains( "%2" ) && ret.contains( "%3" ) );
    ret = ret.arg( htmlATag( color, url, underline, bold ) ).arg( htmlATag( color, url2, underline, bold ) ).arg( "</a>" );
  }

  return ret;
}
