# agar-sdl
An agar.io clone made in SDL to be run on linux, on a client-server network structure.

## Installation guide (server)
1. Clone the repo
2. Launch your terminal of choice in the directory you cloned the repo, and type:
```
sudo apt-get install libsdl2-dev 
```
3. Once done, make the server.c AND the Serialization.c files like so:[^1]
``` 
gcc -c server.c -o server.o `sdl2-config --cflags --libs`
gcc -c net/Serialization.c -o serialization.o
```
4. Merge the server.o and serialization.o files into one make file:
```
gcc server.o socket.o serialization.o -o server
```

[^1]: This ensures that the make file will actually use sdl2. the typical `make file.c` will fail as it doesn't understand all it's dependencies)

4. Lastly, run! 
```
./server
```

You might want to port-forward your router to enable true online connectivity. You could also avoid the hassle and play in LAN instead.
