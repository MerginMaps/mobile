/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

package uk.co.lutraconsulting;

import org.qtproject.qt.android.bindings.QtActivity;

import java.lang.Exception;

import android.util.Log;
import android.os.Bundle;
import android.os.Build;
import android.os.VibratorManager;
import android.os.Vibrator;
import android.os.VibrationEffect;
import android.os.VibrationAttributes;
import android.view.Display;
import android.view.Surface;
import android.view.View;
import android.view.DisplayCutout;
import android.view.Window;
import android.view.WindowManager;
import android.view.WindowInsets;
import android.view.WindowInsets.Type;
import android.view.WindowInsetsController;
import android.graphics.Insets;
import android.graphics.Color;

import android.app.Activity;
import android.content.Intent;
import android.net.Uri;
import android.content.ActivityNotFoundException;
import java.io.File;
import java.io.InputStream;
import java.io.OutputStream;
import java.io.FileOutputStream;
import java.io.IOException;
import java.io.FileNotFoundException;
import androidx.core.content.FileProvider;
import android.widget.Toast;
import android.database.Cursor;
import android.provider.OpenableColumns;

import androidx.core.view.WindowCompat;
import androidx.core.splashscreen.SplashScreen;

import java.util.Arrays;

public class InputActivity extends QtActivity
{
  private static final String TAG = "Mergin Maps Input Activity";
  private static final int MEDIA_CODE = 101;
  private boolean keepSplashScreenVisible = true;
  private String localTargetPath = null;
  private String imageCode = null;

  @Override
  public void onCreate(Bundle savedInstanceState)
  {
    SplashScreen splashScreen = SplashScreen.installSplashScreen( this );
    super.onCreate(savedInstanceState);
    
    // this is to keep the screen on all the time so the device does not
    // go into sleep and recording is not interrupted
    getWindow().addFlags(WindowManager.LayoutParams.FLAG_KEEP_SCREEN_ON);

    splashScreen.setKeepOnScreenCondition( () -> keepSplashScreenVisible );

    setCustomStatusAndNavBar();
  }

  public String homePath()
  {
    return getFilesDir().getAbsolutePath();
  }

  void setCustomStatusAndNavBar() 
  {
    if (Build.VERSION.SDK_INT < Build.VERSION_CODES.R) {
      Log.d( TAG, "Unsupported Android version for painting behind system bars." );
      return;
    } 
    else {
      WindowCompat.setDecorFitsSystemWindows(getWindow(), false);

      Window window = getWindow();

      // on Android 15+ all apps are edge-to-edge
      if (Build.VERSION.SDK_INT < Build.VERSION_CODES.VANILLA_ICE_CREAM) {
        // draw app edge-to-edge
        window.addFlags(WindowManager.LayoutParams.FLAG_DRAWS_SYSTEM_BAR_BACKGROUNDS);

        // make the status bar background color transparent
        window.setStatusBarColor(Color.TRANSPARENT);

        // make the navigation button background color transparent
        window.setNavigationBarColor(Color.TRANSPARENT);
      }

      // do not show background dim for the navigation buttons
      window.setNavigationBarContrastEnforced(false); 

      // change the status bar text color to black
      WindowInsetsController insetsController = window.getDecorView().getWindowInsetsController();
    
      if (insetsController != null) {
          insetsController.setSystemBarsAppearance(WindowInsetsController.APPEARANCE_LIGHT_STATUS_BARS, WindowInsetsController.APPEARANCE_LIGHT_STATUS_BARS);
      }
    }
  }

  public String getSafeArea() {

    if (Build.VERSION.SDK_INT < Build.VERSION_CODES.R) {
      Log.d( TAG, "Unsupported Android version for painting behind system bars." );
      return ( "0,0,0,0" );
    }
    else {
      WindowInsets windowInsets = getWindow().getDecorView().getRootWindowInsets();

      if ( windowInsets == null ) {
        Log.d( TAG, "Try to ask for insets later" );
        return null;
      }

      Insets safeArea = windowInsets.getInsets( android.view.WindowInsets.Type.statusBars() | 
                                                android.view.WindowInsets.Type.navigationBars() | 
                                                android.view.WindowInsets.Type.displayCutout() );
                                                
      return ( "" + safeArea.top + "," + safeArea.right + "," + safeArea.bottom + "," + safeArea.left );
    }
  }

  public String getManufacturer() {
    return android.os.Build.MANUFACTURER.toUpperCase();
  }

  public String getDeviceModel() {
      return android.os.Build.MODEL.toUpperCase();
  }

  public void hideSplashScreen()
  {
    keepSplashScreenVisible = false;
  }

  public boolean openFile( String filePath ) {
    File file = new File( filePath );

    if ( !file.exists() ) 
    {
        return false;
    }

    Intent showFileIntent = new Intent( Intent.ACTION_VIEW );

    try 
    {
      Uri fileUri = FileProvider.getUriForFile( this, "uk.co.lutraconsulting.fileprovider", file );

      showFileIntent.setData( fileUri );

      // FLAG_GRANT_READ_URI_PERMISSION grants temporary read permission to the content URI.
      // FLAG_ACTIVITY_NEW_TASK is used when starting an Activity from a non-Activity context.
      showFileIntent.setFlags( Intent.FLAG_ACTIVITY_NEW_TASK | Intent.FLAG_GRANT_READ_URI_PERMISSION );
    } 
    catch ( IllegalArgumentException e )
    {
      return false;
    }

    if ( showFileIntent.resolveActivity( getPackageManager() ) != null ) 
    {
      startActivity( showFileIntent );
    } 
    else 
    {
      return false;
    }
    
    return true;
  }

  public String importImage(Uri imageUri, String targetPath) {
    String fileName = getFileName( imageUri );
    File newCopyFile = new File( targetPath + "/" + fileName );
    try {
      newCopyFile.createNewFile();
      InputStream fileStream = getContentResolver().openInputStream( imageUri );
      copyFile( fileStream, newCopyFile );
      return Uri.fromFile( newCopyFile ).toString();
    } catch (IOException e) {
      Log.e( TAG, "IOException while importing image from gallery!" );
      return "";
    }
  }

  public void copyFile(InputStream src, File dst) throws IOException {
    OutputStream out = null;

    try {
      out = new FileOutputStream(dst);
      // Transfer bytes from src to out
      byte[] buf = new byte[1024];
      int len;
      while ((len = src.read(buf)) > 0) {
        out.write(buf, 0, len);
      }
    } catch (IOException e) {
      throw new IOException("Cannot copy a photo to working directory.");
    } finally {
      if (src != null)
        src.close();
      if (out != null)
        out.close();
    }
  }

  public String getFileName(Uri uri) {
    String result = null;
    // try to get the file name from DISPLAY_NAME column in URI data
    if (uri.getScheme().equals("content")) {
      Cursor cursor = getContentResolver().query(uri, new String[]{OpenableColumns.DISPLAY_NAME}, null, null, null);
      try {
        if (cursor != null && cursor.moveToFirst()) {
          result = cursor.getString(0);
        }
      } finally {
        cursor.close();
      }
    }
    // if the previous approach fails just grab the name from URI, the last segment is the file name without suffix
    if (result == null) {
      result = uri.getPath();
      int cut = result.lastIndexOf('/');
      if (cut != -1) {
        result = result.substring(cut + 1);
      }
    }
    return result;
  }

  public void quitGracefully()
  {
    String man = android.os.Build.MANUFACTURER.toUpperCase();

    Log.d( TAG, String.format("quitGracefully() from Java, MANUFACTURER: '%s'", man ) );
    
    //
    // QT app exit on back button causes crashes on some manufacturers (mainly Huawei, but also Samsung Galaxy recently).
    //
    // Let's play safe and only use this fix for HUAWEI phones for now.
    // If the fix proves itself in next release, we can add it for all other manufacturers.
    //
    // Qt bug: QTBUG-82617
    // See: https://developernote.com/2022/03/crash-at-std-thread-and-std-mutex-destructors-on-android/#comment-694101
    // See: https://stackoverflow.com/questions/61321845/qt-app-crashes-at-the-destructor-of-stdthread-on-android-10-devices
    //

    boolean shouldQuit = man.contains( "HUAWEI" );

    if ( shouldQuit )
    {
      try
      {
        finishAffinity();
        System.exit(0);
      }
      catch ( Exception exp )
      {
        exp.printStackTrace();
        Log.d( TAG, String.format( "quitGracefully() failed to execute: '%s'", exp.toString() ) );
      }
    }
  }

  public void vibrate()
  {
    Vibrator vib;
    if (Build.VERSION.SDK_INT < Build.VERSION_CODES.S)
    {
      vib = (Vibrator) getSystemService(getApplicationContext().VIBRATOR_SERVICE);
    } else
    {
      VibratorManager vibManager = (VibratorManager) getSystemService(getApplicationContext().VIBRATOR_MANAGER_SERVICE);
      vib = vibManager.getDefaultVibrator();
    }

    // The reason why we use duplicate calls to vibrate is because some manufacturers (samsung) don't support
    // the usage of predefined VibrationEffect and vice versa. In the end only one vibration gets executed.
    if (Build.VERSION.SDK_INT < Build.VERSION_CODES.TIRAMISU)
    {
      if (Build.VERSION.SDK_INT < Build.VERSION_CODES.Q)
      {
        vib.vibrate(VibrationEffect.createOneShot(100, VibrationEffect.DEFAULT_AMPLITUDE));
      } else
      {
        vib.vibrate(VibrationEffect.createPredefined(VibrationEffect.EFFECT_CLICK));
        vib.vibrate(VibrationEffect.createOneShot(100, VibrationEffect.DEFAULT_AMPLITUDE));
      }
    } else
    {
      vib.vibrate(VibrationEffect.createPredefined(VibrationEffect.EFFECT_CLICK),
        VibrationAttributes.createForUsage(VibrationAttributes.USAGE_CLASS_FEEDBACK));
      vib.vibrate(VibrationEffect.createOneShot(100, VibrationEffect.DEFAULT_AMPLITUDE));
    }
  }

  @Override
  protected void onDestroy()
  {
    super.onDestroy();
  }
}
