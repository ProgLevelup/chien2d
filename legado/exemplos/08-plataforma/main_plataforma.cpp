#include <stdio.h>
#include <c2d2/chien2d2.h>
#include <c2d2/chien2d2mapa.h>
#include <c2d2/chien2d2primitivas.h>
#include <c2d2/chienaudio2.h>
#include <c2d2/ator.h>
#include "jogo_atores.h"
#include "jogo_darkphoenix.h"
#include "jogo_bola.h"

// As dimensões da tela desejada (cheia)
#define LARGURA_TELA	640
#define ALTURA_TELA	480
#define GRAVIDADE	250
#define MAXGRAVIDADE	2000

void ProcessaControle(Ator *a);

void ProcessaControle(Ator *a)
{
	static Evento ev;
	static C2D2_Botao *teclado = C2D2_PegaTeclas();
	// TEsta se envia mensagens
	if(teclado[C2D2_CIMA].pressionado)
	{
		ev.tipoEvento = EVT_PRESSIONOU_CIMA;
		ATOR_EnviaEvento(a, &ev);
	}
	if(teclado[C2D2_CIMA].liberado)
	{
		ev.tipoEvento = EVT_LIBEROU_CIMA;
		ATOR_EnviaEvento(a, &ev);
	}
	if(teclado[C2D2_BAIXO].pressionado)
	{
		ev.tipoEvento = EVT_PRESSIONOU_BAIXO;
		ATOR_EnviaEvento(a, &ev);
	}
	if(teclado[C2D2_BAIXO].liberado)
	{
		ev.tipoEvento = EVT_LIBEROU_BAIXO;
		ATOR_EnviaEvento(a, &ev);
	}
	if(teclado[C2D2_ESQUERDA].pressionado)
	{
		ev.tipoEvento = EVT_PRESSIONOU_ESQ;
		ATOR_EnviaEvento(a, &ev);
	}
	if(teclado[C2D2_ESQUERDA].liberado)
	{
		ev.tipoEvento = EVT_LIBEROU_ESQ;
		ATOR_EnviaEvento(a, &ev);
	}
	if(teclado[C2D2_DIREITA].pressionado)
	{
		ev.tipoEvento = EVT_PRESSIONOU_DIR;
		ATOR_EnviaEvento(a, &ev);
	}
	if(teclado[C2D2_DIREITA].liberado)
	{
		ev.tipoEvento = EVT_LIBEROU_DIR;
		ATOR_EnviaEvento(a, &ev);
	}
	if(teclado[C2D2_Z].pressionado)
	{
		ev.tipoEvento = EVT_PRESSIONOU_BOTAO1;
		ATOR_EnviaEvento(a, &ev);
	}
}

int main(int narg, char **valarg)
{
	// Inicia a Chien2D 2 e testa se deu tudo certo
	if(C2D2_Inicia(LARGURA_TELA, ALTURA_TELA, C2D2_JANELA, C2D2_DESENHO_OPENGL, "Darkphoenix - O Baixinho Invocado"))
		printf("Iniciou a Chien2D 2 com sucesso\n");
	else
	{
		printf("Falhou ao iniciar a Chien2D 2. Encerrando o programa.\n");
		return 0;
	}
	// Inicia as primitivas
	C2D2P_Inicia();
	// Inicia os mapas
	C2D2M_Inicia();
	// Inicia o áudio
	CA2_Inicia();
	// Inicia os personagens
	ATOR_Inicia();
	// Carrega a fonte do sistema
	unsigned int fonte = C2D2_CarregaFonte("imagens/isabelle64_alpha.png", 64);
	// Carrega o mapa
	unsigned int mapa = C2D2M_CarregaMapaMappy("fases/Aula04-Mapa.FMP", "fases/Aula04-tileset.png");
	// Dá as velocidades. As duas últimas devem ser 1. As demais, incrementa de 1 em 1
	int numcamadas = 4;
	C2D2M_VelocidadeCamadaMapa(mapa, numcamadas-1, 1);
	// Faz um for esotérico para atribuir as velocidades. Se pra você facilitar, use uma camada só que não dá nada
	for(int i=0, vel=numcamadas-1; i<numcamadas-1;i++, vel--)
		C2D2M_VelocidadeCamadaMapa(mapa, i, vel);
	// A camada de marcas é sempre a última
	C2D2M_CamadaMarcas(mapa, 4, 89);
	// Indica a gravidade a aplicar no mapa
	C2D2M_GravidadeMapa(mapa, GRAVIDADE, MAXGRAVIDADE);
	// Carrega o personagem
	bool cdark = JOGO_CarregaDarkPhoenix();
	bool cbola = JOGO_CarregaBola();
	// As músicas
	unsigned int musicas[2];
	musicas[0] = CA2_CarregaMusica("audio/AulaPaulo_byPiovezan.it");
	musicas[1] = CA2_CarregaMusica("audio/venceu.wav");
	// Testa se carregou certo (se é diferente de 0)
	if(fonte==0 || mapa == 0 || !cdark || !cbola)
	{
		printf("Falhou ao carregar alguma coisa. Encerrando.\n");
		// Encerra a Chien2d2
		CA2_Encerra();
		ATOR_Encerra();
		C2D2M_Encerra();
		C2D2_Encerra();
		return 0;
	}
	C2D2_TrocaCorLimpezaTela(121, 188, 255);
	C2D2_Botao *teclado = C2D2_PegaTeclas();
	// cria o personagem
	int x=60, y=60;
	C2D2M_PrimeiroBlocoMarca(mapa, C2D2M_INICIO, &x, &y);
	Ator *dark = ATOR_CriaAtor(DARKPHOENIX, x, y, 0);
	Evento ev;
	bool nafase=true;
	// Procura saber quantos inimigos tem na fase
	int numInimigos=0;
	if(C2D2M_PrimeiroBlocoMarca(mapa, MARCA_BOLA_DIREITA, &x, &y))
	{
		numInimigos++;
		while(C2D2M_ProximoBlocoMarca(mapa, &x, &y))
			numInimigos++;
	}
	if(C2D2M_PrimeiroBlocoMarca(mapa, MARCA_BOLA_ESQUERDA, &x, &y))
	{
		numInimigos++;
		while(C2D2M_ProximoBlocoMarca(mapa, &x, &y))
			numInimigos++;
	}
	Ator **inimigos = (Ator**)malloc(sizeof(Ator*)*numInimigos);
	memset(inimigos, 0, numInimigos*sizeof(Ator*));
	numInimigos=0;
	if(C2D2M_PrimeiroBlocoMarca(mapa, MARCA_BOLA_DIREITA, &x, &y))
	{
		inimigos[numInimigos++]=ATOR_CriaAtor(BOLA, x, y, 0);
		while(C2D2M_ProximoBlocoMarca(mapa, &x, &y))
			inimigos[numInimigos++]=ATOR_CriaAtor(BOLA, x, y, 0);
	}
	if(C2D2M_PrimeiroBlocoMarca(mapa, MARCA_BOLA_ESQUERDA, &x, &y))
	{
		inimigos[numInimigos++]=ATOR_CriaAtor(BOLA, x, y, 180);
		while(C2D2M_ProximoBlocoMarca(mapa, &x, &y))
			inimigos[numInimigos++]=ATOR_CriaAtor(BOLA, x, y, 180);
	}
	// Coloca a música para tocar
	CA2_TocaMusica(musicas[0], -1);
	// Indica se e a primeira vez que vai tocar a musicado fim da fase
	bool primeira = false;
	while(!teclado[C2D2_ESC].pressionado && nafase)
	{
		C2D2_LimpaTela();
		// Testa as colisões
		if(ATOR_ColidiuBlocoCenario(dark, mapa, C2D2M_MORTE))
		{
			ev.tipoEvento = EVT_COLIDIU_ARMADILHA;
			ATOR_EnviaEvento(dark, &ev);
		}
		if(ATOR_ColidiuBlocoCenario(dark, mapa, C2D2M_CHECK))
		{
			ev.tipoEvento = EVT_CHECKPOINT;
			ATOR_EnviaEvento(dark, &ev);
		}
		if(ATOR_ColidiuBlocoCenario(dark, mapa, C2D2M_FIM))
		{
			ev.tipoEvento = EVT_FIM_FASE;
			ATOR_EnviaEvento(dark, &ev);
			if(!primeira)
			{
				CA2_TocaMusica(musicas[1], -1);
				primeira=true;
			}
		}
		for(int i=0;i<numInimigos;i++)
			if(inimigos[i]!=0)
				ATOR_ColidiuAtores(dark, inimigos[i]);
		// Atualiza a lógica
		ATOR_AplicaEstado(dark, mapa, LARGURA_TELA, ALTURA_TELA);
		for(int i=0;i<numInimigos;i++)
			if(inimigos[i]!=0)
				ATOR_AplicaEstado(inimigos[i], mapa, LARGURA_TELA, ALTURA_TELA);
		// Lê os controles
		ProcessaControle(dark);
		// Atualiza os personagens
		ATOR_Atualiza(dark, mapa);
		for(int i=0;i<numInimigos;i++)
			if(inimigos[i]!=0)
				if(!ATOR_Atualiza(inimigos[i], mapa))
				{
					free(inimigos[i]);
					inimigos[i]=0;
				}
		// Centraliza o mapa no personagem principal
		ATOR_CentraMapa(dark, mapa, LARGURA_TELA, ALTURA_TELA);
		// Roda os eventos do jogo
		while(ATOR_ProximoEventoJogo(&ev))
		{
			switch(ev.tipoEvento)
			{
			case EVT_FIM_FASE:
				nafase=false;
				break;
			}
		}

		// Desenha o cenário
		C2D2M_DesenhaCamadaMapa(mapa, 0, 0, 0, 640, 480);
		C2D2M_DesenhaCamadaMapa(mapa, 1, 0, 0, 640, 480);
		C2D2M_DesenhaCamadaMapa(mapa, 2, 0, 0, 640, 480);
		// DEsenha os personagens
		ATOR_Desenha(dark, mapa, 0, 0);
		for(int i=0;i<numInimigos;i++)
			if(inimigos[i]!=0)
				ATOR_Desenha(inimigos[i], mapa, 0, 0);
		// Desenha a camada mais superior
		C2D2M_DesenhaCamadaMapa(mapa, 3, 0, 0, 640, 480);
		// DEsenha as mensagens
		if((dark->estado.estado == DARK_MORRENDO || dark->estado.estado == DARK_MORREU) && dark->vidas>0)
			C2D2_DesenhaTexto(fonte, LARGURA_TELA/2, ALTURA_TELA/2, "Ops!", C2D2_TEXTO_CENTRALIZADO);
		if((dark->estado.estado == DARK_MORRENDO || dark->estado.estado == DARK_MORREU) && dark->vidas==0)
			C2D2_DesenhaTexto(fonte, LARGURA_TELA/2, ALTURA_TELA/2, "Game Over", C2D2_TEXTO_CENTRALIZADO);
		if(dark->estado.estado == DARK_VITORIA)
			C2D2_DesenhaTexto(fonte, LARGURA_TELA/2, ALTURA_TELA/2, "Fase Completa!", C2D2_TEXTO_CENTRALIZADO);

		C2D2_Sincroniza(C2D2_FPS_PADRAO);
		if(teclado[C2D2_ENTER].ativo)
			C2D2_Pausa(50);
		C2D2M_AnimaMapa(mapa);
	}
	// Apaga os personagens
	free(dark);
	for(int i=0;i<numInimigos;i++)
		if(inimigos[i]!=0)
			free(inimigos[i]);
	free(inimigos);
	// Descarrega o personagem
	ATOR_DescarregaAtor(DARKPHOENIX);
	// Apaga as imagens carregadas na memória
	C2D2_RemoveFonte(fonte);
	// Encerra os atores
	ATOR_Encerra();
	// Encerra os mapas
	C2D2M_Encerra();
	// Encerra o áudio
	CA2_Encerra();
	// Encerra a Chien2D 2
	C2D2_Encerra();
	return 0;
}
