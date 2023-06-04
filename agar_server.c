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
void* GameLogic(void *arg);




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
    struct sockaddr_in clientAddr;
    socklen_t clientAddrSize = sizeof(clientAddr);
    int clientSocket = accept(serverSocket, (struct sockaddr*) &clientAddr, &clientAddrSize);
    if (clientSocket == -1) {
        perror("Error accepting client connection");
    }

    return clientSocket;
}

void SendMessage(int socket, ServerMessage* message) {
    
    // Serialize the server message
    SerializableServerMessage* serializedServerMessage = new_SerializableServerMessage();
    serializedServerMessage->message = *message;
    serializedServerMessage->base.to_bin((Serializable*)serializedServerMessage);

    // Get the serialized server message data and size
    char* serializedServerData = serializedServerMessage->base.data((Serializable*)serializedServerMessage);
    int32_t serializedServerSize = serializedServerMessage->base.size((Serializable*)serializedServerMessage);

    // Send the serialized server message data to the client
    send(socket, serializedServerData, serializedServerSize, 0);

    free_SerializableServerMessage(serializedServerMessage);

}


void ReceiveMessage(int socket) {
    // Receive the serialized message data from the client
    char* serializedData = malloc(sizeof(ClientMessage));
    int bytesRead = recv(socket, serializedData, sizeof(ClientMessage), 0);
    if (bytesRead == -1) {
        perror("Error receiving message from client socket number " + socket);
    }

    if (bytesRead == 0) {
        printf("Client socket number %d disconnected\n", socket);
        close(socket);
        clientSockets[socket] = 0;
    }

    // Get the serialized client message data and size
    SerializableClientMessage* serializedClientMessage = new_SerializableClientMessage();
    char* serializedClientData = serializedClientMessage->base.data((Serializable*)serializedData);
    int32_t serializedClientSize = serializedClientMessage->base.size((Serializable*)serializedClientMessage);


    SerializableClientMessage* deserializedSerializedClientMessage = malloc(sizeof(SerializableClientMessage));
    deserializedSerializedClientMessage->base.from_bin((Serializable*)deserializedSerializedClientMessage, serializedClientData);

    printf("Message received from client socket number %d\n", socket);

    ApplyMessage(&deserializedSerializedClientMessage->message);


    
}

void ApplyMessage(ClientMessage* message) {

    for (int i = 0; i < MAX_PLAYERS; i++) {
        if (serverPlayers[i].playerIndex == message->player.playerIndex) {
            checkPlayerMovement(&serverPlayers[i], message->player.x, message->player.y);
            break;
        }
    }
}

void checkPlayerMovement(Player* player, float x, float y){

}


void HandleClients(int serverSocket, int clientSockets[], int maxClients) {
    fd_set readfds;
    int maxSocket = serverSocket;
    int activity;

    pthread_create(&gameLogicThread, NULL, GameLogic, NULL);

    while (true) {
        FD_ZERO(&readfds);
        FD_SET(serverSocket, &readfds);

        // Accept client connection
        int clientSocket = AcceptConnection(serverSocket);

        if (currentPlayers >= MAX_PLAYERS) {
            // Reject the connection if the maximum number of players is reached
            printf("Max player limit reached. Rejecting new connection.\n");
            close(clientSocket);
            continue;
        }
        else currentPlayers++; // Increment the number of connected players


        for (int i = 0; i < maxClients; i++) {
            if (clientSockets[i] == 0) {
                clientSockets[i] = clientSocket;
                break;
            }
        }

        //Send the player index to the client
        ServerMessage message;
        bool set = false;
        for (int i = 0; i < MAX_PLAYERS; i++) {
            if (serverPlayers[i].playerIndex == -2 && !set) {
                serverPlayers[i].playerIndex = i;
                set = true;
            }
            message.players[i] = serverPlayers[i];
        }

        message.messageId = 101;

        SendMessage(clientSocket, &message);

        

        activity = select(maxSocket + 1, &readfds, NULL, NULL, NULL);
        if (activity == -1) {
            perror("Error selecting sockets");
            exit(EXIT_FAILURE);
        }

        if (FD_ISSET(serverSocket, &readfds)) {
            int clientSocket = AcceptConnection(serverSocket);
            for (int i = 0; i < maxClients; i++) {
                if (clientSockets[i] == 0) {
                    clientSockets[i] = clientSocket;
                    break;
                }
            }
        }

        for (int i = 0; i < maxClients; i++) {
            int socket = clientSockets[i];
            if (socket != 0 && FD_ISSET(socket, &readfds)) {
                ReceiveMessage(socket);
                ServerMessage serverMessage;
                serverMessage.messageId = 1;
                memcpy(serverMessage.players, &serverPlayers, sizeof(serverPlayers));
                SendMessage(socket, &serverMessage);
            }
        }
    }

    pthread_join(gameLogicThread, NULL);
}

void* GameLogic(void* arg) {
    while (true) {
        for (int i = 0; i < MAX_PLAYERS; i++) {
            if (serverPlayers[i].playerIndex >= 0) {
                PlayerCollision(&serverPlayers[i]);
                checkPlayerMovement(&serverPlayers[i], serverPlayers[i].x, serverPlayers[i].y);
                
                if (clientSockets[i] != 0) {
                    ServerMessage message;
                    message.messageId = 2;
                    memcpy(message.players, &serverPlayers, sizeof(serverPlayers));
                    SendMessage(clientSockets[i], &message);
                }
            }
        }

        usleep(1000);
    }

    return NULL;
}

