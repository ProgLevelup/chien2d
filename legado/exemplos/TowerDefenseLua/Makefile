objetos = TowerDefense.o atualizar.o inicializar.o

libs = -lSDL -lSDL_mixer -lSDL_image -lGL -lGLU -lc2d2 -lca2 -lcator -lcm2 -llua5.1 -lc2d2lua

comp = g++ -g -D __USES_LUA__

main : $(objetos)
	$(comp) -o TowerDefense $(objetos) $(libs)

TowerDefense.o :
	$(comp) -c TowerDefense.cpp
atualizar.o :
	$(comp) -c atualizar.cpp
inicializar.o :
	$(comp) -c inicializar.cpp
#mainPersonagensMovendo.o :
#	$(comp) -c mainPersonagensMovendo.cpp
clean :
	rm TowerDefense $(objetos)
