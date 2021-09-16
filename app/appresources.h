/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#pragma once

#include <QObject>

/**
 * Namespace Resources contains const variables that can be used in different parts
 * of application in order to avoid redefinitions and repetitions
 */
namespace Resources
{

  namespace Texts
  {

    namespace Validation
    {

      const QString numberValidationFailed = QObject::tr( "Value must be a number" );
      const QString numberUpperBoundReached = QObject::tr( "Value must be lower than %1" );
      const QString numberLowerBoundReached = QObject::tr( "Value must be higher than %1" );
      const QString numberExceedingVariableLimits = QObject::tr( "Value is too large" );

      const QString textTooLong = QObject::tr( "Can not be longer than %1 characters" );

      const QString softNotNullFailed = QObject::tr( "Field should not be empty" );
      const QString hardNotNullFailed = QObject::tr( "Field must not be empty" );
      const QString softUniqueFailed = QObject::tr( "Value should be unique" );
      const QString hardUniqueFailed = QObject::tr( "Value must be unique" );
      const QString softExpressionFailed = QObject::tr( "Unmet QGIS expression constraint" );
      const QString hardExpressionFailed = QObject::tr( "Unmet QGIS expression constraint" );

      const QString genericValidationFailed = QObject::tr( "Not a valid value" );
    }
  }
}
