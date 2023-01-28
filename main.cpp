//Using SDL and standard IO
#include "SDL2-2.26.2/include/SDL.h"
#include "SDL.h"
#include <iostream>

//Screen dimension constants
const int SCREEN_WIDTH = 640;
const int SCREEN_HEIGHT = 480;

const int TICKS_PER_SECOND = 60;
const int SKIP_TICKS = 1000 / TICKS_PER_SECOND;
const int MAX_FRAMESKIP = 5;

int main(int argc, char* argv[]) {
    if (SDL_Init(SDL_INIT_VIDEO) != 0) {
        // SDL initialization failed
        return 1;
    }

    SDL_Window* window = SDL_CreateWindow("Simple Game Loop", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, 800, 600, 0);
    if (!window) {
        // Window creation failed
        SDL_Quit();
        return 1;
    }

    bool running = true;
    SDL_Event event;
    while (running) {
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) {
                running = false;
            }
        }
        // update game logic here
        // render game here
    }

    SDL_DestroyWindow(window);
    SDL_Quit();
    return 0;
}

