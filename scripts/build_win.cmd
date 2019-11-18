@echo on

set OLD_PATH=%PATH%
set INPUT_SDK_DIR=C:\projects\input-sdk\x86_64\stage
if not exist %INPUT_SDK_DIR% (echo missing_sdk & goto error)
set ROOT_DIR=C:\projects\input\x86_64
set STAGE_PATH=%ROOT_DIR%\stage
set BUILD_PATH=%ROOT_DIR%\build
IF EXIST "C:\projects\input\app\input.pro" (
    rem in APPVEYOR environment
    set REPO_PATH=C:\projects\input
)
if not exist %ROOT_DIR% mkdir %ROOT_DIR%
if not exist %BUILD_PATH% mkdir %BUILD_PATH%
if not exist %STAGE_PATH%  mkdir %STAGE_PATH%
IF NOT EXIST %REPO_PATH% (echo INPUT REPO not cloned & goto error)

if not "%PROGRAMFILES(X86)%"=="" set PF86=%PROGRAMFILES(X86)%
if "%PF86%"=="" set PF86=%PROGRAMFILES%
if "%PF86%"=="" (echo PROGRAMFILES not set & goto error)
set VS140COMNTOOLS=%PF86%\Microsoft Visual Studio 14.0\Common7\Tools
set VS14ROOT=%PF86%\Microsoft Visual Studio 14.0
call "%VS14ROOT%\VC\vcvarsall.bat" amd64
path %path%;%VS14ROOT%\VC\bin
path %path%;%INPUT_SDK_DIR%\apps\Qt5\bin;%PATH%

cd %BUILD_PATH%
IF NOT EXIST "%BUILD_PATH%\release\Input.exe" (
  qmake CONFIG+=force_debug_info %REPO_PATH%\app
  nmake release VERBOSE=1
  rem for debugging use %BUILD_PATH%\release\*.pdb
)
IF NOT EXIST "%BUILD_PATH%\release\Input.exe" goto error

:package

xcopy %BUILD_PATH%\release\Input.exe %STAGE_PATH%\ /Y

call %INPUT_SDK_DIR%\apps\Qt5\bin\qtenv2.bat
%INPUT_SDK_DIR%\apps\Qt5\bin\windeployqt --release %STAGE_PATH%\Input.exe 

rem OSGeo
xcopy %INPUT_SDK_DIR%\bin\qgis_core.dll %STAGE_PATH%\ /Y
xcopy %INPUT_SDK_DIR%\bin\qgis_native.dll %STAGE_PATH%\ /Y
xcopy %INPUT_SDK_DIR%\bin\qgis_quick.dll %STAGE_PATH%\ /Y
xcopy %INPUT_SDK_DIR%\bin\proj*.dll %STAGE_PATH%\ /Y
xcopy %INPUT_SDK_DIR%\bin\geos_c.dll %STAGE_PATH%\ /Y 
xcopy %INPUT_SDK_DIR%\bin\gdal3*.dll %STAGE_PATH%\ /Y 
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

robocopy %INPUT_SDK_DIR%\qml %STAGE_PATH%\qml /E
robocopy %INPUT_SDK_DIR%\images\QgsQuick %STAGE_PATH%\images\QgsQuick /E

rem system
xcopy %VS14ROOT%\VC\redist\x64\Microsoft.VC140.CRT\vcruntime140.dll %STAGE_PATH%\ /Y
xcopy %VS14ROOT%\VC\redist\x64\Microsoft.VC140.CRT\vccorlib140.dll %STAGE_PATH%\ /Y
xcopy %VS14ROOT%\VC\redist\x64\Microsoft.VC140.CRT\msvcp140.dll %STAGE_PATH%\ /Y

rem Qt5
xcopy %INPUT_SDK_DIR%\apps\Qt5\bin\Qt5Sql.dll %STAGE_PATH%\ /Y
xcopy %INPUT_SDK_DIR%\apps\Qt5\bin\Qt5Concurrent.dll %STAGE_PATH%\ /Y
xcopy %INPUT_SDK_DIR%\apps\Qt5\bin\Qt5Xml.dll %STAGE_PATH%\ /Y
xcopy %INPUT_SDK_DIR%\apps\Qt5\bin\Qt5PrintSupport.dll %STAGE_PATH%\ /Y
xcopy %INPUT_SDK_DIR%\apps\Qt5\bin\qca-qt5.dll %STAGE_PATH%\ /Y
xcopy %INPUT_SDK_DIR%\apps\Qt5\bin\qt5keychain.dll %STAGE_PATH%\ /Y
xcopy %INPUT_SDK_DIR%\apps\Qt5\bin\Qt5Positioning.dll %STAGE_PATH%\ /Y
xcopy %INPUT_SDK_DIR%\apps\Qt5\bin\Qt5Location.dll %STAGE_PATH%\ /Y
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

robocopy %INPUT_SDK_DIR%\apps\Qt5\plugins\position %STAGE_PATH%\position /E /NFL /XF *d.dll
robocopy %INPUT_SDK_DIR%\apps\Qt5\plugins\sqldrivers %STAGE_PATH%\sqldrivers /E /NFL /XF *d.dll
robocopy %INPUT_SDK_DIR%\apps\Qt5\plugins\imageformats %STAGE_PATH%\imageformats /E /NFL /XF *d.dll
robocopy %INPUT_SDK_DIR%\apps\Qt5\plugins\crypto %STAGE_PATH%\crypto /E /NFL /XF *d.dll

IF NOT EXIST %RESULT_FILE% "C:\Program Files (x86)\NSIS\makensis.exe" %REPO_PATH%\scripts\input_win.nsi
dir %RESULT_FILE%

IF NOT EXIST %RESULT_FILE% goto error


set PATH=%OLD_PATH%
cd %~dp0
echo "all done!"
goto end

:error
echo ENV ERROR %ERRORLEVEL%: %DATE% %TIME%
path %OLD_PATH%
cd %~dp0
echo "error!"
exit /b 1

:end