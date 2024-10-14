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

size_t serial_write(const void *buf, size_t offset, size_t len) {
  char *chars = (char *)buf;
	int i;
	for(i=0; i<len; i++){
		putch(chars[i]);
	}
	return len;
}

size_t events_read(void *buf, size_t offset, size_t len) {
	printf("in events_read\n");
	AM_INPUT_KEYBRD_T ev = io_read(AM_INPUT_KEYBRD);
//	char temp[100];
	int cnt = 0;

	if(ev.keycode == AM_KEY_NONE)
		return 0;

	if(ev.keydown){
		cnt = sprintf(buf, "kd %s\n", keyname[ev.keycode]); 
	}else{
		cnt = sprintf(buf, "ku %s\n", keyname[ev.keycode]);
	}
	assert(cnt < len);
//	strcpy(buf, temp);

	return cnt;
}

size_t dispinfo_read(void *buf, size_t offset, size_t len) {
  return 0;
}

size_t fb_write(const void *buf, size_t offset, size_t len) {
  return 0;
}

void init_device() {
  Log("Initializing devices...");
  ioe_init();
}
