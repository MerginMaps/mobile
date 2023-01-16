/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

package uk.co.lutraconsulting;

import android.app.Activity;
import android.content.Intent;
import android.os.Bundle;
import android.view.View;
import android.net.Uri;
import android.util.Log;

public class ScannerActivity extends Activity {
    
    private static final String TAG = "Mergin Maps Scanner";

    private static final int ZXING_SCAN_CODE = 10;
    private static final String ZXING_APP_ID = "com.google.zxing.client.android.SCAN";

    @Override
    public void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        
        //
        // As a workaround for Qt bug https://bugreports.qt.io/browse/QTBUG-109168 (Android-specific), we use external app
        // to scan and parse QR and other codes.
        //
        // See https://stackoverflow.com/questions/7233453/zxing-how-to-scan-qr-code-and-1d-barcode
        // and https://stackoverflow.com/questions/21764803/zxing-sometimes-picks-up-the-wrong-data-from-barcode/21764891#21764891
        // and https://stackoverflow.com/questions/2607858/android-barcode-scanning-options-zxing 
        //

        Log.d(TAG, "Starting scanner activity");

        try {
            Intent intent = new Intent(ZXING_APP_ID);
            intent.putExtra("SCAN_MODE", "QR_CODE,DATA_MATRIX,CODABAR,CODE_39,CODE_93,CODE_128,EAN_8,EAN_13");

            startActivityForResult(intent, ZXING_SCAN_CODE);
        } 
        catch (Exception e) {
            Log.d(TAG, "ZXING app not installed");

            Intent resultIntent = new Intent();
            resultIntent.putExtra("message", "not_installed");
            setResult(Activity.RESULT_CANCELED, resultIntent);

            finish();
        }
    }
    
    @Override
    protected void onActivityResult(int requestCode, int resultCode, Intent data) {           
        super.onActivityResult(requestCode, resultCode, data);
        
        Intent resultIntent = new Intent();
        
        if (requestCode == ZXING_SCAN_CODE) {
            
            if (resultCode == RESULT_OK) {
                String contents = data.getStringExtra("SCAN_RESULT");
                resultIntent.putExtra("message", contents);
                setResult(Activity.RESULT_OK, resultIntent);
            }

            if (resultCode == RESULT_CANCELED) {
                resultIntent.putExtra("message", "canceled");
                setResult(Activity.RESULT_CANCELED, resultIntent);
            }
        }
        
        finish();
    }
}