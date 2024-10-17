/*
 * this is the first process in the OS:
 *   display a splash screen
 *   display a boot menu and receive input
 */

#include <stdio.h>
#include <unistd.h>
#include <stdint.h>
#include <fcntl.h>
#include <assert.h>
#include <string.h>
#include <stdlib.h>
#include <SDL.h>
#include <SDL_bmp.h>
#include <SDL_bdf.h>

static SDL_Surface *screen = NULL;
static SDL_Surface *logo_sf = NULL;

static void clear_display(void) {
  SDL_FillRect(screen, NULL, 0xffffff);
}

int main(int argc, char *argv[], char *envp[]) {
  SDL_Init(0);

  logo_sf = SDL_LoadBMP("/share/pictures/projectn.bmp");
  assert(logo_sf);
  
  screen = SDL_SetVideoMode(0, 0, 32, SDL_HWSURFACE);
  printf("MAIN: screen size=(%d,%d)\n",screen->w,screen->h);
	
	printf("navy: call Fillrect\n");
	clear_display();
	printf("navy: Fillrect done\n");
	SDL_Rect rect = { .x = screen->w - logo_sf->w, .y = 0 };
	
	printf("navy: call blit\n");
	SDL_BlitSurface(logo_sf, NULL, screen, &rect);
	printf("navy: call updaterect\n");
	SDL_UpdateRect(screen, 0, 0, 0, 0);
	printf("navy: updaterect done\n");

	/*
  SDL_Event e;
  do {
    SDL_WaitEvent(&e);
  } while (e.type != SDL_KEYDOWN);

  switch (e.key.keysym.sym) {
    case SDLK_LEFT: 
    	printf("left key!\n"); 
    	break;
    case SDLK_RIGHT: 
    	printf("left key!\n"); 
    	break;
  }
  */
	while(1) ;
  return -1;
}

