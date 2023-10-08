#ifndef GAME_H__
#define GAME_H__

#include <vector>
#include <iostream>
#include <memory>
#include <algorithm>
#include <stdlib.h>
#include <time.h>

#include "SDL.h"

#include "box2d/box2d.h"

#include "Ball.h"
#include "Texture.h"
#include "Utils.h"
#include "Text.h"

#define LEFT_BORDER_X 50.0f
#define LEFT_BORDER_Y 50.0f

#define RIGHT_BORDER_X 550.0f
#define RIGHT_BORDER_Y 50.0f

#define BOTTOM_EDGE_X 50.0f
#define BOTTOM_EDGE_Y 650.0f

#define GAME_BOX_HEIGHT 600.0f
#define GAME_BOX_WIDTH 500.0f

#define SCORE_TEXT_X 50
#define SCORE_TEXT_Y 710

#define NEXT_BALL_TEXT_X 625
#define NEXT_BALL_TEXT_Y 70

// only allow dropping every 0.75 seconds
#define DROP_DELAY 750

// gravity vector
static const b2Vec2 _gravity = b2Vec2(0, 10);

static Texture * loadAllBallTextures(SDL_Renderer * renderer)
{
    static Texture * textures = new Texture[TOTAL_TYPES];
    for (int i = 0; i < TOTAL_TYPES; i++)
    {
        std::string fileName = std::string(ballTypeToString[i]).append(".png");
        textures[i].setRenderer(renderer);
        bool res = textures[i].loadFromFile(fileName);

        if (!res) 
        {
            std::cout << "Unable to load file: " << fileName.c_str() << std::endl;
            return NULL;
        }
    }
    return textures;
}

// Edges of the game world to keep balls within
struct Edge
{
    float x;
    float y;

    float width;
    float height;

    b2Body * body;
    b2BodyDef bodyDef;
    b2EdgeShape * shape;
    b2FixtureDef * fixture;
};

struct BallQueueData
{
    float x;
    float y;
    int ballType;
    b2Vec2 initialVelocity;
};


// forward declaration for BallContactListener
class Game;

// handles contact between 2 balls
class BallContactListener : public b2ContactListener
{
    public:
        // Store a reference to the game, the b2world and bounds for the edges of the game
        BallContactListener(Game * game, b2World * world);
        ~BallContactListener();

        void BeginContact(b2Contact * contact);
        //void PreSolve(b2Contact* contact, const b2Manifold* oldManifold);
    private:
        Game * _game;
        b2World * _world;
};

class Game
{
    public:
        Game(SDL_Renderer * renderer, bool * close);
        ~Game();

        // Initializes dependencies and other things before starting a game.
        bool init();
        void tick() noexcept;
        // Adds all the necessary components for a game to be played
        void startGame() noexcept;

        const std::unique_ptr<Ball>& getBallFromBody(b2Body * body) noexcept;
        
        void addBallToQueue(float x, float y, int ballType, b2Vec2 initialVelocity) noexcept;

        int score = 0;

        void updateScoreText();
    private:
        Game(const Game& game); // no copy constructor
        
        void render() noexcept;
        void pollEvents() noexcept;
        void addEdge(float x, float y, float width, float height) noexcept;

        // drop a ball from the dropper
        void dropBall() noexcept;

        // adds a ball to the game world
        void addBall(Ball * ball);
        void addBall(float x, float y, BallType type) noexcept;
        void addBall(float x, float y, BallType type, b2Vec2 initialVelocity) noexcept;

        // Removes a ball and destroys it with given body
        bool removeBall(b2Body * body) noexcept;

        void cleanUp() noexcept;
        
        b2BodyDef _groundBody;
        b2World _world;
        SDL_Renderer * _renderer;
        bool * _onClose;
        Texture * ballTextures;
        BallContactListener * _contactListener;

        std::vector<std::unique_ptr<Ball>> _balls;
        std::vector<std::unique_ptr<Edge>> _edges;

        std::vector<BallQueueData> _ballsToAdd;

        int mouseX = 0;
        int mouseY = 0;

        int dropperY = 70;
        int dropperX = 300;

        bool _paused;
        bool canDrop;
        
        Uint32 lastDrop = -DROP_DELAY;
        Uint32 currentTime = 0;

        // when the game gets paused, save the difference in time
        // so that when the game gets unpaused the delay persists
        Uint32 pauseDifference = 0;

        BallType currentBall;
        BallType nextBall;

        std::string scoreText;
        TextObject scoreTextObject;
        TextObject nextBallTextObject;
};

#endif