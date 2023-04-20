//
// Created by pauli on 2/6/2023.
//

#include "MenuState.h"
#include "TetrisApp.h"
#include <iostream>

namespace Tetris {
    MenuState::MenuState(App *app) : app(app) {}

    MainMenuState::MainMenuState(App *app) : MenuState(app) {}

    void MenuState::update(float ts) {
        // update components
        for (const std::shared_ptr<AppComponent> &component: components) {
            std::shared_ptr<UpdatingComponent> updatingComponent = std::dynamic_pointer_cast<UpdatingComponent>(
                    component);
            if (updatingComponent)
                updatingComponent->update(ts);
        }
    }

    void MenuState::render(SDL_Renderer *renderer, float ts) {
        // render components
        for (const auto &component: components) {
            if (!component->isEnabled())
                continue;

            component->render(renderer, ts);
        }
    }

    void MenuState::loadComponents() {
        std::shared_ptr<AppComponent> fpsPtr = std::make_shared<FPSComponent>(this->app, "fps", 0, 0, 200, 50);
        components.push_back(fpsPtr);
    }

    void MainMenuState::loadComponents() {
        MenuState::loadComponents();

        std::shared_ptr<ButtonComponent> button = std::make_shared<ButtonComponent>(this->app, "start_button",
                                                                                    "start_button",
                                                                                    App::WINDOW_WIDTH / 2,
                                                                                    App::WINDOW_HEIGHT / 2, 153, 56);
        button->onClick([&]() {
            std::cout << "Start button clicked" << std::endl;
//            this->app->changeState(App::State::GAME);
        });

        buttons.push_back(button);

        button = std::make_shared<ButtonComponent>(this->app, "mp_button", "mp_button", App::WINDOW_WIDTH / 2,
                                                   App::WINDOW_HEIGHT / 2 + 100, 153, 56);
        button->onClick([&]() {
            std::cout << "MP button clicked" << std::endl;
            std::shared_ptr<MenuState> nextState = std::make_shared<TestMenuState>(app);
            nextState->loadComponents();
            app->changeState(nextState);
        });

        buttons.push_back(button);
    }

    void MainMenuState::update(float ts) {
        MenuState::update(ts);

        // Check and determine which button is currently being selected
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
        if (app->isKeyPressed(SDL_SCANCODE_RETURN)) {
            std::shared_ptr<ButtonComponent> selected = buttons[selectedButton];
            selected->click();
        }
    }

    void MainMenuState::render(SDL_Renderer *renderer, float ts) {
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

    TestMenuState::TestMenuState(App *app) : MenuState(app) {

    }

    void TestMenuState::loadComponents() {
        MenuState::loadComponents();
    }

    void TestMenuState::update(float ts) {
        MenuState::update(ts);
    }

    void TestMenuState::render(SDL_Renderer *renderer, float ts) {
        MenuState::render(renderer, ts);
    }
}
