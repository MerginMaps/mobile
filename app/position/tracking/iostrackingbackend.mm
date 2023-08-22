/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include "iostrackingbackend.h"
#include "iostrackingbackendimpl.h"

#include "coreutils.h"

void IOSTrackingBackend::startPositionProvider( IOSTrackingBackend *me )
{
  if ( mBackendImpl == nullptr )
  {

    @try
    {
      IOSTrackingBackendImpl *impl = [[IOSTrackingBackendImpl alloc] initWithObserver: this];
      mBackendImpl = impl;
    }
    @catch ( NSException *e )
    {
      NSString *message = [NSString stringWithFormat:@"%@ %@", e.name, e.reason];

      CoreUtils::log( TAG, QStringLiteral( "Fatal: unable to initialize location obj-c backend" ) + QString::fromNSString( message ) );
    }
  }

  @try
  {
    [mBackendImpl setup: mDistanceFilter];
  }
  @catch ( NSException *e )
  {
    NSString *message = [ NSString stringWithFormat:@"%@ %@", e.name, e.reason ];

    CoreUtils::log( TAG, QStringLiteral( "Fatal: unable to call setup" ) + QString::fromNSString( message ) );
  }
}

void IOSTrackingBackend::releaseObjc()
{
  if ( mBackendImpl )
  {
    [mBackendImpl release];
    mBackendImpl = nil;
  }
}
