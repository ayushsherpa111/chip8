#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include "opcodes/chip8.h"

#define WIDTH_MASK 0b11110000
#define HEIGHT_MASK 0b00001111

void get_size(unsigned char);

int main() {
    chip8* chip = initialize();
    load("./ROMS/Airplane.ch8");  // Load the game to play in memory
    /* disp_mem(); */
    for (;;) {
        // Emulate one cycle
        emulateCycle(chip);
    }
}

void get_size(unsigned char sprite_size) {
    unsigned char width = (sprite_size & WIDTH_MASK) >> 4;
    printf("The width is %dpx. \n", width);
    unsigned char height = (sprite_size & HEIGHT_MASK);
    printf("The width is %dpx.\n", height);
}
