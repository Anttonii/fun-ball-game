#ifndef TEXT_H__
#define TEXT_H__

#include <iostream>
#include <memory>

#include <SDL.h>
#include <SDL_ttf.h>


static TTF_Font * SCORE_FONT()
{
    TTF_Font * font = TTF_OpenFont("./assets/fonts/IBM_VGA.ttf", 36);
    if (!font)
    {
        std::cout << "Unable to find font for score!" << std::endl;
    }
    return font;
}

// Represents a renderable text object
class TextObject
{
    private:
        int x;
        int y;

        SDL_Rect quad;
        SDL_Texture * texture;
        SDL_Renderer * renderer;
        TTF_Font * font;
    
    public:
        void createText(SDL_Renderer * _renderer, int fs, int _x, int _y, std::string text, const char * fPath)
        {
            x = _x;
            y = _y;
            renderer = _renderer;
            
            font = TTF_OpenFont(fPath, 36);

            SDL_Surface * textSurface = TTF_RenderText_Solid(font, text.c_str(), { 255, 255, 255, 255 });

            texture = SDL_CreateTextureFromSurface(renderer, textSurface);
            quad = { x, y, textSurface->w, textSurface->h };
            
            SDL_FreeSurface(textSurface);
        };

        void updateText(std::string text)
        {
            SDL_Surface * textSurface = TTF_RenderText_Solid(font, text.c_str(), { 255, 255, 255, 255 });

            texture = SDL_CreateTextureFromSurface(renderer, textSurface);
            quad = { x, y, textSurface->w, textSurface->h };
            
            SDL_FreeSurface(textSurface);
        }

        void renderText()
        {
            SDL_RenderCopy(renderer, texture, NULL, &quad);
        }
};

#endif