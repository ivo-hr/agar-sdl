#include <SDL2/SDL.h>
#include <stdbool.h>
#include <math.h>
#include <stdio.h>

#define WINDOW_WIDTH 640
#define WINDOW_HEIGHT 480
#define INI_RADIUS 50
#define LAG_FACTOR 0.001

typedef struct {
    float x, y;
    int radius;
    bool alive;
} player;
player players[11];
//number of the player of this machine
int playerNumber;

void insertPlayer(int x, int y, int radius) {
    int i = 0;
    while (players[i].alive)
        i++;
    players[i].x = x;
    players[i].y = y;
    players[i].radius = radius;
    players[i].alive = true;
}

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

void PlayerCircle(SDL_Renderer* renderer) {
    //update enemies pos and circle radius
    int i = 0;
    while (players[i].alive) {
        if (i != playerNumber)
            DrawCircle(renderer, players[i].x, players[i].y, players[i].radius, 255, 0, 0);
        else
            DrawCircle(renderer, players[i].x, players[i].y, players[i].radius, 0, 0, 255);

        i++;
    }
}

void collitions() {
    int i = 0;
    while (players[i].alive) {
        if (i != playerNumber) {
            if (players[i].x > players[playerNumber].x - players[playerNumber].radius && players[i].x < players[playerNumber].x + players[playerNumber].radius)
                if (players[i].y > players[playerNumber].y - players[playerNumber].radius && players[i].y < players[playerNumber].y + players[playerNumber].radius)
                    printf("collision ");
        }
        i++;
    }
}

void FollowPlayer(float* cameraX, float* cameraY) {
    // Adjust the camera position based on the player's position
    *cameraX = players[playerNumber].x - WINDOW_WIDTH / 2;
    *cameraY = players[playerNumber].y - WINDOW_HEIGHT / 2;
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

    //generating the player
    int i = 0;
    while (players[i].alive) {
        i++;
    }
    playerNumber = i;
    insertPlayer(WINDOW_WIDTH / 2, WINDOW_WIDTH / 2, INI_RADIUS);

    //inserting example enemies
    insertPlayer(WINDOW_WIDTH / 3, WINDOW_WIDTH / 5, 60);
    insertPlayer(WINDOW_WIDTH / 5, WINDOW_WIDTH / 5, 20);

    // Camera position
    float cameraX = 0;
    float cameraY = 0;

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
            players[playerNumber].x -= (WINDOW_WIDTH / 2 - mouseX) * LAG_FACTOR;
        else if (WINDOW_WIDTH / 2 < mouseX)
            players[playerNumber].x += (mouseX - WINDOW_WIDTH / 2) * LAG_FACTOR;

        if (WINDOW_HEIGHT/ 2 > mouseY)
            players[playerNumber].y -= (WINDOW_HEIGHT/ 2 - mouseY) * LAG_FACTOR;
        else if (WINDOW_HEIGHT/ 2  < mouseY)
            players[playerNumber].y += (mouseY - WINDOW_HEIGHT/ 2 ) * LAG_FACTOR;

        FollowPlayer(&cameraX, &cameraY); // Adjust the camera position

        // Render the players with the camera offset
        int i = 0;
        while (players[i].alive) {
            float renderX = players[i].x - cameraX;
            float renderY = players[i].y - cameraY;

            if (i != playerNumber)
                DrawCircle(renderer, renderX, renderY, players[i].radius, 255, 0, 0);
            else
                DrawCircle(renderer, renderX, renderY, players[i].radius, 0, 0, 255);

            i++;
        }

        SDL_RenderPresent(renderer);
    }

    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}
