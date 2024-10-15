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
static int canvas_w = 0, canvas_h = 0;

static int fd_fb = -1, fd_dispinfo = -1, fd_event = -1;

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
	assert(fd_event != -1);
	int cnt = read(fd_event, (void *)buf, len);
	return cnt;
}

void NDL_OpenCanvas(int *w, int *h) {
  // something not know
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

	// add function of pa3
	// if w and w equal 0
	if (*w == 0 && *h == 0){
		canvas_w = screen_w;
		*w = screen_w;

		canvas_h = screen_h;
		*h = screen_h;
		return;
	}

	// if w and w are specified
	canvas_w = *w;
	canvas_h = *h;

	printf("in open_canvas: canvas size: w=%d, h=%d\n", canvas_w, canvas_h);
}

void NDL_DrawRect(uint32_t *pixels, int x, int y, int w, int h) {
	printf("now in NDL_drawrect: x=%d, y=%d, w=%d, h=%d\n", x,y,w,h);
	int i, offset;
	int fd = open("/dev/fb", 0);
	for(i=0; i<h; i++){
		offset = (y+i) * screen_w + x;
		printf("NDL_drawrect: lseek to offset=%d, write %d pixels from pixels+%d*%d\n", offset,w, w,i);
		lseek(fd, offset, SEEK_SET);
		write(fd, pixels + w*i, w);
	}
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
	
	// temporarily store screen size
	char temp[50];
	
	// open device files
	fd_dispinfo = open("/proc/dispinfo", 0, 0);
	fd_event    = open("/dev/events", 0, 0);
	fd_fb       = open("/dev/fb", 0, 0);
	
	// read screen size
	int cnt = read(fd_dispinfo, temp, 50);
	assert(cnt > 0);

	cnt = sscanf(temp, "WIDTH:%d HEIGHT:%d", &screen_w, &screen_h);
	assert(cnt == 2);
	
	printf("in sdl_init: screen width: w=%d, h=%d\n", screen_w, screen_h);
	
	if (getenv("NWM_APP")) {
    evtdev = 3;
  }
  return 0;
}

void NDL_Quit() {
	close(fd_event);
	close(fd_fb);
	close(fd_dispinfo);
}
