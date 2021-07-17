#include "chip8.h"
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include "../memory/memory.h"

// Initializes the Environment required for the chip
// sets the required registers
chip8* initialize() {
    chip8* chip = malloc(sizeof(chip8));
    chip->I = 0;
    chip->PC = 0x200;
    chip->SP = 0x0;

    init_mem();

    clear_disp();
    return chip;
}

// Returns the equality comparison result of 2 values
bool _equ(uint16_t lhs, uint16_t rhs) {
    return lhs == rhs;
}

/* bool _gt_() { */

/* } */

// Load ROM into working memory
int load(const char* gameLocation) {
    FILE* ROM = fopen(gameLocation, "rb");
    if (ROM == NULL) {
        return 1;
    }
    load_rom(ROM);

    fclose(ROM);
    return 0;
}

void decode_exec(uint16_t opcode, chip8* chip) {
    uint16_t* _st_pop;
    switch (opcode & 0xF000) {
        case 0x0000:
            switch (opcode & 0x000F) {
                case 0x0000:
                    break;
                case 0x000E:  // return from subroutine
                    _st_pop = pop_stk(chip->SP);
                    chip->PC = _st_pop[0];
                    chip->SP = _st_pop[1];
                    free(_st_pop);
                    break;
            }
            break;
        case 0x1000:
            // jmp address
            chip->PC = opcode & 0x0FFF;
            break;
        case 0x2000:
            chip->SP = push_stk(chip->SP, chip->PC);
            chip->PC = opcode & 0x0FFF;
            break;
        case 0x3000:
            // compare Vx to 0x0F00
            if (reg_cmp_val((opcode & 0x0F00) >> 2, opcode & 0x00FF, &_equ)) {
                chip->PC += 2;
            }
            break;
        case 0x4000:
            if (!reg_cmp_val((opcode & 0x0F00) >> 2, opcode & 0x00FF, &_equ)) {
                chip->PC += 2;
            }
            break;
        case 0x5000:
            if (reg_cmp_reg((opcode & 0x0F00) >> 2, (opcode & 0x00F0) >> 1,
                            &_equ)) {
                chip->PC += 2;
            }
            break;
        case 0x6000:
            set_reg((opcode & 0x0F00) >> 2, opcode & 0x00FF);
            break;
        default:
            printf("Unkown opcode\n");
    }
}

void emulateCycle(chip8* chip) {
    // fetch opcode
    uint16_t opcode = get_opcode(chip->PC);

    // decode opcode
    decode_exec(opcode, chip);

    // execute opcode

    // Update Timers
}
