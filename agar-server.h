
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
#define MAX_PLAYERS 12
#define MAX_CLIENTS 12

#define MAX_VELOCITY 200
#define MAX_TIMEOUT 5000 // 5 seconds until client is considered disconnected

int createServerSocket(int port);
int acceptClientConnection(int serverSocket);
void handleClientRequests(int serverSocket, int clientSockets[], int maxClients);
void handleCollisions(Player* players);
void handleClient(int clientSocket, Player* players);
long long getCurrentTimestamp();
void checkTimeout(Player* players);
int clientSockets[MAX_CLIENTS];
long long lastClientTicks[MAX_CLIENTS];