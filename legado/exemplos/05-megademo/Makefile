objetos = megademo.o

libs = -lSDL -lSDL_mixer -lSDL_image -lGL -lGLU -lc2d2 -lca2

comp = g++

main : $(objetos)
	$(comp) -o megademo $(objetos) $(libs)

megademo.o :
	$(comp) -c megademo.cpp
clean :
	rm megademo $(objetos)
