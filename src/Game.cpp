#include "Game.h"

Game::Game(Application *_app) : world(gravity), groundBodyDef(), balls(),
                                app(_app), paused(true)
{
    groundBodyDef.position.x = 0;
    groundBodyDef.position.y = -10.0;
    b2Body *groundBody = world.CreateBody(&groundBodyDef);
}

Game::~Game()
{
}

bool Game::init()
{
    loadAllTextures(app->getRenderer());
    if (!textures)
    {
        std::cout << "Unable to initialize game, failed to load ball textures." << std::endl;
        return false;
    }

    // create and register contact listener
    contactListener = new BallContactListener(this, &world);
    paused = false;

    // start every game with 2 white balls and one red ball
    currentBall = BallType::WHITE;
    nextBall = BallType::WHITE;

    // Add edges of the playing board to the screen
    addEdge(LEFT_BORDER_X, LEFT_BORDER_Y, 0.0f, GAME_BOX_HEIGHT);   // left edge
    addEdge(RIGHT_BORDER_X, RIGHT_BORDER_Y, 0.0f, GAME_BOX_HEIGHT); // right edge
    addEdge(BOTTOM_EDGE_X, BOTTOM_EDGE_Y, GAME_BOX_WIDTH, 0.0f);    // bottom edge

    scoreTextObject.createText(app->getRenderer(), 36, SCORE_TEXT_X, SCORE_TEXT_Y, "Score: ", SCOREFONT_PATH);
    nextBallTextObject.createText(app->getRenderer(), 36, NEXT_BALL_TEXT_X, NEXT_BALL_TEXT_Y, "Next:", SCOREFONT_PATH);
    pauseTextObject.createText(app->getRenderer(), 36, 0, 0, "Paused", SCOREFONT_PATH);
    pauseTextObject.setPositionCenter(app->getWidth() / 2, app->getHeight() / 2 - 200);
    updateScoreText();

    popAudio = new Audio();
    popAudio->init("pop.wav");

    // generate a seed
    srand(time(NULL));

    app->show();
    setMouseState(true);

    return true;
}

void Game::restartGame() noexcept
{
    currentBall = BallType::WHITE;
    nextBall = BallType::WHITE;
    lastDrop = -DROP_DELAY;

    for (int i = 0; i < balls.size(); i++)
        delete balls[i];

    balls.clear();

    score = 0;
    updateScoreText();
}

/// @brief Handles tick updates, keeps rendering and physics in sync.
void Game::update()
{
    if (paused)
        return;

    world.Step(TIME_STEP, 8, 3);

    checkState();
    clearBodies();

    if (app->getCurrentTime() - lastDrop >= DROP_DELAY && !queueUpdated)
        generateNextBall();

    for (int i = 0; i < ballsToAdd.size(); i++)
    {
        balls.push_back(ballsToAdd[i]);
        ballsToAdd[i]->attachBody();
    }

    ballsToAdd.clear();

    for (int i = 0; i < balls.size(); i++)
    {
        auto body = balls[i]->getBody();
        balls[i]->x += body->GetPosition().x * PIXEL_CONVERSION - balls[i]->x;
        balls[i]->y += body->GetPosition().y * PIXEL_CONVERSION - balls[i]->y;
        balls[i]->angle += body->GetAngularVelocity() * TIME_STEP * PIXEL_CONVERSION;
    }
}

/// @brief Renders game objects to the screen.
void Game::render(SDL_Renderer *renderer)
{
    SDL_RenderClear(renderer);

    if (paused)
    {
        renderPauseScreen();
        return;
    }

    for (int i = 0; i < edges.size(); i++)
    {
        int x = (int)edges[i].x;
        int y = (int)edges[i].y;
        int width = (int)edges[i].width;
        int height = (int)edges[i].height;

        // Draw lines in white
        SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
        SDL_RenderDrawLine(renderer, x, y, x + width, y + height);
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    }

    // render dropper ball
    // dropping is allowed only every 3 second,
    if (app->getCurrentTime() - lastDrop >= DROP_DELAY && queueUpdated)
    {
        int radius = (int)ballTypeToRadius[currentBall];
        int diameter = radius * 2;
        int x = dropperX - radius;
        int y = dropperY - radius;
        SDL_Rect quad = {x, y, diameter, diameter};
        SDL_RenderCopyEx(renderer, textures[currentBall].inner, NULL, &quad, 0.0, NULL, SDL_FLIP_NONE);
    }

    int nextBallRadius = (int)ballTypeToRadius[BallType::RED];
    int nextBallDiameter = nextBallRadius * 2;
    SDL_Rect nextBallQuad = {670 - (int)ballTypeToRadius[BallType::RED], 160 - (int)ballTypeToRadius[BallType::RED],
                             nextBallDiameter, nextBallDiameter};
    SDL_RenderCopyEx(renderer, textures[nextBall].inner, NULL, &nextBallQuad, 0.0, NULL, SDL_FLIP_NONE);

    // Draws the square around next ball
    drawRect(renderer, 620, 110, 100, 100, 255, 255, 255, 255);

    for (int i = 0; i < balls.size(); i++)
    {
        float radius = balls[i]->getRadius();
        int diameter = (int)radius * 2.0f;
        int x = (int)balls[i]->x - radius;
        int y = (int)balls[i]->y - radius;
        double angle = (double)balls[i]->angle;
        BallType type = balls[i]->getType();
        SDL_Rect quad = {x, y, diameter, diameter};
        SDL_RenderCopyEx(renderer, textures[type].inner, NULL, &quad, angle, NULL, SDL_FLIP_NONE);
    }

    scoreTextObject.renderText();
    nextBallTextObject.renderText();
}

void Game::cleanUp()
{
    scoreTextObject.destroyText();
    nextBallTextObject.destroyText();
}

void Game::clearBodies() noexcept
{
    std::vector<b2Body *> bodiesToRemove;
    b2Body *body = world.GetBodyList();

    for (; body; body = body->GetNext())
    {
        uintptr_t userData = body->GetUserData().pointer;
        if (userData != 0)
        {
            UserDataFlags *flags = (UserDataFlags *)body->GetUserData().pointer;
            if (!flags->isAlive)
                bodiesToRemove.push_back(body);
        }
    };

    std::sort(bodiesToRemove.begin(), bodiesToRemove.end());
    unsigned int i = 0;
    unsigned int size = bodiesToRemove.size();
    while (i < size)
    {
        b2Body *b = bodiesToRemove[i++];
        while (i < size && bodiesToRemove[i] == b)
            ++i;
        removeBall(b);
        b = 0;
    }

    bodiesToRemove.clear();
}

void Game::dropBall() noexcept
{
    lastDrop = app->getCurrentTime();
    queueUpdated = false;
    addBall(new Ball(dropperX, dropperY, currentBall, &world, b2Vec2_zero));
}

void Game::checkState() noexcept
{
    // iterate over all balls, check if their y-position + radius is higher
    // game boards max y
    b2Body *body = world.GetBodyList();

    for (; body; body = body->GetNext())
    {
        uintptr_t userData = body->GetUserData().pointer;
        if (userData != 0)
        {
            UserDataFlags *flags = (UserDataFlags *)body->GetUserData().pointer;
            float position = (body->GetPosition().y - flags->radius) * PIXEL_CONVERSION;
            if (flags->isAlive && flags->hasCollided && position < GAME_BOX_TOP)
                restartGame();
        }
    };
}

void Game::addBall(Ball *ball)
{
    ballsToAdd.push_back(ball);
}

void Game::addEdge(float x, float y, float width, float height) noexcept
{
    float hWidth = width * 0.5f;
    float hHeight = height * 0.5f;

    b2BodyDef bodyDef;
    bodyDef.type = b2_staticBody;
    bodyDef.position.Set((x + hWidth) / PIXEL_CONVERSION, (y + hHeight) / PIXEL_CONVERSION);
    b2Body *body = world.CreateBody(&bodyDef);
    b2EdgeShape shape;

    const b2Vec2 p1 = b2Vec2(-hWidth / PIXEL_CONVERSION, -hHeight / PIXEL_CONVERSION);
    const b2Vec2 p2 = b2Vec2(hWidth / PIXEL_CONVERSION, hHeight / PIXEL_CONVERSION);
    shape.SetTwoSided(p1, p2);

    b2FixtureDef fixture;
    fixture.shape = &shape;
    fixture.friction = 1.0f;
    body->CreateFixture(&fixture);

    Edge edge{x, y, width, height, body, bodyDef, &shape, &fixture};
    edges.push_back(edge);
}

Ball *Game::getBallFromBody(b2Body *body) noexcept
{
    for (int i = 0; i < balls.size(); i++)
    {
        if (balls[i]->getBody() == body)
        {
            return balls[i];
        }
    }

    return NULL;
}

bool Game::removeBall(b2Body *body) noexcept
{
    for (int i = 0; i < balls.size(); i++)
    {
        if (balls[i]->getBody() == body)
        {
            delete balls[i];
            balls.erase(balls.begin() + i);
            return true;
        }
    }
    return false;
}

void Game::togglePause() noexcept
{
    paused = !paused;

    if (paused)
    {
        setMouseState(false);
        scoreTextObject.setPositionCenter(app->getWidth() / 2, app->getHeight() / 2 - 160);
    }
    else
    {
        setMouseState(true);
        scoreTextObject.setPosition(SCORE_TEXT_X, SCORE_TEXT_Y);
    }
}

void Game::renderPauseScreen() noexcept
{
    scoreTextObject.renderText();
    pauseTextObject.renderText();
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

void Game::setMouseState(bool state) noexcept
{
    app->grabMouse(state);
    app->hideMouse(state);
}

void Game::generateNextBall() noexcept
{
    // Refers to which balls can be generated
    int ballPool = 3;
    if (score >= 750)
        ballPool = 4;
    if (score >= 2000)
        ballPool = 5;

    BallType generated = static_cast<BallType>(rand() % ballPool);
    currentBall = nextBall;
    nextBall = generated;
    queueUpdated = true;
}

void Game::pollEvents()
{
    for (SDL_Event event : app->getFrameEvents())
    {
        switch (event.type)
        {
        case SDL_KEYDOWN:
            switch (event.key.keysym.scancode)
            {
            case SDL_SCANCODE_ESCAPE:
                app->setShouldClose(true);
                break;
            case SDL_SCANCODE_C:
                app->toggleMouse();
                break;
            case SDL_SCANCODE_R:
                restartGame();
                break;
            case SDL_SCANCODE_P:
                if (!paused)
                    pauseDifference = app->getCurrentTime() - lastDrop;
                else
                    lastDrop = app->getCurrentTime() - pauseDifference;

                togglePause();
                break;
            default:
                break;
            }
        case SDL_MOUSEMOTION:
            if (!paused)
                dropperX = clamp(app->getMouseX(), (int)(LEFT_BORDER_X + ballTypeToRadius[currentBall]), (int)(RIGHT_BORDER_X - ballTypeToRadius[currentBall]));
            break;
        case SDL_MOUSEBUTTONDOWN:
            switch (event.button.button)
            {
            case SDL_BUTTON_LEFT:
                if (app->getCurrentTime() - lastDrop >= DROP_DELAY && !paused)
                    dropBall();
                break;
            }
            break;
        }
    }
}

void Game::loadAllTextures(SDL_Renderer *renderer)
{
    textures = new Texture[TOTAL_TYPES];
    for (int i = 0; i < TOTAL_TYPES; i++)
    {
        std::string fileName = std::string(ballTypeToString[i]).append(".png");
        textures[i].setRenderer(renderer);
        bool res = textures[i].loadFromFile(fileName);

        if (!res)
            std::cout << "Unable to load file: " << fileName.c_str() << std::endl;
    }
}