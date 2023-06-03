#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <math.h>
#include <unistd.h>
#include <time.h>
#include "net/Socket.h"
#include "net/Serialization.h"

#define WORLD_SIZE 4000
#define INITIAL_RADIUS 20


Player serverPlayers[MAX_PLAYERS];

void initServerPlayers();

void playerMovement(Player* player, float x, float y);
void playerCollision(Player* player);

int CreateServerSocket(int port);
int AcceptConnection(int serverSocket);
void ReceiveMessage(int socket, ClientMessage* message);

void SendMessage(int socket, ServerMessage* message);


void initServerPlayers() {
    for (int i = 0; i < MAX_PLAYERS; i++) {
        serverPlayers[i].playerIndex = -2;
        serverPlayers[i].x = rand() % WORLD_SIZE;
        serverPlayers[i].y = rand() % WORLD_SIZE;
        serverPlayers[i].radius = INITIAL_RADIUS;
        serverPlayers[i].alive = false;
    }
}



