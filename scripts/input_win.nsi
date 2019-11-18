; The name of the installer
Name "InputApp"

; The file to write
OutFile "C:\projects\input\x86_64\inputapp-win-x86_64.exe"

; The default installation directory
InstallDir $DESKTOP\InputApp

; Request application privileges for Windows Vista
RequestExecutionLevel user

;--------------------------------

; Pages

Page directory
Page instfiles

;--------------------------------

; The stuff to install
Section "" ;No components page, name is not important

  ; Set output path to the installation directory.
  SetOutPath $INSTDIR
  
  ; Put file there
  File /r "C:\projects\input\x86_64\stage\*"
  
SectionEnd ; end the section
