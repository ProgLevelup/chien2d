objetos = animacao.o

libs = -lSDL -lSDL_image -lGL -lGLU -lc2d2

comp = g++

animacao : $(objetos)
	$(comp) -o animacao $(objetos) $(libs)

animacao.o :
	$(comp) -c animacao.cpp
clean :
	rm animacao $(objetos)	
