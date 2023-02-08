//
// Created by pauli on 2/3/2023.
//

#ifndef MPTETRIS_ASSETHELPER_H
#define MPTETRIS_ASSETHELPER_H

#include "SDL_image.h"
#include "SDL_ttf.h"
#include <string>

namespace Tetris {
    struct FontHolder {
        TTF_Font * font;
        std::string name;
    };

    struct TextureHolder {
        SDL_Texture * texture;
        std::string name;
        int width, height;
    };

    TextureHolder loadTexture(const char *path, const std::string & textureName, SDL_Renderer * renderer);
    FontHolder loadFont(const char * path, const std::string & fontName, int pt);
}

#endif //MPTETRIS_ASSETHELPER_H