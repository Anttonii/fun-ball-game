#include "Application.h"
#include "Game.h"

#include <iostream>

int main(int argc, char *argv[])
{
    Application * application = new Application("Suika Game", 800, 800);
    application->initApplication(WINDOW_HIDDEN);
    
    Game * game = new Game(application);
    if(!game->init())
    {
        application->setShouldClose(true);
    }
    game->startGame();
    
    application->setCurrentScene(game);
    while (application->isRunning()) {
        application->tick();
    }

    return 0;
}