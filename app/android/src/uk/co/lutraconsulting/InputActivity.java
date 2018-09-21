package uk.co.lutraconsulting;
import android.os.Bundle;
import android.view.WindowManager;
import org.qtproject.qt5.android.bindings.QtActivity;

public class InputActivity extends QtActivity
{
  @Override
  public void onCreate(Bundle savedInstanceState)
  {
    super.onCreate(savedInstanceState);
    
    // this is to keep the screen on all the time so the device does not
    // go into sleep and recording is not interrupted
    getWindow().addFlags(WindowManager.LayoutParams.FLAG_KEEP_SCREEN_ON);
  }
  @Override
  protected void onDestroy()
  {
    super.onDestroy();
  }
}
