#include "Application.h"
#include "Game.h"
#include "Mainmenu.h"

#include <iostream>

int main(int argc, char *argv[])
{
    Application * application = new Application("Suika Game", 800, 800);
    application->initApplication(WINDOW_HIDDEN);
    
    Game * game = new Game(application);
    MainMenu * menu = new MainMenu(application);

    application->registerScene(game, GAME_SCENE);
    application->registerScene(menu, MAINMENU_SCENE);
    
    application->setCurrentScene(menu);
    while (application->isRunning()) {
        application->tick();
    }

    return 0;
}