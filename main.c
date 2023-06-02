#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <stdbool.h>
#include <math.h>
#include <stdio.h>
#include <string.h>

#define WINDOW_WIDTH 640
#define WINDOW_HEIGHT 480
#define INI_RADIUS 40
#define LAG_FACTOR 0.01
#define MAX_PLAYERS 10
#define MAX_FOOD 100
#define FOOD_RADIUS 10
#define WORLD_SIZE 1000

typedef struct
{
    char username[20];
    float x, y;
    float radius;
    bool alive;
} Player;

typedef struct
{
    float x, y;
    bool alive;
} Food;

// players methods
void initializePlayers(Player players[])
{

    for (int i = 0; i < MAX_PLAYERS; i++)
    {

        players[i].radius = 0;
        players[i].alive = false;
    }
}

void insertPlayer(Player players[], int x, int y, int radius, const char *name)
{

    int i = 0;
    while (players[i].alive)
        i++;
    players[i].x = x;
    players[i].y = y;
    players[i].radius = radius;
    players[i].alive = true;
    strncpy(players[i].username, name, sizeof(players[i].username) - 1);
    players[i].username[sizeof(players[i].username) - 1] = '\0';
}

void insertFood(Food food[], int x, int y)
{
    int i = 0;
    while (food[i].alive && i < MAX_FOOD)
    {
        i++;
    }
    if (i < MAX_FOOD)
    {
        food[i].x = x;
        food[i].y = y;
        food[i].alive = true;
    }
}

void generateFood(Food food[], int numFood)
{

    int x, y;
    for (int i = 0; i < numFood; i++)
    {
        x = (rand() % (WORLD_SIZE - (-WORLD_SIZE) + 1)) + (-WORLD_SIZE);
        y = (rand() % (WORLD_SIZE - (-WORLD_SIZE) + 1)) + (-WORLD_SIZE);
        printf("X: %d", x);
        printf("Y: %d\n", y);
        insertFood(food, x, y);
    }
}

void MovePlayer(Player* myPlayer, int mouseX, int mouseY)
{
    int deltaX, deltaY;

    if (WINDOW_WIDTH / 2 > mouseX)
        deltaX = -(WINDOW_WIDTH / 2 - mouseX) * LAG_FACTOR;
    else if (WINDOW_WIDTH / 2 < mouseX)
        deltaX = (mouseX - WINDOW_WIDTH / 2) * LAG_FACTOR;
    else
        deltaX = 0;

    if (WINDOW_HEIGHT / 2 > mouseY)
        deltaY = -(WINDOW_HEIGHT / 2 - mouseY) * LAG_FACTOR;
    else if (WINDOW_HEIGHT / 2 < mouseY)
        deltaY = (mouseY - WINDOW_HEIGHT / 2) * LAG_FACTOR;
    else
        deltaY = 0;

    // // Circle follow cursor movement with lag
    // if (WINDOW_WIDTH / 2 > mouseX)
    //     players[myPlayerNum].x -= (WINDOW_WIDTH / 2 - mouseX) * LAG_FACTOR;
    // else if (WINDOW_WIDTH / 2 < mouseX)
    //     players[myPlayerNum].x += (mouseX - WINDOW_WIDTH / 2) * LAG_FACTOR;

    // if (WINDOW_HEIGHT / 2 > mouseY)
    //     players[myPlayerNum].y -= (WINDOW_HEIGHT / 2 - mouseY) * LAG_FACTOR;
    // else if (WINDOW_HEIGHT / 2 < mouseY)
    //     players[myPlayerNum].y += (mouseY - WINDOW_HEIGHT / 2) * LAG_FACTOR;
        


    // Update player's position
    myPlayer->x += deltaX;
    myPlayer->y += deltaY;

    // Ensure the player's position stays within bounds
    if (myPlayer->x < -WORLD_SIZE)
        myPlayer->x = -WORLD_SIZE;
    else if (myPlayer->x > WORLD_SIZE)
        myPlayer->x = WORLD_SIZE;

    if (myPlayer->y < -WORLD_SIZE)
        myPlayer->y = -WORLD_SIZE;
    else if (myPlayer->y > WORLD_SIZE)
        myPlayer->y = WORLD_SIZE;
}

// food methods
void initializeFood(Food food[])
{
    for (int i = 0; i < MAX_FOOD; i++)
    {
        food[i].alive = false;
    }
    generateFood(food, MAX_FOOD);
}

void removeFood(Food food[], int index)
{
    if (index >= 0 && index < MAX_FOOD)
    {
        food[index].alive = false;
    }
}

// render methods
void DrawText(SDL_Renderer *renderer, const char *text, int x, int y, TTF_Font *font, Uint8 red, Uint8 green, Uint8 blue)
{
    SDL_Color color = {red, green, blue};
    SDL_Surface *textSurface = TTF_RenderText_Solid(font, text, color);
    SDL_Texture *textTexture = SDL_CreateTextureFromSurface(renderer, textSurface);
    SDL_Rect textRect;
    textRect.x = x;
    textRect.y = y;
    textRect.w = textSurface->w;
    textRect.h = textSurface->h;

    SDL_RenderCopy(renderer, textTexture, NULL, &textRect);

    SDL_DestroyTexture(textTexture);
    SDL_FreeSurface(textSurface);
}

void Ranking(SDL_Renderer *renderer, Player players[], int numPlayers, TTF_Font *font)
{
    int fontSize = 20;
    int padding = 10;
    int lineHeight = fontSize + padding;
    int initialX = padding;
    int initialY = padding;

    // Create a temporary array to hold the players
    Player sortedPlayers[numPlayers];
    memcpy(sortedPlayers, players, sizeof(Player) * numPlayers);

    // Sort the temporary array by radius in descending order
    for (int i = 0; i < numPlayers - 1; i++)
    {
        for (int j = 0; j < numPlayers - i - 1; j++)
        {
            if (sortedPlayers[j].radius < sortedPlayers[j + 1].radius)
            {
                Player temp = sortedPlayers[j];
                sortedPlayers[j] = sortedPlayers[j + 1];
                sortedPlayers[j + 1] = temp;
            }
        }
    }
    

    // Render the ranking
    char rankText[20];
    snprintf(rankText, sizeof(rankText), "Ranking");
    DrawText(renderer, rankText, initialX, initialY, font, 0, 0, 0);
    for (int i = 0; i < numPlayers; i++)
    {
        if (sortedPlayers[i].alive)
        {

            int rank = i + 1;
            snprintf(rankText, sizeof(rankText), "%d.", rank);

            char playerInfo[100];
            int radius = (int)sortedPlayers[i].radius;
            snprintf(playerInfo, sizeof(playerInfo), "%s: %d", sortedPlayers[i].username, radius);
            Uint8 red = 0;
            Uint8 green = 0;
            Uint8 blue = 0;
            if (i == 0)
            {
                red = 239;
                green = 184;
                blue = 16;
            }
            else if (i == 1)
            {
                red = 138;
                green = 149;
                blue = 151;
            }
            else if (i == 2)
            {
                red = 191;
                green = 137;
                blue = 112;
            }
            DrawText(renderer, rankText, initialX, initialY + (i + 1) * lineHeight, font, red, green, blue);
            DrawText(renderer, playerInfo, initialX + strlen(rankText) * fontSize, initialY + (i + 1) * lineHeight, font, red, green, blue);
        }
    }
}

void DrawCircle(SDL_Renderer *renderer, int centerX, int centerY, int radius, Uint8 red, Uint8 green, Uint8 blue)
{
    SDL_SetRenderDrawColor(renderer, red, green, blue, 255);

    for (int w = 0; w < radius * 2; ++w)
    {
        for (int h = 0; h < radius * 2; ++h)
        {
            int dx = radius - w; // horizontal offset
            int dy = radius - h; // vertical offset
            if ((dx * dx + dy * dy) <= (radius * radius))
            {
                SDL_RenderDrawPoint(renderer, centerX + dx, centerY + dy);
            }
        }
    }
}

void DrawPlayer(SDL_Renderer *renderer, Player players[], int myPlayerNum, int cameraX, int cameraY, float scale, TTF_Font *font)
{
    // Render the players with the camera offset
    for (int i = 0; i < MAX_PLAYERS; i++)
    {
        if (players[i].alive)
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
    for (int i = 0; i < MAX_PLAYERS; i++)
    {
        if (players[i].alive)
        {

            float renderX = (players[i].x - cameraX) * scale;
            float renderY = (players[i].y - cameraY) * scale;
            float renderRadius = players[i].radius * scale;
            int nameWidth, nameHeight;
            TTF_SizeText(font, players[i].username, &nameWidth, &nameHeight);
            int nameX = renderX - nameWidth / 2;
            int nameY = renderY + renderRadius + 10 * scale; // Adjust the vertical position based on scale

            if (i != myPlayerNum)
            {
                DrawText(renderer, players[i].username, nameX, nameY, font, 150, 0, 0);
            }
            else
            {
                DrawText(renderer, players[i].username, nameX, nameY, font, 0, 0, 150);
            }
        }
    }
}

void drawFood(SDL_Renderer *renderer, Food food[], int cameraX, int cameraY, float scale)
{
    for (int i = 0; i < MAX_FOOD; i++)
    {
        if (food[i].alive)
        {
            float renderX = (food[i].x - cameraX) * scale;
            float renderY = (food[i].y - cameraY) * scale;
            float renderRadius = FOOD_RADIUS * scale;
            DrawCircle(renderer, renderX, renderY, renderRadius, 0, 150, 0);
            DrawCircle(renderer, renderX, renderY, renderRadius * 0.80, 0, 255, 0);
        }
    }
}
// collision methods
void CollisionPlayers(Player players[])
{
    for (int i = 0; i < MAX_PLAYERS; i++)
    {
        if (players[i].alive)
        {
            for (int j = 0; j < MAX_PLAYERS; j++)
            {
                if (players[j].alive)
                {
                    float disX = fabs(players[i].x - players[j].x);
                    float disY = fabs(players[i].y - players[j].y);
                    float radiusSum = players[i].radius + players[j].radius;

                    if ((disX <= players[i].radius && disY <= players[j].radius) || disX <= players[j].radius && disY <= players[i].radius)
                    {
                        if (players[i].radius > players[j].radius && players[j].alive)
                        {
                            players[j].alive = false;
                            players[i].radius += players[j].radius / 2;
                        }
                    }
                }
            }
        }
    }
}

bool CollisionFood(Food food[], Player myPlayer, float scale)
{
    float scaledRadiusSum = (myPlayer.radius + FOOD_RADIUS) * scale;

    for (int i = 0; i < MAX_FOOD; i++)
    {
        if (food[i].alive)
        {
            float dx = myPlayer.x - food[i].x;
            float dy = myPlayer.y - food[i].y;
            float squaredDistance = dx * dx + dy * dy;
            float squaredRadiusSum = scaledRadiusSum * scaledRadiusSum;

            if (squaredDistance <= squaredRadiusSum)
            {
                removeFood(food, i);
                generateFood(food, 1);
                return true;
            }
        }
    }
    return false;
}

// camera method
void FollowPlayer(Player myPlayer, float *cameraX, float *cameraY, float *scale)
{
    // Adjust the camera position based on the player's position
    if (myPlayer.alive)
    {
        *cameraX = myPlayer.x - (WINDOW_WIDTH / 2) / (*scale);
        *cameraY = myPlayer.y - (WINDOW_HEIGHT / 2) / (*scale);
        *scale = INI_RADIUS / myPlayer.radius; // Adjust the scale based on player's radius
    }
}

int main(int argc, char *argv[])
{

    if (argc < 2)
    {
        printf("Please provide the IP address as a command-line argument.\n");
        return 1;
    }
    SDL_Init(SDL_INIT_VIDEO);
    TTF_Init();
    char windowTitle[100];
    sprintf(windowTitle, "agar.SDL - Server at %s", argv[1]);
    SDL_Window *window = SDL_CreateWindow(windowTitle, SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, WINDOW_WIDTH, WINDOW_HEIGHT, 0);
    SDL_Renderer *renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);

    TTF_Font *font = TTF_OpenFont("/usr/share/fonts/truetype/tlwg/TlwgTypo-Bold.ttf", 24);

    if (font == NULL)
    {
        printf("Failed to load font: %s\n", TTF_GetError());
        return 1;
    }

    Food food[MAX_FOOD];
    initializeFood(food);

    // esta parte quitarla cuando se meta en el servidor
    Player players[MAX_PLAYERS];
    initializePlayers(players);

    // generating the player
    int i = 0;
    while (players[i].alive)
    {
        i++;
    }
    int myPlayerNum = i;
    insertPlayer(players, WINDOW_WIDTH / 2, WINDOW_WIDTH / 2, INI_RADIUS, "yo");

    // inserting example enemies
    insertPlayer(players, WINDOW_WIDTH / 4, WINDOW_WIDTH / 4, 51, "malo 1");
    insertPlayer(players, WINDOW_WIDTH / 3, WINDOW_WIDTH / 3, 35, "malo 2");

    // Camera position and scale
    float cameraX = 0;
    float cameraY = 0;
    float scale = 1.5;

    // Main loop flag and event handler
    bool quit = false;
    SDL_Event event;

    // MAIN LOOP
    while (!quit)
    {

        while (SDL_PollEvent(&event))
        {
            if (event.type == SDL_QUIT)
            {
                quit = true;
            }
        }

        int mouseX, mouseY;
        SDL_GetMouseState(&mouseX, &mouseY);

        SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
        SDL_RenderClear(renderer);

        // // Circle follow cursor movement with lag
        // if (WINDOW_WIDTH / 2 > mouseX)
        //     players[myPlayerNum].x -= (WINDOW_WIDTH / 2 - mouseX) * LAG_FACTOR;
        // else if (WINDOW_WIDTH / 2 < mouseX)
        //     players[myPlayerNum].x += (mouseX - WINDOW_WIDTH / 2) * LAG_FACTOR;

        // if (WINDOW_HEIGHT / 2 > mouseY)
        //     players[myPlayerNum].y -= (WINDOW_HEIGHT / 2 - mouseY) * LAG_FACTOR;
        // else if (WINDOW_HEIGHT / 2 < mouseY)
        //     players[myPlayerNum].y += (mouseY - WINDOW_HEIGHT / 2) * LAG_FACTOR;
        
        
        MovePlayer(&players[myPlayerNum], mouseX, mouseY);
                

        drawFood(renderer, food, cameraX, cameraY, scale);

        FollowPlayer(players[myPlayerNum], &cameraX, &cameraY, &scale); // Adjust the camera position

        // esto hay que meterlo en el server
        CollisionPlayers(players);

        DrawPlayer(renderer, players, myPlayerNum, cameraX, cameraY, scale, font);

        size_t size = sizeof(players) / sizeof(players[0]);
        Ranking(renderer, players, size, font);

        if (CollisionFood(food, players[myPlayerNum], scale))
            players[myPlayerNum].radius += 2;

        SDL_RenderPresent(renderer);
    }
    // esto  igual hay que quitarlo
    TTF_CloseFont(font);

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
    return 0;
}
