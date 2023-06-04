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
                else if (serverPlayers[i].radius > player->radius) {
                    serverPlayers[i].radius += player->radius/2;
                }
            }
            printf("Player %d collided\n", player->playerIndex);
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
    printf("Sending message\n");
    SerializableServerMessage *serializedMessage = new_SerializableServerMessage();
    serializedMessage->message = *message;
    serializedMessage->base.to_bin((Serializable*)serializedMessage);
    printf("Serialized message\n");

    char* serializedServerData = serializedMessage->base.data((Serializable*)serializedMessage);
    int32_t serializedServerSize = serializedMessage->base.size((Serializable*)serializedMessage);
    printf("Server message data saved\n");

    if (send(socket, serializedServerData, serializedServerSize, 0) == -1) {
        perror("Error sending message");
    }

    free_SerializableServerMessage(serializedMessage);
    printf("Freed memory\n");
}


void ReceiveMessage(int socket) {
    printf("Receiving message\n");

    // Create a buffer to receive the serialized message
    char buffer[sizeof(SerializableClientMessage)];

    // Receive the serialized message
    ssize_t bytesRead = recv(socket, buffer, sizeof(buffer), 0);
    if (bytesRead == -1) {
        perror("Error receiving message");
        return;
    }

    // Create a SerializableClientMessage object
    SerializableClientMessage* deserializedClientData = new_SerializableClientMessage();

    // Deserialize the received data
    deserializedClientData->base.from_bin((Serializable*)deserializedClientData, buffer);

    printf("Deserialized message\n");

    // Apply the message
    ApplyMessage(&deserializedClientData->message);
    printf("Applied message\n");

    // Free the dynamically allocated memory
    free_SerializableClientMessage(deserializedClientData);

    printf("Freed memory\n");
}




void ApplyMessage(ClientMessage* message) {
    printf("Applying client message\n");
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
    printf("Applied client message\n");
}

void HandleClients(int serverSocket, int clientSockets[], int maxClients) {
    printf("Handling clients...\n");
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
                    ServerMessage* message = (ServerMessage*)malloc(sizeof(ServerMessage));
                    printf("Preparing to send message to client %d\n", i);
                    message->messageId = 100;
                    memcpy(message->players, serverPlayers, sizeof(serverPlayers));
                    printf("Created message\n");
                    SendMessage(socket, message);
                }
            }
        }
    }
}

void checkPlayerMovement(Player* player, float x, float y) {
    printf("Checking player movement\n");
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
    printf("Checked player movement, moving onto collision...\n");
    PlayerCollision(player);
}

void* GameLogic(void* arg) {
    printf("Game logic started!\n");
    while (true) {
        usleep(100000); // sleep for 0.1 seconds

        for (int i = 0; i < MAX_PLAYERS; i++) {
            Player* player = &serverPlayers[i];
            if (player->playerIndex > -1) {

                

                checkPlayerMovement(player, player->x + rand() % 21 - 10, player->y + rand() % 21 - 10);
                PlayerCollision(player);


                ServerMessage* message = (ServerMessage*)malloc(sizeof(ServerMessage));
                    printf("Preparing to send game state to client %d\n", i);
                    message->messageId = 100;
                    memcpy(message->players, serverPlayers, sizeof(serverPlayers));
                    printf("Created game state message\n");
                    SendMessage(clientSockets[i], message);
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

    printf("\n\n Booting up server...\n");

    srand(time(NULL));

    initServerPlayers();
    printf("Initializing player array...\n");


    serverSocket = CreateServerSocket(port);
    printf("Server started on port %d\n", port);
    
    memset(clientSockets, 0, sizeof(clientSockets));
    printf("Initializing client sockets...\n");

    pthread_t gameLogicThread;
    pthread_create(&gameLogicThread, NULL, GameLogic, NULL);
    printf("Created game logic thread...\n");
    HandleClients(serverSocket, clientSockets, MAX_PLAYERS);

    pthread_join(gameLogicThread, NULL);
    return 0;
}
