#include "./sdl/sdl.h"
#include "memory/memory.h"
#include "opcodes/chip8.h"

#include <SDL2/SDL.h>
#include <SDL2/SDL_error.h>
#include <SDL2/SDL_events.h>
#include <SDL2/SDL_log.h>
#include <SDL2/SDL_pixels.h>
#include <SDL2/SDL_render.h>
#include <SDL2/SDL_timer.h>
#include <SDL2/SDL_video.h>
#include <math.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

// VSYNC prevents screen tears i.e. Half of once screen half of another.

#define WIDTH_MASK 0b11110000
#define HEIGHT_MASK 0b00001111

void
get_size(unsigned char);

int
main(int argc, char* argv[])
{

    srand(time(NULL));
    bool is_runnning = true;
    int delay_SDL = 1500;

    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        SDL_LogError(SDL_LOG_CATEGORY_ERROR,
                     "Couldnt Initialize video, Reason: %s",
                     SDL_GetError());
        return 1;
    }

    SDL_Window* _emu_win;
    SDL_Renderer* _emu_renderer;

    if (SDL_CreateWindowAndRenderer(
          64 * 10, 32 * 10, SDL_WINDOW_SHOWN, &_emu_win, &_emu_renderer)) {
        SDL_LogError(SDL_LOG_CATEGORY_ERROR,
                     "Couldnt Initialize Window and Renderer, Reason: %s",
                     SDL_GetError());
        return 1;
    }

    /* create_window_and_renderer(&_emu_win, &_emu_renderer); */

    /* if (!_emu_win || !_emu_renderer) { */
    /*     printf("Failed to init window|renderer %s", SDL_GetError()); */
    /*     return 1; */
    /* } */

    SDL_Texture* _emu_texture = SDL_CreateTexture(_emu_renderer,
                                                  SDL_PIXELFORMAT_RGBA32,
                                                  SDL_TEXTUREACCESS_STREAMING,
                                                  64,
                                                  32);
    if (!_emu_texture) {
        printf("Failed to create texture %s", SDL_GetError());
        return 2;
    }

    if (!_emu_win) {
        fprintf(stderr, "ERROR: %s", SDL_GetError());
    } else {
        chip8* chip = initialize();

        // Load the game to play in memory
        if (load("./ROMS/IBM.ch8") > 0) {
            printf("Failed to load ROM");
            return 1;
        }
        /* SDL_Event* usr_eve; */
        while (is_runnning) {
            // keep executing instructions until the draw flag is set and the
            // screen needs to be updated
            while (!chip->draw) {
                SDL_PumpEvents();
                emulateCycle(chip);
            }
            input(chip, &is_runnning);
            // TODO store frame buffer on the chip, abstract the drawing of each
            // frame using renderer and textures

            if (chip->draw) {
                chip->draw = false;

                int b = 4;
                uint32_t* pixels;
                SDL_LockTexture(_emu_texture, NULL, (void*)&pixels, &b);
                memcpy(pixels, chip->gfx, 64 * 32 * 4);
                // SDL_UpdateTexture(_emu_texture, NULL, chip->gfx, 64 * b);
                SDL_UnlockTexture(_emu_texture);
                SDL_RenderCopy(_emu_renderer, _emu_texture, NULL, NULL);
                SDL_RenderPresent(_emu_renderer);
                /* draw_frame(chip->gfx, _emu_renderer, _emu_texture); */
            }

            SDL_Delay(delay_SDL);
        }
        free(chip);
    }

    SDL_DestroyWindow(_emu_win);
    SDL_DestroyRenderer(_emu_renderer);
    SDL_DestroyTexture(_emu_texture);

    SDL_Quit();

    cleanup();
    return 0;
}

void
get_size(unsigned char sprite_size)
{
    unsigned char width = (sprite_size & WIDTH_MASK) >> 4;
    printf("The width is %dpx. \n", width);
    unsigned char height = (sprite_size & HEIGHT_MASK);
    printf("The width is %dpx.\n", height);
}

/* if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO) != 0) { */
/*     printf("Error: %s", SDL_GetError()); */
/*     return 1; */
/* } */
/* printf("Initialization success"); */
/* SDL_Window* win = */
/*     SDL_CreateWindow("This is a window", SDL_WINDOWPOS_CENTERED, */
/*                      SDL_WINDOWPOS_CENTERED, 640, 480, SDL_WINDOW_SHOWN); */
/* if (!win) { */
/*     printf("Error :%s", SDL_GetError()); */
/*     SDL_Quit(); */
/*     return 1; */
/* } */

/* uint32_t render_flags = */
/*     SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC; */
/* SDL_Renderer* rend = SDL_CreateRenderer(win, -1, render_flags); */
/* if (!rend) { */
/*     SDL_DestroyWindow(win); */
/*     SDL_Quit(); */
/*     return 1; */
/* } */
/* printf("\nEVENT %x\n", SDL_KEYUP); */

/* SDL_Event handle; */
/* bool isRunning = true; */
/* while (isRunning) { */
/*     while (SDL_PollEvent(&handle)) { */
/*         if (handle.key.keysym.sym == SDLK_q) { */
/*             printf("Exiting"); */
/*             isRunning = 0; */
/*         } */
/*     } */
/*     SDL_UpdateWindowSurface(win); */
/* } */

/* SDL_DestroyWindow(win); */
/* SDL_Quit(); */
