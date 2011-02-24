objetos = tilemap.o

libs = -lSDL -lSDL_mixer -lSDL_image -lGL -lc2d2 -lca2 -lcm2

comp = g++

tilemap : $(objetos)
	$(comp) -o tilemap $(objetos) $(libs)

tilemap.o :
	$(comp) -c tilemap.cpp
clean :
	rm tilemap $(objetos)
