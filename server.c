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
#include "net/Socket.h"

#define SERVER_PORT 8080
#define MAX_PLAYERS 12
#define MAX_CLIENTS 12

#define MAX_VELOCITY 200
#define MAX_TIMEOUT 5000 // 5 seconds until client is considered disconnected


int createServerSocket(int port);
int acceptClientConnection(int serverSocket);
void handleClientRequests(int serverSocket, int clientSockets[], int maxClients);
void handleCollisions(Player* players[]);
void handleClient(int clientSocket, Player* players[]);
long long getCurrentTimestamp();

int clientSockets[MAX_CLIENTS];
long long lastClientTicks[MAX_CLIENTS];

void handleCollisions(Player* players[]) {
    for (int i = 0; i < MAX_PLAYERS; i++) {
        if (players[i]->alive) {
            for (int j = 0; j < MAX_PLAYERS; j++) {
                if (players[j]->alive && i != j) {
                    float disX = fabs(players[i]->x - players[j]->x);
                    float disY = fabs(players[i]->y - players[j]->y);
                    float radiusSum = players[i]->radius + players[j]->radius;

                    if ((disX <= radiusSum && disY <= radiusSum) || disX <= players[j]->radius && disY <= players[i]->radius) {
                        if (players[i]->radius > players[j]->radius && players[j]->alive) {
                            players[j]->alive = false;
                            players[i]->radius += players[j]->radius / 2;
                        }
                    }
                }
            }
        }
    }
}

void handleClient(int clientSocket, Player* players[]) {
    SerializableClientMessage* clientMessage = new_SerializableClientMessage();
    long long currentTimestamp = getCurrentTimestamp();


    while (1) {
        int32_t dataSize;
        if (recv(clientSocket, &dataSize, sizeof(int32_t), 0) <= 0)
            printf("Error: Failed to receive data size from socket %d\n" , clientSocket);
            break;

        char* data = (char*)malloc(dataSize);
        if (recv(clientSocket, data, dataSize, 0) <= 0)
            printf("Error: Failed to receive data from socket %d\n" , clientSocket);
            break;

        if (clientMessage->base.from_bin((Serializable*)clientMessage, data) == -1) {
            printf("Error: Failed to deserialize client message\n");
            break;
        }

        int playerIndex = clientMessage->message.playerIndex;
        if (playerIndex >= 0 && playerIndex < MAX_PLAYERS) {

            lastClientTicks[playerIndex] = currentTimestamp;

            // Calculate velocity based on the elapsed time since the last received message
            float deltaTime = (currentTimestamp - clientMessage->message.timestamp) / 1000.0;
            float velocity = sqrtf(powf(clientMessage->message.x - players[playerIndex]->x, 2) +
                                   powf(clientMessage->message.y - players[playerIndex]->y, 2)) / deltaTime;


            if (velocity > MAX_VELOCITY) {
                printf("Client with index %d moved too fast! Adjusting position.\n", playerIndex);

                // Backtrack the client position to a reasonable value
                float backtrackDistance = MAX_VELOCITY * deltaTime;
                float directionX = (players[playerIndex]->x - clientMessage->message.x) / velocity;
                float directionY = (players[playerIndex]->y - clientMessage->message.y) / velocity;

                players[playerIndex]->x += backtrackDistance * directionX;
                players[playerIndex]->y += backtrackDistance * directionY;
            } else {
                players[playerIndex]->x = clientMessage->message.x;
                players[playerIndex]->y = clientMessage->message.y;
            }

            players[playerIndex]->radius = clientMessage->message.radius;
            players[playerIndex]->alive = true;
        }

        handleCollisions(players);

        ServerMessage serverMessage;
        serverMessage.messageId = 1;
        serverMessage.serverTimestamp = getCurrentTimestamp();
        memcpy(serverMessage.players, players, sizeof(Player) * MAX_PLAYERS);

        SerializableServerMessage* serializableServerMessage = new_SerializableServerMessage();
        serializableServerMessage->message = serverMessage;
        serializableServerMessage->base.to_bin((Serializable*)serializableServerMessage);

        int32_t serverDataSize = serializableServerMessage->base._size;

        if (send(clientSocket, &serverDataSize, sizeof(int32_t), 0) <= 0)
            break;
        if (send(clientSocket, serializableServerMessage->base._data, serverDataSize, 0) <= 0)
            break;

        free(serializableServerMessage->base._data);
        free(serializableServerMessage);
        free(data);

        checkTimeout(players);

        if (lastClientTicks[playerIndex] < 0){
            free_SerializableClientMessage(clientMessage);
            close(clientSocket);
            exit(0);
        }
    }

    free_SerializableClientMessage(clientMessage);
    close(clientSocket);
}

int createServerSocket(int port) {
    int serverSocket;
    struct sockaddr_in serverAddress;

    printf("Starting server on port %d\n", port);
    // Create server socket
    serverSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (serverSocket == -1) {
        perror("Error: Failed to create server socket\n");
        exit(1);
    }

    // Set server address configuration
    serverAddress.sin_family = AF_INET;
    serverAddress.sin_addr.s_addr = INADDR_ANY;
    serverAddress.sin_port = htons(port);

    // Bind the server socket to the specified IP address and port
    if (bind(serverSocket, (struct sockaddr*)&serverAddress, sizeof(serverAddress)) < 0) {
        perror("Error: Failed to bind server socket\n");
        exit(1);
    }

    // Listen for client connections
    if (listen(serverSocket, MAX_CLIENTS) < 0) {
        perror("Error: Failed to listen for client connections\n");
        exit(1);
    }

    printf("Server started successfully!\n");
    
    return serverSocket;
}

int acceptClientConnection(int serverSocket) {
    struct sockaddr_in clientAddress;
    int clientSocket;
    int clientAddressSize = sizeof(clientAddress);

    // Accept client connection
    clientSocket = accept(serverSocket, (struct sockaddr*)&clientAddress, (socklen_t*)&clientAddressSize);
    if (clientSocket < 0) {
        perror("Error: Failed to accept client connection\n");
        exit(1);
    }

    return clientSocket;
}

int numConnectedPlayers = 0;

void handleClientRequests(int serverSocket, int clientSockets[], int maxClients) {
    Player players[MAX_PLAYERS];

    for (int i = 0; i < MAX_PLAYERS; i++) {
        strcpy(players[i].username, "");
        players[i].x = 0.0;
        players[i].y = 0.0;
        players[i].radius = 0.0;
        players[i].alive = false;
        lastClientTicks[i] = 0;
    }

   while (1) {
        // Accept client connection
        int clientSocket = acceptClientConnection(serverSocket);

        if (numConnectedPlayers >= MAX_PLAYERS) {
            // Reject the connection if the maximum number of players is reached
            printf("Max player limit reached. Rejecting new connection.\n");
            close(clientSocket);
            continue;
        }

        // Add client socket to the list
        for (int i = 0; i < maxClients; i++) {
            if (clientSockets[i] == 0) {
                clientSockets[i] = clientSocket;
                break;
            }
        }

        // Create a new process to handle the client
        int pid = fork();

        if (pid < 0) {
            perror("Error: Failed to create child process\n");
            exit(1);
        } else if (pid == 0) {
            // Child process handles the client
            handleClient(clientSocket, players);
            break;
        }

        numConnectedPlayers++; // Increment the number of connected players
    }
}


long long getCurrentTimestamp() {
    time_t currentTime = time(NULL);
    return (long long)currentTime * 1000;
}

void releaseSocket(int clientSocket, int* clientSockets, int maxClients) {
    // Close the client socket
    close(clientSocket);

    // Remove the client socket from the array
    for (int i = 0; i < maxClients; i++) {
        if (clientSockets[i] == clientSocket) {
            clientSockets[i] = 0;
            break;
        }
    }
}

void resetPlayerInfo(int playerIndex, Player* players) {
    // Reset player information
    strcpy(players[playerIndex].username, "");
    players[playerIndex].x = 0.0;
    players[playerIndex].y = 0.0;
    players[playerIndex].radius = 0.0;
    players[playerIndex].alive = false;
    lastClientTicks[playerIndex] = 0;
}

void checkTimeout(Player* players[]) {
    long long currentTimestamp = getCurrentTimestamp();

    for (int i = 0; i < MAX_PLAYERS; i++) {
        if (players[i]->alive && currentTimestamp - lastClientTicks[i] > MAX_TIMEOUT) {
            // Player has timed out, handle the disconnection
            printf("Player with index %d has timed out. Disconnecting...\n", i);
            
            releaseSocket(clientSockets[i], clientSockets, MAX_CLIENTS);
            resetPlayerInfo(i, players);

            numConnectedPlayers--;

            lastClientTicks[i] = -1;
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

    int serverSocket = createServerSocket(port);

    for (int i = 0; i < MAX_CLIENTS; i++) {
        clientSockets[i] = 0;
    }

    handleClientRequests(serverSocket, clientSockets, MAX_CLIENTS);
    close(serverSocket);
    return 0;
}
