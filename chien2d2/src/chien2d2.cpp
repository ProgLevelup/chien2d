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

/******************************

Modificações:

	12/08/2010 - Inclusão da função para definir o callback de sincronização do usuário.
		Inclui a definição do ponteiro de função e da função que o preenche. O callback
		deve ser invocado na função específica de sincronização do renderer específico.
		Responsável: Paulo V. W. Radtke

*******************************/

#include <math.h>
#include <c2d2/chien2d2.h>
#include <c2d2/chien2d2_interno.h>

#include <assert.h>

// Função usada para validar os sprites
static int ValidaSprite(unsigned int id, unsigned int indice);

// Função para recuperar o tempo do sistema
//
// Data: 30/07/2008
//
unsigned long C2D2_TempoSistema()
{
	return SDL_GetTicks();
}

// Área de variáveis globais da Chien2D v2.0

// indica se a Chien2D 2 já foi inicializada. Por default, não foi ainda.
bool inicializado;
// A tela principal
SDL_Surface *screen;
// O vetor com os sprite sets
C2D2_SpriteSet sprites[C2D2_MAX_SPRITESET];
// Finalmente, o vetor com as fontes
C2D2_Fonte fontes[C2D2_MAX_FONTES];
// Indica se o sistema está em shutdown ou não
bool c_shutdown = false;
// O vetor de teclas utilizadas
C2D2_Botao teclas[C2D2_MAXTECLAS];
// O mouse do sistema
C2D2_Mouse mouse;
// Variável do Joystick
static C2D2_Joystick joysticks[C2D2_MAX_JOYSTICKS] = {0};
// indica a cor para limpar a tela, incluindo o alpha
GLubyte limpaR, limpaG, limpaB, limpaA;        
// Variáveis relativas e efeitos da versão OpenGL
bool	faz_blend;
bool	texturiza;
GLuint	textura;
GLenum	blendOrigem, blendDestino;
GLenum  wrapTexturaS, wrapTexturaT;
GLenum  filtroMax, filtroMin;
GLint maxtexsize;

//Uint32 rmask, gmask, bmask, amask;
#if SDL_BYTEORDER == SDL_BIG_ENDIAN
	Uint32 rmask = 0xff000000;
    Uint32 gmask = 0x00ff0000;
    Uint32 bmask = 0x0000ff00;
    Uint32 amask = 0x000000ff;
#else
	Uint32 rmask = 0x000000ff;
    Uint32 gmask = 0x0000ff00;
    Uint32 bmask = 0x00ff0000;
    Uint32 amask = 0xff000000;
#endif


// Ponteiro de função de sincronização do jogo do usuário
void (*C2D2_SincronizaUsuario)()=0;

// Função que inicia a Chien2D 2 de acordo com o tipo do render.
//
// Data: 10/06/2008
//
bool C2D2_Inicia(unsigned int largura, unsigned int altura, int modoVideo, int tipoRender, char *tituloJanela)
{
    if(inicializado)
        return false; 
	else
		reset();
	// Indica que não está em shutdown
	c_shutdown = false;
    // Testa se já foi inicializado o vídeo (se foi, cai fora!)
    if(SDL_WasInit(SDL_INIT_VIDEO))
        return false;
    // Inicia o vídeo
	if ( SDL_InitSubSystem(SDL_INIT_VIDEO | SDL_INIT_JOYSTICK) < 0 ) 
    {
		printf("A Chien2D 2 não conseguiu iniciar a SDL. Mensagem do sistema: %s\n", 
                SDL_GetError());
        return false;
    }

    // Imprime informação do modo de vídeo requisitado
    printf("Iniciando o modo de vídeo (OpenGL): %ix%ix32bpp\n", largura, altura);
    
    // Flags do modo de vídeo (se ficar assim, roda em janela)
    Uint32 flags = SDL_OPENGL; // | SDL_GL_DOUBLEBUFFER;
    // Se for rodar em tela cheia, tem que colocar um flag a mair
	switch(modoVideo)
	{
	case C2D2_JANELA:
		printf("A aplicação requisitou um modo de vídeo em janela.\n");
		break;
	case C2D2_TELA_CHEIA:
		printf("A aplicação requisitou um modo de vídeo em tela cheia.\n");
		flags |= SDL_FULLSCREEN;
		break;
	default:
		printf("A aplicação requisitou um modo de vídeo inválido. Por segurança, será utilizado um modo em janela.\n");
		break;
	}

	printf("A Chien2DLite vai verificar a disponibilidade do modo de vídeo requisitado.\n");
    
    if(SDL_VideoModeOK(largura, altura, 32, flags) ) 
    {
        printf("O sistema indica que o modo é suportado. Tenta iniciar.\n"); 
        // Indica os atributos da OpenGL
        SDL_GL_SetAttribute( SDL_GL_RED_SIZE, 8 );
        SDL_GL_SetAttribute( SDL_GL_GREEN_SIZE, 8 );
        SDL_GL_SetAttribute( SDL_GL_BLUE_SIZE, 8 );
        SDL_GL_SetAttribute( SDL_GL_ALPHA_SIZE, 8 );        
        SDL_GL_SetAttribute( SDL_GL_DEPTH_SIZE, 16 );
        SDL_GL_SetAttribute( SDL_GL_DOUBLEBUFFER, 1 );  
		SDL_GL_SetAttribute( SDL_GL_SWAP_CONTROL, 1 );
        // Inicia o modo de vídeo e recupera a surface (diferente de 0 se tudo ok)
        screen = SDL_SetVideoMode(largura, altura, 32,  flags);
        if ( screen == 0 ) 
        {
            printf("Não foi possível iniciar o modo de vídeo requisitado. Encerrando.");
            SDL_QuitSubSystem(SDL_INIT_VIDEO);
            return false;
        }
        // Testa se conseguiu colocar o modo de vídeo como double buffer
        // Não é um erro grave, mas para melhores resultados, queremos o double buffer
        int atributo;
        SDL_GL_GetAttribute(SDL_GL_DOUBLEBUFFER,&atributo);
        if(atributo != 0)
			printf("A aplicação está utilizando double buffer como esperado.\n");
        else
			printf("ATENÇÃO: a aplicação não está usando double buffer. Para melhores resultados, utilize um hardware que o suporte.\n");
    }
    else
    {
        printf("O modo de vídeo não é suportado pelo hardware. Encerrando.");; 
		SDL_QuitSubSystem(SDL_INIT_VIDEO);
        return false;
    }
    printf("Modo de vídeo iniciado com sucesso!\n");
    // Coloca o nome na janela
    SDL_WM_SetCaption(tituloJanela, 0);

    // Prepara a visão do sistema
	glDisable(GL_DEPTH_TEST);
	glDisable(GL_CULL_FACE);

	glViewport(0, 0, largura, altura);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glOrtho(0, largura, altura, 0, -1.0, 1.0);
	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glTranslatef(0.0f, 0.0f, 0.0f);
	
	// Limpa a tela
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
    resetaGL();
    glDisable(GL_BLEND);    
    glDisable(GL_TEXTURE_2D);  
    glLoadIdentity();			
    SDL_GL_SwapBuffers();
    // Testa o tamanho da textura
	glGetIntegerv(GL_MAX_TEXTURE_SIZE, &maxtexsize);
	
    printf("O tamanho máximo de textura suportado é de %ix%i pixels.\n", maxtexsize, maxtexsize);
    // Associa a cor pra limpar a tela padrão
    limpaR=0;
    limpaG=0;
    limpaB=0;
    limpaA=0;  

    // Desabilita o cursor do mouse
    SDL_ShowCursor(SDL_DISABLE);
  
    // Indica que foi iniciado e pula fora
    inicializado=true;
    return true;
}

// função que indica as dimensões da tela
//
// Data: 26/01/2011
//
bool C2D2_DimensoesTela(int *largura, int *altura)
{
	if(inicializado)
	{
		*largura = screen->w;
		*altura = screen->h;
		return true;
	}
	else
		return false;
}

// Função que encerra a Chien2D 2
//
// Data: 29/03/2007

void C2D2_Encerra()
{
    // Testa por via das dúvidas para não ter problemas
    if(!inicializado)
        return;
    // Indica que está encerrando
    c_shutdown = true;
        
    printf("Apagando os spritesets e fontes do sistema.\n");

    // Apaga as fontes
    for(int i=0;i<C2D2_MAX_FONTES;i++)
        C2D2_RemoveFonte(i+1);

    // Apaga os sprite sets
    for(int i=0;i<C2D2_MAX_SPRITESET;i++)
        C2D2_RemoveSpriteSet(i+1);

    printf("Encerrando a SDL.\n");
    // Encerra o vídeo
	SDL_FreeSurface(screen);
	SDL_QuitSubSystem(SDL_INIT_VIDEO);
	printf("Encerrou a SDL+OpenGL");

    inicializado=false;
}

// Função para sincronizar o vídeo a 60ppfps
//
// Data: 13/04/2007
//
// Alteração:
//	12/08/2010	Paulo Radtke	Chamada do callback do usuário.
//
void C2D2_Sincroniza(Uint8 fps)
{
    // Inicializa e pega o tempo atual (só faz isso a primeira vez)
    static Uint32 tempoAnterior=0;
    // Chama a função de sincronização do usuário
    if(C2D2_SincronizaUsuario != 0)
        C2D2_SincronizaUsuario();

	// Verifica se o parâmetro é válido
	if (fps <= 0) fps = 1;

    // -- Espera 16ms entre cada quadro --	
    // Variável para o tempo atual
    int espera=(1000/fps)-SDL_GetTicks()+tempoAnterior;
    if(espera > 0)
       SDL_Delay(espera);
    tempoAnterior=SDL_GetTicks();


    // Troca os buffers
    SDL_GL_SwapBuffers();
    // Atualiza as entradas
    C2D2_Entrada();
}

// Função para limpar a tela
//
// Data: 13/04/2007
//
void C2D2_LimpaTela()
{
    // Limpa a tela e buffer de profundidade
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
    // Reseta o estado da OpenGL
    resetaGL();
    // Carrega a matriz identidade
    glLoadIdentity();

    // Indica que não quer usar textura
    setaTexturizacao(false);
    // Desenha um quad OpenGL
    glBegin(GL_QUADS);		       
	glColor4ub((GLubyte)limpaR, (GLubyte)limpaG, (GLubyte)limpaB, 255);
    // Coordenada do vértice na tela
    glVertex2i(0, 0);
    // E assim por diante
	glVertex2i( screen->w, 0);
    glVertex2i( screen->w, screen->h);
    glVertex2i( 0, screen->h);
    // Encerra o quad
    glEnd();
}

// Função para escolher a cor de limpeza da tela
//
// Data: 13/04/2007
//
void C2D2_TrocaCorLimpezaTela(Uint8 r, Uint8 g, Uint8 b)
{
	limpaR=r;	
	limpaG=g;
	limpaB=b;
}

// Procura um spriteset pelo nome do arquivo original
//
// Data: 13/04/2007
//
unsigned int C2D2_ProcuraSpriteSet(const char *apelido)
{
    // Índice de spriteset inválido, caso não encontre
    unsigned int idx=0;
    for(int i=0;i<C2D2_MAX_SPRITESET;i++)
        if(strcmp(sprites[i].apelido, apelido)==0)
        {
            // O índice é a posição atual + 1
            idx=i+1;
            // Encerra a busca
            break;
        }
    return idx;
}

// Funções para manipular sprites

// função para carregar um sprite set na memória
//
// Data: 13/04/2007
//
unsigned int C2D2_CarregaSpriteSet(const char *arquivo, int largura, int altura)
{
    // Verifica se o spriteset não existe já
    unsigned int idx = C2D2_ProcuraSpriteSet(arquivo);
    // Se já existe um sprite com o nome do arquivo, retorna o índice associado
    if(idx != 0)
    {
		sprites[idx-1].usuarios=sprites[idx-1].usuarios+1;
        return idx;
    }
        
    // Se não existe, procura o primeiro índice vago (idx ainda é igual a 0)
    for(int i=0;i<C2D2_MAX_SPRITESET;i++)
        // O spriteset é vago (não tem imagem associada?
        if(sprites[i].imagem == 0)
        {
            idx=i+1;
            break;
        }
 
    // Testa se ainda tem espaço na lista
    //
    // ATENÇÃO: não ter espaço na memória não é relacionado a este teste. Apenas
    // testa-se aqui se existe um índice vago na lista. Se não existe, idx==0
    if(idx==0)
        // Retorna um índice inválido para indicar que a função falhou
        return 0;
    
    // Se chegou até aqui, idx contém o identificador correto 
    // Calcula a posição no array (sem incrementar 1)
    idx-=1;    
    // Indica o caminho do arquivo no apelido
    strcpy(sprites[idx].apelido,arquivo);

    // Carrega a imagem no spriteset
    sprites[idx].imagem = IMG_Load(arquivo);

    // Testa: conseguiu abrir a imagem? Se deu erro, retorna spriteset inválido
    if (sprites[idx].imagem == 0) 
    {
		printf("A função C2D2_CarregaSPriteset falhou ao carregar o arquivo %s. Erro: %s.\n",
                arquivo, SDL_GetError());
        sprites[idx].apelido[0] = '\0';
        return 0;
    }
	// Indica o tamanho da imagem carregada
	sprites[idx].largura = sprites[idx].imagem->w;
	sprites[idx].altura = sprites[idx].imagem->h;
	// Verifica se pode usar como imagem dos sprites
	if(largura!=0 && altura!=0)
	{
		if(sprites[idx].largura%largura!=0 && sprites[idx].altura%altura!=0)
		{
			printf("A imagem %s não pode ser usada como fonte de sprites de tamanho %ix%i\n.",
				arquivo, largura, altura);
	        SDL_FreeSurface(sprites[idx].imagem);
			sprites[idx].imagem = 0;
			sprites[idx].apelido[0] = '\0';
			return 0;
		}
		else
		{
			sprites[idx].spLargura=largura;
			sprites[idx].spAltura=altura;
			sprites[idx].matrizX = sprites[idx].imagem->w/largura;
			sprites[idx].matrizY = sprites[idx].imagem->h/altura;
		}
	}
	else
	{
		sprites[idx].spLargura=sprites[idx].imagem->w;
		sprites[idx].spAltura=sprites[idx].imagem->h;
		sprites[idx].matrizX = 1;
		sprites[idx].matrizY = 1;
	}
	// Surface temporária. Será eliminada posteriormente se utilizada
	SDL_Surface *temp = 0;
	// Testa se uma das dimensões (qualquer uma) é potência de 2
    bool potencia=false;
    // Testamos de potência de 4 (16) até potência de 10 (1024);
    for(int i=4;i<=10;i++)
		if(pow(2.0,i)==sprites[idx].imagem->w)
        {
            potencia=true;
            break;
        }
	// Parte OpenGL. Testa se a imagem é quadrada e potência de 2. 
	// MUITO provável que não seja
	if(sprites[idx].imagem->w != sprites[idx].imagem->h || !potencia || sprites[idx].imagem->format->BytesPerPixel!=4)
	{
		// Guarda, temporariamente, a surface antiga em temp (para extrair as bitmasks)
		temp = sprites[idx].imagem;
		// DEtermina a dimensão da imagem a criar
		int maior;
		if(sprites[idx].imagem->w > sprites[idx].imagem->h)
			maior = sprites[idx].imagem->w;
		else
			maior = sprites[idx].imagem->h;
		// A imagem pode ser armazenada numa textura?
		if(maior > maxtexsize)
		{
	        SDL_FreeSurface(sprites[idx].imagem);
			sprites[idx].imagem = 0;
			sprites[idx].apelido[0] = '\0';
			printf("A imagem excede o limite de %ix%i para uma textura OpenGL no sistema.\n", maxtexsize, maxtexsize);
			return 0;
		}
		// Qual a potencia da imagem a criar?
		int pot=2;
		// Determina qual a melhor dimensão para a imagem
		while((int)pow(2.0,pot) < maior)
			pot++;
		// Indica o tamanho da textura
		sprites[idx].larguraTextura = sprites[idx].alturaTextura = (int)pow(2.0,pot);
		// Cria a nova imagem (a antiga está preservada em temp)
		sprites[idx].imagem = SDL_CreateRGBSurface(SDL_SWSURFACE, 
			sprites[idx].larguraTextura, sprites[idx].alturaTextura, 
            32, rmask, gmask, bmask, amask);
        // Testa se conseguiu criar a surface RGBA
        if(sprites[idx].imagem == 0) 
        {
			printf("Não conseguiu criar a imagem temporária ao carregar o sprite set. Arquivo: %s. Erro: %s.\n", 
				arquivo, SDL_GetError());
	        SDL_FreeSurface(sprites[idx].imagem);
			sprites[idx].imagem = 0;
			sprites[idx].apelido[0] = '\0';
            return 0;
        }
        // Copia a imagem original na nova imagem 32bpp
		if(temp->format->BytesPerPixel != 4)
			SDL_BlitSurface(temp, 0, sprites[idx].imagem, 0);
		else
		{
			for(int i=0;i<temp->h;i++)
			{
				memcpy(&((Uint8*)sprites[idx].imagem->pixels)[sprites[idx].imagem->pitch*i],
					&((Uint8*)temp->pixels)[temp->pitch*i], temp->pitch);
			}
		}
		// Apaga a surface temporária
		SDL_FreeSurface(temp);
	}
	
	// Recupera o tamanho da textura
	sprites[idx].larguraTextura=sprites[idx].imagem->w;
	sprites[idx].alturaTextura=sprites[idx].imagem->h;

	// Converte os pontos magenta da imagem em transparentes
	MagentaParaPontoTransparente(sprites[idx].imagem);
	

	sprites[idx].bmask = C2D2_CriaVetorBitMasks(sprites[idx].imagem, sprites[idx].spLargura, sprites[idx].spAltura, sprites[idx].largura, sprites[idx].altura);
	// Se falhou, deve indicar isto
    if(sprites[idx].bmask == 0) 
    {
		printf("Não conseguiu criar o vetor de bitmasks ao carregar os sprites do arquivo %s.\n", arquivo);
        SDL_FreeSurface(sprites[idx].imagem);
		sprites[idx].imagem = 0;
		sprites[idx].apelido[0] = '\0';
        return 0;
    }   

	C2D2_PreencheVetorBitMasks(sprites[idx].imagem, sprites[idx].spLargura, sprites[idx].spAltura, sprites[idx].largura, sprites[idx].altura, sprites[idx].bmask);	
	#ifdef DDEBUG
		// Faz o trace da máscara
		char saida[100];

		sprintf(saida, "%s.txt", arquivo);
		FILE *trace = fopen(saida, "wt");
		if(saida != 0)
		{
			for(int i=0;i<sprites[idx].matrizX*sprites[idx].matrizY;i++)
			{
				C2D2_BitMask *mascara = sprites[idx].bmask[i];
				for(int l=0;l<sprites[idx].spAltura;l++)
				{
					for(int c=0;c<sprites[idx].spLargura;c++)
					{
						if(C2D2_GetBit(mascara, c, l))
							fprintf(trace, "#");
						else
							fprintf(trace, " ");
					}
					fprintf(trace, "\n");
				}
			}
			fclose(trace);
		}
	#endif
	// Indica que tem um usuário
    sprites[idx].usuarios = 1;
	// Se temp foi usado, apaga
	// Liga a textura
    
	// Cria uma textura
	glGenTextures(1, &(sprites[idx].textura));
    // Associa a textura    
    glBindTexture(GL_TEXTURE_2D, sprites[idx].textura);   // 2d texture (x and y size)

    // Deve cobrir a textura até a borda??    
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
    // Faz escala linear
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR); // scale linearly when image bigger than texture
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR); // scale linearly when image smalled than texture

    // Cria textura 2D, detalhe normal (0), 4 componentes (RGBA), a partir
    // da largura e altura da imagem,  sem borda, dados em RGBA, dados em
    // bytes não sianlizados e, finalmente, os dados em si
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, sprites[idx].imagem->w, 
		sprites[idx].imagem->h, 0, GL_RGBA, GL_UNSIGNED_BYTE, 
        sprites[idx].imagem->pixels);
	sprites[idx].texturaLigada=true;

    // idx+1 indica o identificador. Retorna e encerra (ufa!)
    return idx+1;
}

// Remove um sprite set da memória
//
// Data: 13/04/2007
//
void C2D2_RemoveSpriteSet(unsigned int id)
{
    // O identificador é válido?
    if(id > C2D2_MAX_SPRITESET || id == 0)
        return;
    // Só apaga se o número de usuários for um ou se estiver em shutdown
    if(sprites[id-1].usuarios > 1 && !c_shutdown)
    {
        sprites[id-1].usuarios -= 1;
        return;
    }
    // Se cair aqui, é porquê não tem mais usuários
    sprites[id-1].usuarios = 0;
	// tem a textura devidamente associada?
	if(sprites[id-1].texturaLigada)
	{
		if(sprites[id-1].textura != 0)
		{
			// Remove the texture
			glDeleteTextures(1, &(sprites[id-1].textura));
			sprites[id-1].textura = 0;
		}
		sprites[id-1].texturaLigada=false;
	}
    // Tem uma surface SDL associada?
    if(sprites[id-1].imagem != 0)
    {
        // Apaga a surface
        SDL_FreeSurface(sprites[id-1].imagem);
        // Associa um valor nulo para reutilizar depois
        sprites[id-1].imagem = 0;
        // Tem que tirar o nome
        sprites[id-1].apelido[0] = '\0';
		// Remove a textura
		// Indica que a textura não existe mais
		sprites[id-1].textura=0;
		sprites[id-1].texturaLigada=false;

    }
    // Se tem máscara de colisão, apaga
	if(sprites[id-1].bmask != 0)
	{
		// Apaga as máscaras do vetor
		for(int i=0;i<sprites[id-1].matrizX*sprites[id-1].matrizY;i++)
			C2D2_RemoveBitMask(sprites[id-1].bmask[i]);
		// Apaga o vetor das máscaras     
		free(sprites[id-1].bmask);
        // Prepara para o próximo uso
        sprites[id-1].bmask=0;
    }
}

// Função para desenhar um sprite
//
// Data: 13/04/2007
//
bool C2D2_DesenhaSprite(unsigned int id, unsigned int indice, int x, int y)
{
	// O id é válido?
	if(id == 0)
		return false;
	int idx = id-1;
	// O índice é válido?
	if(indice >= (unsigned int)(sprites[idx].matrizX * sprites[idx].matrizY) || sprites[idx].textura==0)
		return false;
	// Tudo certo, calcula as coordenadas iniciais a desenhar na textura:
	int xImg = (indice%sprites[idx].matrizX)*sprites[idx].spLargura;
	int yImg = (indice/sprites[idx].matrizX)*sprites[idx].spAltura;
	// Calcula o ponto inicial da textura baseado no tamanho total desta
	double posx = (double)xImg/(double)(sprites[idx].larguraTextura);
	double posy = (double)yImg/(double)(sprites[idx].alturaTextura);
	// qual a largura (em porcentagem) na imagem
	double largura = (double)(sprites[idx].spLargura)/(double)(sprites[idx].larguraTextura);
	double altura = (double)(sprites[idx].spAltura)/(double)(sprites[idx].alturaTextura);
	// Faz o desenho em um quad OpenGL
    // Indica que quer usar textura
    setaTexturizacao(true);
    // Seleciona a textura a utilizar
	setaTextura(sprites[idx].textura);
    // Prepara o blend de imagens
    alteraBlend(true);
    modoBlend(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);
    // Indica como pega a subtextura e como faz a escala
    modoWrapTextura(GL_CLAMP, GL_CLAMP);
    modoFiltragem(GL_LINEAR, GL_LINEAR);
    // Desenha o sprite num quad OpenGL
    glBegin(GL_QUADS);		       
    glColor4ub(255, 255, 255, 255);
    // DEfine o primeiro vértice na textura
    glTexCoord2f((GLfloat)posx, (GLfloat)posy); 
    // Coordenada do vértice na tela
    glVertex2f((GLfloat)x, (GLfloat)y);
    // E assim por diante
    glTexCoord2f((GLfloat)(posx+largura), (GLfloat)(posy)); 
	glVertex2f( (GLfloat)(x+sprites[idx].spLargura), (GLfloat)y);
    glTexCoord2f((GLfloat)(posx+largura), (GLfloat)(posy+altura)); 
    glVertex2f( (GLfloat)(x+sprites[idx].spLargura), (GLfloat)(y+sprites[idx].spAltura));
    glTexCoord2f((GLfloat)posx, (GLfloat)(posy+altura)); 
    glVertex2f((GLfloat)x, (GLfloat)(y+sprites[idx].spAltura));
    // Encerra o quad
    glEnd();

	return true;
}

// Função para desenhar um sprite em ponto flutuante. Não funciona direito, só para
// demonostração da função correta, DesenhaSpriteSubpixel.
//
// Data: 13/04/2007
//
bool C2D2_DesenhaSpriteF(unsigned int id, unsigned int indice, float x, float y)
{
	// O id é válido?
	if(id == 0)
		return false;
	int idx = id-1;
	// O índice é válido?
	if(indice >= (unsigned int)(sprites[idx].matrizX * sprites[idx].matrizY) || sprites[idx].textura==0)
		return false;
	// Tudo certo, calcula as coordenadas iniciais a desenhar na textura:
	int xImg = (indice%sprites[idx].matrizX)*sprites[idx].spLargura;
	int yImg = (indice/sprites[idx].matrizX)*sprites[idx].spAltura;
	// Calcula o ponto inicial da textura baseado no tamanho total desta
	double posx = (double)xImg/(double)(sprites[idx].larguraTextura);
	double posy = (double)yImg/(double)(sprites[idx].alturaTextura);
	// qual a largura (em porcentagem) na imagem
	double largura = (double)(sprites[idx].spLargura)/(double)(sprites[idx].larguraTextura);
	double altura = (double)(sprites[idx].spAltura)/(double)(sprites[idx].alturaTextura);
	// Faz o desenho em um quad OpenGL
    // Indica que quer usar textura
    setaTexturizacao(true);
    // Seleciona a textura a utilizar
	setaTextura(sprites[idx].textura);
    // Prepara o blend de imagens
    alteraBlend(true);
    modoBlend(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);
    // Indica como pega a subtextura e como faz a escala
    modoWrapTextura(GL_CLAMP, GL_CLAMP);
    modoFiltragem(GL_LINEAR, GL_LINEAR);
    // Desenha o sprite num quad OpenGL
    glBegin(GL_QUADS);		       
    glColor4ub(255, 255, 255, 255);
    // DEfine o primeiro vértice na textura
    glTexCoord2f((GLfloat)posx, (GLfloat)posy); 
    // Coordenada do vértice na tela
    glVertex2f((GLfloat)x, (GLfloat)y);
    // E assim por diante
    glTexCoord2f((GLfloat)(posx+largura), (GLfloat)(posy)); 
	glVertex2f( (GLfloat)(x+sprites[idx].spLargura), (GLfloat)y);
    glTexCoord2f((GLfloat)(posx+largura), (GLfloat)(posy+altura)); 
    glVertex2f( (GLfloat)(x+sprites[idx].spLargura), (GLfloat)(y+sprites[idx].spAltura));
    glTexCoord2f((GLfloat)posx, (GLfloat)(posy+altura)); 
    glVertex2f((GLfloat)x, (GLfloat)(y+sprites[idx].spAltura));
    // Encerra o quad
    glEnd();

	return true;
}


static int ValidaSprite(unsigned int id, unsigned int indice)
{
	// O id é válido?
	if(id == 0)
		return -1;
	int idx = id-1;
	// O índice é válido?
	if(indice >= (unsigned int)(sprites[idx].matrizX * sprites[idx].matrizY) || sprites[idx].textura==0)
		return -1;

	return idx;
}

// Função para desenhar um sprite
//
// Data: 13/04/2007
//
bool C2D2_DesenhaSpriteEfeito(unsigned int id, unsigned int indice, int x[4], int y[4], Uint8 r, Uint8 g, Uint8 b, Uint8 a)
{
	// O id é válido?
	int idx = ValidaSprite(id, indice);
	if(idx < 0)
		return false;
	
	// Tudo certo, calcula as coordenadas iniciais a desenhar na textura:
	int xImg = (indice%sprites[idx].matrizX)*sprites[idx].spLargura;
	int yImg = (indice/sprites[idx].matrizX)*sprites[idx].spAltura;
	// Calcula o ponto inicial da textura baseado no tamanho total desta
	double posx = (double)xImg/(double)(sprites[idx].larguraTextura);
	double posy = (double)yImg/(double)(sprites[idx].alturaTextura);
	// qual a largura (em porcentagem) na imagem
	double largura = (double)(sprites[idx].spLargura)/(double)(sprites[idx].larguraTextura);
	double altura = (double)(sprites[idx].spAltura)/(double)(sprites[idx].alturaTextura);
	// Faz o desenho em um quad OpenGL
    // Indica que quer usar textura
    setaTexturizacao(true);
    // Seleciona a textura a utilizar
	setaTextura(sprites[idx].textura);
    // Prepara o blend de imagens
    alteraBlend(true);
    modoBlend(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);
    // Indica como pega a subtextura e como faz a escala
    modoWrapTextura(GL_CLAMP, GL_CLAMP);
    modoFiltragem(GL_LINEAR, GL_LINEAR);
    // Desenha o sprite num quad OpenGL
    glBegin(GL_QUADS);		       
    glColor4ub(r, g, b, a);
    // DEfine o primeiro vértice na textura
    glTexCoord2f((GLfloat)posx, (GLfloat)posy); 
    // Coordenada do vértice na tela
    glVertex2f((GLfloat)x[0], (GLfloat)y[0]);
    // E assim por diante
    glTexCoord2f((GLfloat)(posx+largura), (GLfloat)posy); 

	glVertex2f( (GLfloat)x[1], (GLfloat)y[1]);
    glTexCoord2f((GLfloat)(posx+largura), (GLfloat)(posy+altura)); 
    glVertex2f( (GLfloat)x[2], (GLfloat)y[2]);
    glTexCoord2f((GLfloat)posx, (GLfloat)(posy+altura));
    glVertex2f((GLfloat)x[3], (GLfloat)y[3]);
    // Encerra o quad
    glEnd();

	return true;
}

bool C2D2_DesenhaSpriteCentro(unsigned int id, unsigned int indice, double xcentro, double ycentro, int largura, int altura)
{
	return C2D2_DesenhaSpriteCentroAlfa(id, indice, xcentro, ycentro, largura, altura, 255, 255, 255, 255);	
}

bool C2D2_DesenhaSpriteCentroAlfa(unsigned int id, unsigned int indice, double xcentro, double ycentro, int largura, int altura, Uint8 r, Uint8 g, Uint8 b, Uint8 a)
{
	int x[4];
	int y[4];

	double offsetx = xcentro - (largura  / 2);
	double offsety = ycentro - (altura / 2);

	x[0] = x[3] = (int)offsetx;
	x[1] = x[2] = (int)(offsetx + largura);

	y[0] = y[1] = (int)offsety;
	y[2] = y[3] = (int)(offsety + altura);

	return C2D2_DesenhaSpriteEfeito(id, indice, x, y, r, g, b, a);
}


// Função para desenhar um sprite distorcido e com efeitos de cor/alpha em resolução subpixel
//
// Data: 26/02/2011
//
bool C2D2_DesenhaSpriteEfeitoSubpixel(unsigned int identificador, unsigned int indice, float x[4], float y[4], Uint8 r, Uint8 g, Uint8 b, Uint8 a)
{
	// O id é válido?
	int idx = ValidaSprite(identificador, indice);
	if(idx < 0)
		return false;
	
	// Tudo certo, calcula as coordenadas iniciais a desenhar na textura:
	int xImg = (indice%sprites[idx].matrizX)*sprites[idx].spLargura;
	int yImg = (indice/sprites[idx].matrizX)*sprites[idx].spAltura;
	// Calcula o ponto inicial da textura baseado no tamanho total desta
	double posx = (double)(xImg+1)/(double)(sprites[idx].larguraTextura);
	double posy = (double)(yImg+1)/(double)(sprites[idx].alturaTextura);
	// qual a largura (em porcentagem) na imagem
	double largura = (double)(sprites[idx].spLargura-2)/(double)(sprites[idx].larguraTextura);
	double altura = (double)(sprites[idx].spAltura-2)/(double)(sprites[idx].alturaTextura);
	// Faz o desenho em um quad OpenGL
    // Indica que quer usar textura
    setaTexturizacao(true);
    // Seleciona a textura a utilizar
	setaTextura(sprites[idx].textura);
    // Prepara o blend de imagens
    alteraBlend(true);
    modoBlend(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);
    // Indica como pega a subtextura e como faz a escala
    modoWrapTextura(GL_CLAMP, GL_CLAMP);
    modoFiltragem(GL_LINEAR, GL_LINEAR);
    // Desenha o sprite num quad OpenGL
    glBegin(GL_QUADS);		       
    glColor4ub(r, g, b, a);
    // DEfine o primeiro vértice na textura
    glTexCoord2f((GLfloat)posx, (GLfloat)posy); 
    // Coordenada do vértice na tela
    glVertex2f((GLfloat)x[0], (GLfloat)y[0]);
    // E assim por diante
    glTexCoord2f((GLfloat)(posx+largura), (GLfloat)posy); 

	glVertex2f( (GLfloat)x[1], (GLfloat)y[1]);
    glTexCoord2f((GLfloat)(posx+largura), (GLfloat)(posy+altura)); 
    glVertex2f( (GLfloat)x[2], (GLfloat)y[2]);
    glTexCoord2f((GLfloat)posx, (GLfloat)(posy+altura));
    glVertex2f((GLfloat)x[3], (GLfloat)y[3]);
    // Encerra o quad
    glEnd();

	return true;

}

// Função para desenhar um sprite com resolução subpixel
//
// Data: 26/02/2011
//
bool C2D2_DesenhaSpriteSubpixel(unsigned int identificador, unsigned int indice, float x, float y)
{
    // Valida o sprite e pega o índice, para pegar a largura
	int idx = ValidaSprite(identificador, indice);
	if(idx < 0)
		return false;
    // Calcula as coordenadas
	static float vx[4], vy[4];
	vx[0]=vx[3] = x;
	vx[1]=vx[2] = x + sprites[idx].spLargura-2;
	vy[0]=vy[1] = y;
	vy[2]=vy[3] = y + sprites[idx].spAltura-2;
	// Desenha o sprite com efeito
    C2D2_DesenhaSpriteEfeitoSubpixel(identificador, indice, vx, vy, 255, 255, 255, 255);
}


// Métodos específicos para a versão OpenGL

inline void resetaGL()

{
    faz_blend = false;
    glDisable(GL_BLEND);
    texturiza = false;
    glDisable(GL_TEXTURE_2D);
    textura = 0;
    blendOrigem = 0xffffffff;
    blendDestino = 0xffffffff; 
    wrapTexturaS = 0xffffffff; 
    wrapTexturaT = 0xffffffff; 
    filtroMax = 0xffffffff; 
    filtroMin = 0xffffffff; 
}

inline void alteraBlend(bool estado)
{
	if(faz_blend == estado)
		return;

	if(estado)
		glEnable(GL_BLEND);
	else
		glDisable(GL_BLEND);
	faz_blend = estado;
}

void setaTexturizacao(bool estado)
{
	if(texturiza == estado)
		return;

	if(estado)
		glEnable(GL_TEXTURE_2D);
	else
		glDisable(GL_TEXTURE_2D);
	texturiza = estado;
}

inline void modoBlend(GLenum origem, GLenum destino)
{
	if((blendOrigem == origem) && (blendDestino == destino))
		return;

	glBlendFunc(origem, destino);

	blendOrigem = origem;
	blendDestino = destino;
}

inline void setaTextura(GLuint text)
{
	if(text == textura)
		return;

	glBindTexture(GL_TEXTURE_2D, text);
	textura = text;
}

inline void modoWrapTextura(GLenum wTexturaS, GLenum wTexturaT)
{
	if((wrapTexturaS == wTexturaS) && (wrapTexturaT == wTexturaT))
		return;

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, wTexturaS);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, wTexturaT);

	wrapTexturaS = wTexturaS;
	wrapTexturaT = wTexturaT;
}
inline void modoFiltragem(GLenum fMax, GLenum fMin)
{
	if((filtroMin == fMin) && (filtroMax == fMax))
		return;

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, fMin);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, fMax);

	filtroMin = fMin;
	filtroMax = fMax;
}

// Função auziliar para criar pontos transparentes a partir de cor chave
//
// Data: 12/06/2008
//

void MagentaParaPontoTransparente(SDL_Surface *surface)
{
	if(surface == 0)
		return;
	SDL_LockSurface(surface);
	// Quantos bytes tem por pixel a imagem
    int bpp = surface->format->BytesPerPixel;
    // Endereço do ponto a recuperar
    Uint8 *p;
	Uint32 cor;
	Uint32 cor2 = SDL_MapRGBA(surface->format, 255, 0, 255,255);
	for(int y=0;y<surface->h;y++)
		for(int x=0;x<surface->w;x++)
		{
			p= (Uint8 *)surface->pixels + y * surface->pitch + x * bpp;
			// Só funciona em imagens com 32 bits e canal de alfa
			cor = *(Uint32 *)p;
			// Se for magenta o ponto, coloca 0 (transparente)
			if(cor == cor2)// && (cor&surface->format->Amask) != 0)
				*(Uint32 *)p=0;
		}
	SDL_UnlockSurface(surface);
}


// Função para verificar a colisão entre um sprite e um quadrado de referência
//
// Data: 22/06/2010
//
bool C2D2_ColidiuQuadrados(int x1b, int y1b, int l1b, int a1b,
					       int x2b, int y2b, int l2b, int a2b)
{
	// Calcula o offset x e y do segundo sprite em relação ao primeiro
	int offx, offy;
	offx=x2b-x1b;
	offy=y2b-y1b;
	// Testa se o bounding box dos sprites se sobrepõe
	// verifica quem está à esquerda
	if(offx>=0)
	{
		// Quando o offset é positivo, o sprite 2 está mais à direita
		// O offset não pode ser maior que a largura do quadrado do sprite 1
		if(offx > l1b)
			return 0;
	}
	else
	{
		// Senão é o contrário
		if(-offx > l2b)
			return 0;
	}
	// Verifica quem está acima
	if(offy>=0)
	{
		// Quando o offset é positivo, o sprite 1 está acima
		// O offset não pode ser maior que a altura do quadrado do sprite 1
		if(offy>a1b)
			return 0;
	}
	else
	{
		// Senão é o contrário
		if(-offy > a2b)
			return 0;
	}

	return 1;
}

// Função para verificar a colisão entre sprites usando bounding box
//
// Data: 22/06/2010
//
bool C2D2_ColidiuSprites(unsigned int id1, unsigned int indice1, int x1, int y1, int x1b, int y1b, int l1b, int a1b,
					     unsigned int id2, unsigned int indice2, int x2, int y2, int x2b, int y2b, int l2b, int a2b)
{
	x1 += x1b;
	y1 += y1b;

	x2 += x2b;
	y2 += y2b;

	// Calcula o offset x e y do segundo sprite em relação ao primeiro
	int offx, offy;
	offx=x2-x1;
	offy=y2-y1;
	// verifica se os índices dos sprites são válidos]
	if(indice1 >= (unsigned int)(sprites[id1-1].matrizX*sprites[id1-1].matrizY) || indice2 >= (unsigned int)(sprites[id2-1].matrizX*sprites[id2-1].matrizY))
		return 0;
	// Testa se o bounding box dos sprites se sobrepõe
	// verifica quem está à esquerda
	if(offx>=0)
	{
		// Quando o offset é positivo, o sprite 2 está mais à direita
		// O offset não pode ser maior que a largura do quadrado do sprite 1
		if(offx > l1b)
			return 0;
	}
	else
	{
		// Senão é o contrário
		if(-offx > l2b)
			return 0;
	}
	// Verifica quem está acima
	if(offy>=0)
	{
		// Quando o offset é positivo, o sprite 1 está acima
		// O offset não pode ser maior que a altura do quadrado do sprite 1
		if(offy>a1b)
			return 0;
	}
	else
	{
		// Senão é o contrário
		if(-offy > a2b)
			return 0;
	}

	return C2D2_ColidiuSprites(id1, indice1, x1, y1, id2, indice2, x2, y2);
}

// Função para verificar a colisão entre sprites
//
// Data: 03/05/2007
//
bool C2D2_ColidiuSprites(unsigned int id1, unsigned int indice1, int x1, int y1, 
					unsigned int id2, unsigned int indice2, int x2, int y2)
{
	// Verifica se os sprites são válidos
	if(id1==0 || id2==0)
	{
		printf("Tentou colidir com um sprite invalido (0)\n");
		return false;
	}
	// Calcula o offset x e y do segundo sprite em relação ao primeiro
	int offx, offy;
	offx=x2-x1;
	offy=y2-y1;
	// verifica se os índices dos sprites são válidos]
	if(indice1 >= (unsigned int)(sprites[id1-1].matrizX*sprites[id1-1].matrizY) || indice2 >= (unsigned int)(sprites[id2-1].matrizX*sprites[id2-1].matrizY))
		return 0;
	// Testa se o bounding box dos sprites se sobrepõe
	// verifica quem está à esquerda
	if(offx>=0)
	{
		// Quando o offset é positivo, o sprite 2 está mais à direita
		// O offset não pode ser maior que a largura do sprite 1
		if(offx > sprites[id1-1].spLargura)
			return 0;
	}
	else
	{
		// Senão é o contrário
		if(-offx > sprites[id2-1].spLargura)

			return 0;
	}
	// Verifica quem está acima
	if(offy>=0)
	{
		// Quando o offset é positivo, o sprite 1 está acima
		// O offset não pode ser maior que a altura do sprite 1
		if(offy>sprites[id1-1].spAltura)
			return 0;
	}
	else
	{
		// Senão é o contrário
		if(-offy > sprites[id2-1].spAltura)
			return 0;
	}

	// Se chegou aqui, retorna o resultado da colisão ponto-a-ponto
	return C2D2_SobrepoeBitMasks(sprites[id1-1].bmask[indice1], 
		sprites[id2-1].bmask[indice2], offx, offy) ? true : false;

}


// Função para retornar as dimensões de um sprites
//
//   Data: 22/01/2011
//
bool C2D2_DimensoesSprite(unsigned int idx, int *largura, int *altura)
{
	// Verifica se os sprites são válidos
	if(idx == 0)
	{
		printf("Sprite inválido - 0.\n");
		return false;
	}
	// Ajusta o indice
	idx--;
	// Verifica se o sprite existe mesmo
   	if( sprites[idx].textura==0)
   		return false;
	*largura = sprites[idx].spLargura;
	*altura = sprites[idx].spAltura;
	return true;
}


////////////////////////////////
// Funções para manipular Fontes
////////////////////////////////

// função para carregar uma fonte na memória
//
// Data: 24/04/2007
//
unsigned int C2D2_CarregaFonte(const char *arquivo, int dimensao)
{
    // Verifica se a fonte existe
    unsigned int idx = C2D2_ProcuraFonte(arquivo);
    // Se já existe uma fonte com o apelido, retorna o índice associado
    if(idx != 0)
    {
        fontes[idx-1].usuarios = fontes[idx-1].usuarios+1;
        return idx;
    }
        
    // Se não existe, procura o primeiro índice vago (idx ainda é igual a 0)
    for(int i=0;i<C2D2_MAX_FONTES;i++)
        // A fonte é vaga (não tem imagem associada?
		if(fontes[i].idSpriteSet == 0)
        {
            idx=i+1;
            break;
        }
    
    // ATENÇÃO: não ter espaço na memória não é relacionado a este teste. Apenas
    // testa-se aqui se existe um índice vago na lista. Se não existe, idx==0
    if(idx==0)

        // Retorna um índice inválido para indicar que o método falhou
        return 0;
    
    // Se chegou até aqui, idx contém o identificador correto 

    // posição no array (sem incrementar 1)
    idx=idx-1;    

    // Indica o apelido
	strcpy(fontes[idx].apelido, arquivo);

	// Guarda o tamanho, em pontos, da fonte
	fontes[idx].tamanhoFonte = dimensao;
	// Carrega o spriteset
	fontes[idx].idSpriteSet = C2D2_CarregaSpriteSet(arquivo, dimensao, dimensao);
    // Conseguiu carregar o spriteset? Se for inválido, retorna 0
    if(fontes[idx].idSpriteSet == 0)
    {
		printf("Não conseguir abrir a fonte do arquivo %s.\n", arquivo);
        fontes[idx].apelido[0]='\0';
        return 0;
    }
	 // Testa se o tamanho da imagem está certo para a fonte
	 // Se inválido, retorna 0
	 if(sprites[fontes[idx].idSpriteSet-1].imagem->w != dimensao*16 ||
	  sprites[fontes[idx].idSpriteSet-1].imagem->h != dimensao*16)
	 {
		 printf("As dimensões da fonte são inválidas. utilize um arquivo de %ix%i pixels, com as fontes numa matriz 16x16.\n", dimensao*16, dimensao*16);
		 fontes[idx].apelido[0]='\0';
		 C2D2_RemoveSpriteSet(fontes[idx].idSpriteSet);
		 return 0;
	 }    

    // Preenche os deslocamentos
	SDL_LockSurface(sprites[fontes[idx].idSpriteSet-1].imagem);
    for(int linha=0;linha<16;linha++)
        for(int coluna=0;coluna<16;coluna++)
        {           
            // Procura a primeira coluna com pixels não transparentes
            
            // A primeira coluna com pixel não transparente
            int primeira=-1;
            // Para todas as colunas e linhas da fonte...
            for(int x=0;x<fontes[idx].tamanhoFonte;x++)
            {
                for(int y=0;y<fontes[idx].tamanhoFonte;y++)
                if(!PontoTransparente(sprites[fontes[idx].idSpriteSet-1].imagem, 
                    x+coluna*fontes[idx].tamanhoFonte, 
                    y+linha*fontes[idx].tamanhoFonte))
                {
                    primeira = x;
                    break;
                }
                // Deve parar o laço do x?
                if(primeira != -1)
                    break;
            }
			SDL_UnlockSurface(sprites[fontes[idx].idSpriteSet-1].imagem);
            // Achou uma coluna não transparente?
            if(primeira >= 0)
				    
                    fontes[idx].deslocamentos[linha*16+coluna] = primeira;
            else
            {
                // A letra é toda transparente, pega o máximo de largura e deslocamento 0
                fontes[idx].larguras[linha*16+coluna] = fontes[idx].tamanhoFonte;
                fontes[idx].deslocamentos[linha*16+coluna] = 0;
                // Pula para a próxima letra
                continue;
            }
            
            // Agora procura a primeira coluna com todos os pontos transparntes
            // (executa só se há uma coluna transparente, vide else anterior)

            // Para todas as colunas e linhas da fonte...
            int ultima=-1;
            // Indica que todos os pontos na coluna são transparentes
            bool todosBrancos;
			SDL_LockSurface(sprites[fontes[idx].idSpriteSet-1].imagem);
            for(int x=primeira+1;x<fontes[idx].tamanhoFonte;x++)
            {
                // Todos os pontos são transparentes até prova contrária
                todosBrancos = true;
                for(int y=0;y<fontes[idx].tamanhoFonte;y++)
					if(!PontoTransparente(sprites[fontes[idx].idSpriteSet-1].imagem, 
						x+coluna*fontes[idx].tamanhoFonte, 
						y+linha*fontes[idx].tamanhoFonte))
					{
						todosBrancos = false;
						break;
					}
                // Deve parar o laço do x?
                if(todosBrancos)
                {
                    ultima=x;
                    break;
                }
            }
			SDL_UnlockSurface(sprites[fontes[idx].idSpriteSet-1].imagem);
            // Achou uma coluna transparente?
            if(ultima >= 0)
            {
                // Adiciona um pouco à largura, de acordo com o tamanho da fonte
                if(fontes[idx].tamanhoFonte<16)
					ultima+=1;
				else if(fontes[idx].tamanhoFonte<32)
                    ultima+=1;
				else if(fontes[idx].tamanhoFonte<64)
					ultima+=2;
				else
					ultima+=3;
                // Testa se não saiu fora da fonte
                if(ultima <= fontes[idx].tamanhoFonte)
                    fontes[idx].larguras[linha*16+coluna] = ultima - primeira;
                else
                    // Se saiu fora da fonte, volta pra dentro. Pode ser que não sobre espaço
                    fontes[idx].larguras[linha*16+coluna] = fontes[idx].tamanhoFonte - primeira;
            }
            else
                fontes[idx].larguras[linha*16+coluna] = fontes[idx].tamanhoFonte - primeira;          
        }
    // Deixa o espaço como metade do tamanho da fonte
    fontes[idx].larguras[' '] = fontes[idx].tamanhoFonte / 2;
    fontes[idx].deslocamentos[' '] = 0;    
    // Indica que tem um usuário
    fontes[idx].usuarios = 1;
    return idx+1;
    
}


// Remove uma fonte da memória
//
// Data: 24/04/2007
//
void C2D2_RemoveFonte(unsigned int id)
{
    // Está na faixa esperada para remover?
    if(id > C2D2_MAX_FONTES || id==0)
        return;
    // Só remove se existe apenas um usuário da fonte ou se estiver em shutdown
    if(fontes[id-1].usuarios > 1 && !c_shutdown)
    {
        fontes[id-1].usuarios = fontes[id-1].usuarios-1;
        return;
    }
        
    if(fontes[id-1].idSpriteSet != 0)
    {
        C2D2_RemoveSpriteSet(fontes[id-1].idSpriteSet);
        fontes[id-1].idSpriteSet = 0;
        strcpy(fontes[id-1].apelido, "");
    }
    fontes[id-1].usuarios = 0;
}


// Procura uma fonte pelo nome do arquivo original
//
// Data: 24/04/2007
//
unsigned int C2D2_ProcuraFonte(const char *apelido)
{
    for(int i=0;i<C2D2_MAX_FONTES;i++)
		if(strcmp(fontes[i].apelido, apelido) == 0)
            // O índice é a posição atual + 1
            return i+1;
    return 0;
}

// Função para desenhar um texto
//
// Data: 24/04/2007
//
bool C2D2_DesenhaTexto(unsigned int identificador, int x, int y, char const *texto, unsigned int alinhamento)
{
    // A fonte existe?

    if(identificador > C2D2_MAX_FONTES || identificador ==0)
        return false;
    // A largura real do texto
    int larguraReal, alturaReal;

    if(!C2D2_DimensoesTexto(identificador, texto, &larguraReal, &alturaReal))
    {
        printf("Falhou ao procurar as dimensões do texto %s com a fonte %s.\n", 
			texto, fontes[identificador-1].apelido);
        return false;
    }

    // Pega a fonte para acesso local
    C2D2_Fonte *fonte = &(fontes[identificador-1]);
    
    // Desenha de acordo com o alinhamento
    switch(alinhamento)
    {
        case C2D2_TEXTO_CENTRALIZADO:
        {
            // Começa a desenhar a partir da coordenada x-deslocamento da primeira imagem
            int posicao = x - larguraReal/2;
			int letras = (int)strlen(texto);
            for(int i=0;i<letras;i++)
            {   
                C2D2_DesenhaSprite(fonte->idSpriteSet, (unsigned char)texto[i], posicao-fonte->deslocamentos[(unsigned char)(texto[i])], y);
                // Atualiza para a próxima letra
                posicao+=fonte->larguras[(unsigned char)(texto[i])];
            }
            break;
        }

        case C2D2_TEXTO_DIREITA:
        {
            // Começa a desenhar a partir da coordenada x-deslocamento da primeira imagem
            int posicao = x - larguraReal;
			int letras = (int)strlen(texto);
            for(int i=0;i<letras;i++)
            {   
				C2D2_DesenhaSprite(fonte->idSpriteSet, (unsigned char)texto[i], posicao-fonte->deslocamentos[(unsigned char)(texto[i])], y);
                // Atualiza para a próxima letra
                posicao+=fonte->larguras[(unsigned char)(texto[i])];
            }
            break;
        }

        case C2D2_TEXTO_ESQUERDA:
        default:
        {
            // Começa a desenhar a partir da coordenada x-deslocamento da primeira imagem
            int posicao = x;
			int letras = (int)strlen(texto);
            for(int i=0;i<letras;i++)
            {   
                C2D2_DesenhaSprite(fonte->idSpriteSet, (unsigned char)texto[i], posicao-fonte->deslocamentos[(unsigned char)(texto[i])], y);
                // Atualiza para a próxima letra
                posicao+=fonte->larguras[(unsigned char)(texto[i])];
            }
            break;
        }
    }    
    return true;
}

// Função para calcular as dimensões de um texto        
//
// Data: 25/04/2007
//
bool C2D2_DimensoesTexto(unsigned int idFonte, const char *texto, int *largura, int *altura)
{
    // A fonte existe?
    if(idFonte > C2D2_MAX_FONTES || idFonte == 0)
        return false;
    C2D2_Fonte *fonte = &(fontes[idFonte-1]);
	if(fonte->idSpriteSet != 0)
    {
        //  Pega a altura
		if(altura != 0)
			*altura=fonte->tamanhoFonte;
        // Largura local
        int larg=0;
        // Calcula o tamanho
		int letras = (int)strlen(texto);
        for(int i =0;i<letras;i++)
            larg+=fonte->larguras[(unsigned char)(texto[i])];
        // Passa o valor
		if(largura != 0)
			*largura=larg;
        return true;
    }
    return false;
}

// função para retornar a dimensão de uma fonte
//
// Data: 26/01/2011
//
bool C2D2_DimensaoFonte(unsigned int idx, int *altura)
{
	if(idx > C2D2_MAX_FONTES || idx == 0)
		return false;
	C2D2_Fonte *fonte = &(fontes[idx-1]);
	if(fonte->idSpriteSet != 0)
	{
		//  Pega a altura
		if(altura != 0)
		*altura=fonte->tamanhoFonte;
		return true;
	}
	return false;
}

// Funções inline auxiliares para pressionar um botão
//
// Data: 04/05/2007
//
inline void C2D2_PressionaBotao(C2D2_Botao *botao)
{
	botao->pressionado=true;
	botao->liberado=false;
	botao->ativo=true;
	
}

inline void C2D2_LiberaBotao(C2D2_Botao *botao)
{
	botao->ativo=false;
	botao->liberado=true;
	botao->pressionado=false;
}

inline void C2D2_AtualizaDirecional(C2D2_Botao *botao, int sdlHatValue, int sdlHatFlag)
{
	if(sdlHatValue & sdlHatFlag)
	{
		if(!botao->pressionado)
		{
			C2D2_PressionaBotao(botao);
		}
	}
	else if(botao->pressionado)
	{						
		C2D2_LiberaBotao(botao);
	}
}


// Função auxiliar para processar uma tecla
//
// Data: 09/02/2007
//

inline void C2D2_ProcessaTecla(C2D2_Botao *tecla, Uint8 evento)
{
    switch(evento)
    {
        case SDL_KEYDOWN:
            tecla->pressionado=true;
            tecla->liberado=false;
            tecla->ativo=true;
            break;
        case SDL_KEYUP:
            tecla->ativo=false;
            tecla->liberado=true;
            tecla->pressionado=false;
            break;
    }
}

// Poling das entradas. TRata todos os eventos relacionados a dispositivos de
// entrada do sistema
//
// Data: 04/05/2007
//
void C2D2_Entrada()
{
	// Remove o pressionamento das teclas do passo anterior
	for(int i=0;i<C2D2_MAXTECLAS;i++)
		teclas[i].pressionado = false;
	// Remove o liberamento das teclas do passo anterior
	for(int i=0;i<C2D2_MAXTECLAS;i++)
		teclas[i].liberado=false;
	// Remove o pressionamento do mouse do passo anterior
	for(int i=0;i<C2D2_MMAX;i++)
		mouse.botoes[i].pressionado = false;
	// Remove o liberamento do mpouse do passo anterior
	for(int i=0;i<C2D2_MMAX;i++)
		mouse.botoes[i].liberado=false;

	for(int i = 0;i < C2D2_MAX_JOYSTICKS; ++i)
	{
		for(int j = 0;j < C2D2_MAX_JBOTOES; ++j)
		{
			joysticks[i].botoes[j].liberado = false;
			joysticks[i].botoes[j].pressionado = false;
		}
	}


    // A estrutura que recebe os eventos
     SDL_Event event;
     // Enquanto houverem eventos a processar ...
     while(SDL_PollEvent( &event ))
     {
         // TRata de acordo com o tipo de evento
         switch( event.type )
         {
                case SDL_QUIT:
				C2D2_PressionaBotao(&teclas[C2D2_ENCERRA]);
				break;
                  
              case SDL_KEYDOWN:
              case SDL_KEYUP:
                        switch(event.key.keysym.sym)
                        {
                                case SDLK_UP:
                                    C2D2_ProcessaTecla(&teclas[C2D2_CIMA], event.type);
									break;
								case SDLK_DOWN:
									C2D2_ProcessaTecla(&teclas[C2D2_BAIXO], event.type);
									break;
								case SDLK_LEFT:
									C2D2_ProcessaTecla(&teclas[C2D2_ESQUERDA], event.type);
									break;									
								case SDLK_RIGHT:
									C2D2_ProcessaTecla(&teclas[C2D2_DIREITA], event.type);
									break;
                                case SDLK_ESCAPE:
                                  	C2D2_ProcessaTecla(&teclas[C2D2_ESC], event.type);
                                  	break;
                                case SDLK_F1:
                                  	C2D2_ProcessaTecla(&teclas[C2D2_F1], event.type);
                                  	break;
                                case SDLK_F2:
                                  	C2D2_ProcessaTecla(&teclas[C2D2_F2], event.type);
                                  	break;
                                case SDLK_F3:
                                  	C2D2_ProcessaTecla(&teclas[C2D2_F3], event.type);
                                  	break;
                                case SDLK_F4:
                                  	C2D2_ProcessaTecla(&teclas[C2D2_F4], event.type);
                                  	break;
/*                                case SDLK_p:
                                  	C2D2_ProcessaTecla(&teclas[C2D2_P], event.type);
                                  	break;*/
                                case SDLK_RETURN:
                                  	C2D2_ProcessaTecla(&teclas[C2D2_ENTER], event.type);
                                  	break;
								case SDLK_SPACE:
									C2D2_ProcessaTecla(&teclas[C2D2_ESPACO], event.type);
									break;
								case SDLK_LALT:
									C2D2_ProcessaTecla(&teclas[C2D2_LALT], event.type);
									break;
								case SDLK_RALT:
									C2D2_ProcessaTecla(&teclas[C2D2_RALT], event.type);
									break;
								case SDLK_LCTRL:
									C2D2_ProcessaTecla(&teclas[C2D2_LCTRL], event.type);
									break;
								case SDLK_RCTRL:
									C2D2_ProcessaTecla(&teclas[C2D2_RCTRL], event.type);
									break;
								case SDLK_LSHIFT:
									C2D2_ProcessaTecla(&teclas[C2D2_LSHIFT], event.type);
									break;
								case SDLK_RSHIFT:
									C2D2_ProcessaTecla(&teclas[C2D2_RSHIFT], event.type);
									break;
/*								case SDLK_z:
									C2D2_ProcessaTecla(&teclas[C2D2_Z], event.type);
									break;
								case SDLK_x:
									C2D2_ProcessaTecla(&teclas[C2D2_X], event.type);
									break;
								case SDLK_c:
									C2D2_ProcessaTecla(&teclas[C2D2_C], event.type);
									break;*/

                                    // Processa as teclas de letras do jeito mais fácil
                                  default:  
                                        if(event.key.keysym.sym >= SDLK_a && event.key.keysym.sym <= SDLK_z)
                                            C2D2_ProcessaTecla(&teclas[C2D2_A+event.key.keysym.sym-SDLK_a], event.type);
                                        else if(event.key.keysym.sym >= SDLK_0 && event.key.keysym.sym <= SDLK_9)
                                            C2D2_ProcessaTecla(&teclas[C2D2_0+event.key.keysym.sym-SDLK_0], event.type);
                                  	break;
                        }
                        break;


			  case SDL_MOUSEMOTION:
				  mouse.x=event.motion.x;
				  mouse.y=event.motion.y;
				  break;
			  case SDL_MOUSEBUTTONDOWN:
				  mouse.x=event.button.x;
				  mouse.y=event.button.y;
				  switch(event.button.button)
				  {
				  case SDL_BUTTON_LEFT:
					  C2D2_PressionaBotao(&mouse.botoes[C2D2_MESQUERDO]);
					  break;
				  case SDL_BUTTON_RIGHT:
					  C2D2_PressionaBotao(&mouse.botoes[C2D2_MDIREITO]);
					  break;
				  case SDL_BUTTON_MIDDLE:
					  C2D2_PressionaBotao(&mouse.botoes[C2D2_MMEIO]);
					  break;
				  }
				  break;
			  case SDL_MOUSEBUTTONUP:
				  mouse.x=event.button.x;
				  mouse.y=event.button.y;
				  switch(event.button.button)
				  {
				  case SDL_BUTTON_LEFT:
					  C2D2_LiberaBotao(&mouse.botoes[C2D2_MESQUERDO]);
					  break;
				  case SDL_BUTTON_RIGHT:
					  C2D2_LiberaBotao(&mouse.botoes[C2D2_MDIREITO]);
					  break;
				  case SDL_BUTTON_MIDDLE:
					  C2D2_LiberaBotao(&mouse.botoes[C2D2_MMEIO]);
					  break;
				  }
				  break;

			  case SDL_JOYBUTTONDOWN:
			  case SDL_JOYBUTTONUP:
				  if(event.jbutton.which >= C2D2_MAX_JOYSTICKS)
					  break;

				  if(event.jbutton.button >= C2D2_MAX_JBOTOES)
					  break;

				  if(joysticks[event.jbutton.which].joystick == NULL)
					  break;

				  (event.jbutton.state == SDL_PRESSED ? C2D2_PressionaBotao : C2D2_LiberaBotao)(&joysticks[event.jbutton.which].botoes[event.jbutton.button]);
				  break;	

			  case SDL_JOYHATMOTION:
				  if(event.jhat.which >= C2D2_MAX_JOYSTICKS)
					  break;

				  if(event.jhat.hat >= C2D2_MAX_DIRECIONAIS)
					  break;

				  {
					C2D2_Joystick *joy = joysticks + event.jhat.which;
					if(joy->joystick == NULL)
					  break;			

					C2D2_AtualizaDirecional(&joy->direcional[event.jhat.hat][C2D2_CIMA], event.jhat.value, SDL_HAT_UP);
					C2D2_AtualizaDirecional(&joy->direcional[event.jhat.hat][C2D2_DIR_DIREITA], event.jhat.value, SDL_HAT_RIGHT);
					C2D2_AtualizaDirecional(&joy->direcional[event.jhat.hat][C2D2_DIR_BAIXO], event.jhat.value, SDL_HAT_DOWN);
					C2D2_AtualizaDirecional(&joy->direcional[event.jhat.hat][C2D2_DIR_ESQUERDA], event.jhat.value, SDL_HAT_LEFT);
				  }
				  break;

			  case SDL_JOYBALLMOTION:
				  if(event.jball.which >= C2D2_MAX_JOYSTICKS)
					  break;

				  if(event.jball.ball >= C2D2_MAX_DIRECIONAIS)
					  break;

				  C2D2_LiberaBotao(&mouse.botoes[C2D2_MMEIO]);
				  break;
                                            
              default:
					break;
        }
     }
}

// Função para recuperar as teclas
//
// Data: 04/05/2007
//
C2D2_Botao* C2D2_PegaTeclas()
{
	return teclas;
}

// Função para recuperar o mouse
//
// Data: 04/05/2007
//
C2D2_Mouse* C2D2_PegaMouse()
{
	return &mouse;
}

// Função para zerar os dados da Chien2D 2
//
// Data: 29/03/2007

void reset()
{
	// Zera os sprites
	for(int i=0;i<C2D2_MAX_SPRITESET; i++)
	{
		strcpy(sprites[i].apelido, "");
		sprites[i].imagem = 0;
		sprites[i].usuarios = 0;
		sprites[i].bmask = 0;
		sprites[i].textura=0;
		sprites[i].texturaLigada=false;
	}
	// Zera as fontes
	for(int i=0;i<C2D2_MAX_FONTES;i++)
	{
		strcpy(fontes[i].apelido, "");
		fontes[i].idSpriteSet = 0;
		fontes[i].usuarios = 0;
	}
}

// Função auziliar para pontos transparentes
//
// Data: 25/04/2007
//

bool PontoTransparente(SDL_Surface *surface, int x, int y)
{
	if(x >= surface->w || y >= surface->h)
		return false;
    // Quantos bytes tem por pixel a imagem
    int bpp = surface->format->BytesPerPixel;
    // Endereço do ponto a recuperar
    Uint8 *p = (Uint8 *)surface->pixels + y * surface->pitch + x * bpp;
    // Só funciona em imagens com 32 bits e canal de alfa
    Uint32 cor = *(Uint32 *)p;
    Uint32 cor2 = SDL_MapRGBA(surface->format, 255, 0, 255, 255);
    if((cor != cor2) && ((cor&AMASK) != 0) ) //(cor&surface->format->Amask) != 0)
        return false;
    else
        return true;
    
    return false;
}

///////////////////////////////////////////////////
// Funções relacionadas com as bitmasks dos sprites
///////////////////////////////////////////////////

// Função para criar uma bit mask para um sprite
//
// Data: 03/05/2007
//
C2D2_BitMask* C2D2_CriaBitMask(int largura, int altura)
{
	// Cria a Bitmask para retornar
	C2D2_BitMask* temp = (C2D2_BitMask*)malloc(sizeof(C2D2_BitMask));
	// Se falhou retorna uma máscara nula
	if(temp == 0)
		return 0;
	// SE deu certo toca o barco
	temp->altura = altura;
	temp->largura = largura;
	// O número de linhas está correto. Falta saber quantos quadwords são necessários por linha
	int quads = largura/32;
	// Verifica se não precisa de um extra
	if(largura%32)
		quads++;
	// Aloca o vetor da máscara em si
	temp->ldwords = quads;
	temp->mask = (Uint32*)malloc(sizeof(Uint32)*quads*altura);
	// Se falhou, remove e retorna nulo
	if(temp->mask == 0)
	{
		// Como falhou a alocação da máscara, tem que apagar o bit mask
		free(temp);
		// Retorna o ponteiro nulo
		return 0;
	}
	// Se chegou até aqui, temp possui a bitmask completa
	return temp;
}

// Função para remover uma bitmask de um sprite
//
// Data: 03/05/2007
//
void C2D2_RemoveBitMask(C2D2_BitMask* bmask)
{
	// Verifica se tem corretamente a máscara
	if(bmask->mask)
		free(bmask->mask);
	// Remove o bitmask
	free(bmask);
}

// Função para criar o vetor de bitmasks de um spriteset
//
// Data: 03/05/2007
//
C2D2_BitMask** C2D2_CriaVetorBitMasks(SDL_Surface *imagem, int spx, int spy, int limagem, int aimagem)
{
	// Cria as máscaras de bits
	C2D2_BitMask **masks = 0;
	// Quantas máscaras de bits são necessárias?
	int colunas = limagem/spx;
	int linhas = aimagem/spy;
	int nummasks = colunas*linhas;
	// Aloca o vetor de máscaras
	masks=(C2D2_BitMask**)malloc(sizeof(C2D2_BitMask*)*nummasks);
	// Se não alocou com sucesso deve retornar a falha. Difícil, mas vai que acontece o 0,00001% de chance de erro
	if(masks == 0)
		return 0;
	// Zera o vetor de ponteiro de máscaras
	memset(masks, 0, sizeof(C2D2_BitMask*)*nummasks);
	// Uma vez inicializado o vetor de máscaras, devemo palocar as máscaras uma a uma
	int conta;
	for(conta=0;conta<nummasks;conta++)
	{
		masks[conta] = C2D2_CriaBitMask(spx, spy);
		if(masks[conta] == 0)
			break;
	}
	// Verifica se bugou
	if(conta != nummasks)
	{
		// Se entrou aqui, tem que apagar o que alocou até agora e retornar nulo
		// Apaga as máscaras que foram alocadas corretamente
		for(int i=0;i<conta-1;i++)
			C2D2_RemoveBitMask(masks[i]);
		// Remove o vetor de máscaras
		free(masks);
		// Retorna o vetor nulo
		return 0;
	}
	// Se chegou aqui é porquê tudo está certo. Retorna as máscaras
	return masks;
}

// Função para setar um bit do bitmask
//
// Data: 03/05/2007
//
void C2D2_SetBit(C2D2_BitMask *bmask, int x, int y, int val)
{
	// Testa se pode colocar um bit nesta posição
	if(x<0 || x>=bmask->largura && y<0 && y>=bmask->altura)
		return;
	// Determina em qual quadword da linha vai o bit
	int quad = x/32;
	// Determina em qual bit da quadword vai o bit do usuário
	int bitindex = x%32;
	// Cria a máscara para acessar o bit
	Uint32 mask = 0x80000000;
	// Rotaciona os bits
	mask = mask >> bitindex;
	// Coloca o bit na máscara
	if(val)
		bmask->mask[y*bmask->ldwords + quad] = bmask->mask[y*bmask->ldwords + quad] | mask;
	else
		bmask->mask[y*bmask->ldwords + quad] = bmask->mask[y*bmask->ldwords + quad] & ~mask;
}

// Função para setar um bit do bitmask
//
// Data: 03/05/2007
//
bool C2D2_GetBit(C2D2_BitMask *bmask, int x, int y)
{
	// Testa se pode pegar um bit nesta posição
	if(x<0 || x>=bmask->largura && y<0 && y>=bmask->altura)
		return false;
	// Determina em qual quadword da linha vai o bit
	int quad = x/32;
	// Determina em qual bit da quadword vai o bit do usuário
	int bitindex = x%32;
	// Cria a máscara para acessar o bit
	Uint32 mask = 0x80000000;
	// Rotaciona os bits
	mask = mask >> bitindex;
	// Coloca o bit na máscara
	if(bmask->mask[y*bmask->ldwords + quad] & mask)
		return true;
	else
		return false;
}


// Função para preencher o vetor de bitmasks de um spriteset
//
// Data: 03/05/2007
//
void C2D2_PreencheVetorBitMasks(SDL_Surface *imagem, int spx, int spy, int limagem, int aimagem, C2D2_BitMask** bmask)
{
	// Quantas máscaras de bits são necessárias?
	int colunas = limagem/spx;
	int linhas = aimagem/spy;
	// A máscara local 
	C2D2_BitMask* lbmask;
	// AS coordenadas x e y iniciais no sprite dentro da imagem
	int posx,posy;
	// As coordenadas x e y dentro do sprite
	int x,y;
	// Trava a surface SDL
	SDL_LockSurface(imagem);
	// Para cada sprite no spriteset
	for(int l=0;l<linhas;l++)
		for(int c=0;c<colunas;c++)
		{
			// Pega os pontos dentro da imagem
			posx=c*spx;
			posy=l*spy;
			// Pega a máscara sendo acessada
			lbmask = bmask[l*colunas+c];
			// Pega as posições do sprite (ufa)
			for(y=0;y<spy;y++)
				for(x=0;x<spx;x++)
					if(PontoTransparente(imagem, x+posx, y+posy))
						C2D2_SetBit(lbmask, x, y, 0);
					else
						C2D2_SetBit(lbmask, x, y, 1);
		}

	// DEstrava a surface SDL
	SDL_UnlockSurface(imagem);
}


// Função para testar a colisão entre dois bitmasks, dado um offset x e y da segunda em relação à primeira
//
// Data: 03/05/2007
//
int C2D2_SobrepoeBitMasks(C2D2_BitMask *a, C2D2_BitMask *b, int offx, int offy)
{
	// Coisa idiota, faz um dump das máscaras
	/*printf("Sprite1:\n");
	for(int i=0;i<a->altura;i++)
	{
		for(int j=0;j<a->ldwords;j++)
			printf("%i ", a->mask[i*a->ldwords + j]);
		printf("\n");
	}*/
			
	// A largura e altura a comparar
	int largura, altura;
	// O ponto DENTRO das bitmasks a comparar
	int xa, ya, xb, yb;
	
	// Calcula a largura e os pontos x dentro da imagem. Esta parte de longe é a mais embaçada
	// do processo. Mas passando isso é sossegado.

	// Se a imagem b está à direita
	if(offx>=0)
	{
		// Como o offset é positivo, ele representa o ponto inicial na imagem 1
		xa=offx;
		// Como a imagem b está à direita, começa no ponto 0
		xb=0;
		// A largura a princípio é a da máscara a menos o offset
		largura = a->largura-offx;
		// Se a largura for negativa ou zero, retorna
		if(largura <= 0)
			return 0;
		// Caso a largura da colisão seja menor que a largura da máscara b, esta passa a ser a largura a testar
		if(largura>b->largura)
			largura=b->largura;
	}
	// Senão ela está à esquerda
	else
	{
		// Como a imagem a está à direita, começa no ponto 0
		xa=0;
		// Como o offset é negativo, o xb é o módulo do offset
		xb=-offx;
		// A largura a princípio é a da máscara b mais o offset
		largura = b->largura+offx;
		// Se a largura for negativa ou zero, retorna
		if(largura <= 0)
			return 0;
		// Caso a largura da colisão seja menor que a largura da máscara a, esta passa a ser a largura a testar
		if(largura>a->largura)
			largura=a->largura;
	}
	// Se a imagem a está acima
	if(offy>=0)

	{

		// Como o offset y é positivo, ele é o ponto y inicial da imagem a
		ya = offy;
		// Como a imagem b está abaixo, o ponto inicial é 0
		yb=0;
		// A largura a princípio é a altura da máscara a menos o offset
		altura = a->altura - offy;
		// Se a altura for zero ou menor, pula fora
		if(altura <= 0)
			return 0;
		// Caso a altura da colisão seja maior que a altura da máscara b, esta passa a ser a altura a testar
		if(altura>b->altura)
			altura=b->altura;
	}
	// Caso contrário, a imagem b está acima
	else
	{
		// Como a imagem a está abaixo, 0 é o ponto inicial y dela
		ya=0;
		// Como o offset é negativo, o módulo dele é o ponto y inicial na imagem b
		yb=-offy;
		// A altura é a da máscara b mais o offset
		altura=b->altura+offy;
		// Se a altura for zero ou menor, pula fora
		if(altura<=0)
			return 0;
		// Caso a altura da colisão for mais alta que a imagem a, esta passa a ser a altura da colisão
		if(altura>a->altura)
			altura=a->altura;
	}

	// Enfim, com as máscaras devidamente alinhadas, podemos começar as comparações de verdade

	// Faz um for para os quadwords inteiros que ele busca
	int quads=largura/32;
	// Os quads e bits a indexar
	int quada, quadb, bita, bitb;
	// Os valores a testar
	int vquada, vquadb, temp;
	// Calcula o offset de bits
	bita=xa%32;
	bitb=xb%32;
	// A máscara de bits
	Uint32 mask;
	// Calcula o quad inicial a testar
	quada=xa/32+ya*a->ldwords;
	quadb=xb/32+yb*b->ldwords;

	for(int l=0;l<altura;l++)
		for(int c=0;c<quads;c++)
		{
			//
			// Recupera o valor do quad da máscara A
			//

			// Coloca todos os bits da máscara em 1
			mask = 0xFFFFFFFF;
			// Rotaciona para a direita os bits da máscara em bita bits
			mask = mask >> bita;
			// Recupera a primeira parte do valor quad
			vquada = a->mask[quada + c +l*a->ldwords] & mask;
			// Coloca os bits no começo (importante)
			vquada = vquada << bita;
			// Testa se tem que recuperar outra parte do valor do quad (offset em bits diferente de 0)
			if(bita>0)
			{
				// Coloca todos os bits da máscara em 1
				mask = 0xFFFFFFFF;
				// Rotaciona para a esquerda os bits da máscara em 32-bita bits
				mask = mask << (32-bita);
				// Recupera os bits
				temp = a->mask[quada + c + 1 + l*a->ldwords] & mask;
				// Coloca os bits para a direita
				temp = temp >> (32-bita);
				// Consolida com o resto da máscara o valor do quad
				vquada = vquada | temp;
			}
			//
			// Recupera o valor do quad da máscara B
			//

			// Coloca todos os bits da máscara em 1
			mask = 0xFFFFFFFF;
			// Rotaciona para a direita os bits da máscara em bita bits
			mask = mask >> bitb;
			// Recupera a primeira parte do valor quad
			vquadb = b->mask[quadb + l*b->ldwords] & mask;
			// Coloca os bits no começo (importante)
			vquadb = vquadb << bitb;

			// Testa se tem que recuperar outra parte do valor do quad (offset em bits diferente de 0)
			if(bitb>0)
			{
				// Coloca todos os bits da máscara em 1
				mask = 0xFFFFFFFF;
				// Rotaciona para a esquerda os bits da máscara em 32-bita bits
				mask = mask << (32-bitb);
				// Recupera os bits
				temp = b->mask[quadb + c + 1 + l*b->ldwords] & mask;
				// Coloca os bits para a direita
				temp = temp >> (32-bitb);
				// Consolida com o resto da máscara o valor do quad
				vquadb = vquadb | temp;
			}

			// testa, enfim, se colidiu
			if(vquada & vquadb)
				return -1;
		}
	// Faz um último for para o quadword quebrado que tem que recuperar
	if(largura%32)
	{
		// Calcula o quad inicial a testar
		quada=xa/32+quads+ya*a->ldwords;
		quadb=xb/32+quads+yb*b->ldwords;
		// Calcula o quadword inicial
		for(int l=0;l<altura;l++)
		{
			//
			// Recupera o valor do quad da máscara A
			//

			// Coloca todos os bits da máscara em 1
			mask = 0xFFFFFFFF;
			// Rotaciona para a direita os bits da máscara em bita bits
			mask = mask >> bita;
			// Recupera a parte final do valor quad
			vquada = a->mask[quada + l*a->ldwords] & mask;
			// Rotaciona o quad para colocar na posição inicial os bits
			vquada = vquada << bita;
			// Testa se tem que recuperar outra parte do valor do quad (offset em bits diferente de 0)
			if(32-bita < largura%32)
			{
				// Coloca todos os bits da máscara em 1
				mask = 0xFFFFFFFF;
				// Rotaciona para a esquerda os bits da máscara em 32-bita bits
				mask = mask << (largura%32 - (32-bita));
				// Recupera os bits
				temp = a->mask[quada + 1 + l*a->ldwords] & mask;
				// Coloca os bits para a direita
				temp = temp >> (largura%32 - (32-bita));
				// Consolida com o resto da máscara o valor do quad
				vquada = vquada | temp;
			}


			//
			// Recupera o valor do quad da máscara B
			//

			// Coloca todos os bits da máscara em 1
			mask = 0xFFFFFFFF;
			// Rotaciona para a direita os bits da máscara em bita bits
			mask = mask >> bitb;
			// Recupera a primeira parte do valor quad
			vquadb = b->mask[quadb + l*b->ldwords] & mask;
			// Rotaciona o quad para colocar na posição inicial os bits
			vquadb = vquadb << bitb;
			// Testa se tem que recuperar outra parte do valor do quad (offset em bits diferente de 0)
			if(32-bitb < largura%32)
			{
				// Coloca todos os bits da máscara em 1
				mask = 0xFFFFFFFF;
				// Rotaciona para a esquerda os bits da máscara em 32-bita bits
				mask = mask << (largura%32 - (32-bitb));
				// Recupera os bits
				temp = b->mask[quadb + 1 + l*b->ldwords] & mask;
				// Coloca os bits para a direita
				temp = temp >> (largura%32 - (32-bitb));
				// Consolida com o resto da máscara o valor do quad
				vquadb = vquadb | temp;
			}


			// Coloca a máscara para pegar apenas a diferença de pixels
			mask = 0xFFFFFFFF;
			mask = mask << (32-largura%32);
			vquada = vquada & mask;
			vquadb = vquadb & mask;

			// testa, enfim, se colidiu
			if(vquada & vquadb)
				return -1;
		}
	}
	// Se chegou até aqui, é porquê não colidiu. Então retorna falso
	return 0;
}

// Função para dar uma pausa na aplicação.
//
// Data: 17/05/2009
//
void C2D2_Pausa(Uint32 pausa)
{
	SDL_Delay(pausa);
}

// Função que indica qual a função de sincronização do usuário vai ser usada.
//
// Data: 24/07/2010
void C2D2_DefineSincronizaUsuario(void (*funcao)())
{
    C2D2_SincronizaUsuario = funcao;
}

//
//
//Lists os Joysticks no console
int C2D2_ListaJoysticks()
{
	int num = SDL_NumJoysticks();
	printf("Foram econtrados %i joysticks.\n\n", num );

	if(num > 0)
	{
		printf("Sendo nomeados:\n");
			
		for( int i=0; i < num; i++ ) 
		{
			printf("    %s\n", SDL_JoystickName(i));
		}
	}

	return num;
}

bool C2D2_LigaJoystick(int index)
{
	C2D2_Joystick *joystick = C2D2_PegaJoystick(index);
	if(joystick == NULL)
		return false;

	if(joystick->joystick != NULL)
		return true;

	joysticks[index].joystick = SDL_JoystickOpen(index);
	if(joysticks[index].joystick == NULL)
		return false;

	printf(
		"Ligando joystick: %s, numBotoes: %d, numDirecionais: %d, numBalls: %d, numAxes: %d\n", 
		SDL_JoystickName(index), 
		SDL_JoystickNumButtons(joysticks[index].joystick),
		SDL_JoystickNumHats(joysticks[index].joystick),
		SDL_JoystickNumBalls(joysticks[index].joystick),
		SDL_JoystickNumAxes(joysticks[index].joystick)
	);

	return joysticks[index].joystick != NULL;
}

void C2D2_DesligaJoystick(int index)
{
	C2D2_Joystick *joystick = C2D2_PegaJoystick(index);
	if(joystick == NULL)
		return;

	if(joystick->joystick == NULL)
		return;
	
	SDL_JoystickClose(joystick->joystick);	
	joystick->joystick = NULL;
}

C2D2_Joystick *C2D2_PegaJoystick(int index)
{
	assert(index < C2D2_MAX_JOYSTICKS);
	assert(index >= 0);

	if((index < 0) || (index >= C2D2_MAX_JOYSTICKS))
		return NULL;

	return joysticks + index;
}

