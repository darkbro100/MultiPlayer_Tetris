//
// Created by pauli on 2/2/2023.
//

#include "TetrisApp.h"

namespace Tetris {
    App::App() {
        window = nullptr;
        renderer = nullptr;
        initialized = false;
        running = false;

        initialize();
    }

    App::~App() {
        // destroy renderer
        SDL_DestroyRenderer(renderer);

        // destroy textures
        for (const TextureHolder &holder: loadedTextures)
            SDL_DestroyTexture(holder.texture);
        loadedTextures.clear();

        // destroy window
        SDL_DestroyWindow(window);

        // close SDL
        SDL_Quit();
    }

    void App::initialize() {
        // returns zero on success else non-zero
        if (SDL_Init(SDL_INIT_EVERYTHING) != 0) {
            printf("error initializing SDL: %s\n", SDL_GetError());
            this->initialized = false;
        }

        if (TTF_Init() != 0) {
            std::cout << "error initializing SDL_ttf: " << TTF_GetError() << std::endl;
            this->initialized = false;
        }

        this->initialized = true;
    }

    void App::loadAssets() {
        // Load block texture
        TextureHolder holder = loadTexture("./assets/image.png", "tetronimo", renderer);
        if (!holder.texture)
            this->initialized = false;
        loadedTextures.push_back(holder);

        // Load main font
        FontHolder fontHolder = loadFont("./assets/OpenSans.ttf", "opensans", 18);
        if (!fontHolder.font)
            this->initialized = false;
        loadedFonts.push_back(fontHolder);

        // Load larger font
        FontHolder largeFontHolder = loadFont("./assets/OpenSans.ttf", "opensans_large", 36);
        if (!largeFontHolder.font)
            this->initialized = false;
        loadedFonts.push_back(largeFontHolder);

        // Load textures for buttons & arrow
        holder = loadTexture("./assets/arrow.png", "arrow", renderer);
        if (!holder.texture)
            this->initialized = false;

        loadedTextures.push_back(holder);

        holder = loadTexture("./assets/button_start.png", "start_button", renderer);
        if (!holder.texture)
            this->initialized = false;

        loadedTextures.push_back(holder);

        holder = loadTexture("./assets/button_multiplayer.png", "mp_button", renderer);
        if (!holder.texture)
            this->initialized = false;

        loadedTextures.push_back(holder);

        // Load main menu state, set that as the current state
        currentState = std::make_shared<MainMenuState>(this);
        currentState->loadComponents();
    }

    void App::run() {
        if (!initialized) {
            std::cout << "Could not initialize App" << std::endl;
            return;
        }

        // creates a window
        window = SDL_CreateWindow("GAME",
                                  SDL_WINDOWPOS_CENTERED,
                                  SDL_WINDOWPOS_CENTERED,
                                  WINDOW_WIDTH, WINDOW_HEIGHT, 0);

        // creates a renderer to render our images
        renderer = SDL_CreateRenderer(window, -1, RENDER_FLAGS);

        // Load textures in
        loadAssets();

        running = initialized; //reassign back to initialized, loadAssets may have set it back to false if a texture failed to load

        while (running) {
            // Calculate timings
            Uint64 now = SDL_GetPerformanceCounter();
            Uint64 last = lastFrame;
            lastFrame = now;
            float timestep = ((float(now - last)) / (float) SDL_GetPerformanceFrequency());

            // Events management
            SDL_Event event;
            while (SDL_PollEvent(&event)) {
                switch (event.type) {

                    case SDL_QUIT: {
                        // handling of close button
                        running = false;
                        break;
                    }

                    case SDL_KEYDOWN: {

                        // handle copy, paste, and backspace
                        if (event.key.keysym.scancode == SDL_SCANCODE_BACKSPACE && inputText.length() > 0) {
                            inputText.pop_back();
                        } else if (event.key.keysym.sym == SDLK_v && SDL_GetModState() & KMOD_CTRL) {
                            inputText = SDL_GetClipboardText();
                        }

                        // keyboard API for key pressed
                        keys[event.key.keysym.scancode] = true;
                        break;
                    }

                    case SDL_KEYUP: {
                        // keyboard API for key release
                        keys[event.key.keysym.scancode] = false;
                        break;
                    }

                    case SDL_TEXTINPUT: {

                        //Not copy or pasting
                        if (!(SDL_GetModState() & KMOD_CTRL &&
                              (event.text.text[0] == 'c' || event.text.text[0] == 'C' || event.text.text[0] == 'v' ||
                               event.text.text[0] == 'V'))) {
                            //Append character
                            inputText += event.text.text;
                        }
                        break;
                    }
                }
            }
            // Update the state of the app, and then render it
            onUpdate(timestep);

            // clears the screen
            SDL_SetRenderDrawColor(renderer, 0, 0, 0, 0);
            SDL_RenderClear(renderer);

            // render the app
            onRender(timestep);

            // Double buffering, present the renderer
            SDL_RenderPresent(renderer);
        }
    }

    void App::onUpdate(float timestep) {
        currentState->update(timestep);
    }

    void App::onRender(float timestep) {
        currentState->render(renderer, timestep);
    }

    TextureHolder App::getTexture(const std::string &name) {
        for (auto holder: loadedTextures) {
            if (holder.name == name)
                return holder;
        }

        return {};
    }

    FontHolder App::getFont(const std::string &name) {
        for (auto holder: loadedFonts) {
            if (holder.name == name)
                return holder;
        }

        return {};
    }

    bool App::isKeyPressed(int scanCode) {
        return keys[scanCode];
    }

    void App::changeState(const std::shared_ptr<MenuState> &newState) {
        this->currentState = newState;
    }

    void App::resize(int width, int height) {
        SDL_SetWindowPosition(window, 0, 0);
        SDL_SetWindowSize(window, width, height);
    }

    void App::maxWindowSize(int &w, int &h) {
        SDL_DisplayMode dm;
        SDL_GetDesktopDisplayMode(0, &dm);
        w = dm.w;
        h = dm.h;
    }

    SDL_Window *App::getWindow() {
        return window;
    }

    const std::string &App::getInputText() {
        return inputText;
    }
}