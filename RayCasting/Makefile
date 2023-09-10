CFLAGS = -Wall -Wextra
CC = g++
LDFLAGS = -lSDL2 -lSDL2_image
TARGET:rayCasting.exe

OBJS = game.o map.o player.o raycaster.o entities_manager.o weapon.o hud.o

rayCasting.exe:main.o ${OBJS}
	${CC} ${CFLAGS} main.o ${OBJS} $(LDFLAGS) -o rayCasting.exe

main.o:main.cpp
	${CC} ${CFLAGS} -c main.cpp -o main.o

game.o:game.cpp
	${CC} ${CFLAGS} -c game.cpp -o game.o

player.o:player.cpp
	${CC} ${CFLAGS} -c player.cpp -o player.o

map.o:map.cpp
	${CC} ${CFLAGS} -c map.cpp -o map.o

raycaster.o:raycaster.cpp
	${CC} ${CFLAGS} -c raycaster.cpp -o raycaster.o

entities_manager.o:entities_manager.cpp
	${CC} ${CFLAGS} -c entities_manager.cpp -o entities_manager.o

weapon.o:weapon.cpp
	${CC} ${CFLAGS} -c weapon.cpp -o weapon.o

hud.o:hud.cpp
	${CC} ${CFLAGS} -c hud.cpp -o hud.o

clean:
	rm -f *.o
	rm -f *.exe
	rm -f index.*

run:
	make clean >/dev/null
	make
	./rayCasting.exe

emcc:
	em++ --bind main.cpp game.cpp map.cpp player.cpp raycaster.cpp entities_manager.cpp weapon.cpp hud.cpp -s USE_SDL=2 -s USE_SDL_IMAGE=2 -s SDL2_IMAGE_FORMATS='["png"]' -s ALLOW_MEMORY_GROWTH=1 -s STACK_SIZE=10000000 --preload-file ressources -o index.html -std=c++17 -fsanitize=undefined
	python3 -m http.server