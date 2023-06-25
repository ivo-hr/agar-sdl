#include "agar-server.h"


//player

void handleCollisions() {
    for (int i = 0; i < MAX_PLAYERS; i++) {
        if (players[i].alive) {
            for (int j = 0; j < MAX_PLAYERS; j++) {
                if (players[j].alive && i != j) {
                    float disX = fabs(players[i].x - players[j].x);
                    float disY = fabs(players[i].y - players[j].y);
                    float radiusSum = players[i].radius + players[j].radius;

                    if ((disX <= radiusSum && disY <= radiusSum) || disX <= players[j].radius && disY <= players[i].radius) {
                        if (players[i].radius > players[j].radius && players[j].alive) {
                            players[j].alive = false;
                            reSpawnPlayer(j);
                            players[i].radius += players[j].radius / 2;
                        }
                    }
                }
            }
        }
    }
}

int reSpawnPlayer(int playerNum)
{

    players[playerNum].x = (rand() % (WORLD_SIZE - (-WORLD_SIZE) + 1)) + (-WORLD_SIZE);
    players[playerNum].y = (rand() % (WORLD_SIZE - (-WORLD_SIZE) + 1)) + (-WORLD_SIZE);
    players[playerNum].radius = INI_RADIUS;
    players[playerNum].alive = true;

    return i;
}

void MovePlayer(int playerNum, int mouseX, int mouseY)
{
    float dx = (float)mouseX - WINDOW_WIDTH / 2;
    float dy = (float)mouseY - WINDOW_HEIGHT / 2;
    float distance = sqrt(dx * dx + dy * dy);

    if (distance > 0)
    {
        players[playerNum]->x += dx * LAG_FACTOR;
        players[playerNum]->y += dy * LAG_FACTOR;
    }
}

int insertPlayer(int playerNum)
{
    if (players[playerNum].alive)
    {
        printf("Error: Trying to create character on an already existing one %d\n" , clientSocket);
        return -1;
    }
    players[playerNum].x = (rand() % (WORLD_SIZE - (-WORLD_SIZE) + 1)) + (-WORLD_SIZE);
    players[playerNum].y = (rand() % (WORLD_SIZE - (-WORLD_SIZE) + 1)) + (-WORLD_SIZE);
    players[playerNum].radius = INI_RADIUS;
    players[playerNum].playerIndex = playerNum;
    players[playerNum].alive = true;
    return playerNum;
}

void removePlayer(int playerNum)
{
    players[playerNum].x = 0.0;
    players[playerNum].y = 0.0;
    players[playerNum].radius = 0.0;
    players[playerNum].alive = false;
    players[playerNum].playerIndex = false;
    players[playerNum].playerIndex = -2;
    lastClientTicks[playerNum] = 0; 
}

//server/client

void handleClient(int clientSocket, Player* players) {
    SerializableClientMessage* clientMessage = new_SerializableClientMessage();
    long long currentTimestamp = getCurrentTimestamp();


    while (1) {
        checkTimeout(players, int messagetype);

        //int playernum = playerSocket;
        //si el mensaje es login, se crea un nuevo jugador.
            //insertPlayer(playerNum);  

        //si el mensaje es logout se destruye el jugador y se corta la conexion
            //removePlayer(playerNum);

        //si el mensaje es input, actualiza la posicion del jugador
            //MovePlayer(playerNum, mouseX, mouseY);


        int32_t dataSize;
        if (recv(clientSocket, &dataSize, sizeof(int32_t), 0) < 0){
            printf("Error: Failed to receive data size from socket %d\n" , clientSocket);
            perror("recv");
            break;
        }
        char* data = (char*)malloc(dataSize);
        if (recv(clientSocket, data, dataSize, 0) < 0){
            printf("Error: Failed to receive data from socket %d\n" , clientSocket);
            perror("recv");
            break;
        }

        if (clientMessage->base.from_bin((Serializable*)clientMessage, data) == -1) {
            printf("Error: Failed to deserialize client message\n");
            break;
        }

        int playerIndex = clientMessage->message.playerIndex;
        printf("Received message from client %s with index %d\n", clientMessage->message.username, playerIndex);

        handleCollisions(players);

        ServerMessage serverMessage;
        serverMessage.messageId = 1;
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

 

        if (lastClientTicks[playerIndex] < 0){
            free_SerializableClientMessage(clientMessage);
            //close(clientSocket);
            exit(0);
        }
    }

    free_SerializableClientMessage(clientMessage);
    close(clientSocket);
}

// int createServerSocket(int port) {
//     int serverSocket;
//     struct sockaddr_in serverAddress;
//     printf("Starting server on port %d\n", port);
//     // Create server socket
//     serverSocket = socket(AF_INET, SOCK_STREAM, 0);
//     if (serverSocket == -1) {
//         perror("Error: Failed to create server socket\n");
//         exit(1);
//     }
//     // Set server address configuration
//     serverAddress.sin_family = AF_INET;
//     serverAddress.sin_addr.s_addr = INADDR_ANY;
//     serverAddress.sin_port = htons(port);
//     // Bind the server socket to the specified IP address and port
//     if (bind(serverSocket, (struct sockaddr*)&serverAddress, sizeof(serverAddress)) < 0) {
//         perror("Error: Failed to bind server socket\n");
//         exit(1);
//     }
//     // Listen for client connections
//     if (listen(serverSocket, MAX_CLIENTS) < 0) {
//         perror("Error: Failed to listen for client connections\n");
//         exit(1);
//     }
//     printf("Server started successfully!\n");  
//     return serverSocket;
// }
// int acceptClientConnection(int serverSocket) {
//     struct sockaddr_in clientAddress;
//     int clientSocket;
//     int clientAddressSize = sizeof(clientAddress);
//     // Accept client connection
//     clientSocket = accept(serverSocket, (struct sockaddr*)&clientAddress, (socklen_t*)&clientAddressSize);
//     if (clientSocket < 0) {
//         perror("Error: Failed to accept client connection\n");
//         exit(1);
//     }
//     return clientSocket;
// }

//recibe el mensaje del jugador
void handleClientRequests(int maxClients) {


//     Player players[MAX_PLAYERS];
//     for (int i = 0; i < MAX_PLAYERS; i++) {
//         strcpy(players[i].username, "");
//         players[i].x = 0.0;
//         players[i].y = 0.0;
//         players[i].radius = 0.0;
//         players[i].alive = false;
//         lastClientTicks[i] = 0;
//     }
//    while (1) {
//         // Accept client connection
//         int clientSocket = acceptClientConnection(serverSocket);
//         if (numConnectedPlayers >= MAX_PLAYERS) {
//             // Reject the connection if the maximum number of players is reached
//             printf("Max player limit reached. Rejecting new connection.\n");
//             close(clientSocket);
//             continue;
//         }
//         // Add client socket to the list
//         for (int i = 0; i < maxClients; i++) {
//             if (clientSockets[i] == 0) {
//                 clientSockets[i] = clientSocket;
//                 break;
//             }
//         }
//         // Create a new process to handle the client
//         int pid = fork();
//         //Send the player index to the client      
//         if (pid < 0) {
//             perror("Error: Failed to create child process\n");
//             exit(1);
//         } else if (pid == 0) {
//             // Child process handles the client
//             handleClient(clientSocket, players);
//             break;
//         }
//         numConnectedPlayers++; // Increment the number of connected players
//}
}

long long getCurrentTimestamp() {
    time_t currentTime = time(NULL);
    return (long long)currentTime * 1000;
}

// void releaseSocket(int clientSocket, int* clientSockets, int maxClients) {
//     Close the client socket
//     close(clientSocket);
//     Remove the client socket from the array
//     for (int i = 0; i < maxClients; i++) {
//         if (clientSockets[i] == clientSocket) {
//             clientSockets[i] = 0;
//             break;
//         }
//     }
// }

void checkTimeout(Player* players) {
    long long currentTimestamp = getCurrentTimestamp();

    for (int i = 0; i < MAX_PLAYERS; i++) {
        if (players[i].alive && currentTimestamp - lastClientTicks[i] > MAX_TIMEOUT) {
            // Player has timed out, handle the disconnection
            printf("Player with index %d has timed out. Disconnecting...\n", i);
            
            releaseSocket(clientSockets[i], clientSockets, MAX_CLIENTS);
            resetPlayerInfo(i, players);

            numConnectedPlayers--;

            lastClientTicks[i] = -1;
        }
    }
}

//food

bool CollisionFood(Food food[], Player myPlayer)
{
    for (int i = 0; i < MAX_FOOD; i++)
    {
        if (food[i].alive)
        {
            float dx = food[i].x - myPlayer.x;
            float dy = food[i].y - myPlayer.y;
            float distance = sqrt(dx * dx + dy * dy);

            if (distance < (myPlayer.radius) + FOOD_RADIUS)
            {
                food[i].alive = false;
                return true;
            }
        }
    }

    return false;
}

void insertFood(Food food[], int x, int y)
{
    int i = 0;
    while (food[i].alive)
        i++;

    food[i].x = x;
    food[i].y = y;
    food[i].alive = true;
}

void generateFood(Food food[], int numFood)
{
    for (int i = 0; i < numFood; i++)
    {
        int x = (rand() % (WORLD_SIZE - (-WORLD_SIZE) + 1)) + (-WORLD_SIZE);
        int y = (rand() % (WORLD_SIZE - (-WORLD_SIZE) + 1)) + (-WORLD_SIZE);
        insertFood(food, x, y);
    }
}

void initializeFood(Food food[])
{
    for (int i = 0; i < MAX_FOOD; i++)
    {
        food[i].alive = false;
    }
}

void removeFood(Food food[], int index)
{
    food[index].alive = false;
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