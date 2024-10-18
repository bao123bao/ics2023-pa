#include <NDL.h>
#include <sdl-video.h>
#include <assert.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
void SDL_BlitSurface(SDL_Surface *src, SDL_Rect *srcrect, SDL_Surface *dst, SDL_Rect *dstrect) {
	
	//printf("in sdl blit\n");
	// ensure the validity of args
	assert(dst && src);
  assert(dst->format->BitsPerPixel == src->format->BitsPerPixel);
	
	printf("SDL_BlitSurface: src bpp=%d, dst bpp=%d\n", 
		src->format->BitsPerPixel, dst->format->BitsPerPixel);

	int di, dj, si, sj;
	int di_start, dj_start, si_start, sj_start;
	int w_len, h_len;
	int i, j;
	uint32_t *dst_pixel_ptr_32;
	uint32_t *src_pixel_ptr_32;
	
	uint8_t *dst_pixel_ptr_8;
	uint8_t *src_pixel_ptr_8;
	
	// set src rect location and size
	if (srcrect){
		si_start = srcrect->y;
		sj_start = srcrect->x;
		w_len = srcrect->w;
		h_len = srcrect->h;
	}else{
		si_start = 0;
		sj_start = 0;
		w_len = src->w;
		h_len = src->h;
	}
	
	// set dest rect location
	if (dstrect){
		di_start = dstrect->y;
		dj_start = dstrect->x;
	}else{
		di_start = 0;
		dj_start = 0;
	}
	
	//printf("copy src(%d,%d,%d,%d) to dst(%d,%d)\n", 
		//sj_start,si_start,w_len,h_len,dj_start,di_start);
	// copy from src_sf to dst_sf
	for(i = 0; i < h_len; i++){
		di = di_start + i;
		si = si_start + i;
		
		for(j = 0; j < w_len; j++){
			dj = dj_start + j;
			sj = sj_start + j;
			
			// copy element form src[si][sj] to dst[di][dj]
			if(dst->format->BitsPerPixel == 32){
				dst_pixel_ptr_32 = (uint32_t *)dst->pixels + di * dst->w + dj;
  	    src_pixel_ptr_32 = (uint32_t *)src->pixels + si * src->w + sj;
    	  *dst_pixel_ptr_32 = *src_pixel_ptr_32;
			}else if(dst->format->BitsPerPixel == 8){
				dst_pixel_ptr_8 = (uint8_t *)dst->pixels + di * dst->w + dj;
  	    src_pixel_ptr_8 = (uint8_t *)src->pixels + si * src->w + sj;
    	  *dst_pixel_ptr_8 = *src_pixel_ptr_8;
			}else{
				assert(0);
			}
		}
	}
}

void SDL_FillRect(SDL_Surface *dst, SDL_Rect *dstrect, uint32_t color) {
	printf("int sdl_fillrect\n");
	int i, j, di, dj;
	int di_start, dj_start, w_len, h_len;
	uint32_t *pixel_ptr_32;
	uint8_t  *pixel_ptr_8;
	
	if(dstrect){
		di_start = dstrect->y;
		dj_start = dstrect->x;
		w_len = dstrect->w;
		h_len = dstrect->h;
	}else{
		di_start = 0;
		dj_start = 0;
		w_len = dst->w;
		h_len = dst->h;
	}

	printf("fillrect:x=%d,y=%d,w=%d,h=%d\n",dj_start,di_start,w_len,h_len);
	
	// update pixels in dst surface to color
	for(i = 0; i < h_len; i++){
		di = di_start + i;
		for(j = 0; j < w_len; j++){
			dj = dj_start + j;
			//printf("pixel=(i=%d,j=%d)\n",di,dj);
			
			if(dst->format->BitsPerPixel == 32){
				pixel_ptr_32 = (uint32_t *)dst->pixels + di * dst->w + dj;
				//printf("rectfill: ptr=%p\n", pixel_ptr);
				*pixel_ptr_32 = color;
			}else if(dst->format->BitsPerPixel == 8){
				pixel_ptr_8 = (uint8_t *)dst->pixels + di * dst->w + dj;
				*pixel_ptr_8 = (uint8_t) color;
			}
		}
	}
	printf("rectfill return");
}

void SDL_UpdateRect(SDL_Surface *s, int x, int y, int w, int h) {
	// fill the full screen area
	printf("SDL_UpdateRect call NDL_DrawRect:x=%d,y=%d,w=%d,h=%d, color_type=%d\n",x,y,w,h,s->format->BitsPerPixel);
	
	int update_x, update_y, update_w, update_h;
	
	// full screen update
	if (x == 0 && y == 0 && w == 0 && h == 0){
		update_w = s->w;
		update_h = s->h;
		update_x = 0;
		update_y = 0;
		
	}else{ // update partial screen using args
		update_w = w;
		update_h = h;
		update_x = x;
		update_y = y;
	}
	
	// BitsPerPixel is 32
	if(s->format->BitsPerPixel == 32){
		NDL_DrawRect((uint32_t *)s->pixels, update_x, update_y, update_w, update_h);
		return;
	}
	 
	// BitsPerPixel is 8, use palette data
	if(s->format->BitsPerPixel == 8){
		int len = s->w * s->h;
		uint32_t *colorbuf = malloc(s->w * s->h * sizeof(uint32_t));
		uint8_t *pixels = (uint8_t *)s->pixels;
		SDL_Color *colors = s->format->palette->colors;
		
		printf("SDL_UpdateRect: colorbuf <len=%d, size=%d>, palette len=%d\n", 
			len, sizeof(colorbuf), s->format->palette->ncolors);
		
		for(int i=0; i<len; i++){
			colorbuf[i] = colors[pixels[i]].val;
		}
		printf("loop over\n");
		NDL_DrawRect(colorbuf, update_x, update_y, update_w, update_h);
	
	}
	printf("SDL_UpdateRect return\n");
	
}

// APIs below are already implemented.

static inline int maskToShift(uint32_t mask) {
  switch (mask) {
    case 0x000000ff: return 0;
    case 0x0000ff00: return 8;
    case 0x00ff0000: return 16;
    case 0xff000000: return 24;
    case 0x00000000: return 24; // hack
    default: assert(0);
  }
}

SDL_Surface* SDL_CreateRGBSurface(uint32_t flags, int width, int height, int depth,
    uint32_t Rmask, uint32_t Gmask, uint32_t Bmask, uint32_t Amask) {
  assert(depth == 8 || depth == 32);
  SDL_Surface *s = malloc(sizeof(SDL_Surface));
  assert(s);
  s->flags = flags;
  s->format = malloc(sizeof(SDL_PixelFormat));
  assert(s->format);
  if (depth == 8) {
    s->format->palette = malloc(sizeof(SDL_Palette));
    assert(s->format->palette);
    s->format->palette->colors = malloc(sizeof(SDL_Color) * 256);
    assert(s->format->palette->colors);
    memset(s->format->palette->colors, 0, sizeof(SDL_Color) * 256);
    s->format->palette->ncolors = 256;
  } else {
    s->format->palette = NULL;
    s->format->Rmask = Rmask; s->format->Rshift = maskToShift(Rmask); s->format->Rloss = 0;
    s->format->Gmask = Gmask; s->format->Gshift = maskToShift(Gmask); s->format->Gloss = 0;
    s->format->Bmask = Bmask; s->format->Bshift = maskToShift(Bmask); s->format->Bloss = 0;
    s->format->Amask = Amask; s->format->Ashift = maskToShift(Amask); s->format->Aloss = 0;
  }

  s->format->BitsPerPixel = depth;
  s->format->BytesPerPixel = depth / 8;

  s->w = width;
  s->h = height;
  s->pitch = width * depth / 8;
  assert(s->pitch == width * s->format->BytesPerPixel);

  if (!(flags & SDL_PREALLOC)) {
    s->pixels = malloc(s->pitch * height);
    assert(s->pixels);
  }

  return s;
}

SDL_Surface* SDL_CreateRGBSurfaceFrom(void *pixels, int width, int height, int depth,
    int pitch, uint32_t Rmask, uint32_t Gmask, uint32_t Bmask, uint32_t Amask) {
  SDL_Surface *s = SDL_CreateRGBSurface(SDL_PREALLOC, width, height, depth,
      Rmask, Gmask, Bmask, Amask);
  assert(pitch == s->pitch);
  s->pixels = pixels;
  return s;
}

void SDL_FreeSurface(SDL_Surface *s) {
  if (s != NULL) {
    if (s->format != NULL) {
      if (s->format->palette != NULL) {
        if (s->format->palette->colors != NULL) free(s->format->palette->colors);
        free(s->format->palette);
      }
      free(s->format);
    }
    if (s->pixels != NULL && !(s->flags & SDL_PREALLOC)) free(s->pixels);
    free(s);
  }
}

SDL_Surface* SDL_SetVideoMode(int width, int height, int bpp, uint32_t flags) {
  if (flags & SDL_HWSURFACE) NDL_OpenCanvas(&width, &height);
  return SDL_CreateRGBSurface(flags, width, height, bpp,
      DEFAULT_RMASK, DEFAULT_GMASK, DEFAULT_BMASK, DEFAULT_AMASK);
}

void SDL_SoftStretch(SDL_Surface *src, SDL_Rect *srcrect, SDL_Surface *dst, SDL_Rect *dstrect) {
  assert(src && dst);
  assert(dst->format->BitsPerPixel == src->format->BitsPerPixel);
  assert(dst->format->BitsPerPixel == 8);

  int x = (srcrect == NULL ? 0 : srcrect->x);
  int y = (srcrect == NULL ? 0 : srcrect->y);
  int w = (srcrect == NULL ? src->w : srcrect->w);
  int h = (srcrect == NULL ? src->h : srcrect->h);

  assert(dstrect);
  if(w == dstrect->w && h == dstrect->h) {
    /* The source rectangle and the destination rectangle
     * are of the same size. If that is the case, there
     * is no need to stretch, just copy. */
    SDL_Rect rect;
    rect.x = x;
    rect.y = y;
    rect.w = w;
    rect.h = h;
    SDL_BlitSurface(src, &rect, dst, dstrect);
  }
  else {
    //return;
		assert(0);
  }
}

void SDL_SetPalette(SDL_Surface *s, int flags, SDL_Color *colors, int firstcolor, int ncolors) {
  //printf("SDL_SetPalette called\n");
	assert(s);
  assert(s->format);
  assert(s->format->palette);
  assert(firstcolor == 0);

  s->format->palette->ncolors = ncolors;
  memcpy(s->format->palette->colors, colors, sizeof(SDL_Color) * ncolors);

  if(s->flags & SDL_HWSURFACE) {
    assert(ncolors == 256);
    for (int i = 0; i < ncolors; i ++) {
      uint8_t r = colors[i].r;
      uint8_t g = colors[i].g;
      uint8_t b = colors[i].b;
    }
    SDL_UpdateRect(s, 0, 0, 0, 0);
  }
	//printf("SDL_SetPalette return\n");
}

static void ConvertPixelsARGB_ABGR(void *dst, void *src, int len) {
  int i;
  uint8_t (*pdst)[4] = dst;
  uint8_t (*psrc)[4] = src;
  union {
    uint8_t val8[4];
    uint32_t val32;
  } tmp;
  int first = len & ~0xf;
  for (i = 0; i < first; i += 16) {
#define macro(i) \
    tmp.val32 = *((uint32_t *)psrc[i]); \
    *((uint32_t *)pdst[i]) = tmp.val32; \
    pdst[i][0] = tmp.val8[2]; \
    pdst[i][2] = tmp.val8[0];

    macro(i + 0); macro(i + 1); macro(i + 2); macro(i + 3);
    macro(i + 4); macro(i + 5); macro(i + 6); macro(i + 7);
    macro(i + 8); macro(i + 9); macro(i +10); macro(i +11);
    macro(i +12); macro(i +13); macro(i +14); macro(i +15);
  }

  for (; i < len; i ++) {
    macro(i);
  }
}

SDL_Surface *SDL_ConvertSurface(SDL_Surface *src, SDL_PixelFormat *fmt, uint32_t flags) {
  assert(src->format->BitsPerPixel == 32);
  assert(src->w * src->format->BytesPerPixel == src->pitch);
  assert(src->format->BitsPerPixel == fmt->BitsPerPixel);

  SDL_Surface* ret = SDL_CreateRGBSurface(flags, src->w, src->h, fmt->BitsPerPixel,
    fmt->Rmask, fmt->Gmask, fmt->Bmask, fmt->Amask);

  assert(fmt->Gmask == src->format->Gmask);
  assert(fmt->Amask == 0 || src->format->Amask == 0 || (fmt->Amask == src->format->Amask));
  ConvertPixelsARGB_ABGR(ret->pixels, src->pixels, src->w * src->h);

  return ret;
}

uint32_t SDL_MapRGBA(SDL_PixelFormat *fmt, uint8_t r, uint8_t g, uint8_t b, uint8_t a) {
  assert(fmt->BytesPerPixel == 4);
  uint32_t p = (r << fmt->Rshift) | (g << fmt->Gshift) | (b << fmt->Bshift);
  if (fmt->Amask) p |= (a << fmt->Ashift);
  return p;
}

int SDL_LockSurface(SDL_Surface *s) {
  return 0;
}

void SDL_UnlockSurface(SDL_Surface *s) {
}
