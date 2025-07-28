/***************************************************************************
*                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include "hapticsmodel.h"

HapticsModel::HapticsModel( QObject *parent ) : QAbstractListModel( parent )
{
#ifdef DESKTOP_OS
  mHapticsTypes.append( {AppSettings::HapticsType::HapticsOff, tr( "Off" )} );
  mHapticsTypes.append( {AppSettings::HapticsType::HapticsSound, tr( "Sound" )} );
#else
  mHapticsTypes.append( {AppSettings::HapticsType::HapticsOff, tr( "Off" )} );
  mHapticsTypes.append( {AppSettings::HapticsType::HapticsVibration, tr( "Vibration" )} );
  mHapticsTypes.append( {AppSettings::HapticsType::HapticsSound, tr( "Sound" )} );
  mHapticsTypes.append( {AppSettings::HapticsType::HapticsVibrationSound, tr( "Vibration & Sound" )} );
#endif
}

int HapticsModel::rowCount( const QModelIndex &parent ) const
{
  Q_UNUSED( parent );
  return static_cast<int>( mHapticsTypes.size() );
}

QVariant HapticsModel::data( const QModelIndex &index, const int role ) const
{
  if ( !index.isValid() || index.row() >= mHapticsTypes.size() )
    return {};

  const auto [value, text] = mHapticsTypes[index.row()];
  switch ( role )
  {
    case ValueRole:
      return value;
    case TextRole:
      return text;
    default:
      return {};
  }
}

QHash<int, QByteArray> HapticsModel::roleNames() const
{
  return
  {
    {ValueRole, QStringLiteral( "value" ).toLatin1()},
    {TextRole, QStringLiteral( "text" ).toLatin1()}
  };
}

int HapticsModel::count() const
{
  return rowCount( QModelIndex() );
}
