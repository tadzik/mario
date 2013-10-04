#ifndef LEVEL_H
#define LEVEL_H

#include <SDL.h>

#define HEIGHT 12
#define WIDTH  16

class Level {
    int w, h;
    char *data;
    Uint32 grass_colour;
    Uint32 brick_colour;
    Uint32 sky_colour;
    int rectw, recth;

    char at(int, int);
    Uint32 colour(char);

public:
    int offset, max_offset;
    Level(SDL_Surface *);
    bool load(const char *);
    void draw(SDL_Surface *);
    bool legal_pos(int, int);

    ~Level() { free(data); }
};

#endif
