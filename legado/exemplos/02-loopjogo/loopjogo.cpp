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

int main(int narg, char **valarg)
{
	// Inicia a Chien2D 2 e testa se deu tudo certo
	if(C2D2_Inicia(800, 600, C2D2_JANELA, C2D2_DESENHO_PADRAO, "Primeira aula"))
		printf("Iniciou a Chien2D 2 com sucesso\n");
	else
	{
		printf("Falhou ao iniciar a Chien2D 2. Encerrando o programa.\n");
		return 0;
	}
	// Cria o personagem do jogo
	unsigned int spDark = C2D2_CarregaSpriteSet("darkphoenix.png", 32, 42);
	// Carrega a imagem
	unsigned int fundo = C2D2_CarregaSpriteSet("imagem.jpg", 0,0);
	// Carrega a fonte
	unsigned int fonte = C2D2_CarregaFonte("isabelle64.png", 64);

	// Recupera o teclado
	C2D2_Botao *teclas = C2D2_PegaTeclas();
	// Recupera o mouse
	C2D2_Mouse *mouse = C2D2_PegaMouse();

	int xtecla=100, ytecla=100, xmouse, ymouse;

	do
	{
		// Executa a lógica do personagem do mouse
		xmouse = mouse->x;
		ymouse = mouse->y;
		// Executa a lógica do personagem do teclado
		if(teclas[C2D2_CIMA].ativo)
			ytecla-=2;
		else if(teclas[C2D2_BAIXO].ativo)
			ytecla+=2;
		if(teclas[C2D2_ESQUERDA].ativo)
			xtecla-=2;
		else if(teclas[C2D2_DIREITA].ativo)
			xtecla+=2;

		// Limpa a tela com a cor de fundo atual
		C2D2_LimpaTela();
		// Desenha os sprites
		C2D2_DesenhaSprite(fundo, 0, 0, 0);
		C2D2_DesenhaSprite(spDark, 1, xtecla, ytecla);
		C2D2_DesenhaSprite(spDark, 4, xmouse, ymouse);
		// Desenha o texto
		C2D2_DesenhaTexto(fonte, 400,300, "Hello Chien2D 2!", C2D2_TEXTO_CENTRALIZADO);
		// DE acordo com o botão do mouse
		if(mouse->botoes[C2D2_MESQUERDO].pressionado)
			C2D2_DesenhaTexto(fonte, 20, 530, "Botão esquerdo foi pressionado.", C2D2_TEXTO_ESQUERDA);
		else if(mouse->botoes[C2D2_MESQUERDO].ativo)
			C2D2_DesenhaTexto(fonte, 20, 530, "Botão esquerdo ativo.",C2D2_TEXTO_ESQUERDA);
		else if(mouse->botoes[C2D2_MESQUERDO].liberado)
			C2D2_DesenhaTexto(fonte, 20, 530, "Botão esquerdo foi liberado.", C2D2_TEXTO_ESQUERDA);
		else
			C2D2_DesenhaTexto(fonte, 20, 530, "Botão esquerdo inativo.", C2D2_TEXTO_ESQUERDA);
		// Testa se os sprites colidiram. Note que são dois grupos de valores, idênticos aos
		// usados para desenhar um sprite.
		if(C2D2_ColidiuSprites(spDark, 1, xtecla, ytecla, spDark, 4, xmouse, ymouse))
			C2D2_DesenhaTexto(fonte, 400, 30, "Os sprites colidiram.", C2D2_TEXTO_CENTRALIZADO);
		// Faz a troca do front buffer com o back buffer e atualiza as entradas
		C2D2_Sincroniza(C2D2_FPS_PADRAO);
	}
	while(!teclas[C2D2_ENCERRA].pressionado && !teclas[C2D2_ESC].pressionado);
	// Remove as imagens
	C2D2_RemoveSpriteSet(spDark);
	C2D2_RemoveSpriteSet(fundo);
	// Remove a fonte
	C2D2_RemoveFonte(fonte);

	// Encerra a Chien2D 2
	C2D2_Encerra();
	return 0;
}
