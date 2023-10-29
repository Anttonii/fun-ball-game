# Suika Game

A mixup between Tetris and 2048. Combine same colored balls to create new larger balls.
Win the game by getting to the last ball and having the highest score!

Originally game developed in Japan and can be played on browser at:
https://suika-game.app/

## Dependencies

Suika Game depends on the following libraries:

 - SDL2
 - SDL2_image
 - SDL2_ttf
 - Box2D

Makefiles and library linking is handled by CMake.

## Building

### Building on Windows

To build the game yourself you need to have installed the dependency libraries.
The CMake lists file is currently built to work on a personal computer, so they might require tweaking.

### Building on Mac OS X

Start by installing the dependencies

```
brew install SDL2
brew install SDL2_image
brew install SDL2_ttf
brew install Box2D
```

Generate the Unix makefiles and run make:

```
mkdir -p build
cd build
cmake -G "Unix Makefiles" ..
make
cd ..
./bin/SuikaGame
```

or use build.sh in projects root directory.

```
sh build.sh
```

## License

This repository is not under any license, feel to do anything with the code.