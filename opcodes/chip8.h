#pragma once
#include <stdint.h>

typedef struct {
    uint16_t PC;  // Program counter
    uint16_t SP;  // Stack pointer
    uint16_t I;   // Index Pointer
} chip8;

chip8* initialize();

void emulateCycle(chip8*);
int load(const char*);
