#include <Application.h>

Application::Application(const char * _title, int _width, int _height)
    : title(_title), width(_width), height(_height)
{
}

Application::~Application()
{
    destroyApplication();
}

void Application::destroyApplication()
{
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
}

// Initialize the application with given flags
// default value set to 0.
void Application::initApplication(Uint32 flags)
{
    // Init SDL.
    if (SDL_Init(SDL_INIT_EVERYTHING) != 0) {
        printf("error initializing SDL: %s\n", SDL_GetError());
    }

    // Create window
    SDL_Window* win = SDL_CreateWindow(title.c_str(),
                                       SDL_WINDOWPOS_CENTERED,
                                       SDL_WINDOWPOS_CENTERED,
                                       width, height, flags);

    if (win == NULL) {
        printf("error initializing window: %s\n", SDL_GetError());
    }

    // Construct SDL renderer object
    Uint32 render_flags = SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC;
    SDL_Renderer* rend = SDL_CreateRenderer(win, -1, render_flags);

    if (rend == NULL) {
        printf("error initializing renderer: %s\n", SDL_GetError());
    }

    // Initialize font engine
    int ttf = TTF_Init();
    if (ttf != 0)
    {
        printf("error initializing sdl_ttf: %s\n", SDL_GetError());
    }

    window = win;
    renderer = rend;
    _isRunning = true;
}

void Application::tick()
{
    currentTime = SDL_GetTicks();

    SDL_Event event;
    while(SDL_PollEvent(&event) != 0)
    {
        getFrameEvents().push_back(event);
    }

    handleEvents();
    currentScene->pollEvents();
    currentScene->update();
    currentScene->render(renderer);

    getFrameEvents().clear();

    SDL_Delay(1000 / 60);
}

// Handles events to keep track mouse position and
// whether or not SDL_Quit event is triggered.
void Application::handleEvents()
{
    for (SDL_Event event : getFrameEvents())
    {
        // update mouse position on mouse updates
        if (event.type == SDL_MOUSEMOTION || event.type == SDL_MOUSEBUTTONDOWN || event.type == SDL_MOUSEBUTTONUP)
            SDL_GetMouseState(&mouseX, &mouseY);

        if (event.type == SDL_QUIT)
            this->setShouldClose(true);
    }
}

void Application::setCurrentScene(Scene * scene)
{
    if (currentScene != NULL)
    {
        currentScene->cleanUp();
        currentScene = NULL;
    }
    currentScene = scene;
}

// Both grabs and hides the mouse depending on the current state.
void Application::toggleMouse()
{
    SDL_SetWindowMouseGrab(window, mouseGrabbed ? SDL_FALSE : SDL_TRUE);
    SDL_ShowCursor(mouseHidden ?  SDL_TRUE : SDL_FALSE);

    mouseGrabbed = !mouseGrabbed;
    mouseHidden = !mouseHidden;
}

// Grabs/ungrabs the mouse depending on state value.
void Application::grabMouse(bool state)
{
    mouseGrabbed = state;
    SDL_SetWindowMouseGrab(window, state ? SDL_TRUE : SDL_FALSE);
}

// Releases the mouse and shows it.
void Application::hideMouse(bool state)
{
    mouseHidden = state;
    SDL_ShowCursor(state ? SDL_FALSE : SDL_TRUE);
}