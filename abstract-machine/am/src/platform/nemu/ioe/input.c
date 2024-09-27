#include <am.h>
#include <nemu.h>
#include <stdio.h>
#include <assert.h>
#define KEYDOWN_MASK 0x8000

void __am_input_keybrd(AM_INPUT_KEYBRD_T *kbd) {
	//uint32_t kc = inl(KBD_ADDR);
    //kbd->keydown = kc & KEYDOWN_MASK ? true : false;
   // kbd->keycode = kc & ~KEYDOWN_MASK;
	//printf("__am_input_keybrd called\n");
	uint32_t scancode = inl(KBD_ADDR);
	//assert(KBD_ADDR == 0xa0000060);
	//printf("scancode: 0x%d @ 0x%d\n", scancode, KBD_ADDR);
	kbd->keycode = scancode & ~KEYDOWN_MASK;
	kbd->keydown = scancode & KEYDOWN_MASK;
	
}
