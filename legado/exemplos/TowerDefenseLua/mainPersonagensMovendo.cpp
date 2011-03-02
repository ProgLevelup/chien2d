#include <c2d2/ator.h>
#include <c2d2/chien2d2.h>

// Enumeração de atores
enum {ENEMY_DEATH, ENEMY_GHOST, ENEMY_ZOMBIE, ENEMY_SKEL, ENEMY_BOW_SKEL, ENEMY_MUSHYMAN};

// Estados da nave
enum {ENEMY_IDLE=ATOR_PROG_INI};

// Vetor com as animações
//dados\\sprites.png
Animacao animax[] ={
	// Ordem: número de quadros, tempo entre os quadros, vetor com a seqüência de quadros
	// Todos os frames
	{8, 10, {19, 20, 21, 22, 23, 24, 25, 26}},
	//Andando
	{2, 10, {19, 20}},
	//Lutando
	{2, 10, {21, 22}},
	//Lutando 2
	{2, 10, {23, 24}},
	//Morrendo
	{2, 10, {25, 26}}
};

Animacao anima[] ={
	// Ordem: número de quadros, tempo entre os quadros, vetor com a seqüência de quadros
	// Atacando
	{13, 6, {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12}},
	//Desaparecendo
	{9, 6, {13, 14, 15, 16, 17, 18, 19, 20, 21}},
	//Andando
	{8, 6, {22, 23, 24, 25, 26, 27, 28, 29}}

};
Animacao anima2[] ={
	// Ordem: número de quadros, tempo entre os quadros, vetor com a seqüência de quadros
	// Atacando
	{13, 6, {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 10, 10}},
	//Desaparecendo
	{9, 6, {11, 12, 13, 14, 15, 16, 17, 18, 19}},
	//Andando
	{8, 6, {20, 21, 22, 23, 24, 25, 26, 27}}

};

Animacao anima3[] ={
	// Ordem: número de quadros, tempo entre os quadros, vetor com a seqüência de quadros
	// Atacando
	{13, 6, {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 10, 10}},
	//Desaparecendo
	{9, 6, {11, 12, 13, 14, 15, 16, 17, 17, 17}},
	//Andando
	{8, 6, {18, 19, 20, 21, 22, 23, 24, 25}}

};

// Protótipo de funções

bool atualizaInimigos(Ator *a, unsigned int mapa);

// Funções

bool atualizaInimigos(Ator *a, unsigned int mapa)
{
	switch(a->estado.estado)
	{
	case ATOR_NASCENDO:
		// Muda para o estado adequado
		ATOR_TrocaEstado(a, ENEMY_IDLE, false);
		break;
	case ENEMY_IDLE:
		if(a->estado.subestado==ESTADO_INICIO)
		{
			// coloca a animação
			ATOR_TrocaAnimacao(a, 1);
			// Troca o sub-estado
			a->estado.subestado=ESTADO_RODANDO;
		}
		break;
	}
	return true;
}

int main(int ac, char **av)
{
	if(!C2D2_Inicia(800, 600, C2D2_JANELA, C2D2_DESENHO_OPENGL, "Projeto 1"))
	{
		printf("Nao conseguiu iniciar a tela. Encerrando.\n");
		return 1;
	}

	// Inicia os personagens
	ATOR_Inicia();

	// Registra os atores no sistema
	bool result = ATOR_CarregaAtorEstatico(ENEMY_DEATH, "dados/morte.png", 128, 128, 0, 0, 128, 128, anima, true,
		0, 0, &atualizaInimigos);

	if (result)
		result = ATOR_CarregaAtorEstatico(ENEMY_GHOST, "dados/fantasma.png", 96, 96, 0, 0, 96, 96, anima, true,
			0, 0, &atualizaInimigos);

	if (result)
		result = ATOR_CarregaAtorEstatico(ENEMY_ZOMBIE, "dados/zumbi.png", 96, 96, 0, 0, 96, 96, anima2, true,
			0, 0, &atualizaInimigos);

	if (result)
		result = ATOR_CarregaAtorEstatico(ENEMY_SKEL, "dados/esqueleto.png", 96, 96, 0, 0, 96, 96, anima2, true,
			0, 0, &atualizaInimigos);

	if (result)
		result = ATOR_CarregaAtorEstatico(ENEMY_BOW_SKEL, "dados/esqueleto_arco.png", 96, 96, 0, 0, 96, 96, anima, true,
			0, 0, &atualizaInimigos);

	if (result)
		result = ATOR_CarregaAtorEstatico(ENEMY_MUSHYMAN, "dados/cogumelo.png", 96, 96, 0, 0, 96, 96, anima3, true,
			0, 0, &atualizaInimigos);

	if(result)
	{
		printf("Carregou");
		// Cria o ator
		Ator *death = ATOR_CriaAtor(ENEMY_DEATH, 100, 300, 0);
		Ator *ghost = ATOR_CriaAtor(ENEMY_GHOST, 200, 300, 0);
		Ator *zombie = ATOR_CriaAtor(ENEMY_ZOMBIE, 300, 300, 0);
		Ator *skel = ATOR_CriaAtor(ENEMY_SKEL, 400, 300, 0);
		Ator *bow_skel = ATOR_CriaAtor(ENEMY_BOW_SKEL, 500, 300, 0);
		Ator *mushyman = ATOR_CriaAtor(ENEMY_MUSHYMAN, 600, 300, 0);

		C2D2_Botao *teclado = C2D2_PegaTeclas();
		C2D2_TrocaCorLimpezaTela(106, 76, 48);
		while(!teclado[C2D2_ESC].pressionado && !teclado[C2D2_ENCERRA].pressionado)
		{
			C2D2_LimpaTela();
			//ATOR_AplicaEstado(death, 0, 800, 600);
			ATOR_AplicaEstado(ghost, 0, 800, 600);
			ATOR_AplicaEstado(zombie, 0, 800, 600);
			ATOR_AplicaEstado(skel, 0, 800, 600);
			ATOR_AplicaEstado(bow_skel, 0, 800, 600);
			ATOR_AplicaEstado(mushyman, 0, 800, 600);
			//ATOR_Atualiza(death, 0);
			ATOR_Atualiza(ghost, 0);
			ATOR_Atualiza(zombie, 0);
			ATOR_Atualiza(skel, 0);
			ATOR_Atualiza(bow_skel, 0);
			ATOR_Atualiza(mushyman, 0);
			//ATOR_Desenha(death, 0, 0, 0);
			ATOR_Desenha(ghost, 0, 0, 0);
			ATOR_Desenha(zombie, 0, 0, 0);
			ATOR_Desenha(skel, 0, 0, 0);
			ATOR_Desenha(bow_skel, 0, 0, 0);
			ATOR_Desenha(mushyman, 0, 0, 0);
			C2D2_Sincroniza();
		}
		// elimina o personagem
		free(death);
		free(ghost);
		free(zombie);
		// descarrega os dados
		ATOR_DescarregaAtor(ENEMY_DEATH);
		ATOR_DescarregaAtor(ENEMY_GHOST);
		ATOR_DescarregaAtor(ENEMY_ZOMBIE);

	}

	// Encerra
	ATOR_Encerra();
	return 0;
}
