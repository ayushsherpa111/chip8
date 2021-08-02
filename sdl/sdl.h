#pragma once
#include <SDL2/SDL_render.h>
#include <SDL2/SDL_video.h>
#include <stdint.h>

// create_window
SDL_Window*
create_window(const char* title, uint32_t flags);

SDL_Renderer*
create_renderer(SDL_Window*);

SDL_Texture*
create_texture();
