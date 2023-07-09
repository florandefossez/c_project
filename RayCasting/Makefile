CFLAGS = -Wall -Wextra
CC = g++
LDFLAGS = -lsfml-graphics -lsfml-window -lsfml-system
TARGET:rayCasting.exe

OBJS = game.o player.o

rayCasting.exe:main.o ${OBJS}
	${CC} ${CFLAGS} main.o ${OBJS} $(LDFLAGS) -o rayCasting.exe

main.o:main.cpp
	${CC} ${CFLAGS} -c main.cpp -o main.o

game.o:game.cpp
	${CC} ${CFLAGS} -c game.cpp -o game.o

player.o:player.cpp
	${CC} ${CFLAGS} -c player.cpp -o player.o

clean:
	rm -f *.o
	rm -f *.exe

run:
	make clean >/dev/null
	make
	./rayCasting.exe