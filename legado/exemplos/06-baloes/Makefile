objetos = baloes.o

libs = -lSDL -lSDL_mixer -lSDL_image -lGL -lGLU -lc2d2 -lca2

comp = g++

baloes : $(objetos)
	$(comp) -o baloes $(objetos) $(libs)

baloes.o :
	$(comp) -c baloes.cpp
clean :
	rm baloes $(objetos)
