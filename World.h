#ifndef WORLD_H
#define WORLD_H

#include <SDL.h>
#include "Level.h"
#include "Player.h"

class World {
    SDL_Surface *screen;
    Player player;
    Player player_cand;
    Level level;
    bool running;
    bool keyboard_map[SDLK_LAST];
    int rectw, recth;

    void draw();
    void keypress_handler(SDLKey);
    void keyrelease_handler(SDLKey);
    void loop();
    bool player_can_move(int, int);
    bool player_on_ground();
    bool player_pos_ok(Player&);
    void tick_handler();
    void quit_handler();

public:
    World(SDL_Surface *, const char *);
    void run();

};

#endif
