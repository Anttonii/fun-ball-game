#ifndef APPLICATION_H
#define APPLICATION_H

#include <iostream>
#include <string>
#include <vector>

#include "SDL.h"
#include "SDL_ttf.h"

#include "Scene.h"

#define WINDOW_HIDDEN SDL_WINDOW_HIDDEN

#define GAME_SCENE 0
#define MAINMENU_SCENE 1

// Application holds the key components of SDL window
class Application
{
    public:
        Application(const char * _title, int _width, int _height);
        ~Application();

        void initApplication(Uint32 flags = 0);
        void destroyApplication();

        void tick();

        void toggleMouse();
        void grabMouse(bool state);
        void hideMouse(bool state);

        inline void hide() noexcept { SDL_HideWindow(window); };
        inline void show() noexcept { SDL_ShowWindow(window); };

        inline void registerScene(Scene * scene, Uint8 id) noexcept { scenes[id] = scene; };

        inline SDL_Renderer * getRenderer() noexcept { return renderer; };
        inline bool isRunning() const noexcept { return _isRunning; };

        void setCurrentScene(Scene * scene);
        void setCurrentSceneById(Uint8 id);
        
        inline void setShouldClose(bool flag) noexcept { _isRunning = false; };

        inline int getMouseX() const noexcept { return mouseX; };
        inline int getMouseY() const noexcept { return mouseY; };
        inline int getWidth() const noexcept { return width; };
        inline int getHeight() const noexcept { return height; };

        /// Returns the time since SDL context was created.
        inline Uint32 getCurrentTime() const noexcept { return currentTime; };

        std::vector<SDL_Event>& getFrameEvents()
        {
            static std::vector<SDL_Event> frame_events;
            return frame_events;
        }

    private:
        void handleEvents();

        SDL_Window * window;
        SDL_Renderer * renderer;
        Scene * currentScene = NULL;

        std::string title;
        int width, height;
        bool _isRunning = false;

        Uint32 currentTime;
        int mouseX, mouseY;

        bool mouseHidden = false;
        bool mouseGrabbed = false;

        Scene ** scenes;
        Uint8 sceneID = 0;
};

#endif