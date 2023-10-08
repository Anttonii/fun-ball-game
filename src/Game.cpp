#include "Game.h"

Game::Game(SDL_Renderer * renderer, bool * onClose) : _world(_gravity), _groundBody(), _balls(), 
    _onClose(onClose), _renderer(renderer), _paused(true)
{
    _groundBody.position.x = 0;
    _groundBody.position.y = -10.0;
    b2Body * groundBody = _world.CreateBody(&_groundBody);
}

Game::~Game() 
{
}

bool Game::init()
{
    ballTextures = loadAllBallTextures(_renderer);
    if (!ballTextures)
    {
        std::cout << "Unable to initialize." << std::endl;
        return false;
    }

    // create and register contact listener
    _contactListener = new BallContactListener(this, &_world);
    _paused = false;
    
    // generate a seed
    srand(time(NULL));

    return true;
}

void Game::startGame() noexcept
{
    // start every game with 2 white balls
    currentBall = BallType::WHITE;
    nextBall = BallType::WHITE;

    // Add edges of the playing board to the screen
    addEdge(LEFT_BORDER_X, LEFT_BORDER_Y, 0.0f, GAME_BOX_HEIGHT);  // left edge
    addEdge(RIGHT_BORDER_X, RIGHT_BORDER_Y, 0.0f, GAME_BOX_HEIGHT); // right edge
    addEdge(BOTTOM_EDGE_X, BOTTOM_EDGE_Y, GAME_BOX_WIDTH, 0.0f); // bottom edge
    
    scoreTextObject.createText(_renderer, 36, SCORE_TEXT_X, SCORE_TEXT_Y, "Score: ", "./assets/fonts/IBM_VGA.ttf");
    nextBallTextObject.createText(_renderer, 36, NEXT_BALL_TEXT_X, NEXT_BALL_TEXT_Y, "Next:", "./assets/fonts/IBM_VGA.ttf");
    updateScoreText();
}

/// @brief Handles tick updates, keeps rendering and physics in sync.
void Game::tick() noexcept
{
    pollEvents();
    if(!_paused)
    {
        _world.Step(TIME_STEP, 8, 3);

        cleanUp();
        
        for(int i = 0; i < _ballsToAdd.size(); i++)
        {
            float x = _ballsToAdd[i].x;
            float y = _ballsToAdd[i].y;
            int ballType = _ballsToAdd[i].ballType;
            addBall(x, y, static_cast<BallType>(ballType));
        }
        _ballsToAdd.clear();

        for(int i = 0; i < _balls.size(); i++)
        {
            auto body = _balls[i]->getBody();
            _balls[i]->x += body->GetPosition().x * PIXEL_CONVERSION - _balls[i]->x;
            _balls[i]->y += body->GetPosition().y * PIXEL_CONVERSION - _balls[i]->y;
            _balls[i]->angle += body->GetAngularVelocity() * TIME_STEP * PIXEL_CONVERSION;
        }
    }
    render();

    SDL_Delay(1000 / 60);
}

/// @brief Renders game objects to the screen.
void Game::render() noexcept
{
    currentTime = SDL_GetTicks();

    SDL_RenderClear(_renderer);
    for(int i = 0; i < _edges.size(); i++)
    {
        int x = (int) _edges[i]->x;
        int y = (int) _edges[i]->y;
        int width = (int) _edges[i]->width;
        int height = (int) _edges[i]->height;

        // Draw lines in white
        SDL_SetRenderDrawColor(_renderer, 255, 255, 255, 255);
        SDL_RenderDrawLine(_renderer, x, y, x + width, y + height);
        SDL_SetRenderDrawColor(_renderer, 0, 0, 0, 255);
    }

    // render dropper ball
    // dropping is allowed only every 3 second, 
    if (currentTime - lastDrop >= DROP_DELAY && !_paused)
    {
        int radius = (int) ballTypeToRadius[currentBall];
        int diameter = radius * 2;
        int x = dropperX - radius;
        int y = dropperY - radius;
        SDL_Rect quad = { x, y, diameter, diameter };
        SDL_RenderCopyEx(_renderer,  ballTextures[currentBall].inner, NULL, &quad, 0.0, NULL, SDL_FLIP_NONE);
    }

    int nextBallRadius = (int) ballTypeToRadius[nextBall];
    int nextBallDiameter = nextBallRadius * 2;
    SDL_Rect nextBallQuad = { 670 - (int) ballTypeToRadius[nextBall], 160 - (int) ballTypeToRadius[nextBall],
        nextBallDiameter, nextBallDiameter };
    SDL_RenderCopyEx(_renderer, ballTextures[nextBall].inner, NULL, &nextBallQuad, 0.0, NULL, SDL_FLIP_NONE);
    
    SDL_SetRenderDrawColor(_renderer, 255, 255, 255, 255);
    SDL_RenderDrawLine(_renderer, 620, 110, 720, 110);
    SDL_RenderDrawLine(_renderer, 620, 110, 620, 210);
    SDL_RenderDrawLine(_renderer, 620, 210, 720, 210);
    SDL_RenderDrawLine(_renderer, 720, 110, 720, 210);
    SDL_SetRenderDrawColor(_renderer, 0, 0, 0, 255);
    
    for(int i = 0; i < _balls.size(); i++)
    {
        float radius = _balls[i]->getRadius();
        int diameter = (int) radius * 2.0f;
        int x = (int) _balls[i]->x - radius;
        int y = (int) _balls[i]->y - radius;
        double angle = (double) _balls[i]->angle;
        BallType type = _balls[i]->getType();
        SDL_Rect quad = { x, y, diameter, diameter };
        SDL_RenderCopyEx(_renderer, ballTextures[type].inner, NULL, &quad, angle, NULL, SDL_FLIP_NONE);
    }

    scoreTextObject.renderText();
    nextBallTextObject.renderText();

    SDL_RenderPresent(_renderer);
}

void Game::cleanUp() noexcept
{
    std::vector<b2Body *> bodiesToRemove;
    b2Body * body = _world.GetBodyList();

    for( ; body; body = body->GetNext())
    {
        uintptr_t userData = body->GetUserData().pointer;
        if(userData != 0)
        {
            UserDataFlags * flags = (UserDataFlags *) body->GetUserData().pointer;
            if(!flags->isAlive)
                bodiesToRemove.push_back(body);
        }
    };

    std::sort(bodiesToRemove.begin(), bodiesToRemove.end());
    unsigned int i = 0;
    unsigned int size = bodiesToRemove.size();
    while(i < size) 
    {
        b2Body * b = bodiesToRemove[i++];
        while(i < size && bodiesToRemove[i] == b) ++i;
        removeBall(b);
        b = 0;
    }

    bodiesToRemove.clear();
}

void Game::dropBall() noexcept
{
    BallType generated = static_cast<BallType>(rand() % 3);
    lastDrop = SDL_GetTicks();

    // dropperX is defined as clamp(mouseX, LEFT_BORDER_X, RIGHT_BORDER_X)
    addBall((float) dropperX, (float) dropperY, currentBall);

    currentBall = nextBall;
    nextBall = generated;
}

void Game::addBall(Ball * ball)
{
    std::unique_ptr<Ball> b(ball);
    _balls.push_back(std::move(b));
    b = nullptr;
}

void Game::addBall(float x, float y, BallType type) noexcept
{
    addBall(x, y, type, b2Vec2_zero);
}

void Game::addBall(float x, float y, BallType type, b2Vec2 initialVelocity) noexcept
{
    std::unique_ptr<Ball> b = std::make_unique<Ball>(x, y, type, &_world, initialVelocity);
    _balls.push_back(std::move(b));
    b = nullptr;
}

void Game::addEdge(float x, float y, float width, float height) noexcept
{
    float hWidth = width * 0.5f;
    float hHeight = height * 0.5f;

    b2BodyDef bodyDef;
    bodyDef.type = b2_staticBody;
    bodyDef.position.Set((x + hWidth) / PIXEL_CONVERSION, (y + hHeight) / PIXEL_CONVERSION);
    b2Body * body = _world.CreateBody(&bodyDef);
    b2EdgeShape shape;

    const b2Vec2 p1 = b2Vec2(-hWidth / PIXEL_CONVERSION, -hHeight / PIXEL_CONVERSION);
    const b2Vec2 p2 = b2Vec2(hWidth / PIXEL_CONVERSION, hHeight / PIXEL_CONVERSION);
    shape.SetTwoSided(p1,p2);

    b2FixtureDef fixture;
    fixture.shape = &shape;
    fixture.friction = 1.0f;
    body->CreateFixture(&fixture);

    Edge edge { x, y, width, height, body, bodyDef, &shape, &fixture };
    std::unique_ptr<Edge> e = std::make_unique<Edge>(edge);
    _edges.push_back(std::move(e));
    e = nullptr;
}

const std::unique_ptr<Ball>& Game::getBallFromBody(b2Body * body) noexcept
{
    for(int i = 0; i < _balls.size(); i++)
    {
        if(_balls[i]->getBody() == body)
        {
            return _balls[i];
        }
    }

    return nullptr;
}

bool Game::removeBall(b2Body * body) noexcept
{
    for(int i = 0; i < _balls.size(); i++)
    {
        if(_balls[i]->getBody() == body)
        {
            _balls.erase(_balls.begin() + i);
            return true;
        }
    }
    return false;
}

void Game::addBallToQueue(float x, float y, int ballType, b2Vec2 initialVelocity) noexcept
{
    BallQueueData bqd;
    bqd.x = x;
    bqd.y = y;
    bqd.ballType = ballType;
    bqd.initialVelocity = initialVelocity;

    _ballsToAdd.push_back(bqd);
}

void Game::updateScoreText()
{
    auto format = "Score: %i";
    auto size = std::snprintf(nullptr, 0, format, score);
    std::string output(size + 1, '\0');
    std::sprintf(&output[0], format, score);
    scoreText = output;
    scoreTextObject.updateText(scoreText);
}

void Game::pollEvents() noexcept
{
    SDL_Event event;
    while (SDL_PollEvent(&event)) 
    {
        // update mouse position on mouse updates
        if (event.type == SDL_MOUSEMOTION || event.type == SDL_MOUSEBUTTONDOWN || event.type == SDL_MOUSEBUTTONUP)
        {
            SDL_GetMouseState(&mouseX, &mouseY);
            dropperX = clamp(mouseX, (int) (LEFT_BORDER_X + ballTypeToRadius[currentBall]), (int) (RIGHT_BORDER_X - ballTypeToRadius[currentBall]));
        }

        switch (event.type) {
            
        case SDL_QUIT:
            *_onClose = true;
            break;

        case SDL_KEYDOWN:
            switch (event.key.keysym.scancode) {
            case SDL_SCANCODE_ESCAPE:
                *_onClose = true;
                break;
            case SDL_SCANCODE_P:
                if (!_paused)
                    pauseDifference = currentTime - lastDrop;
                else
                    lastDrop = currentTime - pauseDifference;
                
                _paused = !_paused;
                break;
            /*case SDL_SCANCODE_KP_ENTER:
                SDL_CaptureMouse(SDL_FALSE);
                break;*/
            default:
                break;
            }

        case SDL_MOUSEBUTTONDOWN:
            switch(event.button.button)
            {
            case SDL_BUTTON_LEFT:
                if(currentTime - lastDrop >= DROP_DELAY && !_paused)
                    dropBall();
                break;
            }
        }

    }
}

BallContactListener::BallContactListener(Game * game, b2World * world) : _game(game), _world(world)
{
    world->SetContactListener(this);
}

BallContactListener::~BallContactListener()
{
}

void BallContactListener::BeginContact(b2Contact* contact)
{
    b2WorldManifold worldManifold;
    contact->GetWorldManifold(&worldManifold);
    
    b2Body* bodyA = contact->GetFixtureA()->GetBody();
    b2Body* bodyB = contact->GetFixtureB()->GetBody();

    b2Vec2 point = worldManifold.points[0];

    const auto bodyARef = &_game->getBallFromBody(bodyA);
    const auto bodyBRef = &_game->getBallFromBody(bodyB);

    if(bodyARef != nullptr && bodyBRef != nullptr) 
    {
        const BallType btA = bodyARef->get()->getType();
        const BallType btB = bodyBRef->get()->getType();

        UserDataFlags * userDataA = (UserDataFlags*) bodyA->GetUserData().pointer;
        UserDataFlags * userDataB = (UserDataFlags*) bodyB->GetUserData().pointer;

        if (btA == btB && btA != BallType::BLACK && userDataA->isAlive && userDataB->isAlive) 
        {
            b2Vec2 velocitySum = bodyA->GetLinearVelocity() + bodyB->GetLinearVelocity();

            float x = point.x * PIXEL_CONVERSION;
            float y = point.y * PIXEL_CONVERSION;

            if (y + ballTypeToRadius[btA + 1] > BOTTOM_EDGE_Y)
                y -= y + ballTypeToRadius[btA + 1] - BOTTOM_EDGE_Y;

            userDataA->isAlive = false;
            userDataB->isAlive = false;

            _game->addBallToQueue(x, y, btA + 1, velocitySum);

            // add the type of the ball times twice and for both balls to the score
            _game->score += (btA + 1) * 2 * 2;
            _game->updateScoreText();
        }
    }
}