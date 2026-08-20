/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

package uk.co.lutraconsulting;

import android.os.Build;
import android.os.IBinder;
import android.app.Service;
import android.app.PendingIntent;
import android.content.Intent;
import android.content.pm.ServiceInfo;

import android.app.Notification;
import android.app.NotificationChannel;
import android.app.NotificationManager;

/**
 * Runs in the app's default process for as long as CameraActivity is waiting on the external
 * camera app, to raise this process's priority and make it much less likely to be killed while
 * the camera is in the foreground.
 * This is a mitigation, not a guarantee -- under severe enough memory pressure the process can
 * still be killed; CameraActivity does not currently resume state after that, so a kill mid-wait
 * causes the camera capture to restart from scratch on the recreated process.
 */
public class CameraForegroundService extends Service {

    private static final String CHANNEL_ID = "CameraForegroundServiceChannel";
    private static final int SERVICE_ID = 1011;

    @Override
    public IBinder onBind(Intent intent) {
        return null;
    }

    @Override
    public int onStartCommand(Intent intent, int flags, int startId) {
        NotificationChannel serviceChannel = new NotificationChannel(
                CHANNEL_ID,
                "Camera Foreground Service Channel",
                NotificationManager.IMPORTANCE_LOW
        );

        NotificationManager manager = getSystemService(NotificationManager.class);
        manager.createNotificationChannel(serviceChannel);

        Intent notificationIntent = new Intent(this, MMActivity.class);
        PendingIntent pendingIntent = PendingIntent.getActivity(this, 0, notificationIntent, PendingIntent.FLAG_IMMUTABLE);

        Notification notification = new Notification.Builder(this, CHANNEL_ID)
                .setSmallIcon(R.drawable.ic_notification)
                .setContentTitle("Waiting for photo")
                .setColor(getResources().getColor(R.color.grassColor))
                .setContentIntent(pendingIntent)
                .build();

        // We never request the POST_NOTIFICATIONS runtime permission from the user, so this
        // notification will silently not be shown unless the user has manually enabled it for
        // the app in system Settings. startForeground() still succeeds and still elevates the
        // process's priority either way -- the permission only gates the notification's
        // visibility, not the foreground service state itself.
        if (Build.VERSION.SDK_INT >= 35) {
            startForeground(SERVICE_ID, notification, ServiceInfo.FOREGROUND_SERVICE_TYPE_SHORT_SERVICE);
        } else {
            startForeground(SERVICE_ID, notification);
        }

        // if this service alone gets killed there is nothing useful to resume, it only exists to
        // keep the process's priority elevated while CameraActivity waits on startActivityForResult().
        return START_NOT_STICKY;
    }
}
