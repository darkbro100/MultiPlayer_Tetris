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

    /**
     * Represents the Menu relating to our SinglePlayer game. In terms of declarations, there is not too much to do since we just derive from our MenuState class
     */
    class GameMenuStateSP : public MenuState {
    public:
        explicit GameMenuStateSP(App *app);
        void loadComponents() override;
        void update(float ts) override;
        void render(SDL_Renderer *renderer, float ts) override;
    private:
        /**
         * Input Holder, contains what we've pressed during the game
         */
        InputHolder inputs{};

        /**
         * The TextureHolder for our tetromino block
         */
        TextureHolder holder;

        /**
         * SinglePlayer Game Struct
         */
        SinglePlayer player;

        /**
         * PRNG State
         */
        std::mt19937 engine;
    };

}

#endif //MPTETRIS_GAMEMENUSTATESP_H
