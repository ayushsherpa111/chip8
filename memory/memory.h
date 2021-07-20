#pragma once

#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>

void init_mem();

uint16_t get_opcode(uint16_t __opcode);

uint16_t get_PC();

void clear_disp();

void load_rom(FILE* __rom_file);

void disp_mem();

uint16_t push_stk(uint16_t _sp, uint16_t _stk_val);

uint16_t delay;
uint16_t sound_timer;

// pop_stk
// Pops data off the stack
uint16_t* pop_stk(uint16_t _sp);

// reg_cmp_val
// Compares the value in the registers with an immediate value
// Given a compare function. That takes 2 uint16_t as arguments
// And returns a boolean
bool reg_cmp_val(uint8_t __idx,
                 uint16_t _cmp_val,
                 bool (*_cmp_func)(uint16_t, uint16_t));

// reg_cmp_reg
// Compares the value in the registers with another register
// Given a compare function. That takes 2 uint16_t as arguments
// And returns a boolean
bool reg_cmp_reg(uint8_t __idx_lhs,
                 uint8_t __idx_rhs,
                 bool (*_cmp_func)(uint16_t, uint16_t));

// set_reg sets the value of the register _reg_num
void set_reg(uint8_t _reg_num, uint16_t _reg_val);

// get_reg gets the value in a given register
uint16_t get_reg(uint8_t _reg_num);

