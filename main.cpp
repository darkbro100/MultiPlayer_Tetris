//Using SDL and standard IO
#include "SDL.h"
#include "SDL_image.h"
#include "SDL_ttf.h"
#include <iostream>
#include <chrono>
#include <string>

//Screen dimension constants
const int SCREEN_WIDTH = 640;
const int SCREEN_HEIGHT = 480;

const int TOTAL_TEXTURES = 7;
const int TEXTURE_SIZE = 512; // for both width and height

const int TICKS_PER_SECOND = 60;
const int SKIP_TICKS = 1000 / TICKS_PER_SECOND;
const int MAX_FRAMESKIP = 5;

void renderText(SDL_Renderer * renderer, SDL_Rect dest, TTF_Font * font, std::string text) {
    SDL_Color fg = { 255, 255, 255 };
    SDL_Surface* surf = TTF_RenderText_Solid(font, text.c_str(), fg);

    dest.w = surf->w;
    dest.h = surf->h;

    SDL_Texture* tex = SDL_CreateTextureFromSurface(renderer, surf);

    SDL_RenderCopy(renderer, tex, NULL, &dest);
    SDL_DestroyTexture(tex);
    SDL_FreeSurface(surf);
}

int main(int argc, char *argv[]) {

    bool keys[SDL_NUM_SCANCODES] = {false};

    // returns zero on success else non-zero
    if (SDL_Init(SDL_INIT_EVERYTHING) != 0) {
        printf("error initializing SDL: %s\n", SDL_GetError());
    }

    if (TTF_Init() != 0) {
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

    TTF_Font *font = TTF_OpenFont("../OpenSans.ttf", 36);
    if (font == NULL) {
        std::cout << "error loading font: " << TTF_GetError() << std::endl;
    }

    SDL_Rect textRect = {0, 0, 100, 75};

    // let us control our image position
    // so that we can move it with our keyboard.
    int square = rand() % TOTAL_TEXTURES;

    SDL_FRect dest = {0, 0, TEXTURE_SIZE, TEXTURE_SIZE};
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
    float speed = 300.0f;

    Uint64 now = SDL_GetPerformanceCounter();
    Uint64 last;
    float timestep;

    // animation loop
    while (!close) {
        // calculate timesteps
        last = now;
        now = SDL_GetPerformanceCounter();
        timestep = ((float(now - last)) / (float) SDL_GetPerformanceFrequency());

        // for debug
        float fps = 1.0f / timestep;

        // Events management
        SDL_Event event;
        while (SDL_PollEvent(&event)) {
            switch (event.type) {

                case SDL_QUIT: {
                    // handling of close button
                    close = 1;
                    break;
                }

                case SDL_KEYDOWN: {
                    // keyboard API for key pressed
                    keys[event.key.keysym.scancode] = true;
                    break;
                }

                case SDL_KEYUP: {
                    // keyboard API for key release
                    keys[event.key.keysym.scancode] = false;
                    break;
                }
            }
        }

        // move the entity
        if (keys[SDL_SCANCODE_W] || keys[SDL_SCANCODE_UP])
            dest.y -= speed * timestep;
        if (keys[SDL_SCANCODE_A] || keys[SDL_SCANCODE_LEFT])
            dest.x -= speed * timestep;
        if (keys[SDL_SCANCODE_S] || keys[SDL_SCANCODE_DOWN])
            dest.y += speed * timestep;
        if (keys[SDL_SCANCODE_D] || keys[SDL_SCANCODE_RIGHT])
            dest.x += speed * timestep;

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

        // draw back to the screen
        SDL_RenderCopyF(rend, tex, &src, &dest);
        renderText(rend, textRect, font, "FPS: " + std::to_string(fps));

        // triggers the double buffers
        // for multiple rendering
        SDL_RenderPresent(rend);
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

