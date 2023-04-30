//
// Created by pauli on 4/25/2023.
//

#include "HostGameMenuState.h"
#include "JoinGameMenuState.h"
#include "GameMenuStateMP.h"
#include "../../TetrisApp.h"

namespace Tetris {

    GameMenuStateMP::GameMenuStateMP(App *app) : MenuState(app) {

    }

    void GameMenuStateMP::loadComponents() {
        MenuState::loadComponents();

        // Ensure this is updated in case the user is already holding return
        lastReturnPress = app->isKeyPressed(SDL_SCANCODE_RETURN);

        std::shared_ptr<ButtonComponent> button = std::make_shared<ButtonComponent>(this->app, "host_button",
                                                                                    "Host Game",
                                                                                    App::WINDOW_WIDTH / 2,
                                                                                    App::WINDOW_HEIGHT / 2, 153, 56, true);
        buttons.push_back(button);

        button->onClick([this]() {
            std::shared_ptr<MenuState> nextState = std::make_shared<HostGameMenuState>(app);
            nextState->loadComponents();
            app->changeState(nextState);
        });

        button = std::make_shared<ButtonComponent>(this->app, "join_button", "Join Game", App::WINDOW_WIDTH / 2,
                                                   App::WINDOW_HEIGHT / 2 + 100, 153, 56, true);
        button->onClick([this]() {
            std::shared_ptr<MenuState> nextState = std::make_shared<JoinGameMenuState>(app);
            nextState->loadComponents();
            app->changeState(nextState);
        });

        buttons.push_back(button);
    }

    void GameMenuStateMP::update(float ts) {
        MenuState::update(ts);

        // Check and determine which button is currently being selected
        bool isRetPressed = app->isKeyPressed(SDL_SCANCODE_RETURN);
        bool wasRetPressed = this->lastReturnPress;
        this->lastReturnPress = isRetPressed;

        bool isPressed = app->isKeyPressed(SDL_SCANCODE_DOWN) || app->isKeyPressed(SDL_SCANCODE_UP);
        bool wasPressed = this->lastDownPress;
        this->lastDownPress = isPressed;

        // cycle between menu buttons
        if (app->isKeyPressed(SDL_SCANCODE_DOWN) && !wasPressed) {
            selectedButton = (selectedButton + 1) % buttons.size();
        } else if (app->isKeyPressed(SDL_SCANCODE_UP) && !wasPressed) {
            selectedButton = (selectedButton - 1) % buttons.size();
        }

        // Determine if the currently selected button is being pressed
        if (isRetPressed && !wasRetPressed) {
            std::shared_ptr<ButtonComponent> selected = buttons[selectedButton];
            selected->click();
        }
    }

    void GameMenuStateMP::render(SDL_Renderer *renderer, float ts) {
        MenuState::render(renderer, ts);

        // render all buttons
        for (auto &button: buttons)
            button->render(renderer, ts);

        // load arrow holder
        TextureHolder holder = app->getTexture("arrow");
        SDL_Rect src = {0, 0, holder.width, holder.height};

        // define some shit
        float arrowDiff = 25.0f;
        std::shared_ptr<ButtonComponent> selected = buttons[selectedButton];
        float arrowX = selected->getX() - selected->getWidth();
        float arrowY = selected->getY() - (selected->getHeight() / 2) - (arrowDiff / 2);

        SDL_FRect dst = {arrowX, arrowY, arrowDiff, arrowDiff};
        renderTexture(renderer, holder.texture, &src, &dst);
    }

} // Tetris