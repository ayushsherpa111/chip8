#include "internal.h"
#include "chip8.h"
#include <stdint.h>

void
set_frame(uint32_t* frame, uint8_t _x_coord, uint8_t _y_coord, uint32_t value)
{
    // formula for treating a 1d array like a 2d array
    frame[X_SCREEN * _y_coord + _x_coord] = value;
}

uint32_t
get_px(uint32_t* frame, uint8_t _x_coord, uint8_t _y_coord)
{
    return frame[X_SCREEN * _y_coord + _x_coord];
}

void
wrap_coords(uint8_t* _x_coord, uint8_t* _y_coord)
{
    if (*_x_coord >= X_SCREEN)
        *_x_coord %= X_SCREEN;
    if (*_x_coord < 0)
        *_x_coord += X_SCREEN;

    if (*_y_coord >= Y_SCREEN)
        *_y_coord %= Y_SCREEN;
    if (*_y_coord < 0)
        *_y_coord += Y_SCREEN;
}

bool
is_key_pressed(uint16_t _key_map, uint8_t _val)
{
    return (_key_map >> _val) & 0x1;
}
