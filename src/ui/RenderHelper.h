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
#include <map>
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

    void renderPlayers(SDL_Renderer * renderer, std::map<uint32_t, Player> & players, FontHolder & holder, uint32_t id = 1);

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

    void renderField(SDL_Renderer * renderer, TextureHolder & holder, int startX, int startY, uint8_t * field, std::vector<unsigned int> & lines, bool gameOver, int cellSize = CELL_SIZE);

}

#endif //MPTETRIS_RENDERHELPER_H
