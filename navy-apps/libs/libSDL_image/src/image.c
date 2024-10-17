#include <stdlib.h>
#include <stdio.h>
#include <assert.h>

#define SDL_malloc  malloc
#define SDL_free    free
#define SDL_realloc realloc

#define SDL_STBIMAGE_IMPLEMENTATION
#include "SDL_stbimage.h"

SDL_Surface* IMG_Load_RW(SDL_RWops *src, int freesrc) {
  assert(src->type == RW_TYPE_MEM);
  assert(freesrc == 0);
  return NULL;
}

SDL_Surface* IMG_Load(const char *filename) {
	SDL_Surface *psf = NULL;
	FILE *f = fopen(filename, "r");

	fseek(f, 0, SEEK_END);
	int fsize = ftell(f);
	fseek(f, 0, SEEK_SET);

	unsigned char *buf = malloc(fsize);
	assert(buf);
	printf("IMG_load: filename=<%s>, fsize=%d\n", filename, fsize);
	psf = STBIMG_LoadFromMemory(buf, fsize);
	assert(psf);
	
	fclose(f);
	free(buf);

	return psf;
}

int IMG_isPNG(SDL_RWops *src) {
  return 0;
}

SDL_Surface* IMG_LoadJPG_RW(SDL_RWops *src) {
  return IMG_Load_RW(src, 0);
}

char *IMG_GetError() {
  return "Navy does not support IMG_GetError()";
}
