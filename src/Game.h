#ifndef GAME_H
#define GAME_H

#include <vector>
#include <iostream>
#include <memory>
#include <algorithm>
#include <stdlib.h>
#include <time.h>

#include "SDL.h"

#include "box2d.h"

#include "Application.h"
#include "Ball.h"
#include "Texture.h"
#include "Utils.h"
#include "Text.h"
#include "Scene.h"
#include "Audio.h"

#define LEFT_BORDER_X 50.0f
#define LEFT_BORDER_Y 70.0f

#define RIGHT_BORDER_X 550.0f
#define RIGHT_BORDER_Y 70.0f

#define BOTTOM_EDGE_X 50.0f
#define BOTTOM_EDGE_Y 670.0f

#define GAME_BOX_HEIGHT 600.0f
#define GAME_BOX_WIDTH 500.0f
#define GAME_BOX_TOP (BOTTOM_EDGE_Y - GAME_BOX_HEIGHT)

#define SCORE_TEXT_X 50
#define SCORE_TEXT_Y 710

#define NEXT_BALL_TEXT_X 625
#define NEXT_BALL_TEXT_Y 70

// only allow dropping every 0.75 seconds
#define DROP_DELAY 750

// gravity vector
static const b2Vec2 gravity = b2Vec2(0, 10);

// Edges of the game world to keep balls within
struct Edge
{
    float x;
    float y;

    float width;
    float height;

    b2Body *body;
    b2BodyDef bodyDef;
    b2EdgeShape *shape;
    b2FixtureDef *fixture;
};

class Ball;
class BallContactListener;
enum BallType : unsigned int;

class Game : public Scene
{
public:
    Game(Application *app);
    ~Game();

    // Initializes necessary game elements before starting
    bool init();

    // Returns a pointer to a ball from given b2Body
    // If none was found, returns NULL.
    Ball *getBallFromBody(b2Body *body) noexcept;

    // Adds to ball to queue, which then gets added
    // after the next physics step has occured
    void addBall(Ball *ball);

    // Virtual functions defined by scene.
    void render(SDL_Renderer *renderer);
    void pollEvents();
    void cleanUp();
    void update();

    void updateScoreText();

    inline void addScore(int _score) noexcept { score += _score; };
    inline const std::vector<Ball *> &getBalls() const noexcept { return balls; };
    inline void playPop() const noexcept { popAudio->play(); };

private:
    Game(const Game &game); // no copy constructor

    void addEdge(float x, float y, float width, float height) noexcept;

    /// Checks the game state, has the player won or lost?
    void checkState() noexcept;

    // Restarts the game to the initial state.
    void restartGame() noexcept;

    // drop a ball from the dropper
    void dropBall() noexcept;

    // Removes a ball and destroys it with given body
    bool removeBall(b2Body *body) noexcept;

    // Removes b2Bodies that are no longer needed
    // called after balls combine.
    void clearBodies() noexcept;

    // Pauses the game
    void togglePause() noexcept;

    // Renders a pause screen
    void renderPauseScreen() noexcept;

    // Toggles mouse state, grabbed and hidden when true
    void setMouseState(bool state) noexcept;

    // Generates randomly the next balls
    void generateNextBall() noexcept;

    // Loads game related textures
    void loadAllTextures(SDL_Renderer *renderer);

    Application *app;

    b2BodyDef groundBodyDef;
    b2World world;
    BallContactListener *contactListener;

    std::vector<Ball *> balls;
    std::vector<Edge> edges;

    std::vector<Ball *> ballsToAdd;

    int score = 0;

    float dropperY = 70.0f;
    float dropperX = 300.0f;

    bool paused;
    bool canDrop;

    Uint32 lastDrop = -DROP_DELAY;

    // when the game gets paused, save the difference in time
    // so that when the game gets unpaused the delay persists
    Uint32 pauseDifference = 0;

    BallType currentBall;
    BallType nextBall;

    std::string scoreText;
    TextObject scoreTextObject;
    TextObject nextBallTextObject;
    TextObject pauseTextObject;

    // Represents state of the droppers queue
    // makes updating more intuitive, the next ball gets updated
    // when the player is allowed to move instead of instantly.
    bool queueUpdated = true;

    // The audio to play when bubbles pop.
    Audio *popAudio;

    // Ball textures loaded
    Texture *textures;
};

#endif