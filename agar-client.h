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


typedef struct
{
    float x, y;
    bool alive;
} Food;

Player players[MAX_PLAYERS];

// Function prototypes
int insertPlayer(Player players[], const char *name);
int reSpawnPlayer(Player players[], Player myPlayer);
void MovePlayer(Player* myPlayer, int mouseX, int mouseY);
void insertFood(Food food[], int x, int y);
void generateFood(Food food[], int numFood);
void initializeFood(Food food[]);
void removeFood(Food food[], int index);
 void DrawText(SDL_Renderer *renderer, const char *text, int x, int y, TTF_Font *font, Uint8 red, Uint8 green, Uint8 blue);
void Ranking(SDL_Renderer *renderer, Player players[], int numPlayers, TTF_Font *font);
void DrawCircle(SDL_Renderer *renderer, int centerX, int centerY, int radius, Uint8 red, Uint8 green, Uint8 blue);
void DrawPlayer(SDL_Renderer *renderer, Player players[], int myPlayerNum, int cameraX, int cameraY, float scale, TTF_Font *font);
void drawFood(SDL_Renderer *renderer, Food food[], int cameraX, int cameraY, float scale);
bool CollisionFood(Food food[], Player myPlayer, float scale);
void FollowPlayer(Player myPlayer, float *cameraX, float *cameraY, float *scale);

void updateGameFromServer(Player* players, int numPlayers);
void sendClientMessageToServer(ClientMessage* clientMessage, int sock);
void receiveServerMessageFromServer(ServerMessage* serverMessage, int socket);
void processServerMessage(ServerMessage* message);