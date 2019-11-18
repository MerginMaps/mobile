@echo on
set INPUT_SDK_DIR=C:\projects\input-sdk\x86_64\stage

curl -fsSL --connect-timeout 60 -o input-sdk-download.zip "%WINSDK%"
7z x input-sdk-download.zip -o%INPUT_SDK_DIR%
dir %INPUT_SDK_DIR%