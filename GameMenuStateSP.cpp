//
// Created by pauli on 2/6/2023.
//

#include "TetrisApp.h"
#include "GameMenuStateSP.h"

namespace Tetris {

    GameMenuStateSP::GameMenuStateSP(App *app) : MenuState(app) {
        this->currentPiece = 5;
        this->currentX = FIELD_WIDTH / 2;
        this->currentY = 0;
        this->currentRotation = 0;

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

        // Updating variables based on input
        bool isRotatePressed = app->isKeyPressed(SDL_SCANCODE_Z);
        bool isLeftPressed = app->isKeyPressed(SDL_SCANCODE_LEFT);
        bool isRightPressed = app->isKeyPressed(SDL_SCANCODE_RIGHT);
        bool isDownPressed = app->isKeyPressed(SDL_SCANCODE_DOWN);

        bool wasRotatePressed = this->wasPressed[SDL_SCANCODE_Z];
        bool wasDownPressed = this->wasPressed[SDL_SCANCODE_DOWN];
        bool wasLeftPressed = this->wasPressed[SDL_SCANCODE_LEFT];
        bool wasRightPressed = this->wasPressed[SDL_SCANCODE_RIGHT];

        this->wasPressed[SDL_SCANCODE_Z] = isRotatePressed;
        this->wasPressed[SDL_SCANCODE_LEFT] = isLeftPressed;
        this->wasPressed[SDL_SCANCODE_RIGHT] = isRightPressed;
        this->wasPressed[SDL_SCANCODE_DOWN] = isDownPressed;

        // Input handling for moving forward/backward/down
        if (isLeftPressed && !wasLeftPressed && Tetromino::canFit(currentPiece, currentX - 1, currentY, currentRotation, field)) {
            currentX--;
        } else if (isRightPressed && !wasRightPressed && Tetromino::canFit(currentPiece, currentX + 1, currentY, currentRotation, field)) {
            currentX++;
        } else if (isDownPressed && !wasDownPressed && Tetromino::canFit(currentPiece, currentX, currentY + 1, currentRotation, field)) {
            currentY++;
        }

        // Input handling for rotating
        int nextRotation = (currentRotation + 1) % 4;
        if (isRotatePressed && !wasRotatePressed &&
            Tetromino::canFit(currentPiece, currentX, currentY, nextRotation, field)) {
            currentRotation = nextRotation;
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
    }
}