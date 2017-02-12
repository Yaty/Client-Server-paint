#include "display.h"
#include <stdio.h>
#include <unistd.h>

static SDL_Window *sdlWindow = NULL;
static SDL_Renderer *sdlRenderer = NULL;
static SDL_Texture *sdlScreenTexture = NULL;
static SDL_Texture *sdlTexture = NULL;

/**
 * Opens a window with a title and a size.
 * @param caption the title of the window
 * @param width the width of the window
 * @param height the height of the window
 * @return 0 if ok, 1 if error
 */
int displayInit(const char* caption, int width, int height) {
  // First init the SDL system
  if (SDL_Init(SDL_INIT_VIDEO) != 0){
    printf("SDL_Init failed: %s\n", SDL_GetError());
    return 1;
  }
  // Second creates the window
  sdlWindow = SDL_CreateWindow(caption,
			       SDL_WINDOWPOS_UNDEFINED,
			       SDL_WINDOWPOS_UNDEFINED,
			       width, height,
			       SDL_WINDOW_OPENGL);
  if (sdlWindow == NULL) {
    printf("SDL_CreateWindow failed: %s\n", SDL_GetError());
    return 1;
  }
  // Third creates the renderer for this window
  sdlRenderer = SDL_CreateRenderer(sdlWindow, -1, 0);
  if (sdlRenderer == NULL) {
    printf("SDL_CreateRenderer failed: %s\n", SDL_GetError());
    return 1;
  }
  sdlScreenTexture = SDL_CreateTexture(sdlRenderer,
				 SDL_PIXELFORMAT_ARGB8888,
				 SDL_TEXTUREACCESS_TARGET,
				 width, height);
  if (sdlScreenTexture == NULL) {
    printf("SDL_CreateTexture failed : %s\n",SDL_GetError());
    return -1;
  }

  SDL_SetRenderDrawBlendMode(sdlRenderer, SDL_BLENDMODE_BLEND);
  SDL_SetRenderTarget(sdlRenderer, sdlScreenTexture);
  SDL_SetRenderDrawColor(sdlRenderer, 0, 0, 0, 255);
  SDL_RenderClear(sdlRenderer);
  SDL_SetRenderTarget(sdlRenderer, NULL);
  SDL_RenderClear(sdlRenderer);
  SDL_RenderPresent(sdlRenderer);
  return 0;
} // displayInit(caption, width, height)

/** Displays the persistent screen on-screen.
 */
void displayPersistentScreen() {
  SDL_RenderClear(sdlRenderer);
  SDL_RenderCopy(sdlRenderer, sdlScreenTexture, NULL, NULL);
  SDL_RenderPresent(sdlRenderer);
}

/** Checks for SDL events (non-blocking)
 * @return true iff the application should quit
 */
_Bool checkEvents() {
  SDL_Event e;
  while (SDL_PollEvent(&e)){
    switch (e.type) {
    case SDL_QUIT:
        return true;
    }
  } // while more events
  return false;
}

/**
 * closes the window and terminates the SDL system
 */
void displayQuit() {
  if (sdlScreenTexture != NULL) {
    SDL_DestroyTexture(sdlScreenTexture);
    sdlScreenTexture = NULL;
  }
  if (sdlTexture != NULL) {
    SDL_DestroyTexture(sdlTexture);
    sdlTexture = NULL;
  }
  if (sdlRenderer != NULL) {
    SDL_DestroyRenderer(sdlRenderer);
    sdlRenderer = NULL;
  }
  if (sdlWindow != NULL) {
    SDL_DestroyWindow(sdlWindow);
    sdlWindow = NULL;
  }
  SDL_Quit();
} //displayQuit()

/** Waits for the specified time.
 * @param milliseconds the time to wait. 1000 is 1 second.
 */
void delay(int milliseconds) {
  SDL_Delay(milliseconds);
} // delay(int)

/** Displays the specified pixels at the specified location of the window.
 * @param pixels a linear array of ARGB pixels (2x2 pixels => B00,G00,R00,A00,B01,G01,R01,B10,G10,R10,A10,B11,G11,R11)
 * @param x the left-coordinate of the top-left corner of the rectangle to fill with pixels
 * @param y the top-coordinate of the top-left corner of the rectangle to fill with pixels
 * @param with the width of the rectangle to fill with pixels
 * @param height the height of the rectangle to fill with pixels
 * @param scale the scale to apply to the pixels during drawing. 2 is twice larger, 0.25 is quarter.
 */
void displayPixels(void *pixels, int x, int y, int width, int height, float scale, _Bool persistent) {
  SDL_Surface *surf = SDL_CreateRGBSurfaceFrom(pixels,width,height,32,4*width,0x00FF0000,0x0000FF00,0x000000FF,0xFF000000); // standard masks for ARGB
  if (surf == NULL)
    printf("SDL_CreateSurfaceFrom failed : %s\n",SDL_GetError());
  else {
    SDL_Texture *texture = SDL_CreateTextureFromSurface(sdlRenderer, surf);
    if (texture == NULL)
      printf("SDL_CreateTextureFromSurface failed : %s\n",SDL_GetError());
    else {
      SDL_Rect dest = {x,y,(int)(width*scale+0.5),(int)(height*scale+0.5)}; // Round pixels to the nearest
      if (persistent) {
	if (SDL_SetRenderTarget(sdlRenderer, sdlScreenTexture) != 0)
	  printf("SDL_SetRenderTarget failed : %s\n", SDL_GetError());
      } else {
	SDL_SetRenderDrawColor(sdlRenderer, 0, 0, 0, 255);
	SDL_RenderClear(sdlRenderer);
	SDL_RenderCopy(sdlRenderer, sdlScreenTexture, NULL, NULL);
      }
      if (SDL_SetTextureBlendMode(texture, SDL_BLENDMODE_BLEND) != 0)
	printf("SDL_SetTextureBlendMode failed : %s\n",SDL_GetError());
      if (SDL_RenderCopy(sdlRenderer, texture, NULL, &dest) != 0)
	printf("SDL_RenderCopy failed : %s\n",SDL_GetError());
      else {
	if (persistent) {
	  if (SDL_SetRenderTarget(sdlRenderer, NULL) != 0)
	    printf("SDL_SetRenderTarget failed : %s\n", SDL_GetError());
	  SDL_RenderCopy(sdlRenderer, sdlScreenTexture, NULL, NULL);
	}
	SDL_RenderPresent(sdlRenderer);
      }
      SDL_DestroyTexture(texture);
    } // if surf created

    SDL_FreeSurface(surf);
  } // if surf created
} // displayPixels(void*, int, int, int, int)

/** Prepares a texture of the requested size.
 * @param width the texture width
 * @param height the texture height
 * @return 0 if Ok ; -1 if error
 */
int displayPrepareTexture(int width, int height) {
  sdlTexture = SDL_CreateTexture(sdlRenderer,
				 SDL_PIXELFORMAT_ARGB8888,
				 SDL_TEXTUREACCESS_STREAMING,
				 width, height);
  if (sdlTexture == NULL) {
    printf("SDL_CreateTexture failed : %s\n",SDL_GetError());
    return -1;
  }
  if (SDL_SetTextureBlendMode(sdlTexture, SDL_BLENDMODE_BLEND) != 0)
    printf("SDL_SetTextureBlendMode failed : %s\n",SDL_GetError());
  return 0;
}

/** Updates the texture with specified pixels and put it on the window.
 * @param pixels [in] the pixels to put into the texture
 * @param x the left-coordinate of the top-left corner of the window rectangle to fill with pixels
 * @param y the top-coordinate of the top-left corner of the window rectangle to fill with pixels
 * @param width the width of the texture
 * @param height the height of the texture
 * @param scale the scale to apply to the pixels during drawing. 2 is twice larger, 0.25 is quarter.
 */
void displayUpdateAndDisplayTexture(void *pixels, int x, int y, int width, int height, float scale, _Bool persistent) {
  SDL_UpdateTexture(sdlTexture, NULL, pixels, width * sizeof (Uint32));
  if (persistent) {
    if (SDL_SetRenderTarget(sdlRenderer, sdlScreenTexture)!=0)
      printf("SDL_SetRenderTarget failed : %s\n", SDL_GetError());
  } else {
    SDL_SetRenderDrawColor(sdlRenderer, 0, 0, 0, 255);
    SDL_RenderClear(sdlRenderer);
    SDL_RenderCopy(sdlRenderer, sdlScreenTexture, NULL, NULL);
  }
  SDL_Rect dest = {x,y,(int)(width*scale+0.5),(int)(height*scale+0.5)}; // Round
  SDL_RenderCopy(sdlRenderer, sdlTexture, NULL, &dest);
  if (persistent) {
    if (SDL_SetRenderTarget(sdlRenderer, NULL) != 0)
      printf("SDL_SetRenderTarget failed : %s\n", SDL_GetError());
    SDL_RenderCopy(sdlRenderer, sdlScreenTexture, NULL, NULL);
  }
  SDL_RenderPresent(sdlRenderer);
}


/** Draw a line on the persistent screen
 * @param x1 the x-coordinate of the first terminal
 * @param y1 the y-coordinate of the first terminal
 * @param x2 the x-coordinate of the last terminal
 * @param y2 the y-coordinate of the last terminal
 * @param couleur qui contient les composantes ARGB
 */
void displayDrawLine(int x1, int y1, int x2, int y2, Argb couleur) {
  if (SDL_SetRenderTarget(sdlRenderer, sdlScreenTexture) != 0)
    printf("SDL_SetRenderTarget failed : %s\n", SDL_GetError());
  if (SDL_SetRenderDrawColor(sdlRenderer, couleur.r, couleur.g, couleur.b, couleur.a) != 0) {
    printf("SDL_SetRenderDrawColor failed : %s\n",SDL_GetError());
  } else {
    if (SDL_RenderDrawLine(sdlRenderer, x1, y1, x2, y2) != 0)
      printf("SDL_RenderDrawLine failed : %s\n",SDL_GetError());
  }
  if (SDL_SetRenderTarget(sdlRenderer, NULL) != 0)
    printf("SDL_SetRenderTarget failed : %s\n", SDL_GetError());
} // displayDrawLine(int, int, int, int,int)

/** Draw a rectangle on the persistent screen
 * @param x the x-coordinate of the top-left corner
 * @param y the y-coordinate of the top-left corner
 * @param width the width of the rectangle
 * @param height the height of the rectangle
 * @param couleur les composantes a r g b
 * @param remplit true if the rectangle is to be filled
 */
void displayDrawRect(int x, int y, int width, int height, Argb couleur, _Bool remplit) {
  if (SDL_SetRenderTarget(sdlRenderer, sdlScreenTexture) != 0)
    printf("SDL_SetRenderTarget failed : %s\n", SDL_GetError());
  if (SDL_SetRenderDrawColor(sdlRenderer, couleur.r, couleur.g, couleur.b, couleur.a) != 0) {
    printf("SDL_SetRenderDrawColor failed : %s\n",SDL_GetError());
  } else {
    struct SDL_Rect rect = {x,y,width,height};
    if (remplit) {
      if (SDL_RenderFillRect(sdlRenderer, &rect) != 0)
	printf("SDL_RenderFillRect failed : %s\n",SDL_GetError());
    } else {
      if (SDL_RenderDrawRect(sdlRenderer, &rect) != 0)
	printf("SDL_RenderDrawRect failed : %s\n",SDL_GetError());
    }
  }
  if (SDL_SetRenderTarget(sdlRenderer, NULL) != 0)
    printf("SDL_SetRenderTarget failed : %s\n", SDL_GetError());
} // displayDrawLine(int, int, int, int,int)


