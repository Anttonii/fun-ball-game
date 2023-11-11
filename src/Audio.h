#ifndef AUDIO_H
#define AUDIO_H

#include <string>
#include <iostream>

#include "SDL.h"
#include "SDL_mixer.h"

class Audio
{
public:
    Audio();
    ~Audio();

    void init(std::string path);
    void play() noexcept;

private:
    // Loads the WAV chunk.
    Mix_Chunk *chunk;

    // Whether or not the audio has been initialized
    bool initialized;
};

#endif