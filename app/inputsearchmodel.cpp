#include "inputsearchmodel.h"

InputSearchModel::InputSearchModel( QObject *parent )
{
}

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
