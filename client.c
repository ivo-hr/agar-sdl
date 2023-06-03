//Enrique JG
//Manuel PM

//CLIENT SIDE

#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <stdbool.h>
#include <math.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>


#include "net/Serialization.h"


#define WINDOW_WIDTH 640
#define WINDOW_HEIGHT 480
#define INI_RADIUS 20
#define LAG_FACTOR 0.01
#define MAX_FOOD 50
#define FOOD_RADIUS 10
#define WORLD_SIZE 1000


