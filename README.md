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
- SDK2_mixer
- Box2D

Makefiles and library linking is handled by CMake.

## Building

### Building on Windows

To build the game yourself you need to have installed the dependency libraries.
The CMake lists file is currently built to work on a personal computer, so they will require tweaking.

Here is an example project structure that works on Windows:

```
📦dependencies
 ┣ 📂box2d
 ┃ ┣ 📂include
 ┃ ┃ ┗ 📂box2d
 ┃ ┗ 📂lib
 ┗ 📂SDL2
 ┃ ┣ 📂bin
 ┃ ┃ ┣ 📜SDL2.dll
 ┃ ┃ ┣ 📜SDL2_image.dll
 ┃ ┃ ┣ 📜SDL2_mixer.dll
 ┃ ┃ ┗ 📜SDL2_ttf.dll
 ┃ ┣ 📂include
 ┃ ┃ ┗ 📂SDL2
 ┃ ┣ 📂lib
 ┃ ┃ ┣ 📂cmake
 ┃ ┃ ┃ ┣ 📂SDL2
 ┃ ┃ ┃ ┣ 📂SDL2_image
 ┃ ┃ ┃ ┣ 📂SDL2_mixer
 ┃ ┃ ┃ ┗ 📂SDL2_ttf
 ┃ ┃ ┣ 📂pkgconfig
 ┃ ┗ 📂share
```

Essentially putting everything SDL related to their own folder and creating another one for box2d.  
Afterwards the project can be built by running the build script in project root:

```
./build.bat
```

### Building on Mac OS X

Start by installing the dependencies

```
brew install SDL2
brew install SDL2_image
brew install SDL2_ttf
brew install SDL2_mixer
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
