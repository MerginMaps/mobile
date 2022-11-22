@echo on

set STAGE_PATH=%PACKAGE_DIR%\stage
set CWD=%cd%
echo "Packaging Win64 Mergin Maps Input release"
echo "STAGE_PATH %STAGE_PATH%"
echo "INPUT_SOURCE_DIR: %INPUT_SOURCE_DIR%"
echo "INPUT_BUILD_DIR: %INPUT_BUILD_DIR%"
echo "INPUT_SDK_DIR: %INPUT_SDK_DIR%"
echo "Qt6_Dir: %Qt6_Dir%"

IF NOT EXIST %STAGE_PATH% (echo err_STAGE_PATH & goto error)
IF NOT EXIST %INPUT_SOURCE_DIR%\CMakeLists.txt (echo err_INPUT_SOURCE_DIR & goto error)
IF NOT EXIST %INPUT_BUILD_DIR%\Input.exe (echo err_INPUT_BUILD_DIR & goto error)
IF NOT EXIST %INPUT_SDK_DIR%\bin\geodiff.dll (echo err_INPUT_SDK_DIR & goto error)
IF NOT EXIST %Qt6_Dir%\bin\qmake.exe (echo err_Qt6_Dir & goto error)

cd %STAGE_PATH%
xcopy %INPUT_BUILD_DIR%\Input.exe %STAGE_PATH%\ /Y
windeployqt --release Input.exe

if not exist %STAGE_PATH%\images mkdir %STAGE_PATH%\images
xcopy %INPUT_SOURCE_DIR%\app\win\AppIcon.ico %STAGE_PATH%\images\ /Y
echo GNU GENERAL PUBLIC LICENSE Version 2 > %STAGE_PATH%\license.txt

xcopy %INPUT_SDK_DIR%\bin\*.dll %STAGE_PATH%\ /Y
robocopy %INPUT_SDK_DIR%\bin\Qca\crypto %STAGE_PATH%\ /E /NFL /XF
xcopy %INPUT_SDK_DIR%\tools\qgis\plugins\*.dll %STAGE_PATH%\ /Y 
xcopy %INPUT_SOURCE_DIR%\app\i18n\*.qm %STAGE_PATH%\translations /Y 

robocopy %Qt6_Dir%\qml\Qt %STAGE_PATH%\qml\Qt /E /NFL /XF *d.dll
robocopy %Qt6_Dir%\qml\QtLocation %STAGE_PATH%\qml\QtLocation /E /NFL /XF *d.dll
robocopy %Qt6_Dir%\qml\QtMultimedia %STAGE_PATH%\qml\QtMultimedia /E /NFL /XF *d.dll
robocopy %Qt6_Dir%\qml\QtQml %STAGE_PATH%\qml\QtQml /E /NFL /XF *d.dll
robocopy %Qt6_Dir%\qml\QtQuick %STAGE_PATH%\qml\QtQuick /E /NFL /XF *d.dll
robocopy %Qt6_Dir%\qml\QtQuick.2 %STAGE_PATH%\qml\QtQuick.2 /E /NFL /XF *d.dll
robocopy %Qt6_Dir%\qml\QtPositioning %STAGE_PATH%\qml\QtPositioning /E /NFL /XF *d.dll
robocopy %Qt6_Dir%\qml\QtGraphicalEffects %STAGE_PATH%\qml\QtGraphicalEffects /E /NFL /XF *d.dll

robocopy %Qt6_Dir%\plugins\position %STAGE_PATH%\position /E /NFL /XF *d.dll
robocopy %Qt6_Dir%\plugins\sqldrivers %STAGE_PATH%\sqldrivers /E /NFL /XF *d.dll
robocopy %Qt6_Dir%\plugins\imageformats %STAGE_PATH%\imageformats /E /NFL /XF *d.dll

xcopy %Qt6_Dir%\bin\Qt6Sql.dll %STAGE_PATH%\ /Y
xcopy %Qt6_Dir%\bin\Qt6Concurrent.dll %STAGE_PATH%\ /Y
xcopy %Qt6_Dir%\bin\Qt6PrintSupport.dll %STAGE_PATH%\ /Y
xcopy %Qt6_Dir%\bin\Qt6Xml.dll %STAGE_PATH%\ /Y
xcopy %Qt6_Dir%\bin\Qt6Positioning.dll %STAGE_PATH%\ /Y
xcopy %Qt6_Dir%\bin\Qt6Location.dll %STAGE_PATH%\ /Y
xcopy %Qt6_Dir%\bin\Qt6Sensors.dll %STAGE_PATH%\ /Y
xcopy %Qt6_Dir%\bin\Qt6QuickTemplates2.dll %STAGE_PATH%\ /Y
xcopy %Qt6_Dir%\bin\Qt6QuickControls2.dll %STAGE_PATH%\ /Y
xcopy %Qt6_Dir%\bin\Qt6QuickWidgets.dll %STAGE_PATH%\ /Y
xcopy %Qt6_Dir%\bin\Qt6QuickParticles.dll %STAGE_PATH%\ /Y
xcopy %Qt6_Dir%\bin\Qt6Quick.dll %STAGE_PATH%\ /Y
xcopy %Qt6_Dir%\bin\Qt6PositioningQuick.dll %STAGE_PATH%\ /Y
xcopy %Qt6_Dir%\bin\Qt6MultimediaQuick.dll %STAGE_PATH%\ /Y
xcopy %Qt6_Dir%\bin\Qt6Multimedia.dll %STAGE_PATH%\ /Y
xcopy %Qt6_Dir%\bin\Qt6Bluetooth.dll %STAGE_PATH%\ /Y
xcopy %Qt6_Dir%\bin\Qt6XmlPatterns.dll %STAGE_PATH%\ /Y
xcopy %Qt6_Dir%\bin\Qt6QuickShapes.dll %STAGE_PATH%\ /Y
xcopy %Qt6_Dir%\bin\Qt6Qml.dll %STAGE_PATH%\ /Y
xcopy %Qt6_Dir%\bin\Qt6Network.dll %STAGE_PATH%\ /Y
xcopy %Qt6_Dir%\bin\Qt6QmlWorkerScript.dll %STAGE_PATH%\ /Y

robocopy %INPUT_SOURCE_DIR%\app\android\assets\qgis-data %STAGE_PATH%\qgis-data /E /NFL
robocopy %INPUT_SOURCE_DIR%\app\android\assets\demo-projects %STAGE_PATH%\demo-projects /E /NFL


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