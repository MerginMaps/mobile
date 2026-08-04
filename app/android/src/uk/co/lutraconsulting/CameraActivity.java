/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

package uk.co.lutraconsulting;

import java.text.SimpleDateFormat;
import java.io.File;
import java.io.IOException;
import java.io.InputStream;
import java.io.FileInputStream;
import java.io.OutputStream;
import java.io.FileOutputStream;
import java.util.Date;
import java.util.HashMap;
import java.util.List;
import java.util.stream.Collectors;

import android.os.Bundle;
import android.os.Environment;
import android.net.Uri;
import android.app.Activity;
import android.content.Context;
import android.content.Intent;
import android.content.pm.PackageManager;
import android.content.pm.ResolveInfo;
import android.util.Log;
import android.provider.MediaStore;
import android.graphics.Bitmap;
import androidx.core.content.FileProvider;
import android.hardware.SensorManager;
import android.hardware.camera2.CameraManager;

import uk.co.lutraconsulting.EXIFUtils;
import uk.co.lutraconsulting.OrientationSensor;

public class CameraActivity extends Activity {
    private static final String TAG = "Camera Activity";
    private static final int CAMERA_CODE = 102;
    private static final String KEY_TARGET_PATH = "targetPath";
    private static final String KEY_CAMERA_FILE_PATH = "cameraFilePath";

    private String targetPath;
    private File cameraFile;

    private SensorManager mSensorManager;
    private OrientationSensor orientationSensor;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        Log.d(TAG, "onCreate()");
        super.onCreate(savedInstanceState);

        // Init sensors
        mSensorManager = (SensorManager) getSystemService(
                Context.SENSOR_SERVICE);
        orientationSensor = new OrientationSensor(mSensorManager, null);
        orientationSensor.Register(this, SensorManager.SENSOR_DELAY_NORMAL);

        if (savedInstanceState != null && savedInstanceState.containsKey(KEY_CAMERA_FILE_PATH)) {
            // Process was killed and recreated while the camera app held the foreground.
            // The capture is already in flight -- resume instead of relaunching it.
            targetPath = savedInstanceState.getString(KEY_TARGET_PATH);
            cameraFile = new File(savedInstanceState.getString(KEY_CAMERA_FILE_PATH));
            Log.d(TAG, "Resumed after process recreation, cameraFile: " + cameraFile.getAbsolutePath());
            return;
        }

        targetPath = getIntent().getExtras().getString("targetPath");
        Log.d(TAG, "targetPath: " + targetPath);

        Intent takePictureIntent = new Intent(MediaStore.ACTION_IMAGE_CAPTURE);
        if (takePictureIntent.resolveActivity(getPackageManager()) != null) {

            File photoFile = null;
            try {
                photoFile = createImageFile(targetPath);
            } catch (IOException ex) {
                // Handled in else branch since photoFile == null;
            }
            // Continue only if the File was successfully created
            if (photoFile != null) {
                Uri photoURI = FileProvider.getUriForFile(this,
                        "uk.co.lutraconsulting.fileprovider",
                        photoFile);

                takePictureIntent.putExtra(MediaStore.EXTRA_OUTPUT, photoURI);
                takePictureIntent.setFlags(Intent.FLAG_GRANT_WRITE_URI_PERMISSION
                        | Intent.FLAG_GRANT_READ_URI_PERMISSION);

                // Explicitly grant URI permission to every app that can resolve this intent.
                // Required because the URI is passed via EXTRA_OUTPUT rather than setData(),
                // and some OEM camera apps (confirmed: Motorola) don't reliably honor the
                // FLAG_GRANT_* flags in that case.
                List<ResolveInfo> resolvedActivities = getPackageManager()
                        .queryIntentActivities(takePictureIntent, PackageManager.MATCH_DEFAULT_ONLY);
                for (ResolveInfo resolveInfo : resolvedActivities) {
                    grantUriPermission(resolveInfo.activityInfo.packageName, photoURI,
                            Intent.FLAG_GRANT_WRITE_URI_PERMISSION | Intent.FLAG_GRANT_READ_URI_PERMISSION);
                }

                takePictureIntent.putExtra("__RESULT__", "takePictureIntent__RESULT__");
                startForegroundService(new Intent(this, CameraForegroundService.class));
                startActivityForResult(takePictureIntent, CAMERA_CODE);
            } else {
                Intent activityIntent = getIntent();
                activityIntent.putExtra("__RESULT__", "Cannot access target path.");
                setResult(Activity.RESULT_CANCELED, activityIntent);
                finish();
            }
        }

        return;
    }

    @Override
    protected void onSaveInstanceState(Bundle outState) {
        super.onSaveInstanceState(outState);
        outState.putString(KEY_TARGET_PATH, targetPath);
        if (cameraFile != null) {
            outState.putString(KEY_CAMERA_FILE_PATH, cameraFile.getAbsolutePath());
        }
    }

    private File createImageFile(String targetPath) throws IOException {
        // Create an image file name
        String currentPhotoPath;
        String timeStamp = new SimpleDateFormat("yyyyMMdd_HHmmss").format(new Date());
        String imageFileName = "JPEG_" + timeStamp + "_";

        cameraFile = File.createTempFile(
                imageFileName, /* prefix */
                ".jpg", /* suffix */
                getCacheDir() /* directory */
        );

        // Save a file: path for use with ACTION_VIEW intents
        currentPhotoPath = cameraFile.getAbsolutePath();
        Log.d(TAG, "currentPhotoPath: " + currentPhotoPath);
        return cameraFile;
    }

    @Override
    protected void onActivityResult(int requestCode, int resultCode, Intent data) {
        Log.d(TAG, "onActivityResult()");
        Log.d(TAG, "request: " + requestCode);
        Log.d(TAG, "resultCode: " + resultCode);
        orientationSensor.Unregister();

        if (requestCode == CAMERA_CODE) {
            // no-op if it isn't running (e.g. process was killed and recreated), stops it either way
            stopService(new Intent(this, CameraForegroundService.class));
        }

        if (requestCode == CAMERA_CODE && resultCode == Activity.RESULT_OK) {
            if (cameraFile == null) {
                Log.e(TAG, "cameraFile is null in onActivityResult - lost capture state.");
                Intent resultData = getIntent();
                resultData.putExtra("__RESULT__", "Lost photo capture state.");
                setResult(Activity.RESULT_CANCELED, resultData);
                finish();
                return;
            }
            Log.d(TAG, "tmp exists: " + cameraFile.exists());
            Log.d(TAG, "tmp path: " + cameraFile.getAbsolutePath());

            try {
                extendGPSExifData(cameraFile.lastModified());
                copyFile(cameraFile, new File(targetPath, cameraFile.getName()));
                if (data == null) {
                    data = getIntent();
                }
                data.putExtra("__RESULT__", cameraFile.getAbsolutePath());
                setResult(Activity.RESULT_OK, data);

            } catch (IOException e) {
                Intent intent = this.getIntent();
                if (data == null) {
                    data = getIntent();
                }
                data.putExtra("__RESULT__", e.getMessage());
                setResult(Activity.RESULT_CANCELED, data);
            }

            // TODO: after copy, verify if is correctly copied and then remove the old one
        }
        finish();
    }

    @Override
    protected void onDestroy() {
        super.onDestroy();
        // no-op if it was already stopped in onActivityResult() or never started
        stopService(new Intent(this, CameraForegroundService.class));
        if (cameraFile != null) {
            try {
                Uri photoURI = FileProvider.getUriForFile(this,
                        "uk.co.lutraconsulting.fileprovider", cameraFile);
                revokeUriPermission(photoURI,
                        Intent.FLAG_GRANT_WRITE_URI_PERMISSION | Intent.FLAG_GRANT_READ_URI_PERMISSION);
            } catch (IllegalArgumentException e) {
                // cameraFile isn't covered by file_paths.xml -- nothing was granted, nothing to revoke
            }
        }
    }

    private void extendGPSExifData(long captureTime) {
        int direction = getValueByTime(orientationSensor.m_azimuth_data, captureTime);
        if (direction < 0) {
            Log.d(TAG, "Skipping writing to EXIF, have no data from sensor");
            return;
        }

        String direction_ref = "M"; // stands for Magnetic North since we use magnetic sensor
        EXIFUtils.writeExifGpsDirection(cameraFile.getAbsolutePath(), direction, direction_ref);
        orientationSensor.m_azimuth_data.clear();
    }

    private int getValueByTime(HashMap<Long, Integer> data, long time) {
        List<Integer> result = data.entrySet().stream()
                .filter(x -> Math.abs(x.getKey() - time) <= orientationSensor.DATA_FREQUENCY)
                .map(x->x.getValue())
                .collect(Collectors.toList());
        if (result.isEmpty()) return -1;
        return result.get(0);
    }

    private void copyFile(File src, File dst) throws IOException {
        Log.d(TAG, "Copied file: " + src.getAbsolutePath() + " to file: " + dst.getAbsolutePath());
        InputStream in = null;
        OutputStream out = null;

        try {
            in = new FileInputStream(src);
            out = new FileOutputStream(dst);
            // Transfer bytes from in to out
            byte[] buf = new byte[1024];
            int len;
            while ((len = in.read(buf)) > 0) {
                out.write(buf, 0, len);
            }
        } catch (IOException e) {
            throw new IOException("Cannot copy a photo to working directory.");
        } finally {
            if (in != null)
                in.close();
            if (out != null)
                out.close();
        }

    }
}
