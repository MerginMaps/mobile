rem should be started from build-Input folder
rem PACKAGE_DIR variable should be defined
set STAGE_PATH=%PACKAGE_DIR%\stage
set CWD=%cd%
echo "Packaging Win64 Mergin Maps Input release"
echo "STAGE_PATH %STAGE_PATH%"
echo "INPUT_SOURCE_DIR: %INPUT_SOURCE_DIR%"
echo "INPUT_BUILD_DIR: %INPUT_BUILD_DIR%"
echo "INPUT_SDK_DIR: %INPUT_SDK_DIR%"
echo "Qt5_Dir: %Qt5_Dir%"

IF NOT EXIST %STAGE_PATH% (echo err_STAGE_PATH & goto error)
IF NOT EXIST %INPUT_SOURCE_DIR%\app\input.pro (echo err_INPUT_SOURCE_DIR & goto error)
IF NOT EXIST %INPUT_BUILD_DIR%\release\Input.exe (echo err_INPUT_BUILD_DIR & goto error)
IF NOT EXIST %INPUT_SDK_DIR%\bin\geodiff.dll (echo err_INPUT_SDK_DIR & goto error)
IF NOT EXIST %Qt5_Dir%\bin\qmake.exe (echo err_Qt5_Dir & goto error)

cd %STAGE_PATH%
xcopy %INPUT_BUILD_DIR%\release\Input.exe %STAGE_PATH%\ /Y
windeployqt --release Input.exe

robocopy %INPUT_SOURCE_DIR%\app\android\assets\qgis-data %STAGE_PATH%\qgis-data /E /NFL
robocopy %INPUT_SOURCE_DIR%\app\android\assets\demo-projects %STAGE_PATH%\demo-projects /E /NFL

if not exist %STAGE_PATH%\images mkdir %STAGE_PATH%\images
xcopy %INPUT_SOURCE_DIR%\images\AppIcon.ico %STAGE_PATH%\images\ /Y
more /P %INPUT_SOURCE_DIR%\LICENSE > %STAGE_PATH%\license.txt

xcopy %INPUT_SDK_DIR%\bin\*.dll %STAGE_PATH%\ /Y
xcopy %INPUT_SDK_DIR%\bin\Qca\crypto\*.dll %STAGE_PATH%\ /Y


xcopy %INPUT_SDK_DIR%\plugins\*provider.dll %STAGE_PATH%\ /Y

rem Qt5
xcopy %Qt5_Dir%\bin\Qt5Sql.dll %STAGE_PATH%\ /Y
xcopy %Qt5_Dir%\bin\Qt5Concurrent.dll %STAGE_PATH%\ /Y
xcopy %Qt5_Dir%\bin\Qt5Xml.dll %STAGE_PATH%\ /Y
xcopy %Qt5_Dir%\bin\Qt5PrintSupport.dll %STAGE_PATH%\ /Y
xcopy %Qt5_Dir%\bin\qca-qt5.dll %STAGE_PATH%\ /Y
xcopy %Qt5_Dir%\bin\qt5keychain.dll %STAGE_PATH%\ /Y
xcopy %Qt5_Dir%\bin\Qt5Positioning.dll %STAGE_PATH%\ /Y
xcopy %Qt5_Dir%\bin\Qt5Location.dll %STAGE_PATH%\ /Y
xcopy %Qt5_Dir%\bin\Qt5Sensors.dll %STAGE_PATH%\ /Y
xcopy %Qt5_Dir%\bin\Qt5QuickTemplates2.dll %STAGE_PATH%\ /Y
xcopy %Qt5_Dir%\bin\Qt5QuickControls2.dll %STAGE_PATH%\ /Y
xcopy %Qt5_Dir%\bin\Qt5QuickWidgets.dll %STAGE_PATH%\ /Y
xcopy %Qt5_Dir%\bin\Qt5QuickParticles.dll %STAGE_PATH%\ /Y
xcopy %Qt5_Dir%\bin\Qt5Quick.dll %STAGE_PATH%\ /Y
xcopy %Qt5_Dir%\bin\Qt5PositioningQuick.dll %STAGE_PATH%\ /Y
xcopy %Qt5_Dir%\bin\Qt5MultimediaQuick.dll %STAGE_PATH%\ /Y
xcopy %Qt5_Dir%\bin\Qt5Multimedia.dll %STAGE_PATH%\ /Y

robocopy %Qt5_Dir%\qml\Qt %STAGE_PATH%\qml\Qt /E /NFL /XF *d.dll
robocopy %Qt5_Dir%\qml\QtLocation %STAGE_PATH%\qml\QtLocation /E /NFL /XF *d.dll
robocopy %Qt5_Dir%\qml\QtMultimedia %STAGE_PATH%\qml\QtMultimedia /E /NFL /XF *d.dll
robocopy %Qt5_Dir%\qml\QtQml %STAGE_PATH%\qml\QtQml /E /NFL /XF *d.dll
robocopy %Qt5_Dir%\qml\QtQuick %STAGE_PATH%\qml\QtQuick /E /NFL /XF *d.dll
robocopy %Qt5_Dir%\qml\QtQuick.2 %STAGE_PATH%\qml\QtQuick.2 /E /NFL /XF *d.dll
robocopy %Qt5_Dir%\qml\QtPositioning %STAGE_PATH%\qml\QtPositioning /E /NFL /XF *d.dll
robocopy %Qt5_Dir%\qml\QtGraphicalEffects %STAGE_PATH%\qml\QtGraphicalEffects /E /NFL /XF *d.dll

robocopy %Qt5_Dir%\plugins\position %STAGE_PATH%\position /E /NFL /XF *d.dll
robocopy %Qt5_Dir%\plugins\sqldrivers %STAGE_PATH%\sqldrivers /E /NFL /XF *d.dll
robocopy %Qt5_Dir%\plugins\imageformats %STAGE_PATH%\imageformats /E /NFL /XF *d.dll
robocopy %Qt5_Dir%\plugins\crypto %STAGE_PATH%\crypto /E /NFL /XF *d.dll

cd %CWD%

echo "all done!"
goto end

:error
echo ENV ERROR %ERRORLEVEL%: %DATE% %TIME%
path %OLD_PATH%
cd %~dp0
echo "error!"
exit /b 1

:end