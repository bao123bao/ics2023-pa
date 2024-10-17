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
  
	char buf[50];
	char namebuf[20];
	char key_act;
	int cnt;
	
	if(NDL_PollEvent(buf, sizeof(buf))==0)
		return 0;
	
	// get key up or down
	key_act = buf[1];
	if(key_act == 'u'){
		ev->type = SDL_KEYUP;
		sscanf(buf, "ku %s", namebuf);
	}else if(key_act == 'd'){
		ev->type = SDL_KEYDOWN;
		sscanf(buf, "kd %s", namebuf);
	}else{
		assert(0);
	}
	
	int i, nlen;
	nlen = sizeof(keyname) / sizeof(char*);

	for(i=0; i<nlen; i++){
		if(strcmp(keyname[i], namebuf) == 0){
			assert(i != 0);
			ev->key.keysym.sym = i;
			return 1;
		}
	}

	printf("no keyname matched\n");
	return 0;
}

int SDL_WaitEvent(SDL_Event *event) {
	char buf[50];
	char namebuf[20];
	char key_act;
	int cnt;
	
	while(1) {
		if(NDL_PollEvent(buf, sizeof(buf)))
			break;
	}
	
	// get key up or down
	key_act = buf[1];
	if(key_act == 'u'){
		event->type = SDL_KEYUP;
		sscanf(buf, "ku %s", namebuf);
	}else if(key_act == 'd'){
		event->type = SDL_KEYDOWN;
		sscanf(buf, "kd %s", namebuf);
	}else{
		assert(0);
	}
	
	int i, nlen;
	nlen = sizeof(keyname) / sizeof(char*);

	for(i=0; i<nlen; i++){
		if(strcmp(keyname[i], namebuf) == 0){
			assert(i != 0);
			event->key.keysym.sym = i;
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
