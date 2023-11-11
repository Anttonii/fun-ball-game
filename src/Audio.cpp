#include "Audio.h"

Audio::Audio()
{
}

Audio::~Audio()
{
    Mix_FreeChunk(chunk);
}

void Audio::init(std::string path)
{
    std::string fullPath = std::string("./assets/audio/").append(path);
    chunk = Mix_LoadWAV(fullPath.c_str());
    if (!chunk)
    {
        fprintf(stderr, "Could not open %s: %s\n", fullPath.c_str(), SDL_GetError());
        Mix_FreeChunk(chunk);
    }

    initialized = true;
}

void Audio::play() noexcept
{
    if (!initialized)
    {
        std::cout << "Can't play audio that hasn't been properly initialized" << std::endl;
        return;
    }

    if (Mix_PlayChannel(-1, chunk, 0) == -1)
    {
        printf("Waves sound could not be played!\n"
               "SDL_Error: %s\n",
               SDL_GetError());
        Mix_FreeChunk(chunk);
    }
}