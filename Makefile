CC = clang++
CFLAGS = -ggdb -std=c++11 -pedantic -Wall -Wextra $(shell pkg-config --cflags sdl)
LIBS = $(shell pkg-config --libs sdl SDL_image) -lm

all: level.lvl main

level.lvl: level
	./make_level level

Level.o: Level.h Level.cpp
	$(CC) -c $(CFLAGS) Level.cpp

Player.o: Player.h Player.cpp
	$(CC) -c $(CFLAGS) Player.cpp

World.o: Level.h Player.h World.h World.cpp
	$(CC) -c $(CFLAGS) World.cpp

main.o: World.h main.cpp
	$(CC) -c $(CFLAGS) main.cpp

main: main.o Level.o Player.o World.o
	$(CC) main.o Level.o Player.o World.o $(LIBS) -o main

clean:
	rm -f *.o main level.lvl
