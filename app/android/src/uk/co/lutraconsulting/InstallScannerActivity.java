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
import android.net.Uri;
import android.util.Log;

public class InstallScannerActivity extends Activity {
    
    private static final String TAG = "Mergin Maps Install Scanner";
    private static final String ZXING_STORE_URL = "market://details?id=com.google.zxing.client.android";

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

        Log.d(TAG, "Opening ZXing store url");

        Uri marketUri = Uri.parse(ZXING_STORE_URL);
        Intent marketIntent = new Intent(Intent.ACTION_VIEW, marketUri);

        startActivity(marketIntent);

        finish();
    }
}