#include "chip8.h"
#include <math.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "../memory/memory.h"
#define V_FLAG 0xF0
#define X_SCREEN 64
#define Y_SCREEN 32

// clr_scr clears the screen by setting each pixel value to 0
void clr_scr(uint8_t** _scr) {
    for (int row = 0; row < Y_SCREEN; row++) {
        for (int col = 0; col < 8; col++) {
            _scr[row][col] = 0;  // *(*(_scr + row) + col) = 0;
        }
    }
}

// clr_scr clears the screen by setting each pixel value to 0
void init_scr(uint8_t** _scr) {
    _scr = calloc(X_SCREEN, sizeof(uint8_t*));
    for (int row = 0; row < Y_SCREEN; row++) {  // 32 rows
        _scr[row] = calloc(
            8, sizeof(uint8_t));  // each row contains 8 - 8bits aka 8 pixels
    }
}

// TODO wrap the X and Y coordinates
void wrap_cords() {}

// Initializes the Environment required for the chip
// sets the required registers
chip8* initialize() {
    chip8* chip = malloc(sizeof(chip8));
    chip->I = 0;
    chip->PC = 0x200;
    chip->SP = 0x0;
    chip->drawflag = false;
    chip->wrapX = false;
    chip->wrapY = false;
    init_scr(chip->screen);

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
    uint8_t _op_X;
    uint8_t _op_Y;
    uint8_t height;
    uint16_t _x_coord;
    uint16_t _y_coord;

    struct timespec spec;
    clock_gettime(CLOCK_REALTIME, &spec);
    srand(spec.tv_nsec);
    switch (opcode & 0xF000) {
        case 0x0000:
            switch (opcode & 0x000F) {
                case 0x0000:
                    break;
                case 0x000E:  // return from subroutine
                    // Check if there is anything on the stack
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
                    get_reg_value((opcode & 0x0F00) >> 8) + (opcode & 0x00FF));
            break;
        case 0x8000:
            _op_X = get_reg_value((opcode & 0x0F00) >> 8);
            _op_Y = get_reg_value((opcode & 0x00F0) >> 4);
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
                    set_reg(V_FLAG, _op_X & 0x80);
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
            chip->PC = get_reg_value(0x0) + opcode & 0x0FFF;
            break;
        case 0xC000:
            set_reg((opcode & 0x0F00) >> 8, (rand() % 255) & (opcode & 0x00FF));
            break;
        case 0xD000:
            // Draw
            height = opcode & 0x000F;
            _x_coord =
                get_reg_value((opcode & 0x0F00) >> 8);  // X position on screen
            _y_coord =
                get_reg_value((opcode & 0x00F0) >> 4);  // Y position on screen
            // _x_coord = _x_coord > X_SCREEN ? _x_coord % X_SCREEN : _x_coord;
            // _y_coord = _y_coord > Y_SCREEN ? _y_coord % Y_SCREEN : _y_coord;
            for (int i = 0; i < height; i++) {
                uint8_t sprite = (uint8_t)read_mem(chip->I + i);
                uint8_t _s_sprt = (sprite >> (_x_coord % 8));
                if ((chip->screen[_y_coord][_x_coord] & _s_sprt) > 0) {
                    set_reg(V_FLAG, 0x1);
                } else {
                    set_reg(V_FLAG, 0x0);
                }
                chip->screen[_y_coord][_x_coord / 8] ^= _s_sprt;
                // TODO implement wrap logic
                if (chip->wrapX)
                    if (_x_coord % 8 != 0) {
                        // if there were any bits that
                        // had to be wrapped from the
                        // sprite it will be in _s_sprt
                        _s_sprt = sprite & ((uint8_t)pow(2, _x_coord % 8))
                                               << (8 - _x_coord % 8);
                    }
                ++_y_coord;
                // TODO implement Y axis wrap logic
                if (_y_coord > Y_SCREEN)
                    break;
            }
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
