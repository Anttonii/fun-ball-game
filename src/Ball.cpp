#include "Ball.h"

Ball::Ball(float _x, float _y, BallType type, b2World *world, b2Vec2 _initialVelocity) : x(_x), y(_y), _type(type),
                                                                                         _radius(ballTypeToRadius[type]), _suspended(true), _world(world), angle(0.0f), initialVelocity(_initialVelocity)
{
    _userData = std::make_unique<UserDataFlags>();
    _userData->isAlive = true;
    _userData->radius = _radius / PIXEL_CONVERSION;
}

Ball::~Ball()
{
    _world->DestroyBody(_body);
}

void Ball::attachBody() noexcept
{
    _bodyDef.type = b2_dynamicBody;
    _bodyDef.position.Set(x / PIXEL_CONVERSION, y / PIXEL_CONVERSION);
    _body = _world->CreateBody(&_bodyDef);
    _body->GetUserData().pointer = reinterpret_cast<uintptr_t>(_userData.get());

    b2CircleShape circleShape;
    circleShape.m_radius = _radius / PIXEL_CONVERSION;

    b2FixtureDef fixtureDef;
    fixtureDef.shape = &circleShape;
    fixtureDef.density = BASE_DENSITY;
    fixtureDef.friction = 0.2f;
    _body->CreateFixture(&fixtureDef);

    applyImpulse(initialVelocity);
}

BallContactListener::BallContactListener(Game *_game, b2World *_world) : game(_game), world(_world)
{
    world->SetContactListener(this);
}

BallContactListener::~BallContactListener()
{
}

void BallContactListener::applyForce(float x, float y, float radius) noexcept
{
    std::vector<Ball *> balls = game->getBalls();
    for (std::vector<Ball *>::const_iterator it = balls.begin(); it != balls.end(); it++)
    {
        UserDataFlags *userData = (UserDataFlags *)(*it)->getBody()->GetUserData().pointer;
        if (!userData->isAlive)
            return;

        float x2 = (*it)->x;
        float y2 = (*it)->y;
        float r2 = (*it)->getRadius();

        if (circlesOverlap(x, y, radius + 3, x2, y2, r2))
        {
            float slope = (y2 - y) / (x2 - x);
            float angle = atan(slope);

            b2Vec2 impulse(cos(angle), -sin(angle));
            impulse *= std::min(3.0f, (3.0f / abs(impulse.x + impulse.y)));
            (*it)->applyImpulse(impulse);
        }
    }
}

void BallContactListener::BeginContact(b2Contact *contact)
{
    b2WorldManifold worldManifold;
    contact->GetWorldManifold(&worldManifold);

    b2Body *bodyA = contact->GetFixtureA()->GetBody();
    b2Body *bodyB = contact->GetFixtureB()->GetBody();

    b2Vec2 point = worldManifold.points[0];

    Ball *ballA = game->getBallFromBody(bodyA);
    Ball *ballB = game->getBallFromBody(bodyB);

    if (ballA == NULL || ballB == NULL)
        return;

    const BallType btA = ballA->getType();
    const BallType btB = ballB->getType();

    UserDataFlags *userDataA = (UserDataFlags *)bodyA->GetUserData().pointer;
    UserDataFlags *userDataB = (UserDataFlags *)bodyB->GetUserData().pointer;

    if (!userDataA->hasCollided)
        userDataA->hasCollided = true;
    if (!userDataB->hasCollided)
        userDataB->hasCollided = true;

    if (btA == btB && btA != BallType::BLACK && userDataA->isAlive && userDataB->isAlive)
    {
        b2Vec2 velocitySum = bodyA->GetLinearVelocity() + bodyB->GetLinearVelocity();
        velocitySum *= 0.25f;

        float x = point.x * PIXEL_CONVERSION;
        float y = point.y * PIXEL_CONVERSION;
        float radius = ballTypeToRadius[btA + 1];

        if (y + radius > BOTTOM_EDGE_Y)
            y -= y + radius - BOTTOM_EDGE_Y;

        if (x + radius > RIGHT_BORDER_X)
            x -= x + radius - RIGHT_BORDER_X;
        else if (x - radius < LEFT_BORDER_X)
            x += LEFT_BORDER_X - (x - radius);

        userDataA->isAlive = false;
        userDataB->isAlive = false;

        applyForce(x, y, radius);
        game->addBall(new Ball(x, y, static_cast<BallType>(btA + 1), world, velocitySum));
        game->playPop();

        // add the type of the ball times twice and for both balls to the score
        game->addScore((btA + 1) * 2 * 2);
        game->updateScoreText();
    }
}
