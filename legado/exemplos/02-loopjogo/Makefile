objetos = loopjogo.o

libs = -lSDL -lSDL_image -lGL -lGLU -lc2d2

comp = g++

loopjogo : $(objetos)
	$(comp) -o loopjogo $(objetos) $(libs)

loopjogo.o :
	$(comp) -c loopjogo.cpp
clean :
	rm loopjogo $(objetos)	
