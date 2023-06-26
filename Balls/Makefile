CFLAGS = -Wall -Wextra -g
CC = gcc
TARGET:test.exe

OBJS = 
LIBS = -lcsfml-graphics -lcsfml-window -lcsfml-system -lm

test.exe:main.o ${OBJS}
	${CC} ${CFLAGS} main.o ${OBJS} -o test.exe  ${LIBS}


main.o:main.c
	${CC} ${CFLAGS} -c main.c -o main.o  ${LIBS}


clean:
	rm -f *.o
	rm -f *.exe

run:
	make clean >/dev/null
	make
	./test.exe
