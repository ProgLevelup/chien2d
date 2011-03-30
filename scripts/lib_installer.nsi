; The name of the installer
Name "Bibliotecas Chien2d"

; The file to write
OutFile "Chien2d_libs.exe"

; The default installation directory
InstallDir $PROGRAMFILES32\Chien2d

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
  SetOutPath $INSTDIR\include\
  
  ; Put file there
  File "..\Chien2D2\include\c2d2\*.h"
  
  SetOutPath $INSTDIR\lib
  File "..\Chien2D2\lib\*.lib"
  
  SetOutPath $INSTDIR
  File "..\Chien2D2\*.txt" 
  
SectionEnd ; end the section
