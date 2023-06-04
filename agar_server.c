#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <math.h>
#include <unistd.h>
#include <time.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <sys/select.h>
#include <pthread.h>
//#include <sys/types.h>


#include "net/Serialization.h"

#define MAX_PLAYERS 12
#define WORLD_SIZE 4000
#define INITIAL_RADIUS 20
#define MAX_VELOCITY 10


Player serverPlayers[MAX_PLAYERS];
Player clientPlayerParams[MAX_PLAYERS];
int serverSocket;
int clientSockets[MAX_PLAYERS];

int currentPlayers = 0;

pthread_t gameLogicThread;


void initServerPlayers();

void checkPlayerMovement(Player* player, float x, float y);
void PlayerCollision(Player* player);

int CreateServerSocket(int port);
int AcceptConnection(int serverSocket);
void SendMessage(int socket, ServerMessage* message);
void ReceiveMessage(int socket);
void ApplyMessage(ClientMessage* message);

void HandleClients(int serverSocket, int clientSockets[], int maxClients);
void* GameLogic(void* arg);


void initServerPlayers() {
    for (int i = 0; i < MAX_PLAYERS; i++) {
        serverPlayers[i].playerIndex = -2;
        serverPlayers[i].x = rand() % WORLD_SIZE;
        serverPlayers[i].y = rand() % WORLD_SIZE;
        serverPlayers[i].radius = INITIAL_RADIUS;
        serverPlayers[i].alive = false;

        clientPlayerParams[i].playerIndex = -2;
        clientPlayerParams[i].x = rand() % WORLD_SIZE;
        clientPlayerParams[i].y = rand() % WORLD_SIZE;
        clientPlayerParams[i].radius = INITIAL_RADIUS;
        clientPlayerParams[i].alive = false;
    }
}

void PlayerCollision(Player* player) {
    for (int i = 0; i < MAX_PLAYERS; i++) {
        if (serverPlayers[i].playerIndex != player->playerIndex && serverPlayers[i].alive) {
            float dx = serverPlayers[i].x - player->x;
            float dy = serverPlayers[i].y - player->y;
            float distance = sqrt(dx * dx + dy * dy);

            if (distance < serverPlayers[i].radius + player->radius) {
                if (serverPlayers[i].radius < player->radius) {
                    serverPlayers[i].alive = false;
                }
            }
        }
    }
}

int CreateServerSocket(int port) {
    int serverSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (serverSocket == -1) {
        perror("Error creating server socket");
        exit(EXIT_FAILURE);
    }

    struct sockaddr_in serverAddr;
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(port);
    serverAddr.sin_addr.s_addr = INADDR_ANY;

    if (bind(serverSocket, (struct sockaddr*) &serverAddr, sizeof(serverAddr)) == -1) {
        perror("Error binding server socket");
        exit(EXIT_FAILURE);
    }

    if (listen(serverSocket, 1) == -1) {
        perror("Error listening server socket");
        exit(EXIT_FAILURE);
    }

    return serverSocket;
}

int AcceptConnection(int serverSocket) {
    int clientSocket;

    // Accept client connection
    clientSocket = accept(serverSocket, NULL, NULL);
    if (clientSocket < 0) {
        perror("Error: Failed to accept client connection\n");
        exit(1);
    }

    return clientSocket;
}

void SendMessage(int socket, ServerMessage* message) {
    SerializableServerMessage *serializedMessage = new_SerializableServerMessage();
    serializedMessage->message = *message;
    serializedMessage->base.to_bin((Serializable*)serializedMessage);

    if (send(socket, &serializedMessage, sizeof(serializedMessage), 0) == -1) {
        perror("Error sending message");
    }
}

void ReceiveMessage(int socket) {
    SerializableClientMessage *serializedMessage = new_SerializableClientMessage();
    ssize_t bytesRead = recv(socket, serializedMessage, sizeof(SerializableClientMessage), 0);
    if (bytesRead == -1) {
        perror("Error receiving message");
        return;
    }

    char* serializedClientData = serializedMessage->base.data((Serializable*)serializedMessage);
    int32_t serializedClientSize = serializedMessage->base.size((Serializable*)serializedMessage);

    // Allocate memory for deserialized data
    void* deserializedClientData = malloc(serializedClientSize);

    // Copy serialized data to the allocated memory
    memcpy(deserializedClientData, serializedClientData, serializedClientSize);

    // Create a new deserialized message object
    SerializableClientMessage* deserializedSerializedMessage = new_SerializableClientMessage();
    deserializedSerializedMessage->base.from_bin((Serializable*)deserializedSerializedMessage, deserializedClientData);

    // Apply the message
    ApplyMessage(&deserializedSerializedMessage->message);

    // Free the dynamically allocated memory
    free(deserializedClientData);
}


void ApplyMessage(ClientMessage* message) {
    for (int i = 0; i < MAX_PLAYERS; i++) {
        if (clientPlayerParams[i].playerIndex == -2) {
            clientPlayerParams[i].playerIndex = message->player.playerIndex;
            clientPlayerParams[i].alive = message->player.alive;
            clientPlayerParams[i].x = message->player.x;
            clientPlayerParams[i].y = message->player.y;
            clientPlayerParams[i].radius = message->player.radius;

            checkPlayerMovement(&serverPlayers[i], message->player.x, message->player.y);
            
            currentPlayers++;
            break;
        }
    }
}

void HandleClients(int serverSocket, int clientSockets[], int maxClients) {
    fd_set readfds;
    int maxSocket = serverSocket;

    while (true) {
        FD_ZERO(&readfds);
        FD_SET(serverSocket, &readfds);

        for (int i = 0; i < maxClients; i++) {
            int socket = clientSockets[i];
            if (socket > 0) {
                FD_SET(socket, &readfds);
                if (socket > maxSocket) {
                    maxSocket = socket;
                }
            }
        }

        int activity = select(maxSocket + 1, &readfds, NULL, NULL, NULL);
        if (activity < 0) {
            perror("Error in select");
            continue;
        }

        if (FD_ISSET(serverSocket, &readfds)) {
            int clientSocket = AcceptConnection(serverSocket);
            for (int i = 0; i < maxClients; i++) {
                if (clientSockets[i] == 0) {
                    clientSockets[i] = clientSocket;
                    printf("New connection, socket fd is %d\n", clientSocket);
                    break;
                }
            }
        }

        for (int i = 0; i < maxClients; i++) {
            int socket = clientSockets[i];
            if (FD_ISSET(socket, &readfds)) {
                ReceiveMessage(socket);
            }
        }

        for (int i = 0; i < maxClients; i++) {
            int socket = clientSockets[i];
            if (socket > 0) {
                Player* player = &clientPlayerParams[i];
                if (player->alive) {
                    ServerMessage message;
                    message.messageId = 100;
                    memcpy(&message.players, serverPlayers, sizeof(serverPlayers));

                    SendMessage(socket, &message);
                }
            }
        }
    }
}

void checkPlayerMovement(Player* player, float x, float y) {
    float dx = x - player->x;
    float dy = y - player->y;

    float distance = sqrt(dx * dx + dy * dy);
    if (distance > MAX_VELOCITY) {
        dx = (dx / distance) * MAX_VELOCITY;
        dy = (dy / distance) * MAX_VELOCITY;
    }

    player->x += dx;
    player->y += dy;

    if (player->x < 0) {
        player->x = 0;
    } else if (player->x > WORLD_SIZE) {
        player->x = WORLD_SIZE;
    }

    if (player->y < 0) {
        player->y = 0;
    } else if (player->y > WORLD_SIZE) {
        player->y = WORLD_SIZE;
    }

    PlayerCollision(player);
}

void* GameLogic(void* arg) {
    while (true) {
        usleep(100000); // sleep for 0.1 seconds

        for (int i = 0; i < MAX_PLAYERS; i++) {
            Player* player = &serverPlayers[i];
            if (player->alive) {
                checkPlayerMovement(player, player->x + rand() % 21 - 10, player->y + rand() % 21 - 10);
            }
        }
    }
}

int main() {

    int port;
    printf("Welcome to the AGAR.SDL server!\n\n To play this game truly online, you will need to port forward. If you don't, you will be restricted to your network. \n Info about how you can port forward your router can be found here: https://portforward.com/how-to-port-forward/\n\n");
    sleep(2);
    printf("Enter the port number: ");
    scanf("%d", &port);

    bool foundPort = false;
    while (!foundPort) {
        if (port <= 0 || port > 65535) {
            printf("\n\nInvalid port number! The port number must be between 1 and 65535.\n");
            sleep(1);
            printf("Enter the port number: ");
            scanf("%d", &port);
        } else {
            foundPort = true;
        }
    }

    srand(time(NULL));

    initServerPlayers();

    serverSocket = CreateServerSocket(port);

    printf("Server started on port %d\n", port);

    memset(clientSockets, 0, sizeof(clientSockets));

    HandleClients(serverSocket, clientSockets, MAX_PLAYERS);

    return 0;
}
