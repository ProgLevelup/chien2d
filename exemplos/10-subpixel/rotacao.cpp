/*
Copyright 2008-2010, Paulo Vinicius Wolski Radtke (pvwradtke@gmail.com)

   Licensed under the Apache License, Version 2.0 (the "License");
   you may not use this file except in compliance with the License.
   You may obtain a copy of the License at

       http://www.apache.org/licenses/LICENSE-2.0

   Unless required by applicable law or agreed to in writing, software
   distributed under the License is distributed on an "AS IS" BASIS,
   WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
   See the License for the specific language governing permissions and
   limitations under the License.
*/


#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <c2d2/chien2d2.h>

// Cuidado: o código só aceita largura de tela menor ou igual a 2040.
#define LARGTELA	1024
#define ALTTELA		768
#define TAMSPRITE   126
#define PI		3.1415926535897932384626433832795
#define RAD		PI/180


int main(int narg, char **valarg)
{
	// Inicia a Chien2D 2 e testa se deu tudo certo
	if(C2D2_Inicia(LARGTELA, ALTTELA, C2D2_JANELA, C2D2_DESENHO_PADRAO, "Rotação"))
		printf("Iniciou a Chien2D 2 com sucesso\n");
	else
	{
		printf("Falhou ao iniciar a Chien2D 2. Encerrando o programa.\n");
		return 0;
	}
	// os sprites
	unsigned int spritescertos[4], spriteserrados[4], help;
	// Coordenadas na tela
	float xf[16], yf[16], xtrf[16], ytrf[16];
	int xi[16], yi[16], xtri[16], ytri[16];
	float angulo = 0;
	// Escolhe se desenha certo ou não
	bool certo = false;
	// O zoom a aplicar
	int zoom = 200;
	for(int i=0;i<4;i++)
	{
	    char nome[40];
	    sprintf(nome, "graficos/rotacao%i.png", i);
	    spritescertos[i]= C2D2_CarregaSpriteSet(nome, TAMSPRITE+2, TAMSPRITE+2);    
    	xf[0+i*4] = xf[3+i*4] = -TAMSPRITE + TAMSPRITE*(i%2);
        xf[1+i*4] = xf[2+i*4] = (i%2)*TAMSPRITE;
    	yf[0+i*4] = yf[1+i*4] = (TAMSPRITE - (i/2)*TAMSPRITE); 
    	yf[2+i*4] = yf[3+i*4] = -(i/2)*TAMSPRITE;  
        // Faz o mesmo para os sprites errados
   	    sprintf(nome, "graficos/rotacaoerrado%i.png", i);
	    spriteserrados[i]= C2D2_CarregaSpriteSet(nome, TAMSPRITE, TAMSPRITE);    
    	xi[0+i*4] = xi[3+i*4] = -TAMSPRITE + TAMSPRITE*(i%2);
        xi[1+i*4] = xi[2+i*4] = (i%2)*TAMSPRITE;
    	yi[0+i*4] = yi[1+i*4] = (TAMSPRITE - (i/2)*TAMSPRITE); 
    	yi[2+i*4] = yi[3+i*4] = -(i/2)*TAMSPRITE;  
	}
	help = C2D2_CarregaSpriteSet("graficos/help.png", 0, 0);
	// Recupera o teclado
	C2D2_Botao *teclas = C2D2_PegaTeclas();
	// Recupera o mouse
	C2D2_Mouse *mouse = C2D2_PegaMouse();
    C2D2_TrocaCorLimpezaTela(255,0,0);
	do
	{
	    // Muda o zoom?
	    if(teclas[C2D2_Z].ativo && zoom > 1)
	        zoom--;
	    else if(teclas[C2D2_X].ativo && zoom < 400)
	        zoom++;
	    // Troca o render?
	    if(teclas[C2D2_ESPACO].pressionado)
	        certo = !certo;
	    // Calcula o novo angulo
	    if(teclas[C2D2_ENTER].ativo)
	        angulo+=1.5;
	    else
	        angulo+=0.3;
	    if(angulo>360)
	        angulo-=360;
	    // Rotaciona os pontos
	    for(int i=0;i<16;i++)
	    {
	        // Os pontos certos
            xtrf[i] = LARGTELA/2 + (xf[i]*zoom/100.0)*cos(RAD*angulo) + (yf[i]*zoom/100.0)*sin(RAD*angulo);
            ytrf[i] = ALTTELA/2 - ((xf[i]*zoom/100.0)*-sin(RAD*angulo) + (yf[i]*zoom/100.0)*cos(RAD*angulo));
	        // Os pontos errados
            xtri[i] = LARGTELA/2 + (xi[i]*zoom/100.0)*cos(RAD*angulo) + (yi[i]*zoom/100.0)*sin(RAD*angulo);
            ytri[i] = ALTTELA/2 - ((xi[i]*zoom/100.0)*-sin(RAD*angulo) + (yi[i]*zoom/100.0)*cos(RAD*angulo));
            
	    }
		// Limpa a tela com a cor de fundo atual
		C2D2_LimpaTela();
		// Desenha os sprites
		for(int i=0;i<4;i++)
		{
		    if(certo)
			    C2D2_DesenhaSpriteEfeitoSubpixel(spritescertos[i], 0, &xtrf[i*4], &ytrf[i*4], 255, 255, 255, 255);
			 else
			    C2D2_DesenhaSpriteEfeito(spriteserrados[i], 0, &xtri[i*4], &ytri[i*4], 255, 255, 255, 255);
	    }
	    C2D2_DesenhaSprite(help, 0, 10, 10);
		
		// Faz a troca do front buffer com o back buffer e atualiza as entradas
		C2D2_Sincroniza(C2D2_FPS_PADRAO);
	}
	while(!teclas[C2D2_ENCERRA].pressionado && !teclas[C2D2_ESC].pressionado);
	// Remove as imagens
	for(int i=0;i<4;i++)
	{
        C2D2_RemoveSpriteSet(spritescertos[i]);
        C2D2_RemoveSpriteSet(spriteserrados[i]);
     }
     C2D2_RemoveSpriteSet(help);

	// Encerra a Chien2D 2
	C2D2_Encerra();
	return 0;
}
