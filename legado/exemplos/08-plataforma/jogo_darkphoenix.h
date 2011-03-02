#ifndef JOGO_DARKPHOENIX_H
#define JOGO_DARKPHOENIX_H

#include <c2d2/ator.h>

#define VANDA	4
#define VPULO   1000


// Enumeração dos estados da atriz do jogo
enum {DARK_INICIO=ATOR_PROG_INI, DARK_PARADO, DARK_ANDANDO, DARK_PULANDO,
DARK_CAINDO, DARK_VITORIA, DARK_MORRENDO, DARK_MORREU};

// A função que carrega o personagem atriz (pra facilitar).
//
bool JOGO_CarregaDarkPhoenix();
// A função para fazer a lógica da atriz
bool AtualizaDarkPhoenix(Ator *a, unsigned int mapa);


#endif
