#ifndef UTILS_H
#define UTILS_H

#include "SDL.h"

#include <math.h>
#include <iostream>

// defines the pixel conversion rate when doing calculations with box2d
// 1 unit in box2d => 15 pixels and vice versa.
#define PIXEL_CONVERSION 50.0f

// time step for each update of the physics engine
static const float TIME_STEP = 1.0f / 60.0f;

// Returns min if val < min, max if val > max and val if min < val < max
static const int clamp(int val, int min, int max)
{
    if(val < min) return min;
    else if (val > max) return max;
    else return val;
}

static bool inline isInBounds(int x, int y, SDL_Rect rect)
{
    return x >= rect.x && x <= rect.x + rect.w &&
            y >= rect.y && y <= rect.y + rect.h;
};

static void drawRect(SDL_Renderer * renderer, int x, int y, int w, int h, Uint8 r, Uint8 g, Uint8 b, Uint8 a)
{
    Uint8 ored, ogre, oblu, oalp; // the original colors
    SDL_GetRenderDrawColor(renderer, &ored, &ogre, &oblu, &oalp);
    SDL_SetRenderDrawColor(renderer, r, g, b, a);
    SDL_RenderDrawLine(renderer, x, y, x + w, y);
    SDL_RenderDrawLine(renderer, x, y, x, y + h);
    SDL_RenderDrawLine(renderer, x, y + h, x + w, y + h);
    SDL_RenderDrawLine(renderer, x + w, y, x + w, y + h);
    SDL_SetRenderDrawColor(renderer, ored, ogre, oblu, oalp);
}

static bool circlesOverlap(float x, float y, float r, float x2, float y2, float r2)
{
    float dist = (float) sqrt(pow(abs(x-x2), 2) + pow(abs(y-y2), 2));
    return dist <= r + r2;
}

#endif