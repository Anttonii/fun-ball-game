#ifndef UTILS_H__
#define UTILS_H__

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

#endif