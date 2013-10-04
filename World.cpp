#include "World.h"
#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include <time.h>
#include <ctype.h>

static int sdl_timer_cb(int interval, void *world)
{
    SDL_Event event;
    event.type = SDL_USEREVENT;
    event.user.code = 0;
    event.user.data1 = world;
    SDL_PushEvent(&event);
    return interval;
}

World::World(SDL_Surface *s, const char *player_path)
    : screen(s), player(player_path), level(screen), running(0)
{
    // every 16 ms, for approx. 60 fps
    SDL_AddTimer(16, (SDL_NewTimerCallback)sdl_timer_cb, this);
    if (!level.load("level.lvl")) {
        puts("Level file fucked!");
    }
    for (int i = 0; i < SDLK_LAST; i++) {
        keyboard_map[i] = false;
    }
    rectw = screen->w / WIDTH;
    recth = screen->h / HEIGHT;
}

void World::draw()
{
    level.draw(screen);
    player.draw(screen);
    SDL_Flip(screen);
}

void World::keypress_handler(SDLKey) {}
void World::keyrelease_handler(SDLKey) {}

void World::loop()
{
    SDL_Event ev;
    while (running) {
        draw();
        SDL_WaitEvent(&ev);
        switch (ev.type) {
        case SDL_QUIT:
            quit_handler();
            break;
        case SDL_USEREVENT:
            if (ev.user.data1 == this) tick_handler();
            break;
        case SDL_KEYDOWN:
            keypress_handler(ev.key.keysym.sym);
            keyboard_map[ev.key.keysym.sym] = true;
            break;
        case SDL_KEYUP:
            keyrelease_handler(ev.key.keysym.sym);
            keyboard_map[ev.key.keysym.sym] = false;
            break;
        default:
            /* nothing */
            break;
        }
    }
}

// modifies player_cand!
bool World::player_can_move(int dx, int dy)
{
    player.moved(dx, dy, player_cand);
    return player_pos_ok(player_cand);
}

bool World::player_on_ground()
{
    return !player_can_move(0, 1);
}

bool World::player_pos_ok(Player& p)
{
    if (p.x + p.rect.w >= screen->w)
        return false;
    if (!level.legal_pos(p.x, p.y + p.rect.h)) {
        return false;
    }
    if (!level.legal_pos(p.x + p.rect.w, p.y + p.rect.h)) {
        return false;
    }
    if (!level.legal_pos(p.x, p.y)) {
        return false;
    }
    if (!level.legal_pos(p.x + p.rect.w, p.y)) {
        return false;
    }

    return true;
}

void World::tick_handler()
{
    static int tick_counter = 0;
    //gravity
    bool moved = false;
    while (player.vvel != 0 && !moved) {
        if (player_can_move(0, player.vvel)) {
            player.y += player.vvel;
            moved = true;
        } else {
            if (player.vvel > 0)
                player.vvel--;
            else
                player.vvel++;
        }
    }
    if (keyboard_map[SDLK_RIGHT]) {
        if (player_can_move(5, 0)) {
            player.x += 5;
            if (player.x > screen->w / 2 && level.offset < level.max_offset) {
                level.offset += 5;
                player.x -= 5;
            }
        }
    }
    if (keyboard_map[SDLK_LEFT]) {
        if (player_can_move(-5, 0)) {
            player.x -= 5;
        }
    }
    if (keyboard_map[SDLK_UP]) {
        if (!player.vvel && player_on_ground()) {
            player.vvel -= 18;
        }
    }

    if (!player_on_ground()) {
        player.vvel++;
    }

    tick_counter++;
}

void World::run()
{
    running = true;
    loop();
}

void World::quit_handler()
{
    running = false;
}
