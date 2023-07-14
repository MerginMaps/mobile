/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

package uk.co.lutraconsulting;

import android.util.Log;

import android.content.Intent;
import android.content.Context;
import android.content.IntentFilter;
import android.content.BroadcastReceiver;


public class PositionTrackingBroadcastMiddleware {
    
    private static native void notifyListenersPositionUpdated();
    private static native void notifyListenersStatusUpdate( String status );

    private static final String TAG = "PositionTrackingBroadcastMiddleware";

    public static final String TRACKING_POSITION_UPDATE_ACTION = "uk.co.lutraconsulting.tracking.position";
    public static final String TRACKING_POSITION_UPDATE_LON_TAG = "uk.co.lutraconsulting.position.update.lon";
    public static final String TRACKING_POSITION_UPDATE_LAT_TAG = "uk.co.lutraconsulting.position.update.lat";
    public static final String TRACKING_POSITION_UPDATE_ALT_TAG = "uk.co.lutraconsulting.position.update.alt";

    public static final String TRACKING_STATUS_MESSAGE_ACTION = "uk.co.lutraconsulting.tracking.status";
    public static final String TRACKING_STATUS_MESSAGE_TAG = "uk.co.lutraconsulting.tracking.status.message";

    public PositionTrackingBroadcastMiddleware() {
        Log.i( TAG, "Broadcast middleware instantiated! " + this.toString() );
    }

    public void registerBroadcastReceiver( Context context ) {

        // Register custom type of intent action
        IntentFilter intentFilter = new IntentFilter();
        intentFilter.addAction( TRACKING_POSITION_UPDATE_ACTION );
        intentFilter.addAction( TRACKING_STATUS_MESSAGE_ACTION );

        // Set this as the receiver of POSITION_UPDATE and STATUS_MESSAGE intent actions
        context.registerReceiver( serviceMessageReceiver, intentFilter );

        Log.i( TAG, "Registered tracking broadcast middleware " + this.toString() );
    }

    public void unregisterBroadcastReceiver( Context context ) {
        context.unregisterReceiver( serviceMessageReceiver );
        
        Log.i( TAG, "Unregistered tracking broadcast " + this.toString() );
    }

    private BroadcastReceiver serviceMessageReceiver = new BroadcastReceiver() {
        
        @Override
        public void onReceive( Context context, Intent intent ) {

            if ( intent.getAction().equals( TRACKING_POSITION_UPDATE_ACTION ) ) {
                notifyListenersPositionUpdated();
            }

            if ( intent.getAction().equals( TRACKING_STATUS_MESSAGE_ACTION ) ) {
                String message = intent.getStringExtra( TRACKING_STATUS_MESSAGE_TAG );
                notifyListenersStatusUpdate( message );
            }
        }
    };
}
