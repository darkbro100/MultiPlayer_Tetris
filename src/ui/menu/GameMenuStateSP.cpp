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

        this->currentPiece = engine() % 7;
        this->nextPiece = engine() % 7;
        this->currentX = FIELD_WIDTH / 2;
        this->currentY = 0;
        this->currentRotation = 0;
        this->storedRotation = 0;
        this->currentSpeed = 0.7f;
        this->currentSpeedTimer = 0.0f;
        this->linesCreated = 0;
        this->score = 0;

        this->leftDelay = INPUT_DELAY;
        this->rightDelay = INPUT_DELAY;
        this->downDelay = INPUT_DELAY;

        this->field = new unsigned int[FIELD_WIDTH * FIELD_HEIGHT];

        // Initialize the field
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

    void GameMenuStateSP::loadComponents() {
        MenuState::loadComponents();

        asio::io_context io_context;
        asio::ip::tcp::resolver resolver(io_context);
    }

    void GameMenuStateSP::update(float ts) {
        MenuState::update(ts);

        if (gameOver) {
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
        if (!lines.empty()) {
            currentSpeedTimer += ts;
            if (currentSpeedTimer >= currentSpeed) {
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

                // Update lines cleared & clear the vector
                linesCreated += lines.size();
                score += (1 << lines.size()) * 100;
                lines.clear();

                // Update timings to be the normal game timings TODO: store previous timings
                currentSpeedTimer = 0.0f;
                currentSpeed = 0.7f;
            }

            return;
        }

        // Check inputs
        checkInputs();

        // Game timings
        currentSpeedTimer += ts;
        if (currentSpeedTimer >= currentSpeed) {
            currentSpeedTimer = 0.0f;

            // Check if can move down
            if (Tetromino::canFit(currentPiece, currentX, currentY + 1, currentRotation, field)) {
                currentY++;
            } else {
                // Place the piece
                Tetromino::place(currentPiece, currentX, currentY, currentRotation, field);

                // Check for lines
                for (int y = 0; y < Tetromino::TETROMINO_SIZE; y++) {
                    if (currentY + y >= FIELD_HEIGHT - 1)
                        continue;

                    bool line = true;

                    // If a line has an empty slot
                    for (int x = 1; x < FIELD_WIDTH - 1; x++) {
                        int index = (y + currentY) * FIELD_WIDTH + x;
                        if (field[index] == EMPTY_ID) {
                            line = false;
                        }
                    }

                    if (line) {
                        lines.push_back(currentY + y);
                        currentSpeedTimer = 0.0f;
                        currentSpeed = 0.25f;
                    }
                }

                // Generate new piece
                currentPiece = nextPiece;
                nextPiece = engine() % 7;
                currentX = FIELD_WIDTH / 2;
                currentY = 0;
                currentRotation = 0;
                storedRotation = 0;
                currentSpeedTimer = 0.0f;
                leftDelay = INPUT_DELAY;
                rightDelay = INPUT_DELAY;
                downDelay = INPUT_DELAY;
                hasStored = false;

                if (!Tetromino::canFit(currentPiece, currentX, currentY, currentRotation, field))
                    gameOver = true;
            }
        }
    }

    void GameMenuStateSP::render(SDL_Renderer *renderer, float ts) {
        MenuState::render(renderer, ts);

        int startX = (App::WINDOW_WIDTH / 2) - (FIELD_WIDTH * CELL_SIZE) / 2;
        int startY = (App::WINDOW_HEIGHT / 2) - (FIELD_HEIGHT * CELL_SIZE) / 2;

        // Render the field
        for (int x = 0; x < FIELD_WIDTH; x++) {
            for (int y = 0; y < FIELD_HEIGHT; y++) {
                int index = x + y * FIELD_WIDTH;
                unsigned int id = field[index];

                int posX = startX + x * CELL_SIZE;
                int posY = startY + y * CELL_SIZE;

                // Check if boundary
                if (id == BOUNDARY_ID) {
                    SDL_Texture *tetrominoTexture = app->getTexture("tetronimo").texture;
                    int textureX = TEXTURE_SIZE * 7;

                    SDL_Rect srcRect = {textureX + TEXTURE_BORDER, TEXTURE_BORDER, 460, 460};
                    SDL_Rect dstRec = {posX, posY, CELL_SIZE, CELL_SIZE};

                    renderTexture(renderer, tetrominoTexture, &srcRect, &dstRec, gameOver ? 100 : 255);
                } else if (id == EMPTY_ID) {
                    // Draw blank square
                    SDL_SetRenderDrawColor(renderer, 128, 128, 128, gameOver ? 100 : 255);
                    SDL_Rect rect = {posX, posY, CELL_SIZE, CELL_SIZE};
                    SDL_RenderDrawRect(renderer, &rect);
                } else {
                    // Otherwise draw in the correct teromino piece
                    SDL_Texture *tetrominoTexture = app->getTexture("tetronimo").texture;
                    int textureX = TEXTURE_SIZE * (id - 1);

                    SDL_Rect srcRect = {textureX + TEXTURE_BORDER, TEXTURE_BORDER, 460, 460};
                    SDL_Rect dstRec = {posX, posY, CELL_SIZE, CELL_SIZE};

                    // If a line exists on this row, use the boundary texture instead of the normal texture, otherwise draw it normally
                    if (std::count(lines.begin(), lines.end(), y) > 0) {
                        textureX = TEXTURE_SIZE * 7;
                        srcRect = {textureX + TEXTURE_BORDER, TEXTURE_BORDER, 460, 460};

                        renderTexture(renderer, tetrominoTexture, &srcRect, &dstRec, gameOver ? 100 : 255);
                    } else
                        renderTexture(renderer, tetrominoTexture, &srcRect, &dstRec, gameOver ? 100 : 255);
                }
            }
        }

        // Render the lines
        FontHolder fontHolder = app->getFont("opensans");
        SDL_Rect textRect = {startX + (FIELD_WIDTH * CELL_SIZE) + 10, startY, 150, 75};
        renderText(renderer, fontHolder.font, "Lines: " + std::to_string(linesCreated), {255, 255, 255, 255},
                   &textRect);

        // Render the score
        textRect = {startX + (FIELD_WIDTH * CELL_SIZE) + 10, startY + 100, 150, 75};
        renderText(renderer, fontHolder.font, "Score: " + std::to_string(score), {255, 255, 255, 255}, &textRect);

        // If the game isn't over... show the current piece, otherwise show a Game Over msg
        if (!gameOver) {
            // Render current piece in
            renderPiece(renderer, currentX, currentY, currentPiece, currentRotation, startX, startY);

            // Render "ghost" piece (where it will land)
            int ghostY = currentY;
            while (Tetromino::canFit(currentPiece, currentX, ghostY + 1, currentRotation, field)) {
                ghostY++;
            }
            renderPiece(renderer, currentX, ghostY, currentPiece, currentRotation, startX, startY, 50);

            // Render the next piece under the score
            textRect = {startX + (FIELD_WIDTH * CELL_SIZE) + 10, startY + 200, 150, 75};
            renderText(renderer, fontHolder.font, "Next", {255, 255, 255, 255}, &textRect);
            renderPiece(renderer, 0, 0, nextPiece, 0, startX + (FIELD_WIDTH * CELL_SIZE) + 10, startY + 225, 100);

            // Render the stored piece directly under the next piece
            if (storedPiece != INVALID_SHAPE) {
                textRect = {startX + (FIELD_WIDTH * CELL_SIZE) + 10, startY + 400, 150, 75};
                renderText(renderer, fontHolder.font, "Stored", {255, 255, 255, 255}, &textRect);
                renderPiece(renderer, 0, 0, storedPiece, 0, startX + (FIELD_WIDTH * CELL_SIZE) + 10, startY + 425, 100);
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
        if (!hasStored && isStorePressed && !wasStorePressed) {
            unsigned int temp = storedPiece;
            storedPiece = currentPiece;
            currentPiece = temp;
            hasStored = true;
        }

        // Input handling for moving forward/backward/down
        if (isLeftPressed) {

            // means that they are just tapping the arrow key once
            if (!wasLeftPressed && Tetromino::canFit(currentPiece, currentX - 1, currentY, currentRotation, field)) {
                currentX--;
                lastPress[LEFT_KEY] = SDL_GetTicks64() + leftDelay;
            } else if (wasLeftPressed && canPressLeft &&
                       Tetromino::canFit(currentPiece, currentX - 1, currentY, currentRotation, field)) {
                currentX--;
                lastPress[LEFT_KEY] = SDL_GetTicks64() + leftDelay;
                leftDelay /= leftDelay > 0 ? 2 : 1;
            }

        } else {
            leftDelay = INPUT_DELAY;
        }

        if (isRightPressed) {

            // means that they are just tapping the arrow key once
            if (!wasRightPressed && Tetromino::canFit(currentPiece, currentX + 1, currentY, currentRotation, field)) {
                currentX++;
                lastPress[RIGHT_KEY] = SDL_GetTicks64() + rightDelay;
            } else if (wasRightPressed && canPressRight &&
                       Tetromino::canFit(currentPiece, currentX + 1, currentY, currentRotation, field)) {
                currentX++;
                lastPress[RIGHT_KEY] = SDL_GetTicks64() + rightDelay;
                rightDelay /= rightDelay > 0 ? 2 : 1;
            }

        } else {
            rightDelay = INPUT_DELAY;
        }

        if (isDownPressed) {

            // means that they are just tapping the arrow key once
            if (!wasDownPressed && Tetromino::canFit(currentPiece, currentX, currentY + 1, currentRotation, field)) {
                currentY++;
                lastPress[DOWN_KEY] = SDL_GetTicks64() + downDelay;
            } else if (wasDownPressed && canPressDown &&
                       Tetromino::canFit(currentPiece, currentX, currentY + 1, currentRotation, field)) {
                currentY++;
                lastPress[DOWN_KEY] = SDL_GetTicks64() + downDelay;
                downDelay /= downDelay > 0 ? 2 : 1;
            }

        } else {
            downDelay = INPUT_DELAY;
        }

        // Input handling for rotating
        if (canPressRotate) {
            storedRotation = (storedRotation + 1) % 4;
            if (Tetromino::canFit(currentPiece, currentX, currentY, storedRotation, field)) {
                currentRotation = storedRotation;
            }
        }

        // When spacebar is pressed, the piece is dropped instantly and locked to the board
        if (isInstantPressed && !wasInstantPressed) {
            while (Tetromino::canFit(currentPiece, currentX, currentY + 1, currentRotation, field)) {
                currentY++;
            }
            currentSpeedTimer = currentSpeed;
        }

    }

    void
    GameMenuStateSP::renderPiece(SDL_Renderer *renderer, int x, int y, unsigned int piece, int rotation, int startX,
                                 int startY, int alpha) {
        for (int pieceX = 0; pieceX < Tetromino::TETROMINO_SIZE; pieceX++) {
            for (int pieceY = 0; pieceY < Tetromino::TETROMINO_SIZE; pieceY++) {
                const unsigned int *shape = Tetromino::getShape(piece);
                int index = Tetromino::rotate(pieceX, pieceY, rotation);
                unsigned int value = shape[index];

                if (value != 0) {
                    int posX = startX + (x + pieceX) * CELL_SIZE;
                    int posY = startY + (y + pieceY) * CELL_SIZE;

                    SDL_Texture *tetrominoTexture = app->getTexture("tetronimo").texture;
                    int textureX = TEXTURE_SIZE * piece;

                    SDL_Rect srcRect = {textureX + TEXTURE_BORDER, TEXTURE_BORDER, 460, 460};
                    SDL_Rect dstRec = {posX, posY, CELL_SIZE, CELL_SIZE};

                    renderTexture(renderer, tetrominoTexture, &srcRect, &dstRec, alpha);
                }
            }
        }
    }
}