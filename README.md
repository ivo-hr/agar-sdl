# agar-sdl
An WIP agar.io clone made in SDL to be run on linux, on a TCP client-server network structure.

## Installation guide (test)
1. Clone the repo
2. Launch your terminal of choice in the directory you cloned the repo, and type:
```
sudo apt-get install libsdl2-dev 
```
3. Once done, make the main.c file like so:[^1]
``` 
gcc -o main main.c $(sdl2-config --cflags --libs) -lSDL2_ttf 
```
4. Lastly, run!
```
./main 1234
```

## Installation guide (client)
1. Clone the repo
2. Launch your terminal of choice in the directory you cloned the repo, and type:
```
sudo apt-get install libsdl2-dev 
```
3. Once done, make the server.c AND the Serialization.c files like so:[^1]
``` 
gcc -c client.c -o client.o `sdl2-config --cflags --libs` -lm -lSDL2_ttf -lSDL2
gcc -c net/Serialization.c -o serialization.o
```
4. Merge the server.o and serialization.o files into one make file:
```
gcc client.o socket.o serialization.o -o client -lm -lSDL2_ttf -lSDL2
```

[^1]: This ensures that the make file will actually use sdl2. the typical `make file.c` will fail as it doesn't understand all it's dependencies)

4. Lastly, run! 
```
./client
```


## Installation guide (server)
1. Clone the repo
2. Launch your terminal of choice in the directory you cloned the repo, and type:
```
sudo apt-get install libsdl2-dev 
```
3. Once done, make the server.c AND the Serialization.c files like so:[^1]
``` 
gcc -c agar_server.c -o agar_server.o `sdl2-config --cflags --libs` -lm
gcc -c net/Serialization.c -o serialization.o
```
4. Merge the server.o and serialization.o files into one make file:
```
gcc agar_server.o socket.o serialization.o -o server -lm
```

[^1]: This ensures that the make file will actually use sdl2. the typical `make file.c` will fail as it doesn't understand all it's dependencies)

4. Lastly, run! 
```
./server
```

You might want to port-forward your router to enable true online connectivity. You could also avoid the hassle and play in LAN instead.
