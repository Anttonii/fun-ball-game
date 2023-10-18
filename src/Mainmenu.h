#ifndef MAINMENU_H
#define MAINMENU_H

#include "SDL.h"

#include "Application.h"
#include "Scene.h"
#include "Text.h"
#include "Utils.h"

class MainMenu : public Scene
{
    public:
        MainMenu(Application * _app);
        ~MainMenu();

        void render(SDL_Renderer * renderer);
        void pollEvents();
        void cleanUp();
        void update();
        bool init();

    private:
        TextObject playButtonText;
        TextObject quitButtonText;
        TextObject hiscoresButtonText;
        
        Application * app;
};

#endif