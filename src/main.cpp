#include <stdio.h>

#include "SDL.h"
#include "SDL_image.h"
#include "SDL_timer.h"
#include "SDL_ttf.h"

#include "Game.h"

#include <iostream>

int main(int argc, char *argv[])
{
    if (SDL_Init(SDL_INIT_EVERYTHING) != 0) {
        printf("error initializing SDL: %s\n", SDL_GetError());
    }

    SDL_Window* win = SDL_CreateWindow("Funny Ball Game",
                                       SDL_WINDOWPOS_CENTERED,
                                       SDL_WINDOWPOS_CENTERED,
                                       800, 800, 0);
 
    Uint32 render_flags = SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC;
    SDL_Renderer* rend = SDL_CreateRenderer(win, -1, render_flags);
    bool close = false;

    int ttf = TTF_Init();
    if (ttf != 0)
    {
        std::cout << "Failed to initialize TTF engine." << std::endl;
    }
    
    Game game(rend, &close);
    if(!game.init())
    {
        close = true;
    }
    game.startGame();
    
    while (!close) {
        game.tick();
    }
 
    SDL_DestroyRenderer(rend);
    SDL_DestroyWindow(win);
    SDL_Quit();
    return 0;
}