#ifndef JOGO_BOLA_H
#define JOGO_BOLA_H

#include <c2d2/ator.h>

//#define VANDAB	3
#define VPULO   1000


// Enumeração dos estados da atriz do jogo
enum {BOLA_PULANDO=ATOR_PROG_INI, BOLA_MORRENDO};

// A função que carrega o personagem atriz (pra facilitar).
//
bool JOGO_CarregaBola();
// A função para fazer a lógica da atriz
bool AtualizaBola(Ator *a, unsigned int mapa);


#endif
