#include <SDL.h>
#include <SDL_bmp.h>
#include <stdio.h>
#include <assert.h>

int main() {
  SDL_Init(0);

  while (1) {
    printf("new while loop\n");
		SDL_Event e;
    SDL_WaitEvent(&e);

    if (e.type == SDL_KEYDOWN) {
      switch(e.key.keysym.sym) {
        case SDLK_J:
        case SDLK_DOWN: 
        	printf("tester: J or Down catched\n");
        	break;
        case SDLK_K:
        case SDLK_UP:
        	printf("tester: K or Up catched\n");
        	break;
      }
    }
  }

  return 0;
}
