//
// Created by pauli on 2/6/2023.
//

#ifndef MPTETRIS_GAMEMENUSTATESP_H
#define MPTETRIS_GAMEMENUSTATESP_H

#include "../../game/Tetronimo.h"
#include "../../game/Tetronimo.h"
#include "MenuState.h"
#include <vector>
#include <memory>
#include <map>
#include <random>

namespace Tetris {

    // Pieces initially drop every half second.
    // Need to adjust how the tetrominos initially spawn
    // Game board is too small?

    // Forward Declare App Class
    class App;

    class GameMenuStateSP : public MenuState {
    public:
        explicit GameMenuStateSP(App *app);
        void loadComponents() override;
        void update(float ts) override;
        void render(SDL_Renderer *renderer, float ts) override;
    private:
        InputHolder inputs{};
        TextureHolder holder;
        SinglePlayer player;

        std::mt19937 engine;
    };

}

#endif //MPTETRIS_GAMEMENUSTATESP_H
