#include <SDL2/SDL.h>
#include <stdbool.h>
#include <math.h>
#include <stdio.h>

#define WINDOW_WIDTH 640
#define WINDOW_HEIGHT 480
#define CIRCLE_RADIUS 50
#define LAG_FACTOR 0.001

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

void PlayerCircle(SDL_Renderer* renderer, int x, int y) {
    DrawCircle(renderer, x, y, CIRCLE_RADIUS, 0, 0, 255);
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

    float circleX = WINDOW_WIDTH / 2;  // Initial X position of the circle
    float circleY = WINDOW_HEIGHT / 2; // Initial Y position of the circle

    // Main loop flag and event handler
    bool quit = false;
    SDL_Event event;

    // Mouse position
    float mouseX = 0;
    float mouseY = 0;

    // MAIN LOOP
    while (!quit) {
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) {
                quit = true;
            } else if (event.type == SDL_MOUSEMOTION) {
                mouseX = event.motion.x;
                mouseY = event.motion.y;
            }
        }

        SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
        SDL_RenderClear(renderer);

        // Circle follow cursor movement with lag
        if (circleX > mouseX)
            circleX -= (circleX - mouseX) * LAG_FACTOR;
        else if (circleX < mouseX)
            circleX += (mouseX - circleX) * LAG_FACTOR;

        if (circleY > mouseY)
            circleY -= (circleY - mouseY) * LAG_FACTOR;
        else if (circleY < mouseY)
            circleY += (mouseY - circleY) * LAG_FACTOR;

        PlayerCircle(renderer, circleX, circleY);

        SDL_RenderPresent(renderer);
    }

    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}
