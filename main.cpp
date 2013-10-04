// if you cringe at the notion of mixing C with C++... you better not read that

#include <SDL.h>
#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include <time.h>
#include <ctype.h>

#include "Level.h"

#include "Player.h"

int sdl_timer_cb(int interval, void *world)
{
    SDL_Event event;
    event.type = SDL_USEREVENT;
    event.user.code = 0;
    event.user.data1 = world;
    SDL_PushEvent(&event);
    return interval;
}


class World {
    SDL_Surface *screen;
    Player player;
    Player player_cand;
    Level level;
    bool running;
    bool keyboard_map[SDLK_LAST];
    int rectw, recth;

public:
    World(SDL_Surface *s, const char *player_path)
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

    void draw()
    {
        level.draw(screen);
        player.draw(screen);
        SDL_Flip(screen);
    }

    void keypress_handler(SDLKey) {}
    void keyrelease_handler(SDLKey) {}

    void loop()
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
    bool player_can_move(int dx, int dy)
    {
        player.moved(dx, dy, player_cand);
        return player_pos_ok(player_cand);
    }

    bool player_on_ground()
    {
        return !player_can_move(0, 1);
    }

    bool player_pos_ok(Player& p)
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

    void run()
    {
        running = true;
        loop();
    }

    void tick_handler()
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

    void quit_handler()
    {
        running = false;
    }
};

int main()
{
    SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER);
    SDL_Surface* screen = SDL_SetVideoMode(800, 600, 32, SDL_HWSURFACE | SDL_DOUBLEBUF);
    World w(screen, "smallmario.png");
    w.run();
    SDL_FreeSurface(screen);
    SDL_Quit();
    return 0;
}
