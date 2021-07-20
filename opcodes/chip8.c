#include "chip8.h"
#include <bits/stdint-uintn.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "../memory/memory.h"

#define V_FLAG 0xF

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
    uint16_t result;
    uint16_t _op_X;
    uint16_t _op_Y;
    struct timespec spec;
    clock_gettime(CLOCK_REALTIME, &spec);
    srand(spec.tv_nsec);
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
            if (reg_cmp_val((opcode & 0x0F00) >> 8, opcode & 0x00FF, &_equ)) {
                chip->PC += 2;
            }
            break;
        case 0x4000:
            if (!reg_cmp_val((opcode & 0x0F00) >> 8, opcode & 0x00FF, &_equ)) {
                chip->PC += 2;
            }
            break;
        case 0x5000:
            if (reg_cmp_reg((opcode & 0x0F00) >> 8, (opcode & 0x00F0) >> 4,
                            &_equ)) {
                chip->PC += 2;
            }
            break;
        case 0x6000:
            set_reg((opcode & 0x0F00) >> 8, opcode & 0x00FF);
            break;
        case 0x7000:
            set_reg((opcode & 0x0F00) >> 8,
                    get_reg((opcode & 0x0F00) >> 8) + (opcode & 0x00FF));
            break;
        case 0x8000:
            _op_X = get_reg((opcode & 0x0F00) >> 8);
            _op_Y = get_reg((opcode & 0x00F0) >> 4);
            switch (opcode & 0x000F) {
                case 0x0000:
                    // set Vx = Vy
                    result = _op_X;
                    break;
                case 0x0001:
                    result = _op_X | _op_Y;
                    break;
                case 0x0002:
                    result = _op_X & _op_Y;
                    break;
                case 0x0003:
                    result = _op_X ^ _op_Y;
                    break;
                case 0x0004:
                    result = _op_X + _op_Y;
                    if (result < _op_X) {
                        set_reg(V_FLAG, 0x1);
                    } else {
                        set_reg(V_FLAG, 0x0);
                    }
                    break;
                case 0x0005:
                    result = _op_X - _op_Y;
                    if (result > _op_X) {
                        set_reg(V_FLAG, 0x1);
                    } else {
                        set_reg(V_FLAG, 0x0);
                    }
                    break;
                case 0x0006:
                    set_reg(V_FLAG, _op_X & 0x0001);
                    result = _op_X >> 1;
                    break;
                case 0x0007:
                    result = _op_Y - _op_X;
                    if (result > _op_Y) {
                        set_reg(V_FLAG, 0x1);
                    } else {
                        set_reg(V_FLAG, 0x0);
                    }
                    break;
                case 0x000E:
                    set_reg(V_FLAG, 0x8000);
                    result = _op_X << 1;
                    break;
            }
            set_reg((opcode & 0x0F00) >> 8, result);
            break;
        case 0x9000:
            if (!reg_cmp_reg((opcode & 0x0F00) >> 8, (opcode & 0x00F0) >> 4,
                             &_equ))
                chip->PC += 2;
            break;
        case 0xA000:
            chip->I = opcode & 0x0FFF;
            break;
        case 0xB000:
            chip->PC = get_reg(0x0) + opcode & 0x0FFF;
            break;
        case 0xC000:
            set_reg((opcode & 0x0F00) >> 8, (rand() % 255) & (opcode & 0x00FF));
            break;
        case 0xD000:
            // Draw
            break;
        case 0xE000:
            break;
        case 0xF000:
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

    // Update Timers
}
