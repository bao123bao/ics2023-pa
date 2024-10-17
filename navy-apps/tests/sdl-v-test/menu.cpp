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


static void clear_display(void) {
  SDL_FillRect(screen, NULL, 0xffffff);
}

int main(int argc, char *argv[], char *envp[]) {
  SDL_Init(0);
  screen = SDL_SetVideoMode(0, 0, 32, SDL_HWSURFACE);

  logo_sf = SDL_LoadBMP("/share/pictures/projectn.bmp");
  assert(logo_sf);
  set_i_max();
	
	//display_menu(1);
	
  while (1) {
  	printf("navy: new while loop\n");		
		clear_display();
		SDL_Rect rect = { .x = screen->w - logo_sf->w, .y = 0 };
		
		printf("display: call blit\n");
		SDL_BlitSurface(logo_sf, NULL, screen, &rect);
		
		printf("Available applications:\n");
		
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
		
  }
  return -1;
}

static void display_menu(int n) {
  clear_display();
  SDL_Rect rect = { .x = screen->w - logo_sf->w, .y = 0 };
  printf("display: call blit\n");
	SDL_BlitSurface(logo_sf, NULL, screen, &rect);
  printf("Available applications:\n");
  /*
	char buf[80];
  int i;
  for (i = 0; i <= n; i ++) {
    auto *item = &items[page * 10 + i];
    sprintf(buf, "  [%d] %s", i, item->name);
    draw_text_row(buf, i);
  }

  i = 11;

  sprintf(buf, "  page = %2d, #total apps = %d", page, nitems);
  draw_text_row(buf, i);
  i ++;

  sprintf(buf, "  help:");
  draw_text_row(buf, i);
  i ++;

  sprintf(buf, "  <-  Prev Page");
  draw_text_row(buf, i);
  i ++;

  sprintf(buf, "  ->  Next Page");
  draw_text_row(buf, i);
  i ++;

  sprintf(buf, "  0-9 Choose");
  draw_text_row(buf, i);
  i ++;
*/
  SDL_UpdateRect(screen, 0, 0, 0, 0);

  printf("========================================\n");
  printf("Please Choose.\n");
  fflush(stdout);
}
