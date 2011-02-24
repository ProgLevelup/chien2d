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

#include "constantes.h"
#include "atualizar.h"
#include "inicializar.h"

#include <stdlib.h>

// Funções
bool atualizaInimigos(Ator *a, unsigned int mapa)
{
	switch(a->estado.estado)
	{
	case ATOR_NASCENDO:
		// Muda para o estado adequado
		ATOR_TrocaEstado(a, ESTADO_INIMIGO_INATIVO, false);
		break;
	case ESTADO_INIMIGO_INATIVO:
		if(a->estado.subestado==ESTADO_INICIO)
		{
			// Animação de andando
			ATOR_TrocaAnimacao(a, ANIMA_INIMIGO_ANDANDO);

			// Muda para o estado adequado
			ATOR_TrocaEstado(a, ESTADO_INIMIGO_ANDANDO, false);
		}
		break;
	case ESTADO_INIMIGO_ANDANDO:
		if(a->estado.subestado==ESTADO_INICIO)
		{
			a->velocidade = 1.0/(rand() % (MAX_INIMIGOS*2) + 1.0) + 1.0 + a->aux;
			a->direcao = 0;
			a->estado.subestado= ESTADO_RODANDO;
		}
		else
		{
			if (a->x >= 635)
			{
				// Animação de andando
				ATOR_TrocaAnimacao(a, ANIMA_INIMIGO_BATENDO);

				// Muda para o estado adequado
				ATOR_TrocaEstado(a, ESTADO_INIMIGO_BATENDO, false);

				a->velocidade = 0;
			}
		}
		break;
	case ESTADO_INIMIGO_BATENDO:
			if(a->estado.subestado==ESTADO_INICIO)
			{
				a->estado.subestado= ESTADO_RODANDO;
				atacantes++;
			}
		break;
	case ESTADO_INIMIGO_MORRENDO:
			if(a->estado.subestado==ESTADO_INICIO)
			{
				// Animação de morrendo
				ATOR_TrocaAnimacao(a, ANIMA_INIMIGO_MORRENDO);

				a->velocidade = 0;
				a->estado.subestado= ESTADO_RODANDO;

				if (atacantes > 0)
					atacantes--;
			}
			if (a->quadroAtual == 8) {
				a->x = 0;

				a->aux+=0.2;

				// Animação de andando
				ATOR_TrocaAnimacao(a, ANIMA_INIMIGO_ANDANDO);

				// Muda para o estado adequado
				ATOR_TrocaEstado(a, ESTADO_INIMIGO_ANDANDO, false);
			}

		break;

	}
	return true;
}

bool atualizaBonus(Ator *a, unsigned int mapa)
{
	switch(a->estado.estado)
	{
	case ATOR_NASCENDO:
		// Muda para o estado adequado
		ATOR_TrocaEstado(a, ESTADO_BONUS_INICIO, false);
		break;
	case ESTADO_BONUS_INICIO:
		if(a->estado.subestado==ESTADO_INICIO)
		{
			a->velocidade = 2;
			a->direcao = 90;
			a->estado.subestado=ESTADO_RODANDO;
			vida_torre -= QTDE_DANO*atacantes;
		}
		else
		{
			if (a->y <= 250) {
				a->y = 400;
				vida_torre -= QTDE_DANO*atacantes;
			}
		}
		break;
	}
	return true;
}

bool atualizaMira(Ator *a, unsigned int mapa)
{
	switch(a->estado.estado)
	{
	case ATOR_NASCENDO:
		// Muda para o estado adequado
		ATOR_TrocaEstado(a, ESTADO_MIRA_INICIO, false);
		break;
	case ESTADO_MIRA_INICIO:
		if(a->estado.subestado==ESTADO_INICIO)
		{
			a->estado.subestado=ESTADO_RODANDO;
		}
		else {
			a->x = mmouse->x;
			a->y = mmouse->y;
			a->olhandoPara++;
		}
	}
	return true;
}

bool atualizaArqueiro(Ator *a, unsigned int mapa)
{
	switch(a->estado.estado)
	{
	case ATOR_NASCENDO:
		// Muda para o estado adequado
		ATOR_TrocaEstado(a, ESTADO_ARQUEIRO_INICIO, false);
		break;
	case ESTADO_ARQUEIRO_INICIO:
		if(a->estado.subestado==ESTADO_INICIO)
		{
			ATOR_TrocaAnimacao(a, ANIMA_ARQUEIRO_ATIRANDO);
			a->estado.subestado=ESTADO_RODANDO;
		}
	}
	return true;
}
