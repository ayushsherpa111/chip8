#include "./sdl.h"
#include <SDL2/SDL.h>
#include <SDL2/SDL_error.h>
#include <SDL2/SDL_log.h>
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
create_window_and_renderer(SDL_Window** _win, SDL_Renderer** _rend)
{
    if (SDL_CreateWindowAndRenderer(800, 600, SDL_WINDOW_SHOWN, _win, _rend))
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION,
                     "Couldnt create window and renderer. Reason: %s",
                     SDL_GetError());
}

void
draw_frame(uint32_t* _frame, SDL_Renderer* _rend, SDL_Texture* _text)
{
    SDL_UpdateTexture(_text, NULL, _frame, WIDTH * 4);
    SDL_RenderCopy(_rend, _text, NULL, NULL);
    SDL_RenderPresent(_rend);
}
