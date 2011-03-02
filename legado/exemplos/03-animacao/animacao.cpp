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
#include <c2d2/chien2d2.h>
#include <c2d2/chien2d2primitivas.h>

int main(int narg, char **valarg)
{
	// Inicia a Chien2D 2 e testa se deu tudo certo
	if(C2D2_Inicia(800, 600, C2D2_JANELA, C2D2_DESENHO_PADRAO, "Animação"))
		printf("Iniciou a Chien2D 2 com sucesso\n");
	else
	{
		printf("Falhou ao iniciar a Chien2D 2. Encerrando o programa.\n");
		return 0;
	}
	C2D2P_Inicia();
	// Cria o personagem do jogo
	unsigned int spDark = C2D2_CarregaSpriteSet("darkphoenix.png", 32, 42);
	// Carrega a fonte
	unsigned int fonte = C2D2_CarregaFonte("isabelle64.png", 64);

	// Recupera o teclado
	C2D2_Botao *teclas = C2D2_PegaTeclas();
	// Recupera o mouse
	C2D2_Mouse *mouse = C2D2_PegaMouse();
	
	// Troca a cor de fundo da tela
	C2D2_TrocaCorLimpezaTela(0, 0, 0);

	// Variáveis para animar a reta
	int yreta=0;
	int vreta=4;
	// Variáveis para animar o texto
	int vtexto=-8;
	int ltexto;
	C2D2_DimensoesTexto(fonte, "Demonstração de texto animado com a Chien2D 2.", &ltexto, 0);
	int xtexto=799+ltexto;
	// Variáveis para animar o personagem
	int xpers = -32;
	int qpers[4] = {0, 1, 2, 1};
	int qatual=0;
	int tquadro=10;
	do
	{
		// Faz a lógica da reta
		yreta += vreta;
		if((yreta<=0 && vreta<0)|| (yreta>=599 && vreta >0))
			vreta *= -1;
		// Faz a lógica do texto animado
		xtexto += vtexto;
		if(xtexto <= 0)
			xtexto=799+ltexto;
		// Faz a lógica para que o personagem ande
		xpers+=2;
		if(xpers>799)
			xpers=-32;
		// Faz a lógica da animação do personagem
		if(--tquadro == 0)
		{
			qatual = (qatual+1)%4;
			tquadro = 10;
		}

		// Limpa a tela com a cor de fundo atual
		C2D2_LimpaTela();

		// Desenha a reta animada
		C2D2P_Linha(0, yreta, 799, yreta, 255, 255, 255);
		// Desenha o texto em movimento
		C2D2_DesenhaTexto(fonte, xtexto, 530, "Demonstração de texto animado com a Chien2D 2.", C2D2_TEXTO_DIREITA); 
		// Desenha o sprite
		C2D2_DesenhaSprite(spDark, qpers[qatual], xpers, 268);

		// Faz a troca do front buffer com o back buffer e atualiza as entradas
		C2D2_Sincroniza(C2D2_FPS_PADRAO);
	}
	while(!teclas[C2D2_ENCERRA].pressionado && !teclas[C2D2_ESC].pressionado);
	// Remove as imagens
	C2D2_RemoveSpriteSet(spDark);
	// Remove a fonte
	C2D2_RemoveFonte(fonte);

	// Encerra a Chien2D 2
	C2D2_Encerra();
	return 0;
}
