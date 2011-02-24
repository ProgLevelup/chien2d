#include <c2d2/chien2d2.h>

#include "jogo_atores.h"
#include "jogo_atriz.h"

//Animacao 
// Vetor com as animações da nave (2 no total)
Animacao animAtriz[] ={
	// Ordem: número de quadros, tempo entre os quadros, vetor com a seqüência de quadros
	// ATRIZ_INVULNERAVEL: 0
	{2, 1, {12, 13}},
	// ATRIZ_PARADA
	//para baixo: 1
	{1, 1, {0}},
	//para cima: 2
	{1, 1, {3}},
	//para esquerda: 3
	{1, 1, {6}},
	//para direita: 4
	{1, 1, {9}},
	// ATRIZ_ANDANDO
	// para baixo: 5
	{4, 10, {0, 1, 0, 2}},
	// para cima: 6
	{4, 10, {3, 4, 3, 5}},
	// para esquerda: 7
	{4, 10, {6, 7, 6, 8}},
	// para direita: 8
	{4, 10, {9, 10, 9, 11}},
	//ATRIZ_ATIRANDO
	// para baixo: 9
	{4, 5, {19, 23, 19, 23}}, 
	// para cima: 10
	{4, 5, {18, 22, 18, 22}}, 
	// para esquerda: 11
	{4, 5, {16, 20, 16, 20}}, 
	// para direita: 12
	{4, 5, {17, 21, 17, 21}}, 
	// ATRIZ_VENCEU: 13
	{4, 15, {0, 6, 3, 9}}, 
	//ATRIZ_MORRENDO
	{1, 1, {15}}
};


// A função que carrega a atriz (pra facilitar). DEscarregar não precisa assim por usar só
// o identificador, que o jogo tem que conhecer de qualquer maneira
//
// 0,5 ponto na nota pro primeiro aluno que explicar em detalhes a vantagem de ter 
// essa função AQUI, ao invés de carregar explicitamente na main.
//
bool JOGO_CarregaAtriz()
{
	return ATOR_CarregaAtorEstatico(JOGO_ATRIZ, "atriz.png", 32, 64, 10, 51, 14, 11, 
		animAtriz, false, 0, 0, &AtualizaAtriz);
}

// A função para fazer a lógica da atriz
//
// Data: 10/08/2007
// Última atualização: 21/08/2007
//
bool AtualizaAtriz(Ator *a, unsigned int mapa)
{
	Evento ev;
	switch(a->estado.estado)
	{
	case ATOR_NASCENDO:
		// Muda para o estado adequado
		ATOR_TrocaEstado(a, ATRIZ_INVULNERAVEL, false);
		// Indica para qual direção está olhando
		a->olhandoPara=270;
		break;
	case ATRIZ_INVULNERAVEL:
		// Se inicia, coloca o personagem invulnerável e escolhe a animação 0
		if(a->estado.subestado==ESTADO_INICIO)
		{
			ATOR_TrocaAnimacao(a, 0);
			a->invulneravel=true;
			a->estado.subestado=ESTADO_RODANDO;
			// Coloca o temporizador para 2 segundos
			a->temporizadores[0]=120;
		}
		while(ATOR_ProximoEvento(a, &ev))
		{
			switch(ev.tipoEvento)
			{
			case EVT_TEMPO:
				if(ev.subtipo==0)
					ATOR_TrocaEstado(a, ATRIZ_PARADA, false);
			}
		}
		break;
	case ATRIZ_PARADA:
		// quando começa, coloca a animação de acordo com 
		// a direção e zera a velocidade
		if(a->estado.subestado==ESTADO_INICIO)
		{
			// Escolhe a animação entre os quatro angulos possíveis
			switch(a->olhandoPara)
			{
			case 270:
				ATOR_TrocaAnimacao(a, 1);
				break;
			case 90:
				ATOR_TrocaAnimacao(a, 2);
				break;
			case 180:
				ATOR_TrocaAnimacao(a, 3);
				break;
			case 0:
			default:
				ATOR_TrocaAnimacao(a, 4);
				break;
			}
			// zera a velocidade e muda o sub-estado
			a->velocidade=0;
			a->estado.subestado=ESTADO_RODANDO;
		}
		// Pega os eventos
		while(ATOR_ProximoEvento(a, &ev))
		{
			switch(ev.tipoEvento)
			{
			case EVT_PRESSIONOU_BAIXO:
				// Dá uma direção ao personagem
				a->direcao=270;
				ATOR_TrocaEstado(a, ATRIZ_ANDANDO, false);
				break;
			case EVT_PRESSIONOU_ESQ:
				// Dá uma direção ao personagem
				a->direcao=180;
				ATOR_TrocaEstado(a, ATRIZ_ANDANDO, false);
				break;
			case EVT_PRESSIONOU_DIR:
				// Dá uma direção ao personagem
				a->direcao=0;
				ATOR_TrocaEstado(a, ATRIZ_ANDANDO, false);
				break;
			case EVT_PRESSIONOU_CIMA:
				// Dá uma direção ao personagem
				a->direcao=90;
				ATOR_TrocaEstado(a, ATRIZ_ANDANDO, false);
				break;
			}
		}
		break;
	case ATRIZ_ANDANDO:
		// quando começa, coloca a animação de acordo com a direção
		if(a->estado.subestado==ESTADO_INICIO)
		{
			a->olhandoPara=a->direcao;
			a->velocidade=VATRIZ;
			// Escolhe a animação entre os quatro angulos possíveis
			switch(a->olhandoPara)
			{
			case 270:
				ATOR_TrocaAnimacao(a, 5);
				break;
			case 90:
				ATOR_TrocaAnimacao(a, 6);
				break;
			case 180:
				ATOR_TrocaAnimacao(a, 7);
				break;
			case 0:
			default:
				ATOR_TrocaAnimacao(a, 8);
				break;
			}
			a->estado.subestado=ESTADO_RODANDO;
		}
		// Processa os eventos
		while(ATOR_ProximoEvento(a, &ev))
		{
			switch(ev.tipoEvento)
			{
			case EVT_LIBEROU_BAIXO:
				if(270 == a->direcao)
				{
					a->velocidade=0;
					ATOR_TrocaEstado(a, ATRIZ_PARADA, false);
				}
				break;
			case EVT_LIBEROU_CIMA:
				if(90 == a->direcao)
				{
					a->velocidade=0;
					ATOR_TrocaEstado(a, ATRIZ_PARADA, false);
				}
				break;
			case EVT_LIBEROU_ESQ:
				if(180 == a->direcao)
				{
					a->velocidade=0;
					ATOR_TrocaEstado(a, ATRIZ_PARADA, false);
				}
				break;
			case EVT_LIBEROU_DIR:
				if(0 == a->direcao)
				{
					a->velocidade=0;
					ATOR_TrocaEstado(a, ATRIZ_PARADA, false);
				}
				break;
			case EVT_COLIDIU_PAREDE:
				switch(ev.subtipo)
				{
					case SUBEVT_COLIDIU_PAREDE_BAIXO:
						printf("Bateu em parede em baixo.\n");
						break;
					case SUBEVT_COLIDIU_PAREDE_CIMA:
						printf("Bateu em parede em cima.\n");
						break;
					case SUBEVT_COLIDIU_PAREDE_ESQ:
						printf("Bateu em parede a esquerda.\n");
						break;
					case SUBEVT_COLIDIU_PAREDE_DIR:
						printf("Bateu em parede a direita.\n");
						break;
				}	
				break;
			}
		}
		break;
	}
	return true;
}
