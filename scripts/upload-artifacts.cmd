@echo on
set APPVEYOR_REPO_NAME=lutraconsulting/input
set DROPBOX_FOLDER="win"

if [%APPVEYOR_REPO_TAG%]==[true] (
    set APK_FILE=inputapp-win-x86_64-%APPVEYOR_REPO_TAG_NAME%.exe
) else (
    set APK_FILE=inputapp-win-x86_64-%APPVEYOR_REPO_COMMIT%.exe
)
set SRC_FILE=C:\projects\input\x86_64\inputapp-win-x86_64.exe
if not exist %SRC_FILE% (echo missing_result & goto error)

echo f | xcopy /f /Y %SRC_FILE% %APK_FILE%

if not ["%APPVEYOR_PULL_REQUEST_TITLE%"]==[""] (
    echo "Deployment of pull request"
    rem set DROPBOX_FOLDER="pulls"
    set GITHUB_API=https://api.github.com/repos/%APPVEYOR_REPO_NAME%/issues/%APPVEYOR_PULL_REQUEST_NUMBER%/comments
) else if ["%APPVEYOR_REPO_TAG%"]==["true"] (
    echo "Deploying tagged release"
    set GITHUB_API=https://api.github.com/repos/%APPVEYOR_REPO_NAME%/commits/%APPVEYOR_REPO_COMMIT%/comments
) else if ["%APPVEYOR_REPO_BRANCH%"]==["master"] (
    echo "Deploying master branch"
    rem set DROPBOX_FOLDER="master"
    set GITHUB_API=https://api.github.com/repos/%APPVEYOR_REPO_NAME%/commits/%APPVEYOR_REPO_COMMIT%/comments
) else (
    echo "Deploying other commit"
    rem set DROPBOX_FOLDER="other"
    set GITHUB_API=https://api.github.com/repos/%APPVEYOR_REPO_NAME%/commits/%APPVEYOR_REPO_COMMIT%/comments
)

rem do not leak DROPBOX_TOKEN
echo "%PYEXE% C:\projects\input\scripts\uploader.py --source %APK_FILE% --destination "/%DROPBOX_FOLDER%/%APK_FILE%""
@echo off
%PYEXE% C:\projects\input\scripts\uploader.py --source %APK_FILE% --destination "/%DROPBOX_FOLDER%/%APK_FILE%" --token DROPBOX_TOKEN  > uploader.log
@echo off

tail -n 1 uploader.log > last_line.log
set /p APK_URL= < last_line.log

echo "Dropbox URL: %APK_URL%"

rem do not leak GITHUB_TOKEN
echo "push to github comment"
echo { "body": "win-apk: [x86_64](%APK_URL%) (SDK: [win-%WINSDK_VER%](https://github.com/lutraconsulting/input-sdk/releases/tag/win-%WINSDK_VER%))"} > github.json
type github.json
@echo off
curl -u inputapp-bot:%GITHUB_TOKEN% -X POST -d @github.json %GITHUB_API% --header "Content-Type: application/json"
@echo off

echo "all done!"
goto end
:error
echo ENV ERROR %ERRORLEVEL%: %DATE% %TIME%
echo "error!"
exit /b 1

:end