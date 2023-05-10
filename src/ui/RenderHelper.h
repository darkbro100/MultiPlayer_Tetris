//
// Created by pauli on 2/3/2023.
//

#ifndef MPTETRIS_RENDERHELPER_H
#define MPTETRIS_RENDERHELPER_H

#include "SDL.h"
#include "SDL_ttf.h"
#include "SDL_image.h"
#include <string>
#include "../game/GameCommon.h"
#include <unordered_map>
#include <vector>
#include "AssetHelper.h"

namespace Tetris {
    /**
     * A very simple, yet inefficient way to render text given the proper variables. This should also be moved to its own source file at some point.
     * @param renderer Renderer to draw the text on
     * @param font Font to use
     * @param text Text to render
     * @param color Color of the text
     * @param pos Position of where the text should be rendered (x, y, width, height)
     */
    void renderText(SDL_Renderer *renderer, TTF_Font *font, const std::string &  text, SDL_Color color, SDL_FRect * pos);

    /**
     * A very simple, yet inefficient way to render text given the proper variables. This should also be moved to its own source file at some point.
     * @param renderer Renderer to draw the text on
     * @param font Font to use
     * @param text Text to render
     * @param color Color of the text
     * @param pos Position of where the text should be rendered (x, y, width, height)
     */
    void renderText(SDL_Renderer *renderer, TTF_Font *font, const std::string &  text, SDL_Color color, SDL_Rect * pos);

    /**
     * Render a texture to a given renderer.
     * @param renderer Renderer to render the texture on
     * @param texture Texture to render
     * @param src Source rectangle. If null, the entire texture will be rendered.
     * @param dst Destination rectangle. If null, the entire renderer will be used.
     * @param alpha Alpha value of the texture. 255 is opaque, 0 is transparent.
     */
    void renderTexture(SDL_Renderer *renderer, SDL_Texture *texture, SDL_Rect *src, SDL_FRect *dst, Uint8 alpha = 255);

    /**
     * Render a texture to a given renderer.
     * @param renderer Renderer to render the texture on
     * @param texture Texture to render
     * @param src Source rectangle. If null, the entire texture will be rendered.
     * @param dst Destination rectangle. If null, the entire renderer will be used.
     * @param alpha Alpha value of the texture. 255 is opaque, 0 is transparent.
     */
    void renderTexture(SDL_Renderer *renderer, SDL_Texture *texture, SDL_Rect *src, SDL_Rect *dst, Uint8 alpha = 255);

    /**
     * This renders our "lobby" state in a Multiplayer game, before the game is started. Will show all the players, including their pings
     * @param renderer Pointer to the renderer
     * @param players Reference to the map containing all the players
     * @param holder FontHolder to use for rendering the text
     * @param id ID of the client who is calling this function (default to the server)
     */
    void renderPlayers(SDL_Renderer * renderer, std::unordered_map<uint32_t, std::shared_ptr<Player>> & players, FontHolder & holder, uint32_t id = 1);

    /**
     * Renders a piece on the field
     * @param renderer SDL Renderer
     * @param x Position of the piece (on the field)
     * @param y Position of the piece (on the field)
     * @param piece Piece to render
     * @param rotation Rotation of the piece
     * @param startX Start X of the field
     * @param startY Start Y of the field
     * @param alpha Alpha of texture
     * @param cellSize Size of a cell
     */
    void renderPiece(SDL_Renderer *renderer, TextureHolder & holder, int x, int y, unsigned int piece, int rotation, int startX, int startY, int alpha = 255, int cellSize = CELL_SIZE);

    /**
     * Renders the field to the screen
     * @param renderer Pointer to the renderer
     * @param holder Holder to the Tetromino texture
     * @param startX Where the field should begin on the actual screen ( screen coord x)
     * @param startY Where the field should begin on the actual screen ( screen coord y)
     * @param field Pointer to the field
     * @param lines Vector containing the lines
     * @param gameOver Whether or not the game has ended
     * @param cellSize The size of each cell
     */
    void renderField(SDL_Renderer * renderer, TextureHolder & holder, int startX, int startY, const uint8_t * field, std::vector<unsigned int> & lines, bool gameOver, int cellSize = CELL_SIZE);

}

#endif //MPTETRIS_RENDERHELPER_H
