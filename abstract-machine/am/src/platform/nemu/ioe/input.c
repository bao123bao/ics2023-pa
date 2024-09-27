#include <am.h>
#include <nemu.h>
#include <stdio.h>
#define KEYDOWN_MASK 0x8000

void __am_input_keybrd(AM_INPUT_KEYBRD_T *kbd) {
	printf("__am_input_keybrd called ");
	kbd->keycode = AM_KEY_NONE;
	kbd->keydown = 0;
		
	uint32_t scancode = inl(KBD_ADDR);
	printf("scancode: 0x%x @ 0x%x\n", scancode, KBD_ADDR);
	kbd->keycode = scancode & (KEYDOWN_MASK - 1);
	kbd->keydown = scancode & KEYDOWN_MASK;
	
}
