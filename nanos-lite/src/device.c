#include <common.h>

#if defined(MULTIPROGRAM) && !defined(TIME_SHARING)
# define MULTIPROGRAM_YIELD() yield()
#else
# define MULTIPROGRAM_YIELD()
#endif

#define NAME(key) \
  [AM_KEY_##key] = #key,

static const char *keyname[256] __attribute__((used)) = {
  [AM_KEY_NONE] = "NONE",
  AM_KEYS(NAME)
};

static int screen_w, screen_h;

size_t serial_write(const void *buf, size_t offset, size_t len) {
  char *chars = (char *)buf;
	int i;
	for(i=0; i<len; i++){
		putch(chars[i]);
	}
	return len;
}

size_t events_read(void *buf, size_t offset, size_t len) {
//	printf("in events_read\n");
	AM_INPUT_KEYBRD_T ev = io_read(AM_INPUT_KEYBRD);
//	char temp[100];
	int cnt = 0;

	if(ev.keycode == AM_KEY_NONE)
		return 0;

	if(ev.keydown){
		cnt = sprintf(buf, "kd %s\n", keyname[ev.keycode]); 
//		printf("evens_read: catch\n");
	}else{
		cnt = sprintf(buf, "ku %s\n", keyname[ev.keycode]);
	//	printf("evens_read: catch\n");
	}
	assert(cnt < len);
//	strcpy(buf, temp);

	return cnt;
}

size_t dispinfo_read(void *buf, size_t offset, size_t len) {
  screen_w = io_read(AM_GPU_CONFIG).width;
  screen_h = io_read(AM_GPU_CONFIG).height;
	int ret = sprintf(buf, "WIDTH:%d\nHEIGHT:%d", screen_w, screen_h);
	return ret;
}

size_t fb_write(const void *buf, size_t offset, size_t len) {
	
	// only write one line
	int x, y;

	y = (offset+1) / screen_w;
	x = ((offset+1) % screen_w) - 1;

	// if exceeds boundary, return 0
	if (x >= screen_w || y >= screen_h)
		return 0;
	
	// if not exceed, write to vmem and return len
	io_write(AM_GPU_FBDRAW, x, y, buf, len, 1, true);

	return len;
}

void init_device() {
  Log("Initializing devices...");
  ioe_init();
}
