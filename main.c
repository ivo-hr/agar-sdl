#include <SDL2/SDL.h>
#include <stdbool.h>
#include <math.h>
#include <stdio.h>

#define WINDOW_WIDTH 640
#define WINDOW_HEIGHT 480
#define INI_RADIUS 40
#define LAG_FACTOR 0.01
#define MAX_PLAYERS 10
#define MAX_FOOD 20
#define FOOD_RADIUS 10

typedef struct {
    float x, y;
    float radius;
    bool alive;
} Player;

typedef struct {
    float x, y;
    bool alive;
} Food;

//players methods
void initializePlayers(Player players[]) {

    for (int i = 0; i < MAX_PLAYERS; i++) {
        players[i].alive = false;
    }
}

void insertPlayer(Player players[], int x, int y, int radius) {

    int i = 0;
    while (players[i].alive)
        i++;
    players[i].x = x;
    players[i].y = y;
    players[i].radius = radius;
    players[i].alive = true;
}

void insertFood(Food food[], int x, int y) {
    int i = 0;
    while (food[i].alive && i < MAX_FOOD) {
        i++;
    }
    if (i < MAX_FOOD) {
        food[i].x = x;
        food[i].y = y;
        food[i].alive = true;
    }
}

void generateFood(Food food[], int numFood, int maxX, int maxY) { 
    int x, y;
    for (int i = 0; i < numFood; i++) {
        x = rand() % maxX;
        y = rand() % maxY; 
        insertFood( food, x, y);
    }
}

//food methods
void initializeFood(Food food[]) {
    for (int i = 0; i < MAX_FOOD; i++) {
        food[i].alive = false;
    }
    generateFood(food, MAX_FOOD, WINDOW_HEIGHT, WINDOW_WIDTH);
}

void removeFood(Food food[], int index) {
    if (index >= 0 && index < MAX_FOOD) {
        food[index].alive = false;
    }
}

//render methods
void DrawCircle(SDL_Renderer* renderer, int centerX, int centerY, int radius, Uint8 red, Uint8 green, Uint8 blue) {
    SDL_SetRenderDrawColor(renderer, red, green, blue, 255);

    for (int w = 0; w < radius * 2; ++w) {
        for (int h = 0; h < radius * 2; ++h) {
            int dx = radius - w; // horizontal offset
            int dy = radius - h; // vertical offset
            if ((dx * dx + dy * dy) <= (radius * radius)) {
                SDL_RenderDrawPoint(renderer, centerX + dx, centerY + dy);
            }
        }
    }
}

void DrawPlayer(SDL_Renderer* renderer, Player players[], int myPlayerNum, int cameraX, int cameraY, float scale)
{
    // Render the players with the camera offset
    int i = 0;
    for(int i = 0; i < MAX_PLAYERS; i++) {
        if(players[i].alive)
        {

            float renderX = (players[i].x - cameraX) * scale;
            float renderY = (players[i].y - cameraY) * scale;
            float renderRadius = players[i].radius * scale;

            if (i != myPlayerNum)
            {
                DrawCircle(renderer, renderX, renderY, renderRadius, 150, 0, 0);
                DrawCircle(renderer, renderX, renderY, renderRadius * 0.95, 255, 0, 0);
            }
            else
            {
                DrawCircle(renderer, renderX, renderY, renderRadius, 0, 0, 150);
                DrawCircle(renderer, renderX, renderY, renderRadius * 0.95, 0, 0, 255);
            }
        }
    }
}

void drawFood(SDL_Renderer* renderer, Food food[], int cameraX, int cameraY, float scale)
{
    for(int i = 0; i < MAX_FOOD; i++) {
        if(food[i].alive)
        {
            float renderX = (food[i].x - cameraX) * scale;
            float renderY = (food[i].y - cameraY) * scale;
            float renderRadius = FOOD_RADIUS * scale;
            DrawCircle(renderer, renderX, renderY, renderRadius, 0, 150, 0);
            DrawCircle(renderer, renderX, renderY, renderRadius * 0.80, 0, 255, 0);
        }
    }
}
//collision methods
void CollisionPlayers(Player players[]) {
    for(int i = 0; i < MAX_PLAYERS; i++) {
        if (players[i].alive) {
            for(int j = 0; j < MAX_PLAYERS; j++) {
                if(players[j].alive){
                    float disX = fabs(players[i].x - players[j].x);
                    float disY = fabs(players[i].y - players[j].y);
                    float radiusSum = players[i].radius + players[j].radius;

                    if ((disX <= players[i].radius && disY <= players[j].radius) || disX <= players[j].radius && disY <= players[i].radius) {
                        if(players[i].radius > players[j].radius && players[j].alive)
                        {
                            players[j].alive = false;
                            players[i].radius += players[j].radius/2;
                        }

                    }
                }
            }
        }
    }
}

bool CollisionFood(Food food[], Player myPlayer, float scale) {
    float scaledRadiusSum = (myPlayer.radius + FOOD_RADIUS) * scale;

    for (int i = 0; i < MAX_FOOD; i++) {
        if (food[i].alive) {
            float dx = myPlayer.x - food[i].x;
            float dy = myPlayer.y - food[i].y;
            float squaredDistance = dx * dx + dy * dy;
            float squaredRadiusSum = scaledRadiusSum * scaledRadiusSum;

            if (squaredDistance <= squaredRadiusSum) {
                removeFood(food, i);
                generateFood(food, 1, WINDOW_WIDTH, WINDOW_HEIGHT);
                return true;
            }
        }
    }
    return false;
}

//camera method
void FollowPlayer(Player myPlayer, float* cameraX, float* cameraY, float* scale) {
    // Adjust the camera position based on the player's position
    if (myPlayer.alive) {
        *cameraX = myPlayer.x - (WINDOW_WIDTH / 2) / (*scale);
        *cameraY = myPlayer.y - (WINDOW_HEIGHT / 2) / (*scale);
        *scale = INI_RADIUS / myPlayer.radius; // Adjust the scale based on player's radius
    }
}


int main(int argc, char* argv[]) {

    if (argc < 2) {
        printf("Please provide the IP address as a command-line argument.\n");
        return 1;
    }
    SDL_Init(SDL_INIT_VIDEO);

    char windowTitle[100];
    sprintf(windowTitle, "agar.SDL - Server at %s", argv[1]);
    SDL_Window* window = SDL_CreateWindow(windowTitle, SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, WINDOW_WIDTH, WINDOW_HEIGHT, 0);
    SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);


    Food food[MAX_FOOD];
    initializeFood(food);
    

    //esta parte quitarla cuando se meta en el servidor
    Player players[MAX_PLAYERS];
    initializePlayers(players);

    //generating the player
    int i = 0;
    while (players[i].alive) {
        i++;
    }
    int myPlayerNum = i;
    insertPlayer(players, WINDOW_WIDTH / 2, WINDOW_WIDTH / 2, INI_RADIUS);

    //inserting example enemies
    insertPlayer(players, WINDOW_WIDTH / 4, WINDOW_WIDTH /4, 51);
    insertPlayer(players, WINDOW_WIDTH / 3, WINDOW_WIDTH /3, 35);



    // Camera position and scale
    float cameraX = 0;
    float cameraY = 0;
    float scale = 1.5;


    // Main loop flag and event handler
    bool quit = false;
    SDL_Event event;

    // MAIN LOOP
    while (!quit) {

        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) {
                quit = true;
            }
        }

        int mouseX, mouseY;
        SDL_GetMouseState(&mouseX, &mouseY);

        SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
        SDL_RenderClear(renderer);

        // Circle follow cursor movement with lag
        if (WINDOW_WIDTH / 2 > mouseX)
            players[myPlayerNum].x -= (WINDOW_WIDTH / 2 - mouseX) * LAG_FACTOR;
        else if (WINDOW_WIDTH / 2 < mouseX)
            players[myPlayerNum].x += (mouseX - WINDOW_WIDTH / 2) * LAG_FACTOR;

        if (WINDOW_HEIGHT/ 2 > mouseY)
            players[myPlayerNum].y -= (WINDOW_HEIGHT/ 2 - mouseY) * LAG_FACTOR;
        else if (WINDOW_HEIGHT/ 2  < mouseY)
            players[myPlayerNum].y += (mouseY - WINDOW_HEIGHT/ 2 ) * LAG_FACTOR;


        drawFood(renderer, food, cameraX, cameraY, scale);
     
     
        FollowPlayer(players[myPlayerNum], &cameraX, &cameraY, &scale); // Adjust the camera position
        
        //esto hay que meterlo en el server
        CollisionPlayers(players);

        DrawPlayer(renderer, players, myPlayerNum, cameraX, cameraY, scale);

        if(CollisionFood(food, players[myPlayerNum], scale))
            players[myPlayerNum].radius += 2;


        SDL_RenderPresent(renderer);
    }

    SDL_DestroyWindow(window);
    SDL_Quit();
    return 0;
}
