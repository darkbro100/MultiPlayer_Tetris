//
// Created by pauli on 2/2/2023.
//

#include "TetrisApp.h"

#include <memory>

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
        for (const TextureHolder& holder : loadedTextures)
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
        TextureHolder holder = loadTexture("../image.png", "tetronimo", renderer);
        if (holder.texture)
            loadedTextures.push_back(holder);
        else
            this->initialized = false;

        FontHolder fontHolder = loadFont("../OpenSans.ttf", "opensans", 24);
        if (fontHolder.font)
            loadedFonts.push_back(fontHolder);
        else
            this->initialized = false;

        std::shared_ptr<AppComponent> fpsPtr = std::make_shared<FPSComponent>(this, "fps", 0, 0, 200, 75);
        components.push_back(fpsPtr);
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
                                  WIDTH, HEIGHT, 0);

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
            // Update the state of the app, and then render it
            onUpdate(timestep);

            // clears the screen
            SDL_RenderClear(renderer);

            // render the app
            onRender(timestep);

            // Double buffering, present the renderer
            SDL_RenderPresent(renderer);
        }
    }

    void App::onUpdate(float timestep) {

        // move the entity
        if (keys[SDL_SCANCODE_W] || keys[SDL_SCANCODE_UP])
            entityPos.y -= SPEED * timestep;
        if (keys[SDL_SCANCODE_A] || keys[SDL_SCANCODE_LEFT])
            entityPos.x -= SPEED * timestep;
        if (keys[SDL_SCANCODE_S] || keys[SDL_SCANCODE_DOWN])
            entityPos.y += SPEED * timestep;
        if (keys[SDL_SCANCODE_D] || keys[SDL_SCANCODE_RIGHT])
            entityPos.x += SPEED * timestep;

        // right boundary
        if (entityPos.x + entityPos.w > 1000)
            entityPos.x = 1000 - entityPos.w;

        // left boundary
        if (entityPos.x < 0)
            entityPos.x = 0;

        // bottom boundary
        if (entityPos.y + entityPos.h > 1000)
            entityPos.y = 1000 - entityPos.h;

        // upper boundary
        if (entityPos.y < 0)
            entityPos.y = 0;

        // update components
        for(const std::shared_ptr<AppComponent>& component : components) {
            std::shared_ptr<UpdatingComponent> updatingComponent = std::dynamic_pointer_cast<UpdatingComponent>(component);
            if(updatingComponent)
                updatingComponent->update(timestep);
        }
    }

    void App::onRender(float timestep) {
        TextureHolder holder = loadedTextures[0];
        int color = lastFrame % 7;

        SDL_Texture *texture = holder.texture;
        SDL_Rect src = {color * 512, 0, 512, 512};

//        renderText(renderer, loadedFonts[0].font, fps, {255, 255, 255, 255}, {0, 0, 200, 48});
        SDL_RenderCopyF(renderer, texture, &src, &entityPos);

        // render components
        for(const auto& component : components)
            component->render(renderer, timestep);
    }

    TextureHolder App::getTexture(const std::string & name) {
        for(auto holder : loadedTextures) {
            if(holder.name == name)
                return holder;
        }

        return {};
    }

    FontHolder App::getFont(const std::string &name) {
        for(auto holder : loadedFonts) {
            if(holder.name == name)
                return holder;
        }

        return {};
    }
}