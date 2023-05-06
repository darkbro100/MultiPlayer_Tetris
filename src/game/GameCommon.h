//
// Created by pauli on 5/4/2023.
//

#ifndef MPTETRIS_GAMECOMMON_H
#define MPTETRIS_GAMECOMMON_H

#include "SDL.h"
#include "Tetronimo.h"
#include "../net/NetworkClient.h"

namespace Tetris {

    const static int CELL_SIZE = 48;
    const static int TEXTURE_BORDER = 26;
    const static int TEXTURE_SIZE = 512;

    const static int INVALID_SHAPE = 10;
    const static int INPUT_DELAY = 100;
    const static int FIELD_WIDTH = 12;
    const static int FIELD_HEIGHT = 21;
    const static int BOUNDARY_ID = 8;
    const static int EMPTY_ID = 0;

    const static int STORE_KEY = SDL_SCANCODE_C;
    const static int ROTATE_KEY = SDL_SCANCODE_UP;
    const static int LEFT_KEY = SDL_SCANCODE_LEFT;
    const static int RIGHT_KEY = SDL_SCANCODE_RIGHT;
    const static int DOWN_KEY = SDL_SCANCODE_DOWN;
    const static int INSTANT_DROP_KEY = SDL_SCANCODE_SPACE;

    struct Player {
        uint32_t id;
        uint16_t ping;

        uint8_t field[FIELD_WIDTH * FIELD_HEIGHT];
        uint8_t currentPiece, nextPiece, storedPiece;

        int16_t currentX, currentY;

        float currentSpeed;

        bool hasStored, gameOver;
    };

    struct SinglePlayer {
        uint32_t id;
        uint16_t ping;

        uint8_t field[FIELD_WIDTH * FIELD_HEIGHT];
        uint8_t currentPiece, nextPiece, storedPiece, currentRotation, storedRotation;

        int16_t currentX, currentY;

        float currentSpeed, currentSpeedTimer;

        int linesCreated = 0;
        std::vector<unsigned int> lines;
        uint32_t score = 0;

        bool hasStored, gameOver, started;
    };

    /**
     * Initializes the field with the boundary and empty cells
     * @param field Array of the field
     */
    void initField(uint8_t *field);
}

#endif //MPTETRIS_GAMECOMMON_H
