//Using SDL and standard IO
#include "SDL.h"
#include "SDL_image.h"
#include "SDL_ttf.h"
#include <iostream>
#include <chrono>

//Screen dimension constants
const int SCREEN_WIDTH = 640;
const int SCREEN_HEIGHT = 480;

const int TOTAL_TEXTURES = 7;
const int TEXTURE_SIZE = 512; // for both width and height

const int TICKS_PER_SECOND = 60;
const int SKIP_TICKS = 1000 / TICKS_PER_SECOND;
const int MAX_FRAMESKIP = 5;

int main(int argc, char *argv[]) {

    // returns zero on success else non-zero
    if (SDL_Init(SDL_INIT_EVERYTHING) != 0) {
        printf("error initializing SDL: %s\n", SDL_GetError());
    }

    if(TTF_Init() != 0) {
        std::cout << "error initializing SDL_ttf: " << TTF_GetError() << std::endl;
    }

    SDL_Window *win = SDL_CreateWindow("GAME", // creates a window
                                       SDL_WINDOWPOS_CENTERED,
                                       SDL_WINDOWPOS_CENTERED,
                                       1000, 1000, 0);

    // triggers the program that controls
    // your graphics hardware and sets flags
    Uint32 render_flags = SDL_RENDERER_ACCELERATED;

    // creates a renderer to render our images
    SDL_Renderer *rend = SDL_CreateRenderer(win, -1, render_flags);

    // creates a surface to load an image into the main memory
    SDL_Surface *surface;

    // please provide a path for your image
    surface = IMG_Load("../image.png");

    // loads image to our graphics hardware memory.
    SDL_Texture *tex = SDL_CreateTextureFromSurface(rend, surface);

    TTF_Font *font = TTF_OpenFont("../OpenSans.ttf", 24);
    if(font == NULL) {
        std::cout << "error loading font: " << TTF_GetError() << std::endl;
    }
    SDL_Color white = {255, 255, 255};
    SDL_Rect textRect = {0, 0, 25, 25};

    SDL_Surface * surfaceMessage = TTF_RenderText_Solid(font, "Test", white);
    SDL_Texture * messageText = SDL_CreateTextureFromSurface(rend, surfaceMessage);

    // clears main-memory
    SDL_FreeSurface(surface);
    SDL_FreeSurface(surfaceMessage);

    // let us control our image position
    // so that we can move it with our keyboard.
    int square = rand() % TOTAL_TEXTURES;

    SDL_Rect dest = {0, 0, TEXTURE_SIZE, TEXTURE_SIZE};
    SDL_Rect src = {square * TEXTURE_SIZE, 0, TEXTURE_SIZE, TEXTURE_SIZE};

    // adjust height and width of our image box.
    dest.w /= 6;
    dest.h /= 6;

    // sets initial x-position of object
    dest.x = (1000 - dest.w) / 2;

    // sets initial y-position of object
    dest.y = (1000 - dest.h) / 2;

    // controls animation loop
    int close = 0;

    // speed of box
    int speed = 300;

    Uint64 lastGameTick = SDL_GetTicks64();
    int frames = 0;
    Uint64 elapsed = 0;

    // animation loop
    while (!close) {
        Uint64 gameTick = SDL_GetTicks64();
        Uint64 delta = gameTick - lastGameTick;
        lastGameTick = gameTick;

        elapsed += delta;
        frames++;

        if(elapsed >= 1000) {
            std::cout << "FPS: " << frames << std::endl;
            frames = 0;
            elapsed = 0;
        }

        SDL_Event event;

        // Events management
        while (SDL_PollEvent(&event)) {
            switch (event.type) {

                case SDL_QUIT:
                    // handling of close button
                    close = 1;
                    break;

                case SDL_KEYDOWN: {
                    // keyboard API for key pressed
                    if (event.key.keysym.scancode == SDL_SCANCODE_W)
                        dest.y -= speed / 30;
                    if (event.key.keysym.scancode == SDL_SCANCODE_A)
                        dest.x -= speed / 30;
                    if (event.key.keysym.scancode == SDL_SCANCODE_S)
                        dest.y += speed / 30;
                    if (event.key.keysym.scancode == SDL_SCANCODE_D)
                        dest.x += speed / 30;
                }
            }
        }

        // right boundary
        if (dest.x + dest.w > 1000)
            dest.x = 1000 - dest.w;

        // left boundary
        if (dest.x < 0)
            dest.x = 0;

        // bottom boundary
        if (dest.y + dest.h > 1000)
            dest.y = 1000 - dest.h;

        // upper boundary
        if (dest.y < 0)
            dest.y = 0;

        // clears the screen
        SDL_RenderClear(rend);
        SDL_RenderCopy(rend, messageText, NULL, &textRect);
        SDL_RenderCopy(rend, tex, &src, &dest);

        // triggers the double buffers
        // for multiple rendering
        SDL_RenderPresent(rend);

        // calculates to 60 fps
        SDL_Delay(1000 / 60);
    }

    // destroy texture
    SDL_DestroyTexture(tex);

    // destroy renderer
    SDL_DestroyRenderer(rend);

    // destroy window
    SDL_DestroyWindow(win);

    // close SDL
    SDL_Quit();

    return 0;
}

