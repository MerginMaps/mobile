/***************************************************************************
  inputsearchmodel.h
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

#ifndef INPUTSEARCHMODEL_H
#define INPUTSEARCHMODEL_H

#include <QObject>
#include <QAbstractListModel>

class InputSearchModel: public QAbstractListModel
{
    Q_OBJECT
    Q_PROPERTY( QString searchExpression READ searchExpression WRITE setSearchExpression )

  public:
    explicit InputSearchModel( QObject *parent = nullptr );
    QString searchExpression() const;
    void setSearchExpression( const QString &searchExpression );

  protected:
    QString mSearchExpression;
};

#endif // INPUTSEARCHMODEL_H
