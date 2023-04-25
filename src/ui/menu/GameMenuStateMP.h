//
// Created by pauli on 4/25/2023.
//

#ifndef MPTETRIS_GAMEMENUSTATEMP_H
#define MPTETRIS_GAMEMENUSTATEMP_H

#include "MenuState.h"

namespace Tetris {

    class GameMenuStateMP : public MenuState {
    public:
        explicit GameMenuStateMP(App *app);
        void loadComponents() override;
        void update(float ts) override;
        void render(SDL_Renderer *renderer, float ts) override;
    };

} // Tetris

#endif //MPTETRIS_GAMEMENUSTATEMP_H
