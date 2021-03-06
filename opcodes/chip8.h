#pragma once
#include <SDL2/SDL_events.h>
#include <stdbool.h>
#include <stdint.h>

#define V_FLAG 0x0F

#define X_SCREEN 64
#define Y_SCREEN 32

#define FONT_HEIGHT 5
#define FONT_WIDTH 4

#define ACTIVE_PX 0xffffffff
#define INACTIVE_PX 0x00000000

// key map
#define KEY_0 0b0000000000000001
#define KEY_1 0b0000000000000010
#define KEY_2 0b0000000000000100
#define KEY_3 0b0000000000001000
#define KEY_4 0b0000000000010000
#define KEY_5 0b0000000000100000
#define KEY_6 0b0000000001000000
#define KEY_7 0b0000000010000000
#define KEY_8 0b0000000100000000
#define KEY_9 0b0000001000000000
#define KEY_A 0b0000010000000000
#define KEY_B 0b0000100000000000
#define KEY_C 0b0001000000000000
#define KEY_D 0b0010000000000000
#define KEY_E 0b0100000000000000
#define KEY_F 0b1000000000000000

struct chip8
{
    uint16_t PC;           // Program counter
    uint16_t SP;           // Stack pointer
    uint16_t I;            // Index Pointer
    uint16_t clock, timer; // timers for audio and delay
    uint32_t* gfx;         // [32*64]uint32_t
    bool wrapX;            // wrap pixel on X axis
    bool wrapY;            // wrap pixel on Y axis
    bool draw; // draw flag to check wheather the screen must be updated or not
    uint16_t keypad; // each bit maps to a key
    uint8_t delay;
    uint8_t sound;
};

typedef struct chip8 chip8;

chip8*
initialize();

void
emulateCycle(chip8*);

int
load(const char*);

void
input(chip8* vm, bool* is_running);
