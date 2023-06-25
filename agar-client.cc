ReceiveMessage#include "agar-client.h"



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

void DrawPlayer(SDL_Renderer *renderer, int cameraX, int cameraY, float scale, TTF_Font *font)
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
            if (i == MyPlayer)
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

void FollowPlayer(float *cameraX, float *cameraY, float *scale)
{
    *cameraX = players[MyPlayer].x - WINDOW_WIDTH / (2 * *scale);
    *cameraY = players[MyPlayer].y - WINDOW_HEIGHT / (2 * *scale);

    float playerSize = INI_RADIUS * *scale;
    float minScale = WINDOW_WIDTH / (playerSize * 4);

    if (*scale < minScale)
        *scale = minScale;
}

pair<float, float> MovePlayer(int mouseX, int mouseY)
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

void initializePlayers()
{
    for (int i = 0; i < MAX_PLAYERS; i++)
    {
        players[i].alive = false;
        players[i].x = 0
        players[i].y = 0
        players[i].radius = 0
    }
}

void initializeFood()
{
    for (int i = 0; i < MAX_FOOD; i++)
    {
        food[i].alive = false;
    }
}
//metodos de comunicacion con el servidor

void makeClientMessage(MessageType type, Socket serverSocket)
{
    printf("Creating client message\n");

    switch (msg->type) {
        case LOGIN: {

            LoginMessage inMsg("aggar_enyojer");
            mySocket->send(inMsg, serverSocket);
        }
        break;

        case INPUT: {
            
            InputMessage iMsg(mouseX,mouseY);
            mySocket->send(iMsg, serverSocket);
        }
        break;

        case LOGOUT: {
            
            LogoutMessage outMsg(MyPlayer);
            mySocket->send(outMsg, serverSocket);
        }
        break;
    
    }

    printf("Client message created\n");
}

void ReceiveMessage(Socket* serverSocket)
{
    Uint32 tick = SDL_GetTicks();
    while(SDL_GetTicks() - tick <= 5){
        Message* msg = new Message();
        int ret = socket->recv(*msg, &serverSocket);
        if(ret == -1) {
            delete msg;
            perror("Error receiving message");
        }
        else processServerMessage(msg);
    }
}

void processServerMessage(Message* msg)
{
    switch (msg->type) {
        case CONFIRM: {
            ConfirmMessage cMsg;
            cMsg.from_bin(msg.data());
            if (cMsg.index != -1) {
                MyPlayer = cMsg.index;
                printf("Player index asignado: ");                
            }
            delete cMsg;
        }
        break;
        case POSITIONS:{
            PositionMessage pMsg;
            pMsg.from_bin(msg.data());
            if (pMsg.players.size() > 0) {
                for(int i = 0; int < pMsg.size(); i++)
                {
                    players[i]->x = pMsg(i).first;
                    players[i]->y = pMsg(i).second;
                }
                printf("Posiciones actualizadas: ");                
            }
            delete pMsg;
        }
        break;

        case SIZES:{
            SizeMessage sMsg;
            sMsg.from_bin(msg.data());
            if (sMsg.players.size() > 0) {
                for(int i = 0; int < sMsg.size(); i++)
                {
                    players[]->size = sMsg(i);
                }
                printf("Tamaños actualizadas: ");                
            }
            else printf("Tamaños invalido: "); 
            delete sMsg;
        }
        break;

        case FOOD:
            FoodMessage pMsg;
            fMsg.from_bin(msg.data());
            if (fMsg.players.size() > 0) {
                for(int i = 0; int < fMsg.size(); i++)
                {
                    if(fMsg(i).first == -1 && fMsg(i).second == -1)
                    {
                        foods[i]->alive = false;
                    }
                    else{

                        foods[i]->alive = true;
                        foods[i]->x = fMsg(i).first;
                        foods[i]->y = fMsg(i).second;
                    }
                }
                printf("Posiciones actualizadas: ");                
            }
            delete fMsg;
        break;
        default:
        break;
    }
    delete msg
  
}

int main()
{
    // Pedir al usuario que ingrese la IP del cliente
    char IP[16];
    printf("Ingrese su propia ip: ");
    fgets(IP, sizeof(IP), stdin);
    IP[strcspn(IP, "\n")] = '\0';

    // Crear un socket del cliente
        mySocket = new socket(IP, "8080");

    if (mySocket->bind() == -1) {
        perror("Error al crear el socket del cliente");
        re
        turn 1;
    }
    // Pedir al usuario que ingrese la IP del servidor
    char IP[16];
    printf("Ingrese la IP del servidor: ");
    fgets(IP, sizeof(IP), stdin);
    IP[strcspn(IP, "\n")] = '\0';

    // Pedir al usuario que ingrese el puerto del servidor
    int serverPort;
    printf("Ingrese el puerto del servidor: ");
    scanf("%d", &serverPort);
    getchar();  // Consumir el salto de línea después del número

    // Crear un socket para la conexión con el servidor
    Socket* serverSocket = new socket(IP, serverPort);

    if (serverSocket->bind() == -1) {
        perror("Error al crear el socket del servidor");
        return 1;
    }
    printf("Socket del servidor creado exitosamente\n");



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


    // inicializa la comida
    initializeFood();
    initializePlayers();
    //mandar login
    
    makeClientMessage(LOGIN, serverSocket);
    printf("Sent login message to server\n");

    ReceiveMessage(serverSocket);
    printf("recived confim message from server\n");

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
                mouseX = event.motion.x;
                mouseY = event.motion.y;
                //printf("x: %f, y: %f\n", myPlayer.x, myPlayer.y);
            }
        }

        makeClientMessage(INPUT, serverSocket);
        ReceiveMessage(serverSocket);

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