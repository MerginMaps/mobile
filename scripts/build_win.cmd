@echo on

call %~dp0\version.cmd
echo Building InputApp: %VERSIONMAJOR%.%VERSIONMINOR%.%VERSIONBUILD%

set OLD_PATH=%PATH%

rem TODO: maybe input-sdk-win-x86_64-%WINSDK_VER% can be dropped for next SDK
set INPUT_SDK_DIR=C:\projects\input-sdk\x86_64\stage\input-sdk-win-x86_64-%WINSDK_VER%
if not exist %INPUT_SDK_DIR% (echo missing_sdk & goto error)
rem TODO: rename cmake/qgis to qgis/cmake so we do not need this workaround
robocopy %INPUT_SDK_DIR%\cmake\qgis %INPUT_SDK_DIR%\cmake\cmake /E /NFL

set ROOT_DIR=C:\projects\input\x86_64
set STAGE_PATH=%ROOT_DIR%\stage
set BUILD_PATH=%ROOT_DIR%\build

set RESULT_FILE=%ROOT_DIR%\inputapp-win-x86_64.exe
set REPO_PATH=%ROOT_DIR%\repo\input
IF EXIST "C:\projects\input\app\input.pro" (
    rem in APPVEYOR environment
    set REPO_PATH=C:\projects\input
)
if not exist %ROOT_DIR% mkdir %ROOT_DIR%
if not exist %BUILD_PATH% mkdir %BUILD_PATH%
if not exist %STAGE_PATH%  mkdir %STAGE_PATH%
IF NOT EXIST %REPO_PATH% (echo INPUT REPO not cloned & goto error)

set CMAKE=C:\Program Files\CMake\bin
set CMAKE_GENERATOR="Visual Studio 14 2015 Win64"

if not "%PROGRAMFILES(X86)%"=="" set PF86=%PROGRAMFILES(X86)%
if "%PF86%"=="" set PF86=%PROGRAMFILES%
if "%PF86%"=="" (echo PROGRAMFILES not set & goto error)

set VS140COMNTOOLS=%PF86%\Microsoft Visual Studio 14.0\Common7\Tools
set VS14ROOT=%PF86%\Microsoft Visual Studio 14.0
call "%VS14ROOT%\VC\vcvarsall.bat" amd64
path %path%;%VS14ROOT%\VC\bin
path %path%;%INPUT_SDK_DIR%\apps\Qt5\bin;%PATH%
path %path%;%CMAKE%

dir "%PF86%\Windows Kits"

set Qt5_DIR=%INPUT_SDK_DIR%\apps\qt5\lib\cmake\Qt5

rem shouldn't all these windows kit includes by set automatically?
set LIB=%INPUT_SDK_DIR%\apps\Qt5\lib;%INPUT_SDK_DIR%\lib
set LIB=%LIB%;%VS14ROOT%\VC\lib\amd64;%PF86%\Windows Kits\8.1\Lib\winv6.3\um\x64
set LIB=%LIB%;%PF86%\Windows Kits\10\Lib\10.0.18362.0\ucrt\x64
set INCLUDE=%INPUT_SDK_DIR%\apps\Qt5\include;%INPUT_SDK_DIR%\include
set INCLUDE=%INCLUDE%;%VS14ROOT%\VC\include;%PF86%\Windows Kits\8.1\Include\um;%PF86%\Windows Kits\8.1\Include\shared
set INCLUDE=%INCLUDE%;%PF86%\Windows Kits\10\Include\10.0.18362.0\ucrt

cd %BUILD_PATH%
qmake CONFIG+=release %REPO_PATH%\app
nmake release VERBOSE=1
IF %ERRORLEVEL% NEQ 0 (echo unable to compile & goto error)
rem for debugging use %BUILD_PATH%\release\*.pdb
IF NOT EXIST "%BUILD_PATH%\release\Input.exe" goto error

:package

rem Input
xcopy %BUILD_PATH%\release\Input.exe %STAGE_PATH%\ /Y

call %INPUT_SDK_DIR%\apps\Qt5\bin\qtenv2.bat
%INPUT_SDK_DIR%\apps\Qt5\bin\windeployqt --release %STAGE_PATH%\Input.exe 

robocopy %REPO_PATH%\app\android\assets\qgis-data %STAGE_PATH%\qgis-data /E /NFL
robocopy %REPO_PATH%\app\android\assets\demo-projects %STAGE_PATH%\demo-projects /E /NFL
if not exist %STAGE_PATH%\images mkdir %STAGE_PATH%\images
xcopy %REPO_PATH%\images\AppIcon.ico %STAGE_PATH%\images\ /Y
more /P %REPO_PATH%\LICENSE > %STAGE_PATH%\license.txt

rem OSGeo
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


cd %STAGE_PATH%
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
