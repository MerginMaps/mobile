package uk.co.lutraconsulting;

import android.util.Log;

import android.content.Intent;
import android.content.Context;
import android.content.IntentFilter;
import android.content.BroadcastReceiver;


public class PositionTrackingBroadcastMiddleware {
    
    private static native void notifyListenersPositionUpdated( double longitude, double latitude, double altitude );
    private static native void notifyListenersStatusUpdate( String status );

    private static final String TAG = "PositionTrackingBroadcastMiddleware";

    public static final String TRACKING_POSITION_UPDATE_ACTION = "uk.co.lutraconsulting.tracking.position";
    public static final String TRACKING_POSITION_UPDATE_LON_TAG = "uk.co.lutraconsulting.position.update.lon";
    public static final String TRACKING_POSITION_UPDATE_LAT_TAG = "uk.co.lutraconsulting.position.update.lat";
    public static final String TRACKING_POSITION_UPDATE_ALT_TAG = "uk.co.lutraconsulting.position.update.alt";

    public static final String TRACKING_STATUS_MESSAGE_ACTION = "uk.co.lutraconsulting.tracking.status";
    public static final String TRACKING_STATUS_MESSAGE_TAG = "uk.co.lutraconsulting.tracking.status.message";

    public PositionTrackingBroadcastMiddleware() {
        Log.i( TAG, "Broadcast middleware instantiated!");
    }

    public void registerServiceBroadcastReceiver( Context context ) {

        // Register custom type of intent action
        IntentFilter intentFilter = new IntentFilter();
        intentFilter.addAction( TRACKING_POSITION_UPDATE_ACTION );
        intentFilter.addAction( TRACKING_STATUS_MESSAGE_ACTION );

        // Set this as receiver to intents of TRACKING_POSITION_UPDATE_ACTION action type
        context.registerReceiver( serviceMessageReceiver, intentFilter );

        Log.i( TAG, "Registered tracking broadcast middleware" );
    }

    private BroadcastReceiver serviceMessageReceiver = new BroadcastReceiver() {
        
        @Override
        public void onReceive( Context context, Intent intent ) {

            if ( intent.getAction().equals( TRACKING_POSITION_UPDATE_ACTION ) ) {
                double lon = intent.getDoubleExtra( TRACKING_POSITION_UPDATE_LON_TAG, 0 );
                double lat = intent.getDoubleExtra( TRACKING_POSITION_UPDATE_LAT_TAG, 0 );
                double alt = intent.getDoubleExtra( TRACKING_POSITION_UPDATE_ALT_TAG, 0 );

                notifyListenersPositionUpdated( lon, lat, alt );
            }

            if ( intent.getAction().equals( TRACKING_STATUS_MESSAGE_ACTION ) ) {
                String message = intent.getStringExtra( TRACKING_STATUS_MESSAGE_TAG );

                notifyListenersStatusUpdate( message );
            }
        }
    };
}
