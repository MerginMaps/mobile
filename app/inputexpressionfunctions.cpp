/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include "inputexpressionfunctions.h"

InputExpressionFunctions::InputExpressionFunctions()
{
}

void InputExpressionFunctions::registerFunctions()
{
  QgsExpression::registerFunction( new ReadExif() );
  QgsExpression::registerFunction( new ReadExifImgDirection() );
  QgsExpression::registerFunction( new ReadExifLongitude() );
  QgsExpression::registerFunction( new ReadExifLatitude() );
}
