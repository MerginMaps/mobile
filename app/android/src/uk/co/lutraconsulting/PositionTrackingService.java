package uk.co.lutraconsulting;

import android.util.Log;

import android.Manifest;
import android.os.Build;
import android.os.IBinder;
import android.app.Service;
import android.content.Intent;
import android.app.PendingIntent;
import android.content.pm.PackageManager;

import android.app.Notification;
import android.app.NotificationChannel;
import android.app.NotificationManager;

import android.location.Location;
import android.location.LocationManager;
import android.location.LocationListener;

import uk.co.lutraconsulting.PositionTrackingBroadcastMiddleware;

import java.util.Timer;
import java.util.TimerTask;

public class PositionTrackingService extends Service implements LocationListener {

    private static final String TAG = "PositionTrackingService";

    // Channel for notifications
    public static final String CHANNEL_ID = "PositionTrackingServiceChannel";

    public static final int SERVICE_ID = 1010;

    Location loc;
    double latitude;
    double longitude;

    private static long MIN_DISTANCE_CHANGE_FOR_UPDATES = 1;
    private static long MIN_TIME_BW_UPDATES = 1000; //ms

    protected LocationManager locationManager;


    @Override
    public void onCreate() {
        super.onCreate();
        Log.i( TAG, "Created" );
    }

    @Override
    public IBinder onBind( Intent intent ) {
        // We don't provide binding, so return null
        return null;
    }

    @Override
    public void onDestroy() {
        super.onDestroy();
        Log.i( TAG, "Destroyed" );
    }

    @Override
    public int onStartCommand( Intent intent, int flags, int startId ) {
        Log.i( TAG, "Starting onStartCommand" );

        if ( Build.VERSION.SDK_INT < Build.VERSION_CODES.O ) {
            Log.i( TAG, "Error, tracking is not supported on your Android version ( Android O (8.0) required )" );
            stopSelf();

            return START_STICKY;
        }

        // Build channel for notifications
        NotificationChannel serviceChannel = new NotificationChannel(
            CHANNEL_ID,
            "Foreground Service Channel",
            NotificationManager.IMPORTANCE_HIGH
        );
        
        NotificationManager manager = getSystemService( NotificationManager.class );
        manager.createNotificationChannel( serviceChannel );

        // Build notification for position tracking
        Intent notificationIntent = new Intent( this, PositionTrackingService.class );

        PendingIntent pendingIntent = PendingIntent.getActivity( this, 0, notificationIntent, PendingIntent.FLAG_IMMUTABLE );

        Notification notification = new Notification.Builder( this, CHANNEL_ID )
            .setContentTitle( getText( R.string.notification_title ) )
            .setContentText( getText( R.string.notification_message ) )
            .setContentIntent( pendingIntent )
            .setSmallIcon( R.drawable.ic_merginmaps_launcher_background )
            .setTicker( getText( R.string.ticker_text ) )
            .build();

        startForeground( SERVICE_ID, notification );

        Log.i( TAG, "Started the foreground service!" );

        locationManager = ( LocationManager ) getApplication().getSystemService( LOCATION_SERVICE );

        boolean isGPSAvailable = locationManager.isProviderEnabled( LocationManager.GPS_PROVIDER );
        if ( !isGPSAvailable ) {
            Log.d( TAG, "Error, GPS is not available!" );
            stopSelf();
            stopForeground( true );
        }
        else {
            boolean fineLocationAccessGranted = checkSelfPermission( Manifest.permission.ACCESS_FINE_LOCATION ) == PackageManager.PERMISSION_GRANTED;
            boolean coarseLocationAccessGranted = checkSelfPermission(Manifest.permission.ACCESS_COARSE_LOCATION) == PackageManager.PERMISSION_GRANTED;

            if ( !fineLocationAccessGranted || !coarseLocationAccessGranted ) {
                Log.d( TAG, "Error, missing location permissions!" );
                stopSelf();
                stopForeground(true);
            }

            locationManager.requestLocationUpdates(
                LocationManager.GPS_PROVIDER,
                MIN_TIME_BW_UPDATES,
                MIN_DISTANCE_CHANGE_FOR_UPDATES,
                this
            );

            Log.i( TAG, "Started to listen to position updates!" );
        }

        return START_STICKY;
    }

    @Override
    public void onLocationChanged( Location location ) {
        Log.d( TAG, String.format( "Location updated %s", location.toString() ) );

        Intent sendToBroadcastIntent = new Intent();

        sendToBroadcastIntent.setAction( PositionTrackingBroadcastMiddleware.TRACKING_BROADCAST_NOTIFY_ACTION );
        sendToBroadcastIntent.putExtra( "uk.co.lutraconsulting.position.update.lon", location.getLongitude() );
        sendToBroadcastIntent.putExtra( "uk.co.lutraconsulting.position.update.lat", location.getLatitude() );
        sendToBroadcastIntent.putExtra( "uk.co.lutraconsulting.position.update.alt", location.getAltitude() );

        sendBroadcast( sendToBroadcastIntent );
    }
}
