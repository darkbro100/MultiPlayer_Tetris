//
// Created by pauli on 2/3/2023.
//
#include <iostream>
#include "AssetHelper.h"

namespace Tetris {
    FontHolder loadFont(const char * path, std::string fontName, int ptSize) {
        TTF_Font *font = TTF_OpenFont("../OpenSans.ttf", ptSize);
        if (!font) {
            std::cout << "error loading font: " << TTF_GetError() << std::endl;
        }

        // define struct
        FontHolder holder;
        holder.font = font;
        holder.name = fontName;

        return holder;
    }

    TextureHolder loadTexture(const char *path, std::string textureName, SDL_Renderer * renderer) {
        SDL_Surface *surface = IMG_Load("../image.png");
        if (!surface) {
            std::cout << "error loading image: " << IMG_GetError() << std::endl;
        }

        SDL_Texture *tex = SDL_CreateTextureFromSurface(renderer, surface);

        if (!tex) {
            std::cout << "error creating texture: " << SDL_GetError() << std::endl;
        }

        // define struct
        TextureHolder holder;
        holder.texture = tex;
        holder.name = "tetronimo";

        // free the surface
        SDL_FreeSurface(surface);

        return holder;
    }
}