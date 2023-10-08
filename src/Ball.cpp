#include "Ball.h"

Ball::Ball(float _x, float _y, BallType type, b2World * world, b2Vec2 initialVelocity) : x(_x), y(_y), _type(type), 
    _radius(ballTypeToRadius[type]), _suspended(true), _world(world), angle(0.0f)
{
    _userData = std::make_unique<UserDataFlags>();
    _userData->isAlive = true;

    _bodyDef.type = b2_dynamicBody;
    _bodyDef.position.Set(x / PIXEL_CONVERSION, y / PIXEL_CONVERSION);
    _body = _world->CreateBody(&_bodyDef);
    _body->GetUserData().pointer = reinterpret_cast<uintptr_t>(_userData.get());

    // TODO: Fix this force application
    _body->ApplyLinearImpulseToCenter(b2Vec2(initialVelocity.x * 10.0f, initialVelocity.y * 10.0f), true);

    b2CircleShape circleShape;
    circleShape.m_radius = _radius / PIXEL_CONVERSION;

    b2FixtureDef fixtureDef;
    fixtureDef.shape = &circleShape;
    fixtureDef.density = ballTypeToDensity[type];
    fixtureDef.friction = 0.2f;
    _body->CreateFixture(&fixtureDef);
}

Ball::~Ball()
{
    _world->DestroyBody(_body); 
}