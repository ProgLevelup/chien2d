/*
Copyright 2009-2010, Fabio Vinicius Binder (fbinder@gmail.com)

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

#ifndef __CONSTANTES_H__
#define __CONSTANTES_H__


#include <c2d2/ator.h>

// Enumeração de atores
enum {INIMIGO_COGUMELO, INIMIGO_ESQUELETO, INIMIGO_FANTASMA, INIMIGO_ZUMBI, BONUS_10, MIRA, ARQUEIRO};

// Estados Inimigos
enum {ESTADO_INIMIGO_INATIVO=ATOR_PROG_INI, ESTADO_INIMIGO_ANDANDO, ESTADO_INIMIGO_BATENDO, ESTADO_INIMIGO_MORRENDO};

// Estados Bônus
enum {ESTADO_BONUS_INICIO=ATOR_PROG_INI};

// Estados Mira
enum {ESTADO_MIRA_INICIO=ATOR_PROG_INI};

// Estados Arqueiro
enum {ESTADO_ARQUEIRO_INICIO=ATOR_PROG_INI};

// Animações
enum {ANIMA_INIMIGO_BATENDO, ANIMA_INIMIGO_MORRENDO, ANIMA_INIMIGO_ANDANDO};

enum {ANIMA_ARQUEIRO_ATIRANDO };

#define MAX_INIMIGOS 4

#define QTDE_DANO 100

#endif
