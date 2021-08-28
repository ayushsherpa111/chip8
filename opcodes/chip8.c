#include "chip8.h"
#include "../memory/memory.h"
#include "internal.h"
#include <SDL2/SDL_events.h>
#include <SDL2/SDL_keyboard.h>
#include <SDL2/SDL_keycode.h>
#include <math.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void
clear_disp(uint32_t* gfx)
{
    memset(gfx, INACTIVE_PX, sizeof(uint32_t) * X_SCREEN * Y_SCREEN);
}

void
print_vm_state(chip8* vm)
{
    /* printf("PC -- %d",); */
}

// clr_scr clears the screen by setting each pixel value to 0
void
init_scr(uint32_t** _scr)
{
    *_scr = malloc(sizeof(uint32_t) * X_SCREEN * Y_SCREEN);
}

// NOTE does not handle warping coordinates
// Pass coordinates once they have been warped
void
set_screen(chip8* _vm, uint8_t _x_coord, uint8_t _y_coord, uint8_t height);

// Initializes the Environment required for the chip
// sets the required registers
chip8*
initialize()
{
    chip8* chip = malloc(sizeof(chip8));
    chip->I = 0;
    chip->PC = 0x200;
    chip->SP = 0x0;
    chip->draw = false;
    chip->wrapX = false;
    chip->wrapY = false;
    chip->keypad = 0;
    /* init_scr(&chip->gfx); */
    chip->gfx = calloc(X_SCREEN * Y_SCREEN, sizeof(uint32_t));

    memset(chip->gfx, ACTIVE_PX, sizeof(uint32_t) * X_SCREEN * Y_SCREEN);

    init_mem();

    return chip;
}

void
reset(chip8* vm)
{
    clear_disp(vm->gfx);
    reset_stack();
    reset_reg();
    vm->draw = vm->I = vm->keypad = vm->SP = vm->delay = vm->sound = 0;
    vm->PC = 0x200;
}

// Returns the equality comparison result of 2 values
bool
_equ(uint16_t lhs, uint16_t rhs)
{
    return lhs == rhs;
}

// Load ROM into working memory
int
load(const char* gameLocation)
{
    FILE* ROM = fopen(gameLocation, "rb");
    if (ROM == NULL) {
        return 1;
    }
    load_rom(ROM);

    fclose(ROM);
    return 0;
}

void
decode_exec(uint16_t opcode, chip8* chip)
{
    printf("%x -- %x -- %x\n", opcode, chip->PC, chip->SP);
    /* get_stk(); */
    uint16_t result;
    uint8_t _op_X;
    uint8_t _op_Y;
    uint8_t height;
    uint8_t _x_coord;
    uint8_t _y_coord;
    uint8_t bcd;

    chip->PC += 2;

    switch (opcode & 0xF000) {
        case 0x0000:
            switch (opcode & 0x00FF) {
                case 0x0000:
                    break;
                case 0x00E0:
                    clear_disp(chip->gfx);
                    break;
                case 0x00EE: // return from subroutine
                    // Check if there is anything on the stack
                    chip->PC = pop_stk(--chip->SP);
                    /* chip->SP -= 1; */
                    break;
            }
            break;
        case 0x1000:
            // jmp address
            chip->PC = opcode & 0x0FFF;
            break;
        case 0x2000:
            // push current address to stack
            chip->SP = push_stk(chip->SP, chip->PC);
            chip->PC = opcode & 0x0FFF;
            break;
        case 0x3000:
            // Skip next instruction if NN == V[X]
            chip->PC += _V((opcode & 0x0F00) >> 8) == (opcode & 0x00FF) ? 2 : 0;
            break;
        case 0x4000:
            // Skip next instruction if NN != V[X]
            chip->PC +=
              !reg_cmp_val((opcode & 0x0F00) >> 8, opcode & 0x00FF, &_equ) ? 2
                                                                           : 0;
            break;
        case 0x5000:
            // Skip next instruction if V[x] == V[y]
            chip->PC +=
              reg_cmp_reg((opcode & 0x0F00) >> 8, (opcode & 0x00F0) >> 4, &_equ)
                ? 2
                : 0;
            break;
        case 0x6000:
            // set value of register
            set_reg((opcode & 0x0F00) >> 8, opcode & 0x00FF);
            break;
        case 0x7000:
            // Add N to V[x]
            set_reg((opcode & 0x0F00) >> 8,
                    _V((opcode & 0x0F00) >> 8) + (opcode & 0x00FF));
            // carry bit
            _V((opcode & 0x0F00) >> 8) < (opcode & 0x00FF) ? set_reg(V_FLAG, 1)
                                                           : set_reg(V_FLAG, 0);
            break;
        case 0x8000: {
            int _X = (opcode & 0x0F00) >> 8;
            _op_X = _V(_X);
            _op_Y = _V((opcode & 0x00F0) >> 4);
            switch (opcode & 0x000F) {
                case 0x0000:
                    // set Vx = Vy
                    set_reg(_X, _op_Y);
                    break;
                case 0x0001:
                    set_reg(_X, _op_X | _op_Y);
                    break;
                case 0x0002:
                    set_reg(_X, _op_X & _op_Y);
                    break;
                case 0x0003:
                    set_reg(_X, _op_X ^ _op_Y);
                    break;
                case 0x0004:
                    set_reg(_X, _op_X + _op_Y);
                    set_reg(V_FLAG, result < _op_X ? 0x1 : 0x0);
                    break;
                case 0x0005:
                    set_reg(_X, _op_X - _op_Y);
                    set_reg(V_FLAG, result > _op_X ? 0x1 : 0x0);
                    break;
                case 0x0006:
                    set_reg(V_FLAG, _op_X & 0x0001);
                    set_reg(_X, _op_X >> 1);
                    break;
                case 0x0007:
                    set_reg(_X, _op_Y - _op_X);
                    set_reg(V_FLAG, result > _op_Y ? 0x1 : 0x0);
                    break;
                case 0x000E:
                    set_reg(V_FLAG, _op_X & 0x80);
                    set_reg(_X, _op_X << 1);
                    break;
            }
            break;
        }
        case 0x9000:
            chip->PC += !reg_cmp_reg(
                          (opcode & 0x0F00) >> 8, (opcode & 0x00F0) >> 4, &_equ)
                          ? 2
                          : 0;
            break;
        case 0xA000:
            chip->I = opcode & 0x0FFF;
            break;
        case 0xB000:
            chip->PC = _V(0x0) + (opcode & 0x0FFF);
            break;
        case 0xC000:
            set_reg((opcode & 0x0F00) >> 8, (rand() % 255) & (opcode & 0x00FF));
            break;
        case 0xD000:
            // set collision flag to 0
            set_reg(V_FLAG, 0x0);

            // Draw
            /* chip->PC++; */ // ???? WHYY

            height = opcode & 0x000F;

            _x_coord = _V((opcode & 0x0F00) >> 8); // X position on screen
            _y_coord = _V((opcode & 0x00F0) >> 4); // Y position on screen

            // wrap coords
            wrap_coords(&_x_coord, &_y_coord);
            // draw a pixel pattern for `height` number of rows
            set_screen(chip, _x_coord, _y_coord, height);
            break;
        case 0xE000:
            switch (opcode & 0x00FF) {
                case 0x009E:
                    chip->PC +=
                      is_key_pressed(chip->keypad, opcode & (0x0F00 >> 8)) ? 2
                                                                           : 0;
                    break;
                case 0x00A1:
                    chip->PC +=
                      !is_key_pressed(chip->keypad, opcode & (0x0F00 >> 8)) ? 2
                                                                            : 0;
                    break;
            }
            break;
        case 0xF000:
            switch (opcode & 0x00FF) {
                case 0x0007:
                    // set delay
                    set_reg((opcode & 0x0F00) >> 8, chip->delay);
                    break;
                case 0x000A:
                    chip->PC -= 2;
                    for (uint16_t i = 0; i < 16; i++) {
                        if (chip->keypad & (0x8000 >> i)) {
                            set_reg((opcode & 0x0F00) >> 8, i);
                            chip->PC += 2;
                            break;
                        }
                    }
                    break;
                case 0x0015:
                    chip->delay = _V((opcode & 0x0F00) >> 8);
                    break;
                case 0x0018:
                    chip->sound = _V((opcode & 0x0F00) >> 8);
                    break;
                case 0x001E:
                    chip->I += _V((opcode & 0x0F00) >> 8);
                    break;
                case 0x0029:
                    // 5 - wide font. 0x50-> starting address for fonts
                    chip->I = 0x50 + (5 * (opcode & 0x0F00) >> 8);
                    break;
                case 0x0033:
                    bcd = _V((opcode & 0x0F00) >> 8);
                    set_mem(chip->I, (bcd % 1000) / 100);
                    set_mem((chip->I) + 1, (bcd % 100) / 10);
                    set_mem((chip->I) + 2, bcd % 10);
                    break;
                case 0x0055: {
                    for (int i = 0; i <= ((opcode & 0x0F00) >> 8); i++) {
                        set_mem((chip->I) + i, _V(i));
                    }
                    break;
                }
                case 0x0065: {
                    for (int i = 0; i <= ((opcode & 0x0F00) >> 8); i++) {
                        set_reg(i, read_mem(chip->I + i));
                    }
                    break;
                }
            }
            break;
        default:
            printf("Unkown opcode\n");
    }
}

void
emulateCycle(chip8* chip)
{
    // fetch opcode
    uint16_t opcode = get_opcode(chip->PC);

    // decode opcode
    decode_exec(opcode, chip);

    // Update Timers
    if (chip->delay > 0)
        chip->delay--;

    if (chip->sound > 0)
        chip->sound--;
}

void
input(chip8* vm, bool* is_running)
{
    SDL_Event ev;
    while (SDL_PollEvent(&ev)) {
        printf("EV TYPE: %x, KEY: %c\n", ev.type, ev.key.keysym.sym);
        switch (ev.type) {
            case SDL_QUIT:
                *is_running = false;
                break;
            case SDL_KEYDOWN:
                switch (ev.key.keysym.sym) { // set the keypress
                    case SDLK_1:
                        vm->keypad |= KEY_1;
                        break;
                    case SDLK_2:
                        vm->keypad |= KEY_2;
                        break;
                    case SDLK_3:
                        vm->keypad |= KEY_3;
                        break;
                    case SDLK_4:
                        vm->keypad |= KEY_C;
                        break;
                    case SDLK_q:
                        vm->keypad |= KEY_4;
                        break;
                    case SDLK_w:
                        vm->keypad |= KEY_5;
                        break;
                    case SDLK_e:
                        vm->keypad |= KEY_6;
                        break;
                    case SDLK_r:
                        vm->keypad |= KEY_D;
                        break;
                    case SDLK_a:
                        vm->keypad |= KEY_7;
                        break;
                    case SDLK_s:
                        vm->keypad |= KEY_8;
                        break;
                    case SDLK_d:
                        vm->keypad |= KEY_9;
                        break;
                    case SDLK_f:
                        vm->keypad |= KEY_E;
                        break;
                    case SDLK_z:
                        vm->keypad |= KEY_A;
                        break;
                    case SDLK_x:
                        vm->keypad |= KEY_0;
                        break;
                    case SDLK_c:
                        vm->keypad |= KEY_B;
                        break;
                    case SDLK_v:
                        vm->keypad |= KEY_F;
                        break;
                    case SDLK_p:
                        *(is_running) = false;
                        break;
                    // TODO add a clear screen/reset button
                    case SDLK_o:
                        reset(vm);
                        break;
                    default:
                        break;
                }
                break;
            case SDL_KEYUP:
                switch (ev.key.keysym.sym) { // set the keypress
                    case SDLK_1:
                        vm->keypad &= ~(KEY_1);
                        break;
                    case SDLK_2:
                        vm->keypad &= ~(KEY_2);
                        break;
                    case SDLK_3:
                        vm->keypad &= ~(KEY_3);
                        break;
                    case SDLK_4:
                        vm->keypad &= ~(KEY_C);
                        break;
                    case SDLK_q:
                        vm->keypad &= ~(KEY_4);
                        break;
                    case SDLK_w:
                        vm->keypad &= ~(KEY_5);
                        break;
                    case SDLK_e:
                        vm->keypad &= ~(KEY_6);
                        break;
                    case SDLK_r:
                        vm->keypad &= ~(KEY_D);
                        break;
                    case SDLK_a:
                        vm->keypad &= ~(KEY_7);
                        break;
                    case SDLK_s:
                        vm->keypad &= ~(KEY_8);
                        break;
                    case SDLK_d:
                        vm->keypad &= ~(KEY_9);
                        break;
                    case SDLK_f:
                        vm->keypad &= ~(KEY_E);
                        break;
                    case SDLK_z:
                        vm->keypad &= ~(KEY_A);
                        break;
                    case SDLK_x:
                        vm->keypad &= ~(KEY_0);
                        break;
                    case SDLK_c:
                        vm->keypad &= ~(KEY_B);
                        break;
                    case SDLK_v:
                        vm->keypad &= ~(KEY_F);
                        break;
                }
                break;
        }
    }
}

bool
check_collision(uint8_t target, uint8_t frame)
{
    return target & frame;
}

void
set_screen(chip8* vm, uint8_t _x_coord, uint8_t _y_coord, uint8_t height)
{
    // draw for N number of rows
    set_reg(V_FLAG, 0x0);
    for (int row = 0; row < height; row++) {
        uint8_t sprite = read_mem(vm->I + row);
        // pixel value of the current screen
        for (int col = 0; col < 8; col++) {
            uint8_t target_px = sprite & (0x80 >> col);
            // is the px set
            if (target_px) {
                if (get_px(vm->gfx, _x_coord + col, _y_coord + row) &
                    ACTIVE_PX) {
                    // collision
                    set_reg(V_FLAG, 0x1);
                    set_frame(
                      vm->gfx, _x_coord + col, _y_coord + row, INACTIVE_PX);
                } else {
                    set_frame(
                      vm->gfx, _x_coord + col, _y_coord + row, ACTIVE_PX);
                }
            }
        }
    }
    // set draw flag
    vm->draw = true;
}
