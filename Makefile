.PHONY: all run clean

CC     := gcc
CFLAGS := -Wall -Wextra -O3 -std=c99
SRC    := src/main.c src/math3d.c src/shapes.c src/render.c src/input.c src/menu.c

all:
	mkdir -p build
	$(CC) $(CFLAGS) $(SRC) -o build/spinn -lm

run:
	mkdir -p build
	$(CC) $(CFLAGS) $(SRC) -o build/spinn -lm
	./build/spinn

clean:
	rm -rf build

