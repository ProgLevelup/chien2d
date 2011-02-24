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
	if(C2D2_Inicia(800, 600, C2D2_JANELA, C2D2_DESENHO_OPENGL, "Primeiro exemplo"))
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

	// Limpa a tela com a cor de fundo atual
	C2D2_LimpaTela();
	// Desenha os sprites
	C2D2_DesenhaSprite(fundo, 0, 0, 0);
	C2D2_DesenhaSprite(spDark, 1, 100, 100);
	// Faz a troca do front buffer com o back buffer
	C2D2_Sincroniza(C2D2_FPS_PADRAO);
	// Aguarda 2 segundos
	C2D2_Pausa(2000);

	// Remove as imagens
	C2D2_RemoveSpriteSet(spDark);
	C2D2_RemoveSpriteSet(fundo);

	// Encerra a Chien2D 2
	C2D2_Encerra();
	return 0;
}
