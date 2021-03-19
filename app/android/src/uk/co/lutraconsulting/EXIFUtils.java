package uk.co.lutraconsulting;

import android.os.Bundle;
import androidx.exifinterface.media.ExifInterface;
import java.io.File;
import java.io.IOException;
import java.io.InputStream;
import java.io.FileInputStream;
import java.io.OutputStream;
import java.io.FileOutputStream;
import java.util.Date;
import java.util.HashMap;
import java.util.ArrayList;
import java.util.List;
import java.util.stream.Collectors;
import android.util.Log;

public class EXIFUtils
{
    private static final String TAG = "EXIF Utils";
    // GPS EXIF TAGS
    private static final String GPS_DIRECTION_TAG = "GPSImgDirection";
    private static final String GPS_DIRECTION_REF_TAG = "GPSImgDirectionRef";
    private static final String GPS_LON_TAG = "GPSLongitude";
    private static final String GPS_LAT_TAG = "GPSLatitude";
    private static final String GPS_DATE_TAG = "GPSDateStamp";

    /**
     * Writes image direction to EXIF metadata.
     * @param src Absolute path of a file
     * @param direction [0-359] degrees (0=North, 90=East, ...)
     * @param direction_ref Either "M" as Magnetic North or "T" as True North
     */
    public static void writeExifGpsDirection(String src, int direction, String direction_ref) {
        if (direction >= 0 && direction <= 359) {
            HashMap<String, String> attributes = new HashMap<String, String>();
            attributes.put(GPS_DIRECTION_TAG, direction + "/1"); // has to be in rational format
            attributes.put(GPS_DIRECTION_REF_TAG, direction_ref);
            writeExifAttributes(src, attributes);
        } else {
            Log.d(TAG, "Skipped writing direction (" + direction + ") - it is out of the range");
        }
    }

    /**
     * Writes given attributes to EXIF metadata.
     * @param src Absolute path of a file
     * @param attributes Map of exif tag -> value to be written into EXIF
     */
    public static void writeExifAttributes(String src, HashMap<String, String> attributes) {
        Log.d(TAG, "WriteExif: " + src + " to file: ");
        InputStream in = null;
        try {
            ExifInterface exifInterface = new ExifInterface(src);
            for (String key: attributes.keySet()) {
                exifInterface.setAttribute(key, attributes.get(key));
                Log.d(TAG, "Set attribute: " + key + " -> " + attributes.get(key));
            }
            exifInterface.saveAttributes();
        } catch (IOException e) {
            // Handle any errors
            Log.d(TAG, "WriteExif ERROR: " + e.getMessage());
        } finally {
            if (in != null) {
                try {
                    in.close();
                } catch (IOException ignored) {
                }
            }
        }
    }

    public static HashMap<String, String> getExifGpsAttributes(String filepath) {
        List<String> exifTags = new ArrayList<String>();
        exifTags.add(GPS_DIRECTION_TAG);
        exifTags.add(GPS_LAT_TAG);
        exifTags.add(GPS_LON_TAG);
        exifTags.add(GPS_DATE_TAG);
        return getEXIFdata(filepath, exifTags);
    }

    /**
     * Reads and returns EXIF values for given file and for given EXIF Tag.
     * @param filepath Absolute path of a file
     * @param tag String EXIF tag to be read
     * @return String EXIF value for given parameters
     */
    public static String getEXIFAttribute(String filepath, String tag) {
        Log.d(TAG, "getEXIFAttribute: " + filepath + " - " + tag);
        List<String> exifTags = new ArrayList<String>();
        exifTags.add(tag);
        return getEXIFdata(filepath, exifTags).get(tag);
    }

    /**
     * Reads and returns EXIF values for given file and for given EXIF Tags.
     * @param filepath Absolute path of a file
     * @param exifTags List of EXIF tags to read
     * @return Map of EXIF tag -> values
     */
    public static HashMap<String, String> getEXIFdata(String filepath, List<String> exifTags) {
        Log.d(TAG, "getEXIFdata: " + filepath + " to file: ");
        InputStream in = null;
        HashMap<String, String> result = new HashMap<String, String>();

        if (exifTags.isEmpty()) return result;

        try {
            in = new FileInputStream(filepath);
            ExifInterface exifInterface = new ExifInterface(in);
            for (String tag : exifTags) {
                String value = exifInterface.getAttribute(tag);
                result.put(tag, value);
                Log.d(TAG, "getEXIFdata: " + tag + " -> " + value);
            }
        } catch (IOException e) {
            // Handle any errors
        } finally {
            if (in != null) {
                try {
                    in.close();
                } catch (IOException ignored) {
                }
            }
        }
        return result;
    }
}
