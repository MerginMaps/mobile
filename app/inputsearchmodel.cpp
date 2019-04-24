/***************************************************************************
  inputsearchmodel.cpp
  --------------------------------------
  Date                 : May 2019
  Copyright            : (C) 2019 by Viktor Sklencar
  Email                : viktor.sklencar at lutraconsulting dot co dot uk
 ***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include "inputsearchmodel.h"

InputSearchModel::InputSearchModel( QObject *parent )
{}

QString InputSearchModel::searchExpression() const
{
  return mSearchExpression;
}

void InputSearchModel::setSearchExpression( const QString &searchExpression )
{
  if ( searchExpression != mSearchExpression )
  {
    mSearchExpression = searchExpression;
    // Hack to model changed signal
    endResetModel();
  }
}
