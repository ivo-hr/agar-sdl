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
#include <vector>

#include "net/Serializable.h"
#include "net/Message.h"
#include "net/Socket.h"


#define WINDOW_WIDTH 640
#define WINDOW_HEIGHT 480
#define LAG_FACTOR 0.01
#define FOOD_RADIUS 10
#define MAX_FOOD 50;
#define MAX_PLAYERS 2;

typedef struct {
    int playerIndex;
    float x, y;
    float radius;
    bool alive;
} Player;

Player players[MAX_PLAYERS];
int MyPlayer;
typedef struct
{
    float x, y;
    bool alive;
} Food;
Food foods[MAX_FOOD];
Socket* mySocket;
float mouseX, mouseY;
// Function prototypes

void DrawText(SDL_Renderer *renderer, const char *text, int x, int y, TTF_Font *font, Uint8 red, Uint8 green, Uint8 blue);
void DrawCircle(SDL_Renderer *renderer, int centerX, int centerY, int radius, Uint8 red, Uint8 green, Uint8 blue);
void DrawPlayer(SDL_Renderer *renderer, Player players[], int myPlayerNum, int cameraX, int cameraY, float scale, TTF_Font *font);
void drawFood(SDL_Renderer *renderer, Food food[], int cameraX, int cameraY, float scale);
void FollowPlayer(Player myPlayer, float *cameraX, float *cameraY, float *scale);
void initializeFood();
void initializePlayers();


void sendClientMessageToServer(Message* clientMessage, int sock);

void ReceiveMessage(int socket);

void processServerMessage(Message* message);

void init();