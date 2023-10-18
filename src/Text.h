#ifndef TEXT_H
#define TEXT_H

#include <iostream>
#include <memory>

#include <SDL.h>
#include <SDL_ttf.h>

static const char * MENUFONT_PATH = "./assets/fonts/IBM_VGA.ttf";
static const char * SCOREFONT_PATH = "./assets/fonts/IBM_VGA.ttf";

// Represents a renderable text object
class TextObject
{
    private:
        SDL_Rect quad;
        SDL_Texture * texture;
        SDL_Renderer * renderer;
        TTF_Font * font;
    
    public:
        void createText(SDL_Renderer * _renderer, int fs, int _x, int _y, std::string text, const char * fPath)
        {
            TTF_Font * _font = TTF_OpenFont(fPath, fs);
            if(_font == NULL)
            {
                std::cout << "Failed to initialize font with path: " << fPath << std::endl;
                return;
            }
            this->createText(_renderer, _x, _y, text, _font);
        }

        void createText(SDL_Renderer * _renderer, int _x, int _y, std::string& text, TTF_Font * _font)
        {
            renderer = _renderer;
            font = _font;

            SDL_Surface * textSurface = TTF_RenderText_Solid(font, text.c_str(), { 255, 255, 255, 255 });

            texture = SDL_CreateTextureFromSurface(renderer, textSurface);
            quad = { _x, _y, textSurface->w, textSurface->h };
            
            SDL_FreeSurface(textSurface);
        };

        void updateText(std::string text)
        {
            SDL_Surface * textSurface = TTF_RenderText_Solid(font, text.c_str(), { 255, 255, 255, 255 });

            SDL_DestroyTexture(texture);
            texture = SDL_CreateTextureFromSurface(renderer, textSurface);
            quad = { quad.x, quad.y, textSurface->w, textSurface->h };
            
            SDL_FreeSurface(textSurface);
        }

        void setPosition(int _x, int _y)
        {
            quad.x = _x;
            quad.y = _y;
        }

        SDL_Rect& getBounds()
        {
            return this->quad;
        }

        void renderText()
        {
            SDL_RenderCopy(renderer, texture, NULL, &quad);
        }

        void destroyText()
        {
            TTF_CloseFont(font);
            SDL_DestroyTexture(texture);
        }
};

#endif