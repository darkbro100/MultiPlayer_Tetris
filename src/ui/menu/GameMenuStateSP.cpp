//
// Created by pauli on 2/6/2023.
//

#include "asio.hpp"
#include "../../TetrisApp.h"
#include "GameMenuStateSP.h"
#include <random>

namespace Tetris {

    GameMenuStateSP::GameMenuStateSP(App *app) : MenuState(app) {
        // Prime PRNG state
        std::random_device dev;
        engine.seed(dev());

        player = {};
        player.currentPiece = 5;
        player.nextPiece = engine() % 7;
        player.currentX = FIELD_WIDTH / 2;
        player.currentY = 0;
        player.currentRotation = 0;
        player.storedRotation = 0;
        player.currentSpeed = 0.7f;
        player.currentSpeedTimer = 0.0f;
        player.linesCreated = 0;
        player.score = 0;

        this->leftDelay = INPUT_DELAY;
        this->rightDelay = INPUT_DELAY;
        this->downDelay = INPUT_DELAY;

        // Initialize the field
        initField(player.field);
    }

    void GameMenuStateSP::loadComponents() {
        MenuState::loadComponents();
        holder = app->getTexture("tetronimo");
    }

    void GameMenuStateSP::update(float ts) {
        MenuState::update(ts);

        if (player.gameOver) {
            bool returnKey = app->isKeyPressed(SDL_SCANCODE_RETURN);

            // If any key has been pressed after the game over screen has popped up. Go back to the main menu
            if (returnKey) {
                std::shared_ptr<MenuState> nextState = std::make_shared<MainMenuState>(app);
                nextState->loadComponents();
                app->changeState(nextState);
            }

            return;
        }

        // Check if any lines are happening. If there are, don't let anything else happen, shift the lines down
        if (!player.lines.empty()) {
            player.currentSpeedTimer += ts;
            if (player.currentSpeedTimer >= player.currentSpeed) {
                for (auto &line: player.lines) {
                    for (int x = 1; x < FIELD_WIDTH - 1; x++) {
                        int index = line * FIELD_WIDTH + x;
                        player.field[index] = EMPTY_ID;
                    }

                    for (int y = line; y > 0; y--) {
                        for (int x = 1; x < FIELD_WIDTH - 1; x++) {
                            int index = y * FIELD_WIDTH + x;
                            player.field[index] = player.field[index - FIELD_WIDTH];
                        }
                    }
                }

                // Update lines cleared & clear the vector
                player.linesCreated += player.lines.size();
                player.score += (1 << player.lines.size()) * 100;
                player.lines.clear();

                // Update timings to be the normal game timings TODO: store previous timings
                player.currentSpeedTimer = 0.0f;
                player.currentSpeed = 0.7f;
            }

            return;
        }

        // Check inputs
        checkInputs();

        // Game timings
        player.currentSpeedTimer += ts;
        if (player.currentSpeedTimer >= player.currentSpeed) {
            player.currentSpeedTimer = 0.0f;

            // Check if can move down
            if (Tetromino::canFit(player.currentPiece, player.currentX, player.currentY + 1, player.currentRotation, player.field)) {
                player.currentY++;
            } else {
                // Place the piece
                Tetromino::place(player.currentPiece, player.currentX, player.currentY, player.currentRotation, player.field);

                // Check for lines
                for (int y = 0; y < Tetromino::TETROMINO_SIZE; y++) {
                    if (player.currentY + y >= FIELD_HEIGHT - 1)
                        continue;

                    bool line = true;

                    // If a line has an empty slot
                    for (int x = 1; x < FIELD_WIDTH - 1; x++) {
                        int index = (y + player.currentY) * FIELD_WIDTH + x;
                        if (player.field[index] == EMPTY_ID) {
                            line = false;
                        }
                    }

                    if (line) {
                        player.lines.push_back(player.currentY + y);
                        player.currentSpeedTimer = 0.0f;
                        player.currentSpeed = 0.25f;
                    }
                }

                // Generate new piece
                player.currentPiece = player.nextPiece;
                player.nextPiece = engine() % 7;
                player.currentX = FIELD_WIDTH / 2;
                player.currentY = 0;
                player.currentRotation = 0;
                player.storedRotation = 0;
                player.currentSpeedTimer = 0.0f;
                leftDelay = INPUT_DELAY;
                rightDelay = INPUT_DELAY;
                downDelay = INPUT_DELAY;
                player.hasStored = false;

                if (!Tetromino::canFit(player.currentPiece, player.currentX, player.currentY, player.currentRotation, player.field))
                    player.gameOver = true;
            }
        }
    }

    void GameMenuStateSP::render(SDL_Renderer *renderer, float ts) {
        MenuState::render(renderer, ts);

        int startX = (App::WINDOW_WIDTH / 2) - (FIELD_WIDTH * CELL_SIZE) / 2;
        int startY = (App::WINDOW_HEIGHT / 2) - (FIELD_HEIGHT * CELL_SIZE) / 2;

        // Render the field
        renderField(renderer, holder, startX, startY, player.field, player.lines, player.gameOver);

        // Render the lines
        FontHolder fontHolder = app->getFont("opensans");
        SDL_Rect textRect = {startX + (FIELD_WIDTH * CELL_SIZE) + 10, startY, 150, 75};
        renderText(renderer, fontHolder.font, "Lines: " + std::to_string(player.linesCreated), {255, 255, 255, 255},
                   &textRect);

        // Render the score
        textRect = {startX + (FIELD_WIDTH * CELL_SIZE) + 10, startY + 100, 150, 75};
        renderText(renderer, fontHolder.font, "Score: " + std::to_string(player.score), {255, 255, 255, 255}, &textRect);

        // If the game isn't over... show the current piece, otherwise show a Game Over msg
        if (!player.gameOver) {
            // Render current piece in
            renderPiece(renderer, holder, player.currentX, player.currentY, player.currentPiece, player.currentRotation, startX, startY);

            // Render "ghost" piece (where it will land)
            int ghostY = player.currentY;
            while (Tetromino::canFit(player.currentPiece, player.currentX, ghostY + 1, player.currentRotation, player.field)) {
                ghostY++;
            }
            renderPiece(renderer, holder, player.currentX, ghostY, player.currentPiece, player.currentRotation, startX, startY, 50);

            // Render the next piece under the score
            textRect = {startX + (FIELD_WIDTH * CELL_SIZE) + 10, startY + 200, 150, 75};
            renderText(renderer, fontHolder.font, "Next", {255, 255, 255, 255}, &textRect);
            renderPiece(renderer, holder,  0, 0, player.nextPiece, 0, startX + (FIELD_WIDTH * CELL_SIZE) + 10, startY + 225, 100);

            // Render the stored piece directly under the next piece
            if (player.storedPiece != INVALID_SHAPE) {
                textRect = {startX + (FIELD_WIDTH * CELL_SIZE) + 10, startY + 400, 150, 75};
                renderText(renderer, fontHolder.font, "Stored", {255, 255, 255, 255}, &textRect);
                renderPiece(renderer, holder,  0, 0, player.storedPiece, 0, startX + (FIELD_WIDTH * CELL_SIZE) + 10, startY + 425, 100);
            }
        } else {
            // Draw Game Over
            fontHolder = app->getFont("opensans_large");
            textRect = {startX + (FIELD_WIDTH * CELL_SIZE) / 2 - 100,
                        startY + (FIELD_HEIGHT * CELL_SIZE) / 2 - 50, 200, 100};
            renderText(renderer, fontHolder.font, "Game Over", {255, 255, 255, 255}, &textRect);

            // Draw "Press Enter to Continue" below
            textRect = {startX + (FIELD_WIDTH * CELL_SIZE) / 2 - 150,
                        startY + (FIELD_HEIGHT * CELL_SIZE) / 2 + 50, 300, 100};
            renderText(renderer, fontHolder.font, "Press Enter", {255, 255, 255, 255}, &textRect);
        }
    }

    void GameMenuStateSP::checkInputs() {
        // Updating variables based on input
        bool isInstantPressed = app->isKeyPressed(INSTANT_DROP_KEY);
        bool isRotatePressed = app->isKeyPressed(ROTATE_KEY);
        bool isLeftPressed = app->isKeyPressed(LEFT_KEY);
        bool isRightPressed = app->isKeyPressed(RIGHT_KEY);
        bool isDownPressed = app->isKeyPressed(DOWN_KEY);
        bool isStorePressed = app->isKeyPressed(STORE_KEY);

        bool wasInstantPressed = this->wasInstantPressed;
        bool wasRotatePressed = this->wasRotatePressed;
        bool wasLeftPressed = this->wasLeftPressed;
        bool wasRightPressed = this->wasRightPressed;
        bool wasDownPressed = this->wasDownPressed;
        bool wasStorePressed = this->wasStorePressed;

        this->wasStorePressed = isStorePressed;
        this->wasLeftPressed = isLeftPressed;
        this->wasRightPressed = isRightPressed;
        this->wasDownPressed = isDownPressed;
        this->wasRotatePressed = isRotatePressed;
        this->wasInstantPressed = isInstantPressed;

        bool canPressRotate = isRotatePressed && !wasRotatePressed;
        bool canPressLeft = lastPress.count(LEFT_KEY) == 0 || SDL_GetTicks64() > this->lastPress[LEFT_KEY];
        bool canPressRight = lastPress.count(RIGHT_KEY) == 0 || SDL_GetTicks64() > this->lastPress[RIGHT_KEY];
        bool canPressDown = lastPress.count(DOWN_KEY) == 0 || SDL_GetTicks64() > this->lastPress[DOWN_KEY];

        // First check if we can store a piece
        if (!player.hasStored && isStorePressed && !wasStorePressed) {
            unsigned int temp = player.storedPiece;
            player.storedPiece = player.currentPiece;
            player.currentPiece = temp;
            player.hasStored = true;
        }

        // Input handling for moving forward/backward/down
        if (isLeftPressed) {
            // means that they are just tapping the arrow key once
            if (!wasLeftPressed && Tetromino::canFit(player.currentPiece, player.currentX - 1, player.currentY, player.currentRotation, player.field)) {
                player.currentX--;
                lastPress[LEFT_KEY] = SDL_GetTicks64() + leftDelay;
            } else if (wasLeftPressed && canPressLeft &&
                       Tetromino::canFit(player.currentPiece, player.currentX - 1, player.currentY, player.currentRotation, player.field)) {
                player.currentX--;
                lastPress[LEFT_KEY] = SDL_GetTicks64() + leftDelay;
                leftDelay /= leftDelay > 0 ? 2 : 1;
            }

        } else {
            leftDelay = INPUT_DELAY;
        }

        if (isRightPressed) {

            // means that they are just tapping the arrow key once
            if (!wasRightPressed && Tetromino::canFit(player.currentPiece, player.currentX + 1, player.currentY, player.currentRotation, player.field)) {
                player.currentX++;
                lastPress[RIGHT_KEY] = SDL_GetTicks64() + rightDelay;
            } else if (wasRightPressed && canPressRight &&
                       Tetromino::canFit(player.currentPiece, player.currentX + 1, player.currentY, player.currentRotation, player.field)) {
                player.currentX++;
                lastPress[RIGHT_KEY] = SDL_GetTicks64() + rightDelay;
                rightDelay /= rightDelay > 0 ? 2 : 1;
            }

        } else {
            rightDelay = INPUT_DELAY;
        }

        if (isDownPressed) {

            // means that they are just tapping the arrow key once
            if (!wasDownPressed && Tetromino::canFit(player.currentPiece, player.currentX, player.currentY + 1, player.currentRotation, player.field)) {
                player.currentY++;
                lastPress[DOWN_KEY] = SDL_GetTicks64() + downDelay;
            } else if (wasDownPressed && canPressDown &&
                       Tetromino::canFit(player.currentPiece, player.currentX, player.currentY + 1, player.currentRotation, player.field)) {
                player.currentY++;
                lastPress[DOWN_KEY] = SDL_GetTicks64() + downDelay;
                downDelay /= downDelay > 0 ? 2 : 1;
            }

        } else {
            downDelay = INPUT_DELAY;
        }

        // Input handling for rotating
        if (canPressRotate) {
            player.storedRotation = (player.storedRotation + 1) % 4;
            if (Tetromino::canFit(player.currentPiece, player.currentX, player.currentY, player.storedRotation, player.field)) {
                player.currentRotation = player.storedRotation;
            }
        }

        // When spacebar is pressed, the piece is dropped instantly and locked to the board
        if (isInstantPressed && !wasInstantPressed) {
            while (Tetromino::canFit(player.currentPiece, player.currentX, player.currentY + 1, player.currentRotation, player.field)) {
                player.currentY++;
            }
            player.currentSpeedTimer = player.currentSpeed;
        }

    }
}