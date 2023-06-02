#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <stdbool.h>
#include <math.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>


#include "net/Serialization.h"


#define WINDOW_WIDTH 640
#define WINDOW_HEIGHT 480
#define INI_RADIUS 20
#define LAG_FACTOR 0.01
#define MAX_FOOD 50
#define FOOD_RADIUS 10
#define WORLD_SIZE 1000


typedef struct
{
    float x, y;
    bool alive;
} Food;

Player players[MAX_PLAYERS];

// Function prototypes
void initializePlayers(Player players[]);
int insertPlayer(Player players[], const char *name);
int reSpawnPlayer(Player players[], Player myPlayer);
void MovePlayer(Player* myPlayer, int mouseX, int mouseY);
void insertFood(Food food[], int x, int y);
void generateFood(Food food[], int numFood);
void initializeFood(Food food[]);
void removeFood(Food food[], int index);
void DrawText(SDL_Renderer *renderer, const char *text, int x, int y, TTF_Font *font, Uint8 red, Uint8 green, Uint8 blue);
void Ranking(SDL_Renderer *renderer, Player players[], int numPlayers, TTF_Font *font);
void DrawCircle(SDL_Renderer *renderer, int centerX, int centerY, int radius, Uint8 red, Uint8 green, Uint8 blue);
void DrawPlayer(SDL_Renderer *renderer, Player players[], int myPlayerNum, int cameraX, int cameraY, float scale, TTF_Font *font);
void drawFood(SDL_Renderer *renderer, Food food[], int cameraX, int cameraY, float scale);
void CollisionPlayers(Player players[]);
bool CollisionFood(Food food[], Player myPlayer, float scale);
void FollowPlayer(Player myPlayer, float *cameraX, float *cameraY, float *scale);

void updateGameFromServer(Player* players, int numPlayers);
void sendClientMessageToServer(ClientMessage* clientMessage);
void receiveServerMessageFromServer(ServerMessage* serverMessage);


// Función para actualizar el juego con los datos recibidos del servidor
void updateGameFromServer(serverMessage serverMessage, Player mainPlayer)
{
    

    printf("Actualizando juego desde el servidor:\n");
    for (int i = 0; i < MAX_PLAYERS; i++)
    {
        players[i] = serverMessage.players[i];
        if (mainPlayer.username == players[i].username)
        {
            mainPlayer = players[i];
        }
    }
}

// Función para serializar el mensaje del cliente y enviarlo al servidor
void sendClientMessageToServer(ClientMessage* clientMessage)
{
    // Creamos una instancia de SerializableClientMessage y lo llenamos con el mensaje del cliente
    SerializableClientMessage* serializableClientMessage = new_SerializableClientMessage();
    serializableClientMessage->message = *clientMessage;

    // Serializamos el mensaje del cliente
    serializableClientMessage->base.to_bin((Serializable*)serializableClientMessage);

    // Enviamos el mensaje serializado al servidor
    sendToServer((Serializable*)serializableClientMessage);

    // Liberamos la memoria del mensaje serializado
    free_SerializableClientMessage(serializableClientMessage);
}

// Función para recibir y deserializar el mensaje del servidor y actualizar el juego
void receiveServerMessageAndProcess()
{
    int32_t dataSize;
        if (recv(server, &dataSize, sizeof(int32_t), 0) < 0){
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
}


// players methods
void initializePlayers(Player players[])
{
    for (int i = 0; i < MAX_PLAYERS; i++)
    {
        players[i].radius = 0;
        players[i].alive = false;
    }
}

int insertPlayer(Player players[], const char *name)
{
    int i = 0;
    while (players[i].alive)
        i++;

    players[i].x = (rand() % (WORLD_SIZE - (-WORLD_SIZE) + 1)) + (-WORLD_SIZE);
    players[i].y = (rand() % (WORLD_SIZE - (-WORLD_SIZE) + 1)) + (-WORLD_SIZE);
    players[i].radius = INI_RADIUS;
    players[i].alive = true;
    strncpy(players[i].username, name, sizeof(players[i].username) - 1);
    players[i].username[sizeof(players[i].username) - 1] = '\0';

    return i;
}

int reSpawnPlayer(Player players[], Player myPlayer)
{
    int i = 0;
    while (players[i].alive)
        i++;

    players[i].x = (rand() % (WORLD_SIZE - (-WORLD_SIZE) + 1)) + (-WORLD_SIZE);
    players[i].y = (rand() % (WORLD_SIZE - (-WORLD_SIZE) + 1)) + (-WORLD_SIZE);
    players[i].radius = INI_RADIUS;
    players[i].alive = true;
    strcpy(players[i].username, myPlayer.username);

    return i;
}

void MovePlayer(Player* myPlayer, int mouseX, int mouseY)
{
    float dx = (float)mouseX - WINDOW_WIDTH / 2;
    float dy = (float)mouseY - WINDOW_HEIGHT / 2;
    float distance = sqrt(dx * dx + dy * dy);

    if (distance > 0)
    {
        myPlayer->x += dx * LAG_FACTOR;
        myPlayer->y += dy * LAG_FACTOR;
    }
}

// food methods
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

// Drawing methods
void DrawText(SDL_Renderer *renderer, const char *text, int x, int y, TTF_Font *font, Uint8 red, Uint8 green, Uint8 blue)
{
    SDL_Surface *surfaceMessage = TTF_RenderText_Solid(font, text, (SDL_Color){red, green, blue});
    SDL_Texture *message = SDL_CreateTextureFromSurface(renderer, surfaceMessage);

    SDL_Rect textRect;
    textRect.x = x;
    textRect.y = y;
    textRect.w = surfaceMessage->w;
    textRect.h = surfaceMessage->h;

    SDL_RenderCopy(renderer, message, NULL, &textRect);

    SDL_DestroyTexture(message);
    SDL_FreeSurface(surfaceMessage);
}

void Ranking(SDL_Renderer *renderer, Player players[], int numPlayers, TTF_Font *font)
{
    for (int i = 0; i < numPlayers; i++)
    {
        if (players[i].alive)
        {
            char text[30];
            sprintf(text, "%s (%.0f)", players[i].username, players[i].radius);

            DrawText(renderer, text, 10, 10 + i * 20, font, 255, 255, 255);
        }
    }
}

void DrawCircle(SDL_Renderer *renderer, int centerX, int centerY, int radius, Uint8 red, Uint8 green, Uint8 blue)
{
    for (double angle = 0; angle < 360; angle += 0.1)
    {
        double radians = angle * M_PI / 180.0;
        int x = centerX + radius * cos(radians);
        int y = centerY + radius * sin(radians);
        SDL_RenderDrawPoint(renderer, x, y);
    }
}

void DrawPlayer(SDL_Renderer *renderer, Player players[], int myPlayerNum, int cameraX, int cameraY, float scale, TTF_Font *font)
{
    for (int i = 0; i < MAX_PLAYERS; i++)
    {
        if (players[i].alive)
        {
            int x = (players[i].x - cameraX) * scale + WINDOW_WIDTH / 2;
            int y = (players[i].y - cameraY) * scale + WINDOW_HEIGHT / 2;
            int radius = players[i].radius * scale;

            Uint8 red, green, blue;

            // Color based on player number
            if (i == myPlayerNum)
            {
                red = 0;
                green = 255;
                blue = 0;
            }
            else
            {
                red = 255;
                green = 0;
                blue = 0;
            }

            // Draw player circle
            DrawCircle(renderer, x, y, radius, red, green, blue);

            // Draw player username
            DrawText(renderer, players[i].username, x - radius, y - radius - 15, font, red, green, blue);
        }
    }
}

void drawFood(SDL_Renderer *renderer, Food food[], int cameraX, int cameraY, float scale)
{
    for (int i = 0; i < MAX_FOOD; i++)
    {
        if (food[i].alive)
        {
            int x = (food[i].x - cameraX) * scale + WINDOW_WIDTH / 2;
            int y = (food[i].y - cameraY) * scale + WINDOW_HEIGHT / 2;
            int radius = FOOD_RADIUS * scale;

            // Draw food circle
            DrawCircle(renderer, x, y, radius, 0, 255, 0);
        }
    }
}

// Collision methods
void CollisionPlayers(Player players[])
{
    for (int i = 0; i < MAX_PLAYERS; i++)
    {
        if (players[i].alive)
        {
            for (int j = 0; j < MAX_PLAYERS; j++)
            {
                if (i != j && players[j].alive)
                {
                    float dx = players[j].x - players[i].x;
                    float dy = players[j].y - players[i].y;
                    float distance = sqrt(dx * dx + dy * dy);

                    if (distance < players[j].radius && players[j].radius > players[i].radius)
                    {
                        players[i].alive = false;
                        players[j].radius += players[i].radius;
                    }
                }
            }
        }
    }
}

bool CollisionFood(Food food[], Player myPlayer, float scale)
{
    for (int i = 0; i < MAX_FOOD; i++)
    {
        if (food[i].alive)
        {
            float dx = food[i].x - myPlayer.x;
            float dy = food[i].y - myPlayer.y;
            float distance = sqrt(dx * dx + dy * dy);

            if (distance < (myPlayer.radius * scale) + FOOD_RADIUS)
            {
                food[i].alive = false;
                return true;
            }
        }
    }

    return false;
}

void FollowPlayer(Player myPlayer, float *cameraX, float *cameraY, float *scale)
{
    *cameraX = myPlayer.x - WINDOW_WIDTH / (2 * *scale);
    *cameraY = myPlayer.y - WINDOW_HEIGHT / (2 * *scale);

    float playerSize = INI_RADIUS * *scale;
    float minScale = WINDOW_WIDTH / (playerSize * 4);

    if (*scale < minScale)
        *scale = minScale;
}


int main()
{
    // Pedir al usuario que ingrese la IP del servidor
    char serverIP[16];
    printf("Ingrese la IP del servidor: ");
    fgets(serverIP, sizeof(serverIP), stdin);
    serverIP[strcspn(serverIP, "\n")] = '\0';

    // Pedir al usuario que ingrese el puerto del servidor
    int serverPort;
    printf("Ingrese el puerto del servidor: ");
    scanf("%d", &serverPort);
    getchar();  // Consumir el salto de línea después del número

    // Crear un socket para la conexión con el servidor
    int sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd == -1) {
        perror("Error al crear el socket");
        return 1;
    }

    // Configurar la dirección del servidor
    struct sockaddr_in serverAddr;
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(serverPort);
    if (inet_pton(AF_INET, serverIP, &(serverAddr.sin_addr)) <= 0) {
        perror("Error al configurar la dirección del servidor");
        return 1;
    }

    // Conectar con el servidor
    if (connect(sockfd, (struct sockaddr*)&serverAddr, sizeof(serverAddr)) == -1) {
        perror("Error al conectar con el servidor");
        return 1;
    }


    // Inicialización de SDL, TTF y variables
    SDL_Init(SDL_INIT_VIDEO);
    TTF_Init();

    SDL_Window *window = SDL_CreateWindow("Game", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, WINDOW_WIDTH, WINDOW_HEIGHT, 0);
    SDL_Renderer *renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);

    TTF_Font *font = TTF_OpenFont("arial.ttf", 16);
    if (!font)
    {
        printf("Error al cargar la fuente\n");
        return 1;
    }

    bool running = true;

    Player myPlayer;
    Player players[MAX_PLAYERS];
    Food food[MAX_FOOD];

    initializePlayers(players);
    initializeFood(food);

    // Insertar jugador
    int myPlayerNum = insertPlayer(players, "Player1");

    // Generar alimentos
    generateFood(food, MAX_FOOD);

    while (running)
    {
        SDL_Event event;

        // Movimiento del jugador
        while (SDL_PollEvent(&event))
        {
            if (event.type == SDL_QUIT)
            {
                running = false;
            }
            else if (event.type == SDL_MOUSEMOTION)
            {
                MovePlayer(&myPlayer, event.motion.x, event.motion.y);
            }
        }

        // Actualizar el juego con los datos recibidos del servidor
        

        // Actualizar posición del jugador local en el servidor
        ClientMessage clientMessage;
        strcpy(clientMessage.username, myPlayer.username);
        clientMessage.x = myPlayer.x;
        clientMessage.y = myPlayer.y;
        clientMessage.radius = myPlayer.radius;
        clientMessage.playerIndex = myPlayerNum;
        clientMessage.timestamp = SDL_GetTicks();

        send(sockfd, &clientMessage, sizeof(clientMessage), 0);

        // Dibujar el juego
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
        SDL_RenderClear(renderer);

        // Actualizar cámara
        float cameraX, cameraY, scale;
        FollowPlayer(myPlayer, &cameraX, &cameraY, &scale);

        // Dibujar jugadores
        DrawPlayer(renderer, players, myPlayerNum, cameraX, cameraY, scale, font);

        // Dibujar alimentos
        drawFood(renderer, food, cameraX, cameraY, scale);

        // Dibujar ranking
        Ranking(renderer, players, MAX_PLAYERS, font);

        SDL_RenderPresent(renderer);
    }

    // Liberar recursos
    TTF_CloseFont(font);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    TTF_Quit();
    SDL_Quit();

    return 0;
}

