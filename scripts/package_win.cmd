xcopy build-Input\release\Input.exe %PACKAGE_DIR%\stage\ /Y
windeployqt --release input-package\ Input.exe 
            
robocopy %REPO_PATH%\app\android\assets\qgis-data %STAGE_PATH%\qgis-data /E /NFL
            robocopy %REPO_PATH%\app\android\assets\demo-projects %STAGE_PATH%\demo-projects /E /NFL
            if not exist %STAGE_PATH%\images mkdir %STAGE_PATH%\images
            xcopy %REPO_PATH%\images\AppIcon.ico %STAGE_PATH%\images\ /Y
            more /P %REPO_PATH%\LICENSE > %STAGE_PATH%\license.txt

            xcopy %INPUT_SDK_DIR%\bin\qgis_core.dll %STAGE_PATH%\ /Y
            xcopy %INPUT_SDK_DIR%\bin\qgis_native.dll %STAGE_PATH%\ /Y
            xcopy %INPUT_SDK_DIR%\bin\proj*.dll %STAGE_PATH%\ /Y
            xcopy %INPUT_SDK_DIR%\bin\geos_c.dll %STAGE_PATH%\ /Y 
            xcopy %INPUT_SDK_DIR%\bin\gdal*.dll %STAGE_PATH%\ /Y 
            xcopy %INPUT_SDK_DIR%\bin\expat.dll %STAGE_PATH%\ /Y 
            xcopy %INPUT_SDK_DIR%\bin\zip.dll %STAGE_PATH%\ /Y 
            xcopy %INPUT_SDK_DIR%\bin\spatialindex-64.dll %STAGE_PATH%\ /Y 
            xcopy %INPUT_SDK_DIR%\bin\sqlite3.dll %STAGE_PATH%\ /Y 
            xcopy %INPUT_SDK_DIR%\bin\spatialite.dll %STAGE_PATH%\ /Y 
            xcopy %INPUT_SDK_DIR%\bin\libpq.dll %STAGE_PATH%\ /Y 
            xcopy %INPUT_SDK_DIR%\bin\geos.dll %STAGE_PATH%\ /Y 
            xcopy %INPUT_SDK_DIR%\bin\zlib1.dll %STAGE_PATH%\ /Y 
            xcopy %INPUT_SDK_DIR%\bin\freexl.dll %STAGE_PATH%\ /Y 
            xcopy %INPUT_SDK_DIR%\bin\iconv.dll %STAGE_PATH%\ /Y 
            xcopy %INPUT_SDK_DIR%\bin\ogdi.dll %STAGE_PATH%\ /Y 
            xcopy %INPUT_SDK_DIR%\bin\xerces-c_*.dll %STAGE_PATH%\ /Y 
            xcopy %INPUT_SDK_DIR%\bin\netcdf.dll %STAGE_PATH%\ /Y 
            xcopy %INPUT_SDK_DIR%\bin\jpeg.dll %STAGE_PATH%\ /Y 
            xcopy %INPUT_SDK_DIR%\bin\libmysql.dll %STAGE_PATH%\ /Y 
            xcopy %INPUT_SDK_DIR%\bin\hdf5.dll %STAGE_PATH%\ /Y 
            xcopy %INPUT_SDK_DIR%\bin\libcurl.dll %STAGE_PATH%\ /Y 
            xcopy %INPUT_SDK_DIR%\bin\libpng*.dll %STAGE_PATH%\ /Y 
            xcopy %INPUT_SDK_DIR%\bin\openjp2.dll %STAGE_PATH%\ /Y 
            xcopy %INPUT_SDK_DIR%\bin\liblzma.dll %STAGE_PATH%\ /Y 
            xcopy %INPUT_SDK_DIR%\bin\zstd.dll %STAGE_PATH%\ /Y 
            xcopy %INPUT_SDK_DIR%\bin\iconv-2.dll %STAGE_PATH%\ /Y 
            xcopy %INPUT_SDK_DIR%\bin\libxml2.dll %STAGE_PATH%\ /Y 
            xcopy %INPUT_SDK_DIR%\bin\lwgeom.dll %STAGE_PATH%\ /Y 
            xcopy %INPUT_SDK_DIR%\bin\libcrypto-*.dll %STAGE_PATH%\ /Y 
            xcopy %INPUT_SDK_DIR%\bin\libssl-*.dll %STAGE_PATH%\ /Y 
            xcopy %INPUT_SDK_DIR%\bin\hdf5_hl.dll %STAGE_PATH%\ /Y 
            xcopy %INPUT_SDK_DIR%\bin\libeay32.dll %STAGE_PATH%\ /Y 
            xcopy %INPUT_SDK_DIR%\bin\ssleay32.dll %STAGE_PATH%\ /Y 
            xcopy %INPUT_SDK_DIR%\bin\geodiff.dll %STAGE_PATH%\ /Y 
            xcopy %INPUT_SDK_DIR%\bin\libprotobuf-lite.dll %STAGE_PATH%\ /Y
            xcopy %INPUT_SDK_DIR%\bin\szip.dll %STAGE_PATH%\ /Y

            rem qgis providers
            xcopy %INPUT_SDK_DIR%\plugins\*provider.dll %STAGE_PATH%\ /Y 

            rem system
            xcopy %VS14ROOT%\VC\redist\x64\Microsoft.VC140.CRT\*.dll %STAGE_PATH%\ /Y
            xcopy %INPUT_SDK_DIR%\bin\ms*.dll %STAGE_PATH%\ /Y

            rem Qt5
            xcopy %INPUT_SDK_DIR%\apps\Qt5\bin\Qt5Sql.dll %STAGE_PATH%\ /Y
            xcopy %INPUT_SDK_DIR%\apps\Qt5\bin\Qt5Concurrent.dll %STAGE_PATH%\ /Y
            xcopy %INPUT_SDK_DIR%\apps\Qt5\bin\Qt5Xml.dll %STAGE_PATH%\ /Y
            xcopy %INPUT_SDK_DIR%\apps\Qt5\bin\Qt5PrintSupport.dll %STAGE_PATH%\ /Y
            xcopy %INPUT_SDK_DIR%\apps\Qt5\bin\qca-qt5.dll %STAGE_PATH%\ /Y
            xcopy %INPUT_SDK_DIR%\apps\Qt5\bin\qt5keychain.dll %STAGE_PATH%\ /Y
            xcopy %INPUT_SDK_DIR%\apps\Qt5\bin\Qt5Positioning.dll %STAGE_PATH%\ /Y
            xcopy %INPUT_SDK_DIR%\apps\Qt5\bin\Qt5Location.dll %STAGE_PATH%\ /Y
            xcopy %INPUT_SDK_DIR%\apps\Qt5\bin\Qt5Sensors.dll %STAGE_PATH%\ /Y
            xcopy %INPUT_SDK_DIR%\apps\Qt5\bin\Qt5QuickTemplates2.dll %STAGE_PATH%\ /Y
            xcopy %INPUT_SDK_DIR%\apps\Qt5\bin\Qt5QuickControls2.dll %STAGE_PATH%\ /Y
            xcopy %INPUT_SDK_DIR%\apps\Qt5\bin\Qt5QuickWidgets.dll %STAGE_PATH%\ /Y
            xcopy %INPUT_SDK_DIR%\apps\Qt5\bin\Qt5QuickParticles.dll %STAGE_PATH%\ /Y
            xcopy %INPUT_SDK_DIR%\apps\Qt5\bin\Qt5Quick.dll %STAGE_PATH%\ /Y
            xcopy %INPUT_SDK_DIR%\apps\Qt5\bin\Qt5PositioningQuick.dll %STAGE_PATH%\ /Y
            xcopy %INPUT_SDK_DIR%\apps\Qt5\bin\Qt5MultimediaQuick.dll %STAGE_PATH%\ /Y
            xcopy %INPUT_SDK_DIR%\apps\Qt5\bin\Qt5Multimedia.dll %STAGE_PATH%\ /Y

            robocopy %INPUT_SDK_DIR%\apps\Qt5\qml\Qt %STAGE_PATH%\qml\Qt /E /NFL /XF *d.dll
            robocopy %INPUT_SDK_DIR%\apps\Qt5\qml\QtLocation %STAGE_PATH%\qml\QtLocation /E /NFL /XF *d.dll
            robocopy %INPUT_SDK_DIR%\apps\Qt5\qml\QtMultimedia %STAGE_PATH%\qml\QtMultimedia /E /NFL /XF *d.dll
            robocopy %INPUT_SDK_DIR%\apps\Qt5\qml\QtQml %STAGE_PATH%\qml\QtQml /E /NFL /XF *d.dll
            robocopy %INPUT_SDK_DIR%\apps\Qt5\qml\QtQuick %STAGE_PATH%\qml\QtQuick /E /NFL /XF *d.dll
            robocopy %INPUT_SDK_DIR%\apps\Qt5\qml\QtQuick.2 %STAGE_PATH%\qml\QtQuick.2 /E /NFL /XF *d.dll
            robocopy %INPUT_SDK_DIR%\apps\Qt5\qml\QtPositioning %STAGE_PATH%\qml\QtPositioning /E /NFL /XF *d.dll
            robocopy %INPUT_SDK_DIR%\apps\Qt5\qml\QtGraphicalEffects %STAGE_PATH%\qml\QtGraphicalEffects /E /NFL /XF *d.dll

            robocopy %INPUT_SDK_DIR%\apps\Qt5\plugins\position %STAGE_PATH%\position /E /NFL /XF *d.dll
            robocopy %INPUT_SDK_DIR%\apps\Qt5\plugins\sqldrivers %STAGE_PATH%\sqldrivers /E /NFL /XF *d.dll
            robocopy %INPUT_SDK_DIR%\apps\Qt5\plugins\imageformats %STAGE_PATH%\imageformats /E /NFL /XF *d.dll
            robocopy %INPUT_SDK_DIR%\apps\Qt5\plugins\crypto %STAGE_PATH%\crypto /E /NFL /XF *d.dll
