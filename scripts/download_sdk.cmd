curl -fsSL --connect-timeout 60 -o geodiff.tar.gz %URL_geodiff%
7z x %WINSDK% -so | 7z x -aoa -si -ttar -o"input-sdk"
dir input-sdk
