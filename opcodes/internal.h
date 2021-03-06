#pragma once
#include "stdbool.h"
#include <stdint.h>

// set_frame sets the value at a particular x and y coordinate
// NOTE Does not check for boundaries
void
set_frame(uint32_t* frame, uint8_t _x_coord, uint8_t _y_coord, uint32_t value);

// get_frame returns the value in the x and y coord in the frame
// NOTE: Does not check for boundaries
uint32_t
get_px(uint32_t* frame, uint8_t _x_coord, uint8_t _y_coord);

void
wrap_coords(uint8_t* _x_coord, uint8_t* _y_coord);

// check if key pressed is same as the value in the register
bool
is_key_pressed(uint16_t _key_map, uint8_t _val);
