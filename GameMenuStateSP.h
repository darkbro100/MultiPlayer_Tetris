//
// Created by pauli on 2/6/2023.
//

#ifndef MPTETRIS_GAMEMENUSTATESP_H
#define MPTETRIS_GAMEMENUSTATESP_H

#include "Tetronimo.h"
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
        const static int INPUT_DELAY = 75;
        const static int FIELD_WIDTH = 12;
        const static int FIELD_HEIGHT = 21;
        const static int BOUNDARY_ID = 8;
        const static int EMPTY_ID = 0;

        const static int ROTATE_KEY = SDL_SCANCODE_UP;
        const static int LEFT_KEY = SDL_SCANCODE_LEFT;
        const static int RIGHT_KEY = SDL_SCANCODE_RIGHT;
        const static int DOWN_KEY = SDL_SCANCODE_DOWN;
        const static int INSTANT_DROP_KEY = SDL_SCANCODE_SPACE;

        explicit GameMenuStateSP(App *app);
        void loadComponents() override;
        void update(float ts) override;
        void render(SDL_Renderer *renderer, float ts) override;
    private:
        /**
         * All the code for checking all the input related things is put here because it is a lot of code
         */
        void checkInputs();

        bool gameOver = false;

        unsigned int * field;
        unsigned int currentPiece;
        int currentRotation;
        int storedRotation; // Used to continue alternating the rotation angles when rotating (when the piece is stuck)
        int currentX, currentY;

        float currentSpeed;
        float currentSpeedTimer;

        std::vector<unsigned int> lines;
        int linesCreated;
        int score;

        std::mt19937 engine;

        int leftDelay, rightDelay, downDelay;

        bool wasInstantPressed = false;
        bool wasRotatePressed = false;
        bool wasLeftPressed = false, wasRightPressed = false, wasDownPressed = false;
        std::map<int, Uint64> lastPress;
    };

}

#endif //MPTETRIS_GAMEMENUSTATESP_H
