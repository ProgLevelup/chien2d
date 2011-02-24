#include <stdio.h>
#include <c2d2/chien2d2.h>
#include <c2d2/chien2d2mapa.h>
#include <c2d2/chien2d2primitivas.h>
#include <c2d2/ator.h>
#include "jogo_atores.h"
#include "jogo_atriz.h"

// As dimensões da tela desejada (cheia)
#define LARGURA_TELA	800
#define ALTURA_TELA	600
// As dimensões da janela na qual vamos desenhar o jogo
#define LARGURA_JANELA	580
#define ALTURA_JANELA	400
// Quantos pontos devemos ignorar no eixo x,y para colocar a janela no centro
#define OFFX			(LARGURA_TELA-LARGURA_JANELA)/2
#define OFFY			(ALTURA_TELA-ALTURA_JANELA)/2

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
	if(teclado[C2D2_ESPACO].liberado)
	{
		ev.tipoEvento = EVT_PRESSIONOU_BOTAO1;
		ATOR_EnviaEvento(a, &ev);
	}
}


int main(int narg, char **valarg)
{
	// Inicia a Chien2D 2 e testa se deu tudo certo
	if(C2D2_Inicia(LARGURA_TELA, ALTURA_TELA, C2D2_TELA_CHEIA, C2D2_DESENHO_OPENGL, "Mapa Top View"))
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
	// Inicia os personagens
	ATOR_Inicia();
	// Carrega a fonte do sistema
	unsigned int arial32 = C2D2_CarregaFonte("arial32.png", 32);
	// Carrega o mapa
	unsigned int mapa = C2D2M_CarregaMapaMappy("arenainclinada.FMP","arena.png");
	bool cAtriz = JOGO_CarregaAtriz();
	// Testa se carregou certo (se é diferente de 0)
	if(!cAtriz || arial32==0 || mapa == 0)
	{
		printf("Falhou ao carregar alguma coisa. Encerrando.\n");
		// Encerra a Chien2d2
		C2D2M_Encerra();
		C2D2_Encerra();
		return 0;
	}
	// Dá a velocidade para a camada do mapa
	C2D2M_VelocidadeCamadaMapa(mapa, 0, 1);
	// Qual a camada de marcas
	C2D2M_CamadaMarcas(mapa, 1, 41);
	// Recupera o teclado
	C2D2_Botao *teclas = C2D2_PegaTeclas();
	// Coloca a cor de limpeza de tela
	C2D2_TrocaCorLimpezaTela(16,16,16);
	// Procura no mapa a posição para iniciar a personagem
	int xinicial, yinicial;
	if(!C2D2M_PrimeiroBlocoMarca(mapa, C2D2M_INICIO, &xinicial, &yinicial))
	{
		// Se não achou, dá um valor default
		xinicial = 300;
		yinicial = 200;
	}
	// Cria a personagem
	Ator *atriz = ATOR_CriaAtor(JOGO_ATRIZ, xinicial, yinicial, 270);
	// Se falhou criar a atriz, falha o jogo
	if(atriz == 0)
	{
		printf("Falhou ao criar a atriz. Encerrando.\n");
		// Encerra a Chien2d2
		C2D2M_Encerra();
		C2D2_Encerra();
		return 0;
	}

	// Indica que achou a saída (ou não, por default)
	bool encerrou=false;
	// O evento
	Evento ev;
	// Roda!
	while(!teclas[C2D2_ENCERRA].ativo && !teclas[C2D2_ESC].ativo && 
		atriz->estado.estado!=ATOR_ENCERRADO)
	{
		// Limpa a tela
		C2D2_LimpaTela();
		// Se não encerrou, roda a lógica
		if(!encerrou)
		{
			ATOR_AplicaEstado(atriz, mapa, LARGURA_JANELA, ALTURA_JANELA);
			// Processa o controle da atriz
			ProcessaControle(atriz);
			// verifica se a atriz não morre/encerrou a fase (colisão com cenário)
			if(atriz->estado.estado != ATOR_ENCERRADO && atriz->estado.estado != ATOR_MORTO)
			{
				if(ATOR_ColidiuBlocoCenario(atriz, mapa, C2D2M_MORTE))
				{
					ev.tipoEvento = EVT_COLIDIU_ARMADILHA;
					ATOR_EnviaEvento(atriz, &ev);
				}
				// Verifica se a atriz não encontrou o bloco da saída
				if(ATOR_ColidiuBlocoCenario(atriz, mapa, C2D2M_FIM))
				{
					ev.tipoEvento = EVT_FIM_FASE;
					ATOR_EnviaEvento(atriz, &ev);
					encerrou = true;
				}
			}
			// Roda a lógica da atriz
			ATOR_Atualiza(atriz, mapa);
		}
		
		// Atualiza o mapa com a atriz
		ATOR_CentraMapa(atriz, mapa, LARGURA_JANELA, ALTURA_JANELA);
		// Desenha a tela
		// Para manter os sprites dentro da janela, devemos limitar a área de desenho
		// No caso de um jogo que ocupe a tela toda, podemos ignorar este passo

		C2D2M_DesenhaCamadaMapa(mapa, 0, OFFX, OFFY, LARGURA_JANELA, ALTURA_JANELA);

		// Desenha os atores
		ATOR_Desenha(atriz, mapa, OFFX, OFFY);

		// Quando acaba de desenhar os atores, pinta de preto as bordas
		C2D2P_RetanguloPintado(0,0,LARGURA_TELA,OFFY, 0, 0, 0);
		C2D2P_RetanguloPintado(0,OFFY+ALTURA_JANELA,LARGURA_TELA,ALTURA_TELA, 0, 0, 0);
		C2D2P_RetanguloPintado(0,OFFY,OFFX,OFFY+ALTURA_JANELA, 0, 0, 0);
		C2D2P_RetanguloPintado(OFFX+LARGURA_JANELA,OFFY,LARGURA_TELA,OFFY+ALTURA_JANELA, 0, 0, 0);		

		// Se encerrou, mostra a mensagem de vitória
		if(encerrou)
			C2D2_DesenhaTexto(arial32, LARGURA_TELA/2, ALTURA_TELA/2, "Você venceu! Parabéns!!",C2D2_TEXTO_CENTRALIZADO);
		else if(atriz->estado.estado == ATOR_MORTO)
			C2D2_DesenhaTexto(arial32, LARGURA_TELA/2, ALTURA_TELA/2, "Game Over",C2D2_TEXTO_CENTRALIZADO);
		// Troca os buffers de imagem e esperar o tempo para 60fps
		C2D2_Sincroniza(C2D2_FPS_PADRAO);
		// Anima o mapa
		C2D2M_AnimaMapa(mapa);
	}
	// Apaga a atriz
	free(atriz);
	// DEscarrega o modelo da atriz
	ATOR_DescarregaAtor(JOGO_ATRIZ);
	// Apaga as imagens carregadas na memória
	C2D2_RemoveFonte(arial32);
	// Encerra os atores
	ATOR_Encerra();
	// Encerra os mapas
	C2D2M_Encerra();
	// Encerra a Chien2D 2
	C2D2_Encerra();

	return 0;
}
