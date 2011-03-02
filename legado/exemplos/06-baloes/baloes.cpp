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

// Indica se deve encerrar ou não a aplicação
bool encerra=false;
// Indica o estado atual (tela) do jogo
// Valores:
//      1 - Menu principal
//      2 - Jogo
//      3 - Créditos
int estadoatual=1;

// Recursos do jogo
unsigned int fonte32=0;
unsigned int fonte64=0;
unsigned int botao_jogar=0;
unsigned int botao_creditos=0;
unsigned int botao_voltar=0;
unsigned int botao_sair=0;
unsigned int spmira=0;
unsigned int spbalao=0;
unsigned int titulo=0;

// Protótipo das funções dos recursos
bool BALOES_CarregaRecursos();
void BALOES_DescarregaRecursos();
// Protótipos das funções das telas do jogo
void BALOES_TelaPrincipal();
void BALOES_TelaJogo();
void BALOES_TelaCreditos();

// Declaração das funções do jogo

bool BALOES_CarregaRecursos()
{
    // Carrega a fonte tamanho 32. Para cada recurso carregado, testamos se ele foi carregado corretamente
    fonte32 = C2D2_CarregaFonte("./graficos/arial32.png", 32);
    if(fonte32==0)
    {
        printf("Falhou carregar a fonte de tamanho 32.\n");
        return false;
    }
    // Como já tem uma fonte carregada, pode fazer a tela de load
    C2D2_TrocaCorLimpezaTela(0,0,255);
    C2D2_LimpaTela();
    C2D2_DesenhaTexto(fonte32, 400, 284, "Carregando. Por favor, aguarde.", C2D2_TEXTO_CENTRALIZADO);
    C2D2_Sincroniza(C2D2_FPS_PADRAO);
    // Continua carregando os recursos
    fonte64 = C2D2_CarregaFonte("./graficos/isabelle64.png", 64);
    if(fonte64==0)
    {
        printf("Falhou carregar a fonte de tamanho 64.\n");        
        return false;
    }
    botao_jogar = C2D2_CarregaSpriteSet("./graficos/botao_jogar.png", 200, 40);
    if(botao_jogar==0)
    {
        printf("Falhou carregar o botão de jogar.\n");        
        return false;
    }
    botao_creditos = C2D2_CarregaSpriteSet("./graficos/botao_creditos.png", 200, 40);
    if(botao_creditos==0)
    {
        printf("Falhou carregar o botão de créditos.\n");        
        return false;
    }
    botao_sair = C2D2_CarregaSpriteSet("./graficos/botao_sair.png", 200, 40);
    if(botao_sair==0)
    {
        printf("Falhou carregar o botão desair.\n");        
        return false;
    }
    botao_voltar = C2D2_CarregaSpriteSet("./graficos/botao_voltar.png", 200, 40);
    if(botao_voltar==0)
    {
        printf("Falhou carregar o botão de voltar.\n");        
        return false;
    }
    spmira = C2D2_CarregaSpriteSet("./graficos/mira.png", 21, 21);
    if(spmira==0)
    {
        printf("Falhou carregar o sprite da mira.\n");        
        return false;
    }
    spbalao = C2D2_CarregaSpriteSet("./graficos/balao.png", 35, 50);
    if(spbalao==0)
    {
        printf("Falhou carregar o sprite do balão.\n");        
        return false;
    }
    titulo = C2D2_CarregaSpriteSet("./graficos/titulo.png", 0, 0);
    if(spbalao==0)
    {
        printf("Falhou carregar a tela de título.\n");        
        return false;
    }

    // Se chegou até aqui, carregou tudo 100%
    return true;  
}

void BALOES_DescarregaRecursos()
{
    // faz a tela de descarregamento
    if(fonte32!=0)
    {
        C2D2_TrocaCorLimpezaTela(0,0,255);
        C2D2_LimpaTela();
        C2D2_DesenhaTexto(fonte32, 400, 284, "Encerrando. Por favor, aguarde.", C2D2_TEXTO_CENTRALIZADO);
        C2D2_Sincroniza(C2D2_FPS_PADRAO);
    }
    // Apaga tudo, se for 0 a biblioteca ignora
    C2D2_RemoveFonte(fonte32);
    C2D2_RemoveFonte(fonte64);
    C2D2_RemoveSpriteSet(botao_jogar);
    C2D2_RemoveSpriteSet(botao_creditos);
    C2D2_RemoveSpriteSet(botao_sair);
    C2D2_RemoveSpriteSet(botao_voltar);
    C2D2_RemoveSpriteSet(spmira);
    C2D2_RemoveSpriteSet(spbalao);    
}


void BALOES_TelaPrincipal()
{
	// Recupera o teclado
	C2D2_Botao *teclas = C2D2_PegaTeclas();
    C2D2_Mouse *mouse = C2D2_PegaMouse();

    // Variáveis para os botões, 1 se o mouse está em cima, 0 caso contrário
    int bjogar=0, bcreditos=0, bsair=0;
    
    // Sincroniza para atualizar o teclado
	C2D2_Sincroniza(C2D2_FPS_PADRAO);
	do
	{
        // Lógica: se pressiona ESC no menu, deve sair fora
        if(teclas[C2D2_ESC].pressionado)
            encerra=true;
        // Vê se o mouse está em cima de um botão e atualiza o estado
        if(C2D2_ColidiuSprites(spmira, 0, mouse->x, mouse->y, botao_jogar, 0, 100,400))
            bjogar=1;
        else
            bjogar=0;
        if(C2D2_ColidiuSprites(spmira, 0, mouse->x, mouse->y, botao_creditos, 0, 500,400))
            bcreditos=1;
        else
            bcreditos=0;
        if(C2D2_ColidiuSprites(spmira, 0, mouse->x, mouse->y, botao_sair, 0, 300,500))
            bsair=1;
        else
            bsair=0;
        // Se o botão do mouse está pressionado, toma uma ação
        if(mouse->botoes[C2D2_MESQUERDO].pressionado)
        {
            // O botão ativo é o botão da ação equivalente
            if(bjogar==1)
                estadoatual = 2;
            else if(bcreditos==1)
                estadoatual=3;
            else if(bsair==1)
                encerra=true;
        }
        
        // Limpa a tela e desenha
		C2D2_LimpaTela();
        // Desenha a tela de título
        C2D2_DesenhaSprite(titulo, 0, 0, 0);
        // Desenha os botões de acordo com o estado
        C2D2_DesenhaSprite(botao_jogar, bjogar, 100,400);
        C2D2_DesenhaSprite(botao_creditos, bcreditos, 500,400);
        C2D2_DesenhaSprite(botao_sair, bsair, 300,500);        
        //Desenha a mira
        C2D2_DesenhaSprite(spmira, 0, mouse->x, mouse->y);
		// Faz a troca do front buffer com o back buffer e atualiza as entradas
		C2D2_Sincroniza(C2D2_FPS_PADRAO);
	}
	while(!encerra && estadoatual==1);
}

void BALOES_TelaJogo()
{
	// Recupera o teclado
	C2D2_Botao *teclas = C2D2_PegaTeclas();
    C2D2_Mouse *mouse = C2D2_PegaMouse();

    // As vidas do jogador (3 no total, uma em uso e 2 na reserva)
    int vidas=2;
    // A posição do balão na tela, já com valores iniciais
    int ybalao=600,xbalao = rand()%765;
    // A velocidade de subida do balão
    int vsobe=1;
    // Sincroniza para atualizar o teclado
	C2D2_Sincroniza(C2D2_FPS_PADRAO);
    // Troca a cor de limpeza para azul céu
    C2D2_TrocaCorLimpezaTela(63, 127, 255);
	do
	{
        // Lógica: se pressiona ESC no jogo, deve sair fora para o menu principal
        if(teclas[C2D2_ESC].pressionado)
            estadoatual=1;
        // Desloca o balão
        ybalao -= vsobe;
        // Se o botão do mouse está pressionado, toma uma ação
        if(mouse->botoes[C2D2_MESQUERDO].pressionado)
        {
            // Testa se acertou um balão
            if(C2D2_ColidiuSprites(spmira, 0, mouse->x, mouse->y,spbalao,0,xbalao, ybalao))
            {
                // Aqui conta o ponto
                // Aqui reiniciliza o balão
                ybalao=600;
                xbalao=rand()%765;
            }
        }
        // Se o balão saiu da tela, tira uma vida. Se baixar de zero, game over
        if(ybalao<=-50)
        {
            // Reinicia o balão
            ybalao=600;
            xbalao=rand()%765;
            // Se perdeu tudo, volta ao menu
            if(--vidas<0)
                estadoatual=1;
        }
        // Limpa a tela e desenha
		C2D2_LimpaTela();
        // Desenha o botão de acordo com o estado
        C2D2_DesenhaSprite(spbalao, 0, xbalao,ybalao);
        //Desenha a mira
        C2D2_DesenhaSprite(spmira, 0, mouse->x, mouse->y);
		// Faz a troca do front buffer com o back buffer e atualiza as entradas
		C2D2_Sincroniza(C2D2_FPS_PADRAO);
	}
	while(estadoatual==2);  
    
}

void BALOES_TelaCreditos()
{
	// Recupera o teclado
	C2D2_Botao *teclas = C2D2_PegaTeclas();
    C2D2_Mouse *mouse = C2D2_PegaMouse();

    // Variável para o botão, 1 se o mouse está em cima, 0 caso contrário
    int bvoltar=0;
    
    // Sincroniza para atualizar o teclado
	C2D2_Sincroniza(C2D2_FPS_PADRAO);
    // Troca a cor de limpeza para verde
    C2D2_TrocaCorLimpezaTela(128, 255, 128);
	do
	{
        // Lógica: se pressiona ESC no menu, deve sair fora
        if(teclas[C2D2_ESC].pressionado)
            estadoatual=1;
        // Vê se o mouse está em cima de um botão e atualiza o estado
        if(C2D2_ColidiuSprites(spmira, 0, mouse->x, mouse->y, botao_voltar, 0, 550, 518))
            bvoltar=1;
        else
            bvoltar=0;
        // Se o botão do mouse está pressionado, toma uma ação
        if(mouse->botoes[C2D2_MESQUERDO].pressionado)
        {
            // O botão ativo é o botão da ação equivalente
            if(bvoltar==1)
                estadoatual=1;
        }
        
        // Limpa a tela e desenha
		C2D2_LimpaTela();
        // Desenha o botão de acordo com o estado
        C2D2_DesenhaSprite(botao_voltar, bvoltar, 550,518);
        //Desenha a mira
        C2D2_DesenhaSprite(spmira, 0, mouse->x, mouse->y);
		// Faz a troca do front buffer com o back buffer e atualiza as entradas
		C2D2_Sincroniza(C2D2_FPS_PADRAO);
	}
	while(estadoatual==3);    
}


int main(int narg, char **valarg)
{
	// Deixa os números aleatórios a cada vez que o programa roda
	srand(time_t(0));
	// Inicia a Chien2D 2 e testa se deu tudo certo
	if(C2D2_Inicia(800, 600, C2D2_JANELA, C2D2_DESENHO_PADRAO, "Baloes"))
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
		printf("Sistema de audio iniciado corretamente. O jogo vai rodar com sons e musica.\n");
	else
		printf("Nao foi possivel iniciar o sistema de audio. Vai rodar sem som e musicas :( ...\n");

    // Carrega os recursos e testa se pode continuar (se carregou tudo certo)
    if(BALOES_CarregaRecursos())
    {
        do
	    {
            switch(estadoatual)
            {
                case 1:
                    BALOES_TelaPrincipal();
                    break;
                case 2:
                    BALOES_TelaJogo();
                    break;
                case 3:
                    BALOES_TelaCreditos();
                    break;
                default:
                    // Deu erro, encerra
                    encerra=true;
                    break;
            }
	    }
	    while(!encerra);
    }
    // Remove os recursos que estiverem carregados
    BALOES_DescarregaRecursos();
               
	// Encerra a Chien Audio 2
	CA2_Encerra();
	// Encerra a Chien2D 2
	C2D2_Encerra();
	return 0;
}
