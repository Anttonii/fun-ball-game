#include "Mainmenu.h"

MainMenu::MainMenu(Application * _app) : app(_app)
{
}

MainMenu::~MainMenu()
{
}

bool MainMenu::init()
{
    playButtonText.createText(app->getRenderer(), 36, 0, 0, "Play!", MENUFONT_PATH);
    hiscoresButtonText.createText(app->getRenderer(), 36, 0, 0, "Hiscores", MENUFONT_PATH);
    quitButtonText.createText(app->getRenderer(), 36, 0, 0, "Quit", MENUFONT_PATH);

    SDL_Rect playButtonRect = playButtonText.getBounds();
    SDL_Rect quitButtonRect = quitButtonText.getBounds();
    SDL_Rect hiscoresButtonRect = hiscoresButtonText.getBounds();

    playButtonText.setPosition(app->getWidth() / 2 - playButtonRect.w / 2, 100);
    hiscoresButtonText.setPosition(app->getWidth() / 2 - hiscoresButtonRect.w / 2, 150);
    quitButtonText.setPosition(app->getWidth() / 2 - quitButtonRect.w / 2, 200);

    app->show();

    return true;
}

void MainMenu::render(SDL_Renderer * renderer)
{
    playButtonText.renderText();
    hiscoresButtonText.renderText();
    quitButtonText.renderText();
}

void MainMenu::pollEvents()
{
    for(SDL_Event event : app->getFrameEvents())
    {
        switch(event.type)
        {
            case SDL_KEYDOWN:
            switch(event.key.keysym.scancode)
            {
                case SDL_SCANCODE_ESCAPE:
                    app->setShouldClose(true);
                    break;
            }
            case SDL_MOUSEBUTTONDOWN:
            switch(event.button.button)
            {
                case SDL_BUTTON_LEFT:
                    if(isInBounds(app->getMouseX(), app->getMouseY(), playButtonText.getBounds()))
                        app->setCurrentSceneById(GAME_SCENE);
                    else if(isInBounds(app->getMouseX(), app->getMouseY(), quitButtonText.getBounds()))
                        app->setShouldClose(true);
                    break;
            }
        }
    }
}

void MainMenu::cleanUp()
{
    playButtonText.destroyText();
    quitButtonText.destroyText();
}

void MainMenu::update()
{

}
