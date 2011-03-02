#include <c2d2/chien2d2.h>

#include "jogo_atores.h"
#include "jogo_bola.h"

//Animacao 
// Vetor com as animações da nave (2 no total)
Animacao animBola[] ={
	// Ordem: número de quadros, tempo entre os quadros, vetor com a seqüência de quadros
	// BOLA_PULA ESQUERDA : 0
	{10, 5, {0, 1, 2, 3, 4, 5, 6, 7, 8, 9}},
	// BOLA_PULA direita : 1
	{10, 5, {9, 8, 7, 6, 5, 4, 3, 2, 1, 0}},
	// bola morreu: 2
	{2, 20, {0,5}}
};

// A função que carrega o DarkPhoenix.
//
bool JOGO_CarregaBola()
{
	return ATOR_CarregaAtorEstatico(BOLA, "imagens/inimigobola.png", 28, 28, 0, 0,
		28, 28, animBola, false, 0, 0, &AtualizaBola);
}

// A função para fazer a lógica do DarkPhoenix
//
// Data: 04/09/2008
// Última atualização: 04/09/2008
//
bool AtualizaBola(Ator *a, unsigned int mapa)
{
	Evento ev;
	switch(a->estado.estado)
	{
	case ATOR_NASCENDO:
		// Muda para o estado adequado
		ATOR_TrocaEstado(a, BOLA_PULANDO, false);
		break;
	case BOLA_PULANDO:
		if(a->estado.subestado==ESTADO_INICIO)
		{
			a->velocidade=2;
			a->estado.subestado=ESTADO_RODANDO;
			if(a->direcao==0)
				ATOR_TrocaAnimacao(a, 1);
			else
				ATOR_TrocaAnimacao(a, 0);
			a->aux_int[0]=3;
			ATOR_Impulsiona(a, 300);
		}

		while(ATOR_ProximoEvento(a, &ev))
		{
			switch(ev.tipoEvento)
			{
			case EVT_FOCO_TELA:
				printf("Personagem entrou na tela.\n");
				break;
			case EVT_PERDE_FOCO_TELA:
				printf("Personagem saiu da tela.\n");
				break;
			case EVT_SAIU_FORA_MAPA:
				ATOR_TrocaEstado(a, ATOR_ENCERRADO, false);
				break;
			case EVT_COLIDIU_PAREDE:
				if(ev.subtipo==SUBEVT_COLIDIU_PAREDE_BAIXO)
				{	
					if(--a->aux_int[0] == 0)
					{
						if(a->direcao==0)
							a->direcao=180;
						else
							a->direcao=0;
						ATOR_TrocaEstado(a, BOLA_PULANDO, false);
					}
					else
						ATOR_Impulsiona(a, 300);
					
				}
				else if(ev.subtipo!=SUBEVT_COLIDIU_PAREDE_CIMA)
					ATOR_TrocaEstado(a, BOLA_PULANDO, false);
				break;
			case EVT_COLIDIU_PERSONAGEM:
				ATOR_TrocaEstado(a, BOLA_MORRENDO, false);
				break;
			}
		}
		break;
	case BOLA_MORRENDO:
		if(a->estado.subestado == ESTADO_INICIO)
		{
			ATOR_TrocaAnimacao(a, 2);
			a->velocidade=0;
			a->temporizadores[0] = 60;
			a->estado.subestado = ESTADO_RODANDO;
		}
		while(ATOR_ProximoEvento(a, &ev))
		{
			switch(ev.tipoEvento)
			{
				break;
			case EVT_TEMPO:
				if(ev.subtipo==0)
					ATOR_TrocaEstado(a, ATOR_ENCERRADO, false);
				break;
			}
		}
		break;
	case ATOR_ENCERRADO:
		return false;
	}
	return true;
}
