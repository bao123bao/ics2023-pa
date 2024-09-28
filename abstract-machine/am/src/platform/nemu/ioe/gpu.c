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
	return;
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
	uint32_t *colorbuf;
	//uint32_t *fb;
	int color_idx = 0;
	int w, h, x, y;

	AM_SCREEN_SIZE_T size;
	__am_screen_size(&size);
//	printf("screen width=%d, height=%d\n", size.width, size.height);

	w = size.width;
	h = size.height;

	x = ctl->x;
	y = ctl->y;

	if (ctl->pixels) {
//		fb = (uint32_t *)(uintptr_t)FB_ADDR;
		colorbuf = ((uint32_t *)(uintptr_t) ctl->pixels);
//		printf("x=%d, y=%d, w=%d, h=%d\n",
// 			ctl->x, ctl->y, ctl->w, ctl->h);
	for(i = 0; i < h; i++){
		for(j = 0; j < w; j++){
			offset = (y + i) * w + (x + j);
//			printf("i=%d, j=%d, offset=%d, x=%d, y=%d, w=%d, h=%d\n", 
//				i, j, offset, ctl->x, ctl->y, ctl->w, ctl->h);
//			fd[] = colorbuf[];
			outl(FB_ADDR + offset*4, colorbuf[color_idx++]);
		}
	}
	}

	if (ctl->sync) {
//		printf("fbdraw sync\n");
    outl(SYNC_ADDR, 1);
  }
}

void __am_gpu_status(AM_GPU_STATUS_T *status) {
  status->ready = true;
}
