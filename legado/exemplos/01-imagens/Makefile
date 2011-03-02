objetos = imagens.o

libs = -lSDL -lSDL_image -lGL -lGLU -lc2d2

comp = g++

imagens : $(objetos)
	$(comp) -o imagens $(objetos) $(libs)

imagens.o :
	$(comp) -c imagens.cpp
clean :
	rm imagens $(objetos)	
