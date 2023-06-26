
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <math.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <time.h>
#include <vector>

#include "net/Serializable.h"
#include "net/Message.h"
#include "net/Socket.h"

#define WINDOW_WIDTH 640
#define WINDOW_HEIGHT 480
#define LAG_FACTOR 0.01

#define SERVER_PORT 8080
#define MAX_PLAYERS 2

#define MAX_VELOCITY 200
#define MAX_TIMEOUT 5000 // 5 seconds until client is considered disconnected

#define MAX_FOOD 50
#define FOOD_RADIUS 10

#define WORLD_SIZE 1000
#define INI_RADIUS 20

typedef struct {
    int playerIndex;
    float x, y;
    float radius;
    bool alive;
} Player;

typedef struct{
    float x, y;
    bool alive;
} Food;   

Socket *mySocket;

Player players[MAX_PLAYERS];

Food foods[MAX_FOOD];

int numConnectedPlayers = 0;

std::vector<std::unique_ptr<Socket>> clients;

//client/server
void do_messages();

//comida
void insertFood(int x, int y);
void generateFood(int numFood);
void initializeFood();
void removeFood(int index);
void CollisionFood();

//jugador
void handleCollisions();
int insertPlayer();
void reSpawnPlayer(int playerNum);
void MovePlayer(int playerNum, int mouseX, int mouseY);
void removePlayer(int playerNum);


//server.h
// /**
//  *  Clase para el servidor de chat
//  */
// class ChatServer
// {
// public:
//     ChatServer(const char * s, const char * p): socket(s, p)
//     {
//         socket.bind();
//     };

//     /**
//      *  Thread principal del servidor recive mensajes en el socket y
//      *  lo distribuye a los clientes. Mantiene actualizada la lista de clientes
//      */
//     void do_messages();

// private:
//     /**
//      *  Lista de clientes conectados al servidor de Chat, representados por
//      *  su socket
//      */
//     std::vector<std::unique_ptr<Socket>> clients;

//     /**
//      * Socket del servidor
//      */
//     Socket socket;
// };