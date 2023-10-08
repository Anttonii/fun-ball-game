#ifndef TEXTURE_H__
#define TEXTURE_H__

#include "SDL.h"
#include "SDL_image.h"

#include <string>
#include <iostream>

class Texture
{
    public:
        SDL_Texture * inner;
        
        Texture();
        Texture(SDL_Renderer * renderer);
        ~Texture();

        // avoid io in contructor.
        bool loadFromFile(std::string path);

        void setRenderer(SDL_Renderer * renderer) noexcept;
    private:
        SDL_Renderer * _renderer;
};

#endif