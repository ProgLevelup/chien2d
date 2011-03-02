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
#include <c2d2/chien2d2.h>
#include <c2d2/chien2d2primitivas.h>
#include <c2d2/chien2d2mapa.h>

#define ALTURA	600
#define LARGURA	800

// Defines para o tamanho da janela do mapa
#define	MAPA_X	100
#define	MAPA_Y	100
#define JANELA_MAPA_X	600
#define JANELA_MAPA_Y	400

bool cima=false, baixo=false, esquerda=false, direita=false, sai=false, pula=false;


int main(int narg, char **valarg)
{
	// Vetor da visibilidade das camadas
	bool camadas[4]={true, true, true, true};
	// Inicia a Chien2D 2 e testa se deu tudo certo
	if(C2D2_Inicia(LARGURA, ALTURA, C2D2_JANELA, C2D2_DESENHO_OPENGL, "Imagem Carregada"))
		printf("Iniciou a Chien2D 2 com sucesso\n");
	else
	{
		printf("Falhou ao iniciar a Chien2D 2. Encerrando o programa.\n");
		return 0;
	}
	// Inicia os mapas
	C2D2M_Inicia();
	// Inicia as primitivas para limpar a área suja
	C2D2P_Inicia();
	// Carrega a imagem
	unsigned int mapa = C2D2M_CarregaMapaMappy("tilemap.fmp", "tileset.png");
	unsigned int idSprite = C2D2_CarregaSpriteSet("darkphoenix.png", 32, 42);
	unsigned int arial32 = C2D2_CarregaFonte("arial32.png", 32);
	// Testa se carregou certo (se é diferente de 0)
	if(mapa == 0 || idSprite==0 || arial32==0)
	{
		printf("Falhou ao carregar alguma coisa. Encerrando.\n");
		// Encerra a Chien2d2
		C2D2_Encerra();
		return 0;
	}
	// Define a velocidade das camadas do mapa
	C2D2M_VelocidadeCamadaMapa(mapa, 0, 3);
	C2D2M_VelocidadeCamadaMapa(mapa, 1, 2);
	C2D2M_VelocidadeCamadaMapa(mapa, 2, 1);
	C2D2M_VelocidadeCamadaMapa(mapa, 3, 1);
	// As coordenadas do mapa
	int xmapa=0,ymapa=0;
	// As coordenadas do sprite do jogador
	int x=0,y=0;
	// Recupera o teclado
	C2D2_Botao *teclas = C2D2_PegaTeclas();
	// Recupera o mouse
	C2D2_Mouse *mouse = C2D2_PegaMouse();
	// Recupera as dimensões do mapa
	int larguraMapa, alturaMapa;
	C2D2M_DimensoesMapa(mapa, &larguraMapa, &alturaMapa);
	// Coloca a cor preta como cor de limpeza de tela
	C2D2_TrocaCorLimpezaTela(64,255,255);
	// Roda!
	while(!teclas[C2D2_ENCERRA].ativo && !teclas[C2D2_ESC].ativo)
	{
		// Atualiza a lógica
		if(teclas[C2D2_ESQUERDA].ativo)
			xmapa-=4;
		else if(teclas[C2D2_DIREITA].ativo)
			xmapa+=4;
		if(teclas[C2D2_CIMA].ativo)
			ymapa-=4;
		else if(teclas[C2D2_BAIXO].ativo)
			ymapa+=4;
		// Valida as coordenadas do mapa
		if(xmapa < 0)
		{
			xmapa=0;
		}
		else if(xmapa > larguraMapa-JANELA_MAPA_X)
		{
			xmapa = larguraMapa-JANELA_MAPA_X;
		}
		if(ymapa < 0)
		{
			ymapa=0;
		}
		else if(ymapa > alturaMapa-JANELA_MAPA_Y)
		{
			ymapa = alturaMapa-JANELA_MAPA_Y;
		}
		// Atualiza o estado das camadas
		if(teclas[C2D2_F1].pressionado)
			camadas[0]=!camadas[0];
		if(teclas[C2D2_F2].pressionado)
			camadas[1]=!camadas[1];
		if(teclas[C2D2_F3].pressionado)
			camadas[2]=!camadas[2];
		if(teclas[C2D2_F4].pressionado)
			camadas[3]=!camadas[3];

		// Atualiza o mouse
		x=mouse->x;
		y=mouse->y;
	
		// Limpa a tela
		C2D2_LimpaTela();
		// Posiciona o mapa
		C2D2M_PosicionaMapa(mapa, xmapa, ymapa);
		// Desenha a imagem do sprite 0 (único) na posição 0,0
		if(camadas[0])
			C2D2M_DesenhaCamadaMapa(mapa,0,MAPA_X,MAPA_Y,JANELA_MAPA_X,JANELA_MAPA_Y);
		if(camadas[1])
			C2D2M_DesenhaCamadaMapa(mapa,1,MAPA_X,MAPA_Y,JANELA_MAPA_X,JANELA_MAPA_Y);
		if(camadas[2])
			C2D2M_DesenhaCamadaMapa(mapa,2,MAPA_X,MAPA_Y,JANELA_MAPA_X,JANELA_MAPA_Y);
		C2D2_DesenhaSprite(idSprite, 0, x, y);
		if(camadas[3])
			C2D2M_DesenhaCamadaMapa(mapa,3,MAPA_X,MAPA_Y,JANELA_MAPA_X,JANELA_MAPA_Y);
		C2D2_DesenhaTexto(arial32, 320, 200, "Hello Chien2D 2!", C2D2_TEXTO_CENTRALIZADO);
		// Limpa as bordas. Isso é REALMENTE necessário. Logo, se tiver algo importante a direita ou a esquerda, desenhe o mapa ANTES senão fica sujeira mesmo!
		C2D2P_RetanguloPintado(0,0,LARGURA,MAPA_Y, 0, 0, 0);
		C2D2P_RetanguloPintado(0,MAPA_Y+JANELA_MAPA_Y,LARGURA,ALTURA, 0, 0, 0);
		C2D2P_RetanguloPintado(0,MAPA_Y,MAPA_X,MAPA_Y+JANELA_MAPA_Y, 0, 0, 0);
		C2D2P_RetanguloPintado(MAPA_X+JANELA_MAPA_X,MAPA_Y,LARGURA,MAPA_Y+JANELA_MAPA_Y, 0, 0, 0);		
		// Troca os buffers de imagem e esperar o tempo para 60fps
		C2D2_Sincroniza(C2D2_FPS_PADRAO);
		// Anima o mapa
		C2D2M_AnimaMapa(mapa);
	}

	// Apaga a imagem carregada da memória
	C2D2M_RemoveMapa(mapa);
	C2D2_RemoveSpriteSet(idSprite);
	// encerra a biblioteca de mapas
	C2D2M_Encerra();
	// Encerra a Chien2D 2
	C2D2_Encerra();

	return 0;
}
