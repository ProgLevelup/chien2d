objetos = microdemo.o

libs = -lSDL -lSDL_mixer -lSDL_image -lGL -lGLU -lc2d2 -lca2

comp = g++

main : $(objetos)
	$(comp) -o microdemo $(objetos) $(libs)

microdemo.o :
	$(comp) -c microdemo.cpp
clean :
	rm microdemo $(objetos)
