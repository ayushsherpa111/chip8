#include "memory.h"
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>

#define MEM_SIZE 4 << 10
#define HEX_KEY_BOARD 16
#define REGISTER_COUNT 16
#define STACK_SIZE 16

#define OFFSET 0x200

void
load_fontset(uint8_t*);

// The fontset used by all ROMS for chip8
uint8_t chip8_fontset[80] = {
    0xF0, 0x90, 0x90, 0x90, 0xF0, // 0
    0x20, 0x60, 0x20, 0x20, 0x70, // 1
    0xF0, 0x10, 0xF0, 0x80, 0xF0, // 2
    0xF0, 0x10, 0xF0, 0x10, 0xF0, // 3
    0x90, 0x90, 0xF0, 0x10, 0x10, // 4
    0xF0, 0x80, 0xF0, 0x10, 0xF0, // 5
    0xF0, 0x80, 0xF0, 0x90, 0xF0, // 6
    0xF0, 0x10, 0x20, 0x40, 0x40, // 7
    0xF0, 0x90, 0xF0, 0x90, 0xF0, // 8
    0xF0, 0x90, 0xF0, 0x10, 0xF0, // 9
    0xF0, 0x90, 0xF0, 0x90, 0x90, // A
    0xE0, 0x90, 0xE0, 0x90, 0xE0, // B
    0xF0, 0x80, 0x80, 0x80, 0xF0, // C
    0xE0, 0x90, 0x90, 0x90, 0xE0, // D
    0xF0, 0x80, 0xF0, 0x80, 0xF0, // E
    0xF0, 0x80, 0xF0, 0x80, 0x80  // F
};

// 4Kb Working memory
// 0-511 (0x000 - 0x1FF) Interpreter
// 80-160 (0x50 - 0x0A0) used for pixel font set
// 512-4095 (0x200 - 0xFFF) Program ROM and work RAM
uint8_t* memory;

// 16 8-bit registers
uint8_t* V;

// Stack
uint16_t* stack;

// Hex key mappings
uint8_t* key;

uint16_t
get_opcode(uint16_t pc)
{
    return memory[pc] << 8 | memory[pc + 1];
}

// Initalize the game state and all the memory involved.
void
init_mem()
{
    memory = (uint8_t*)calloc(MEM_SIZE, sizeof(uint8_t)); // 4 MB
    load_fontset(memory);

    key = (uint8_t*)calloc(HEX_KEY_BOARD, sizeof(uint8_t));
    V = (uint8_t*)calloc(REGISTER_COUNT, sizeof(uint8_t));
    stack = (uint16_t*)calloc(STACK_SIZE, sizeof(uint16_t));
}

bool
check_idx(uint8_t _idx, uint8_t limit)
{
    return _idx >= 0 && _idx < limit;
}

// Push data onto the stack.
// Takes the stack pointer and the value to push on the stack as parameters
// Returns the incremented stack pointer
uint16_t
push_stk(uint16_t _sp, uint16_t _stk_val)
{
    if (_sp < STACK_SIZE && _sp > 0) {
        stack[_sp] = _stk_val;
        _sp++;
    }
    return _sp;
}

// Pop data out of stack
// Takes the stack pointer as a parameter
// Returns an array with 2 index.
// 0 Value that was stored at the stack pointer
// 1 New Stack pointer
uint16_t*
pop_stk(uint16_t _sp)
{
    uint16_t* _stk_dat = calloc(2, sizeof(uint16_t));
    if (_sp > 0) {
        _stk_dat[0] = stack[_sp];
        _stk_dat[1] = --_sp;
    }
    return _stk_dat;
}

void
load_fontset(uint8_t* mem)
{
    memcpy(memory + 0x50, mem, 16 * 5);
}

void
clear_disp()
{}

void
load_rom(FILE* rom_file)
{
    int num_bytes;

    fseek(rom_file, 0L, SEEK_END);
    num_bytes = ftell(rom_file);

    fseek(rom_file, 0L, SEEK_SET);

    fread(memory + OFFSET, sizeof(memory), num_bytes, rom_file);
}

// PERF
void
disp_mem()
{
    for (int i = 0x200; i < MEM_SIZE; i++) {
        printf("% 2d ----- %x\n", i, memory[i]);
    }
}

bool
reg_cmp_val(uint8_t __idx,
            uint16_t _cmp_val,
            bool (*_cmp_func)(uint16_t, uint16_t))
{
    return check_idx(__idx, REGISTER_COUNT) // Checks if the index is valid
             ? (*_cmp_func)(V[__idx], _cmp_val)
             : false;
}

bool
reg_cmp_reg(uint8_t __idx_lhs,
            uint8_t __idx_rhs,
            bool (*_cmp_func)(uint16_t, uint16_t))
{
    return check_idx(__idx_lhs, REGISTER_COUNT) &&
               check_idx(__idx_rhs, REGISTER_COUNT)
             ? (*_cmp_func)(V[__idx_lhs], V[__idx_rhs])
             : false;
}

void
set_reg(uint8_t _reg_num, uint8_t _reg_val)
{
    if (check_idx(_reg_num, REGISTER_COUNT))
        V[_reg_num] = _reg_num;
}

uint8_t
get_reg_value(uint8_t _reg_num)
{
    if (check_idx(_reg_num, REGISTER_COUNT)) {
        return V[_reg_num];
    }
    return -1;
}

uint16_t
read_mem(uint16_t ptr)
{
    if (!(ptr < 0 || ptr > MEM_SIZE)) {
        return memory[ptr];
    }
    return 0;
}
