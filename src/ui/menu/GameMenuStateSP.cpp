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
        player.piece.current = 5;
        player.piece.next = engine() % 7;
        player.pos.x = FIELD_WIDTH / 2;
        player.pos.y = 0;
        player.pos.rotation = 0;
        player.pos.storedRotation = 0;
        player.speed.current = 0.7f;
        player.speed.timer = 0.0f;
        player.linesCreated = 0;
        player.score = 0;

        inputs.leftDelay = INPUT_DELAY;
        inputs.rightDelay = INPUT_DELAY;
        inputs.downDelay = INPUT_DELAY;

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
            player.speed.timer += ts;
            if (player.speed.timer >= player.speed.current) {
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
                player.speed.timer = 0.0f;
                player.speed.current = 0.7f;
            }

            return;
        }

        // Check inputs
        checkInputs(inputs, app, player.piece, player.pos, player.speed, player.field);

        // Game timings
        player.speed.timer += ts;
        if (player.speed.timer >= player.speed.current) {
            player.speed.timer = 0.0f;

            // Check if can move down
            if (Tetromino::canFit(player.piece.current, player.pos.x, player.pos.y + 1, player.pos.rotation,
                                  player.field)) {
                player.pos.y++;
            } else {
                // Place the piece
                Tetromino::place(player.piece.current, player.pos.x, player.pos.y, player.pos.rotation,
                                 player.field);

                // Check for lines
                for (int y = 0; y < Tetromino::TETROMINO_SIZE; y++) {
                    if (player.pos.y + y >= FIELD_HEIGHT - 1)
                        continue;

                    bool line = true;

                    // If a line has an empty slot
                    for (int x = 1; x < FIELD_WIDTH - 1; x++) {
                        int index = (y + player.pos.y) * FIELD_WIDTH + x;
                        if (player.field[index] == EMPTY_ID) {
                            line = false;
                        }
                    }

                    if (line) {
                        player.lines.push_back(player.pos.y + y);
                        player.speed.timer = 0.0f;
                        player.speed.current = 0.25f;
                    }
                }

                // Generate new piece
                player.piece.current = player.piece.next;
                player.piece.next = engine() % 7;
                player.pos.x = FIELD_WIDTH / 2;
                player.pos.y = 0;
                player.pos.rotation = 0;
                player.pos.storedRotation = 0;
                player.speed.timer = 0.0f;
                player.piece.hasStored = false;
                inputs.leftDelay = INPUT_DELAY;
                inputs.rightDelay = INPUT_DELAY;
                inputs.downDelay = INPUT_DELAY;

                if (!Tetromino::canFit(player.piece.current, player.pos.x, player.pos.y, player.pos.rotation,
                                       player.field))
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
        renderText(renderer, fontHolder.font, "Score: " + std::to_string(player.score), {255, 255, 255, 255},
                   &textRect);

        // If the game isn't over... show the current piece, otherwise show a Game Over msg
        if (!player.gameOver) {
            // Render current piece in
            renderPiece(renderer, holder, player.pos.x, player.pos.y, player.piece.current, player.pos.rotation,
                        startX, startY);

            // Render "ghost" piece (where it will land)
            int ghostY = player.pos.y;
            while (Tetromino::canFit(player.piece.current, player.pos.x, ghostY + 1, player.pos.rotation,
                                     player.field)) {
                ghostY++;
            }
            renderPiece(renderer, holder, player.pos.x, ghostY, player.piece.current, player.pos.rotation, startX,
                        startY, 50);

            // Render the next piece under the score
            textRect = {startX + (FIELD_WIDTH * CELL_SIZE) + 10, startY + 200, 150, 75};
            renderText(renderer, fontHolder.font, "Next", {255, 255, 255, 255}, &textRect);
            renderPiece(renderer, holder, 0, 0, player.piece.next, 0, startX + (FIELD_WIDTH * CELL_SIZE) + 10,
                        startY + 225, 100);

            // Render the stored piece directly under the next piece
            if (player.piece.stored != INVALID_SHAPE) {
                textRect = {startX + (FIELD_WIDTH * CELL_SIZE) + 10, startY + 400, 150, 75};
                renderText(renderer, fontHolder.font, "Stored", {255, 255, 255, 255}, &textRect);
                renderPiece(renderer, holder, 0, 0, player.piece.stored, 0, startX + (FIELD_WIDTH * CELL_SIZE) + 10,
                            startY + 425, 100);
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
}