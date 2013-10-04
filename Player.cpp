#include "Player.h"

Player::Player(const char *path)
{
    x = y = 5;
    vvel = 0;
    image = IMG_Load(path);
    rect.w = image->w;
    rect.h = image->h;
}

void Player::moved(int dx, int dy, Player& p)
{
    p.x = x + dx;
    p.y = y + dy;
    p.rect = rect;
}

void Player::draw(SDL_Surface *dst)
{
    rect.x = x;
    rect.y = y;
    SDL_BlitSurface(image, NULL, dst, &rect);
}
