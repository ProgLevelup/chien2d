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
  
  ;The files are listed one by one to easily detect files change
  
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
  
  SetOutPath $INSTDIR\graficos
  
  File "..\Exemplos\dados\graficos\arena.png"
  File "..\Exemplos\dados\graficos\atriz.png"
  File "..\Exemplos\dados\graficos\atriz_alpha.png"
  File "..\Exemplos\dados\graficos\balao.png"
  File "..\Exemplos\dados\graficos\botao_creditos.png"
  File "..\Exemplos\dados\graficos\botao_jogar.png"
  File "..\Exemplos\dados\graficos\botao_sair.png"
  File "..\Exemplos\dados\graficos\botao_voltar.png"
  File "..\Exemplos\dados\graficos\darkphoenix.png"
  File "..\Exemplos\dados\graficos\darkphoenix_alpha _white.png"
  File "..\Exemplos\dados\graficos\darkphoenix_alpha.png"
  File "..\Exemplos\dados\graficos\darkphoenix_alpha2.png"
  File "..\Exemplos\dados\graficos\darkphoenix_alpha3.png"
  File "..\Exemplos\dados\graficos\estrela.png"
  File "..\Exemplos\dados\graficos\help.png"
  File "..\Exemplos\dados\graficos\help.xcf"  
  File "..\Exemplos\dados\graficos\imagem.jpg"
  File "..\Exemplos\dados\graficos\inimigo.png"
  File "..\Exemplos\dados\graficos\inimigobola.png"
  File "..\Exemplos\dados\graficos\mira.png"
  File "..\Exemplos\dados\graficos\rotacao0.png"  
  File "..\Exemplos\dados\graficos\rotacao1.png"
  File "..\Exemplos\dados\graficos\rotacao2.png"
  File "..\Exemplos\dados\graficos\rotacao3.png"
  File "..\Exemplos\dados\graficos\rotacaoerrado.png"
  File "..\Exemplos\dados\graficos\rotacaoerrado0.png"
  File "..\Exemplos\dados\graficos\rotacaoerrado1.png"
  File "..\Exemplos\dados\graficos\rotacaoerrado2.png"
  File "..\Exemplos\dados\graficos\rotacaoerrado3.png"
  File "..\Exemplos\dados\graficos\scrollerrado.png"
  File "..\Exemplos\dados\graficos\scrollmaiscorreto.png"
  File "..\Exemplos\dados\graficos\tileset.png"
  File "..\Exemplos\dados\graficos\titulo.png"
  File "..\Exemplos\dados\graficos\titulo_baloes.png"
  File "..\Exemplos\dados\graficos\tuxwindows.jpg"  
  File "..\Exemplos\dados\graficos\Aula04-tileset.png"
  File "..\Exemplos\dados\graficos\fundo.jpg"
  File "..\Exemplos\dados\graficos\plataforma_camadas.png"
  File "..\Exemplos\dados\graficos\plataforma_camadas_alpha.png"
  File "..\Exemplos\dados\graficos\plataforma_camadas_alpha2.png"
  
  SetOutPath $INSTDIR\fontes
  
  File "..\Exemplos\dados\fontes\arial32.png"
  File "..\Exemplos\dados\fontes\arial32_pink.png"
  File "..\Exemplos\dados\fontes\arial32_pink2.png"
  File "..\Exemplos\dados\fontes\digital.png"
  File "..\Exemplos\dados\fontes\digital_solido.png"  
  File "..\Exemplos\dados\fontes\isabelle64.png"
  File "..\Exemplos\dados\fontes\isabelle64_alpha.png"
  File "..\Exemplos\dados\fontes\isabelle64_alpha2.png"
  File "..\Exemplos\dados\fontes\Untitled.bmp"
  
  SetOutPath $INSTDIR\fases
  
  File "..\Exemplos\dados\fases\arena.fmp"
  File "..\Exemplos\dados\fases\arenainclinada.FMP"
  File "..\Exemplos\dados\fases\Aula04-Mapa.FMP"
  File "..\Exemplos\dados\fases\Aula04-Mapa2.FMP"
  File "..\Exemplos\dados\fases\fase1.FMP"
  File "..\Exemplos\dados\fases\tilemap.FMP"    
  
  SetOutPath $INSTDIR\audio
  
  File "..\Exemplos\dados\audio\2nd_PurpleMotion.s3m"
  File "..\Exemplos\dados\audio\AulaPaulo_byPiovezan.it"
  File "..\Exemplos\dados\audio\bate.ogg"
  File "..\Exemplos\dados\audio\bate.wav"
  File "..\Exemplos\dados\audio\morreu.wav"
  File "..\Exemplos\dados\audio\pegada.ogg"
  File "..\Exemplos\dados\audio\Phenomena_Firefox&Tip.s3m"
  File "..\Exemplos\dados\audio\pula.wav"
  File "..\Exemplos\dados\audio\screendoorclose.wav"  
  File "..\Exemplos\dados\audio\venceu.wav"  
  
  SetOutPath $INSTDIR\
                                    
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
