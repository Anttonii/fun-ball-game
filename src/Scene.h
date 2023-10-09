#ifndef SCENE_H
#define SCENE_H

// Scene object represents a logic that the parent application
// class follows.
//
// It's purely virtual class and it's method must be implemented
// by classes that extend it.
class Scene
{
    public:
        virtual void render(SDL_Renderer * renderer) = 0;
        virtual void pollEvents() = 0;
        virtual void update() = 0;
        virtual void cleanUp() = 0;
};

#endif