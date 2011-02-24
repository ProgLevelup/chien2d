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

#ifndef __INICIALIZAR_H__
#define __INICIALIZAR_H__

#include <c2d2/ator.h>
#include <c2d2/chien2d2.h>

	// Variáveis
	extern unsigned int torre;
	extern int vida_torre;
	extern int estado_torre;
	enum { TORRE_INTEIRA, TORRE_DESTRUIDA };

	extern unsigned int agua;
	extern unsigned int chama;
	extern unsigned int berrybush;
	extern unsigned int arvore;
	extern unsigned int torre_frente;
	extern unsigned int fonte;
	extern unsigned int fonte_grande;
	extern unsigned int musica_fundo;
	extern unsigned int tiro;
	extern unsigned int urro;

	extern C2D2_Mouse *mmouse;

	extern int placar;
	extern int atacantes;
	
	extern Animacao anima_cogumelo[];

	extern Animacao anima_esqueleto[];

	extern Animacao anima_fantasma[];

	extern Animacao anima_bonus[];

	extern Animacao anima_arqueiro[];

	extern Ator *inimigos[MAX_INIMIGOS];

	extern Ator *arqueiro;

	extern Ator *bonus;

	extern Ator *mira;

	extern C2D2_Botao *tteclado;


	// Funções
	void carregar();

	void inicializar();

	void finalizar();

	void descarregar();

#endif
