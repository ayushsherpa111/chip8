#include "./sdl.h"
#include <SDL2/SDL.h>
#include <SDL2/SDL_render.h>
#include <SDL2/SDL_stdinc.h>
#include <SDL2/SDL_video.h>
#include <stdint.h>

#define SCALE_WIDTH 64
#define SCALE_HEIGHT 32

SDL_Window*
create_window(const char* title, uint32_t flags)
{
    SDL_Window* _emu_win = NULL;
    if (!SDL_Init(flags)) {
        _emu_win = SDL_CreateWindow(title,
                                    SDL_WINDOWPOS_CENTERED,
                                    SDL_WINDOWPOS_CENTERED,
                                    800,
                                    600,
                                    SDL_WINDOW_SHOWN);
    }

    return _emu_win;
}

SDL_Renderer*
create_renderer(SDL_Window* _win)
{
    return SDL_CreateRenderer(
      _win,
      0,
      SDL_RENDERER_ACCELERATED); // use integrated GPU for faster rendering
}

SDL_Texture*
create_texture(SDL_Renderer* _renderer)
{
    return SDL_CreateTexture(_renderer,
                             SDL_PIXELFORMAT_RGBA32,
                             SDL_TEXTUREACCESS_STREAMING,
                             SCALE_WIDTH,
                             SCALE_HEIGHT);
}
