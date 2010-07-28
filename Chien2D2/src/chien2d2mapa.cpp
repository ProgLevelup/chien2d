/*

Chien 2D Mapa

Copyright 2007-2010, Paulo Vinicius Wolski Radtke (pvwradtke@gmail.com)

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
#include <string.h>
#include <stdlib.h>
#if	defined(linux)
	#include <SDL/SDL_endian.h>
#else
	#include <SDL_endian.h>
#endif

#include <c2d2/chien2d2.h>
#include <c2d2/chien2d2mapa.h>

char cabecalhos[6][5] ={ "FORM", "FMAP", "MPHD", "ANDT", "BODY", "LYR?"};

// indica se a Chien2D2 Mappy j� foi inicializada. Por default, n�o foi ainda.
bool m_inicializado = false;

// O vetor com os mapas
C2D2M_Mapa mapas[C2D2M_MAX_MAPAS];
// Indica se o sistema est� em shutdown ou n�o
bool m_shutdown = false;

// Fun��es exclusivas para desenvolvimento interno. N�o use no seu programa!!

// Fun��o para zerar os dados da Chien2D 2
void m_reset();


// Fun��es da C2D2 Mapa

// Fun��o para zerar os dados da Chien2D 2
//
// Data: 29/03/2007

void m_reset()
{
	// Zera os mapas
	for(int i=0;i<C2D2M_MAX_MAPAS; i++)
	{
		// Apaga o apelido
		strcpy(mapas[i].apelido, "");
		// Zera o n�mero de usu�rios
		mapas[i].usuarios = 0;
		// Zera o ponteiro das camadas
		for(int j=0;j<C2D2M_MAX_CAMADAS;j++)
			mapas[i].camadas[j] = 0;
		mapas[i].idSpriteset = 0;
		mapas[i].inicializado = false;
		mapas[i].camadaMarcas=-1;
	}
}

// Fun��o para iniciar a Chien2D 2 Mapa
//
// Data: 27/07/2007

bool C2D2M_Inicia()
{
    if(m_inicializado)
        return false; 
	else
		m_reset();

	// Indica que foi iniciado e pula fora
    m_inicializado=true;
    return true;
}


// Fun��o que encerra a Chien2D 2
//
// Data: 29/03/2007

void C2D2M_Encerra()
{
    // Testa por via das d�vidas para n�o ter problemas
    if(!m_inicializado)
        return;
    // Indica que est� encerrando
    m_shutdown = true;
        
    printf("Apagando os mapas do sistema.\n");

    // Apaga os mapas
    for(int i=0;i<C2D2M_MAX_MAPAS;i++)
        C2D2M_RemoveMapa(i+1);

    m_inicializado=false;
}

// fun��o para carregar um sprite set na mem�ria
//
// Data: 13/04/2007
//
unsigned int C2D2M_CarregaMapaMappy(const char *arquivo_mapa, const char *arquivo_spriteset)
{
    // Verifica se o mapa n�o existe j�
    unsigned int idx = C2D2M_ProcuraMapa(arquivo_mapa);
    // Se j� existe um sprite com o nome do arquivo, retorna o �ndice associado
    if(idx != 0)
    {
		mapas[idx-1].usuarios=mapas[idx-1].usuarios+1;
        return idx;
    }
        
    // Se n�o existe, procura o primeiro �ndice vago (idx ainda � igual a 0)
    for(int i=0;i<C2D2M_MAX_MAPAS;i++)
        // O spriteset � vago (n�o tem imagem associada?
        if(!mapas[i].inicializado)
        {
            idx=i+1;
            break;
        }
 
    // Testa se ainda tem espa�o na lista
    //
    // ATEN��O: n�o ter espa�o na mem�ria n�o � relacionado a este teste. Apenas
    // testa-se aqui se existe um �ndice vago na lista. Se n�o existe, idx==0
    if(idx==0)
        // Retorna um �ndice inv�lido para indicar que a fun��o falhou
        return 0;
    
    // Se chegou at� aqui, idx cont�m o identificador correto 
    // Calcula a posi��o no array (sem incrementar 1)
    idx-=1;    
	// Indica o caminho do arquivo no apelido
    strcpy(mapas[idx].apelido,arquivo_mapa);

	//////////////////////////////////////////////////

    FILE *mapa = fopen(arquivo_mapa, "rb");
	// Se falhou abrir o arquivo, deve retornar o erro
    if(mapa == 0)
	{
		C2D2M_RemoveMapa(idx+1);
        return 0;
	}
    // pega o tamanho do arquivo
	fseek(mapa, 0, SEEK_END);
    int tamanho = ftell(mapa);
    fseek(mapa, 0, SEEK_SET);
    // Aloca mem�ria para o arquivo inteiro
    char *buffer = (char *)malloc(tamanho*sizeof(char));
    // L� os dados
	fread(buffer, sizeof(char), tamanho, mapa);
    // Fecha o arquivo
      fclose(mapa);
    // Pega o "suposto" cabe�alho do arquivo
    CabecalhoArquivo *cabArq = (CabecalhoArquivo*)buffer;
    // Verifica se o cabecalho � certo
    if(C2D2M_VerificaCabecalhoArquivo(cabArq))
    {
        // Recupera o tamanho do arquivo (est� em BIG ENDIAN)
        int tamanho = cabArq->tamanho;
        if(SDL_BYTEORDER == SDL_LIL_ENDIAN)
            tamanho = SDL_Swap32(tamanho);
        // O tamanho total soma 8
        tamanho+=8;
        // �ndice dentro do buffer lido, j� pulando o header de 12 bytes
        int indice=12;        
        // Indica se o arquivo � armazenado em little endian (recupera do MPHD)
        bool arquivoLilEndian=false;
        // Indica se o sistema � lil endian (usado com a vari�vel em arquivo)
        bool sistemaLilEndian = false;
        if(SDL_BYTEORDER == SDL_LIL_ENDIAN)
            sistemaLilEndian=true;
        // Processa os chunks do arquivo enquanto o �ndice estiver dentro do arquivo
        while(indice < tamanho)
        {
            // pega o pr�ximo chunk
            CabecalhoBloco *bloco = (CabecalhoBloco *)&(buffer[indice]);
            // Faz o indice apontar para os dados do bloco (pula 8 bytes)
            indice += 8;
            // Pega o tamanho do bloco (est� no formato intel)
            int tamBloco = bloco->tamanho;
            if(SDL_BYTEORDER == SDL_LIL_ENDIAN)
                tamBloco = SDL_Swap32(tamBloco);
            //    Aqui extra�mos as informa��es dos blocos. Nos interessam apenas
            // o header do arquivo (MPHD), o cen�rio (BODY), os layers (LYR"X")
            // e as anima��es (ANDT)
            switch(C2D2M_TipoBloco(bloco))
            {
                case CABECALHO_MPHD:
                {
                    // Acessa o header
                    MPHD *mphd = (MPHD *)&(buffer[indice]);
                    // O arquivo � codificado em Little Endian?
                    if(mphd->lsb)
                        arquivoLilEndian=true;
                    // Extra as informa��es relevantes
                    mapas[idx].altura = mphd->mapheight;
                    mapas[idx].largura = mphd->mapwidth;
                    mapas[idx].dimensaoBlocoH = mphd->blockwidth;
                    mapas[idx].dimensaoBlocoV = mphd->blockheight;
                    // Se o sistema e arquivo n�o baterem
                    if(arquivoLilEndian != sistemaLilEndian)
                    {
                        mapas[idx].altura = SDL_Swap16(mapas[idx].altura);
                        mapas[idx].largura = SDL_Swap16(mapas[idx].largura);
                        mapas[idx].dimensaoBlocoH = SDL_Swap16(mapas[idx].dimensaoBlocoH);
                        mapas[idx].dimensaoBlocoV = SDL_Swap16(mapas[idx].dimensaoBlocoV);
                    }
                    // Calcula a largura do mapa em pixels
                    mapas[idx].larguraPixel = mapas[idx].largura*mapas[idx].dimensaoBlocoH;
                    mapas[idx].alturaPixel = mapas[idx].altura*mapas[idx].dimensaoBlocoV;
                    break;
                }
                // Bloco dos dados da camada principal (0)
                case CABECALHO_BODY:
                {
                    // Aloca a mem�ria do layer principal
					mapas[idx].camadas[0] = (short int*)malloc(mapas[idx].altura*mapas[idx].largura*sizeof(short int));
                    // Copia os dados do buffer para a camada
                    memcpy(mapas[idx].camadas[0], (short int*)&(buffer[indice]), 
                            mapas[idx].largura*mapas[idx].altura*sizeof(short int));
                    if(arquivoLilEndian != sistemaLilEndian)
                        for(int i=0;i<mapas[idx].altura*mapas[idx].largura;i++)
                           mapas[idx].camadas[0][i]=SDL_Swap16(mapas[idx].camadas[0][i]); 
                    break;
                }
                // Um dos layers associados
                case CABECALHO_LYR:
                {
                    // Determina a camada
                    int camada = bloco->tipo[3]-'0';
                    // Aloca a mem�ria do layer principal
                    mapas[idx].camadas[camada] = (short int*)malloc(mapas[idx].altura*mapas[idx].largura*sizeof(short int));
                    // Copia os dados do buffer para a camada
                    memcpy(mapas[idx].camadas[camada], (short int*)&(buffer[indice]), 
                            mapas[idx].altura*mapas[idx].largura*sizeof(short int));
                    if(arquivoLilEndian != sistemaLilEndian)
                        for(int i=0;i<mapas[idx].altura*mapas[idx].largura;i++)
                           mapas[idx].camadas[camada][i]=SDL_Swap16(mapas[idx].camadas[camada][i]); 
                    break;
                }
                // Anima��es
                case CABECALHO_ANDT:
                {
                    // Procura as estruturas ANISTR (tem que ter pelo menos uma)
                    int numAniStr=0;
                    while(true)
                    {
                        // Pega a estrutura (a zero � vazia)
                        numAniStr++;
						memcpy(&(mapas[idx].estrutAnimacao[numAniStr]), 
                            (ANISTR*)&(buffer[indice+tamBloco-sizeof(ANISTR)*numAniStr]),
                            sizeof(ANISTR));
                        // Se a anima��o � do tipo AN_END, chegou ao fim
                        if(mapas[idx].estrutAnimacao[numAniStr].antype == AN_END)
                            break;
                    }
                    // Extrai as sequ�ncias de anima��o: tamanho do bloco- ANISTR lidas
                    int numAnimSeq=(tamBloco-sizeof(ANISTR)*numAniStr)/sizeof(long int);
                    memcpy(mapas[idx].seqAnimacao, (long int*)&(buffer[indice]),
                        sizeof(long int)*numAnimSeq);
                    // Ajusta os dados com mais de um byte
                    if(arquivoLilEndian != sistemaLilEndian)
                    {
                        for(int i=0;i<numAniStr;i++)
                        {
                           mapas[idx].estrutAnimacao[i].ancuroff = SDL_Swap32(mapas[idx].estrutAnimacao[i].ancuroff); 
                           mapas[idx].estrutAnimacao[i].anstartoff = SDL_Swap32(mapas[idx].estrutAnimacao[i].anstartoff); 
                           mapas[idx].estrutAnimacao[i].anendoff = SDL_Swap32(mapas[idx].estrutAnimacao[i].anendoff);                                                      
                        }
                        for(int i=0;i<numAnimSeq;i++)
                            mapas[idx].seqAnimacao[i] = SDL_Swap32(mapas[idx].seqAnimacao[i]);
                    }
                    break;                    
                }
                
                // Ignora o bloco e passa para o pr�ximo
                default:
                    break;
            }
            // Atualiza a posi��o do �ndice
            indice += tamBloco;
        }
    }
    // Apaga o buffer
    free(buffer);

	//////////////////////////////////////////////////

	// Tenta carregar o spriteset
	mapas[idx].idSpriteset = C2D2_CarregaSpriteSet(arquivo_spriteset, mapas[idx].dimensaoBlocoH, mapas[idx].dimensaoBlocoV);
	// Se n�o conseguiu, apaga tudo
	if(	mapas[idx].idSpriteset == 0)
	{
		C2D2M_RemoveMapa(idx+1);
		return 0;
	}
	// Daqui pra frente � ladeira ...

	// Zera as velocidades do mapa
	memset(mapas[idx].vCamadas, 0, C2D2M_MAX_CAMADAS*sizeof(int));
	// Indica que tem um usu�rio
    mapas[idx].usuarios = 1;
	// Coordenadas iniciais do mapa
	mapas[idx].x=0;
	mapas[idx].y=0;
	// Camada de marcadores default: -1
	mapas[idx].camadaMarcas = -1;
	// Indica que n�o est� procurando um mapa
	mapas[idx].buscaProximo = false;
	// Coloca a velocidade default como 0 para mapas topview
	mapas[idx].gravidade = 0;
	// Coloca a velocidade de queda m�xima em 0
	mapas[idx].maxgravidade = 0;
	// Indica que inicializou
	mapas[idx].inicializado = true;
    // idx+1 indica o identificador. Retorna e encerra (ufa!)
    return idx+1;
}


// Remove um mapa da mem�ria
//
// Data: 28/07/2007
//
void C2D2M_RemoveMapa(unsigned int id)
{
    // O identificador � v�lido?
    if(id >= C2D2M_MAX_MAPAS || id == 0)
        return;
    // S� apaga se o n�mero de usu�rios for um ou se estiver em shutdown
    if(mapas[id-1].usuarios > 1 && !m_shutdown)
    {
        mapas[id-1].usuarios -= 1;
        return;
    }
    // Se cair aqui, � porqu� n�o tem mais usu�rios
    mapas[id-1].usuarios = 0;
    // Tem uma surface SDL associada?
	if(mapas[id-1].idSpriteset != 0)
	{
		C2D2_RemoveSpriteSet(mapas[id-1].idSpriteset);
		mapas[id-1].idSpriteset = 0;
	}
    for(int i=0;i<C2D2M_MAX_CAMADAS;i++)
		if(mapas[id-1].camadas[i] != 0)
		{
            free(mapas[id-1].camadas[i]);
			mapas[id-1].camadas[i] = 0;
		}
	strcpy(mapas[id-1].apelido, "");
	// Enfim, indica que n�o est� inicializado
	mapas[id-1].inicializado = false;
}


// Procura um mapa pelo nome do arquivo original
//
// Data: 13/04/2007
//
unsigned int C2D2M_ProcuraMapa(const char *apelido)
{
    // �ndice de spriteset inv�lido, caso n�o encontre
    unsigned int idx=0;
    for(int i=0;i<C2D2M_MAX_MAPAS;i++)
        if(strcmp(mapas[i].apelido, apelido)==0)
        {
            // O �ndice � a posi��o atual + 1
            idx=i+1;
            // Encerra a busca
            break;
        }
    return idx;
}

// Fun��o para ver as dimens�es do mapa (retorna true se o mapa � v�lido, false caso contr�rio)
//
// Data: 26/01/2005
// �ltima atualiza��o: 02/08/2007

bool C2D2M_DimensoesMapa(unsigned int idMapa, int *largura, int *altura)
{
	// � um �ndice v�lido?
	if(idMapa >= C2D2M_MAX_MAPAS)
		return false;
    // Se n�o foi inicializado, falha!
    if(!mapas[idMapa-1].inicializado)
        return false;
    // Preenche os valores
	*largura = mapas[idMapa-1].larguraPixel;
	*altura = mapas[idMapa-1].alturaPixel;
    return true;
}

// Posiciona o mapa nas coordenadas x,y indicadas
//
// Data: 16/08/2006
void C2D2M_PosicionaMapa(unsigned int idMapa, int x, int y)
{
    // O identificador � v�lido?
    if(idMapa >= C2D2M_MAX_MAPAS || idMapa == 0)
        return;
    // Se n�o foi inicializado, falha!
    if(!mapas[idMapa-1].inicializado)
        return;
	mapas[idMapa-1].x = x;
	mapas[idMapa-1].y = y;
}

// Recupera a posi��o do mapa
//
// Data: 16/08/2006
bool C2D2M_PosicaoXY(unsigned int idMapa, int *x, int *y)
{
    // O identificador � v�lido?
    if(idMapa >= C2D2M_MAX_MAPAS || idMapa == 0)
        return false;
    // Se n�o foi inicializado, falha!
    if(!mapas[idMapa-1].inicializado)
        return false;
	*x = mapas[idMapa-1].x;
	*y = mapas[idMapa-1].y;
	return true;
}

// Fun��o para dar velocidade a uma camada do mapa
//
// Data: 02/08/2007

void C2D2M_VelocidadeCamadaMapa(unsigned int idMapa, int camada, int velocidade)
{
	// � um mapa v�lido?
	if(idMapa==0 || idMapa >= C2D2M_MAX_MAPAS)
		return;
	// � um �ndice v�lido?
	if(camada >= C2D2M_MAX_MAPAS || camada <0)
		return;
    // Se n�o foi inicializado, falha!
    if(!mapas[idMapa-1].inicializado)
        return;
	// A camada � v�lida?
	if(camada<0 || camada>=C2D2M_MAX_CAMADAS)
		return;
	mapas[idMapa-1].vCamadas[camada] = velocidade;
}

// Fun��o para indicar a camada de marcas do mapa e o n�mero do bloco inicial de marcas no cen�rio
//
// Data: 16/08/2007
//
bool C2D2M_CamadaMarcas(unsigned int idMapa, int camada, int offbloco)
{
	// � um mapa v�lido?
	if(idMapa==0 || idMapa >= C2D2M_MAX_MAPAS)
		return false;
	// � um �ndice v�lido?
	if(camada >= C2D2M_MAX_MAPAS)
		return false;
	// Enfim, marca a camada
	mapas[idMapa-1].camadaMarcas = camada;
	mapas[idMapa-1].offbloco = offbloco;
	// Reseta a busca de blocos, caso algu�m use um dia a habilidade de mudar as camadas do mapa
	mapas[idMapa-1].buscaProximo = false;
	return true;
}


// M�todo para desenhar o mapa na tela a partir das coordenadas x,y do mapa
//
// Data: 26/01/2005
// �ltima atualiza��o: 28/01/2005


// Fun��o para desenhar uma camada do mapa na tela a partir das coordenadas do mapa (x,y na estrutura), dentro da janela especificada
// por xtela,ytela e as dimens�es largura e altura.
//
// Data: 28/01/2005
// �ltima atualiza��o: 16/08/2007

void C2D2M_DesenhaCamadaMapa(unsigned int idMapa, int camada, int xtela, int ytela, int largura, int altura)
{
	// � um �ndice v�lido?
	if(idMapa > C2D2M_MAX_MAPAS || idMapa ==0)
		return;
	if(!mapas[idMapa-1].inicializado)
        return;
    // TEsta se � um layer v�lido
	if(camada >= C2D2M_MAX_CAMADAS || camada <0)
        return;
	// Pega o ponteiro do mapa (mais r�pido)
	C2D2M_Mapa *mapa = &mapas[idMapa-1];
    // Testa se o layer existe
	if(mapa->camadas[camada] == 0)
        return;
	
    // S� desenha o mapa se o ponto �ncora for dentro do mapa e o mapa couber na tela

	if(mapa->x<0 || mapa->y<0 || mapa->x>(mapa->larguraPixel-largura) 
		|| mapa->y>( mapa->alturaPixel - altura))
        return;
	int xmapa, ymapa;
	// Calcula o ponto de acordo com a velocidade do mapa
	if(mapa->vCamadas[camada])
	{
		xmapa=mapa->x/mapa->vCamadas[camada];
		ymapa=mapa->y/mapa->vCamadas[camada];
	}
	else
	{
		xmapa=0;
		ymapa=0;
	}
	// Pega a surface da tela
/*	SDL_Surface *tela = SDL_GetVideoSurface();
	// Prepara o ret�ngulo de cliping
	SDL_Rect rect;
	rect.x=xtela;
	rect.y=ytela;
	rect.w=largura;
	rect.h=altura;
	// Clipa!
	SDL_SetClipRect(tela, &rect);*/
	// Determina as dimens�es da tela em sprites (quanto vai desenhar)
	int larguraSprites = largura/mapa->dimensaoBlocoH;
	int alturaSprites = altura/mapa->dimensaoBlocoV;
	// Se as dimens�es da janela n�o s�o m�ltiplos do tamanho do sprite, tem que desenhar um bloco a mais
	if(largura%mapa->dimensaoBlocoH)
		larguraSprites++;
	if(altura%mapa->dimensaoBlocoV)
		alturaSprites++;
    // Determina os blocos iniciais no mapa
    int blocoX = xmapa/mapa->dimensaoBlocoH;
    int blocoY = ymapa/mapa->dimensaoBlocoV;
    // DEtermina o offset em pixels DENTRO do sprite
    int offsetX = xmapa%mapa->dimensaoBlocoH;
    int offsetY = ymapa%mapa->dimensaoBlocoV;
    // Se tem offset, pode ser preciso desenhar um bloco a mais em cada dire��o
	// Para determinar se precisa ou n�o do offset, verificamos se o offset � maior que o resto da divis�o pelo tamanho do bloco
    // Nota: n�o precisa verificar se vai ultrapassar o limite f�sico do mapa 
    // porque o teste da posi��o no mapa acima j� faz isso
    if(offsetX)
        larguraSprites++;
    if(offsetY)
        alturaSprites++;
	// O sprite a ser desenhado em cada posi��o
	int sprite;
    // Desenha o mapa
    for(int linha=0;linha<alturaSprites;linha++)
        for(int coluna=0;coluna<larguraSprites;coluna++)
        {
            // Recupera o azulejo a desenhar
			sprite = mapa->camadas[camada][mapa->largura*(linha+blocoY)+coluna+blocoX];
            // Se o sprite for negativo, recupera o sprite na lista de anima��es
            if(sprite < 0)
                //    Como o valor � negativo, tem que negar antes, 
                // pegando na sequ�ncia de anima��o
                sprite = mapa->seqAnimacao[mapa->estrutAnimacao[-sprite].ancuroff];
            // N�o desenha o sprite 0 (transparente)
            if(sprite!=0)
				C2D2_DesenhaSprite(mapa->idSpriteset, sprite-1, xtela+coluna*mapa->dimensaoBlocoH-offsetX,
					ytela+linha*mapa->dimensaoBlocoV-offsetY);
        }
	// Retira o Clip
	//SDL_SetClipRect(tela, 0);
}

// fun��o para buscar as coordenadas da primeira ocorr�ncia de um bloco de marca dentro de um mapa
//
// Data: 17/08/2007
bool C2D2M_PrimeiroBlocoMarca(unsigned int idMapa, int bloco, int *x, int *y)
{
	// � um �ndice v�lido?
	if(idMapa >= C2D2M_MAX_MAPAS || idMapa ==0)
		return false;
	if(!mapas[idMapa-1].inicializado)
        return false;
	// Recupera o mapa
	C2D2M_Mapa *mapa = &mapas[idMapa-1];
    // TEsta se op layer de marcas existe
	if(mapa->camadaMarcas == -1)
        return false;
	int pos;
	// Procura em cada posi��o at� achar
	for(pos=0;pos<mapa->largura*mapa->altura;pos++)
	{
		if(mapa->camadas[mapa->camadaMarcas][pos]==bloco+mapa->offbloco)
			break;
	}
	// Se achou, pos � menor que o n�mero total de blocos na camada
	if(pos>=mapa->largura*mapa->altura)
		return false;
	// Se chegou aqui, achou o bloco!!
	// Calcula a posi��o
	*x = (pos % mapa->largura) * mapa->dimensaoBlocoH + mapa->dimensaoBlocoH/2;
	*y = (pos / mapa->largura) * mapa->dimensaoBlocoV + mapa->dimensaoBlocoV/2;
	// Guarda a informa��o para a chamada de C2D2M_ProximoBlocoMarca
	mapa->buscaProximo = true;
	mapa->posicaoUltimo = pos;
	mapa->blocoProcurado = bloco;
	// Retorna verdade
	return true;
}

// fun��o para buscar as coordenadas da pr�xima ocorr�ncia de um bloco de marca dentro de um mapa. Usar apenas AP�S C2D2M_PrimeiroBlocoMarca.
//
// Data: 23/08/2007
bool C2D2M_ProximoBlocoMarca(unsigned int idMapa, int *x, int *y)
{
	// � um �ndice v�lido?
	if(idMapa >= C2D2M_MAX_MAPAS || idMapa ==0)
		return false;
	if(!mapas[idMapa-1].inicializado)
        return false;
	// Recupera o mapa
	C2D2M_Mapa *mapa = &mapas[idMapa-1];

	// TEsta: est� procurando uma seq��ncia de blocos??
	if(!mapa->buscaProximo)
		return false;
	// Se chegou aqui, a camada de marcas existe e � v�lida. Busca
	int pos;
	// Procura em cada posi��o at� achar
	for(pos=mapa->posicaoUltimo+1;pos<mapa->largura*mapa->altura;pos++)
	{
		if(mapa->camadas[mapa->camadaMarcas][pos]==mapa->blocoProcurado+mapa->offbloco)
			break;
	}
	// Se achou, pos � menor que o n�mero total de blocos na camada
	if(pos>=mapa->largura*mapa->altura)
	{
		// Indica que n�o est� mais procurando o pr�ximo
		mapa->buscaProximo = false;
		return false;
	}
	// Se chegou aqui, achou o bloco!!
	// Calcula a posi��o
	*x = (pos % mapa->largura) * mapa->dimensaoBlocoH + mapa->dimensaoBlocoH/2;
	*y = (pos / mapa->largura) * mapa->dimensaoBlocoV + mapa->dimensaoBlocoV/2;
	// Guarda a informa��o para a pr�xima chamada de C2D2M_ProximoBlocoMarca
	mapa->posicaoUltimo = pos;
	// Retorna verdade
	return true;
}

// Fun��o que indica a acelera��o de gravidade do mapa, em pixels por segundo, e a velocidade m�xima de gravidade para simular atrito
bool C2D2M_GravidadeMapa(unsigned int idMapa, double gravidade, double maxgravidade)
{
	// � um �ndice v�lido?
	if(idMapa >= C2D2M_MAX_MAPAS || idMapa ==0)
		return false;
	if(!mapas[idMapa-1].inicializado)
        return false;
	// Recupera o mapa
	C2D2M_Mapa *mapa = &mapas[idMapa-1];
	// Muda a gravidade
	mapa->gravidade = gravidade;
	mapa->maxgravidade = maxgravidade;
	return true;
}

// Anima os blocos animados do mapa. Atualiza os quadros, reseta e etc e tal
//
// Data: 27/01/2005
// �ltima atualiza��o: 02/08/2007

void C2D2M_AnimaMapa(unsigned int id)
{
    if(id > C2D2M_MAX_MAPAS)
		return;
	// Pega o ponteiro do mapa
	C2D2M_Mapa *mapa = &mapas[id-1];
	// Testa se pode fazer a opera��o
    if(!mapa->inicializado)
        return;    
    // Procura as anima��es (0 n�o existe, lembrando)
    int indice = 1;
    while(mapa->estrutAnimacao[indice].antype != AN_END)
    {
        //     Verifica se n�o � uma anima��o (s� Deus sabe porque isso existe)
        //     Se verdade, pula (acredite, isso faz tanto sentido pra mim como 
        // pra voc�, afinal, n�o � pra ser ANIMADO?!?
        if(mapa->estrutAnimacao[indice].antype == AN_NONE)
            continue;
        // Se a anima��o zera, tem que atualizar
        if((mapa->estrutAnimacao[indice].ancount-=1) == 0)
        {
            // Armazena o delay novo
            mapa->estrutAnimacao[indice].ancount = mapa->estrutAnimacao[indice].andelay;
            //    Atualiza o frame e, se estourou, volta para o in�cio de acordo
            // com o tipo da anima��o
            switch(mapa->estrutAnimacao[indice].antype)
            {
                // Loop pra frente
                case AN_LOOPF:            
                {
                    if((mapa->estrutAnimacao[indice].ancuroff+=1) == mapa->estrutAnimacao[indice].anendoff)
                        mapa->estrutAnimacao[indice].ancuroff = mapa->estrutAnimacao[indice].anstartoff;
                    break;
                }
                // Loop reverso
                case AN_LOOPR:
                {
                    if((mapa->estrutAnimacao[indice].ancuroff-=1) < mapa->estrutAnimacao[indice].anstartoff)
                        mapa->estrutAnimacao[indice].ancuroff = mapa->estrutAnimacao[indice].anendoff-1;
                    break;
                }
                // Anima uma �nica vez (n�o muit o�til se n�o for na primeira tela)
                case AN_ONCE:            
                {
                    if(mapa->estrutAnimacao[indice].anuser)
                        break;
                    // Testa se chegou no final
                    if((mapa->estrutAnimacao[indice].ancuroff+=1) == mapa->estrutAnimacao[indice].anendoff-1)
                        mapa->estrutAnimacao[indice].anuser = true;
                    break;        
                }
                // Anima��o ping-pong pra frente
                case AN_PPFF:
                {
                    // Se verdade, vai de marcha-r�
                    if(mapa->estrutAnimacao[indice].anuser==0)
                    {
                        if((mapa->estrutAnimacao[indice].ancuroff-=1) < mapa->estrutAnimacao[indice].anstartoff)
                        {
                            mapa->estrutAnimacao[indice].ancuroff = mapa->estrutAnimacao[indice].anstartoff+1;
                            mapa->estrutAnimacao[indice].anuser = 1;                
                        }
                    }
                    else
                    {
                        if((mapa->estrutAnimacao[indice].ancuroff+=1) == mapa->estrutAnimacao[indice].anendoff)
                        {
                            mapa->estrutAnimacao[indice].ancuroff = mapa->estrutAnimacao[indice].anendoff-2;
                            mapa->estrutAnimacao[indice].anuser = 0;
                        }
                    }   
                    break;
                }
                // Anima��o ping-pong pra tr�s
                case AN_PPRR:
                {
                    // Se verdade, vai pra frente
                    if(mapa->estrutAnimacao[indice].anuser==0)
                    {
                        if((mapa->estrutAnimacao[indice].ancuroff+=1) == mapa->estrutAnimacao[indice].anendoff)
                        {
                            mapa->estrutAnimacao[indice].ancuroff = mapa->estrutAnimacao[indice].anendoff-2;
                            mapa->estrutAnimacao[indice].anuser = 1;
                        }
                    }
                    else
                    {
                        if((mapa->estrutAnimacao[indice].ancuroff-=1) < mapa->estrutAnimacao[indice].anstartoff)
                        {
                            mapa->estrutAnimacao[indice].ancuroff = mapa->estrutAnimacao[indice].anstartoff+1;
                            mapa->estrutAnimacao[indice].anuser = 0;                
                        }
                    }
                    break;
                }
                                   
                default:
                    break;
                
            }
        }
        // Pr�xima anima��o
        indice++;
    }
}

// Ajusta o deslocamento de um boundingbox no mapa. Recebe o mapa aonde o deslocamento
// se realiza, a posi��o do bounding box e o quanto deve mover no eixo x e y. O booleano
// gravidade indica se h� gravidade, situa��o na qual deslocamentos verticais para baixo
// e cima n�o implicam em deslocamento no eixo x. 
//
//  Os par�metros s�o o identificador do mapa (idMapa), as coordenadas do bounding box
// (x,y), as dimens�es do bounding box (largura), os ponteiros para o deslocamento 
// (dx e dy) e se a gravidade est� em a��o.
//
// Data: XX/XX/2007
// �ltima atualiza��o: 28/08/2008
//
void C2D2M_AjustaDeslocamento(unsigned int idMapa, int xorig, int yorig, 
							  int larg, int alt, int *dx, int *dy, bool gravidade)
{
	if(idMapa == 0 || idMapa>=C2D2M_MAX_MAPAS)
		return;
	// SE o mapa n�o foi inicializado, ou se a camada de marcas est� na default (-1)
	if(!mapas[idMapa-1].inicializado || mapas[idMapa-1].camadaMarcas==-1)
		return;
	// REcupera o valor de x e y originais
	int x=xorig,y=yorig;
	// Recupera o mapa
	C2D2M_Mapa *mapa = &mapas[idMapa-1];
	// Bounding box
	int topo, baixo, esquerda, direita;
	// A refer�ncia do ator para inclina��es, o quanto entra no bloco e a diferen�a de pontos
	int meio, entrou, dif;
	// Qual a maior dist�ncia percorrida (mais tarde, passa para o personagem)
	int maior;
	// Qual o bloco
	int bloco;
	if(abs(*dx) > abs(*dy))
		maior = abs(*dx);
	else
		maior = abs(*dy);
	// O deslocamento no eixo a cada passo (mais tarde, passa para o personagem)
	int dxl=0, dyl=0;
	if(*dx>0)
		dxl=1;
	else if(*dx < 0)
		dxl=-1;
	if(*dy>0)
		dyl=1;
	else if(*dy < 0)
		dyl=-1;
	// As vari�veis para indicar se deve deslocar ou n�o
	int xant=0, yant=0;
	// Percorre os pixels no maior eixo
	for(int i = 1; i <= maior ; i++)
	{
		// Aplica o deslocamento no eixo x?
		if(dxl !=0 && (i * *dx)/maior != xant)
		{
			xant = (i * *dx)/maior;
			// Desloca
			x += dxl;
			// Calcula o bounding box
			topo = (y)/mapa->dimensaoBlocoV;
			baixo = (y+alt-1)/mapa->dimensaoBlocoV;
			esquerda = (x)/mapa->dimensaoBlocoH;
			direita = (x+larg-1)/mapa->dimensaoBlocoH;
			// Calcula o bloco em que se encontra o meio
			meio = (x+larg/2)/mapa->dimensaoBlocoH;
			// Testa a colis�o com blocos inclinados
			if(meio >=0 && meio <= mapa->largura && mapa->dimensaoBlocoH == mapa->dimensaoBlocoV)
			{
				// ajusta pela parte de baixo
				if(baixo >= 0 && baixo<= mapa->altura)
				{
					bloco =mapa->camadas[mapa->camadaMarcas][mapa->largura*baixo+meio]-mapa->offbloco; 
					switch(bloco)
					{
					case C2D2M_SOLIDO45:
					case C2D2M_SOLIDO135:
						entrou=x+larg/2 - meio*mapa->dimensaoBlocoH;
						if(bloco == C2D2M_SOLIDO45)
							entrou++;
						else
							entrou = mapa->dimensaoBlocoH-entrou;
						dif = (baixo+1)*mapa->dimensaoBlocoV-1-(y+alt);
						// Aqui colide e ajusta
						if(dif < entrou)
							y -= entrou-dif;
							//ATOR_InsereEvento(a, EVT_COLIDIU_PAREDE_BAIXO);
						// N�o verifica a colis�o na horizontal nesta linha
						baixo--;
						break;
					case C2D2M_SOLIDO22A:
					case C2D2M_SOLIDO157B:
						entrou=x+larg/2 - meio*mapa->dimensaoBlocoH;
						if(bloco == C2D2M_SOLIDO22A)
							entrou++;
						else
							entrou = mapa->dimensaoBlocoH-entrou;
						dif = (baixo+1)*mapa->dimensaoBlocoV-1-(y+alt);
						// Aqui colide e ajusta
						if(dif < entrou/2)
							y -= entrou/2-dif;
						// N�o verifica a colis�o na horizontal nesta linha
						baixo--;
						break;
					case C2D2M_SOLIDO22B:
					case C2D2M_SOLIDO157A:
						entrou=x+larg/2 - meio*mapa->dimensaoBlocoH;
						if(bloco == C2D2M_SOLIDO22B)
							entrou++;
						else
							entrou = mapa->dimensaoBlocoH-entrou;
						dif = (baixo+1)*mapa->dimensaoBlocoV-1-(y+alt);
						// Aqui colide e ajusta
						if(dif < (entrou+mapa->dimensaoBlocoV)/2)
						{
							y -= (entrou+mapa->dimensaoBlocoV)/2-dif;
							//ATOR_InsereEvento(a, EVT_COLIDIU_PAREDE_BAIXO);
						}
						// N�o verifica a colis�o na horizontal nesta linha
						baixo--;
						break;
					}
				}

				// ajusta pela parte de cima
				if(topo >= 0 && topo<= mapa->altura)
				{
					bloco =mapa->camadas[mapa->camadaMarcas][mapa->largura*topo+meio]-mapa->offbloco; 
					switch(bloco)
					{
					case C2D2M_SOLIDO225:
					case C2D2M_SOLIDO315:
						entrou=x+larg/2 - meio*mapa->dimensaoBlocoH;
						if(bloco == C2D2M_SOLIDO315)
							entrou++;
						else
							entrou = mapa->dimensaoBlocoH-entrou;
						dif = y - (topo)*mapa->dimensaoBlocoV;
						// Aqui colide e ajusta
						if(dif < entrou)
							y += entrou-dif;
						// N�o verifica a colis�o na horizontal nesta linha
						topo++;
						break;
					case C2D2M_SOLIDO202B:
					case C2D2M_SOLIDO337A:
						entrou=x+larg/2 - meio*mapa->dimensaoBlocoH;
						if(bloco == C2D2M_SOLIDO337A)
							entrou++;
						else
							entrou = mapa->dimensaoBlocoH-entrou;
						dif = y - topo*mapa->dimensaoBlocoV;
						// Aqui colide e ajusta
						if(dif < entrou/2)
						{
							y += entrou/2-dif;
							//ATOR_InsereEvento(a, EVT_COLIDIU_PAREDE_CIMA);
						}
						// N�o verifica a colis�o na horizontal nesta linha
						topo++;
						break;
					case C2D2M_SOLIDO202A:
					case C2D2M_SOLIDO337B:
						entrou=x+larg/2 - meio*mapa->dimensaoBlocoH;
						if(bloco == C2D2M_SOLIDO337B)
							entrou++;
						else
							entrou = mapa->dimensaoBlocoH-entrou;
						dif = y - topo*mapa->dimensaoBlocoV;
						// Aqui colide e ajusta
						if(dif < (entrou+mapa->dimensaoBlocoV)/2)
						{
							y += (entrou+mapa->dimensaoBlocoV)/2-dif;
							//ATOR_InsereEvento(a, EVT_COLIDIU_PAREDE_CIMA);
						}
						// N�o verifica a colis�o na horizontal nesta linha
						topo++;
						break;
					}
				}
			}

			// Testa a colis�o com blocos completamente s�lidos
			for(int lin=topo;lin<=baixo;lin++)
			{
				// Testa se est� dentro das coordenadas v�lidas
				if(lin < 0 || lin >= mapa->altura)
					continue;
				// Testa se ainda est� dentro do mapa � esquerda 
				if(esquerda >= 0 && esquerda <= mapa->largura && dxl<0)
					// Testa se colidiu
					if(mapa->camadas[mapa->camadaMarcas][mapa->largura*lin+esquerda] == mapa->offbloco + C2D2M_SOLIDO)
					{
						// Se colidiu, aumenta um ponto e encerra
						x++; 
						break;
					}
				// Testa se ainda est� dentro do mapa � direita 
				if(direita >= 0 && direita <= mapa->largura && dxl>0)
					// Testa se colidiu
					if(mapa->camadas[mapa->camadaMarcas][mapa->largura*lin+direita] == mapa->offbloco + C2D2M_SOLIDO)
					{
						// Se colidiu, diminui um ponto e encerra
						x--; 
						break;
					}
			}
		}
		// Teste de ajuste na vertical para quando n�o h� gravidade
		if(dyl !=0 && (i * *dy)/maior != yant && !gravidade)
		{
			yant = (i * *dy)/maior;
			// Desloca
			y += dyl;
			// Calcula o bounding box
			topo = (y)/mapa->dimensaoBlocoV;
			baixo = (y+alt-1)/mapa->dimensaoBlocoV;
			esquerda = (x)/mapa->dimensaoBlocoH;
			direita = (x+larg-1)/mapa->dimensaoBlocoH;
			// Calcula o bloco em que se encontra o meio. Agora, o meio � na horizontal!
			meio = (y+alt/2)/mapa->dimensaoBlocoV;
			// Testa a colis�o com blocos inclinados
			if(meio >=0 && meio <= mapa->altura && mapa->dimensaoBlocoH == mapa->dimensaoBlocoV)
			{
				// ajusta pela parte da esquerda
				if(esquerda >= 0 && esquerda<= mapa->largura)
				{
					bloco =mapa->camadas[mapa->camadaMarcas][mapa->largura*meio+esquerda]-mapa->offbloco; 
					switch(bloco)
					{
					case C2D2M_SOLIDO135:
					case C2D2M_SOLIDO225:
						entrou=y+alt/2 - meio*mapa->dimensaoBlocoV;
						if(bloco == C2D2M_SOLIDO135)
							entrou++;
						else
							entrou = mapa->dimensaoBlocoV-entrou;						
						dif = x-esquerda*mapa->dimensaoBlocoH;
						// Aqui colide e ajusta
						if(dif < entrou)
							x += entrou-dif;
						// Encerra a verifica��o
						esquerda++;
						break;
					case C2D2M_SOLIDO157A:
					case C2D2M_SOLIDO202A:
						entrou=y+alt/2 - meio*mapa->dimensaoBlocoV;
						if(bloco == C2D2M_SOLIDO157A)
							entrou++;
						else
							entrou = mapa->dimensaoBlocoV-entrou;						
						dif = x-esquerda*mapa->dimensaoBlocoH;
						// Aqui colide e ajusta
						if(dif < entrou*2)
						{
							x += entrou*2-dif;

						}
						// Encerra a verifica��o
						esquerda++;
						break;
					case C2D2M_SOLIDO157B:
					case C2D2M_SOLIDO202B:
						entrou=y+alt/2 - meio*mapa->dimensaoBlocoV;
						if(bloco == C2D2M_SOLIDO157B)
							entrou++;
						else
							entrou = mapa->dimensaoBlocoV-entrou;						
						dif = x-esquerda*mapa->dimensaoBlocoH;
						// Aqui colide e ajusta
						if(dif < entrou*2 - mapa->dimensaoBlocoV)
						{
							x += entrou*2 - mapa->dimensaoBlocoV-dif;
						}
						// Encerra a verifica��o
						esquerda++;
						break;

					}
				}
				// ajusta pela parte da direita
				if(direita >= 0 && direita<= mapa->largura)
				{
					bloco =mapa->camadas[mapa->camadaMarcas][mapa->largura*meio+direita]-mapa->offbloco; 
					switch(bloco)
					{
					case C2D2M_SOLIDO45:
					case C2D2M_SOLIDO315:
						entrou=y +alt/2- meio*mapa->dimensaoBlocoV;
						if(bloco == C2D2M_SOLIDO45)
							entrou++;
						else
							entrou = mapa->dimensaoBlocoV-entrou;						
						dif = (direita+1)*mapa->dimensaoBlocoH-1-(x+larg); 
						// Aqui colide e ajusta
						if(dif < entrou)
							x -= entrou-dif;
						// Encerra a verifica��o
						direita--;
						break;
					case C2D2M_SOLIDO22B:
					case C2D2M_SOLIDO337B:
						entrou=y +alt/2- meio*mapa->dimensaoBlocoV;
						if(bloco == C2D2M_SOLIDO22B)
							entrou++;
						else
							entrou = mapa->dimensaoBlocoV-entrou;						
						dif = (direita+1)*mapa->dimensaoBlocoH-1-(x+larg); 
						// Aqui colide e ajusta
						if(dif < entrou*2)
						{
							x -= entrou*2-dif;
							//ATOR_InsereEvento(a, EVT_COLIDIU_PAREDE_BAIXO);
						}
						// Encerra a verifica��o
						direita--;
						break;
					case C2D2M_SOLIDO22A:
					case C2D2M_SOLIDO337A:
						entrou=y +alt/2- meio*mapa->dimensaoBlocoV;
						if(bloco == C2D2M_SOLIDO22A)
							entrou++;
						else
							entrou = mapa->dimensaoBlocoV-entrou;						
						dif = (direita+1)*mapa->dimensaoBlocoH-1-(x+larg); 
						// Aqui colide e ajusta
						if(dif < entrou*2-mapa->dimensaoBlocoV)
						{
							//ATOR_InsereEvento(a, EVT_COLIDIU_PAREDE_BAIXO);
							x -= entrou*2-mapa->dimensaoBlocoV-dif;
						}
						// Encerra a verifica��o
						direita--;
						break;

					}

				}
			}
			// Testa a colis�o com bloco completamente s�lido na vertical
			for(int col=esquerda;col<=direita;col++)
			{
				// Testa se est� dentro das coordenadas v�lidas
				if(col < 0 || col >= mapa->largura)
					continue;
				// Testa se ainda est� dentro do mapa acima
				if(topo >= 0 && topo <= mapa->altura && dyl<0)
					// Testa se colidiu
					if(mapa->camadas[mapa->camadaMarcas][mapa->largura*topo+col] == mapa->offbloco + C2D2M_SOLIDO)
					{
						// Se colidiu, aumenta um ponto e encerra
						y++; //dxl;
						//ATOR_InsereEvento(a, EVT_COLIDIU_PAREDE_CIMA);
						break;
					}
				// Testa se ainda est� dentro do mapa abaixo 
				if(baixo >= 0 && baixo <= mapa->altura && dyl>0)
					// Testa se colidiu
					if(mapa->camadas[mapa->camadaMarcas][mapa->largura*baixo+col] == mapa->offbloco + C2D2M_SOLIDO)
					{
						// Se colidiu, diminui um ponto e encerra
						y--; //dxl;
						//ATOR_InsereEvento(a, EVT_COLIDIU_PAREDE_BAIXO);
						break;
					}
			}


		}
		// Teste de ajuste na vertical para quando h� gravidade e velocidade no y
		if(dyl != 0 && (i * *dy)/maior != yant && gravidade)
		{
			yant = (i * *dy)/maior;
			// Desloca
			y += dyl;
			// Calcula o bounding box
			topo = (y)/mapa->dimensaoBlocoV;
			baixo = (y+alt-1)/mapa->dimensaoBlocoV;
			esquerda = (x)/mapa->dimensaoBlocoH;
			direita = (x+larg-1)/mapa->dimensaoBlocoH;
			// Calcula o bloco em que se encontra o meio. Agora, o meio � na horizontal!
			meio = (x+larg/2)/mapa->dimensaoBlocoH;
			// Testa a colis�o com blocos inclinados
			if(meio >=0 && meio <= mapa->largura && mapa->dimensaoBlocoH == mapa->dimensaoBlocoV)
			{
				// ajusta pela parte de baixo
				if(baixo >= 0 && baixo<= mapa->altura)
				{
					bloco =mapa->camadas[mapa->camadaMarcas][mapa->largura*baixo+meio]-mapa->offbloco; 
					switch(bloco)
					{
					case C2D2M_SOLIDO45:
					case C2D2M_SOLIDO135:
						entrou=x+larg/2 - meio*mapa->dimensaoBlocoH;
						if(bloco == C2D2M_SOLIDO45)
							entrou++;
						else
							entrou = mapa->dimensaoBlocoH-entrou;
						dif = (baixo+1)*mapa->dimensaoBlocoV-1-(y+alt);
						// Aqui colide e ajusta
						if(dif < entrou)
							y -= 1;//entrou-dif;
						// N�o verifica a colis�o na horizontal nesta linha
						//baixo--;
						esquerda++;
						break;
					case C2D2M_SOLIDO22A:
					case C2D2M_SOLIDO157B:
						entrou=x+larg/2 - meio*mapa->dimensaoBlocoH;
						if(bloco == C2D2M_SOLIDO22A)
							entrou++;
						else
							entrou = mapa->dimensaoBlocoH-entrou;
						dif = (baixo+1)*mapa->dimensaoBlocoV-1-(y+alt);
						// Aqui colide e ajusta
						if(dif < entrou/2)
							y --;//= entrou/2-dif;
						//baixo--;
						esquerda++;
						break;
					case C2D2M_SOLIDO22B:
					case C2D2M_SOLIDO157A:
						entrou=x+larg/2 - meio*mapa->dimensaoBlocoH;
						if(bloco == C2D2M_SOLIDO22B)
							entrou++;
						else
							entrou = mapa->dimensaoBlocoH-entrou;
						dif = (baixo+1)*mapa->dimensaoBlocoV-1-(y+alt);
						// Aqui colide e ajusta
						if(dif < (entrou+mapa->dimensaoBlocoV)/2)
						{
							y --;//= (entrou+mapa->dimensaoBlocoV)/2-dif;
						}
						// N�o verifica a colis�o na horizontal nesta linha
						//baixo--;
						esquerda++;
						break;
					}
				}

				// ajusta pela parte de cima
				if(topo >= 0 && topo<= mapa->altura)
				{
					bloco =mapa->camadas[mapa->camadaMarcas][mapa->largura*topo+meio]-mapa->offbloco; 
					switch(bloco)
					{
					case C2D2M_SOLIDO225:
					case C2D2M_SOLIDO315:
						entrou=x+larg/2 - meio*mapa->dimensaoBlocoH;
						if(bloco == C2D2M_SOLIDO315)
							entrou++;
						else
							entrou = mapa->dimensaoBlocoH-entrou;
						dif = y - (topo)*mapa->dimensaoBlocoV;
						// Aqui colide e ajusta
						if(dif < entrou)
							y += entrou-dif;
						//topo++;
						direita--;
						break;
					case C2D2M_SOLIDO202B:
					case C2D2M_SOLIDO337A:
						entrou=x+larg/2 - meio*mapa->dimensaoBlocoH;
						if(bloco == C2D2M_SOLIDO337A)
							entrou++;
						else
							entrou = mapa->dimensaoBlocoH-entrou;
						dif = y - topo*mapa->dimensaoBlocoV;
						// Aqui colide e ajusta
						if(dif < entrou/2)
							y += entrou/2-dif;
						// N�o verifica a colis�o na horizontal nesta linha
						//topo++;
						direita--;
						break;
					case C2D2M_SOLIDO202A:
					case C2D2M_SOLIDO337B:
						entrou=x+larg/2 - meio*mapa->dimensaoBlocoH;
						if(bloco == C2D2M_SOLIDO337B)
							entrou++;
						else
							entrou = mapa->dimensaoBlocoH-entrou;
						dif = y - topo*mapa->dimensaoBlocoV;
						// Aqui colide e ajusta
						if(dif < (entrou+mapa->dimensaoBlocoV)/2)
							y += (entrou+mapa->dimensaoBlocoV)/2-dif;

						// N�o verifica a colis�o na horizontal nesta linha
						//topo++;
						direita--;
						break;
					}
				}

			}
			// Testa a colis�o com bloco completamente s�lido na vertical
			for(int col=esquerda;col<=direita;col++)
			{
				// Testa se est� dentro das coordenadas v�lidas
				if(col < 0 || col >= mapa->largura)
					continue;
				// Testa se ainda est� dentro do mapa acima
				if(topo >= 0 && topo <= mapa->altura && dyl<0)
					// Testa se colidiu
					if(mapa->camadas[mapa->camadaMarcas][mapa->largura*topo+col] == mapa->offbloco + C2D2M_SOLIDO)
					{
						// Se colidiu, aumenta um ponto e encerra
						y++; //dxl;
						//ATOR_InsereEvento(a, EVT_COLIDIU_PAREDE_CIMA);
						break;
					}
				// Testa se ainda est� dentro do mapa abaixo 
				if(baixo >= 0 && baixo <= mapa->altura && dyl>0)
					// Testa se colidiu
					if(mapa->camadas[mapa->camadaMarcas][mapa->largura*baixo+col] == mapa->offbloco + C2D2M_SOLIDO)
					{
						// Se colidiu, diminui um ponto e encerra
						y--; //dxl;
						//ATOR_InsereEvento(a, EVT_COLIDIU_PAREDE_BAIXO);
						break;
					}
			}


		}



		// Para cada passo, verifica as colis�es
		//ATOR_ColideBoundingBox(a, mapa);

	}
	// Atualiza o dx e dy
	*dx = x-xorig;
	*dy = y-yorig;
	return;// (cx || cy);
}

// Para um mapa, calcula a velocidade de um corpo de acordo com o tempo total da queda
// expresso em quadros (60 quadros por segundo)
bool C2D2M_CalculaQueda(unsigned int idMapa, int quadrosQueda, double *vqueda)
{
	if(idMapa == 0 || idMapa>=C2D2M_MAX_MAPAS)
		return false;
	// SE o mapa n�o foi inicializado, ou se a camada de marcas est� na default (-1)
	if(!mapas[idMapa-1].inicializado || mapas[idMapa-1].camadaMarcas==-1)
		return false;
	
	// Recupera o mapa
	C2D2M_Mapa *mapa = &mapas[idMapa-1];
	// Se a gravidade do mapa for zerada ...
	if(mapa->gravidade == 0)
		return false;
	
    // Calcula o tempo, considerando 60 quadros por segundo
    double tempo = quadrosQueda*1.0/60.0;
    // Calcula a velocidade. 
	*vqueda = mapa->gravidade*tempo*tempo;
    // Limita a velocidade ("atrito com o ar")
	if(*vqueda > mapa->maxgravidade/60.0)	
        *vqueda = mapa->maxgravidade/60.0;
    // Retorna o resultado
    return true;
}

// Testa se um bounding box colide com um bloco qualquer de cen�rio.
//
// Data: 28/08/2008
//
bool C2D2M_ColidiuBlocoCenario(unsigned int idMapa, int x, int y, int larg, int alt, int bloco)
{
	if(idMapa == 0 || idMapa>=C2D2M_MAX_MAPAS)
		return false;
	// SE o mapa n�o foi inicializado, ou se a camada de marcas est� na default (-1)
	if(!mapas[idMapa-1].inicializado || mapas[idMapa-1].camadaMarcas==-1)
		return false;
	
	// Recupera o mapa
	C2D2M_Mapa *mapa = &mapas[idMapa-1];
	// Intervalo de blocos em que verificamos a colis�o
	int topo, baixo, esquerda, direita;
	// Calcula o extremo esquerdo
	topo = y/mapa->dimensaoBlocoV;
	esquerda = x/mapa->dimensaoBlocoH;
	// Calcula o extremo direito
	baixo = (y+alt-1)/mapa->dimensaoBlocoV;
	direita = (x+larg-1)/mapa->dimensaoBlocoH;

	// Indica se houve colisao
	bool cxy=false;

	// Para cada bloco no intervalo
	for(int lin=topo;lin<=baixo && ! cxy;lin++)
		for(int col=esquerda;col<=direita && !cxy ;col++)
		{
			// O bloco � v�lido para colis�o?
			// Ele � SE:
			//			- Ele est� dentro do mapa
			//		- Se ele � bloco (ele pode ser colidido)
			// Testa ent�o se est� dentro do mapa
			if(lin< 0 || lin >= mapa->altura || col < 0 || col >= mapa->largura)
				continue;
			// Testa se ele � v�lido
			if(mapa->camadas[mapa->camadaMarcas][mapa->largura*lin+col]==mapa->offbloco+bloco)
				cxy = true;
		}
	return cxy;
}



// M�todo para verificar se o cabe�alho do FMP/FMA � correto
//
// Data: 26/01/2005
// �ltima atualiza��o: 29/07/2007

bool C2D2M_VerificaCabecalhoArquivo(CabecalhoArquivo *cabecalho)
{
    // Est� certo por defini��o
    bool ok=true;
    // Verifica se tem o FORM no come�o
    for(int i=0;i<4;i++)
        if(cabecalho->tipo[i] != cabecalhos[0][i])
        {
            ok=false;
            break;
        }
    if(!ok)
        return ok;
    // Verifica se tem o FMAP no subtipo
    for(int i=0;i<4;i++)
        if(cabecalho->subtipo[i] != cabecalhos[1][i])
        {
            ok=false;
            break;
        }
    return ok;
}

// Fun��o para verificar o tipo do cabe�alho. Retorna um dos tipos enumerados
//
// Data: 26/01/2005
// �ltima atualiza��o: 29/07/2007

int C2D2M_TipoBloco(CabecalhoBloco *cabecalho)
{    
    // Verifica se � o bloco do MPHD no come�o
    bool ok=true;
    for(int i=0;i<4;i++)
        if(cabecalho->tipo[i] != cabecalhos[2][i])
        {
            ok=false;
            break;
        }
    if(ok)
        return CABECALHO_MPHD;
    // Verifica se � o bloco do ANDT no come�o
    ok=true;
    for(int i=0;i<4;i++)
        if(cabecalho->tipo[i] != cabecalhos[3][i])
        {
            ok=false;
            break;
        }
    if(ok)
        return CABECALHO_ANDT;
    // Verifica se � o bloco do BODY no come�o
    ok=true;
    for(int i=0;i<4;i++)
        if(cabecalho->tipo[i] != cabecalhos[4][i])
        {
            ok=false;
            break;
        }
    if(ok)
        return CABECALHO_BODY;
    // Verifica se � uma camada (o quarto caracter � o n�mero da camada)
    ok=true;
    for(int i=0;i<3;i++)
        if(cabecalho->tipo[i] != cabecalhos[5][i])
        {
            ok=false;
            break;
        }
    if(ok)
        return CABECALHO_LYR;  
    
    // Por default
    return CABECALHO_IGNORADO;
}
