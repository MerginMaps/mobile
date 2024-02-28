/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

package uk.co.lutraconsulting;
import android.os.Bundle;
import org.qtproject.qt.android.bindings.QtActivity;
import android.util.Log;
import android.os.Build;
import java.lang.Exception;

import android.view.Display;
import android.view.Surface;
import android.view.View;
import android.view.DisplayCutout;
import android.view.Window;
import android.view.WindowManager;
import android.view.WindowInsets;
import android.graphics.Insets;
import android.view.WindowInsets.Type;
import android.graphics.Color;

import androidx.core.view.WindowCompat;
import android.view.WindowInsetsController;


public class InputActivity extends QtActivity
{
  private static final String TAG = "Mergin Maps Input Activity";

  @Override
  public void onCreate(Bundle savedInstanceState)
  {
    super.onCreate(savedInstanceState);
    
    // this is to keep the screen on all the time so the device does not
    // go into sleep and recording is not interrupted
    getWindow().addFlags(WindowManager.LayoutParams.FLAG_KEEP_SCREEN_ON);
    setCustomStatusAndNavBar();
  }

  public String homePath()
  {
    return getFilesDir().getAbsolutePath();
  }

  void setCustomStatusAndNavBar() 
  {
    WindowCompat.setDecorFitsSystemWindows(getWindow(), false);

    Window window = getWindow();

    // draw app edge-to-edge
    window.addFlags(WindowManager.LayoutParams.FLAG_DRAWS_SYSTEM_BAR_BACKGROUNDS);
    
    // make the status bar background color transparent
    window.setStatusBarColor(Color.TRANSPARENT);
    
    // make the navigation button background color transparent
    window.setNavigationBarColor(Color.TRANSPARENT);

    // do not show background dim for the navigation buttons
    window.setNavigationBarContrastEnforced(false); 

    if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.R) {

      // change the status bar text color to black
      WindowInsetsController insetsController = window.getDecorView().getWindowInsetsController();
    
      if (insetsController != null) {
          insetsController.setSystemBarsAppearance(WindowInsetsController.APPEARANCE_LIGHT_STATUS_BARS, WindowInsetsController.APPEARANCE_LIGHT_STATUS_BARS);
      }
    }
  }

  public String getSafeArea() {

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

  @Override
  protected void onDestroy()
  {
    super.onDestroy();
  }
}
