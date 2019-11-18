@echo on

curl -fsSL --connect-timeout 60 -o input-sdk-download.zip "%WINSDK%"
7z x input-sdk-download.zip
dir
set INPUT_SDK_DIR=C:\projects\input-sdk\x86_64\stage
robocopy input-sdk-download\ %INPUT_SDK_DIR% /E
dir %INPUT_SDK_DIR%