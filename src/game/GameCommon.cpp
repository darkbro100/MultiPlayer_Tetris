//
// Created by pauli on 5/6/2023.
//

#include "GameCommon.h"
#include "../TetrisApp.h"

namespace Tetris {

    void initField(uint8_t *field) {
        for (int x = 0; x < FIELD_WIDTH; x++) {
            for (int y = 0; y < FIELD_HEIGHT; y++) {
                int index = x + y * FIELD_WIDTH;

                // Check if boundary
                if (x == 0 || x == FIELD_WIDTH - 1 || y == FIELD_HEIGHT - 1) {
                    field[index] = BOUNDARY_ID;
                } else {
                    field[index] = EMPTY_ID;
                }
            }
        }
    }

    void checkInputs(InputHolder &holder, App *app, PlayerPiece & piece, PlayerPos &pos, PlayerSpeed & speed, uint8_t * field, std::mt19937 & engine) {
        // Updating variables based on input
        bool isInstantPressed = app->isKeyPressed(INSTANT_DROP_KEY);
        bool isRotatePressed = app->isKeyPressed(ROTATE_KEY);
        bool isLeftPressed = app->isKeyPressed(LEFT_KEY);
        bool isRightPressed = app->isKeyPressed(RIGHT_KEY);
        bool isDownPressed = app->isKeyPressed(DOWN_KEY);
        bool isStorePressed = app->isKeyPressed(STORE_KEY);

        bool wasInstantPressed = holder.wasInstantPressed;
        bool wasRotatePressed = holder.wasRotatePressed;
        bool wasLeftPressed = holder.wasLeftPressed;
        bool wasRightPressed = holder.wasRightPressed;
        bool wasDownPressed = holder.wasDownPressed;
        bool wasStorePressed = holder.wasStorePressed;

        holder.wasStorePressed = isStorePressed;
        holder.wasLeftPressed = isLeftPressed;
        holder.wasRightPressed = isRightPressed;
        holder.wasDownPressed = isDownPressed;
        holder.wasRotatePressed = isRotatePressed;
        holder.wasInstantPressed = isInstantPressed;

        bool canPressRotate = isRotatePressed && !wasRotatePressed;
        bool canPressLeft = holder.lastPress.count(LEFT_KEY) == 0 || SDL_GetTicks64() > holder.lastPress[LEFT_KEY];
        bool canPressRight = holder.lastPress.count(RIGHT_KEY) == 0 || SDL_GetTicks64() > holder.lastPress[RIGHT_KEY];
        bool canPressDown = holder.lastPress.count(DOWN_KEY) == 0 || SDL_GetTicks64() > holder.lastPress[DOWN_KEY];

        // First check if we can store a piece
        if (!piece.hasStored && isStorePressed && !wasStorePressed) {
            if(piece.stored == INVALID_SHAPE) {
                piece.stored = piece.current;
                piece.current = piece.next;
                piece.next = engine() % 7;
            } else {
                unsigned int temp = piece.stored;
                piece.stored = piece.current;
                piece.current = temp;
            }
            piece.hasStored = true;
        }

        // Input handling for moving forward/backward/down
        if (isLeftPressed) {
            // means that they are just tapping the arrow key once
            if (!wasLeftPressed &&
                Tetromino::canFit(piece.current, pos.x - 1, pos.y, pos.rotation,
                                  field)) {
                pos.x--;
                holder.lastPress[LEFT_KEY] = SDL_GetTicks64() + holder.leftDelay;
            } else if (wasLeftPressed && canPressLeft &&
                       Tetromino::canFit(piece.current, pos.x - 1, pos.y,
                                         pos.rotation, field)) {
                pos.x--;
                holder.lastPress[LEFT_KEY] = SDL_GetTicks64() + holder.leftDelay;
                holder.leftDelay /= holder.leftDelay > 0 ? 2 : 1;
            }

        } else {
            holder.leftDelay = INPUT_DELAY;
        }

        if (isRightPressed) {

            // means that they are just tapping the arrow key once
            if (!wasRightPressed &&
                Tetromino::canFit(piece.current, pos.x + 1, pos.y, pos.rotation,
                                  field)) {
                pos.x++;
                holder.lastPress[RIGHT_KEY] = SDL_GetTicks64() + holder.rightDelay;
            } else if (wasRightPressed && canPressRight &&
                       Tetromino::canFit(piece.current, pos.x + 1, pos.y,
                                         pos.rotation, field)) {
                pos.x++;
                holder.lastPress[RIGHT_KEY] = SDL_GetTicks64() + holder.rightDelay;
                holder.rightDelay /= holder.rightDelay > 0 ? 2 : 1;
            }

        } else {
            holder.rightDelay = INPUT_DELAY;
        }

        if (isDownPressed) {

            // means that they are just tapping the arrow key once
            if (!wasDownPressed &&
                Tetromino::canFit(piece.current, pos.x, pos.y + 1, pos.rotation,
                                  field)) {
                pos.y++;
                holder.lastPress[DOWN_KEY] = SDL_GetTicks64() + holder.downDelay;
            } else if (wasDownPressed && canPressDown &&
                       Tetromino::canFit(piece.current, pos.x, pos.y + 1,
                                         pos.rotation, field)) {
                pos.y++;
                holder.lastPress[DOWN_KEY] = SDL_GetTicks64() + holder.downDelay;
                holder.downDelay /= holder.downDelay > 0 ? 2 : 1;
            }

        } else {
            holder.downDelay = INPUT_DELAY;
        }

        // Input handling for rotating
        if (canPressRotate) {
            pos.storedRotation = (pos.storedRotation + 1) % 4;
            if (Tetromino::canFit(piece.current, pos.x, pos.y, pos.storedRotation,
                                  field)) {
                pos.rotation = pos.storedRotation;
            }
        }

        // When spacebar is pressed, the piece is dropped instantly and locked to the board
        if (isInstantPressed && !wasInstantPressed) {
            while (Tetromino::canFit(piece.current, pos.x, pos.y + 1, pos.rotation,
                                     field)) {
                pos.y++;
            }
            speed.timer = speed.current;
        }

    }

    void checkLines(uint8_t *field, std::vector<unsigned int> &lines, PlayerPos & pos, PlayerSpeed & speed) {
        // Check for lines
        for (int y = 0; y < Tetromino::TETROMINO_SIZE; y++) {
            if (pos.y + y >= FIELD_HEIGHT - 1)
                continue;

            bool line = true;

            // If a line has an empty slot
            for (int x = 1; x < FIELD_WIDTH - 1; x++) {
                int index = (y + pos.y) * FIELD_WIDTH + x;
                if (field[index] == EMPTY_ID) {
                    line = false;
                }
            }

            if (line) {
                lines.push_back(pos.y + y);
                speed.timer = 0.0f;
                speed.current = 0.25f;
            }
        }
    }

    void Tetris::resetPiece(PlayerPiece &piece, PlayerPos &pos, PlayerSpeed &speed, InputHolder &inputs,
                            std::mt19937 &engine) {
        piece.current = piece.next;
        piece.next = engine() % 7;
        pos.x = FIELD_WIDTH / 2;
        pos.y = 0;
        pos.rotation = 0;
        pos.storedRotation = 0;
        speed.timer = 0.0f;
        piece.hasStored = false;
        inputs.leftDelay = INPUT_DELAY;
        inputs.rightDelay = INPUT_DELAY;
        inputs.downDelay = INPUT_DELAY;
    }

    void clearLines(uint8_t *field, std::vector<unsigned int> &lines) {
        for (auto &line: lines) {
            for (int x = 1; x < FIELD_WIDTH - 1; x++) {
                int index = line * FIELD_WIDTH + x;
                field[index] = EMPTY_ID;
            }

            for (int y = line; y > 0; y--) {
                for (int x = 1; x < FIELD_WIDTH - 1; x++) {
                    int index = y * FIELD_WIDTH + x;
                    field[index] = field[index - FIELD_WIDTH];
                }
            }
        }
    }

    int getPlace(std::vector<uint32_t> placements, uint32_t id, int totalPlayers) {
        // push_back is called once a player loses. This means that the player who just lost is at the end of the vector

        /**
         * 4 Players Total
         * 3 Players in the List
         * Player at the end of the list just recently lost (2nd place) (they are essentially placements.end() - 1)
         * 2nd Place is 4 - (3 - 1) = 2
         * 3rd Place is 4 - (2 - 1) = 3
         * 4th Place is 4 - (1 - 1) = 4
         *
         * 1st place is 4 - (2 - 1) = 3
         * 2nd place is 4 - (2 - 2) = 4
         * 3rd place is 4 - (2 - 3) = 5
         * 4th place is 4 - (2 - 4) = 6
         */

        auto it = std::find(placements.begin(), placements.end(), id);
        if(it != placements.end()) {
            return totalPlayers - (it - placements.begin());
        } else {
            return 1;
        }
    }

    std::string formatPlacement(int place) {

        // 1st, 2nd, 3rd, 4th, 5th, 6th, 7th, 8th, 9th, 10th, 11th, 12th, 13th, 14th, 21st, 22nd, 23rd, etc.
        std::string str;
        if(place % 10 == 1 && place % 100 != 11) {
            str = std::to_string(place) + "st";
        } else if(place % 10 == 2 && place % 100 != 12) {
            str = std::to_string(place) + "nd";
        } else if(place % 10 == 3 && place % 100 != 13) {
            str = std::to_string(place) + "rd";
        } else {
            str = std::to_string(place) + "th";
        }

        return str;
    }
}
