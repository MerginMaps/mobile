/**************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

/*
 * The source code forked from https://github.com/qgis/QGIS on 25th Nov 2022
 * File: qgsquickcoordinatetransformer.cpp by (C) 2017 by Matthias Kuhn
 */

#include "inputcoordinatetransformer.h"

#include "positionkit.h"
#include "qgslogger.h"

InputCoordinateTransformer::InputCoordinateTransformer( QObject *parent )
  : QObject( parent )
{
  mCoordinateTransform.setSourceCrs( PositionKit::positionCrs3DEllipsoidHeight() );
}

QgsPoint InputCoordinateTransformer::projectedPosition() const
{
  return mProjectedPosition;
}

QgsPoint InputCoordinateTransformer::sourcePosition() const
{
  return mSourcePosition;
}

void InputCoordinateTransformer::setSourcePosition( const QgsPoint &sourcePosition )
{
  if ( mSourcePosition == sourcePosition )
    return;

  mSourcePosition = sourcePosition;

  emit sourcePositionChanged();
  updatePosition();
}

QgsCoordinateReferenceSystem InputCoordinateTransformer::destinationCrs() const
{
  return mCoordinateTransform.destinationCrs();
}

void InputCoordinateTransformer::setDestinationCrs( const QgsCoordinateReferenceSystem &destinationCrs )
{
  if ( destinationCrs == mCoordinateTransform.destinationCrs() )
    return;

  mCoordinateTransform.setDestinationCrs( destinationCrs );
  emit destinationCrsChanged();
  updatePosition();
}

QgsCoordinateReferenceSystem InputCoordinateTransformer::sourceCrs() const
{
  return mCoordinateTransform.sourceCrs();
}

void InputCoordinateTransformer::setSourceCrs( const QgsCoordinateReferenceSystem &sourceCrs )
{
  if ( sourceCrs == mCoordinateTransform.sourceCrs() )
    return;

  mCoordinateTransform.setSourceCrs( sourceCrs );

  emit sourceCrsChanged();
  updatePosition();
}

void InputCoordinateTransformer::setTransformContext( const QgsCoordinateTransformContext &context )
{
  mCoordinateTransform.setContext( context );
  emit transformContextChanged();
}

QgsCoordinateTransformContext InputCoordinateTransformer::transformContext() const
{
  return mCoordinateTransform.context();
}

void InputCoordinateTransformer::updatePosition()
{
  double x = mSourcePosition.x();
  double y = mSourcePosition.y();
  double z = mSourcePosition.z();

  // If Z is NaN, coordinate transformation (proj4) will
  // also set X and Y to NaN. But we also want to get projected
  // coords if we do not have any Z coordinate.
  if ( std::isnan( z ) )
  {
    z = 0;
  }

  try
  {
    mCoordinateTransform.transformInPlace( x, y, z );
  }
  catch ( const QgsCsException &exp )
  {
    QgsDebugError( exp.what() );
  }

  mProjectedPosition = QgsPoint( x, y );
  mProjectedPosition.addZValue( mSourcePosition.z() );

  emit projectedPositionChanged();
}
