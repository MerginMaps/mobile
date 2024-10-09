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
    private static native void notifyListenersAliveResponse( boolean status );

    private static final String TAG = "PositionTrackingBroadcastMiddleware";

    public static final String TRACKING_POSITION_UPDATE_ACTION = "uk.co.lutraconsulting.tracking.position";

    public static final String TRACKING_ALIVE_STATUS_ACTION = "uk.co.lutraconsulting.tracking.alive";
    public static final String TRACKING_ALIVE_STATUS_TAG = "uk.co.lutraconsulting.tracking.alive.status";

    public static final String TRACKING_STATUS_MESSAGE_ACTION = "uk.co.lutraconsulting.tracking.status";
    public static final String TRACKING_STATUS_MESSAGE_TAG = "uk.co.lutraconsulting.tracking.status.message";

    public PositionTrackingBroadcastMiddleware() {

    }

    public void registerBroadcastReceiver( Context context ) {

        // Register custom type of intent action
        IntentFilter intentFilter = new IntentFilter();
        intentFilter.addAction( TRACKING_POSITION_UPDATE_ACTION );
        intentFilter.addAction( TRACKING_STATUS_MESSAGE_ACTION );
        intentFilter.addAction( TRACKING_ALIVE_STATUS_ACTION );

        // Set this as the receiver of POSITION_UPDATE and STATUS_MESSAGE intent actions
        if ( Build.VERSION.SDK_INT >= Build.VERSION_CODES.UPSIDE_DOWN_CAKE ) { // Android >= 14
            context.registerReceiver( serviceMessageReceiver, intentFilter, Context.RECEIVER_EXPORTED );
        }
        else {
            context.registerReceiver( serviceMessageReceiver, intentFilter );
        }
    }

    public void unregisterBroadcastReceiver( Context context ) {
        context.unregisterReceiver( serviceMessageReceiver );
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

            if ( intent.getAction().equals( TRACKING_ALIVE_STATUS_ACTION ) ) {
                boolean isAlive = intent.getBooleanExtra( TRACKING_ALIVE_STATUS_TAG, false );
                notifyListenersAliveResponse( isAlive );
            }
        }
    };
}
