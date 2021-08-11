#include "./sdl/sdl.h"
#include "memory/memory.h"
#include "opcodes/chip8.h"

#include <SDL2/SDL.h>
#include <SDL2/SDL_error.h>
#include <SDL2/SDL_events.h>
#include <SDL2/SDL_render.h>
#include <SDL2/SDL_timer.h>
#include <SDL2/SDL_video.h>
#include <math.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

// VSYNC prevents screen tears i.e. Half of once screen half of another.

#define WIDTH_MASK 0b11110000
#define HEIGHT_MASK 0b00001111

void
get_size(unsigned char);

int
main()
{
    bool is_runnning = true;
    int delay = 16;

    SDL_Window* _emu_win;
    SDL_Renderer* _emu_renderer;
    create_window_and_renderer(_emu_win, _emu_renderer);

    SDL_Texture* _emu_texture = create_texture(_emu_renderer);
    SDL_SetWindowTitle(_emu_win, "CHIP8");

    if (!_emu_win) {
        fprintf(stderr, "ERROR: %s", SDL_GetError());
    } else {
        chip8* chip = initialize();
        load("./ROMS/Airplane.ch8"); // Load the game to play in memory
                                     /* disp_mem(); */
        SDL_Event* usr_eve;
        while (is_runnning) {
            // keep executing instructions until the draw flag is set and the
            // screen needs to be updated
            while (!chip->draw)
                emulateCycle(chip, usr_eve);

            input(chip, &is_runnning);
            // TODO store frame buffer on the chip, abstract the drawing of each
            // frame using renderer and textures

            if (chip->draw) {
            }

            SDL_Delay(delay);
        }
    }

    SDL_DestroyWindow(_emu_win);
    SDL_Quit();
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
