#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <math.h>
#include <unistd.h>
#include <time.h>
#include "net/Socket.h"
#include "net/Serialization.h"

Player serverPlayers[MAX_PLAYERS];

void playerCollision(Player* player);
void playerMovement(Player* player, float x, float y);

int CreateServerSocket(int port);
int AcceptConnection(int serverSocket);
void ReceiveMessage(int socket, ClientMessage* message);

void SendMessage(int socket, ServerMessage* message);
