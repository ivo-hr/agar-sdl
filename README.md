# agar-sdl
An agar.io clone made in SDL to be run on linux, on a client-server network structure.

## Installation guide
1. Clone the repo
2. Launch your terminal of choice in the directory you cloned the repo, and type:
```
sudo apt-get install libsdl2-dev 
```
3. Once done, make the main.c file like so:[^1]
``` 
gcc -o main main.c $(sdl2-config --cflags --libs) -lSDL2_ttf 
```
[^1]: This ensures that the make file will actually use sdl2. the typical `make main.c` will fail as it doesn't understand all it's dependencies)

4. Lastly, run! 
```
./main
```
