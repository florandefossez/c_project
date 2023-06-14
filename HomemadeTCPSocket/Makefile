CFLAGS = -Wall -Wextra
CC = gcc
TARGET:test.exe

OBJS = main.o

test.exe: ${OBJS}
	${CC} ${CFLAGS} ${OBJS} -o test.exe


clean:
	rm -f *.o
	rm -f *.exe
	./delete_tap.sh

run:
	make
	./create_tap.sh
	./test.exe
