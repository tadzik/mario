CC = clang++
CFLAGS = -ggdb -std=c++11 -pedantic -Wall -Wextra $(shell pkg-config --cflags sdl)
LIBS = $(shell pkg-config --libs sdl SDL_gfx SDL_image) -lm -lSDL_ttf -lSDL_mixer

all: level.lvl main

level.lvl: level
	./make_level level

main: main.cpp
	$(CC) -c $(CFLAGS) main.cpp
	$(CC) $(LIBS) main.o -o main

clean:
	rm -f *.o main level.lvl
