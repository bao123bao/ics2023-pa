#include <am.h>
#include <nemu.h>
#include <klib.h>
#define SYNC_ADDR (VGACTL_ADDR + 4)


// newly added regs

void __am_screen_size(AM_SCREEN_SIZE_T *size) {
	uint32_t data = inl(VGACTL_ADDR);
	size->width = (int) (data >> 16);
	size->height = (int) (data & 0xFFFF);
}

void __am_screen_sync(AM_SCREEN_SYNC_T *sync) {
	sync->sync = inl(SYNC_ADDR);
}

// existed regs
void __am_gpu_init() {
	int i;
	AM_SCREEN_SIZE_T size;
	__am_screen_size(&size);
	int w = size.width;    // TODO: get the correct width
	int h = size.height;   // TODO: get the correct height
	
	uint32_t *fb = (uint32_t *)(uintptr_t)FB_ADDR;
	for (i = 0; i < w * h; i ++) fb[i] = i;
	outl(SYNC_ADDR, 1);
}

void __am_gpu_config(AM_GPU_CONFIG_T *cfg) {
	AM_SCREEN_SIZE_T size;
	__am_screen_size(&size);

	*cfg = (AM_GPU_CONFIG_T) {
    .present = true, .has_accel = false,
    .width = size.width, .height = size.height,
    .vmemsz = 0
  };
}

void __am_gpu_fbdraw(AM_GPU_FBDRAW_T *ctl) {
	int i, j;
	int offset = 0;
	
	AM_SCREEN_SIZE_T size;
	__am_screen_size(&size);

	uint32_t color = ((uint32_t *)(uintptr_t) ctl->pixels)[0];

	for(i = 0; i < ctl->h; i++){
		for(j = 0; j < ctl->w; j++){
			offset = (ctl->y + i)*size.width + (ctl->x + j);
		
//		if(ctl->x == 372 && ctl->y == 279){
//			printf("i=%d, j=%d, offset=%d, x=%d, y=%d, w=%d, h=%d\n", 
//				i, j, offset, ctl->x, ctl->y, ctl->w, ctl->h);
//		}

			outl(FB_ADDR + offset, color);
		}
	}
	if (ctl->sync) {
    outl(SYNC_ADDR, 1);
  }
}

void __am_gpu_status(AM_GPU_STATUS_T *status) {
  status->ready = true;
}
