//
// Created by pauli on 2/2/2023.
//

#ifndef MPTETRIS_TETRISAPP_H
#define MPTETRIS_TETRISAPP_H

#include "AssetHelper.h"
#include "RenderHelper.h"
#include "SDL.h"
#include "SDL_ttf.h"
#include "SDL_image.h"
#include <iostream>
#include <string>
#include <vector>
#include <memory>

namespace Tetris {

    // Forward declare here
    class AppComponent;

    class App {
    public:
        App();
        ~App();

        void run();
        TextureHolder getTexture(const std::string & name);
        FontHolder getFont(const std::string & name);
        bool isKeyPressed(int scanCode);
    private:
        void initialize();
        void loadAssets();

        void onRender(float timestep);
        void onUpdate(float timestep);

        // Timings control
        Uint64 lastFrame;

        // Input control
        bool keys[SDL_NUM_SCANCODES]{false};

        // App control
        SDL_FRect entityPos = {0, 0, 512 / 6, 512 / 6};
        bool initialized, running;
        std::vector<TextureHolder> loadedTextures;
        std::vector<FontHolder> loadedFonts;
        std::vector<std::shared_ptr<AppComponent>> components;

        // SDL Related variables
        SDL_Window * window;
        SDL_Renderer * renderer;
    };

    static Uint32 RENDER_FLAGS = SDL_RENDERER_ACCELERATED;
    static int WIDTH = 1000;
    static int HEIGHT = 1000;
    static float SPEED = 300.0f;
}

#endif //MPTETRIS_TETRISAPP_H
