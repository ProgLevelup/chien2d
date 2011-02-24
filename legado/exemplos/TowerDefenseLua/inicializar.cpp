/*
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
#include <c2d2/atorlua.h>
#include <c2d2/chien2d2.h>
#include <c2d2/chien2d2lua.h>
#include <c2d2/chienaudio2.h>

#include "constantes.h"
#include "inicializar.h"
#include "atualizar.h"

// Variáveis para recursos carregados
unsigned int torre;
unsigned int torre_frente;
unsigned int chama;
unsigned int agua;
unsigned int berrybush;
unsigned int arvore;
unsigned int fonte;
unsigned int fonte_grande;
unsigned int musica_fundo;
unsigned int tiro;
unsigned int urro;

// Mouse e teclado
C2D2_Mouse *mmouse;
C2D2_Botao *tteclado;

// Auxiliares para a lógica do jogo
int placar = 0;
int atacantes = 0;
int vida_torre = 10000;

int estado_torre = TORRE_INTEIRA;

// Atores
Ator *inimigos[MAX_INIMIGOS];
Ator *bonus;
Ator *mira;
Ator *arqueiro;

// Animações
Animacao anima_cogumelo[] ={
		// Ordem: número de quadros, tempo entre os quadros, vetor com a seqüência de quadros
		// Atacando
		{11, 5, {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10}},
		//Desaparecendo
		{9, 5, {11, 12, 13, 14, 15, 16, 17, 17, 17}},
		//Andando
		{8, 5, {18, 19, 20, 21, 22, 23, 24, 25}}

	};

	Animacao anima_esqueleto[] ={
		// Ordem: número de quadros, tempo entre os quadros, vetor com a seqüência de quadros
		// Atacando
		{13, 7, {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 10, 10}},
		//Desaparecendo
		{9, 7, {11, 12, 13, 14, 15, 16, 17, 18, 19}},
		//Andando
		{8, 7, {20, 21, 22, 23, 24, 25, 26, 27}}

	};

	Animacao anima_fantasma[] ={
		// Ordem: número de quadros, tempo entre os quadros, vetor com a seqüência de quadros
		// Atacando
		{13, 6, {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12}},
		//Desaparecendo
		{9, 6, {13, 14, 15, 16, 17, 18, 19, 20, 21}},
		//Andando
		{8, 6, {22, 23, 24, 25, 26, 27, 28, 29}}

	};

	Animacao anima_bonus[] ={
		// Ordem: número de quadros, tempo entre os quadros, vetor com a seqüência de quadros
		{1, 1, {0}}
	};

	Animacao anima_arqueiro[] ={
		// Ordem: número de quadros, tempo entre os quadros, vetor com a seqüência de quadros
		{12, 5, {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12}}
	};

void carregar() {

	bool result;
	

#ifdef __USES_LUA__
		result = ATOR_CarregaAtorEstaticoLua("cogumelo.lua", &atualizaInimigos);

	if (result)
		result = ATOR_CarregaAtorEstaticoLua("esqueleto.lua", &atualizaInimigos);

	if (result)
		result = ATOR_CarregaAtorEstaticoLua("fantasma.lua", &atualizaInimigos);

	if (result)
		result = ATOR_CarregaAtorEstaticoLua("mira.lua", &atualizaMira);

	if (result)
		result = ATOR_CarregaAtorEstaticoLua("bonus.lua", &atualizaBonus);

	if (result)
		result = ATOR_CarregaAtorEstaticoLua("zumbi.lua", &atualizaInimigos);

	if (result)
		result = ATOR_CarregaAtorEstaticoLua("arqueiro.lua", &atualizaArqueiro);

	// Carega torre
	torre = C2D2_CarregaSpriteSetLua("torre.lua");

	// Carrega chama
	chama = C2D2_CarregaSpriteSetLua("chama.lua");

	// Carrega chama
	berrybush = C2D2_CarregaSpriteSetLua("berrybush.lua");

	// Carrega árvore
	arvore = C2D2_CarregaSpriteSetLua("arvore.lua");

	// Carrega água
	agua = C2D2_CarregaSpriteSetLua("agua.lua");
#else
	result = ATOR_CarregaAtorEstatico(INIMIGO_COGUMELO, "dados/cogumelo.png", 96, 96, 0, 0, 96, 96, anima_cogumelo, true,
			0, 0, &atualizaInimigos);

	if (result)
		result = ATOR_CarregaAtorEstatico(INIMIGO_ESQUELETO, "dados/esqueleto.png", 96, 96, 0, 0, 96, 96, anima_esqueleto, true,
			0, 0, &atualizaInimigos);

	if (result)
		result = ATOR_CarregaAtorEstatico(INIMIGO_FANTASMA, "dados/fantasma.png", 96, 96, 0, 0, 96, 96, anima_fantasma, true,
			0, 0, &atualizaInimigos);

	if (result)
		result = ATOR_CarregaAtorEstatico(MIRA, "dados/mira.png", 21, 21, 10, 10, 1, 1, anima_bonus, true,
				0, 0, &atualizaMira);

	if (result)
		result = ATOR_CarregaAtorEstatico(BONUS_10, "dados/bonus_100b.png", 55, 26, 0, 0, 55, 26, anima_bonus, true,
				0, 0, &atualizaBonus);

	if (result)
		result = ATOR_CarregaAtorEstatico(INIMIGO_ZUMBI, "dados/zumbi.png", 96, 96, 0, 0, 96, 96, anima_esqueleto, true,
			0, 0, &atualizaInimigos);

	if (result)
		result = ATOR_CarregaAtorEstatico(ARQUEIRO, "dados/arqueiro.png", 96, 96, 0, 0, 96, 96, anima_arqueiro, true,
				0, 0, &atualizaArqueiro);

	// Carega torre
	torre = C2D2_CarregaSpriteSet("dados/torre.png", 0, 0);

	// Carrega chama
	chama = C2D2_CarregaSpriteSet("dados/chama.png", 48, 64);

	// Carrega água
	agua = C2D2_CarregaSpriteSet("dados/agua.png", 64, 64);

	// Carrega chama
	berrybush = C2D2_CarregaSpriteSet("dados/berrybush.png", 0, 0);

	// Carrega árvore
	arvore = C2D2_CarregaSpriteSet("dados/fir.png", 0, 0);
#endif

	// Carrega frente da torre
	torre_frente = C2D2_CarregaSpriteSet("dados/torre_frente.png", 0, 0);

	// Carrega fonte
	fonte = C2D2_CarregaFonte("dados/fonts/Kristen32.bmp", 32);

	// Carrega fonte
	fonte_grande = C2D2_CarregaFonte("dados/fonts/Kristen64.bmp", 64);

	// Carrega música
	musica_fundo = CA2_CarregaMusica("dados/Foxtacy_Dark_Crystal.ogg");

	// Carrega som de tiro
	tiro = CA2_CarregaEfeito("dados/tiro.wav");

	// Carrega som de urro
	urro = CA2_CarregaEfeito("dados/urro.wav");
}

void inicializar() {
	// Cria atores
	inimigos[INIMIGO_COGUMELO] = ATOR_CriaAtor(INIMIGO_COGUMELO, 50, 440, 0);
	inimigos[INIMIGO_ESQUELETO] = ATOR_CriaAtor(INIMIGO_ESQUELETO, 60, 460, 0);
	inimigos[INIMIGO_FANTASMA] = ATOR_CriaAtor(INIMIGO_FANTASMA, 70, 480, 0);
	inimigos[INIMIGO_ZUMBI] = ATOR_CriaAtor(INIMIGO_ZUMBI, 80, 500, 0);

	arqueiro = ATOR_CriaAtor(ARQUEIRO, 710, 350, 0);

	bonus = ATOR_CriaAtor(BONUS_10, 710, 400, 0);

	mira = ATOR_CriaAtor(MIRA, 0, 0, 0);

	tteclado = C2D2_PegaTeclas();
	mmouse = C2D2_PegaMouse();

	C2D2_TrocaCorLimpezaTela(106, 76, 48);

	CA2_TocaMusica(musica_fundo, 0);
	CA2_TocaEfeito(urro, 0);

}

void finalizar() {
	for (int i = 0; i < MAX_INIMIGOS; i++)
	{
		free(inimigos[i]);
	}

	free(bonus);

	free(mira);
}

void descarregar() {
	// descarrega os dados
	ATOR_DescarregaAtor(INIMIGO_COGUMELO);
	ATOR_DescarregaAtor(INIMIGO_ESQUELETO);
	ATOR_DescarregaAtor(INIMIGO_ZUMBI);
	ATOR_DescarregaAtor(INIMIGO_FANTASMA);
	ATOR_DescarregaAtor(MIRA);
	ATOR_DescarregaAtor(BONUS_10);
	ATOR_DescarregaAtor(ARQUEIRO);

#ifdef __USES_LUA__
	C2D2_RemoveSpriteSetLua(torre);
	C2D2_RemoveSpriteSetLua(berrybush);
	C2D2_RemoveSpriteSetLua(arvore);
	C2D2_RemoveSpriteSetLua(chama);
	C2D2_RemoveSpriteSetLua(agua);
#else
	C2D2_RemoveSpriteSet(torre);
	C2D2_RemoveSpriteSet(berrybush);
	C2D2_RemoveSpriteSet(arvore);
	C2D2_RemoveSpriteSet(chama);
	C2D2_RemoveSpriteSet(agua);
#endif

	C2D2_RemoveSpriteSet(torre_frente);

	C2D2_RemoveFonte(fonte);
	C2D2_RemoveFonte(fonte_grande);
	CA2_RemoveEfeito(tiro);
	CA2_RemoveEfeito(urro);
	CA2_RemoveMusica(musica_fundo);

}
