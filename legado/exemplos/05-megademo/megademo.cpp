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
#include <math.h>

#include <c2d2/chien2d2.h>
#include <c2d2/chien2d2primitivas.h>
#include <c2d2/chienaudio2.h>

#define NUM_RECURSOS	8
#define PI		3.1415926535897932384626433832795
#define RAD		PI/180

// o tmepo total já passado
unsigned long int ttotal = 0;

// Música do demo
unsigned int mfundo = 0;
// Declaração dos recursos gráficos do demo

// A fonte do relogio digital
unsigned int fdigital = 0;
// A fonte dos textos padrão
unsigned int fsistema = 0;
// A fonte para texto reduzido
unsigned int fpequeno = 0;
// A tela inicial
unsigned int tuxwindows = 0;
// a tela título
unsigned int titulo = 0;
// O sprite da menina
unsigned int sprite1 = 0;
// O sprite da estrela
unsigned int sprite2 = 0;

// Protótipos de funções
bool Carrega();
void Descarrega();
void BarraCarregamento(int carregados, int total);
unsigned long int RelogioDigital(int x, int y);
void Intro();
void Primitivas();
void Sprites();
void Estrela();
void Creditos();

// Declaração de funções

// Função que carrega os recursos globais
bool Carrega()
{
	int carga=0;
	BarraCarregamento(carga, NUM_RECURSOS);
	mfundo = CA2_CarregaMusica("./dados/2nd_PurpleMotion.s3m");
	BarraCarregamento(++carga, NUM_RECURSOS);
	fdigital = C2D2_CarregaFonte("./dados/digital.png", 16);
	BarraCarregamento(++carga, NUM_RECURSOS);
	fsistema = C2D2_CarregaFonte("./dados/isabelle64.png", 64);
	BarraCarregamento(++carga, NUM_RECURSOS);
	fpequeno = C2D2_CarregaFonte("./dados/arial32.png", 32);
	BarraCarregamento(++carga, NUM_RECURSOS);
	tuxwindows = C2D2_CarregaSpriteSet("./dados/tuxwindows.jpg", 0, 0);
	BarraCarregamento(++carga, NUM_RECURSOS);
	titulo = C2D2_CarregaSpriteSet("./dados/titulo.png", 0, 0);
	BarraCarregamento(++carga, NUM_RECURSOS);
	sprite1 = C2D2_CarregaSpriteSet("./dados/atriz.png", 32, 64);
	BarraCarregamento(++carga, NUM_RECURSOS);
	sprite2 = C2D2_CarregaSpriteSet("./dados/estrela.png", 256, 256);
	BarraCarregamento(++carga, NUM_RECURSOS);

	return true;
}

// Função que descarrega os recursos globais
void Descarrega()
{
	CA2_RemoveMusica(mfundo);
	C2D2_RemoveFonte(fdigital);
	C2D2_RemoveFonte(fsistema);
	C2D2_RemoveFonte(fpequeno);
	C2D2_RemoveSpriteSet(tuxwindows);
	C2D2_RemoveSpriteSet(titulo);
	C2D2_RemoveSpriteSet(sprite1);
	C2D2_RemoveSpriteSet(sprite2);
}

void BarraCarregamento(int carregados, int total)
{
	// Calcula a cor
	int cor = 255*carregados/total;
	C2D2_LimpaTela();
	// A barra tem 600 pontos de largura. Desenha o contorno primeiro
	C2D2P_Retangulo(95, 270, 705, 330, 255-cor, cor, 0);
	// DEsenha a barra proporcional
	C2D2P_RetanguloPintado(100, 275, 100+600*carregados/total , 325, 255-cor, cor, 0);
	C2D2_Sincroniza(C2D2_FPS_PADRAO);
}

// O relógio digital que acompanha o demo
unsigned long int RelogioDigital(int x, int y)
{
	// 1:56 minutos em milisegundos
	static unsigned long tempo = 116000;
	// O tempo inicial do demo
	static unsigned long tinicial = C2D2_TempoSistema();
	// O tempo atual
	static unsigned long tatual;
	static char texto[12];
	// Pega o tempo atual
	tatual = C2D2_TempoSistema();
	// Calcula a diferença
	unsigned long diferenca = tatual-tinicial;
	// Desenha o fundo de 144x32
	C2D2P_RetanguloPintado(x, y, x+91, y+31, 0,0,0);
	C2D2P_Retangulo(x, y, x+91, y+31, 255,255,255);
	// Calcula os tempos
	int minutos=0, segundos=0, centesimos=0;
	if(diferenca <=116000)
	{
		minutos = (tempo-diferenca)/60000;
		segundos = ((tempo-diferenca)%60000)/1000;
		centesimos = ((tempo-diferenca)%1000)/10;
	}
	sprintf(texto, "%02i:%02i:%02i", minutos, segundos, centesimos);
	C2D2_DesenhaTexto(fdigital, x+8, y+8, texto, C2D2_TEXTO_ESQUERDA);
	// Retorna o tempo
	return diferenca;
}

// A função da parte inicial
void Intro()
{
	C2D2_Botao *teclado = C2D2_PegaTeclas();
	unsigned long int tempo;
	// Desenha a tela do tux enquanto não passar 6,5 segundos
	do
	{
		C2D2_DesenhaSprite(tuxwindows, 0, 0, 0);
		// DEsenha o relógio contador e pega o tempo
		tempo=RelogioDigital(692,552);
		C2D2_Sincroniza(C2D2_FPS_PADRAO);
	}
	while(tempo < 6500 && !teclado[C2D2_ESC].pressionado && !teclado[C2D2_ENCERRA].pressionado);
	// Cai a tela por 1,5 segundos
	do
	{
		C2D2_DesenhaSprite(titulo, 0, 0, 0);
		C2D2_DesenhaSprite(tuxwindows, 0, 0, 600*(tempo-6500)/1500);
		// DEsenha o relógio contador e pega o tempo
		tempo=RelogioDigital(692,552);
		C2D2_Sincroniza(C2D2_FPS_PADRAO);
	}
	while(tempo < 8000 && !teclado[C2D2_ESC].pressionado && !teclado[C2D2_ENCERRA].pressionado);
	// Mostra o título por 5 segundos
	do
	{
		C2D2_DesenhaSprite(titulo, 0, 0, 0);
		// DEsenha o relógio contador e pega o tempo
		tempo=RelogioDigital(692,552);
		C2D2_Sincroniza(C2D2_FPS_PADRAO);
	}
	while(tempo < 15000 && !teclado[C2D2_ESC].pressionado && !teclado[C2D2_ENCERRA].pressionado);
	// Indica o tempo total já passado para o próximo módulo
	ttotal = 15000;
}

// Função para demonstrar as primitivas
void Primitivas()
{
	C2D2_Botao *teclado = C2D2_PegaTeclas();
	unsigned long int tempo;
	// DEsenha os círculos pulsando
	C2D2_TrocaCorLimpezaTela(200, 200, 200);
	// usa o seno para fazer o tamanho variável
	int angulo=0, tamanho;
	do
	{
		// Calcula o tamanho
		tamanho = abs(35*sin(angulo*RAD));
		// Atualiza o angulo
		angulo=(angulo+1)%360;

		// Limpa a tela
		C2D2_LimpaTela();
		// DEsenha os círculos do tamanho normal
		for(int i=0;i<10;i++)
			for(int j=0; j<8;j++)
			{
				C2D2P_CirculoPintado(40+80*i, 40+80*j, 5+tamanho, 255, 0, 0);
				C2D2P_Circulo(40+80*i, 40+80*j, 5+tamanho, 128, 0, 0);
			}
		// DEsenha os círculos do tamanho diferente
		for(int i=0;i<12;i++)
			for(int j=0; j<9;j++)
			{
				C2D2P_CirculoPintado(80*i, 80*j, 40-tamanho, 0, 255, 0);
				C2D2P_Circulo(80*i, 80*j, 40-tamanho, 0, 128, 0);
			}

		// DEsenha o relógio contador e pega o tempo
		tempo=RelogioDigital(692,552);
		C2D2_DesenhaTexto(fsistema, 400, 500, "180 círculos com borda.", C2D2_TEXTO_CENTRALIZADO);
		C2D2_Sincroniza(C2D2_FPS_PADRAO);
	}
	while(tempo-ttotal < 13500 && !teclado[C2D2_ESC].pressionado && !teclado[C2D2_ENCERRA].pressionado);
	// Desenha as retas
	int posicao=0, direcao=1;
	int cor=0;
	C2D2_TrocaCorLimpezaTela(0,0,0);
	do
	{
		// Atualiza a posição
		posicao+=direcao;
		if(posicao==7 || posicao==0)
			direcao*=-1;
		C2D2_LimpaTela();
		cor=0;
		// Desenha as linhas para os retângulos vermelhos
		for(int i=0;i<37;i++)
		{
			C2D2P_Linha(posicao+8*i, posicao+8*i, posicao+791-8*i, posicao+8*i, 255-cor, 0, 0);
			C2D2P_Linha(posicao+8*i, posicao+591-8*i, posicao+791-8*i, posicao+591-8*i, 255-cor, 0, 0);
			C2D2P_Linha(posicao+8*i, posicao+8*i, posicao+8*i, posicao+591-8*i, 255-cor, 0, 0);
			C2D2P_Linha(posicao+791-8*i, posicao+8*i, posicao+791-8*i, posicao+591-8*i, 255-cor, 0, 0);
			cor++;
		}
		cor=0;
		// Desenha as linhas para os retângulos azuis
		for(int i=0;i<37;i++)
		{
			C2D2P_Linha(4+8*i, 2+posicao+8*i, 4+791-8*i, 2+posicao+8*i, 0, 0, 255-cor);
			C2D2P_Linha(4+8*i, 2+posicao+591-8*i, 4+791-8*i, 2+posicao+591-8*i, 0, 0, 255-cor);
			C2D2P_Linha(4+8*i, 2+posicao+8*i, 4+8*i, 2+posicao+591-8*i, 0, 0, 255-cor);
			C2D2P_Linha(4+791-8*i, 2+posicao+8*i, 4+791-8*i, 2+posicao+591-8*i, 0, 0, 255-cor);
			cor++;
		}

		C2D2_DesenhaTexto(fsistema, 400, 500, "296 linhas.", C2D2_TEXTO_CENTRALIZADO);
		tempo=RelogioDigital(692,552);
		C2D2_Sincroniza(C2D2_FPS_PADRAO);
	}
	while(tempo-ttotal < 27000 && !teclado[C2D2_ESC].pressionado && !teclado[C2D2_ENCERRA].pressionado);
	// Adiciona ao tempo total o tempo passado nesta parte
	ttotal+=27000;
}

// Função demonstrando o básico de sprites
void Sprites()
{
	C2D2_Botao *teclado = C2D2_PegaTeclas();
	unsigned long int tempo;
	C2D2_TrocaCorLimpezaTela(0,0,0);
	int posicao=0;
	int direcao=1;
	do
	{
		// Atualiza a posição
		posicao+=direcao;
		if(posicao==7 || posicao==0)
			direcao*=-1;
		C2D2_LimpaTela();

		// DEsenha os sprites da parte A
		for(int i=0;i<25;i++)
			for(int j=0;j<10;j++)
			{
				C2D2_DesenhaSprite(sprite1, (i+j)%24, posicao+32*i, 0+64*j);
				C2D2_DesenhaSprite(sprite1, (i+j)%24, -posicao+32*i, 0+64*j);
				C2D2_DesenhaSprite(sprite1, (i+j)%24, 32*i, posicao+64*j);
				C2D2_DesenhaSprite(sprite1, (i+j)%24, 32*i, -posicao+64*j);
			}
		// DEsenha os sprites da parte B
		for(int i=0;i<27;i++)
			for(int j=0;j<12;j++)
			{
				C2D2_DesenhaSprite(sprite1, (i+j+4)%24, -16+posicao+32*i, -32+64*j);
				C2D2_DesenhaSprite(sprite1, (i+j+4)%24, -16-posicao+32*i, -32+64*j);
				C2D2_DesenhaSprite(sprite1, (i+j+4)%24, -16+32*i, -32+posicao+64*j);
				C2D2_DesenhaSprite(sprite1, (i+j+4)%24, -16+32*i, -32-posicao+64*j);
			}

		C2D2_DesenhaTexto(fsistema, 400, 500, "2000 sprites.", C2D2_TEXTO_CENTRALIZADO);
		tempo=RelogioDigital(692,552);
		C2D2_Sincroniza(C2D2_FPS_PADRAO);
	}
	while(tempo-ttotal < 15000 && !teclado[C2D2_ESC].pressionado && !teclado[C2D2_ENCERRA].pressionado);
	do
	{
		// Atualiza a posição
		posicao+=direcao;
		if(posicao==7 || posicao==0)
			direcao*=-1;
		C2D2_LimpaTela();
		// Os vetores de pontos
		int x[4], y[4];
		// DEsenha os sprites da parte A
		for(int i=0;i<25;i++)
			for(int j=0;j<10;j++)
			{
				x[0]=x[3] = posicao+32*i;
				x[1]=x[2] = posicao+32*i+32;
				y[0]=y[1] = 0+64*j;
				y[2]=y[3] = 0+64*j+64;
				C2D2_DesenhaSpriteEfeito(sprite1, (i+j)%24, x, y, 255, 255, 255, 255);
				x[0]=x[3] = -posicao+32*i;
				x[1]=x[2] = -posicao+32*i+32;
				y[0]=y[1] = 0+64*j;
				y[2]=y[3] = 0+64*j+64;
				C2D2_DesenhaSpriteEfeito(sprite1, (i+j)%24, x, y, 255, 0, 0, 255);
				x[0]=x[3] = 32*i;
				x[1]=x[2] = 32*i+32;
				y[0]=y[1] = posicao+64*j;
				y[2]=y[3] = posicao+64*j+64;
				C2D2_DesenhaSpriteEfeito(sprite1, (i+j)%24, x, y, 0, 255, 0, 255);
				x[0]=x[3] = 32*i;
				x[1]=x[2] = 32*i+32;
				y[0]=y[1] = -posicao+64*j;
				y[2]=y[3] = -posicao+64*j+64;
				C2D2_DesenhaSpriteEfeito(sprite1, (i+j)%24, x, y, 0, 0, 255, 255);
			}
		// DEsenha os sprites da parte B
		for(int i=0;i<27;i++)
			for(int j=0;j<12;j++)
			{
				x[0]=x[3] = -16+posicao+32*i;
				x[1]=x[2] = -16+posicao+32*i+32;
				y[0]=y[1] = -32+0+64*j;
				y[2]=y[3] = -32+0+64*j+64;
				C2D2_DesenhaSpriteEfeito(sprite1, (i+j)%24, x, y, 255, 255, 255, 127);
				x[0]=x[3] = -16-posicao+32*i;
				x[1]=x[2] = -16-posicao+32*i+32;
				y[0]=y[1] = -32+0+64*j;
				y[2]=y[3] = -32+0+64*j+64;
				C2D2_DesenhaSpriteEfeito(sprite1, (i+j)%24, x, y, 255, 0, 0, 127);
				x[0]=x[3] = -16+32*i;
				x[1]=x[2] = -16+32*i+32;
				y[0]=y[1] = -32+posicao+64*j;
				y[2]=y[3] = -32+posicao+64*j+64;
				C2D2_DesenhaSpriteEfeito(sprite1, (i+j)%24, x, y, 0, 255, 0, 127);
				x[0]=x[3] = -16+32*i;
				x[1]=x[2] = -16+32*i+32;
				y[0]=y[1] = -32-posicao+64*j;
				y[2]=y[3] = -32-posicao+64*j+64;
				C2D2_DesenhaSpriteEfeito(sprite1, (i+j)%24, x, y, 0, 0, 255, 127);
			}

		C2D2_DesenhaTexto(fsistema, 400, 500, "Efeito de cor e alpha.", C2D2_TEXTO_CENTRALIZADO);
		tempo=RelogioDigital(692,552);
		C2D2_Sincroniza(C2D2_FPS_PADRAO);
	}
	while(tempo-ttotal < 30000 && !teclado[C2D2_ESC].pressionado && !teclado[C2D2_ENCERRA].pressionado);

	// Adiciona ao tempo total o tempo passado nesta parte
	ttotal+=30000;
}

void Estrela()
{
	C2D2_Botao *teclado = C2D2_PegaTeclas();
	unsigned long int tempo;
	int angulo=0;
	int cor=0, dcor=5, zoom=100, dzoom=5;
	C2D2_TrocaCorLimpezaTela(0,0,0);
	// Os vetores de pontos
	int x[4], y[4];
	int estado = 0;
	do
	{
		// calcula os pontos da estrela
		x[0] = 400+(181.0*zoom/100)*cos((angulo+45)*RAD);
		x[1] = 400+(181.0*zoom/100)*cos((angulo+135)*RAD);
		x[2] = 400+(181.0*zoom/100)*cos((angulo+225)*RAD);
		x[3] = 400+(181.0*zoom/100)*cos((angulo+315)*RAD);
		y[0] = 300-(181.0*zoom/100)*sin((angulo+45)*RAD);
		y[1] = 300-(181.0*zoom/100)*sin((angulo+135)*RAD);
		y[2] = 300-(181.0*zoom/100)*sin((angulo+225)*RAD);
		y[3] = 300-(181.0*zoom/100)*sin((angulo+315)*RAD);
		// Calcula a cor
		cor+=dcor;
		if(cor == 0 || cor == 255)
			dcor*=-1;
		// Atualiza o angulo
		angulo=(angulo+2)%360;
		// Calcula o zoom se necessário
		if(estado!=0)
		{
			zoom+=dzoom;
			if(zoom==20 || zoom==200)
				dzoom*=-1;
		}
		// DEsenha a estrela
		C2D2_LimpaTela();
		C2D2_DesenhaSpriteEfeito(sprite2, 0, x, y, 255, 255, cor, 255);
		if(estado==0)
			C2D2_DesenhaTexto(fsistema, 400, 500, "Rotação + cor.", C2D2_TEXTO_CENTRALIZADO);
		else
			C2D2_DesenhaTexto(fsistema, 400, 500, "Rotação+zoom+cor.", C2D2_TEXTO_CENTRALIZADO);
		tempo=RelogioDigital(692,552);
		if(tempo-ttotal>10000)
			estado=1;
		C2D2_Sincroniza(C2D2_FPS_PADRAO);
	}
	while(tempo-ttotal < 20000 && !teclado[C2D2_ESC].pressionado && !teclado[C2D2_ENCERRA].pressionado);
	// Os pontos das 3 estrelas
	int xs[3][4];
	int ys[3][4];
	// O centro das estrelas
	int cx[3]={300, 400, 500};
	int cy[3]={400, 200, 400};
	do
	{
		// calcula os pontos da estrela
		for(int i=0;i<3;i++)
		{
			xs[i][0] = cx[i]+(181.0*zoom/100)*cos((angulo+45)*RAD);
			xs[i][1] = cx[i]+(181.0*zoom/100)*cos((angulo+135)*RAD);
			xs[i][2] = cx[i]+(181.0*zoom/100)*cos((angulo+225)*RAD);
			xs[i][3] = cx[i]+(181.0*zoom/100)*cos((angulo+315)*RAD);
			ys[i][0] = cy[i]-(181.0*zoom/100)*sin((angulo+45)*RAD);
			ys[i][1] = cy[i]-(181.0*zoom/100)*sin((angulo+135)*RAD);
			ys[i][2] = cy[i]-(181.0*zoom/100)*sin((angulo+225)*RAD);
			ys[i][3] = cy[i]-(181.0*zoom/100)*sin((angulo+315)*RAD);
		}
		// Calcula a cor
		cor+=dcor;
		if(cor == 0 || cor == 255)
			dcor*=-1;
		// Atualiza o angulo
		angulo=(angulo+2)%360;
		// Calcula o zoom se necessário
		zoom+=dzoom;
		if(zoom==20 || zoom==300)
			dzoom*=-1;
		// DEsenha as estrela
		C2D2_LimpaTela();
		C2D2_DesenhaSpriteEfeito(sprite2, 0, xs[0], ys[0], cor, 255, 255, 192);
		C2D2_DesenhaSpriteEfeito(sprite2, 0, xs[1], ys[1], 255, cor, 255, 192);
		C2D2_DesenhaSpriteEfeito(sprite2, 0, xs[2], ys[2], 255, 255, cor, 192);
		C2D2_DesenhaTexto(fsistema, 400, 500, "Tudo!", C2D2_TEXTO_CENTRALIZADO);
		tempo=RelogioDigital(692,552);
		if(tempo-ttotal>10000)
			estado=1;
		C2D2_Sincroniza(C2D2_FPS_PADRAO);
	}
	while(tempo-ttotal < 33000 && !teclado[C2D2_ESC].pressionado && !teclado[C2D2_ENCERRA].pressionado);

	ttotal += 33000;
}

void Creditos()
{
	C2D2_Botao *teclado = C2D2_PegaTeclas();
	unsigned long int tempo=ttotal;
	C2D2_TrocaCorLimpezaTela(128,128,128);
	int deslocamento;
	// Desenha as retas no fundo
	int x, y, angulo=45;
	do
	{
		// Calcula a coordenada x,y da reta (diagonal tamanho 396, para círculo de raio 280)
		x = 280*cos(angulo*RAD);
		y = 280*sin(angulo*RAD);
		angulo=(angulo+2)%360;
		// Atualiza o deslocamento
		deslocamento = 600 - (1612.0*(tempo-ttotal))/11700;
		C2D2_LimpaTela();
		// Desenha as retas
		C2D2P_Linha(400+x, 300-y, 400-x, 300+y, 0, 0, 128);
		C2D2P_Linha(400-x, 300-y, 400+x, 300+y, 0, 0, 128);
		C2D2_DesenhaTexto(fpequeno, 400, deslocamento, "Programação e Gráficos", C2D2_TEXTO_CENTRALIZADO);
		C2D2_DesenhaTexto(fsistema, 400, deslocamento+32, "Paulo V. W. Radtke", C2D2_TEXTO_CENTRALIZADO);
		C2D2_DesenhaTexto(fpequeno, 400, deslocamento+196, "Música", C2D2_TEXTO_CENTRALIZADO);
		C2D2_DesenhaTexto(fsistema, 400, deslocamento+228, "2nd Reality", C2D2_TEXTO_CENTRALIZADO);
		C2D2_DesenhaTexto(fpequeno, 400, deslocamento+292, "por Purple Motion", C2D2_TEXTO_CENTRALIZADO);
		C2D2_DesenhaTexto(fpequeno, 400, deslocamento+424, "Agradecimentos", C2D2_TEXTO_CENTRALIZADO);
		C2D2_DesenhaTexto(fsistema, 400, deslocamento+456, "BCS", C2D2_TEXTO_CENTRALIZADO);
		C2D2_DesenhaTexto(fsistema, 400, deslocamento+520, "Mel", C2D2_TEXTO_CENTRALIZADO);
		C2D2_DesenhaTexto(fsistema, 400, deslocamento+584, "Pio", C2D2_TEXTO_CENTRALIZADO);
		C2D2_DesenhaTexto(fpequeno, 400, deslocamento+748, "Copywrong 2008", C2D2_TEXTO_CENTRALIZADO);
		C2D2_DesenhaTexto(fpequeno, 400, deslocamento+780, "Quase nenhum direito reservado.", C2D2_TEXTO_CENTRALIZADO);
		// termina em 812. 
		//tem que pular 600 pontos no começo, mais 100 de lambuja pra cada lado. total de 1612 pontos
		tempo=RelogioDigital(692,552);
		C2D2_Sincroniza(C2D2_FPS_PADRAO);
	}
	while(tempo-ttotal < 11700 && !teclado[C2D2_ESC].pressionado && !teclado[C2D2_ENCERRA].pressionado);
	// Apaga a tela com barras pretas
	deslocamento=0;
	do
	{
		// Calcula o quanto deceu na tela
		deslocamento = (300.0*(tempo-ttotal-11700))/500;
		// Calcula a coordenada x,y da reta (diagonal tamanho 396, para círculo de raio 280)
		x = 280*cos(angulo*RAD);
		y = 280*sin(angulo*RAD);
		angulo=(angulo+2)%360;
		C2D2_LimpaTela();
		// Desenha as retas
		C2D2P_Linha(400+x, 300-y, 400-x, 300+y, 0, 0, 128);
		C2D2P_Linha(400-x, 300-y, 400+x, 300+y, 0, 0, 128);
		C2D2P_RetanguloPintado(0, 0, 799, deslocamento, 0, 0, 0);
		C2D2P_RetanguloPintado(0, 600-deslocamento, 799, 599, 0, 0, 0);
		tempo=RelogioDigital(692,552);
		C2D2_Sincroniza(C2D2_FPS_PADRAO);
	}
	while(tempo-ttotal < 12200 && !teclado[C2D2_ESC].pressionado && !teclado[C2D2_ENCERRA].pressionado);


	ttotal += 12200;	
}

int main(int ac, char **av)
{
	if(!C2D2_Inicia(800, 600, C2D2_TELA_CHEIA, C2D2_DESENHO_OPENGL, "Chien 2D v2.0 Megademo"))
	{
		printf("Nao conseguiu iniciar a tela. Encerrando.\n");
		return 1;
	}
	// Inicia o áudio
	CA2_Inicia();
	// Inicia o desenho de primitivas
	C2D2P_Inicia();
	// Obtém o teclado
	C2D2_Botao *teclado = C2D2_PegaTeclas();
	// Indica se deve encerrar o demo
	bool encerra = false;
	// Indica o estado do demo
	int estado = 0;
	// Carrega os recursos
	if(Carrega())
	{
		// Toca a música
		CA2_TocaMusica(mfundo, 1);
		while(!encerra && !teclado[C2D2_ESC].pressionado && !teclado[C2D2_ENCERRA].pressionado)
		{
			switch(estado)
			{
			case 0:
				Intro();
				estado++;
				break;
			case 1:
				Primitivas();
				estado++;
				break;
			case 2:
				Sprites();
				estado++;
				break;
			case 3:
				Estrela();
				estado++;
				break;
			case 4:
				Creditos();
				estado++;
				break;
			case 5:
				C2D2_TrocaCorLimpezaTela(0,0,0);
				C2D2_LimpaTela();
				if(RelogioDigital(692,552) > 119000)
					estado++;
				C2D2_Sincroniza(C2D2_FPS_PADRAO);
				break;
			default:
				encerra=true;
			}
			
		}
		CA2_PausaMusica();
		// DEscarrega os recursos
		Descarrega();
	}
	else
		printf("Opa! Falhou algo. Encerrando.\n");
	// Encerra tudo
	CA2_Encerra();
	C2D2_Encerra();
	return 0;
}
