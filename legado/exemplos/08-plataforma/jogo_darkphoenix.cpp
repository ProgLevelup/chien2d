#include <c2d2/chien2d2.h>

#include "jogo_atores.h"
#include "jogo_darkphoenix.h"

//Animacao 
// Vetor com as animações da nave (2 no total)
Animacao animDark[] ={
	// Ordem: número de quadros, tempo entre os quadros, vetor com a seqüência de quadros
	// DARK_PARADO direita: 0
	{1, 1, {1}},
	// DARK_PARADO esquerda: 1
	{1, 1, {4}},
	// DARK_ANDANDO direita: 2
	{4, 10, {0, 1, 2, 1}},
	// DARK_ANDANDO esquerda: 3
	{4, 10, {3, 4, 3, 5}},
	// DARK_PULANDO direita: 4
	{1, 1, {2}},
	// DARK_PULANDO esquerda: 5
	{1, 1, {3}},
	// DARK_CAINDO direita: 6
	{2, 5, {0, 2}},
	// DARK_CAINDO esquerda: 7
	{2, 5, {3, 5}},
	// DARK_VITORIA: 8
	{2, 30, {9, 10}},
	// DARK_MORRENDO: 9
	{1, 1, {12}},
	// DARK_MORREU: 10
	{1, 1, {11}}
};

char *sons[]={
	"audio/pula.wav",
	"audio/bate.wav",
	"audio/morreu.wav"
};

// A função que carrega o DarkPhoenix.
//
bool JOGO_CarregaDarkPhoenix()
{
	return ATOR_CarregaAtorEstatico(DARKPHOENIX, "imagens/darkphoenix.png", 32, 42, 4, 2,
		24, 39, animDark, false, sons, 3, &AtualizaDarkPhoenix);
}

// A função para fazer a lógica do DarkPhoenix
//
// Data: 04/09/2008
// Última atualização: 04/09/2008
//
bool AtualizaDarkPhoenix(Ator *a, unsigned int mapa)
{
	Evento ev;
	switch(a->estado.estado)
	{
	case ATOR_NASCENDO:
		// Muda para o estado adequado
		ATOR_TrocaEstado(a, DARK_INICIO, false);
		a->vidas=3;
		break;
	case DARK_INICIO:
		// Indica para qual direção está olhando
		a->olhandoPara=0;
		// Guarda a posição inicial
		a->aux_real[0] = a->x;
		a->aux_real[1] = a->y;
		ATOR_TrocaEstado(a, DARK_PARADO, false);
		break;
	case DARK_PARADO:
		if(a->estado.subestado==ESTADO_INICIO)
		{
			a->velocidade=0;
			a->estado.subestado=ESTADO_RODANDO;
			if(a->direcao==0)
				ATOR_TrocaAnimacao(a, 0);
			else
				ATOR_TrocaAnimacao(a, 1);
		}

		while(ATOR_ProximoEvento(a, &ev))
		{
			switch(ev.tipoEvento)
			{
			case EVT_PRESSIONOU_BOTAO1:
				ATOR_TrocaEstado(a, DARK_PULANDO, false);
				break;
			case EVT_PRESSIONOU_ESQ:
				ATOR_TrocaEstado(a, DARK_ANDANDO,false);
				a->direcao=180;
				break;
			case EVT_PRESSIONOU_DIR:
				ATOR_TrocaEstado(a, DARK_ANDANDO,false);
				a->direcao=0;
				break;
			case EVT_COMECOU_CAIR:
				ATOR_TrocaEstado(a, DARK_CAINDO, false);
				break;
			case EVT_COLIDIU_ARMADILHA:
				ATOR_TrocaEstado(a, DARK_MORRENDO, false);
				break;
			case EVT_CHECKPOINT:
				a->aux_real[0] = a->x;
				a->aux_real[1] = a->y;
				break;
			case EVT_FIM_FASE:
				ATOR_TrocaEstado(a, DARK_VITORIA, false);
				break;
			case EVT_COLIDIU_PERSONAGEM:
				ATOR_TrocaEstado(a, DARK_MORRENDO, false);
				break;
			}
		}
		break;
	case DARK_PULANDO:
		if(a->estado.subestado==ESTADO_INICIO)
		{
			ATOR_Impulsiona(a, VPULO);
			ATOR_TocaEfeitoTela(a, 0, mapa);
			a->estado.subestado=ESTADO_RODANDO;
			if(a->direcao==0)
				ATOR_TrocaAnimacao(a, 4);
			else
				ATOR_TrocaAnimacao(a, 5);

		}
		while(ATOR_ProximoEvento(a, &ev))
		{
			switch(ev.tipoEvento)
			{
			case EVT_TOPO_PULO:
				ATOR_TrocaEstado(a, DARK_CAINDO, false);
				break;
			case EVT_COLIDIU_PAREDE:
				if(ev.subtipo == SUBEVT_COLIDIU_PAREDE_CIMA)
				{
					ATOR_TrocaEstado(a, DARK_CAINDO, false);
					ATOR_TocaEfeitoTela(a, 1, mapa);
				}
				break;
			case EVT_PRESSIONOU_ESQ:
				if(a->velocidade==0)
				{
					a->velocidade = VANDA;
					a->direcao = 180;
					ATOR_TrocaAnimacao(a, 5);
				}
				break;
			case EVT_PRESSIONOU_DIR:
				if(a->velocidade==0)
				{
					a->velocidade = VANDA;
					a->direcao = 0;
					ATOR_TrocaAnimacao(a, 4);
				}
				break;
			case EVT_LIBEROU_ESQ:
				if(a->velocidade!=0 && a->direcao == 180)
					a->velocidade = 0;
				break;
			case EVT_LIBEROU_DIR:
				if(a->velocidade!=0 && a->direcao == 0)
					a->velocidade = 0;
				break;
			case EVT_SAIU_FORA_MAPA:
				if(ev.subtipo != SUBEVT_SAIU_FORA_MAPA_CIMA)
					ATOR_TrocaEstado(a, DARK_MORRENDO, false);
				break;
			case EVT_COLIDIU_ARMADILHA:
				ATOR_TrocaEstado(a, DARK_MORRENDO, false);
				break;
			case EVT_CHECKPOINT:
				a->aux_real[0] = a->x;
				a->aux_real[1] = a->y;
				break;
			case EVT_FIM_FASE:
				ATOR_TrocaEstado(a, DARK_VITORIA, false);
				break;
			case EVT_COLIDIU_PERSONAGEM:
				ATOR_TrocaEstado(a, DARK_MORRENDO, false);
				break;
			}
		}
		break;
	case DARK_CAINDO:
		if(a->estado.subestado==ESTADO_INICIO)
		{
			a->estado.subestado=ESTADO_RODANDO;
			if(a->direcao==0)
				ATOR_TrocaAnimacao(a, 6);
			else
				ATOR_TrocaAnimacao(a, 7);

		}
		while(ATOR_ProximoEvento(a, &ev))
		{
			switch(ev.tipoEvento)
			{
			case EVT_SAIU_FORA_MAPA:
				if(ev.subtipo != SUBEVT_SAIU_FORA_MAPA_CIMA)
					ATOR_TrocaEstado(a, DARK_MORRENDO, false);
				break;
			case EVT_COLIDIU_PAREDE:
				if(ev.subtipo == SUBEVT_COLIDIU_PAREDE_BAIXO)
					if(a->velocidade == 0)
						ATOR_TrocaEstado(a, DARK_PARADO, false);
					else
						ATOR_TrocaEstado(a, DARK_ANDANDO, false);
				break;
			case EVT_PRESSIONOU_ESQ:
				if(a->velocidade==0)
				{
					a->velocidade = VANDA;
					a->direcao = 180;
					ATOR_TrocaAnimacao(a, 5);
				}
				break;
			case EVT_PRESSIONOU_DIR:
				if(a->velocidade==0)
				{
					a->velocidade = VANDA;
					a->direcao = 0;
					ATOR_TrocaAnimacao(a, 4);
				}
				break;
			case EVT_LIBEROU_ESQ:
				if(a->velocidade!=0 && a->direcao == 180)
					a->velocidade = 0;
				break;
			case EVT_LIBEROU_DIR:
				if(a->velocidade!=0 && a->direcao == 0)
					a->velocidade = 0;
				break;
			case EVT_COLIDIU_ARMADILHA:
				ATOR_TrocaEstado(a, DARK_MORRENDO, false);
				break;
			case EVT_CHECKPOINT:
				a->aux_real[0] = a->x;
				a->aux_real[1] = a->y;
				break;
			case EVT_FIM_FASE:
				ATOR_TrocaEstado(a, DARK_VITORIA, false);
				break;
			case EVT_COLIDIU_PERSONAGEM:
				ATOR_TrocaEstado(a, DARK_MORRENDO, false);
				break;

			}
		}

		break;

	case DARK_ANDANDO:
		if(a->estado.subestado==ESTADO_INICIO)
		{
			a->velocidade=VANDA;
			a->estado.subestado=ESTADO_RODANDO;
			if(a->direcao==0)
				ATOR_TrocaAnimacao(a, 2);
			else
				ATOR_TrocaAnimacao(a, 3);
		}
		while(ATOR_ProximoEvento(a, &ev))
		{
			switch(ev.tipoEvento)
			{
			case EVT_PRESSIONOU_BOTAO1:
 				ATOR_TrocaEstado(a, DARK_PULANDO, false);
				break;
			case EVT_LIBEROU_ESQ:
				if(a->direcao==180)
					ATOR_TrocaEstado(a, DARK_PARADO,false);
				break;
			case EVT_LIBEROU_DIR:
				if(a->direcao==0)
					ATOR_TrocaEstado(a, DARK_PARADO,false);
				break;
			case EVT_COLIDIU_PAREDE:
				if(ev.subtipo==SUBEVT_COLIDIU_PAREDE_CIMA)
					ATOR_TocaEfeitoTela(a, 1, mapa);
				break;
			case EVT_SAIU_FORA_MAPA:
				if(ev.subtipo != SUBEVT_SAIU_FORA_MAPA_CIMA)
					ATOR_TrocaEstado(a, DARK_MORRENDO, false);
				break;
			case EVT_COLIDIU_ARMADILHA:
				ATOR_TrocaEstado(a, DARK_MORRENDO, false);
				break;
			case EVT_CHECKPOINT:
				a->aux_real[0] = a->x;
				a->aux_real[1] = a->y;
				break;
			case EVT_FIM_FASE:
				ATOR_TrocaEstado(a, DARK_VITORIA, false);
				break;
			case EVT_COLIDIU_PERSONAGEM:
				ATOR_TrocaEstado(a, DARK_MORRENDO, false);
				break;

			}
		}
		break;
	case DARK_MORRENDO:
		if(a->estado.subestado == ESTADO_INICIO)
		{
			ATOR_Impulsiona(a, VPULO*1.5);
			ATOR_TrocaAnimacao(a, 9);
			ATOR_TocaEfeitoTela(a, 2, mapa);
			a->temporizadores[0] = 120;
			a->estado.subestado = ESTADO_RODANDO;
			a->velocidade=0;
			a->vidas--;
		}
		while(ATOR_ProximoEvento(a, &ev))
		{
			switch(ev.tipoEvento)
			{
			case EVT_COLIDIU_PAREDE:
				if(ev.subtipo == SUBEVT_COLIDIU_PAREDE_BAIXO)
					ATOR_TrocaEstado(a, DARK_MORREU, false);
				break;
			case EVT_TEMPO:
				if(ev.subtipo==0)
					ATOR_TrocaEstado(a, DARK_MORREU, false);
				break;
			}
		}
		break;
	case DARK_MORREU:
		if(a->estado.subestado == ESTADO_INICIO)
		{
			ATOR_TrocaAnimacao(a, 10);
			a->velocidade=0;
			a->temporizadores[0] = 60;
			a->estado.subestado = ESTADO_RODANDO;
		}
		while(ATOR_ProximoEvento(a, &ev))
		{
			switch(ev.tipoEvento)
			{
			case EVT_TEMPO:
				if(ev.subtipo==0)
				{

					if(a->vidas>0)
					{
						a->x = a->aux_real[0];
						a->y = a->aux_real[1];
						a->direcao=0;
						ATOR_TrocaEstado(a, DARK_PARADO, false);
					}
					else
						ATOR_TrocaEstado(a, ATOR_ENCERRADO, false);
				}
				break;
			}
		}
		break;
	case ATOR_ENCERRADO:
		if(a->estado.subestado == ESTADO_INICIO)
		{
			a->estado.subestado=ESTADO_RODANDO;
			a->velocidade=0;
			ev.tipoEvento = EVT_FIM_FASE;
			ev.subtipo = SUBEVT_FIM_FASE_DERROTA;
			ATOR_EnviaEventoJogo(&ev);

		}

		return false;
	case DARK_VITORIA:
		if(a->estado.subestado == ESTADO_INICIO)
		{
			a->estado.subestado=ESTADO_RODANDO;
			ATOR_TrocaAnimacao(a, 8);
			a->temporizadores[0]=240;
			a->velocidade=0;
		}
		while(ATOR_ProximoEvento(a, &ev))
		{
			if(ev.tipoEvento==EVT_TEMPO)
			{
				ev.tipoEvento = EVT_FIM_FASE;
				ev.subtipo = SUBEVT_FIM_FASE_VITORIA;
				ATOR_EnviaEventoJogo(&ev);
			}
		}
		break;
	}
	return true;
}
