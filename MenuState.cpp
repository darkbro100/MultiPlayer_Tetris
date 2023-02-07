//
// Created by pauli on 2/6/2023.
//

#include "MenuState.h"
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

        std::shared_ptr<AppComponent> entityComponent = std::make_shared<ExampleEntity>(this->app, "entity", 0, 0, ExampleEntity::TEXTURE_SIZE / 6, ExampleEntity::TEXTURE_SIZE / 6);
        components.push_back(entityComponent);
    }

    void MainMenuState::update(float ts) {
        MenuState::update(ts);
    }

    void MainMenuState::render(SDL_Renderer *renderer, float ts) {
        MenuState::render(renderer, ts);
    }
}
