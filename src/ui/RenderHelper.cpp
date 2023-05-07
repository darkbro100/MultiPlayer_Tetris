//
// Created by pauli on 2/3/2023.
//
#include "../TetrisApp.h"
#include "../game/GameCommon.h"
#include "RenderHelper.h"

namespace Tetris {
    void renderText(SDL_Renderer *renderer, TTF_Font *font, const std::string & text, SDL_Color color, SDL_FRect * pos) {
        SDL_Surface *surface = TTF_RenderText_Blended(font, text.c_str(), color);
        SDL_Texture *texture = SDL_CreateTextureFromSurface(renderer, surface);
        SDL_FreeSurface(surface);

        SDL_RenderCopyF(renderer, texture, nullptr, pos);
        SDL_DestroyTexture(texture);
    }

    void renderText(SDL_Renderer *renderer, TTF_Font *font, const std::string &  text, SDL_Color color, SDL_Rect * pos) {
        SDL_Surface *surface = TTF_RenderText_Blended(font, text.c_str(), color);
        SDL_Texture *texture = SDL_CreateTextureFromSurface(renderer, surface);
        SDL_FreeSurface(surface);

        SDL_RenderCopy(renderer, texture, nullptr, pos);
        SDL_DestroyTexture(texture);
    }

    void renderTexture(SDL_Renderer *renderer, SDL_Texture *texture, SDL_Rect *src, SDL_FRect *dst, Uint8 alpha) {
        if(alpha < 255) {
            SDL_SetTextureAlphaMod(texture, alpha);
        }
        SDL_RenderCopyF(renderer, texture, src, dst);

        // Reset the alpha
        if(alpha < 255) {
            SDL_SetTextureAlphaMod(texture, 255);
        }
    }

    void renderTexture(SDL_Renderer *renderer, SDL_Texture *texture, SDL_Rect *src, SDL_Rect *dst, Uint8 alpha) {
        if(alpha < 255) {
            SDL_SetTextureAlphaMod(texture, alpha);
        }
        SDL_RenderCopy(renderer, texture, src, dst);

        // Reset the alpha
        if(alpha < 255) {
            SDL_SetTextureAlphaMod(texture, 255);
        }
    }

    void renderPlayers(SDL_Renderer *renderer, std::map<uint32_t, Player> &players, FontHolder & holder, uint32_t id) {
        // Render how many connected players under fps text
        int startY = 150;
        int inc = 50;

        std::string playersText = "Players: " + std::to_string(players.size());
        SDL_Rect dst = {0, startY, 200, inc};
        renderText(renderer, holder.font, playersText, {0, 100, 0, 255}, &dst);

        // Render all the connected players
        if (!players.empty()) {
            int x = 0;
            int y = startY + inc;

            // Render the players
            for (auto &player: players) {
                std::string playerText;
                if(player.second.id == id) {
                    playerText = std::to_string(player.first) + " (You) - " + std::to_string(player.second.ping) + "ms";
                } else {
                    playerText = std::to_string(player.first) + " - " + std::to_string(player.second.ping) + "ms";
                }
                dst = {x, y, 200, inc};
                renderText(renderer, holder.font, playerText, {255, 255, 255, 255}, &dst);
                y += inc;
            }

            // Tell the player they can start the game
            std::string startText = "Ready!";
            dst = {(App::WINDOW_WIDTH / 2) - (100 / 2), (App::WINDOW_HEIGHT / 2) - inc, 100, inc};
            renderText(renderer, holder.font, startText, {255, 255, 255, 255}, &dst);

            dst = {dst.x, dst.y + inc, 100, inc};
            if(id == 1) {
                renderText(renderer, holder.font, "Press Enter", {255, 255, 255, 255}, &dst);
            } else {
                dst = {dst.x - 50, dst.y, 200, inc};
                renderText(renderer, holder.font, "Waiting for host...", {255, 255, 255, 255}, &dst);
            }
        }
    }

    void renderPiece(SDL_Renderer *renderer, TextureHolder & holder, int x, int y, unsigned int piece, int rotation, int startX,
                                 int startY, int alpha, int cellSize) {
        for (int pieceX = 0; pieceX < Tetromino::TETROMINO_SIZE; pieceX++) {
            for (int pieceY = 0; pieceY < Tetromino::TETROMINO_SIZE; pieceY++) {
                const unsigned int *shape = Tetromino::getShape(piece);
                int index = Tetromino::rotate(pieceX, pieceY, rotation);
                unsigned int value = shape[index];

                if (value != 0) {
                    int posX = startX + (x + pieceX) * cellSize;
                    int posY = startY + (y + pieceY) * cellSize;

                    SDL_Texture *tetrominoTexture = holder.texture;
                    int textureX = TEXTURE_SIZE * piece;

                    SDL_Rect srcRect = {textureX + TEXTURE_BORDER, TEXTURE_BORDER, 460, 460};
                    SDL_Rect dstRec = {posX, posY, cellSize, cellSize};

                    renderTexture(renderer, tetrominoTexture, &srcRect, &dstRec, alpha);
                }
            }
        }
    }

    void Tetris::renderField(SDL_Renderer *renderer, TextureHolder &holder, int startX, int startY, const uint8_t *field,
                             std::vector<unsigned int> &lines, bool gameOver, int cellSize) {

        // Render the field
        for (int x = 0; x < FIELD_WIDTH; x++) {
            for (int y = 0; y < FIELD_HEIGHT; y++) {
                int index = x + y * FIELD_WIDTH;
                unsigned int id = field[index];

                int posX = startX + x * cellSize;
                int posY = startY + y * cellSize;

                // Check if boundary
                if (id == BOUNDARY_ID) {
                    SDL_Texture *tetrominoTexture = holder.texture;
                    int textureX = TEXTURE_SIZE * 7;

                    SDL_Rect srcRect = {textureX + TEXTURE_BORDER, TEXTURE_BORDER, 460, 460};
                    SDL_Rect dstRec = {posX, posY, cellSize, cellSize};

                    renderTexture(renderer, tetrominoTexture, &srcRect, &dstRec, gameOver ? 100 : 255);
                } else if (id == EMPTY_ID) {
                    // Draw blank square
                    SDL_SetRenderDrawColor(renderer, 128, 128, 128, gameOver ? 100 : 255);
                    SDL_Rect rect = {posX, posY, cellSize, cellSize};
                    SDL_RenderDrawRect(renderer, &rect);
                } else {
                    // Otherwise draw in the correct teromino piece
                    SDL_Texture *tetrominoTexture = holder.texture;
                    int textureX = TEXTURE_SIZE * (id - 1);

                    SDL_Rect srcRect = {textureX + TEXTURE_BORDER, TEXTURE_BORDER, 460, 460};
                    SDL_Rect dstRec = {posX, posY, cellSize, cellSize};

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

    }

}