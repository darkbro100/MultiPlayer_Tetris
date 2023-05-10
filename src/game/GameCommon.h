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

    // Used for the textures of the Tetromino blocks
    const static int CELL_SIZE = 48;
    const static int TEXTURE_BORDER = 26;
    const static int TEXTURE_SIZE = 512;

    /**
     * Represents an invalid shape inside the field (default values)
     */
    const static int INVALID_SHAPE = 10;

    /**
     * Default input delay applied
     */
    const static int INPUT_DELAY = 100;

    /**
     * Default width/height for each Tetris field
     */
    const static int FIELD_WIDTH = 12;
    const static int FIELD_HEIGHT = 21;

    /**
     * Represents the ID for an empty cell and a boundary cell
     */
    const static int BOUNDARY_ID = 8;
    const static int EMPTY_ID = 0;

    /**
     * Easy representations for each of the controls for the game
     */
    const static int STORE_KEY = SDL_SCANCODE_C;
    const static int ROTATE_KEY = SDL_SCANCODE_UP;
    const static int LEFT_KEY = SDL_SCANCODE_LEFT;
    const static int RIGHT_KEY = SDL_SCANCODE_RIGHT;
    const static int DOWN_KEY = SDL_SCANCODE_DOWN;
    const static int INSTANT_DROP_KEY = SDL_SCANCODE_SPACE;

    /**
     * Struct that holds the input of the Player
     */
    struct InputHolder {
        /**
         * Last time the left,right, and down arrow keys were pressed
         */
        int leftDelay, rightDelay, downDelay;

        /**
         * If these keys were pressed on the previous frame
         */
        bool wasStorePressed = false;
        bool wasInstantPressed = false;
        bool wasRotatePressed = false;
        bool wasLeftPressed = false, wasRightPressed = false, wasDownPressed = false;

        /**
         * Map that also holds the last time a key was pressed
         */
        std::map<int, Uint64> lastPress;
    };

    /**
     * Represents the position of the Player
     */
    struct PlayerPos {
        /**
         * X & Y Position of the piece, used for the field
         */
        int16_t x, y;

        /**
         * Rotation of the piece, storedRotation is used for calculating the current rotation
         */
        uint8_t rotation, storedRotation;
    };

    /**
     * Represents the Player's piece
     */
    struct PlayerPiece {
        /**
         * Current = Current Piece
         * Next = Next Piece
         * Stored = Stored Piece
         */
        uint8_t current, next, stored = INVALID_SHAPE;

        /**
         * Whether or not the Player has already stored a piece this turn
         */
        bool hasStored = false;
    };

    /**
     * Struct that maintains the speed of the Player's game
     */
    struct PlayerSpeed {
        /**
         * Current = Current Speed
         * Timer = Timer adding up to the current speed
         */
        float current, timer;
    };

    /**
     * Struct that maintains a Player's data during a Multiplayer session.
     */
    struct Player {
        /**
         * ID of the player
         */
        uint32_t id{};

        /**
         * Ping of the player
         */
        uint16_t ping{};

        /**
         * Position of the Player
         */
        PlayerPos pos{};

        /**
         * Piece of the Player
         */
        PlayerPiece piece{};

        /**
         * Game speed of the Player
         */
        PlayerSpeed speed{};

        /**
         * Array containing the field
         */
        uint8_t field[FIELD_WIDTH * FIELD_HEIGHT]{};

        /**
         * Number of lines created and the current score
         */
        uint32_t lines{}, score{};

        /**
         * Whether or not the game is over
         */
        bool gameOver{};
    };

    /**
     * Struct that represents a Single Player game instance. This differs from the Multiplayer Player struct because it contains fields that are not able to be serialized (std::vector)
     */
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

    /**
     * Check if there are any lines that exist in the field, marks them as cleared
     * @param field Pointer to the field
     * @param lines Reference to the vector containing the lines
     * @param pos Reference to the piece's position
     * @param speed Reference to the speed of the game (needs to be changed if lines are cleared)
     */
    void checkLines(uint8_t *field, std::vector<unsigned int> &lines, PlayerPos & pos, PlayerSpeed & speed);

    /**
     * Actually clears the lines from the field. (Previous function marks them to be cleared)
     * @param field Pointer to the field
     * @param lines Vector containing the lines to be cleared
     */
    void clearLines(uint8_t * field, std::vector<unsigned int> & lines);

    /**
     * Resets the piece to the top of the field, will also assign it new values
     * @param piece Reference to the piece
     * @param pos Reference to the position of the piece
     * @param speed Reference to the speed of the game
     * @param inputs Reference to the input holder
     * @param engine Reference to the PRNG state
     */
    void resetPiece(PlayerPiece & piece, PlayerPos & pos, PlayerSpeed & speed, InputHolder & inputs, std::mt19937 & engine);

    /**
     * Gets the place of the player
     * @param placements List containing the placements of the players
     * @param id ID of the player to check for
     * @param totalPlayers Total number of  players in the game
     * @return The place of the player
     */
    int getPlace(std::vector<uint32_t> placements, uint32_t id, int totalPlayers);

    /**
     * Formats the placement to a string. (ie. 1 = 1st, 2 = 2nd, etc.)
     * @param place Placement number
     * @return String representing the placement
     */
    std::string formatPlacement(int place);
}

#endif //MPTETRIS_GAMECOMMON_H
