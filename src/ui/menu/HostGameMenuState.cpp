//
// Created by pauli on 4/30/2023.
//

#include "../../TetrisApp.h"
#include "HostGameMenuState.h"

namespace Tetris {

    HostGameMenuState::HostGameMenuState(App *app) : server(25000), MenuState(app) {
    }

    void HostGameMenuState::loadComponents() {
        MenuState::loadComponents();

        // Ensure this is updated in case the user is already holding return
        lastReturnPress = app->isKeyPressed(SDL_SCANCODE_RETURN);

        // Start the server
        server.start();
    }

    void HostGameMenuState::update(float ts) {
        MenuState::update(ts);
        server.update(true);

        // Check and determine which button is currently being selected
        bool isRetPressed = app->isKeyPressed(SDL_SCANCODE_RETURN);
        bool wasRetPressed = this->lastReturnPress;
        this->lastReturnPress = isRetPressed;

        bool isPressed = app->isKeyPressed(SDL_SCANCODE_DOWN) || app->isKeyPressed(SDL_SCANCODE_UP);
        bool wasPressed = this->lastDownPress;
        this->lastDownPress = isPressed;

        // cycle between menu buttons
//        if (app->isKeyPressed(SDL_SCANCODE_DOWN) && !wasPressed) {
//            selectedButton = (selectedButton + 1) % buttons.size();
//        } else if (app->isKeyPressed(SDL_SCANCODE_UP) && !wasPressed) {
//            selectedButton = (selectedButton - 1) % buttons.size();
//        }
    }

    void HostGameMenuState::render(SDL_Renderer *renderer, float ts) {
        MenuState::render(renderer, ts);
    }

} // Tetris