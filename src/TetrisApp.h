//
// Created by pauli on 2/2/2023.
//

#ifndef MPTETRIS_TETRISAPP_H
#define MPTETRIS_TETRISAPP_H

#include "ui/menu/MenuState.h"
#include "ui/AssetHelper.h"
#include "ui/RenderHelper.h"
#include "SDL.h"
#include "SDL_ttf.h"
#include "SDL_image.h"
#include <iostream>
#include <string>
#include <vector>
#include <memory>

namespace Tetris {

    class App {
    public:
        /**
         * Constructor for the app. Calls initialize to set up SDL
         */
        App();

        /**
         * Destructor for the app. Cleans up all the SDL related stuff
         */
        ~App();

        /**
         * Function that runs the app.
         */
        void run();

        /**
         * Changes the current Menu that is being displayed/updated for the App.
         * @param newState The new MenuState to display/update
         */
        void changeState(const std::shared_ptr<MenuState> & newState);

        /**
         * Gets a texture that has been loaded by the app
         * @param name The name/id of the texture to get
         * @return Holder for the texture
         */
        TextureHolder getTexture(const std::string &name);

        /**
         * Gets a font from the loaded fonts
         * @param name The name/id of the font to get
         * @return Holder for the font
         */
        FontHolder getFont(const std::string &name);

        /**
         * Checks if a key is currently being pressed on the keyboard. These use the SDL_SCANCODE_* constants
         * @param scanCode The key to check for being pressed. Use the SDL_SCANCODE_* constants
         * @return True if the key is currently being pressed, false otherwise
         */
        bool isKeyPressed(int scanCode);

        /**
         * Constant variables
         */
        const static Uint32 RENDER_FLAGS = SDL_RENDERER_ACCELERATED;
        const static int WINDOW_WIDTH = 1000;
        const static int WINDOW_HEIGHT = 1000;
        constexpr static float ENTITY_SPEED = 300.0f;
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
        bool initialized, running;
        std::vector<TextureHolder> loadedTextures;
        std::vector<FontHolder> loadedFonts;
        std::shared_ptr<MenuState> currentState;

        // SDL Related variables
        SDL_Window *window;
        SDL_Renderer *renderer;
    };
}

#endif //MPTETRIS_TETRISAPP_H
