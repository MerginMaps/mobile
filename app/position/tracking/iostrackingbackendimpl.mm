
#import "iostrackingbackendimpl.h"

@implementation IOSTrackingBackendImpl

-( id )initWithObserver:( IOSTrackingBackend * )iosObserver
{
  if ( ( self = [ super init ] ) )
  {
    mObserver = iosObserver;
  }
  return self;
}

- ( void )dealloc
{
  NSLog( @"Object deallocated" );

  if ( mManager )
  {
    [mManager stopUpdatingLocation];
    [mManager release];
    mManager = nil;
  }

  if ( mObserver )
  {
    mObserver = nil;
  }

  [super dealloc];
}

-( NSString * ) setup:( double )distanceFilter
{
  NSString *response = @"";

  if ( mManager == nil )
  {
    mManager = [[ CLLocationManager alloc ] init ];

    response = [response stringByAppendingString:@"Info: Built location manager!"];
  }

  if ( [CLLocationManager authorizationStatus] == kCLAuthorizationStatusNotDetermined )
  {
    [mManager requestWhenInUseAuthorization];

    response = [response stringByAppendingString:@" Info: Requested authorization!"];
  }
  else
  {
    response = [response stringByAppendingString:@" Info: Did not request authorization, other state"];
  }

  response = [response stringByAppendingString:@" Requested location"];

  mManager.distanceFilter = distanceFilter;
  mManager.delegate = self;

  [mManager startUpdatingLocation];
  mManager.allowsBackgroundLocationUpdates = true;

  response = [response stringByAppendingString:@" Started updates"];

  return response;
}

- ( void )locationManager:( CLLocationManager * )manager didUpdateLocations:( NSArray * )locations
{
  CLLocation *location = [locations lastObject];

  mObserver->positionUpdate( location.coordinate.longitude, location.coordinate.latitude, location.altitude );
}

- ( void )locationManager:( CLLocationManager * )manager didFailWithError:( NSError * )error
{

  NSString *message = [NSString stringWithFormat:@"%@ %@", @"Error:", error.localizedDescription];

  if ( error.code == kCLErrorDenied )
  {
    // permission is not denied, do not try again
    [mManager stopUpdatingLocation];
    mObserver->stopTrying( message );
  }
  else
  {
    // some other error, but we can continue reading location
    mObserver->logError( message );
  }
}

@end
