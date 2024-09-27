#include <am.h>
#include <nemu.h>

#define KEYDOWN_MASK 0x8000

void __am_input_keybrd(AM_INPUT_KEYBRD_T *kbd) {
	kbd->keycode = 0;
	kbd->keydown = 0;
	
	uint32_t scancode = inl(KBD_ADDR);
	kbd->keycode = scancode & (KEYDOWN_MASK - 1);
	kbd->keydown = scancode & KEYDOWN_MASK;
	
}
