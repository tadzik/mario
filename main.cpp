#include <SDL.h>

#include "World.h"

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
