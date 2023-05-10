//
// Created by pauli on 4/25/2023.
//

#ifndef MPTETRIS_GAMEMENUSTATEMP_H
#define MPTETRIS_GAMEMENUSTATEMP_H

#include "MenuState.h"

namespace Tetris {

    /**
     * Menu state representing once we press Multiplayer in the main menu. Will give the player the option to host or join a game
     */
    class GameMenuStateMP : public MenuState {
    public:
        explicit GameMenuStateMP(App *app);
        void loadComponents() override;
        void update(float ts) override;
        void render(SDL_Renderer *renderer, float ts) override;
    private:
        /**
         * Determines the last arrow key and enter press the user did
         */
        bool lastDownPress = false, lastReturnPress = false;

        /**
         * Holds all the buttons stored
         */
        std::vector<std::shared_ptr<ButtonComponent>> buttons;

        /**
         * What button is currently selected?
         */
        int selectedButton = 0;
    };

} // Tetris

#endif //MPTETRIS_GAMEMENUSTATEMP_H
