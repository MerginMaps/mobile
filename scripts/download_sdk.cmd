@echo on

curl -fsSL --connect-timeout 60 -o input-sdk.zip %WINSDK%
7z x input-sdk.zip -so | 7z x -aoa -si -ttar -o"sdk"
set INPUT_SDK_DIR=C:\projects\input-sdk\x86_64\stage
robocopy sdk %INPUT_SDK_DIR% /E
dir %INPUT_SDK_DIR%