//
// Created by pauli on 2/3/2023.
//

#ifndef MPTETRIS_ASSETHELPER_H
#define MPTETRIS_ASSETHELPER_H

#include "SDL_image.h"
#include "SDL_ttf.h"
#include <string>

namespace Tetris {
    /**
     * Struct to represent a Font
     */
    struct FontHolder {
        /**
         * Reference to the TTF Font pointer
         */
        TTF_Font * font;

        /**
         * Name of the font
         */
        std::string name;
    };

    struct TextureHolder {
        /**
         * Pointer to the SDL_Texture
         */
        SDL_Texture * texture;

        /**
         * Name of the texture
         */
        std::string name;

        /**
         * Width/height of the texture
         */
        int width, height;
    };

    /**
     * Loads a texture from the given path and stores it in holder struct
     * @param path Path to texture
     * @param textureName Name of texture
     * @param renderer Pointer to the renderer
     * @return Holder to texture
     */
    TextureHolder loadTexture(const char *path, const std::string & textureName, SDL_Renderer * renderer);

    /**
     * Loads a font from the given path and stores it in holder struct
     * @param path Path to font file
     * @param fontName Name of font
     * @param pt Size of font
     * @return Holder to font
     */
    FontHolder loadFont(const char * path, const std::string & fontName, int pt);
}

#endif //MPTETRIS_ASSETHELPER_H