DOCS:
CHIP-8 occupies the first 512 Bytes of memory

Programs written in Chip-8 begin at memory location 512 (0x200) and don't access memory below 512

The upper most 256 bytes are reserved for display refresh.
And 96 bytes below that are reserved for the call stack, internal use and other variables.

Chip-8 has 16 8-bit registers. Named V0 to VF.
VF register also behaves as a flag for some instructions.

Address register I uses 12 bits and is used to store memory address.
Only the right most 12 bits are used.

Chip-8 has 2 timers.
Delay Timer: used for timing the events of games.
Sound Timer: used for sound effects.

Input is fed using Hex Keyboard ranging from 0 to F,
'8', '4', '6', and '2' keys used for directions.

The display resolution is 64x32 pixels and uses monochrome display.
Graphics drawn using Sprites.
8 pixels wide and 1-16 pixels in height.

To detect a collision when the sprites are redrawn if the color in the corresponding pixel is flipped during and XOR the carry flag is set.

Opcodes are 2 bytes long and arranged in big-endian and a total of 38 opcodes.

Useful links:
http://devernay.free.fr/hacks/chip8/C8TECH10.HTM
