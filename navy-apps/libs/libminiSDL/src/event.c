#include <NDL.h>
#include <SDL.h>
#include <stdio.h>
#include <string.h>
#include <assert.h>
#define keyname(k) #k,

static const char *keyname[] = {
  "NONE",
  _KEYS(keyname)
};

int SDL_PushEvent(SDL_Event *ev) {
  return 0;
}

int SDL_PollEvent(SDL_Event *ev) {
  return 0;
}

int SDL_WaitEvent(SDL_Event *event) {
	char buf[50];
	char *namebuf;
	char key_act;
	int cnt;
	
	while(1) {
		if(NDL_PollEvent(buf, sizeof(buf)))
			break;
	}

	printf("SDL event from NDL, cnt=%d: %s\n", cnt, buf);
	
	// get key up or down
	key_act = buf[1];
	if(key_act == 'u'){
		event->type = SDL_KEYUP;
		namebuf = buf + 3;
	}else if(key_act == 'd'){
		event->type = SDL_KEYDOWN;
		namebuf = buf + 3;
	}else{
		printf("SDL: key act type error!\n");
		assert(0);
	}
	
	printf("keyname=%s\n",namebuf);
	
	int i, nlen;
	nlen = sizeof(keyname);
	printf("nlen=%d\n", nlen);

	for(i=0; i<nlen; i++){
		if(strcmp(keyname[i], namebuf) == 0){
			assert(i != 0);
			printf("i=%d\n", i);
			event->key.keysym.sym = i;
			printf("SDL key catch: %s\n", namebuf);
			return 1;
		}
	}

	printf("no keyname matched\n");

	return 0;
}

int SDL_PeepEvents(SDL_Event *ev, int numevents, int action, uint32_t mask) {
  return 0;
}

uint8_t* SDL_GetKeyState(int *numkeys) {
  return NULL;
}
