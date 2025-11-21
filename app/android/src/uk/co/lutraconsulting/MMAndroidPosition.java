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
import android.content.Context;
import android.content.pm.PackageManager;
import android.location.GnssStatus;
import android.location.Location;
import android.location.LocationListener;
import android.location.LocationManager;
import android.os.Looper;
import android.os.Handler;
import android.util.Log;

import androidx.annotation.NonNull;
import androidx.core.app.ActivityCompat;

import com.google.android.gms.common.ConnectionResult;
import com.google.android.gms.common.GoogleApiAvailability;
import com.google.android.gms.location.FusedLocationProviderClient;
import com.google.android.gms.location.LocationCallback;
import com.google.android.gms.location.LocationRequest;
import com.google.android.gms.location.LocationResult;
import com.google.android.gms.location.LocationServices;
import com.google.android.gms.location.Priority;



public class MMAndroidPosition {

    static public abstract class Callback {
        public void onPositionChanged(@NonNull Location location, GnssStatus gnssStatus) {
        }
    }

    private static native void jniOnPositionUpdated(int instanceId, Location location, GnssStatus gnssStatus);

    // find out whether fused provider could be actually used
    static public boolean isFusedLocationProviderAvailable(Context context) {
        GoogleApiAvailability googleApiAvailability = GoogleApiAvailability.getInstance();
        return googleApiAvailability.isGooglePlayServicesAvailable(context) == ConnectionResult.SUCCESS;
    }

    // get more details why FLP is not available (e.g. play services missing, disabled, updating...)
    static public String fusedLocationProviderErrorString(Context context) {
        GoogleApiAvailability googleApiAvailability = GoogleApiAvailability.getInstance();
        return googleApiAvailability.getErrorString(googleApiAvailability.isGooglePlayServicesAvailable(context));
    }

    // called from C++ code
    static public MMAndroidPosition createWithJniCallback(Context context, boolean useFused, int instanceId) {
        Log.i("CPP", "[java] createWithJniCallback");

        MMAndroidPosition.Callback callback = new MMAndroidPosition.Callback() {
            @Override
            public void onPositionChanged(@NonNull Location location, GnssStatus gnssStatus) {
                jniOnPositionUpdated(instanceId, location, gnssStatus);
            }
        };

        return new MMAndroidPosition(context, callback, useFused);
    }

    private final Context mContext;
    private final LocationManager mLocationManager;
    private final boolean mUseFused;
    private FusedLocationProviderClient mFusedLocationClient = null;
    private final LocationCallback mLocationCallback;
    private final LocationListener mLocationManagerCallback;
    private final GnssStatus.Callback mGnssStatusCallback;
    private final MMAndroidPosition.Callback mClientCallback;
    private boolean mFusedAvailable = false;
    private boolean mGpsProviderAvailable = false;
    private boolean mIsStarted = false;
    private String mErrorMessage;
    private GnssStatus mLastGnssStatus;

    public MMAndroidPosition(Context context, MMAndroidPosition.Callback clientCallback, boolean useFused) {
        mContext = context;
        mClientCallback = clientCallback;
        mUseFused = useFused;

        Log.i("CPP", "[java] constructor!");

        mLocationManager = (LocationManager) context.getSystemService(Context.LOCATION_SERVICE);

        if (mUseFused) {
            mFusedAvailable = isFusedLocationProviderAvailable(context);
            Log.i("CPP", "[java] fused available: " + mFusedAvailable);
            if (mFusedAvailable) {
                mFusedLocationClient = LocationServices.getFusedLocationProviderClient(context);
            }
        } else {
            mGpsProviderAvailable = mLocationManager.isProviderEnabled(LocationManager.GPS_PROVIDER);
            Log.i("CPP", "[java] gps provider available: " + mGpsProviderAvailable);
        }

        mLocationCallback = new LocationCallback() {
            @Override
            public void onLocationResult(@NonNull LocationResult locationResult) {
                for (Location location : locationResult.getLocations()) {
                    Log.i("CPP", "[java] FLP " + location.getLatitude() + " " + location.getLongitude());

                    // call the native function!
                    mClientCallback.onPositionChanged(location, mLastGnssStatus);
                }
            }
        };

        mGnssStatusCallback = new GnssStatus.Callback() {
            @Override
            public void onSatelliteStatusChanged(@NonNull GnssStatus status) {

                // store the satellite info
                mLastGnssStatus = status;
            }
        };

        mLocationManagerCallback = new LocationListener() {
            @Override
            public void onLocationChanged(@NonNull Location location) {
                Log.i("CPP", "[java] GPS " + location.getLatitude() + " " + location.getLongitude());

                mClientCallback.onPositionChanged(location, mLastGnssStatus);
            }
        };

        Log.i("CPP", "[java] constructor end");

    }

    public String errorMessage() {
        return mErrorMessage;
    }

    public boolean start() {
        Log.i("CPP", "[java] start()");

        if (mIsStarted)
            return false;

        if (mUseFused && !mFusedAvailable) {
            mErrorMessage = "FUSED_NOT_AVAILABLE";
            Log.e("CPP", "[java] FUSED_NOT_AVAILABLE");
            return false;
        }

        if (!mUseFused && !mGpsProviderAvailable) {
            mErrorMessage = "GPS_NOT_AVAILABLE";
            Log.e("CPP", "[java] GPS_NOT_AVAILABLE");
            return false;
        }

        if (ActivityCompat.checkSelfPermission(mContext, Manifest.permission.ACCESS_FINE_LOCATION) != PackageManager.PERMISSION_GRANTED &&
                ActivityCompat.checkSelfPermission(mContext, Manifest.permission.ACCESS_COARSE_LOCATION) != PackageManager.PERMISSION_GRANTED) {
            mErrorMessage = "MISSING_PERMISSIONS";
            Log.e("CPP", "[java] MISSING_PERMISSIONS");
            return false;
        }

        if (mUseFused) {
            LocationRequest locationRequest = new LocationRequest.Builder(Priority.PRIORITY_HIGH_ACCURACY, 1000).build();

            mFusedLocationClient.requestLocationUpdates(locationRequest, mLocationCallback, Looper.getMainLooper());
        }
        else {
            mLocationManager.requestLocationUpdates(LocationManager.GPS_PROVIDER, 1000L, 0.F, mLocationManagerCallback, Looper.getMainLooper());
        }

        mLocationManager.registerGnssStatusCallback(mGnssStatusCallback, new Handler(Looper.getMainLooper()));

        Log.i("CPP", "[java] started!");

        mIsStarted = true;
        return true;
    }

    public boolean stop() {
        Log.i("CPP", "[java] stop()");

        if (!mIsStarted)
            return false;

        if (mUseFused) {
            mFusedLocationClient.removeLocationUpdates(mLocationCallback);
        } else {
            mLocationManager.removeUpdates(mLocationManagerCallback);
        }

        mLocationManager.unregisterGnssStatusCallback(mGnssStatusCallback);

        Log.i("CPP", "[java] stopped!");

        mIsStarted = false;
        return true;
    }
}
