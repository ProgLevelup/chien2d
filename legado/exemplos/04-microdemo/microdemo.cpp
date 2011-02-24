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
#include <stdlib.h>
#include <time.h>
#include <c2d2/chien2d2.h>
#include <c2d2/chien2d2primitivas.h>
#include <c2d2/chienaudio2.h>

int main(int narg, char **valarg)
{
	// Deixa os números aleatórios a cada vez que o programa roda
	srand(time_t(0));
	// Inicia a Chien2D 2 e testa se deu tudo certo
	if(C2D2_Inicia(800, 600, C2D2_TELA_CHEIA, C2D2_DESENHO_PADRAO, "Microdemo"))
		printf("Iniciou a Chien2D 2 com sucesso\n");
	else
	{
		printf("Falhou ao iniciar a Chien2D 2. Encerrando o programa.\n");
		return 0;
	}
	// Inicia as primitivas
	C2D2P_Inicia();
	// Inicia o sistema de áudio. Deve ser feito obrigatoriamente após iniciar a Chien2D 2
	if(CA2_Inicia())
		printf("Sistema de audio iniciado corretamente. O demo vai rodar com sons e musica.\n");
	else
		printf("Nao foi possivel iniciar o sistema de audio. Vai rodar sem som e musicas :( ...\n");
	// Cria o personagem do jogo
	unsigned int spDark = C2D2_CarregaSpriteSet("darkphoenix.png", 32, 42);
	// Carrega a fonte principal
	unsigned int fonte = C2D2_CarregaFonte("isabelle64.png", 64);
	// Carrega a fonte secundária (usada no final)
	unsigned int fonte2 = C2D2_CarregaFonte("arial32.png", 32);
	// Carrega a música
	unsigned int mfundo = CA2_CarregaMusica("Phenomena_Firefox&Tip.s3m");
	// Carrega o efeito sonoro da batida
	unsigned int batida = CA2_CarregaEfeito("bate.ogg");
	// Carrega o efeito sonoro da pegada
	unsigned int pegada = CA2_CarregaEfeito("pegada.ogg");

	// Recupera o teclado
	C2D2_Botao *teclas = C2D2_PegaTeclas();
	// Recupera o mouse
	C2D2_Mouse *mouse = C2D2_PegaMouse();
	
	// Troca a cor de fundo da tela
	C2D2_TrocaCorLimpezaTela(0, 0, 0);

	// Variáveis para animar a reta
	int yreta=0;
	int vreta=4;
	int xreta=0;
	int vreta2=4;

	// Variáveis para animar o texto
	int xtexto=799;
	int vtexto=-4;
	int ltexto;
	C2D2_DimensoesTexto(fonte, "Demonstração de texto animado com a Chien2D 2.", &ltexto, 0);
	// Variáveis para animar o personagem
	int xpers = -32;
	int qpers[4] = {0, 1, 2, 1};
	int qatual=0;
	int tquadro=10;
	int contasom = 0;
	
	// Cria as estrelas no fundo (pontos na verdade)
	C2D2P_Ponto estrelas[50];
	// Deixa as estrelas em posições aleatórias na tela
	for(int i=0;i<50;i++)
	{
		// Calcula uma coluna para a estrela
		estrelas[i][0] = rand()%800;
		// Calcula uma linha
		estrelas[i][1] = rand()%600;
	}

	// Ajusta o volume
	// Começa a tocar a música em loop infinito
	CA2_TocaMusica(mfundo, -1);
	do
	{
		// Faz a lógica da reta
		yreta += vreta;
		if((yreta<=0 && vreta<0)|| (yreta>=599 && vreta >0))
		{
			vreta *= -1;
			// Cada vez que muda o sentido, toca o áudio
			CA2_TocaEfeito(batida, 0);
		}
		// Faz a lógica da reta
		xreta += vreta2;
		if((xreta<=0 && vreta2<0)|| (xreta>=799 && vreta2 >0))
		{
			vreta2 *= -1;
			// Cada vez que muda o sentido, toca o áudio
			CA2_TocaEfeito(batida, xreta);
		}


		// Faz a lógica do texto animado
		xtexto += vtexto;
		if(xtexto <= -ltexto)
			xtexto=799;
		// Faz a lógica para que o personagem ande
		xpers+=2;
		if(xpers>799)
			xpers=-32;
		// Faz a lógica da animação do personagem
		if(--tquadro == 0)
		{
			qatual = (qatual+1)%4;
			tquadro = 10;
			// Toca o efeito do passo se for par o conta som
			if(contasom++ %2 == 0)
				// Para tocar o som fazemos uma regra de 3. a posição -32 é igual a posição -100
				CA2_TocaEfeitoTela(pegada, xpers);
		}
		// Faz a lógica das estrelas
		for(int i=0;i<50;i++)
		{
			// Testa se saiu fora da tela
			if((estrelas[i][0]-=3) < 0)
			{
				// Coloca a estrela fora da tela
				estrelas[i][0]=802;
				// Calcula uma nova linha
				estrelas[i][1] = rand()%600;
			}
		}

		// Limpa a tela com a cor de fundo atual
		C2D2_LimpaTela();
		// Desenha as estrelas
		C2D2P_Pontos(estrelas, 50, 255,255,255);
		// Desenha as retas animadas
		C2D2P_Linha(0, yreta, 799, yreta, 255, 255, 255);
		C2D2P_Linha(xreta, 0, xreta, 599, 255, 255, 255);
		// Desenha o texto em movimento
		C2D2_DesenhaTexto(fonte, xtexto, 530, "Demonstração de texto animado com a Chien2D 2.", C2D2_TEXTO_ESQUERDA); 
		// Desenha o sprite
		C2D2_DesenhaSprite(spDark, qpers[qatual], xpers, 268);

		// Faz a troca do front buffer com o back buffer e atualiza as entradas
		C2D2_Sincroniza(C2D2_FPS_PADRAO);
	}
	while(!teclas[C2D2_ENCERRA].pressionado && !teclas[C2D2_ESC].pressionado);
	CA2_FadeMusica(2500);
	// Troca a cor de fundo da tela
	C2D2_TrocaCorLimpezaTela(220, 220, 220);
	// Encerra o demo, mostrando a mensagem durante 5 segundos
	int tempo = 0;
	do
	{
		C2D2_LimpaTela();
		// Desenha os créditos da música
		C2D2_DesenhaTexto(fonte2, 400, 68, "Música:", C2D2_TEXTO_CENTRALIZADO);
		C2D2_DesenhaTexto(fonte, 400, 118, "Enigma", C2D2_TEXTO_CENTRALIZADO);
		C2D2_DesenhaTexto(fonte2, 400, 192, "por Firefox & Tip", C2D2_TEXTO_CENTRALIZADO);
		// Desenha os créditos do demo
		C2D2_DesenhaTexto(fonte2, 400, 368, "Código:", C2D2_TEXTO_CENTRALIZADO);
		C2D2_DesenhaTexto(fonte, 400, 418, "© PV 2007", C2D2_TEXTO_CENTRALIZADO);
		C2D2_DesenhaTexto(fonte2, 400, 492, "Bibliotecas Chien2D 2 e Chien Audio 2", C2D2_TEXTO_CENTRALIZADO);
		C2D2_Sincroniza(C2D2_FPS_PADRAO);
	}while(tempo++<300 && !teclas[C2D2_ENCERRA].pressionado && !teclas[C2D2_ESC].pressionado);

	// Remove as imagens
	C2D2_RemoveSpriteSet(spDark);
	// Remove a fonte
	C2D2_RemoveFonte(fonte);

	// Encerra a Chien Audio 2
	CA2_Encerra();
	// Encerra a Chien2D 2
	C2D2_Encerra();
	return 0;
}
