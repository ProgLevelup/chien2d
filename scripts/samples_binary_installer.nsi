; The name of the installer
Name "Exemplos Chien2d"

; The file to write
OutFile "Chien2d_samples_bin.exe"

; The default installation directory
InstallDir $PROGRAMFILES32\Chien2d\Exemplos\

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
  File "..\Exemplos\Release\01-imagens.exe"
  File "..\Exemplos\Release\02-loopjogo.exe"
  File "..\Exemplos\Release\03-animacao.exe"
  File "..\Exemplos\Release\04-microdemo.exe"
  File "..\Exemplos\Release\05-megademo.exe"
  File "..\Exemplos\Release\06-baloes.exe"
  File "..\Exemplos\Release\07-tilemap.exe"
  File "..\Exemplos\Release\08-plataforma.exe"
  File "..\Exemplos\Release\09-topview.exe"
  File "..\Exemplos\Release\10-subpixel.exe"  
  
  File "..\Exemplos\01-imagens\darkphoenix.png"
  File "..\Exemplos\01-imagens\darkphoenix_alpha.png"
  File "..\Exemplos\01-imagens\imagem.jpg"
  
  File "..\Exemplos\02-loopjogo\isabelle64.png"
  
  File "..\Exemplos\04-microdemo\arial32.png"
  
  SetOutPath $INSTDIR\dados
  
  File "..\Exemplos\05-megademo\dados\2nd_PurpleMotion.s3m"
  File "..\Exemplos\05-megademo\dados\arial32.png"
  File "..\Exemplos\05-megademo\dados\atriz.png" 
  File "..\Exemplos\05-megademo\dados\digital.png" 
  File "..\Exemplos\05-megademo\dados\digital_solido.png" 
  File "..\Exemplos\05-megademo\dados\estrela.png"
  File "..\Exemplos\05-megademo\dados\isabelle64.png" 
  File "..\Exemplos\05-megademo\dados\titulo.png" 
  File "..\Exemplos\05-megademo\dados\tuxwindows.jpg"  
  
  SetOutPath $INSTDIR\graficos 
  
  File "..\Exemplos\06-baloes\graficos\arial32.png"
  File "..\Exemplos\06-baloes\graficos\balao.png"
  File "..\Exemplos\06-baloes\graficos\botao_creditos.png"
  File "..\Exemplos\06-baloes\graficos\botao_jogar.png"
  File "..\Exemplos\06-baloes\graficos\botao_sair.png"
  File "..\Exemplos\06-baloes\graficos\botao_voltar.png"
  File "..\Exemplos\06-baloes\graficos\isabelle64.png"
  File "..\Exemplos\06-baloes\graficos\mira.png"
  File "..\Exemplos\06-baloes\graficos\titulo.png"
  
  SetOutPath $INSTDIR
  
  File "..\Exemplos\07-tilemap\tileset.png"
  
  SetOutPath $INSTDIR\audio
  
  File "..\Exemplos\08-plataforma\audio\AulaPaulo_byPiovezan.it"
  File "..\Exemplos\08-plataforma\audio\bate.wav"
  File "..\Exemplos\08-plataforma\audio\morreu.wav"
  File "..\Exemplos\08-plataforma\audio\pula.wav"
  File "..\Exemplos\08-plataforma\audio\venceu.wav"
  
  SetOutPath $INSTDIR\fases
  
  File "..\Exemplos\08-plataforma\fases\Aula04-Mapa.BAK"
  File "..\Exemplos\08-plataforma\fases\Aula04-Mapa.FMP"
  File "..\Exemplos\08-plataforma\fases\Aula04-Mapa2.BAK"
  File "..\Exemplos\08-plataforma\fases\Aula04-Mapa2.FMP"
  File "..\Exemplos\08-plataforma\fases\Aula04-tileset.png"
  File "..\Exemplos\08-plataforma\fases\fase1.BAK"
  File "..\Exemplos\08-plataforma\fases\fase1.FMP"
  File "..\Exemplos\08-plataforma\fases\fundo.jpg"
  File "..\Exemplos\08-plataforma\fases\plataforma_camadas.png"
  File "..\Exemplos\08-plataforma\fases\plataforma_camadas_alpha.png"
  File "..\Exemplos\08-plataforma\fases\plataforma_camadas_alpha2.png"
  
  SetOutPath $INSTDIR\imagens
  
  File "..\Exemplos\08-plataforma\imagens\*.*"
  
  SetOutPath $INSTDIR
  
  File "..\Exemplos\09-topview\arena.fmp"
  File "..\Exemplos\09-topview\arena.png"
  File "..\Exemplos\09-topview\arenainclinada.FMP"
  
  File "..\Exemplos\10-subpixel\*.png"
                                    
  File "..\Dependencias\SDL\lib\SDL.dll"
  File "..\Dependencias\SDL_image\lib\SDL_image.dll"
  File "..\Dependencias\SDL_image\lib\jpeg.dll"   
  File "..\Dependencias\SDL_image\lib\libpng12-0.dll"
  File "..\Dependencias\SDL_image\lib\libtiff-3.dll"
  File "..\Dependencias\SDL_image\lib\zlib1.dll"
  File "..\Dependencias\SDL_mixer\lib\libogg-0.dll"
  File "..\Dependencias\SDL_mixer\lib\libvorbis-0.dll"
  File "..\Dependencias\SDL_mixer\lib\libvorbisfile-3.dll"
  File "..\Dependencias\SDL_mixer\lib\mikmod.dll"
  File "..\Dependencias\SDL_mixer\lib\SDL_mixer.dll"
  File "..\Dependencias\SDL_mixer\lib\smpeg.dll"
  File "..\Dependencias\lua\lua5.1.dll"    
  
SectionEnd ; end the section
