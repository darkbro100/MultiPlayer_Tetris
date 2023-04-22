//
// Created by pauli on 2/6/2023.
//

#ifndef MPTETRIS_GAMEMENUSTATESP_H
#define MPTETRIS_GAMEMENUSTATESP_H

#include "Tetronimo.h"
#include "MenuState.h"
#include <vector>
#include <memory>

namespace Tetris {

    // Forward Declare App Class
    class App;

    class GameMenuStateSP : public MenuState {
    public:
        const static int FIELD_WIDTH = 10;
        const static int FIELD_HEIGHT = 20;
        const static int BOUNDARY_ID = 8;
        const static int EMPTY_ID = 0;

        explicit GameMenuStateSP(App *app);
        void loadComponents() override;
        void update(float ts) override;
        void render(SDL_Renderer *renderer, float ts) override;
    private:
        unsigned int * field;
        int currentPiece;
        int currentRotation;
        int currentX, currentY;

        bool test;
    };

}

#endif //MPTETRIS_GAMEMENUSTATESP_H
