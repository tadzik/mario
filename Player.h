#ifndef PLAYER_H
#define PLAYER_H

#include <SDL.h>
#include <SDL_image.h>

class Player {
    SDL_Surface *image;
public:
    SDL_Rect rect;
    Uint16 x, y;
    double vvel;

    Player() : image(nullptr), x(0), y(0), vvel(0)
    {
        rect.w = rect.h = 0;
    }

    Player(const char *);
    void moved(int, int, Player&);

    void draw(SDL_Surface *);

    ~Player()
    {
        SDL_FreeSurface(image);
    }
};

#endif
