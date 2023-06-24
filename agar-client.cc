
#include "agar-client.h"



Player initializePlayer()
{
    int i = 0;
    bool found = false;
    Player myPlayer;
    myPlayer.playerIndex = MAX_PLAYERS;
    while (!found && i < MAX_PLAYERS){

        if (players[i].playerIndex == -2){
            if (i > 0) myPlayer.playerIndex = players[i-1].playerIndex;
            else myPlayer.playerIndex = players[i].playerIndex;
            found = true;
        }
        i++;
    }
    myPlayer.alive = true;
    myPlayer.radius = players[myPlayer.playerIndex].radius;
    myPlayer.x = players[myPlayer.playerIndex].x;
    myPlayer.y = players[myPlayer.playerIndex].y;
    return myPlayer;
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
        if (players[i].playerIndex < -1)
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


//metodos de comunicacion con el servidor

void sendClientMessageToServer(ClientMessage* clientMessage, int sock)
{
    printf("Sending client message to server\n");
    // Creamos una instancia de SerializableClientMessage y lo llenamos con el mensaje del cliente
    SerializableClientMessage* serializableClientMessage = new_SerializableClientMessage();
    serializableClientMessage->message = *clientMessage;
    printf("Client message created\n");
    // Serializamos el mensaje del cliente
    serializableClientMessage->base.to_bin((Serializable*)serializableClientMessage);
    printf("Client message serialized\n");
    // guardamos los datos del mensaje serializado
    char* serializedClienttData = serializableClientMessage->base.data((Serializable*)serializableClientMessage);
    int32_t serializedClientSize = serializableClientMessage->base.size((Serializable*)serializableClientMessage);
    serializedClienttData = malloc(serializedClientSize);
    printf("Client message data saved\n");
    // Enviamos el mensaje serializado al servidor
    send( sock, serializedClienttData, serializedClientSize, 0);

    // Liberamos la memoria del mensaje serializado
    free_SerializableClientMessage(serializableClientMessage);
}

ClientMessage* makeClientMessage(Player player)
{
    printf("Creating client message\n");

    // Allocate memory for the ClientMessage struct
    ClientMessage* clientMessage = (ClientMessage*)malloc(sizeof(ClientMessage));
    
    // Check if memory allocation was successful
    if (clientMessage == NULL) {
        printf("Failed to allocate memory for ClientMessage\n");
        return NULL;
    }

    // Assign values to the members of the ClientMessage struct
    clientMessage->player.x = player.x;
    clientMessage->player.y = player.y;
    clientMessage->player.radius = player.radius;
    clientMessage->player.playerIndex = player.playerIndex;
    clientMessage->timestamp = SDL_GetTicks();

    printf("Client message created\n");

    return clientMessage;
}

void ReceiveMessage(int socket)
{
   printf("Receiving message\n");

    // Create a buffer to receive the serialized message
    char buffer[sizeof(SerializableServerMessage)];

    // Receive the serialized message
    ssize_t bytesRead = recv(socket, buffer, sizeof(buffer), 0);
    if (bytesRead == -1) {
        perror("Error receiving message");
        return;
    }

    // Create a SerializableClientMessage object
    SerializableServerMessage* deserializedServerData = new_SerializableServerMessage();

    // Deserialize the received data
    deserializedServerData->base.from_bin((Serializable*)deserializedServerData, buffer);

    printf("Deserialized message\n");

    // Apply the message
    processServerMessage(&deserializedServerData->message);
    printf("Applied message\n");

    // Free the dynamically allocated memory
    free_SerializableServerMessage(deserializedServerData);

    printf("Freed memory\n");
}

void processServerMessage(ServerMessage* message)
{
    printf("Processing server message\n");
    for(int i = 0; i < MAX_PLAYERS; i++)
    {
        players[i].x = message->players[i].x;
        players[i].y = message->players[i].y;
        players[i].alive = message->players[i].alive;
        players[i].radius = message->players[i].radius;
        players[i].playerIndex = message->players[i].playerIndex;
    }
  
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
    printf("Socket creado exitosamente\n");
    // Configurar la dirección del servidor
    struct sockaddr_in serverAddr;
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(serverPort);
    if (inet_pton(AF_INET, serverIP, &(serverAddr.sin_addr)) <= 0) {
        perror("Error al configurar la dirección del servidor");
        return 1;
    }
    printf("Dirección del servidor configurada exitosamente\n");

    // Conectar con el servidor
    if (connect(sockfd, (struct sockaddr*)&serverAddr, sizeof(serverAddr)) == -1) {
        perror("Error al conectar con el servidor");
        return 1;
    }
    printf("Conexión con el servidor establecida exitosamente\n");


    // Inicialización de SDL, TTF y variables
    SDL_Init(SDL_INIT_VIDEO);
    TTF_Init();

    SDL_Window *window = SDL_CreateWindow("Game", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, WINDOW_WIDTH, WINDOW_HEIGHT, 0);
    SDL_Renderer *renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);

    TTF_Font *font = TTF_OpenFont("Assets/arial.ttf", 16);
    if (!font)
    {
        printf("Error al cargar la fuente\n");
        return 1;
    }

    bool running = true;

    Player myPlayer;
    myPlayer.alive = true;
    myPlayer.radius = INI_RADIUS;
    myPlayer.x = 0;
    myPlayer.y = 0;
    // se le manda se;al al servidor para que cree un personaje
    myPlayer.playerIndex = 13;

    ClientMessage *myClientMessage = makeClientMessage(myPlayer);

    sendClientMessageToServer(myClientMessage, sockfd);
    printf("Sent message to server\n");
    // se le asigna el personaje creado
    ReceiveMessage(sockfd);

    myPlayer = initializePlayer();

    // inicializa la comida
    Food food[MAX_FOOD];
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
                printf("x: %f, y: %f\n", myPlayer.x, myPlayer.y);
            }
        }
        

        // Actualizar posición del jugador local en el servidor
        ClientMessage *myClientMessage = makeClientMessage(myPlayer);

        printf("Sending message to server, my playerindex is %d\n" , myPlayer.playerIndex);

        sendClientMessageToServer(myClientMessage, sockfd);
        //send(sockfd, &clientMessage, sizeof(clientMessage), 0);

        // Actualizar el juego con los datos recibidos del servidor
        ReceiveMessage(sockfd);



        // Dibujar el juego
        SDL_SetRenderDrawColor(renderer, 200, 200, 200, 255);
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

        // tickrate
        SDL_Delay(1000 / 60);
    }

    // Liberar recursos
    TTF_CloseFont(font);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    TTF_Quit();
    SDL_Quit();

    return 0;
}