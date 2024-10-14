#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/time.h>
#include <NDL.h>
#include <assert.h>

static int evtdev = -1;
static int fbdev = -1;
static int screen_w = 0, screen_h = 0;

static struct timeval tv_init = {};

// return us since NDL init
uint32_t NDL_GetTicks() {
  struct timeval tv;
	gettimeofday(&tv, NULL);
	return (tv.tv_sec - tv_init.tv_sec)*1000000 + (tv.tv_usec - tv_init.tv_usec);
}

int NDL_PollEvent(char *buf, int len) {
	// read 0 bytes, return 0
	if(len == 0)
		return 0;

	// read from device events
	int fd = open("/dev/events", 0, 0);
	assert(fd != -1);

	return read(fd, (void *)buf, len);
}

void NDL_OpenCanvas(int *w, int *h) {
  if (getenv("NWM_APP")) {
    int fbctl = 4;
    fbdev = 5;
    screen_w = *w; screen_h = *h;
    char buf[64];
    int len = sprintf(buf, "%d %d", screen_w, screen_h);
    // let NWM resize the window and create the frame buffer
    write(fbctl, buf, len);
    while (1) {
      // 3 = evtdev
      int nread = read(3, buf, sizeof(buf) - 1);
      if (nread <= 0) continue;
      buf[nread] = '\0';
      if (strcmp(buf, "mmap ok") == 0) break;
    }
    close(fbctl);
  }
}

void NDL_DrawRect(uint32_t *pixels, int x, int y, int w, int h) {
}

void NDL_OpenAudio(int freq, int channels, int samples) {
}

void NDL_CloseAudio() {
}

int NDL_PlayAudio(void *buf, int len) {
  return 0;
}

int NDL_QueryAudio() {
  return 0;
}

int NDL_Init(uint32_t flags) {
  
	// set timeval_init
	gettimeofday(&tv_init, NULL);

	if (getenv("NWM_APP")) {
    evtdev = 3;
	
  }
  return 0;
}

void NDL_Quit() {
}
