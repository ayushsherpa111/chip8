#include "./sdl.h"
#include <SDL2/SDL.h>
#include <SDL2/SDL_render.h>
#include <SDL2/SDL_stdinc.h>
#include <SDL2/SDL_video.h>
#include <stdint.h>

#define SCALE 2
#define WIDTH 64
#define HEIGHT 32

SDL_Texture*
create_texture(SDL_Renderer* _renderer)
{
    return SDL_CreateTexture(_renderer,
                             SDL_PIXELFORMAT_RGBA32,
                             SDL_TEXTUREACCESS_STREAMING,
                             WIDTH,
                             HEIGHT);
}

void
create_window_and_renderer(SDL_Window* _win, SDL_Renderer* _rend)
{
    SDL_CreateWindowAndRenderer(
      SCALE * WIDTH, SCALE * HEIGHT, SDL_WINDOW_SHOWN, &_win, &_rend);
}

void
draw_frame(uint8_t** _frame)
{}
