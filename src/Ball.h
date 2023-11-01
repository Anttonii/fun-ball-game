#ifndef BALL_H
#define BALL_H

#include <vector>
#include <iostream>
#include <memory>

#include "Utils.h"

#include "SDL.h"
#include "box2d.h"

#define BASE_RADIUS 25.0f
#define BASE_DENSITY 0.3f

// the radius multiplier applied when balls change color
static const double RADIUS_MULTIPLIER = 1.26;

// the density multiplier applied when balls change color
static const double DENSITY_MULTIPLIER = 1.14;

// total amount of different ball sizes
static const int TOTAL_TYPES = 10;

// determines the color and size of a ball
enum BallType : unsigned int
{
    WHITE  = 0,
    RED    = 1,
    VIOLET = 2,
    ORANGE = 3,
    YELLOW = 4,
    GREEN  = 5,
    LIME   = 6,
    PINK   = 7,
    BLUE   = 8,
    BLACK  = 9
};

static const char* ballTypeToString[TOTAL_TYPES] = {
    "white",
    "red",
    "violet",
    "orange",
    "yellow",
    "green",
    "lime",
    "pink",
    "blue",
    "black"
};

static float ballTypeToRadius[TOTAL_TYPES] = {
    BASE_RADIUS,
    BASE_RADIUS * (float) pow(RADIUS_MULTIPLIER, 1.0),
    BASE_RADIUS * (float) pow(RADIUS_MULTIPLIER, 2.0),
    BASE_RADIUS * (float) pow(RADIUS_MULTIPLIER, 3.0),
    BASE_RADIUS * (float) pow(RADIUS_MULTIPLIER, 4.0),
    BASE_RADIUS * (float) pow(RADIUS_MULTIPLIER, 5.0),
    BASE_RADIUS * (float) pow(RADIUS_MULTIPLIER, 6.0),
    BASE_RADIUS * (float) pow(RADIUS_MULTIPLIER, 7.0),
    BASE_RADIUS * (float) pow(RADIUS_MULTIPLIER, 8.0),
    BASE_RADIUS * (float) pow(RADIUS_MULTIPLIER, 9.0),
};

static float ballTypeToDensity[TOTAL_TYPES] = {
    BASE_DENSITY,
    BASE_DENSITY * (float) pow(DENSITY_MULTIPLIER, 1.0),
    BASE_DENSITY * (float) pow(DENSITY_MULTIPLIER, 2.0),
    BASE_DENSITY * (float) pow(DENSITY_MULTIPLIER, 3.0),
    BASE_DENSITY * (float) pow(DENSITY_MULTIPLIER, 4.0),
    BASE_DENSITY * (float) pow(DENSITY_MULTIPLIER, 5.0),
    BASE_DENSITY * (float) pow(DENSITY_MULTIPLIER, 6.0),
    BASE_DENSITY * (float) pow(DENSITY_MULTIPLIER, 7.0),
    BASE_DENSITY * (float) pow(DENSITY_MULTIPLIER, 8.0),
    BASE_DENSITY * (float) pow(DENSITY_MULTIPLIER, 9.0),
};

// user data to be passed onto box2d objects
struct UserDataFlags
{
    // whether or not the body should remain to be updated
    bool isAlive {};
    // whether or not the body has collided once
    bool hasCollided {};
    // the radius of the body
    float radius {};
};

class Ball
{
    public:
        Ball(float _x, float _y, BallType type, b2World * world, b2Vec2 initialVelocity);
        ~Ball();

        inline BallType getType() noexcept { return _type; };
        inline float getRadius() noexcept { return _radius; };
        inline b2Body * getBody() noexcept { return _body; };
        inline float getDensity() noexcept { return ballTypeToDensity[_type]; };
        
        // Applys an impulse to the balls center of mass
        void applyImpulse(b2Vec2 direction) noexcept;
        
        float x, y;
        float angle;
    private:
        float _radius;
        // determines whether or not the object is handled by the physics engine
        bool _suspended;
        // x and y coordinates which determine the center point of the ball
        // the type of the ball
        BallType _type;

        // Body definition for a ball
        b2BodyDef _bodyDef;
        // Initialized in constructor
        b2Body * _body;
        // Keep track of the world the ball is in.
        b2World * _world;

        std::unique_ptr<UserDataFlags> _userData;
};

#endif