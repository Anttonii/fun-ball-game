if not exists "./build/" mkdir build
cd build
cmake -G "MinGW Makefiles" ..
gnumake
cd ..
"./bin/FunBallGame.exe"