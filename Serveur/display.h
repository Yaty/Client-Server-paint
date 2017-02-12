#ifndef __DISPLAY__
#define __DISPLAY__

#include <SDL2/SDL.h>
#include <stdbool.h>

typedef struct ARGB {
    int r;
    int g;
    int b;
    int a;
} Argb;

typedef struct lignes {
    int x1;
    int y1;
    int x2;
    int y2;
    Argb couleur;
} Ligne;

typedef struct rectangles {
    int x;
    int y;
    int largeur;
    int hauteur;
    Argb couleur;
    _Bool remplit;
} Rect;

/** Opens a window with a title and a size.
 * @param caption the title of the window
 * @param width the width of the window
 * @param height the height of the window
 * @return 0 if ok, 1 if error
 */
int displayInit(const char* caption, int width, int height);

/** Displays the persistent screen on-screen.
 */
void displayPersistentScreen();

/** Checks for SDL events (non-blocking)
 * @return true si la fenetre est fermée
 */
_Bool checkEvents();

/** Waits for the specified time.
 * @param milliseconds the time to wait. 1000 is 1 second.
 */
void delay(int milliseconds);

/** Displays the specified pixels at the specified location of the window.
 * @param pixels [in] a linear array of ARGB pixels (2x2 pixels => B00,G00,R00,A00,B01,G01,R01,B10,G10,R10,A10,B11,G11,R11)
 * @param x the left-coordinate of the top-left corner of the rectangle to fill with pixels
 * @param y the top-coordinate of the top-left corner of the rectangle to fill with pixels
 * @param with the width of the rectangle to fill with pixels
 * @param height the height of the rectangle to fill with pixels
 * @param scale the scale to apply to the pixels during drawing. 2 is twice larger, 0.25 is quarter.
 */
void displayPixels(void *pixels, int x, int y, int width, int height,float scale,_Bool persistent);

/** Prepares a texture of the requested size.
 * @param width the texture width
 * @param height the texture height
 * @return 0 if Ok ; -1 if error
 */
int displayPrepareTexture(int width, int height) ;

/** Updates the texture with specified pixels and put it on the window.
 * @param pixels [in] the pixels to put into the texture
 * @param x the left-coordinate of the top-left corner of the window rectangle to fill with pixels
 * @param y the top-coordinate of the top-left corner of the window rectangle to fill with pixels
 * @param width the width of the texture
 * @param height the height of the texture
 * @param scale the scale to apply to the pixels during drawing. 2 is twice larger, 0.25 is quarter.
 */
void displayUpdateAndDisplayTexture(void *pixels, int x, int y, int width, int height, float scale,_Bool persistent);

/** Draw a line on the screen
 * @param x1 the x-coordinate of the first terminal
 * @param y1 the y-coordinate of the first terminal
 * @param x2 the x-coordinate of the last terminal
 * @param y2 the y-coordinate of the last terminal
 * @param couleur composantes argb
 */
void displayDrawLine(int x1, int y1, int x2, int y2, Argb couleur);

/** Draw a rectangle on the persistent screen
 * @param x the x-coordinate of the top-left corner
 * @param y the y-coordinate of the top-left corner
 * @param width the width of the rectangle
 * @param height the height of the rectangle
 * @param a the color's alpha  component
 * @param r the color's red component
 * @param g the color's green component
 * @param b the color's blue component
 * @param fill true iff the rectangle is to be filled
 */
void displayDrawRect(int x, int y, int width, int height, Argb couleur, _Bool remplit);

/**
 * closes the window and terminates the SDL system
 */
void displayQuit();

#endif
