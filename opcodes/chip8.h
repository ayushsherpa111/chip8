#pragma once
#include <stdbool.h>
#include <stdint.h>

typedef struct {
    uint16_t PC;            // Program counter
    uint16_t SP;            // Stack pointer
    uint16_t I;             // Index Pointer
    uint16_t clock, timer;  // timers for audio and delay
    uint8_t** screen;       // [32][8]uint8_t
    bool drawflag;          // draw on screen
    bool wrapX;             // wrap pixel on X axis
    bool wrapY;             // wrap pixel on Y axis
} chip8;

chip8* initialize();

void emulateCycle(chip8*);
int load(const char*);
