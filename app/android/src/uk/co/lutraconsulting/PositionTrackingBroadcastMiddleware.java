package uk.co.lutraconsulting;

import android.util.Log;

import android.content.Intent;
import android.content.Context;
import android.content.IntentFilter;
import android.content.BroadcastReceiver;


public class PositionTrackingBroadcastMiddleware {
    
    private static native void notifyListenersFromJava( double longitude, double latitude, double altitude );

    private static final String TAG = "PositionTrackingBroadcastMiddleware";
    public static final String TRACKING_BROADCAST_NOTIFY_ACTION = "uk.co.lutraconsulting.tracking.broadcast";

    public PositionTrackingBroadcastMiddleware() {
        Log.i( TAG, "Hello hello!");
    }

    public void registerServiceBroadcastReceiver( Context context ) {

        // Register custom type of intent action
        IntentFilter intentFilter = new IntentFilter();
        intentFilter.addAction( TRACKING_BROADCAST_NOTIFY_ACTION );

        // Set this as receiver to intents of TRACKING_BROADCAST_NOTIFY_ACTION action type
        context.registerReceiver( serviceMessageReceiver, intentFilter );

        Log.i( TAG, "Registered tracking broadcast middleware" );
    }

    private BroadcastReceiver serviceMessageReceiver = new BroadcastReceiver() {
        
        @Override
        public void onReceive( Context context, Intent intent ) {
            Log.i( TAG, "Received message from service" );

            if ( intent.getAction().equals( TRACKING_BROADCAST_NOTIFY_ACTION ) ) {

                // String message = intent.getStringExtra( "uk.co.lutraconsulting.position.update" );

                double lon = intent.getDoubleExtra( "uk.co.lutraconsulting.position.update.lon", 0 );
                double lat = intent.getDoubleExtra( "uk.co.lutraconsulting.position.update.lat", 0 );
                double alt = intent.getDoubleExtra( "uk.co.lutraconsulting.position.update.alt", 0 );

                notifyListenersFromJava( lon, lat, alt );
            }
        }
    };
}
