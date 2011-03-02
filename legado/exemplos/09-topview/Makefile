objetos = main_fase.o jogo_atriz.o

libs = -lSDL -lSDL_mixer -lSDL_image -lGL -lGLU -lc2d2 -lcm2 -lcator -lca2 

comp = g++ -g

09-topview : $(objetos)
	$(comp) -o 09-topview $(objetos) $(libs)

main_fase.o :
	$(comp) -c main_fase.cpp
jogo_atriz.o : jogo_atriz.cpp jogo_atriz.h
	$(comp) -c jogo_atriz.cpp
clean :
	rm 09-topview $(objetos)
