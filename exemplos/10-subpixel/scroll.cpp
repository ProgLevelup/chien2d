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
#include <c2d2/chien2d2.h>

// Cuidado: o código só aceita largura de tela menor ou igual a 2040.
#define LARGTELA	800
#define ALTTELA		600

int main(int narg, char **valarg)
{
	// Inicia a Chien2D 2 e testa se deu tudo certo
	if(C2D2_Inicia(LARGTELA, ALTTELA, C2D2_JANELA, C2D2_DESENHO_PADRAO, "Subpixel Certo"))
		printf("Iniciou a Chien2D 2 com sucesso\n");
	else
	{
		printf("Falhou ao iniciar a Chien2D 2. Encerrando o programa.\n");
		return 0;
	}
	// Cria o cenário
	unsigned int fundo = C2D2_CarregaSpriteSet("scrollmaiscorreto.png", 512, 512);
	unsigned int fundoerrado = C2D2_CarregaSpriteSet("scrollerrado.png", 510, 510);
	unsigned int help = C2D2_CarregaSpriteSet("help.png", 0, 0);			
	// Desenha o cenário certo?
	bool certo = false;

	// Recupera o teclado
	C2D2_Botao *teclas = C2D2_PegaTeclas();
	// Recupera o mouse
	C2D2_Mouse *mouse = C2D2_PegaMouse();

	float x=0;
	int mapa[12]={1,3,0,2,0,1,2,3,1,3,0,2};
//	C2D2_TrocaCorLimpezaTela(164, 164, 255);
	do
	{
	    // Troca o render?
	    if(teclas[C2D2_ESPACO].pressionado)
	        certo = !certo;
		// Atualiza o x
		if(teclas[C2D2_ENTER].ativo)
			x+=5;
		else
			x+=0.01;
		// Verifica se deve rebobinar o mapa
		if(x> 8*510)
			x-= 8*510;
		// Limpa a tela com a cor de fundo atual
		C2D2_LimpaTela();
		// Desenha os sprites
		for(int i=0;i<LARGTELA/510+2;i++)
		    if(certo)
			    C2D2_DesenhaSpriteSubpixel(fundo, mapa[(int)x/510 + i], ((int)x/510)*510-x + 510*i, ALTTELA-510);
			else
			    C2D2_DesenhaSpriteF(fundoerrado, mapa[(int)x/510 + i], ((int)x/510)*510-x + 510*i, ALTTELA-510);
        C2D2_DesenhaSprite(help, 0, 10, 10);		
		// Faz a troca do front buffer com o back buffer e atualiza as entradas
		C2D2_Sincroniza(C2D2_FPS_PADRAO);
	}
	while(!teclas[C2D2_ENCERRA].pressionado && !teclas[C2D2_ESC].pressionado);
	// Remove as imagens
	C2D2_RemoveSpriteSet(fundo);
	C2D2_RemoveSpriteSet(fundoerrado);	
    C2D2_RemoveSpriteSet(help);

	// Encerra a Chien2D 2
	C2D2_Encerra();
	return 0;
}
