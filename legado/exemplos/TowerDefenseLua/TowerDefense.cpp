/*

Tower Defense

Copyright 2009-2010, Fabio Vinicius Binder (fbinder@gmail.com)

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

#include <c2d2/ator.h>
#include <c2d2/chien2d2.h>
#include <c2d2/chien2d2lua.h>
#include <c2d2/chien2d2primitivas.h>
#include <c2d2/chienaudio2.h>
#include <c2d2/chienaudio2lua.h>

#include "constantes.h"
#include "inicializar.h"

#include <time.h>
#include <string.h>

void inicia_bibliotecas();
void finaliza_bibliotecas();

void desenha_fim();
void desenha_cenario_fundo();
void desenha_cenario_frente();
void desenha_inimigos(int fase);
void desenha_torre();
void desenha_barra_torre(int atual, int total);
void desenha_bonus();
void desenha_mira();
void desenha_arqueiro();
void desenha_chama();

void atira();

unsigned int ichama;
unsigned int iagua;
int fase;
int x_torre = 660, 
    y_torre = 300;

int main(int ac, char **av)
{
	// Inicia semente de randomização
	srand((unsigned int)time(NULL));

	// Inicia bibliotecas
	inicia_bibliotecas();

	// Carrega recursos
	carregar();

	// Inicializa recursos
	inicializar();

	ichama = 0;
	iagua = 0;
	fase = 4;

	while(!tteclado[C2D2_ESC].pressionado && !tteclado[C2D2_ENCERRA].pressionado)
	{
		desenha_cenario_fundo();

		desenha_inimigos(fase);

		desenha_torre();

		//if (estado_torre != TORRE_DESTRUIDA)
		//	desenha_arqueiro();

		desenha_cenario_frente();

		desenha_barra_torre(vida_torre, 10000);

		desenha_bonus();

		desenha_mira();

		atira();

		if (vida_torre <= 0)
			break;

		C2D2_Sincroniza(C2D2_FPS_PADRAO);
	}

	desenha_fim();

	finalizar();

	descarregar();

	finaliza_bibliotecas();

	return 0;
}

void desenha_fim() {
	C2D2_TrocaCorLimpezaTela(170, 87, 85);
	C2D2_LimpaTela();
	//C2D2P_RetanguloPintado(150, 240, 700, 360, 0, 0, 0);
	//C2D2P_Retangulo(155, 245, 695, 355, 255, 255, 255);
	C2D2_DesenhaTexto(fonte_grande, 235, 255, "YOU DIED!!!", C2D2_TEXTO_ESQUERDA);
	//C2D2_DesenhaTexto(fonte, 160, 250, "All your base are belong to us.", C2D2_TEXTO_ESQUERDA);
	//C2D2_DesenhaTexto(fonte, 440, 300, "CATS (Zero Wing)", C2D2_TEXTO_ESQUERDA);

	C2D2_Sincroniza(C2D2_FPS_PADRAO);

	C2D2_Pausa(5000);
}

void desenha_cenario_fundo() {
	C2D2_LimpaTela();
	C2D2P_RetanguloPintado(0, 0, 800, 360, 63, 199, 255);

	C2D2P_CirculoPintado(100, 160, 50, 255, 255, 255);
	C2D2P_CirculoPintado(120, 160, 47, 63, 199, 255);

	C2D2_DesenhaTexto(fonte, 580, 10, "Tower Defense!", C2D2_TEXTO_ESQUERDA);
	
	C2D2_DesenhaSprite(berrybush, 0, 100, 330);
	C2D2_DesenhaSprite(berrybush, 0, 500, 360);
	C2D2_DesenhaSprite(berrybush, 0, 250, 380);
}

void desenha_cenario_frente() {
	if (estado_torre != TORRE_DESTRUIDA)
		C2D2_DesenhaSprite(torre_frente, 0, x_torre + 4, y_torre + 56);

	desenha_chama();

	C2D2P_RetanguloPintado(0, 536, 800, 600, 106, 76, 48);

	C2D2_DesenhaSprite(arvore, 0, 250, 390);

	//for (int i = 0; i <= 12; i++)
	//	C2D2_DesenhaSprite(agua, iagua / 10 , i * 64, 570);
	//iagua = (iagua >= 70 )? 0 : iagua + 1;

	char pontos[30];
	sprintf(pontos, "Pontos: %d", placar);
	C2D2_DesenhaTexto(fonte, 20, 10, pontos, C2D2_TEXTO_ESQUERDA);
}

void desenha_barra_torre(int atual, int total) {
	// Contorno
	C2D2P_Retangulo(640, 540, 780, 555, 0, 0, 0);

	// Conteúdo
	int x = 641 + 138*atual/total;
	if (x < 641) x = 641;
	C2D2P_RetanguloPintado(641, 541, x, 554, 255, 0, 0);

	//char debug[30];
	//sprintf(debug, "Torre: %d", vida_torre);
	//C2D2_DesenhaTexto(fonte, 620, 560, debug, C2D2_TEXTO_ESQUERDA);
}


void atira() {
	if (mmouse->botoes[C2D2_MESQUERDO].pressionado) {
			CA2_TocaEfeito(tiro, 0);
			placar--;
			C2D2P_CirculoPintado(mmouse->x, mmouse->y, 10, 255, 0, 0);
			for (int i = MAX_INIMIGOS-1; i >=0; i--)
			{
				if (ATOR_ColidiuAtores(mira, inimigos[i]) && inimigos[i]->estado.estado != ESTADO_INIMIGO_MORRENDO) {
					placar += 5;

					ATOR_TrocaEstado(inimigos[i], ESTADO_INIMIGO_MORRENDO, false);
					CA2_TocaEfeito(urro, 0);
					break;
				}
			}
		}		
}

void desenha_inimigos(int fase) {
	if (fase >= MAX_INIMIGOS)
		fase = MAX_INIMIGOS-1;
	for (int i = 0; i <= fase ; i++)
	{
		ATOR_AplicaEstado(inimigos[i], 0, 800, 600);
		ATOR_Atualiza(inimigos[i], 0);

		ATOR_Desenha(inimigos[i], 0, 0, 0);
	}
}

void desenha_arqueiro() {
	ATOR_AplicaEstado(arqueiro, 0, 800, 600);
	ATOR_Atualiza(arqueiro, 0);
	ATOR_Desenha(arqueiro, 0, 0, 0);
}

void desenha_torre() {
	static unsigned int frame_torre = 0;

#ifdef __USES_LUA__
	C2D2_DesenhaSpriteLua(torre, &frame_torre, &x_torre, &y_torre, &estado_torre);		
#else
	C2D2_DesenhaSpriteLua(torre, 0, 660, 300);		
#endif
	if (vida_torre < 1000)
		estado_torre = TORRE_DESTRUIDA;

	
}

void desenha_chama() {
	static int	x1 = 689,
				y1 = 472,
				x2 = 659,
				y2 = 392,
				x3 = 719,
				y3 = 372;

	if (vida_torre < 7000) {
		ichama++;
		if (ichama > 30) ichama = 20;
#ifdef __USES_LUA__
		C2D2_DesenhaSpriteLua(chama, &ichama, &x1, &y1, &estado_torre);
#else
		C2D2_DesenhaSprite(chama, ichama, x, y);
#endif
		if (vida_torre < 5000)
#ifdef __USES_LUA__
			C2D2_DesenhaSpriteLua(chama, &ichama, &x2, &y2, &estado_torre);
#else
			C2D2_DesenhaSprite(chama, ichama, x, y);
#endif
		if (vida_torre < 2500)
#ifdef __USES_LUA__
			C2D2_DesenhaSpriteLua(chama, &ichama, &x3, &y3, &estado_torre);
#else
			C2D2_DesenhaSprite(chama, ichama, x, y);
#endif
	}
}

void desenha_bonus() {
	if (atacantes > 0)
	{
		ATOR_AplicaEstado(bonus, 0, 800, 600);
		ATOR_Atualiza(bonus, 0);
		ATOR_Desenha(bonus, 0, 0, 0);
	}
}

void desenha_mira() {
	ATOR_AplicaEstado(mira, 0, 800, 600);
	ATOR_Atualiza(mira, 0);
	ATOR_Desenha(mira, 0, 0, 0);
}

void inicia_bibliotecas() {

	// Inicia bibliotecas
#ifdef __USES_LUA__
	CL2_Inicia();
	if(!C2D2_Inicia()) {
#else
	if(!C2D2_Inicia(800, 600, C2D2_JANELA, C2D2_DESENHO_OPENGL, "Projeto 1")) {
#endif
	
		printf("Nao conseguiu iniciar a tela. Encerrando.\n");
		exit(0);
	}	

#ifdef __USES_LUA__
	CA2_IniciaLua();
#else
	CA2_Inicia();
#endif

	ATOR_Inicia();
	C2D2P_Inicia();

}

void finaliza_bibliotecas() {
	CA2_Encerra();
	C2D2_Encerra();
	ATOR_Encerra();

#ifdef __USES_LUA__
	CL2_Encerra();
#endif
}