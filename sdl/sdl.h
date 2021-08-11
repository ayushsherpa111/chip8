#pragma once
#include <SDL2/SDL_render.h>
#include <SDL2/SDL_video.h>
#include <stdint.h>

SDL_Texture*
create_texture(SDL_Renderer* _renderer);

void
create_window_and_renderer(SDL_Window* _win, SDL_Renderer* _rend);

