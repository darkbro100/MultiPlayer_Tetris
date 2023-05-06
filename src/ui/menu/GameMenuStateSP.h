//
// Created by pauli on 2/6/2023.
//

#ifndef MPTETRIS_GAMEMENUSTATESP_H
#define MPTETRIS_GAMEMENUSTATESP_H

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
        /**
         * All the code for checking all the input related things is put here because it is a lot of code
         */
        void checkInputs();

        TextureHolder holder;
        SinglePlayer player;

        std::mt19937 engine;

        int leftDelay, rightDelay, downDelay;

        bool wasStorePressed = false;
        bool wasInstantPressed = false;
        bool wasRotatePressed = false;
        bool wasLeftPressed = false, wasRightPressed = false, wasDownPressed = false;
        std::map<int, Uint64> lastPress;
    };

}

#endif //MPTETRIS_GAMEMENUSTATESP_H
