//
// Created by pauli on 4/25/2023.
//

#include "GameMenuStateMP.h"

namespace Tetris {

    GameMenuStateMP::GameMenuStateMP(App *app) : MenuState(app) {

    }

    void GameMenuStateMP::loadComponents() {
        MenuState::loadComponents();

    }

    void GameMenuStateMP::update(float ts) {
        MenuState::update(ts);
    }

    void GameMenuStateMP::render(SDL_Renderer *renderer, float ts) {
        MenuState::render(renderer, ts);
    }

} // Tetris