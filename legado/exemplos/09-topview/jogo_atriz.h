#ifndef JOGO_ATRIZ_H
#define JOGO_ATRIZ_H

#include <c2d2/ator.h>

#define VATRIZ	2


// Enumeração dos estados da atriz do jogo
enum {ATRIZ_INVULNERAVEL=ATOR_PROG_INI, ATRIZ_PARADA, ATRIZ_ANDANDO, 
		ATRIZ_ATIRANDO, ATRIZ_VENCEU, ATRIZ_MORRENDO};

// A função que carrega a atriz (pra facilitar). DEscarregar não precisa assim por usar só
// o identificador, que o jogo tem que conhecer de qualquer maneira
//
// 0,5 ponto na nota pra quem explicar em detalhes a vantagem de ter essa função AQUI
//
bool JOGO_CarregaAtriz();
// A função para fazer a lógica da atriz
bool AtualizaAtriz(Ator *a, unsigned int mapa);


#endif
