#include <am.h>
#include <nemu.h>

#define KEYDOWN_MASK 0x8000

void __am_input_keybrd(AM_INPUT_KEYBRD_T *kbd) {
	
	kbd->keycode = inl(KBD_ADDR);
//	kbd->keydown = 0;
	if(kbd->keycode != 0){
		kbd->keydown = 1;
	}else{
		kbd->keydown = 0;
	}
	
}
