objetos = main_plataforma.o jogo_darkphoenix.o jogo_bola.o 

libs = -lSDL -lSDL_mixer -lSDL_image -lGL -lGLU -lc2d2 -lca2 -lcator -lcm2

comp = g++ -g

08-plataforma : $(objetos)
	$(comp) -o 08-plataforma $(objetos) $(libs) -Wl,--rpath=\$$ORIGIN/libs

main_plataforma.o :
	$(comp) -c main_plataforma.cpp
jogo_darkphoenix.o : jogo_darkphoenix.cpp jogo_darkphoenix.h
	$(comp) -c jogo_darkphoenix.cpp
jogo_bola.o : jogo_bola.cpp jogo_bola.h
	$(comp) -c jogo_bola.cpp
clean :
	rm 08-plataforma $(objetos)
