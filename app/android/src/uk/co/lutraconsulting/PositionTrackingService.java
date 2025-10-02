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
import android.content.pm.ServiceInfo;

import android.app.Notification;
import android.app.NotificationChannel;
import android.app.NotificationManager;

import android.location.Location;
import android.location.LocationManager;
import android.location.LocationListener;

import java.io.File;
import java.io.FileOutputStream;
import java.io.IOException;
import java.util.Locale;

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
    private FileOutputStream positionUpdatesStream;

    public boolean amIRunning = false;


    @Override
    public void onCreate() {
        super.onCreate();

        // Create the file to store position updates
        File file = new File( getFilesDir(), "tracking_updates.txt" );

        sendStatusUpdateMessage( "Tracking file path:" + file.getAbsolutePath() );

        try {
            // Open the FileOutputStream in append mode
            positionUpdatesStream = new FileOutputStream(file, true);

        } catch ( IOException e ) {
            e.printStackTrace();
            sendStatusUpdateMessage("ERROR #GENERAL: Could not open file stream: " + e.getMessage() );
        }
    }

    @Override
    public IBinder onBind( Intent intent ) {
        // We don't provide binding, so return null
        return null;
    }

    @Override
    public void onDestroy() {

        if (locationManager != null) {
            locationManager.removeUpdates(this);
        }

        // Close the FileOutputStream when the service is destroyed
        try {
            if (positionUpdatesStream != null) {
                positionUpdatesStream.close();
            }
        } catch (IOException e) {
            e.printStackTrace();
            sendStatusUpdateMessage("ERROR #SILENT: Could not close file stream: " + e.getMessage() );
        }

        super.onDestroy();
    }

    public void sendStatusUpdateMessage( String message ) {
        Intent sendToBroadcastIntent = new Intent();

        sendToBroadcastIntent.setAction( PositionTrackingBroadcastMiddleware.TRACKING_STATUS_MESSAGE_ACTION );
        sendToBroadcastIntent.putExtra( PositionTrackingBroadcastMiddleware.TRACKING_STATUS_MESSAGE_TAG, message );

        sendBroadcast( sendToBroadcastIntent );
    }

    public void sendAliveStatusResponse( boolean isAlive ) {
        Intent sendToBroadcastIntent = new Intent();

        sendToBroadcastIntent.setAction( PositionTrackingBroadcastMiddleware.TRACKING_ALIVE_STATUS_ACTION );
        sendToBroadcastIntent.putExtra( PositionTrackingBroadcastMiddleware.TRACKING_ALIVE_STATUS_TAG, isAlive );

        sendBroadcast( sendToBroadcastIntent );
    }

    @Override
    public int onStartCommand( Intent intent, int flags, int startId ) {

        if ( intent.hasExtra( PositionTrackingBroadcastMiddleware.TRACKING_ALIVE_STATUS_ACTION ) ) {
            // we are just checking if the service is running, without intention to start it
            sendStatusUpdateMessage("Responding to alive request from service!!");
            sendAliveStatusResponse( amIRunning );
            stopSelf();

            return START_NOT_STICKY; // do not bother recreating it
        }

        if ( Build.VERSION.SDK_INT < Build.VERSION_CODES.O ) {
            sendStatusUpdateMessage( "ERROR #UNSUPPORTED: tracking is not supported on your Android version ( Android O (8.0) required )" );
            stopSelf();

            return START_NOT_STICKY; // do not bother recreating it
        }

        // Build channel for notifications
        NotificationChannel serviceChannel = new NotificationChannel(
            CHANNEL_ID,
            "Foreground Service Channel",
            NotificationManager.IMPORTANCE_LOW
        );
        
        NotificationManager manager = getSystemService( NotificationManager.class );
        manager.createNotificationChannel( serviceChannel );

        // Build notification for position tracking
        Intent notificationIntent = new Intent( this, MMActivity.class );

        PendingIntent pendingIntent = PendingIntent.getActivity( this, 0, notificationIntent, PendingIntent.FLAG_IMMUTABLE );

        Notification.Builder notificationBuilder = new Notification.Builder( this, CHANNEL_ID )
            .setSmallIcon( R.drawable.ic_notification )
            .setContentTitle( "Tracking in progress" )
            .setColor( getResources().getColor( R.color.grassColor ) )
            .setContentIntent( pendingIntent );

        if ( Build.VERSION.SDK_INT >= Build.VERSION_CODES.S ) {  // only Android >= 12
            notificationBuilder.setForegroundServiceBehavior( Notification.FOREGROUND_SERVICE_IMMEDIATE );
        }

        Notification notification = notificationBuilder.build();

        if ( Build.VERSION.SDK_INT >= 34) { // Android 14 ( Upside Down Cake )
            startForeground( SERVICE_ID, notification, ServiceInfo.FOREGROUND_SERVICE_TYPE_LOCATION );
        }
        else {
            startForeground( SERVICE_ID, notification );
        }

        sendStatusUpdateMessage( "Position tracking: Started the foreground service!" );

        locationManager = ( LocationManager ) getApplication().getSystemService( LOCATION_SERVICE );

        if ( Build.VERSION.SDK_INT < Build.VERSION_CODES.O ) {
            sendStatusUpdateMessage( "ERROR #UNSUPPORTED: tracking is not supported on your Android version ( Android O (8.0) required )" );
            stopSelf();

            return START_NOT_STICKY; // do not bother recreating it
        }

        String positionProvider;
        
        // FUSED_PROVIDER is available since API 31 (Android 12)
        if ( Build.VERSION.SDK_INT < Build.VERSION_CODES.S ) {
            positionProvider = LocationManager.GPS_PROVIDER;
        }
        else {
            positionProvider = LocationManager.FUSED_PROVIDER;
        }

        boolean isGPSAvailable = locationManager.isProviderEnabled( positionProvider );
        if ( !isGPSAvailable ) {
            sendStatusUpdateMessage( "ERROR #GPS_UNAVAILABLE: GPS is not available!" );
            stopSelf();
            stopForeground( true );
        }
        else {
            boolean fineLocationAccessGranted = checkSelfPermission( Manifest.permission.ACCESS_FINE_LOCATION ) == PackageManager.PERMISSION_GRANTED;
            boolean coarseLocationAccessGranted = checkSelfPermission(Manifest.permission.ACCESS_COARSE_LOCATION) == PackageManager.PERMISSION_GRANTED;

            if ( !fineLocationAccessGranted || !coarseLocationAccessGranted ) {
                sendStatusUpdateMessage( "ERROR #PERMISSIONS: missing location permissions!" );
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
                positionProvider,
                timeInterval,
                distanceInterval,
                this
            );

            sendStatusUpdateMessage( "Started to listen to position updates!" );
        }

        amIRunning = true;

        return START_STICKY;
    }

    @Override
    public void onLocationChanged( Location location ) {

        long currentTimeSeconds = System.currentTimeMillis() / 1000; // UTC time
        String positionUpdate = String.format(Locale.US, "%f %f %f %d\n", location.getLongitude(), location.getLatitude(), location.getAltitude(), currentTimeSeconds);

        try {
            if ( positionUpdatesStream != null ) {
                positionUpdatesStream.write( positionUpdate.getBytes() );
            }
        } catch ( IOException e ) {
            e.printStackTrace();
            sendStatusUpdateMessage("ERROR #GENERAL: Could not write to file:" + e.getMessage() );
        }

        // notify cpp about position update
        Intent sendToBroadcastIntent = new Intent();

        sendToBroadcastIntent.setAction( PositionTrackingBroadcastMiddleware.TRACKING_POSITION_UPDATE_ACTION );
        sendBroadcast( sendToBroadcastIntent );
    }
}
