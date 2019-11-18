curl -fsSL --connect-timeout 60 -o geodiff.tar.gz %URL_geodiff%
7z x %WINSDK% -so | 7z x -aoa -si -ttar -o"input-sdk"
dir input-sdk
set INPUT_SDK_DIR=C:\projects\input-sdk\x86_64\stage
robocopy input-sdk %INPUT_SDK_DIR% /E
dir %INPUT_SDK_DIR%