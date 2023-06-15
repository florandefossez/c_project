CFLAGS = -Wall -Wextra
CC = gcc
TARGET:test.exe

OBJS = ether.o

test.exe:main.o ${OBJS}
	${CC} ${CFLAGS} main.o ${OBJS} -o test.exe

main.o:main.c
	${CC} ${CFLAGS} -c main.c -o main.o

ether.o:ether.c
	${CC} ${CFLAGS} -c -I . ether.c -o ether.o


clean:
	rm -f *.o
	rm -f *.exe
	./delete_tap.sh

run:
	make clean >/dev/null
	make
	./create_tap.sh
	./test.exe
