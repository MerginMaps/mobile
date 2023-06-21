/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

package uk.co.lutraconsulting;

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
    }

    @Override
    public IBinder onBind( Intent intent ) {
        // We don't provide binding, so return null
        return null;
    }

    @Override
    public void onDestroy() {
        super.onDestroy();
    }

    public void sendStatusUpdateMessage( String message ) {
        Intent sendToBroadcastIntent = new Intent();

        sendToBroadcastIntent.setAction( PositionTrackingBroadcastMiddleware.TRACKING_STATUS_MESSAGE_ACTION );
        sendToBroadcastIntent.putExtra( PositionTrackingBroadcastMiddleware.TRACKING_STATUS_MESSAGE_TAG, message );

        sendBroadcast( sendToBroadcastIntent );
    }

    @Override
    public int onStartCommand( Intent intent, int flags, int startId ) {

        if ( Build.VERSION.SDK_INT < Build.VERSION_CODES.O ) {
            sendStatusUpdateMessage( "Position tracking: Error, tracking is not supported on your Android version ( Android O (8.0) required )" );
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

        sendStatusUpdateMessage( "Position tracking: Started the foreground service!" );

        locationManager = ( LocationManager ) getApplication().getSystemService( LOCATION_SERVICE );

        boolean isGPSAvailable = locationManager.isProviderEnabled( LocationManager.GPS_PROVIDER );
        if ( !isGPSAvailable ) {
            sendStatusUpdateMessage( "Position tracking: Error, GPS is not available!" );
            stopSelf();
            stopForeground( true );
        }
        else {
            boolean fineLocationAccessGranted = checkSelfPermission( Manifest.permission.ACCESS_FINE_LOCATION ) == PackageManager.PERMISSION_GRANTED;
            boolean coarseLocationAccessGranted = checkSelfPermission(Manifest.permission.ACCESS_COARSE_LOCATION) == PackageManager.PERMISSION_GRANTED;

            if ( !fineLocationAccessGranted || !coarseLocationAccessGranted ) {
                sendStatusUpdateMessage( "Position tracking: Error, missing location permissions!" );
                stopSelf();
                stopForeground(true);
            }
            
            long timeInterval = MIN_TIME_BW_UPDATES;
            long distanceInterval = MIN_DISTANCE_CHANGE_FOR_UPDATES;

            if ( intent.hasExtra( "uk.co.lutraconsulting.tracking.timeInterval" ) ) {
                timeInterval = (long) intent.getDoubleExtra( "uk.co.lutraconsulting.tracking.timeInterval", 1000 );
            }

            if ( intent.hasExtra( "uk.co.lutraconsulting.tracking.distanceInterval" ) ) {
                distanceInterval = (long) intent.getDoubleExtra( "uk.co.lutraconsulting.tracking.distanceInterval", 1 );
            }

            locationManager.requestLocationUpdates(
                LocationManager.GPS_PROVIDER,
                timeInterval,
                distanceInterval,
                this
            );

            sendStatusUpdateMessage( "Position tracking: Started to listen to position updates!" );
        }

        return START_STICKY;
    }

    @Override
    public void onLocationChanged( Location location ) {
        Intent sendToBroadcastIntent = new Intent();

        sendToBroadcastIntent.setAction( PositionTrackingBroadcastMiddleware.TRACKING_POSITION_UPDATE_ACTION );
        sendToBroadcastIntent.putExtra( PositionTrackingBroadcastMiddleware.TRACKING_POSITION_UPDATE_LON_TAG, location.getLongitude() );
        sendToBroadcastIntent.putExtra( PositionTrackingBroadcastMiddleware.TRACKING_POSITION_UPDATE_LAT_TAG, location.getLatitude() );
        sendToBroadcastIntent.putExtra( PositionTrackingBroadcastMiddleware.TRACKING_POSITION_UPDATE_ALT_TAG, location.getAltitude() );

        sendBroadcast( sendToBroadcastIntent );
    }
}
