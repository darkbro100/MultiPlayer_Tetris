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

    void checkInputs(InputHolder &holder, App *app, PlayerPiece & piece, PlayerPos &pos, PlayerSpeed & speed, uint8_t * field) {
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
}
