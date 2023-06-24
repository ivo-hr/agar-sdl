
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <math.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <time.h>

#include "net/Serialization.h"
//#include "net/Socket.h"

#define SERVER_PORT 8080
#define MAX_PLAYERS 2
#define MAX_CLIENTS 2

#define MAX_VELOCITY 200
#define MAX_TIMEOUT 5000 // 5 seconds until client is considered disconnected

#define MAX_FOOD 50
#define WORLD_SIZE 1000
#define INI_RADIUS 20


typedef struct
{
    float x, y;
    bool alive;
} Food;
Player players[MAX_PLAYERS];

typedef struct {
    int playerIndex;
    float x, y;
    float radius;
    bool alive;
} Player;
Player players[MAX_PLAYERS];
int numConnectedPlayers = 0;

int createServerSocket(int port);
int acceptClientConnection(int serverSocket);
void handleClientRequests(int serverSocket, int clientSockets[], int maxClients);
void handleClient(int clientSocket, Player* players);
long long getCurrentTimestamp();
void checkTimeout(Player* players);
int clientSockets[MAX_CLIENTS];
long long lastClientTicks[MAX_CLIENTS];

//comida
void insertFood(Food food[], int x, int y);
void generateFood(Food food[], int numFood);
void initializeFood(Food food[]);
void removeFood(Food food[], int index);
bool CollisionFood(Food food[], Player myPlayer);

//jugador
void handleCollisions();
int insertPlayer();
int reSpawnPlayer(int playerNum);
void MovePlayer(int playerNum, int mouseX, int mouseY);
void removePlayer(int playerNum);
