TARGET=build/app.exe
LIBS=-lm -lcglm -lglfw
CC=gcc
CFLAGS=-g -Wall -Wextra

.PHONY: all default clean run

default: run
all: default

OBJECTS=$(patsubst src/%.c, build/%.o, $(wildcard src/*.c))
HEADERS=$(wildcard src/*.h)

build/%.o: src/%.c
	@mkdir -p $(@D)
	$(CC) $(CFLAGS) -c $< -o $@

$(TARGET): $(OBJECTS)
	$(CC) $(OBJECTS) $(CFLAGS) $(LIBS) -o $@

run: $(TARGET)
	./build/app.exe


clean:
	@rm -rf build