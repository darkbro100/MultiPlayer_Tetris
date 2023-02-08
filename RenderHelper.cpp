//
// Created by pauli on 2/3/2023.
//
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

    void renderTexture(SDL_Renderer *renderer, SDL_Texture *texture, SDL_Rect *src, SDL_FRect *dst) {
        SDL_RenderCopyF(renderer, texture, src, dst);
    }
}