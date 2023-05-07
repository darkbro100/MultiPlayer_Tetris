//
// Created by pauli on 5/4/2023.
//

#ifndef MPTETRIS_GAMECOMMON_H
#define MPTETRIS_GAMECOMMON_H

#include "SDL.h"
#include "Tetronimo.h"
#include "../net/NetworkClient.h"
#include <map>
#include <random>

namespace Tetris {

    // Forward declare
    class App;

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

    struct InputHolder {
        int leftDelay, rightDelay, downDelay;

        bool wasStorePressed = false;
        bool wasInstantPressed = false;
        bool wasRotatePressed = false;
        bool wasLeftPressed = false, wasRightPressed = false, wasDownPressed = false;
        std::map<int, Uint64> lastPress;
    };

    struct PlayerPos {
        int16_t x, y;
        uint8_t rotation, storedRotation;
    };

    struct PlayerPiece {
        uint8_t current, next, stored = INVALID_SHAPE;
        bool hasStored = false;
    };

    struct PlayerSpeed {
        float current, timer;
    };

    struct Player {
        uint32_t id{};
        uint16_t ping{};

        PlayerPos pos{};
        PlayerPiece piece{};
        PlayerSpeed speed{};

        uint8_t field[FIELD_WIDTH * FIELD_HEIGHT]{};

        uint32_t lines{}, score{};

        bool gameOver{};
    };

    struct SinglePlayer {
        uint32_t id;
        uint16_t ping;

        PlayerPos pos{};
        PlayerPiece piece{};
        PlayerSpeed speed{};

        uint8_t field[FIELD_WIDTH * FIELD_HEIGHT];

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

    /**
     * All the code for checking all the input related things is put here because it is a lot of code
     */
    void checkInputs(InputHolder &holder, App *app, PlayerPiece & piece, PlayerPos &pos, PlayerSpeed & speed, uint8_t * field, std::mt19937 & engine);

    void checkLines(uint8_t *field, std::vector<unsigned int> &lines, PlayerPos & pos, PlayerSpeed & speed);

    void clearLines(uint8_t * field, std::vector<unsigned int> & lines);

    void resetPiece(PlayerPiece & piece, PlayerPos & pos, PlayerSpeed & speed, InputHolder & inputs, std::mt19937 & engine);

    int getPlace(std::vector<uint32_t> placements, uint32_t id, int totalPlayers);

    /**
     * Formats the placement to a string. (ie. 1 = 1st, 2 = 2nd, etc.)
     * @param place Placement number
     * @return String representing the placement
     */
    std::string formatPlacement(int place);
}

#endif //MPTETRIS_GAMECOMMON_H
