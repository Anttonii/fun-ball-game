#include "Texture.h"

Texture::Texture() : inner(NULL), _renderer(NULL)
{
}

Texture::Texture(SDL_Renderer * renderer) : inner(NULL), _renderer(renderer)
{
}

Texture::~Texture()
{
}

bool Texture::loadFromFile(std::string path)
{
    if (!_renderer)
        std::cout << "No renderer for texture!" << std::endl;
    
    std::string fullPath = std::string("./assets/sprites/").append(path);

    SDL_Surface *surface = IMG_Load(fullPath.c_str());
    if (surface) {
        inner = SDL_CreateTextureFromSurface(_renderer, surface);
        SDL_FreeSurface(surface);
        return true;
    }
    return false;
}

void Texture::setRenderer(SDL_Renderer * renderer) noexcept
{
    this->_renderer = renderer;
}