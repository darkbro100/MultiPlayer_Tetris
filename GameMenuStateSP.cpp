//
// Created by pauli on 2/6/2023.
//

#include "TetrisApp.h"
#include "GameMenuStateSP.h"
#include <random>

namespace Tetris {

    GameMenuStateSP::GameMenuStateSP(App *app) : MenuState(app) {
        // Prime PRNG state
        std::random_device dev;
        engine.seed(dev());

        this->currentPiece = engine() % 7;
        this->currentX = FIELD_WIDTH / 2;
        this->currentY = 0;
        this->currentRotation = 0;
        this->currentSpeed = 0.7f;
        this->currentSpeedTimer = 0.0f;

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
    }

    void GameMenuStateSP::update(float ts) {
        MenuState::update(ts);

        //TODO: make game over better lol
        if(gameOver) {
            return;
        }

        // Check if any lines are happening. If there are, don't let anything else happen, shift the lines down
        if(!lines.empty()) {
            currentSpeedTimer += ts;
            if(currentSpeedTimer >= currentSpeed) {
                for(auto & line : lines) {
                    for(int x = 1; x < FIELD_WIDTH - 1; x++) {
                        int index = line * FIELD_WIDTH + x;
                        field[index] = EMPTY_ID;
                    }

                    for(int y = line; y > 0; y--) {
                        for(int x = 1; x < FIELD_WIDTH - 1; x++) {
                            int index = y * FIELD_WIDTH + x;
                            field[index] = field[index - FIELD_WIDTH];
                        }
                    }
                }

                lines.clear();

                // Update timings to be the normal game timings TODO: store previous timings
                currentSpeedTimer = 0.0f;
                currentSpeed = 0.7f;
            }

            return;
        }

        // Updating variables based on input
        bool isInstantPressed = app->isKeyPressed(INSTANT_DROP_KEY);
        bool isRotatePressed = app->isKeyPressed(ROTATE_KEY);
        bool isRotateBackwardsPressed = app->isKeyPressed(ROTATE_BACKWARDS_KEY);
        bool isLeftPressed = app->isKeyPressed(LEFT_KEY);
        bool isRightPressed = app->isKeyPressed(RIGHT_KEY);
        bool isDownPressed = app->isKeyPressed(DOWN_KEY);

        bool wasInstantPressed = this->isInstantDropPressed;
        bool wasRotatePressed = this->isRotatePressed;
        bool wasRotateBackwardsPressed = this->isRotateBackwardsPressed;
        this->isRotatePressed = isRotatePressed;
        this->isRotateBackwardsPressed = isRotateBackwardsPressed;
        this->isInstantDropPressed = isInstantPressed;

        bool canPressRotate = isRotatePressed && !wasRotatePressed;
        bool canPressRotateBack = isRotateBackwardsPressed && !wasRotateBackwardsPressed;
        bool canPressLeft = lastPress.count(LEFT_KEY) == 0 || SDL_GetTicks64() > this->lastPress[LEFT_KEY];
        bool canPressRight = lastPress.count(RIGHT_KEY) == 0 || SDL_GetTicks64() > this->lastPress[RIGHT_KEY];
        bool canPressDown = lastPress.count(DOWN_KEY) == 0 || SDL_GetTicks64() > this->lastPress[DOWN_KEY];

        // Input handling for moving forward/backward/down
        if (isLeftPressed && canPressLeft && Tetromino::canFit(currentPiece, currentX - 1, currentY, currentRotation, field)) {
            currentX--;
            lastPress[LEFT_KEY] = SDL_GetTicks64() + INPUT_DELAY;
        }

        if (isRightPressed && canPressRight && Tetromino::canFit(currentPiece, currentX + 1, currentY, currentRotation, field)) {
            currentX++;
            lastPress[RIGHT_KEY] = SDL_GetTicks64() + INPUT_DELAY;
        }

        if (isDownPressed && canPressDown && Tetromino::canFit(currentPiece, currentX, currentY + 1, currentRotation, field)) {
            currentY++;
            lastPress[DOWN_KEY] = SDL_GetTicks64() + INPUT_DELAY;
        }

        // Input handling for rotating
        int nextRotation = (currentRotation + 1) % 4;
        if (canPressRotate && Tetromino::canFit(currentPiece, currentX, currentY, nextRotation, field)) {
            currentRotation = nextRotation;
        }

        nextRotation = currentRotation == 0 ? 3 : currentRotation - 1;
        if (canPressRotateBack && Tetromino::canFit(currentPiece, currentX, currentY, nextRotation, field)) {
            currentRotation = nextRotation;
        }

        // When spacebar is pressed, the piece is dropped instantly and locked to the board
        if(isInstantPressed && !wasInstantPressed) {
            while(Tetromino::canFit(currentPiece, currentX, currentY + 1, currentRotation, field)) {
                currentY++;
            }
            currentSpeedTimer = currentSpeed;
        }

        // Game timings
        currentSpeedTimer += ts;
        if(currentSpeedTimer >= currentSpeed) {
            currentSpeedTimer = 0.0f;

            // Check if can move down
            if(Tetromino::canFit(currentPiece, currentX, currentY + 1, currentRotation, field)) {
                currentY++;
            } else {
                // Place the piece
                Tetromino::place(currentPiece, currentX, currentY, currentRotation, field);

                // Check for lines
                for(int y = 0; y < Tetromino::TETROMINO_SIZE; y++) {
                    if(currentY + y >= FIELD_HEIGHT - 1)
                        continue;

                    bool line = true;

                    // If a line has an empty slot
                    for(int x = 1; x < FIELD_WIDTH - 1; x++) {
                        int index = (y + currentY) * FIELD_WIDTH + x;
                        if(field[index] == EMPTY_ID) {
                            line = false;
                        }
                    }

                    if(line) {
                        lines.push_back(currentY + y);
                        currentSpeedTimer = 0.0f;
                        currentSpeed = 0.5f;
                    }
                }

                // Generate new piece
                currentPiece = engine() % 7;
                currentX = FIELD_WIDTH / 2;
                currentY = 0;
                currentRotation = 0;
                currentSpeedTimer = 0.0f;

                if(!Tetromino::canFit(currentPiece, currentX, currentY, currentRotation, field)) {
                    gameOver = true;
                }
            }
        }
    }

    void GameMenuStateSP::render(SDL_Renderer *renderer, float ts) {
        MenuState::render(renderer, ts);

        int size = 48;

        int borderWidth = 26;
        int textureSize = 512;

        int startX = (App::WINDOW_WIDTH / 2) - (FIELD_WIDTH * size) / 2;
        int startY = (App::WINDOW_HEIGHT / 2) - (FIELD_HEIGHT * size) / 2;

        // Render the field
        for (int x = 0; x < FIELD_WIDTH; x++) {
            for (int y = 0; y < FIELD_HEIGHT; y++) {
                int index = x + y * FIELD_WIDTH;
                unsigned int id = field[index];

                int posX = startX + x * size;
                int posY = startY + y * size;

                // Check if boundary
                if (id == BOUNDARY_ID) {
                    SDL_Texture *tetrominoTexture = app->getTexture("tetronimo").texture;
                    int textureX = textureSize * 7;

                    SDL_Rect srcRect = {textureX + borderWidth, borderWidth, 460, 460};
                    SDL_Rect dstRec = {posX, posY, size, size};

                    renderTexture(renderer, tetrominoTexture, &srcRect, &dstRec);
                } else if (id == EMPTY_ID) {
                    // Draw blank square
                    SDL_SetRenderDrawColor(renderer, 128, 128, 128, 255);
                    SDL_Rect rect = {posX, posY, size, size};
                    SDL_RenderDrawRect(renderer, &rect);
                } else {
                    // Otherwise draw in the correct teromino piece
                    SDL_Texture *tetrominoTexture = app->getTexture("tetronimo").texture;
                    int textureX = textureSize * (id - 1);

                    SDL_Rect srcRect = {textureX + borderWidth, borderWidth, 460, 460};
                    SDL_Rect dstRec = {posX, posY, size, size};

                    if(std::count(lines.begin(), lines.end(), y) > 0) {
                        textureX = textureSize * 7;
                        srcRect = {textureX + borderWidth, borderWidth, 460, 460};

                        renderTexture(renderer, tetrominoTexture, &srcRect, &dstRec);
                    }
                    else
                        renderTexture(renderer, tetrominoTexture, &srcRect, &dstRec);
                }
            }
        }

        // Render current piece in
        for (int pieceX = 0; pieceX < Tetromino::TETROMINO_SIZE; pieceX++) {
            for (int pieceY = 0; pieceY < Tetromino::TETROMINO_SIZE; pieceY++) {
                const unsigned int *shape = Tetromino::getShape(currentPiece);
                int index = Tetromino::rotate(pieceX, pieceY, currentRotation);
                unsigned int value = shape[index];

                if (value != 0) {
                    int posX = startX + (currentX + pieceX) * size;
                    int posY = startY + (currentY + pieceY) * size;

                    SDL_Texture *tetrominoTexture = app->getTexture("tetronimo").texture;
                    int textureX = textureSize * currentPiece;

                    SDL_Rect srcRect = {textureX + borderWidth, borderWidth, 460, 460};
                    SDL_Rect dstRec = {posX, posY, size, size};

                    renderTexture(renderer, tetrominoTexture, &srcRect, &dstRec);
                }
            }
        }

        // Render "ghost" piece (where it will land)
        int ghostY = currentY;
        while (Tetromino::canFit(currentPiece, currentX, ghostY + 1, currentRotation, field)) {
            ghostY++;
        }

        for (int pieceX = 0; pieceX < Tetromino::TETROMINO_SIZE; pieceX++) {
            for (int pieceY = 0; pieceY < Tetromino::TETROMINO_SIZE; pieceY++) {
                const unsigned int *shape = Tetromino::getShape(currentPiece);
                int index = Tetromino::rotate(pieceX, pieceY, currentRotation);
                unsigned int value = shape[index];

                if (value != 0) {
                    int posX = startX + (currentX + pieceX) * size;
                    int posY = startY + (ghostY + pieceY) * size;

                    SDL_Texture *tetrominoTexture = app->getTexture("tetronimo").texture;
                    int textureX = textureSize * currentPiece;

                    SDL_Rect srcRect = {textureX + borderWidth, borderWidth, 460, 460};
                    SDL_Rect dstRec = {posX, posY, size, size};

                    renderTexture(renderer, tetrominoTexture, &srcRect, &dstRec, 50);
                }
            }
        }
    }
}