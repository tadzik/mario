// if you cringe at the notion of mixing C with C++... you better not read that

#include <SDL.h>
#include <SDL_gfxPrimitives.h>
#include <SDL_ttf.h>
#include <SDL_mixer.h>
#include <SDL_image.h>
#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include <time.h>
#include <ctype.h>

#define HEIGHT 12
#define WIDTH  16

int sdl_timer_cb(int interval, void *world)
{
    SDL_Event event;
    event.type = SDL_USEREVENT;
    event.user.code = 0;
    event.user.data1 = world;
    SDL_PushEvent(&event);
    return interval;
}

struct Level {
    int w, h;
    char *data;

    Level() : w(0), h(0), data(nullptr) {}

    bool load(const char *filename)
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
        return true;
    }

    char at(int x, int y)
    {
        return *(data + x * h + y);
    }

    ~Level()
    {
        free(data);
    }
};

struct Player {
    SDL_Surface *image;
    SDL_Rect rect;
    Uint16 x, y;
    double vvel;

    Player()
    {
        x = y = vvel = rect.w = rect.h = 0;
        image = NULL;
    }

    Player(const char *path)
    {
        x = y = 5;
        vvel = 0;
        image = IMG_Load(path);
        rect.w = image->w;
        rect.h = image->h;
    }

    void moved(int dx, int dy, Player& p)
    {
        p.x = x + dx;
        p.y = y + dy;
        p.rect = rect;
    }

    int feet_y()
    {
        return y + rect.y;
    }

    void draw(SDL_Surface *dst)
    {
        rect.x = x;
        rect.y = y;
        SDL_BlitSurface(image, NULL, dst, &rect);
    }

    ~Player()
    {
        SDL_FreeSurface(image);
    }
};

class World {
    SDL_Surface *screen;
    Player player;
    Player player_cand;
    Level level;
    bool running;
    bool keyboard_map[SDLK_LAST];
    Uint32 grass_colour;
    Uint32 brick_colour;
    Uint32 sky_colour;
    int level_offset;

public:
    World(SDL_Surface *s, const char *player_path)
        : screen(s), player(player_path), running(0), level_offset(0)
    {
        // every 16 ms, for approx. 60 fps
        SDL_AddTimer(16, (SDL_NewTimerCallback)sdl_timer_cb, this);
        if (!level.load("level.lvl")) {
            puts("Level file fucked!");
        }
        sky_colour = SDL_MapRGB(screen->format, 100, 100, 255);
        grass_colour = SDL_MapRGB(screen->format, 100, 255, 100);
        brick_colour = SDL_MapRGB(screen->format, 255, 100, 100);
        for (int i = 0; i < SDLK_LAST; i++) {
            keyboard_map[i] = false;
        }
    }

    Uint32 colour(char symbol)
    {
        switch (symbol) {
        case 's': return sky_colour;
        case 'g': return grass_colour;
        case 'b': return brick_colour;
        default: return 0;
        }
    }

    void draw()
    {
        int rectw = screen->w / WIDTH;
        int recth = screen->h / HEIGHT;
        SDL_Rect piecerect = { 0, 0, static_cast<Uint16>(rectw),static_cast<Uint16>(recth) };
        for (int i = 0; i < WIDTH; i++) {
            piecerect.x = i * rectw;
            for (int j = 0; j < HEIGHT; j++) {
                piecerect.y = j * recth;
                SDL_FillRect(screen, &piecerect, colour(tolower(level.at(level_offset + i, j))));
            }
        }
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
        int sx, sy;
        if (p.x < 0 || p.x + p.rect.w > screen->w)
            return false;
        square_at(p.x, p.y + p.rect.h, &sx, &sy);
        if (isupper(level.at(level_offset + sx, sy))) {
            return false;
        }
        square_at(p.x + p.rect.w, p.y + p.rect.h, &sx, &sy);
        if (isupper(level.at(level_offset + sx, sy))) {
            return false;
        }
        square_at(p.x, p.y, &sx, &sy);
        if (isupper(level.at(level_offset + sx, sy))) {
            return false;
        }
        square_at(p.x + p.rect.w, p.y, &sx, &sy);
        if (isupper(level.at(level_offset + sx, sy))) {
            return false;
        }

        return true;
    }

    void square_at(int x, int y, int *sx, int *sy)
    {
        int rectw = screen->w / WIDTH;
        int recth = screen->h / HEIGHT;
        *sx = x / rectw;
        *sy = y / recth;
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
