#include "Level.h"
#include <ctype.h>

Level::Level(SDL_Surface *screen) : w(0), h(0), data(nullptr) {
    sky_colour = SDL_MapRGB(screen->format, 100, 100, 255);
    grass_colour = SDL_MapRGB(screen->format, 100, 255, 100);
    brick_colour = SDL_MapRGB(screen->format, 255, 100, 100);
    rectw = screen->w / WIDTH;
    recth = screen->h / HEIGHT;
}

void Level::draw(SDL_Surface *screen)
{
    int off = offset / rectw;
    SDL_Rect piecerect = { 0, 0, static_cast<Uint16>(rectw + offset),
                                 static_cast<Uint16>(recth) };
    for (int i = off; i <= WIDTH + off; i++) {
        piecerect.x = i * rectw - offset;
        for (int j = 0; j < HEIGHT; j++) {
            piecerect.y = j * recth;
            SDL_FillRect(screen, &piecerect, colour(tolower(at(i, j))));
        }
    }
}

bool Level::legal_pos(int x, int y)
{
    x += offset;
    int sx = x / rectw;
    int sy = y / recth;
    return islower(at(sx, sy));
}

bool Level::load(const char *filename)
{
    free(data);
    FILE *fp = fopen(filename, "r");
    if (fp == nullptr) return false;
    fread(&w, sizeof(int), 1, fp);
    fread(&h, sizeof(int), 1, fp);
    size_t bytes = w * h;
    data = (char *)malloc(bytes * sizeof(char));
    size_t read = fread(data, sizeof(char), bytes, fp);
    fclose(fp);
    if (read != bytes) {
        free(data);
        return false;
    }
    offset = 0;
    max_offset = (w - WIDTH) * rectw;
    return true;
}

char Level::at(int x, int y)
{
    int of = x * h + y;
    if (of > w * h) return 'B';
    return *(data + of);
}

Uint32 Level::colour(char symbol)
{
    switch (symbol) {
    case 's': return sky_colour;
    case 'g': return grass_colour;
    case 'b': return brick_colour;
    default: return 0;
    }
}
