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
import android.view.WindowManager;
import org.qtproject.qt.android.bindings.QtActivity;
import android.util.Log;
import android.os.Build;
import java.lang.Exception;

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
  }

  public String homePath()
  {
    return getFilesDir().getAbsolutePath();
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
